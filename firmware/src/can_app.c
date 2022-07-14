#include "can_app.h"
#include <math.h>

uint32_t can_app_send_state_clk_div;
uint32_t can_app_send_adc_clk_div;

/**
 * @brief Prints a can message via usart
 */
inline void can_app_print_msg(can_t *msg)
{
#ifdef USART_ON
    usart_send_string("ID: ");
    usart_send_uint16(msg->id);
    usart_send_string(". D: ");

    for(uint8_t i = 0; i < msg->length; i++){
      usart_send_uint16(msg->data[i]);
      usart_send_char(' ');
    }

    usart_send_string(". ERR: ");
    can_error_register_t err = can_read_error_register();
    usart_send_uint16(err.rx);
    usart_send_char(' ');
    usart_send_uint16(err.tx);
    usart_send_char('\n');
#endif
}

/**
* @brief Manages the canbus application protocol
*/
inline void can_app_task(void)
{
    check_can();

    if(can_app_send_state_clk_div++ >= CAN_APP_SEND_STATE_CLK_DIV){
#ifdef USART_ON
        VERBOSE_MSG_CAN_APP(usart_send_string("state msg was sent.\n"));
#endif
        can_app_send_state();
        can_app_send_state_clk_div = 0;
    }

    if(can_app_send_adc_clk_div++ >= CAN_APP_SEND_ADC_CLK_DIV){
#ifdef USART_ON
        VERBOSE_MSG_CAN_APP(usart_send_string("adc msg was sent.\n"));
#endif
        can_app_send_adc();
        can_app_send_adc_clk_div = 0;
    }

}

inline void can_app_send_state(void)
{
    can_t msg;
    msg.id                                  = CAN_MSG_MSC19_1_STATE_ID;
    msg.length                              = CAN_MSG_GENERIC_STATE_LENGTH;
    msg.flags.rtr = 0;

    msg.data[CAN_MSG_GENERIC_STATE_SIGNATURE_BYTE]            = CAN_SIGNATURE_SELF;
    msg.data[CAN_MSG_GENERIC_STATE_STATE_BYTE]      = (uint8_t) state_machine;
    msg.data[CAN_MSG_GENERIC_STATE_ERROR_BYTE]      = error_flags.all;

    can_send_message(&msg);
#ifdef VERBOSE_MSG_CAN_APP
    VERBOSE_MSG_CAN_APP(can_app_print_msg(&msg));
#endif
}

inline void can_app_send_adc(void)
{
    can_t msg;
    msg.id                                  = CAN_MSG_MSC19_1_ADC_ID;
    msg.length                              = CAN_MSG_MSC19_1_ADC_ID;
    msg.flags.rtr = 0;

    uint16_t avg_adc0 =
        (measurements.adc0_avg_sum / measurements.adc0_avg_sum_count);

    uint16_t avg_adc0_converted;
    // float avg_adc0_converted_f;

    /* Valores da regressão polinomial para o sensor 1*/
    #if CAN_SIGNATURE_SELF == CAN_SIGNATURE_MSC19_1
    #define adc_adjust 1.0153122815934938
    #define a0 -0.007214537133162624f * adc_adjust
    #define b0 79.75377726031437f * adc_adjust
    #define c0 -70.91969971593136f * adc_adjust
    avg_adc0_converted = (uint16_t) (a0 * powf(avg_adc0, 2) + b0 * avg_adc0 );

    /* Valores da regressão polinomial para o sensor 2*/
    #elif CAN_SIGNATURE_SELF == CAN_SIGNATURE_MSC19_2
    #define adc_adjust 0.9876390605686033          /* fator de correção para o ADC */
    #define a0 -0.004247907622453196f * adc_adjust
    #define b0 79.14536022160605f * adc_adjust
    #define c0 134.28980589764254f * adc_adjust
    avg_adc0_converted = (uint16_t) (a0 * powf(avg_adc0, 2) + b0 * avg_adc0 + c0);

    /* Valores da regressão polinomial para o sensor 3*/
    #elif CAN_SIGNATURE_SELF == CAN_SIGNATURE_MSC19_3
    #define adc_adjust 1.0125156445556946           /* fator de correção para o ADC */
    #define a0 -0.004090813302699868f * adc_adjust
    #define b0 79.87059674657795f * adc_adjust
    #define c0 11.097134814989177f * adc_adjust
    avg_adc0_converted = (uint16_t) (a0 * powf(avg_adc0, 2) + b0 * avg_adc0 + c0);

    /* Valores da regressão polinomial para o sensor 4*/
    #elif CAN_SIGNATURE_SELF == CAN_SIGNATURE_MSC19_4
    // breaks:
    //  [ 35.33670772  42.91257191 147.61808008]
    #define current_adjust 1.0514018691588785 * 4   /* fator de correção para a corrente */
    #define a0 4.8551075756229f * current_adjust
    #define b0 -596.7507265541577f * current_adjust
    #define c0 24491.777388889182f * current_adjust
    #define d0 -334549.3640250371f * current_adjust
    #define a1 0.0001190802454f * current_adjust
    #define b1 -0.04230098869f * current_adjust
    #define c1 32.739227067299f * current_adjust
    #define d1 -125.1678007132723f * current_adjust
    if(avg_adc0 <= 35){
      avg_adc0_converted = 0;
    }else if(avg_adc0 < 42){
      avg_adc0_converted = (uint16_t) (a0 * powf(avg_adc0, 3) + b0 * powf(avg_adc0, 2)+ c0 * avg_adc0 + d0);
      // avg_adc0_converted_f = b1 * avg_adc0;
      // avg_adc0_converted += (uint16_t) (a1 + (avg_adc0_converted_f * (1.f + (c1 / b1))));
    }else{
      avg_adc0_converted = (uint16_t) (a1 * powf(avg_adc0, 3) + b1 * powf(avg_adc0, 2)+ c1 * avg_adc0 + d1);
    }

    /* Valores da regressão polinomial para o sensor 5*/
    #elif CAN_SIGNATURE_SELF == CAN_SIGNATURE_MSC19_5
    // breaks:
    //  [ 32.5787237   37.72656059 744.76651233]
    #define coef_adjust 1.1386079714455681              /* Coeficiente de ajuste entre as placas */
    #define current_adjust 1.065984930032293            /* fator de correção para a corrente */

    #define a0 -6.2127297775771f * current_adjust * coef_adjust
    #define b0 642.5007704667008f * current_adjust * coef_adjust
    #define c0 -21901.214999298354f * current_adjust * coef_adjust
    #define d0 246339.3460664462f * current_adjust * coef_adjust
    #define a1 -6.127458e-07f * current_adjust * coef_adjust
    #define b1 -0.0058459342976f * current_adjust * coef_adjust
    #define c1 25.5236526067201f * current_adjust * coef_adjust
    #define d1 -1.677696304585f * current_adjust * coef_adjust
    if(avg_adc0 <= 32){
      avg_adc0_converted = 0;
    }else if(avg_adc0 < 37){
      avg_adc0_converted = (uint16_t) (a0 * powf(avg_adc0, 3) + b0 * powf(avg_adc0, 2)+ c0 * avg_adc0 + d0);
    }else{
      avg_adc0_converted = (uint16_t) (a1 * powf(avg_adc0, 3) + b1 * powf(avg_adc0, 2)+ c1 * avg_adc0 + d1);
    }
    #endif


    msg.data[CAN_MSG_GENERIC_STATE_SIGNATURE_BYTE]            = CAN_SIGNATURE_SELF;
    msg.data[CAN_MSG_MSC19_1_ADC_AVG_L_BYTE]  = LOW(avg_adc0_converted);
    msg.data[CAN_MSG_MSC19_1_ADC_AVG_L_BYTE]  = HIGH(avg_adc0_converted);
    msg.data[CAN_MSG_MSC19_1_ADC_MIN_L_BYTE]  = LOW(measurements.adc0_min);
    msg.data[CAN_MSG_MSC19_1_ADC_MIN_L_BYTE]  = HIGH(measurements.adc0_min);
    msg.data[CAN_MSG_MSC19_1_ADC_MAX_L_BYTE]  = LOW(measurements.adc0_max);
    msg.data[CAN_MSG_MSC19_1_ADC_MAX_L_BYTE]  = HIGH(measurements.adc0_max);

    can_send_message(&msg);
#ifdef VERBOSE_MSG_CAN_APP
    VERBOSE_MSG_CAN_APP(can_app_print_msg(&msg));
#endif

    reset_measurements();
}

