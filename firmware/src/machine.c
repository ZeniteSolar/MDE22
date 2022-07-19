#include "machine.h"

volatile state_machine_t state_machine;
volatile system_flags_t system_flags;
volatile error_flags_t error_flags;
volatile measurements_t measurements;
volatile uint8_t machine_clk;
volatile uint8_t machine_clk_divider;
volatile uint8_t total_errors;           // Contagem de ERROS
volatile uint8_t led_clk_div;

/**
 * @brief
 */
void machine_init(void)
{
	clr_bit(PRR, PRTIM2);                          // Activates clock

    // MODE 2 -> CTC with TOP on OCR1
    TCCR2A  =    (1 << WGM21) | (0 << WGM20)        // mode 2
              | (0 << COM2B1) | (0 << COM2B0)       // do nothing
              | (0 << COM2A1) | (0 << COM2A0);      // do nothing

    TCCR2B =
#if MACHINE_TIMER_PRESCALER ==     1
                (0 << CS22) | (0 << CS21) | (1 << CS20) // Prescaler N=1
#elif MACHINE_TIMER_PRESCALER ==   8
                (0 << CS22) | (1 << CS21) | (0 << CS20) // Prescaler N=8
#elif MACHINE_TIMER_PRESCALER ==   32
                (0 << CS22) | (1 << CS21) | (1 << CS20) // Prescaler N=32
#elif MACHINE_TIMER_PRESCALER ==   64
                (1 << CS22) | (0 << CS21) | (0 << CS20) // Prescaler N=64
#elif MACHINE_TIMER_PRESCALER ==   128
                (1 << CS22) | (0 << CS21) | (1 << CS20) // Prescaler N=128
#elif MACHINE_TIMER_PRESCALER ==   256
                (1 << CS22) | (1 << CS21) | (0 << CS20) // Prescaler N=256
#elif MACHINE_TIMER_PRESCALER ==   1024
                (1 << CS22) | (1 << CS21) | (1 << CS20) // Prescaler N=1024
#else
                0
#endif
                | (0 << WGM22);      // mode 2

    OCR2A = MACHINE_TIMER_TOP;                       // OCR2A = TOP = fcpu/(N*2*f) -1

    TIMSK2 |=   (1 << OCIE2A);                      // Activates interruption

    set_machine_initial_state();
    set_state_initializing();
}

/**
 * @brief set machine initial state
 */
inline void set_machine_initial_state(void)
{
    error_flags.all = 0;
    machine_clk = machine_clk_divider = led_clk_div = 0;
}

/**
 * @brief set error state
 */
inline void set_state_error(void)
{
    VERBOSE_MSG_MACHINE(usart_send_string("\n>>>STATE ERROR\n"));
    state_machine = STATE_ERROR;
}

/**
* @brief set initializing state
*/
inline void set_state_initializing(void)
{
    VERBOSE_MSG_MACHINE(usart_send_string("\n>>>INITIALIZING STATE\n"));
    state_machine = STATE_INITIALIZING;
}

/**
* @brief set idle state
*/
inline void set_state_idle(void)
{
    VERBOSE_MSG_MACHINE(usart_send_string("\n>>>IDLE STATE\n"));
    state_machine = STATE_IDLE;
}

/**
* @brief set running state
*/
inline void set_state_running(void)
{
    VERBOSE_MSG_MACHINE(usart_send_string("\n>>>RUNNING STATE\n"));
    state_machine = STATE_RUNNING;
}

/**
 * @brief set reset state
 */
inline void set_state_reset(void)
{
    VERBOSE_MSG_MACHINE(usart_send_string("\n>>>RESET STATE\n"));
    state_machine = STATE_RESET;
}

/**
 * @breif prints the configurations and definitions
 */
inline void print_configurations(void)
{
    VERBOSE_MSG_MACHINE(usart_send_string("CONFIGURATIONS:\n"));

    //VERBOSE_MSG_MACHINE(usart_send_string("\nadc_f: "));
    //VERBOSE_MSG_MACHINE(usart_send_uint16( ADC_FREQUENCY ));
    //VERBOSE_MSG_MACHINE(usart_send_char(','));
    //VERBOSE_MSG_MACHINE(usart_send_uint16( ADC_AVG_SIZE_10 ));
    VERBOSE_MSG_MACHINE(usart_send_string("\nmachine_f: "));
    VERBOSE_MSG_MACHINE(usart_send_uint16( MACHINE_FREQUENCY ));

    VERBOSE_MSG_MACHINE(usart_send_char('\n'));
}

/**
* @brief prints the system flags
*/
inline void print_system_flags(void)
{
    //VERBOSE_MSG_MACHINE(usart_send_string(" EN "));
    //VERBOSE_MSG_MACHINE(usart_send_char(48+system_flags.enable));
}

/**
* @brief prints the error flags
*/
inline void print_error_flags(void)
{
    //VERBOSE_MSG_MACHINE(usart_send_string(" errFl: "));
    //VERBOSE_MSG_MACHINE(usart_send_char(48+error_flags.no_canbus));
}

/**
 * @brief Checks if the system is OK to run
 */
inline void task_initializing(void)
{
#ifdef LED_ON
    set_bit(LED1_PORT, LED1);
#endif

    set_machine_initial_state();

    VERBOSE_MSG_INIT(usart_send_string("System initialized without errors.\n"));
    set_state_idle();
}

/**
 * @brief waits for commands while checking the system
 */
