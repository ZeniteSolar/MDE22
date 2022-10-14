#include "hbridge.h"

volatile float duty_coeff;
volatile int tail_diff;
volatile int tail_diff_old;
volatile int duty_msg;
volatile uint32_t hbridge_verbose_clk_div;
volatile uint16_t str_whl_position;
volatile uint8_t hbridge_led_clk_div;
volatile uint8_t hbridge_led_clk_var;
volatile hbridge_flags_t hbridge_flags;
volatile can_app_flags_t can_app_flags;

void hbridge_init(void)
{   
    TCCR0A = (1<<COM0A1) | (1<<COM0A0)              // Set on compare match when up-counting, clear ... on down-counting
            | (1<<COM0B1)| (0<<COM0B0)              // Clear on compare match when up-counting, set ... on down-counting
            | (0<< WGM01) | (1<<WGM00);              

    // fOCnxPCPWM = (fclk_IO)/(N x 510), N being the prescaler
    // Prescaler 010 = 8 -> 3921.5 Hz               // Used IC operates up to 25kHz, this timer presents minimum prescaler of either 1 or 8 
    TCCR0B = (0<<WGM02) | (0<<CS02) | (1<<CS01) | (0<<CS00);                           

    // Sets the OC0A and OC0B duty cycle, remenber Timer/Counter0 is 8bits
    TCNT0 = 0;
    OCR0A = 255;       // No PWM 
    OCR0B = 0;         // No PWM


// Set pins as output DDRxn = 1
    set_bit(HBRIDGE_PWM_DDR, HBRIDGE_PWM_PIN1);
    set_bit(HBRIDGE_PWM_DDR, HBRIDGE_PWM_PIN2);
    set_bit(HBRIDGE_PWM_DDR, HBRIDGE_ENABLE_PIN);
    
// Initiate HBRIDGE disabled;
    clr_bit(HBRIDGE_PORT, HBRIDGE_ENABLE_PIN);

    tail_diff = 0;
    tail_diff_old = 0;
    duty_coeff = 0;
    
    hbridge_flags.all__ = 0;
    hbridge_flags.force_center = 1;
}

/**
 * @brief PROTOTYPE. This function exchanges hbridge's switching side 
 * while keeping current flow direction. 
 */
void hbridge_toggle_side(void)
{
    if(hbridge_flags.curr_path_low_high == 0){

        // which side is switching?
            // make this side higher switch on (set_pwm(...,1))
        // make the other side switch 

        // Set system flag about this state
        hbridge_flags.curr_path_low_high = 1;
    } else {

        // which side is switching?
            // make this side higher switch on (set_pwm(...,1))
        // make the other side switch 

        // Set system flag about this state

        hbridge_flags.curr_path_low_high = 0;
    }

}

/**
 * @brief Sets pwm duty cycle value on OCR identified by 'side'
 * 
 * @param side : Identify switching side (OCR0A or OCR0B)
 * @param duty : duty cycle
 * @return uint8_t 
 */
uint8_t hbridge_set_pwm(uint8_t side, float duty)
{
    if(duty < 0){
        duty = 0;
    }
    if (side == HBRIDGE_SIDE_A){
        OCR0A = (int)(0xff * (1 - duty));
        return HBRIDGE_OK;
    }else if (side == HBRIDGE_SIDE_B){
        OCR0B = (int)(0xff * duty);
        return HBRIDGE_OK;
    }else{
        return HBRIDGE_ERROR;
    }
}

/**
 * @brief 
 * 
 */
