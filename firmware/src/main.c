// coding: utf-8

#include "main.h"

void init(void)
{

    #ifdef USART_ON
        usart_init(MYUBRR,1,1);                         // inicializa a usart
        VERBOSE_MSG_INIT(usart_send_string("\n\n\nUSART... OK!\n"));
    #endif

    _delay_ms(200);

    #ifdef WATCHDOG_ON
        VERBOSE_MSG_INIT(usart_send_string("WATCHDOG..."));
        wdt_init();
        VERBOSE_MSG_INIT(usart_send_string(" OK!\n"));
        wdt_reset();
    #else
        VERBOSE_MSG_INIT(usart_send_string("WATCHDOG... OFF!\n"));
    #endif

    #ifdef WATCHDOG_ON
        wdt_reset();
    #endif

    #ifdef SPI_ON
        set_bit(DDRB, PB2);     //output to be the spi master
        
    #endif

    #ifdef CAN_ON
        VERBOSE_MSG_INIT(usart_send_string("CAN (500kbps)..."));
        #ifdef LED_ON
            set_bit(LED2_PORT, LED2);
        #endif  
        can_init(BITRATE_500_KBPS);
        //can_set_mode(LOOPBACK_MODE);
        VERBOSE_MSG_INIT(usart_send_string(" OK!\n"));
        VERBOSE_MSG_INIT(usart_send_string("CAN filters..."));
        can_static_filter(can_filter);
        VERBOSE_MSG_INIT(usart_send_string(" OK!\n"));
    #else
        VERBOSE_MSG_INIT(usart_send_string("CAN... OFF!\n"));
    #endif

    #ifdef WATCHDOG_ON
        wdt_reset();
    #endif

    #ifdef ADC_ON
        VERBOSE_MSG_INIT(usart_send_string("ADC..."));
        adc_init();
        VERBOSE_MSG_INIT(usart_send_string(" OK!\n"));
    #else
        VERBOSE_MSG_INIT(usart_send_string("ADC... OFF!\n"));
    #endif

    #ifdef WATCHDOG_ON
        wdt_reset();
    #endif

    #ifdef SLEEP_ON 
        VERBOSE_MSG_INIT(usart_send_string("SLEEP..."));
        sleep_init();
        VERBOSE_MSG_INIT(usart_send_string(" OK!\n"));
    #else
        VERBOSE_MSG_INIT(usart_send_string("SLEEP... OFF!\n"));
    #endif

    #ifdef WATCHDOG_ON
        wdt_reset();
    #endif

 	#ifdef MACHINE_ON
        VERBOSE_MSG_INIT(usart_send_string("MACHINE..."));
		machine_init();
        VERBOSE_MSG_INIT(usart_send_string(" OK!\n"));
    #else
        VERBOSE_MSG_INIT(usart_send_string("MACHINE... OFF!\n"));
	#endif

    #ifdef WATCHDOG_ON
        wdt_reset();
    #endif
	
    #ifdef LED_ON
        set_bit(DDRD, LED1);                      // LED como saída
        set_bit(DDRB, LED2);                      // LED como saída
        VERBOSE_MSG_INIT(usart_send_string("LED... OK!\n"));
    #else
        VERBOSE_MSG_INIT(usart_send_string("LED... OFF!\n"));
    #endif

    #ifdef WATCHDOG_ON
        wdt_reset();
    #endif

    #ifdef MACHINE_ON
        print_configurations();
    #endif // MACHINE_ON

    sei();
}

int main(void)
{
    init();
   
	for(;;){
		#ifdef WATCHDOG_ON
            wdt_reset();
		#endif

        #ifdef MACHINE_ON
            machine_run();
        #endif

		#ifdef SLEEP_ON
            sleep_mode();
		#endif
	}
}


/**
 * @brief se em debug, fica chaveando os pinos de debugs até o reset do watchdog
 */
ISR(BADISR_vect)
{
    for(;;){
        VERBOSE_MSG_ERROR(usart_send_string("\nFATAL ERROR: BAD ISR."));
        #ifdef WATCHDOG_ON
            VERBOSE_MSG_ERROR(usart_send_string("WAITING FOR WATCHDOG TO RESET...\n"));
        #endif
        #ifdef DEBUG_ON
            DEBUG0;
            DEBUG1;
            _delay_ms(100);
        #endif
    }
}

