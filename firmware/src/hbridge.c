#include "hbridge.h"


volatile uint16_t position_target;
volatile uint32_t hbridge_testing_clk_div;

void hbridge_init()
{
// PWM timer init

    // Side 1 is standard for connected, while 
    TCCR0A = (1 << COM0A1) | (1 << COM0A0)              // Set OC0A on compare when up-counting , clear... when down-counting
            | (0 << COM0B1)| (0 << COM0B1)              // Disconnect OC0B (PD5) Side 2
            | (0 << WGM01) | (1 << WGM00);              // Phase correct PWM with TOP on OCRA, WGM02 in TCCR0B

    
    // fOCnxPCPWM = (fclk_IO)/(N x 510), N being the prescaler
    TCCR0B = (0 << CS02) | (0 << CS01) | (1 <<CS00)     // No prescaling
            | (1 << WGM02);                             // finished setting Phase correct PWM with TOP on OCRA

    // Sets the OC0A and OC0B duty cycle, remenber Timer/Counter0 is 8bits
    TCNT0 = 0;
    OCR0A = 128;       // Begin on 50%    // OCR0B = unused 

    TIMSK0 |= (0 << OCIE0B) | (0 << OCIE0A) | (0 << TOIE0);  // All interrupt DISABLED

#ifdef HBRIDGE_ON
// Set pins as output DDRxn = 1
    set_bit(HBRIDGE_PWM_DDR, HBRIDGE_PWM_PIN1);
    set_bit(HBRIDGE_PWM_DDR, HBRIDGE_PWM_PIN2);
    set_bit(HBRIDGE_PWM_DDR, HBRIDGE_ENABLE_PIN);
    
// Initiate HBRIDGE enabled
    set_bit(HBRIDGE_PORT, HBRIDGE_ENABLE_PIN);
#endif
}

/**
 * @brief Changes wich H-Bridge side is switching 
 */
void hbridge_toggle_side(void)
{
    if(TCCR0A && 0xC1){     // OC0A set on up   ---> 1100 (COM0A1:0 COM0B1:0) __ 0001 [reserved] [reserved] WGM1:0 
        TCCR0A = 0x21;      // OC0B clear on up ---> 0010 (COM0A1:0 COM0B1:0) __ 0001 [reserved] [reserved] WGM1:0
    }else if(TCCR0A && 0x21){
        TCCR0A = 0xC1;
    }else{
        TCCR0A = 1;         // Disconnect both, keeping WGM config
        // error or system flag
    } 
}

void hbridge_testing()
{
#ifdef HBRIDGE_ON   
    set_bit(HBRIDGE_PORT, HBRIDGE_ENABLE_PIN);  // Enable H-Bridge

    if(hbridge_testing_clk_div++ >= HBRIDGE_TESTING_CLK_DIV_VALUE){
        usart_send_string("H\n");
#ifdef LED_ON
    cpl_bit(LED2_PORT, LED2);
#endif
        hbridge_testing_clk_div=0;
    }
#endif
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