void hbridge_task(void)
{
    
    // Check for errors 
    if (measurements.position_avg > 270){
        //error_flags.invalid_tail = 1;
        //set_state_error();
    }

    // Safeguard for no_mic and steering wheel angle
    // If mic responds and str_whl_angle < 270, hbridge if fine
    if(can_app_flags.no_mic == 1) {
        hbridge_flags.force_center = 1;
    } else {
        if(str_whl_position > 255) {
            //hbridge_flags.force_center = 1;
            usart_send_string("Invalid str wheel angle, value > 270.\n");
        } else {
            hbridge_flags.force_center = 0;
        }
    }

    // Enable for operation
    if(!tst_bit(HBRIDGE_PORT, HBRIDGE_ENABLE_PIN)) {
        set_bit(HBRIDGE_PORT, HBRIDGE_ENABLE_PIN);
        usart_send_string("HBridge is enabled!\n");
    }

// Operation
    if(hbridge_flags.force_center == 1){
        str_whl_position = 158;
    }

    // Determine potentiometer sensors difference: pilot - tail
    tail_diff = str_whl_position - measurements.position_avg;
    
    // Set duty cycle coefficient
    if(tail_diff_old < 0){
        duty_coeff = 0.5 + 0.5*(tail_diff_old * -0.00390625);
    } else if (tail_diff_old == 0) {
        duty_coeff = 0;
    } else { 
        duty_coeff = 0.5 + 0.5*(tail_diff_old * 0.00390625); // 270
    }

    duty_msg = round(duty_coeff*1000);

#ifdef VERBOSE_ON_HBRIDGE
    if(hbridge_verbose_clk_div++ >= HBRIDGE_VERBOSE_CLK_DIV){

        if (tail_diff > TAIL_TOLERANCE){
            VERBOSE_MSG_HBRIDGE(usart_send_string("BORESTE \t\t"));
        } else if (tail_diff < -TAIL_TOLERANCE) { 
            VERBOSE_MSG_HBRIDGE(usart_send_string("BOMBORDO\t\t"));
        } else {
            VERBOSE_MSG_HBRIDGE(usart_send_string("Parado  \t\t"));
        }
        VERBOSE_MSG_HBRIDGE(usart_send_string("Side Switching: "));
        VERBOSE_MSG_HBRIDGE(usart_send_uint8(hbridge_flags.all__));

        VERBOSE_MSG_HBRIDGE(usart_send_string("\tDuty coeff: "));
        VERBOSE_MSG_HBRIDGE(usart_send_int16(duty_msg));
        
        VERBOSE_MSG_HBRIDGE(usart_send_string("\t Taildiff Old: "));
        VERBOSE_MSG_HBRIDGE(usart_send_int16(tail_diff_old));

        if (hbridge_flags.force_center == 1) {
        VERBOSE_MSG_HBRIDGE(usart_send_char('\t'));
        VERBOSE_MSG_HBRIDGE(usart_send_string("Tail shall be centered.\n"));
        }
        VERBOSE_MSG_HBRIDGE(usart_send_char('\n'));
        hbridge_verbose_clk_div = 0;
    }
#endif

#ifdef LED_ON
    // Led 1 displays whether force_center is set (slow means set)    
    if (hbridge_flags.force_center == 1){
        hbridge_led_clk_var = 90;
    } else {
        hbridge_led_clk_var = 15;
    }
    
    if (hbridge_led_clk_div++ >= hbridge_led_clk_var){
        cpl_bit(LED1_PORT, LED1);
        hbridge_led_clk_div = 0;
    }
#endif

    // Check if tail_diff is above tolarance in each direction
    // Check if close to rotation limit
    //       if so, if tail_diff is not decreasing after bridge pwm activated, wrong side turn is detected 
    // Check if rotation limit has been achieved
    // Activate bridge pwm accordingly to tail_diff
    if (tail_diff > TAIL_TOLERANCE){
        /*if (measurements.position_avg > 270){
            if(tail_diff_old > tail_diff){
                // clr_bit(HBRIDGE_PORT, HBRIDGE_ENABLE_PIN);
                usart_send_string("TURNING TO THE WRONG SIDE!\n");
                error_flags.wrong_side_turn = 1;
            }
        }*/

        if (measurements.position_avg < 246){
            hbridge_set_pwm(HBRIDGE_SIDE_A, 0);
            hbridge_set_pwm(HBRIDGE_SIDE_B, duty_coeff);
            hbridge_flags.side_B_switch_on = 1;
            hbridge_flags.side_A_switch_on = 0;
            
        }
        
        if (tail_diff_old < tail_diff){
            tail_diff_old = tail_diff;
        }

    } else if (tail_diff < -TAIL_TOLERANCE){
        /*if (measurements.position_avg < 2) {
            if(tail_diff_old < tail_diff) {
                // clr_bit(HBRIDGE_PORT, HBRIDGE_ENABLE_PIN);
                usart_send_string("TURNING TO THE WRONG SIDE!\n");
                error_flags.wrong_side_turn = 1;
            }
        }*/

        if (measurements.position_avg > 10) {
            hbridge_set_pwm(HBRIDGE_SIDE_A, duty_coeff);
            hbridge_set_pwm(HBRIDGE_SIDE_B, 0);
            hbridge_flags.side_B_switch_on = 0;
            hbridge_flags.side_A_switch_on = 1;
        }

        if (tail_diff_old > tail_diff){
            tail_diff_old = tail_diff;
        }

    } else {
        tail_diff_old = 0;
        hbridge_set_pwm(HBRIDGE_SIDE_A, 0);
        hbridge_set_pwm(HBRIDGE_SIDE_B, 0);
        hbridge_flags.side_B_switch_on = 0;
        hbridge_flags.side_A_switch_on = 0;
    }

}


EMPTY_INTERRUPT(TIMER0_COMPA_vect);
EMPTY_INTERRUPT(TIMER0_COMPB_vect);
EMPTY_INTERRUPT(TIMER0_OVF_vect);