#ifndef __MQTT_H__
#define __MQTT_H__


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <math.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

#include "freertos/FreeRTOS.h"
#include <freertos/semphr.h>
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "freertos/ringbuf.h"


#include "driver/adc.h"
#include "driver/gpio.h"
#include "driver/ledc.h"

#ifdef SET_SSD1306
    #include "driver/i2c.h"
#endif

#include "esp_system.h"
#include "esp_adc_cal.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_flash_spi_init.h"
#include "nvs.h"
#include "esp_partition.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_wps.h"
#include "esp_vfs.h"
#include "esp_vfs_fat.h"

//SET_SDCARD
#include <sys/unistd.h>
#include <sys/stat.h>
#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"
#include "sdmmc_cmd.h"


//SET_FMB630
#include <endian.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/select.h>
//#include <netinet/in.h>
//#include <sys/un.h>
//#include <sys/time.h>
//#include <sys/resource.h>
//#include <netdb.h>
//#include <arpa/inet.h>
//#include <sys/msg.h>
//#include <sys/socket.h>
//#include <sys/ioctl.h>
//

//SET_SERIAL
#include <sys/fcntl.h>
#include <sys/errno.h>
//#include <sys/unistd.h>
//#include <sys/select.h>
#include "driver/uart.h"
#include "esp_intr_alloc.h"
#include "hal/uart_types.h"
#include "soc/uart_caps.h"
//

#include <esp_intr_alloc.h>
#include "esp_types.h"
#include "stdatomic.h"
#include "esp_netif.h"

#include "crt.h"

#include "esp32/rom/ets_sys.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/sens_reg.h"
#include <soc/rmt_struct.h>
#include <soc/dport_reg.h>
#include <soc/gpio_sig_map.h>

#include "lwip/ip_addr.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "lwip/api.h"
#include "lwip/netif.h"

#include "cJSON.h"

#include "esp_timer.h"

//---------------------------------------

#define DEV_MASTER ((uint8_t)0)

#define TLS_PORT_DEF 4545

#define sntp_srv_len   32
#define wifi_param_len 32


#pragma once


#define MAX_FILE_BUF_SIZE 1024
#define RDBYTE "rb"
#define WRBYTE "wb"
#define WRPLUS "w+"
#define RDONLY "r"
#define WRTAIL "a+"

#define BUF_SIZE 1024

//***********************************************************************

#define LED_ON  1
#define LED_OFF 0

#define GPIO_LOG_PIN     23 //log_server started
//#define GPIO_GPS_PIN     16 //fmb630_client connect to server
#define GPIO_WIFI_PIN     4
#define GPIO_WMODE_PIN   15 //1-STA  0-AP
#define GPIO_DELIT_PIN    2 //delete all files from /spiflash

#define GPIO_RESTART_PIN 25//35
#define GPIO_RESTART_PIN_SEL (1<<GPIO_RESTART_PIN)


#define ADC1_TEST_CHANNEL (6) //6 channel connect to pin34
#define ADC1_TEST_PIN    34 //pin34

#define STACK_SIZE_1K    1024
#define STACK_SIZE_1K5   1536
#define STACK_SIZE_2K    2048
#define STACK_SIZE_2K5   2560
#define STACK_SIZE_3K    3072
#define STACK_SIZE_4K    4096

#define STORAGE_NAMESPACE "nvs"
#define PARAM_CLIID_NAME  "cliid"
#define PARAM_SNTP_NAME   "sntp"
#define PARAM_TZONE_NAME  "tzone"
#define PARAM_SSID_NAME   "ssid"
#define PARAM_KEY_NAME    "key"
#define PARAM_WMODE_NAME  "wmode"
#define PARAM_TLS_PORT    "tport"
#define PARAM_WS_PORT     "wsport"
#define PARAM_CAN_SPEED   "cspeed"


#define EXAMPLE_WIFI_SSID CONFIG_WIFI_SSID
#define EXAMPLE_WIFI_PASS CONFIG_WIFI_PASSWORD
#define max_inbuf         1024

#define EXAMPLE_ESP_MAXIMUM_RETRY  3


#define sntp_server_def "pool.ntp.org"//"2.ru.pool.ntp.org"
#define sntp_tzone_def  "EET-2"


#define _100ms       1
#define _1s    10
#define _2s    2 * _1s
#define _5s    5 * _1s
#define _10s   10 * _1s
#define _15s   15 * _1s
#define _20s   20 * _1s
#define _25s   25 * _1s
#define _30s   30 * _1s




//**************************************************************************

#pragma pack(push,1)
typedef struct {
    fpos_t rseek;
    fpos_t wseek;
    uint32_t total;
} s_log_t;
#pragma pack(pop)

extern const char *Version;

extern uint32_t cli_id;

extern uint8_t wmode;
extern char work_ssid[wifi_param_len];

extern const char server_cert[];
extern const char server_key[];

extern EventGroupHandle_t wifi_event_group;
extern const int CONNECTED_BIT;

extern uint8_t total_task;
extern uint8_t restart_flag;

extern uint8_t tls_hangup;
extern uint32_t tls_client_ip;

extern uint8_t temprature_sens_read();
extern uint32_t get_vcc();
extern float get_tChip();

extern uint32_t get_tmr(uint32_t tm);
extern int check_tmr(uint32_t tm);

extern void print_msg(uint8_t with, const char *tag, const char *fmt, ...);

extern esp_err_t read_param(const char *param_name, void *param_data, size_t len);
extern esp_err_t save_param(const char *param_name, void *param_data, size_t len);


#ifdef SET_NET_LOG
    #include "tcp_srv.h"

    uint16_t net_log_port;
    xQueueHandle msgq;
#endif

#ifdef SET_SNTP
    #include "sntp_cli.h"

    extern uint8_t sntp_go;
    extern char work_sntp[sntp_srv_len];
    extern char time_zone[sntp_srv_len];
#endif

#ifdef UDP_SEND_BCAST
    extern uint8_t udp_start;
    extern int8_t udp_flag;
#endif


#ifdef SET_TLS_SRV
    #include "tls_srv.h"
#endif

#ifdef SET_SSD1306
    #include "ssd1306.h"
#endif

#ifdef SET_CAN_DEV
    #include "can.h"
    extern can_mode_t canMode;
    extern uint16_t canSpeed;
#endif

#ifdef SET_WS
    #include "ws.h"

    xQueueHandle wsq;
#endif


#endif
