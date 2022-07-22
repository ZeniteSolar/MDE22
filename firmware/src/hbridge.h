/**
 * @file hbridge.h
 *
 * @defgroup HBRIDGE
 *
 * @brief Implements hbridge activation and control, wich include:
 *  - pwm for bridge control (0 = low side, 1 = high side)
 *  - bridge enable
 *          
 *          for each side of the bridge.
 *
 * Notes on used IC BTS7960:
 * 
 * Protection Functions: 
 *      Overtemperature Protection:
 * Overtemperature leads to a shut down of both output stages,
 * this state is latched until the device is reset by a low signal
 * with a minimum length of t_reset at the INH pin (bridge enable)
 *      
 *      Current Limitation:
 * As soon as the current in forward direction is reaching the limit, (lower is ICL low side = 43A for Tj=150°C [Vs 8:18]),
 * the switch is deactivated and the other switch is activated for tCLS (maximum is 210us for VS=13,5V)
 * INH (bridge enable) still works for shutdown.
 * Due to application nature shutdown should be applied.
 *
 * Dead Time Generation: implemented by IC driver, adjusted automatically 
 * to the selectd slew rate
 *
 */

#ifndef HBRIDGE_H
#define HBRIDGE_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include "conf.h"
#include "../lib/bit_utils.h"
#include "usart.h"

extern volatile uint16_t position_target, position_tail;
extern volatile uint32_t hbridge_testing_clk_div;

//Equation
void hbridge_init(void);
void hbridge_testing(void);
//void hbridge_toggle_side(void);
//void hbridge_follow(uint16_t position_target);

#endif /* ifndef HBRIDGE_H */

