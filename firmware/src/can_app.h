/**
 * @file can_app.h
 *
 * @defgroup CANAPP    Application Module for canbus
 *
 * @brief The application layer module for canbus
 *
 */

#ifndef CAN_APP_H
#define CAN_APP_H

#include "conf.h"
#include "dbg_vrb.h"
#include "../lib/bit_utils.h"
#include "can.h"
#include "can_ids.h"
#include "machine.h"
#include "usart.h"
#include "hbridge.h"

void can_app_print_msg(can_t *msg);
void can_app_task(void);
void can_app_send_state(void);
void can_app_send_steeringbat_measurements(void);

void can_app_msg_extractors_switch(can_t *msg);
void can_app_extractor_mic19_state(can_t *msg);
void can_app_extractor_mic19_mde(can_t *msg);
void can_app_extractor_mic19_pumps(can_t *msg);
void check_can(void);

#ifdef CAN_ON
#define CAN_APP_SEND_STATE_CLK_DIV  CAN_APP_SEND_STATE_FREQ
#define CAN_APP_SEND_ADC_CLK_DIV    CAN_APP_SEND_ADC_FREQ
#else
#define CAN_APP_SEND_STATE_CLK_DIV  1
#define CAN_APP_SEND_ADC_CLK_DIV    1
#endif

#define CAN_APP_CHECKS_WITHOUT_MIC19_MSG  500

typedef union {
    struct {
        uint8_t     no_mic    :1;   //ToDo?: expand flags for no_mic_mde, no_mic_state
    };
    uint8_t     all__;
} can_app_flags_t;

extern uint32_t can_app_send_state_clk_div;
extern uint32_t can_app_send_adc_clk_div;
extern volatile can_app_flags_t can_app_flags;
extern uint8_t sense_select;



#endif /* ifndef CAN_APP_H */