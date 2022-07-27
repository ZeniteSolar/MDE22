#include "hbridge.h"

volatile float duty_coeff;
volatile int tail_diff;
volatile int tail_diff_old;
volatile uint32_t hbridge_testing_clk_div;
volatile uint32_t hbridge_verbose_clk_div;
volatile uint16_t tail_position_pilot;
volatile uint8_t hbridge_led_clk_div;
volatile hbridge_flags_t hbridge_flags;

void hbridge_init(void)
{   
    TCCR0A = (1<<COM0A1) | (1<<COM0A0)              // Set on compare match when up-counting, clear ... on down-counting
            | (1<<COM0B1)| (0<<COM0B0)              // Clear on compare match when up-counting, set ... on down-counting
            | (0<< WGM01) | (1<<WGM00);              

    // fOCnxPCPWM = (fclk_IO)/(N x 510), N being the prescaler
    // Prescaler 010 = 8 -> 3921.5 Hz  
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

void hbridge_task(void)
{
    if ((!hbridge_flags.wrong_side_turn) && (!tst_bit(HBRIDGE_PORT, HBRIDGE_ENABLE_PIN))) {
        set_bit(HBRIDGE_PORT, HBRIDGE_ENABLE_PIN);
        usart_send_string("HBridge is enabled!\n");
    }

    if (tail_position_pilot > 270 || measurements.position_avg > 270){
        error_flags.invalid_str_whl = 1;
        set_state_error();
    }

    if(hbridge_flags.force_center == 1){
        usart_send_string("Tail shall be centered until MIC returns.\n");
        tail_position_pilot = 165;
    }
    tail_diff = tail_position_pilot - measurements.position_avg;    // Check sensor pot difference: pilot - tail
    duty_coeff = 0.8;

    // Responsive duty cycle. 
    // tail_diff_old receives tail_diff if the latter is greater than the first.
    /*if(tail_diff > 0){
        tail_diff_old < tail_diff ? tail_diff_old = tail_diff;          
    } else if (tail_diff < 0) {
        tail_diff_old > tail_diff) ? tail_diff_old = tail_diff;
    } else {
        tail_diff_old = 0;
    }
    // Duty cycle mantained until tail_tolerance state achieved. Duty is maxed at tail_diff=270° 
    //         Minimum at 5%            0.3518 %/°
    duty_coeff = 0.05 + (0.95 * (tail_diff_old / 270)); */

#ifdef VERBOSE_ON_HBRIDGE
    if(hbridge_verbose_clk_div++ >= HBRIDGE_VERBOSE_CLK_DIV){

        VERBOSE_MSG_HBRIDGE(usart_send_uint16(tail_diff_old));
        VERBOSE_MSG_HBRIDGE(usart_send_string(" :tail_diff_old\n"));

        if (tail_diff > TAIL_TOLERANCE){
            VERBOSE_MSG_HBRIDGE(usart_send_string("Vira ESTIBORDO\t"));
        } else if (tail_diff < -TAIL_TOLERANCE) { 
            VERBOSE_MSG_HBRIDGE(usart_send_string("Vira BOMBORDO\t"));
        } else {
            VERBOSE_MSG_HBRIDGE(usart_send_string("Motor parado.\t"));
        }
        VERBOSE_MSG_HBRIDGE(usart_send_string("Side Switching: "));
        VERBOSE_MSG_HBRIDGE(usart_send_uint8(hbridge_flags.all__));
        VERBOSE_MSG_HBRIDGE(usart_send_char('\n'));
        hbridge_verbose_clk_div = 0;
    }
#endif

#ifdef LED_ON
    if (hbridge_led_clk_div++ >= 3){
        cpl_bit(LED1_PORT, LED1);
        hbridge_led_clk_div = 0;
        /*usart_send_char('\n');
        usart_send_uint16(tail_diff_old);
        usart_send_char('\t');
        usart_send_uint16(tail_diff);
        usart_send_char('\n');*/
    }
#endif

    if (tail_diff > TAIL_TOLERANCE){
        hbridge_set_pwm(HBRIDGE_SIDE_A, 0);
        hbridge_set_pwm(HBRIDGE_SIDE_B, duty_coeff);
        hbridge_flags.side_B_switch_on = 1;
        hbridge_flags.side_A_switch_on = 0;
        if(tail_diff_old > tail_diff){
            clr_bit(HBRIDGE_PORT, HBRIDGE_ENABLE_PIN);
            usart_send_string("TURNING TO THE WRONG SIDE!\n");
            hbridge_flags.wrong_side_turn = 1;
        }
    } else if (tail_diff < -TAIL_TOLERANCE){
        hbridge_set_pwm(HBRIDGE_SIDE_A, duty_coeff);
        hbridge_set_pwm(HBRIDGE_SIDE_B, 0);
        hbridge_flags.side_B_switch_on = 0;
        hbridge_flags.side_A_switch_on = 1;
        if(tail_diff_old < tail_diff){
            clr_bit(HBRIDGE_PORT, HBRIDGE_ENABLE_PIN);
            usart_send_string("TURNING TO THE WRONG SIDE!\n");
            hbridge_flags.wrong_side_turn = 1;
        }
    } else {
        hbridge_set_pwm(HBRIDGE_SIDE_A, 0);
        hbridge_set_pwm(HBRIDGE_SIDE_B, 0);
        hbridge_flags.side_B_switch_on = 0;
        hbridge_flags.side_A_switch_on = 0;
    }
}


EMPTY_INTERRUPT(TIMER0_COMPA_vect);
EMPTY_INTERRUPT(TIMER0_COMPB_vect);
EMPTY_INTERRUPT(TIMER0_OVF_vect);