#include "hbridge.h"


volatile uint32_t hbridge_testing_clk_div;
volatile uint16_t tail_position_pilot;
volatile int tail_diff;
volatile uint16_t tail_diff_old;
volatile float duty_coeff;

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
    
    tail_position_pilot = 128;
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

void hbridge_testing(void)
{
    set_bit(HBRIDGE_PORT, HBRIDGE_ENABLE_PIN);  // Enable H-Bridge

    if(hbridge_testing_clk_div++ >= HBRIDGE_TESTING_CLK_DIV_VALUE){

        hbridge_set_pwm(HBRIDGE_SIDE_A, 0);
        hbridge_set_pwm(HBRIDGE_SIDE_B, 0.2);
        
#ifdef LED_ON
    cpl_bit(LED2_PORT, LED2);
#endif
        hbridge_testing_clk_div=0;
    }
}

void hbridge_task(void)
{
    if(tail_position_pilot < 0 || tail_position_pilot > 270){
        error_flags.invalid_str_whl = 1;
        set_state_error();
    }
    if(measurements.position_avg < 0 || measurements.position_avg > 270)
    {
        error_flags.invalid_tail = 1;
        set_state_error();
    }
    
    tail_diff = tail_position_pilot - measurements.position_avg;    // Check sensor pot difference: pilot - tail

    /*if(tail_diff > 0){
        tail_diff_old < tail_diff ? tail_diff_old = tail_diff;          // Used for responsive duty cycle on hbridge pwm
    } else if (tali_diff < 0) {
        tail_diff_old < (-1 * tail_diff) ? tail_diff_old = tail_diff;
    } else {
        tail_diff_old = 0;
    }
    
    // Duty cycle mantained until tail_tolerance state achieved. Duty is maxed at tail_diff=270° 
    //         Minimum at 5%            0.0037 %/°
    duty_coeff = 0.05 + (0.95 * (tail_diff_old / 270)); 
#ifdef VERBOSE_ON_HBRIDGE
    usart_send_uint16(tail_diff_old);
    usart_send_string(" :tail_diff_old\n");
#endif
    */

    duty_coeff = 0.05;

#ifdef VERBOSE_ON_HBRIDGE
        tail_diff > 0 ? usart_send_string("Vira ESTIBORDO\n") : usart_send_string("Vira BOMBORDO\n");
#endif
    if(tail_diff > TAIL_TOLERANCE){
        hbridge_set_pwm(HBRIDGE_SIDE_A, 0);
        hbridge_set_pwm(HBRIDGE_SIDE_B, duty_coeff);
    } else if (tail_diff < TAIL_TOLERANCE){
        hbridge_set_pwm(HBRIDGE_SIDE_A, duty_coeff);
        hbridge_set_pwm(HBRIDGE_SIDE_B, 0);
    } else {
        tail_diff_old = 0;
        hbridge_set_pwm(HBRIDGE_SIDE_A, 0);
        hbridge_set_pwm(HBRIDGE_SIDE_B, 0);
    }
}


EMPTY_INTERRUPT(TIMER0_COMPA_vect);
EMPTY_INTERRUPT(TIMER0_COMPB_vect);
EMPTY_INTERRUPT(TIMER0_OVF_vect);