/**
 * @file conf.h
 *
 * @defgroup CONF Configurations
 *
 * @brief General configuration of the system.
 *
 */

#ifndef CONF_H
//#define CONF_H

#ifndef F_CPU
#define F_CPU 16000000UL    //define a frequencia do microcontrolador - 16MHz
#endif /* ifndef F_CPU */


// CONFIGURACOES DE COMPILACAO
//#define DEBUG_ON
#define VERBOSE_ON
//#define VERBOSE_ON_CAN_APP
#define VERBOSE_ON_MACHINE
#define VERBOSE_ON_ADC
#define VERBOSE_ON_INIT
#define VERBOSE_ON_ERROR

// MODULES ACTIVATION
#define USART_ON
#define CAN_ON
//#define CAN_DEPENDENT
#define ADC_ON
#define MACHINE_ON
#define LED_ON
#define WATCHDOG_ON
#define SLEEP_ON

#define CAN_SIGNATURE_SELF                  CAN_SIGNATURE_MSC19_4

#ifdef ADC_ON
// ADC CONFIGURATION
// note that changing ADC_FREQUENCY may cause problems with avg_sum_samples
#define ADC_FREQUENCY                       10000 // 20000
#define ADC_TIMER_PRESCALER                 8
#define ADC0_AVG                            adc.channel[ADC0].avg
#define ADC0_ANGULAR_COEF                   10000 //(40000/((4/5)*1024))
#define ADC0_LINEAR_COEF                    0
#define ADC_AVG_SIZE_2                      7                  // in base 2
#define ADC_AVG_SIZE_10                     128                // in base 10

//#define FAKE_ADC_ON
#ifdef FAKE_ADC_ON
#define FAKE_ADC                            1
#endif // FAKE_ADC_ON

#endif //ADC_ON


#ifdef MACHINE_ON
// The machine frequency may not be superior of ADC_FREQUENCY/ADC_AVG_SIZE_10
#define MACHINE_TIMER_FREQUENCY             300           //<! machine timer frequency in Hz
#define MACHINE_TIMER_PRESCALER             1024          //<! machine timer prescaler
#define MACHINE_CLK_DIVIDER_VALUE           ((uint64_t)(uint32_t)MACHINE_TIMER_FREQUENCY*(uint32_t)ADC_AVG_SIZE_10)/(ADC_FREQUENCY)           //<! machine_run clock divider
#define MACHINE_FREQUENCY                   (MACHINE_TIMER_FREQUENCY)/(MACHINE_CLK_DIVIDER_VALUE)

// SCALE TO CONVERT ADC DEFINITIONS
#define VSCALE                              (uint16_t)1000

#endif // MACHINE_ON

//#### LED PINS ####
#ifdef LED_ON
#define     LED1_PORT                   PORTD
#define     LED1_PIN                    PIND
#define     LED1_DDR                    DDRD
#define     LED1                        PD3

#define     LED2_PORT                   PORTB
#define     LED2_PIN                    PINB
#define     LED2_DDR                    DDRB
#define     LED2                        PB2

#else
#define     cpl_led()
#define     set_led()
#define     clr_led()
#endif // LED_ON


#ifdef CAN_ON
#define SPI_ON
#define CAN_APP_SEND_STATE_FREQ     40//36000     //<! state msg frequency in Hz
#define CAN_APP_SEND_ADC_FREQ       4//6000      //<! adc msg frequency in Hz

// CANBUS DEFINITONS
// ----------------------------------------------------------------------------
/* Global settings for building the can-lib and application program.
 *
 * The following two #defines must be set identically for the can-lib and
 * your application program. They control the underlying CAN struct. If the
 * settings disagree, the underlying CAN struct will be broken, with
 * unpredictable results.
 * If can.h detects that any of the #defines is not defined, it will set them
 * to the default values shown here, so it is in your own interest to have a
 * consistent setting. Ommiting the #defines in both can-lib and application
 * program will apply the defaults in a consistent way too.
 *
 * Select if you want to use 29 bit identifiers.
 */
#define	SUPPORT_EXTENDED_CANID	0

/* Select if you want to use timestamps.
 * Timestamps are sourced from a register internal to the AT90CAN.
 * Selecting them on any other controller will have no effect, they will
 * be 0 all the time.
 */
#define	SUPPORT_TIMESTAMPS		0
#endif // CAN_ON

#endif /* ifndef CONF_H */
