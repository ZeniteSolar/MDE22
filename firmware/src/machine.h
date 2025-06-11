/**
 * @file machine.h
 *
 * @defgroup MACHINE State Machine Module
 *
 * @brief Implements the main state machine of the system.
 *
 */

#ifndef MACHINE_H
#define MACHINE_H

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>

//***
#include <util/delay.h>

#include "conf.h"


// PRINT INFOS CONSTANTS
#define PRINT_INFOS_TIME        0.2 // seconds for half a period 
#define PRINT_INFOS_CLK_DIV     PRINT_INFOS_TIME * MACHINE_FREQUENCY

// LED CONSTANTS
#define IDLE_LED_TIME           1 // half a period...time that the LED stays on
#define IDLE_LED_CLK_DIV        IDLE_LED_TIME * 2 * MACHINE_FREQUENCY //  
#define RUNNING_LED_TIME        0.25 // half a period...time that the LED stays on
#define RUNNING_LED_CLK_DIV     RUNNING_LED_TIME * 2 * MACHINE_FREQUENCY //  
#define ERROR_LED_TIME          0.05 // half a period...time that the LED stays on
#define ERROR_LED_CLK_DIV       ERROR_LED_TIME * 2 * MACHINE_FREQUENCY //  


#ifdef ADC_ON
#include "adc.h"
#endif
#ifdef USART_ON
#include "usart.h"
#endif
#include "dbg_vrb.h"
#ifdef CAN_ON
#include "can.h"
#include "can_app.h"
#endif
#ifdef HBRIDGE_ON
#include "hbridge.h"
#endif

typedef enum state_machine{
    STATE_INITIALIZING,
    STATE_IDLE,
    STATE_RUNNING,
    STATE_ERROR,
    STATE_RESET,
} state_machine_t;

typedef union system_flags{
    struct{
        uint8_t     none          :1;
    };
    uint8_t   all;
} system_flags_t;

typedef union error_flags{
    struct{
        uint8_t     no_canbus       :1;
        uint8_t     invalid_tail    :1;
        uint8_t     wrong_side_turn :1;
    };
    uint8_t   all;
}error_flags_t;

// TODO: different structures, then add to measurements
typedef struct measurements{
    uint16_t    batvoltage_avg;       
    uint16_t    batvoltage_avg_sum_count;
    uint64_t    batvoltage_avg_sum;   
    uint8_t    position_avg;       
    uint16_t    position_avg_sum_count;
    uint64_t    position_avg_sum;   
    uint16_t    batcurrent_avg;       
    uint16_t    batcurrent_avg_sum_count;
    uint64_t    batcurrent_avg_sum;   
}measurements_t;


// machine checks
void check_buffers(void);

// measurements
void reset_measurements(void);
void compute_measurements(void);
void average_measurements(void);

// debug functions
void print_configurations(void);
void print_system_flags(void);
void print_error_flags(void);
void print_infos(void);

// machine tasks
void task_initializing(void);
void task_idle(void);
void task_running(void);
void task_error(void);
void task_reset(void);

// the machine itself
void set_machine_initial_state(void);
void machine_init(void);
void machine_run(void);
void set_state_error(void);
void set_state_initializing(void);
void set_state_idle(void);
void set_state_running(void);
void set_state_reset(void);

// machine variables
extern volatile state_machine_t state_machine;
extern volatile system_flags_t system_flags;
extern volatile error_flags_t error_flags;
extern volatile measurements_t measurements;
extern volatile uint8_t machine_clk;
extern volatile uint8_t machine_clk_divider;
extern volatile uint8_t total_errors;           // Contagem de ERROS
extern volatile float str_whl_position;

// other variables
extern volatile uint8_t led_clk_div;

#endif /* ifndef MACHINE_H */
