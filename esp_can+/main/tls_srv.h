#ifndef __TLS_SRV_H__
#define __TLS_SRV_H__

#include "hdr.h"

#ifdef SET_TLS_SRV

    #define SET_MD5

    #ifndef SET_MD5
        #include "esp32/sha.h"
        #define SET_SHA1
        #undef SET_SHA2_256
        #undef SET_SHA2_384
        #undef SET_SHA2_512
    #endif

    #include "mbedtls/config.h"
    #include "mbedtls/net.h"
    #include "mbedtls/debug.h"
    #include "mbedtls/ssl.h"
    #include "mbedtls/entropy.h"
    #include "mbedtls/ctr_drbg.h"
    #include "mbedtls/error.h"
    #include "mbedtls/certs.h"
    #include "mbedtls/md5.h"

    #define def_idle_count    60
    #define timeout_auth   30000
    #define timeout_def     2000
    #define timeout_max    60000


    #pragma pack(push,1)
    typedef struct {
        unsigned first : 1;
        unsigned first_send : 1;
        unsigned none : 6;
    } s_tls_flags;
    #pragma pack(pop)

    #pragma pack(push,1)
    typedef struct {
        unsigned char mip[4];
    } s_tls_client_ip;
    #pragma pack(pop)

    typedef enum {
        iCTRL_AUTH = 0,//"auth"//0
        iCTRL_UDP,//"udp"//1
        iCTRL_SNTP,//"sntp"//2
        iCTRL_SNTP_SRV,//"sntp_srv"//3
        iCTRL_TIME_ZONE,//"time_zone"//4
        iCTRL_RESTART,//"restart"//5
        iCTRL_TIME,//"time"//6
        iCTRL_FTP_GO,//"ftp_go"//7
        iCTRL_FTP_SRV,//"ftp_srv"//8
        iCTRL_FTP_USER,//"ftp_user"//9
        iCTRL_GET//"get"//10
    } i_ctrl_t;
    typedef enum {
        sCTRL_STATUS = 0,//"status",
        sCTRL_WIFI,//"wifi",     //{"wifi":"ssid:password"}
        sCTRL_SNTP_SRV,//"sntp_srv", //{"sntp_srv":"2.ru.pool.ntp.org"}
        sCTRL_TIME_ZONE,//"time_zone",//{"time_zone":"UTC+02:00"}
        sCTRL_FTP_SRV,//"ftp_srv",  //{"ftp_srv":"192.168.0.201:21"}
        sCTRL_FTP_USER,//"ftp_user", //{"ftp_user":"login:password"}
        sCTRL_LOG_PORT,//"log_port", //{"log_port":"8008"}
        sCTRL_VERSION,//"version"   //{"version":"4.2 (22.01.2020)"}
        sCTRL_GPS_SRV,//"gps_srv",  //{"gps_srv":"192.168.0.201:9090"}
        sCTRL_GPS_INFO//"gps_info", //{"gps_info":{...}}
    } s_ctrl_t;


    extern char tls_cli_ip_addr[32];

    extern const char *TAGTLS;
    extern uint8_t tls_start;
    extern uint8_t tls_hangup;
    extern time_t mk_hash(char *out, const char *part);
    extern void tls_task(void *arg);
#endif

#endif
