#ifndef __SNTP_CLI_H__
#define __SNTP_CLI_H__

#include "hdr.h"

#ifdef SET_SNTP
    #include <time.h>
    #include <sys/time.h>
    #include "esp_sntp.h"

    extern uint8_t setDateTimeOK;
    extern const char *TAGS;
    extern uint8_t sntp_start;
    extern void sntp_task(void *arg);
#endif

#endif
