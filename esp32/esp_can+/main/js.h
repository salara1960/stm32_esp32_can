#ifndef __JS_H__
#define __JS_H__

#include "hdr.h"

#include "main.h"


typedef enum {
    iCTRL_AUTH = 0,  //"auth"//0
    iCTRL_UDP,       //"udp"//1
    iCTRL_SNTP,      //"sntp"//2
    iCTRL_SNTP_SRV,  //"sntp_srv"//3
    iCTRL_TIME_ZONE, //"time_zone"//4
    iCTRL_RESTART,   //"restart"//5
    iCTRL_TIME,      //"time"//6
    iCTRL_CAN_SPEED, //"can_speed"//7
    iCTRL_GET        //"get"//8
} i_ctrl_t;

typedef enum {
    sCTRL_STATUS = 0,//0//"status",
    sCTRL_WIFI,      //1//"wifi",     //{"wifi":"ssid:password"}
    sCTRL_SNTP_SRV,  //2//"sntp_srv", //{"sntp_srv":"2.ru.pool.ntp.org"}
    sCTRL_TIME_ZONE, //3//"time_zone",//{"time_zone":"UTC+02:00"}
    sCTRL_CAN_SPEED, //4//"can_speed",//{"can_speed":"250"}
    sCTRL_LOG_PORT,  //5//"log_port", //{"log_port":"8008"}
    sCTRL_VERSION    //6//"version"   //{"version":"4.2 (22.01.2020)"}
} s_ctrl_t;


extern int parser_json_str(const char *st, bool *au, const char *str_md5, uint8_t *rst);
extern int mkAck(char *tbuf, int ictrl, int sctrl, bool *au);



#endif

