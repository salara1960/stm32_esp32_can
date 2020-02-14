#undef UDP_SEND_BCAST
#undef SET_TLS_SRV

#define SET_CAN_DEV
#define SET_NET_LOG
#define SET_SSD1306
#define SET_SNTP

#ifdef SET_TLS_SRV
    #undef SET_TIMEOUT60
#endif
