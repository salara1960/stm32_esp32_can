#include "hdr.h"

#ifdef SET_CAN_DEV

#include "main.h"


//------------------------------------------------------------------------------------------------------------

const char *TAGCAN = "eCAN";
const char *canModeName[] = {"CAN_MODE_NORMAL", "CAN_MODE_NO_ACK", "CAN_MODE_LISTEN_ONLY", "UNKNOWN"};

//------------------------------------------------------------------------------------------------------------
/*
#define CAN_TIMING_CONFIG_25KBITS()  {.brp = 128, .tseg_1 = 16, .tseg_2 = 8, .sjw = 3, .triple_sampling = false}
#define CAN_TIMING_CONFIG_50KBITS()  {.brp = 80,  .tseg_1 = 15, .tseg_2 = 4, .sjw = 3, .triple_sampling = false}
#define CAN_TIMING_CONFIG_100KBITS() {.brp = 40,  .tseg_1 = 15, .tseg_2 = 4, .sjw = 3, .triple_sampling = false}
#define CAN_TIMING_CONFIG_125KBITS() {.brp = 32,  .tseg_1 = 15, .tseg_2 = 4, .sjw = 3, .triple_sampling = false}
#define CAN_TIMING_CONFIG_250KBITS() {.brp = 16,  .tseg_1 = 15, .tseg_2 = 4, .sjw = 3, .triple_sampling = false}
#define CAN_TIMING_CONFIG_500KBITS() {.brp = 8,   .tseg_1 = 15, .tseg_2 = 4, .sjw = 3, .triple_sampling = false}
#define CAN_TIMING_CONFIG_800KBITS() {.brp = 4,   .tseg_1 = 16, .tseg_2 = 8, .sjw = 3, .triple_sampling = false}
#define CAN_TIMING_CONFIG_1MBITS()   {.brp = 4,   .tseg_1 = 15, .tseg_2 = 4, .sjw = 3, .triple_sampling = false}
*/
void selectCanSpeed(can_timing_config_t *tc, uint16_t *spd)
{

    tc->triple_sampling = false;
    tc->sjw = 3;
    switch (*spd) {
        case 25://25KBITS
            tc->brp = 128;
            tc->tseg_1 = 16;
            tc->tseg_2 = 8;
        break;
        case 50://50KBITS
            tc->brp = 80;
            tc->tseg_1 = 15;
            tc->tseg_2 = 4;
        break;
        case 100://100KBITS
            tc->brp = 40;
            tc->tseg_1 = 15;
            tc->tseg_2 = 4;
        break;
        case 250://250KBITS
            tc->brp = 16;
            tc->tseg_1 = 15;
            tc->tseg_2 = 4;
        break;
        case 500://500KBITS
            tc->brp = 8;
            tc->tseg_1 = 15;
            tc->tseg_2 = 4;
        break;
        case 800://800KBITS
            tc->brp = 4;
            tc->tseg_1 = 16;
            tc->tseg_2 = 8;
        break;
        case 1000://1000KBITS
            tc->brp = 4;
            tc->tseg_1 = 15;
            tc->tseg_2 = 4;
        break;
            default : {//125KBITS
                tc->brp = 32;
                tc->tseg_1 = 15;
                tc->tseg_2 = 4;
                *spd = 125;
            }
    }
}
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
static esp_err_t send_packet(can_message_t *msg, uint32_t *counter, const uint32_t ms)
{
uint32_t cnt = *counter;
char stx[128] = {0};

    esp_err_t ret = can_transmit(msg, ms / portTICK_RATE_MS);
    if (ret == ESP_OK) {//portMAX_DELAY)
        cnt++;
        ssd1306_clear_lines(ScreenTxLine, 1);
        ssd1306_text_xy(stx, ssd1306_calcx(sprintf(stx, "Tx msg #%u", cnt)), ScreenTxLine);
        *counter = cnt;
        sprintf(stx, "Id=0x%X #%u TX[%u]=", msg->identifier, cnt, msg->data_length_code);
        for (int8_t i = 0; i < msg->data_length_code; i++) sprintf(stx+strlen(stx), " %02X", msg->data[i]);
    } else sprintf(stx, "Can_tx Error (%d) : %s", ret, canErrStr(ret));
    print_msg(1, TAGCAN, "%s\n", stx);

    return ret;
}
//------------------------------------------------------------------------------------------------------------
static esp_err_t recv_packet(can_message_t *msg, uint32_t *counter, const uint32_t ms, bool *ok)
{
uint32_t cnt = *counter;
s_float_t *vcc = NULL;
uint32_t *ts = NULL;
char stx[128] = {0};

    esp_err_t ret = can_receive(msg, ms / portTICK_RATE_MS);
    if (ret == ESP_OK) {
        cnt++;
        ssd1306_clear_lines(ScreenRxLine, 1);
        ssd1306_text_xy(stx, ssd1306_calcx(sprintf(stx, "Rx msg #%u", cnt)), ScreenRxLine);

        if ((msg->identifier == MSG_PACK) && (msg->data_length_code == MAX_LEN_DATA)) {
            ts  = (uint32_t *)&msg->data[0];
            vcc = (s_float_t *)&msg->data[4];
            sprintf(stx, "Id=0x%X #%u Vcc=%u.%u Time=%u RX[%u]=",
                         msg->identifier, cnt, vcc->cel, vcc->dro, *ts, msg->data_length_code);
        } else sprintf(stx, "Id=0x%X #%u RX[%u]=", msg->identifier, cnt, msg->data_length_code);
        for (int8_t i = 0; i < msg->data_length_code; i++) sprintf(stx+strlen(stx), " %02X", msg->data[i]);
        print_msg(1, TAGCAN, "%s\n", stx);

        *counter = cnt;
        *ok = true;
    } else {
        if (ret != ESP_ERR_TIMEOUT) print_msg(1, TAGCAN, "Can_rx Error : %s\n", TAGCAN, canErrStr(ret));
    }

    return ret;
}
//------------------------------------------------------------------------------------------------------------
void can_task(void *arg)
{
total_task++;

uint32_t loop_tmr = get_tmr(_10s);
can_message_t rx_msg = {0};
can_message_t tx_msg = {
    .data_length_code = MAX_LEN_DATA,//8
    .identifier       = MSG_PACK,//0x321
    .self             = 0
};
uint64_t *bit64 = (uint64_t *)&tx_msg.data[0];
uint32_t cnt_tx = 0, cnt_rx = 0;

bool yes = false;
#ifdef SET_EPOCH_SEND
    bool send_epoch = false;
#endif

    uint32_t spd = *(uint32_t *)arg;
    if (canMode == CAN_MODE_NO_ACK) tx_msg.self = 1;


    ets_printf("[%s] Start can_task with mode='%s'(%d) speed=%u KHz | FreeMem %u\n",
               TAGCAN, canModeName[canMode&3], canMode, spd, xPortGetFreeHeapSize());

    while (!restart_flag) {

        if (canMode == CAN_MODE_NO_ACK) {
#ifdef SET_EPOCH_SEND
            send_epoch = true;
#endif
            if (check_tmr(loop_tmr)) {
                (*bit64)++;
                send_packet(&tx_msg, &cnt_tx, 10);
                loop_tmr = get_tmr(5*_1s);
            }
        }
        //
        recv_packet(&rx_msg, &cnt_rx, 10, &yes);
        //
#ifdef SET_EPOCH_SEND
        if (!send_epoch) {
            if (setDateTimeOK && yes) {//send MSG_EPOCH msg
                //
                tx_msg.data_length_code = sizeof(uint32_t);
                time_t it_time = time(NULL);
                memcpy((uint8_t *)&tx_msg.data[0], (uint8_t *)&it_time, sizeof(time_t));
                send_packet(&tx_msg, &cnt_tx, 200);
                send_epoch = true;
            }
        }
#endif
        vTaskDelay(10/portTICK_PERIOD_MS);

    }

    ets_printf("[%s] Stop can_task | FreeMem %u\n", TAGCAN, xPortGetFreeHeapSize());

    if (total_task) total_task--;

    vTaskDelete(NULL);
}
//------------------------------------------------------------------------------------------------------------

#endif
