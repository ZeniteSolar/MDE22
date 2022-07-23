#include "hbridge.h"


volatile uint16_t position_target;
volatile uint32_t hbridge_testing_clk_div;

void hbridge_init()
{   
    TCCR0A = (1<<COM0A1) | (1<<COM0A0)              
            | (1<<COM0B1)| (0<<COM0B0)              
            | (0<< WGM01) | (1<<WGM00);              

    // fOCnxPCPWM = (fclk_IO)/(N x 510), N being the prescaler  16MHz / ( x 510)    
    TCCR0B = (0<<WGM02) | (0<<CS02) | (1<<CS01) | (0<<CS00);                           

    // Sets the OC0A and OC0B duty cycle, remenber Timer/Counter0 is 8bits
    TCNT0 = 0;
    OCR0A = 255;       // Begin on 50%    // OCR0B = unused 
    OCR0B = 128;


// Set pins as output DDRxn = 1
    set_bit(HBRIDGE_PWM_DDR, HBRIDGE_PWM_PIN1);
    set_bit(HBRIDGE_PWM_DDR, HBRIDGE_PWM_PIN2);
    set_bit(HBRIDGE_PWM_DDR, HBRIDGE_ENABLE_PIN);
    
// Initiate HBRIDGE disabled;
    clr_bit(HBRIDGE_PORT, HBRIDGE_ENABLE_PIN);
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

void hbridge_testing()
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

/*void hbridge_follow(uint16_t position_target)
{
    while(position_tail != position_target)
    {
        if(position_tail < position_target){

        }else{

        }
    }
}*/

EMPTY_INTERRUPT(TIMER0_COMPA_vect);
EMPTY_INTERRUPT(TIMER0_COMPB_vect);
EMPTY_INTERRUPT(TIMER0_OVF_vect);