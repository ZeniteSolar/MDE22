/**
 * @file adc.h
 *
 * @defgroup ADC    ADC Module
 *
 * @brief This module implements a simple ADC using a state machine to mux
 * between the adc channels.
 *
 */

#ifndef _ADC_H_
#define _ADC_H_

#include "avr/io.h"
#include "avr/interrupt.h"
#include "conf.h"
#include "dbg_vrb.h"
#include "usart.h"
#include "../lib/bit_utils.h"
#include "../lib/log2.h"

typedef enum adc_channels{
    ADC1 ,ADC2, ADC3
} adc_channels_t;                           //*< the adc_channel type
extern volatile uint8_t print_adc;

extern volatile uint16_t batvoltage, position, batcurrent;

void adc_init(void);

#endif /* ifndef _ADC_H_ */
