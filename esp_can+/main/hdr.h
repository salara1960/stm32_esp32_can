//#define UDP_SEND_BCAST

#define SET_CAN_DEV
//#define SET_TLS_SRV
#define SET_NET_LOG
#define SET_SSD1306
#define SET_SNTP
#define SET_WS

#ifdef SET_TLS_SRV
    #undef SET_TIMEOUT60
#endif