/**
 * @brief extracts the specific MIC19 STATE message
 * @param *msg pointer to the message to be extracted
 */
inline void can_app_extractor_mic17_state(can_t *msg)
{
    // TODO:
    //  - se tiver em erro, desligar acionamento
    if(msg->data[CAN_MSG_GENERIC_STATE_SIGNATURE_BYTE] == CAN_SIGNATURE_MIC19){
        // zerar contador
        if(msg->data[CAN_MSG_GENERIC_STATE_ERROR_BYTE]){
            //ERROR!!!
        }
        /*if(contador == maximo)*/{
            //ERROR!!!
        }
    }
}

/**
 * @brief redirects a specific message extractor to a given message
 * @param *msg pointer to the message to be extracted
 */
inline void can_app_msg_extractors_switch(can_t *msg)
{
    if(msg->data[CAN_MSG_GENERIC_STATE_SIGNATURE_BYTE] == CAN_SIGNATURE_MIC19){
        switch(msg->id){
            case CAN_MSG_MIC19_STATE_ID:
#ifdef USART_ON
                VERBOSE_MSG_CAN_APP(usart_send_string("got a state msg: "));
#endif
                VERBOSE_MSG_CAN_APP(can_app_print_msg(msg));
                can_app_extractor_mic17_state(msg);
                break;
            default:
#ifdef USART_ON
                VERBOSE_MSG_CAN_APP(usart_send_string("got a unknown msg: "));
#endif
                VERBOSE_MSG_CAN_APP(can_app_print_msg(msg));
                break;
        }
    }
}

/**
 * @brief Manages to receive and extract specific messages from canbus
 */
inline void check_can(void)
{
    // If no messages is received from mic17 for
    // CAN_APP_CHECKS_WITHOUT_MIC19_MSG cycles, than it go to a specific error state.
    //VERBOSE_MSG_CAN_APP(usart_send_string("checks: "));
    //VERBOSE_MSG_CAN_APP(usart_send_uint16(can_app_checks_without_mic17_msg));
#ifdef CAN_DEPENDENT
    if(can_app_checks_without_mic17_msg++ >= CAN_APP_CHECKS_WITHOUT_MIC19_MSG){
#ifdef USART_ON
        VERBOSE_MSG_CAN_APP(usart_send_string("Error: too many cycles withtou message.\n"));
#endif
        can_app_checks_without_mic17_msg = 0;
        error_flags.no_canbus = 1;
        set_state_error();
    }
#endif

    if(can_check_message()){
        can_t msg;
        if(can_get_message(&msg)){
            can_app_msg_extractors_switch(&msg);
        }
    }
}