inline void task_idle(void)
{
#ifdef LED_ON
    if(led_clk_div++ >= 30){
        cpl_bit(LED1_PORT, LED1);
        led_clk_div = 0;
    }
#endif

    set_state_running();

}


/**
 * @brief running task checks the system and apply the control action to pwm.
 */
inline void task_running(void)
{
#ifdef LED_ON
    if(led_clk_div++ >= 2){
        //set_bit(LED1_PORT, LED1);
        led_clk_div = 0;
    }
#endif // LED_ON
}


/**
 * @brief error task checks the system and tries to medicine it.
 */
inline void task_error(void)
{
#ifdef LED_ON
    if(led_clk_div++ >= 5){
        cpl_bit(LED1_PORT, LED1);
        set_bit(LED1_PORT, LED1);
        led_clk_div = 0;
    }
#endif


    total_errors++;         // incrementa a contagem de erros
    VERBOSE_MSG_ERROR(usart_send_string("The error code is: "));
    VERBOSE_MSG_ERROR(usart_send_uint16(error_flags.all));
    VERBOSE_MSG_ERROR(usart_send_char('\n'));

    if(error_flags.no_canbus)
        VERBOSE_MSG_ERROR(usart_send_string("\t - No canbus communication with MIC19!\n"));
    if(!error_flags.all)
        VERBOSE_MSG_ERROR(usart_send_string("\t - Oh no, it was some unknown error.\n"));

    VERBOSE_MSG_ERROR(usart_send_string("The error level is: "));
    VERBOSE_MSG_ERROR(usart_send_uint16(total_errors));
    VERBOSE_MSG_ERROR(usart_send_char('\n'));

    if(total_errors < 2){
        VERBOSE_MSG_ERROR(usart_send_string("I will reset the machine state.\n"));
    }
    if(total_errors >= 20){
        VERBOSE_MSG_ERROR(usart_send_string("The watchdog will reset the whole system.\n"));
        set_state_reset();
    }

#ifdef LED_ON
    cpl_bit(LED2_PORT, LED2);
#endif
    set_state_initializing();
}

/**
 * @brief reset error task just freezes the processor and waits for watchdog
 */
inline void task_reset(void)
{
#ifndef WATCHDOG_ON
    //wdt_init();
#endif

    cli();  // disable interrupts

    VERBOSE_MSG_ERROR(usart_send_string("WAITING FOR A RESET!\n"));
    for(;;);
}

void print_infos(void)
{
#ifdef PRINT_INFOS
    static uint8_t i = 0;

    switch(i++){
        case 0:
            usart_send_string("\naux bat voltage: ");
            usart_send_float(measurements.batvoltage_avg);
            break;
        case 1:
            usart_send_string("\tail position: ");
            usart_send_float(measurements.position_avg);
            break;
        case 2:
            usart_send_string("\naux bat current: ");
            usart_send_float(measurements.batcurrent_avg);
            break;
        default:
            //VERBOSE_MSG_MACHINE(usart_send_char('\n'));
            i = 0;
            break;
    }
#endif
}

inline void compute_measurements(void)
{
    measurements.batvoltage_avg_sum += batvoltage;
    measurements.position_avg_sum += position;
    measurements.batcurrent_avg_sum += batcurrent;

    measurements.batvoltage_avg_sum_count++;
    measurements.position_avg_sum_count++;
    measurements.batcurrent_avg_sum_count++;
}

inline void average_measurements(void)
{
    measurements.batvoltage_avg = 100 * measurements.batvoltage_avg_sum / measurements.batvoltage_avg_sum_count;
    measurements.position_avg = 100 * measurements.position_avg_sum / measurements.position_avg_sum_count;
    measurements.batcurrent_avg = 100 * measurements.batcurrent_avg_sum / measurements.batcurrent_avg_sum_count;
}

inline void reset_measurements(void)
{
    measurements.batvoltage_avg_sum = 0;
    measurements.position_avg_sum = 0;
    measurements.batcurrent_avg_sum = 0;

    measurements.batvoltage_avg_sum_count = 0;
    measurements.position_avg_sum_count = 0;
    measurements.batcurrent_avg_sum_count = 0;
}

/**
 * @brief this is the machine state itself.
 */
/**
 * @brief this is the machine state itself.
 */
inline void machine_run(void)
{
    if(print_adc){ print_infos(); print_adc = 0;}

    if(machine_clk){
        machine_clk = 0;

        if(error_flags.all){
            print_system_flags();
            print_error_flags();
            print_infos();
            set_state_error();
        }

        compute_measurements();

        switch(state_machine){
            case STATE_INITIALIZING:
                task_initializing();

                break;
            case STATE_IDLE:
                task_idle();

                break;
            case STATE_RUNNING:
                task_running();
                #ifdef CAN_ON
                    can_app_task();
                #endif /* CAN_ON */   
                
                break;
            case STATE_ERROR:
                task_error();

            case STATE_RESET:
            default:
                task_reset();
                break;
        }
    }
}

/**
* @brief ISR para ações de controle
*/
ISR(TIMER2_COMPA_vect)
{
    if(++machine_clk_divider == MACHINE_CLK_DIVIDER_VALUE){
        machine_clk_divider = 0;
        machine_clk = 1;

        /*if(machine_clk){
            for(;;){
                //pwm_reset();
                VERBOSE_MSG_ERROR(if(machine_clk) usart_send_string("\nERROR: CLOCK CONFLICT!!!\n"));
            }
        }*/
    }
}

