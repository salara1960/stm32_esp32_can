#include "hdr.h"

#ifdef SET_CAN_DEV

#include "main.h"


//------------------------------------------------------------------------------------------------------------

const char *TAGCAN = "eCAN";
const char *canModeName[] = {"CAN_MODE_NORMAL", "CAN_MODE_NO_ACK", "CAN_MODE_LISTEN_ONLY", "UNKNOWN"};
/*
static const can_timing_config_t t_config = CAN_TIMING_CONFIG_500KBITS();
//Filter all other IDs except MSG_ID
static const can_filter_config_t f_config = {
    .acceptance_code = (MSG_ID << 21),
    .acceptance_mask = ~(CAN_STD_ID_MASK << 21),
    .single_filter = true
};
//Set to NO_ACK mode due to self testing with single module
static const can_general_config_t g_config = CAN_GENERAL_CONFIG_DEFAULT(TX_GPIO_NUM, RX_GPIO_NUM, CAN_MODE_NO_ACK);
*/
//------------------------------------------------------------------------------------------------------------

static char *canErrStr(esp_err_t code)
{
    switch (code) {
	case ESP_ERR_INVALID_ARG:
	    return "Arguments are invalid";
	case ESP_ERR_TIMEOUT:
	    return "Timed out waiting for space on TX queue";
	case ESP_FAIL:
	    return "TX queue is disabled and another msg is currently transmitting";
	case ESP_ERR_INVALID_STATE:
	    return "CAN driver not started or not installed";
	case ESP_ERR_NOT_SUPPORTED:
	    return "Listen Only Mode does not support transmissions";
    }
    return "Unknown error";
}

//------------------------------------------------------------------------------------------------------------
//typedef struct {
//    union {
//        struct {
//            //The order of these bits must match deprecated message flags for compatibility reasons
//            uint32_t extd: 1;           /**< Extended Frame Format (29bit ID) */
//            uint32_t rtr: 1;            /**< Message is a Remote Transmit Request */
//            uint32_t ss: 1;             /**< Transmit as a Single Shot Transmission. Unused for received. */
//            uint32_t self: 1;           /**< Transmit as a Self Reception Request. Unused for received. */
//            uint32_t dlc_non_comp: 1;   /**< Message's Data length code is larger than 8. This will break compliance with CAN2.0B. */
//            uint32_t reserved: 27;      /**< Reserved bits */
//        };
//        //Todo: Deprecate flags
//        uint32_t flags;                 /**< Alternate way to set message flags using message flag macros (see documentation) */
//    };
//    uint32_t identifier;                /**< 11 or 29 bit identifier */
//    uint8_t data_length_code;           /**< Data length code */
//    uint8_t data[CAN_FRAME_MAX_DLC];    /**< Data bytes (not relevant in RTR frame) */
//} can_message_t;
//------------------------------------------------------------------------------------------------------------
void can_task(void *arg)
{
total_task++;
esp_err_t err;
uint32_t loop_tmr = get_tmr(_10s);
char stx[128];
char screen[64];
can_message_t rx_msg = {0};
can_message_t tx_msg = {
    .data_length_code = MAX_LEN_DATA,//8
    .identifier       = MSG_ID,//0x321
    .self             = 0
};
uint64_t *bit64 = (uint64_t *)&tx_msg.data[0];
uint32_t cnt_tx = 0, cnt_rx = 0;
if (canMode == CAN_MODE_NO_ACK) tx_msg.self = 1;
s_float_t *vcc = NULL;
uint32_t *ts = NULL;

    uint32_t spd = *(uint32_t *)arg;


    ets_printf("[%s] Start can_task with mode='%s'(%d) speed=%u KHz | FreeMem %u\n",
               TAGCAN, canModeName[canMode&3], canMode, spd, xPortGetFreeHeapSize());

    while (!restart_flag) {
        if (canMode == CAN_MODE_NO_ACK) {
            if (check_tmr(loop_tmr)) {
                (*bit64)++;
                if ((err = can_transmit(&tx_msg, 500 / portTICK_RATE_MS)) == ESP_OK) {//portMAX_DELAY)
                    ssd1306_clear_lines(ScreenTxLine, 1);
                    ssd1306_text_xy(screen, ssd1306_calcx(sprintf(screen, "Tx msg #%u", ++cnt_tx)), ScreenTxLine);
                    stx[0] = 0;
                    for (int8_t i = 0; i < MAX_LEN_DATA; i++) sprintf(stx+strlen(stx), " %02X", tx_msg.data[i]);
                    print_msg(1, TAGCAN, "Id=0x%X #%u TX[%u]=%s\n", tx_msg.identifier, cnt_tx, tx_msg.data_length_code, stx);
                } else {
                    print_msg(1, TAGCAN, "Can_tx Error (%d) : %s\n", TAGCAN, err, canErrStr(err));
                }
                loop_tmr = get_tmr(5*_1s);
            }
        }

        if ((err = can_receive(&rx_msg, 10 / portTICK_RATE_MS)) == ESP_OK) {//portMAX_DELAY);
            cnt_rx++;
            ssd1306_clear_lines(ScreenRxLine, 1);
            ssd1306_text_xy(screen, ssd1306_calcx(sprintf(screen, "Rx msg #%u", cnt_rx)), ScreenRxLine);
            stx[0] = 0;
            for (int8_t i = 0; i < rx_msg.data_length_code; i++) sprintf(stx+strlen(stx), " %02X", rx_msg.data[i]);
             //
            if ((rx_msg.identifier == 0x321) && (rx_msg.data_length_code == MAX_LEN_DATA)) {
                vcc = (s_float_t *)&rx_msg.data[0];
                ts = (uint32_t *)&rx_msg.data[4];
                print_msg(1, TAGCAN, "Id=0x%X #%u Vcc=%u.%u Time=%lu RX[%u]=%s\n",
                                      rx_msg.identifier, cnt_rx, vcc->cel, vcc->dro, *ts, rx_msg.data_length_code, stx);
            } else {
                print_msg(1, TAGCAN, "Id=0x%X #%u RX[%u]=%s\n", rx_msg.identifier, cnt_rx, rx_msg.data_length_code, stx);
            }
        } else {
            if (err != ESP_ERR_TIMEOUT) print_msg(1, TAGCAN, "Can_rx Error : %s\n", TAGCAN, canErrStr(err));
        }

        vTaskDelay(10/portTICK_PERIOD_MS);
    }

    ets_printf("[%s] Stop can_task | FreeMem %u\n", TAGCAN, xPortGetFreeHeapSize());

    if (total_task) total_task--;

    vTaskDelete(NULL);
}
//------------------------------------------------------------------------------------------------------------

#endif
