#ifndef __CAN_H__
#define __CAN_H__

#include "hdr.h"

#ifdef SET_CAN_DEV
    #include "driver/can.h"

    //#define SET_EPOCH_SEND

    #define TX_GPIO_NUM             17//21
    #define RX_GPIO_NUM             16//22
    #define MSG_PACK                0x321//0x555   //11 bit standard format ID
    #define MSG_TIME_SET            0x320
    #define MAX_LEN_DATA            8

    #define ScreenTxLine            4
    #define ScreenRxLine            5

    #pragma pack(push,1)
    typedef struct {
        uint16_t cel;
        uint16_t dro;
    } s_float_t;
    #pragma pack(pop)

    extern const char *TAGCAN;

    extern void selectCanSpeed(can_timing_config_t *tc, uint16_t *spd);
    extern void can_task(void *arg);
#endif

#endif
