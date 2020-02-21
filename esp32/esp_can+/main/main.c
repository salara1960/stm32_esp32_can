#include "hdr.h"
#include "main.h"

#include "../version.c"


//*******************************************************************

volatile uint8_t restart_flag = 0;

uint8_t total_task = 0;
uint8_t last_cmd = 255;

const uint32_t clkBaseKHz = 80000;
static const char *TAG = "MAIN";
static const char *TAGN = "NVS";
static const char *TAGT = "VFS";
static const char *TAGW = "WIFI";

static const char *wmode_name[] = {"NULL", "STA", "AP", "APSTA", "MAX"};
uint8_t wmode = WIFI_MODE_STA;
static unsigned char wifi_param_ready = 0;
char work_ssid[wifi_param_len] = {0};
static char work_pass[wifi_param_len] = {0};
#ifdef SET_SNTP
    char work_sntp[sntp_srv_len] = {0};
    char time_zone[sntp_srv_len] = {0};
#endif
EventGroupHandle_t wifi_event_group;
const int CONNECTED_BIT = BIT0;
xSemaphoreHandle prn_mutex;


uint8_t *macs = NULL;

static char sta_mac[18] = {0};
uint32_t cli_id = 0;
char localip[32] = {0};
esp_ip4_addr_t ip4;
ip_addr_t bca;


#ifdef UDP_SEND_BCAST
    static const char *TAGU = "UDP";
    volatile uint8_t udp_start = 0;
    volatile int8_t udp_flag = 0;
#endif

#ifdef SET_SNTP
    volatile uint8_t sntp_go = 0;
#endif


uint16_t ws_port = 0;


static int s_retry_num = 0;

static uint32_t varta = 0;
static bool scr_ini_done = false;
//static uint8_t scr_len = 0;
//static char scr_line[32] = {0};
static uint8_t secFlag = 10;
static uint8_t led = LED_OFF;
uint32_t tperiod = 100000;//100000 us = 100 ms


#ifdef SET_NET_LOG
    uint16_t net_log_port = 8008;
#endif

#ifdef SET_CAN_DEV
    can_mode_t canMode = CAN_MODE_NORMAL;
    uint16_t canSpeed = 125;
#endif


//***************************************************************************************************************

esp_err_t read_param(const char *param_name, void *param_data, size_t len);
esp_err_t save_param(const char *param_name, void *param_data, size_t len);

//***************************************************************************************************************

//--------------------------------------------------------------------------------------
static uint32_t get_varta()
{
    return varta;
}
//--------------------------------------------------------------------------------------
static void periodic_timer_callback(void* arg)
{
    varta++; //100ms period

    secFlag--;
    if (!secFlag) {
        secFlag = tperiod/10000;//10;
        led = ~led;
        gpio_set_level(GPIO_LOG_PIN, led);
    }
}
//--------------------------------------------------------------------------------------
uint32_t get_tmr(uint32_t tm)
{
    return (get_varta() + tm);
}
//--------------------------------------------------------------------------------------
int check_tmr(uint32_t tm)
{
    if (!tm) return 0;
    return (get_varta() >= tm ? 1 : 0);
}
//--------------------------------------------------------------------------------------
void print_msg(uint8_t with, const char *tag, const char *fmt, ...)
{
size_t len = BUF_SIZE;//1024

    char *st = (char *)calloc(1, len);
    if (st) {
        if (xSemaphoreTake(prn_mutex, portMAX_DELAY) == pdTRUE) {
            int dl = 0, sz;
            va_list args;
            if (with) {
                struct tm *ctimka;
                time_t it_ct = time(NULL);
                ctimka = localtime(&it_ct);
                dl = sprintf(st, "%02d.%02d %02d:%02d:%02d ",
                                 ctimka->tm_mday,ctimka->tm_mon + 1,ctimka->tm_hour,ctimka->tm_min,ctimka->tm_sec);
            }
            if (tag) dl += sprintf(st+strlen(st), "[%s] ", tag);
            sz = dl;
            va_start(args, fmt);
            sz += vsnprintf(st + dl, len - dl, fmt, args);
            printf(st);
            va_end(args);
            xSemaphoreGive(prn_mutex);
        }
#ifdef SET_NET_LOG
        if (tcpCli >= 0) putMsg(st);
#endif
#ifdef SET_WS
        if (wsCliRdy) ws_putMsg(st);
#endif
        free(st);
    }
}
//------------------------------------------------------------------------------------------------------------
const char *wifi_auth_type(wifi_auth_mode_t m)
{

    switch (m) {
        case WIFI_AUTH_OPEN:// = 0
            return "AUTH_OPEN";
        case WIFI_AUTH_WEP:
            return "AUTH_WEP";
        case WIFI_AUTH_WPA_PSK:
            return "AUTH_WPA_PSK";
        case WIFI_AUTH_WPA2_PSK:
            return "AUTH_WPA2_PSK";
        case WIFI_AUTH_WPA_WPA2_PSK:
            return "AUTH_WPA_WPA2_PSK";
        case WIFI_AUTH_WPA2_ENTERPRISE:
            return "AUTH_WPA2_ENTERPRISE";
        default:
            return "AUTH_UNKNOWN";
    }

}
//------------------------------------------------------------------------------------------------------------
static void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{

    if (event_base == WIFI_EVENT) {
        switch (event_id) {
            case WIFI_EVENT_STA_START :
                esp_wifi_connect();
            break;
            case WIFI_EVENT_STA_DISCONNECTED :
                if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY) {
                    esp_wifi_connect();

                    xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);

//                    s_retry_num++;
                    ets_printf("[%s] retry to connect to the AP\n", TAGW);
                } else ets_printf("[%s] connect to the AP fail\n", TAGW);
            break;
            case WIFI_EVENT_STA_CONNECTED :
            {
                wifi_ap_record_t wd;
                if (!esp_wifi_sta_get_ap_info(&wd)) {
                    ets_printf("[%s] Connected to AP '%s' auth(%u):'%s' chan:%u rssi:%d\n",
                               TAGW,
                               (char *)wd.ssid,
                               (uint8_t)wd.authmode, wifi_auth_type(wd.authmode),
                               wd.primary, wd.rssi);
                }
            }
            break;
        }
    } else if (event_base == IP_EVENT) {
        switch (event_id) {
            case IP_EVENT_STA_GOT_IP :
            {
                ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
                sprintf(localip, IPSTR, IP2STR(&event->ip_info.ip));
                ip4 = event->ip_info.ip;
                bca.u_addr.ip4.addr = ip4.addr | 0xff000000;
                ets_printf("[%s] Local ip_addr : %s\n", TAGW, localip);
                s_retry_num = 0;

                xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
            }
            break;
        };
    }

}
//------------------------------------------------------------------------------------------------------------
bool check_pin(uint8_t pin_num)
{
    gpio_pad_select_gpio(pin_num);
    gpio_pad_pullup(pin_num);
    if (gpio_set_direction(pin_num, GPIO_MODE_INPUT) != ESP_OK) return true;

    return (bool)(gpio_get_level(pin_num));
}
//------------------------------------------------------------------------------------------------------------
#ifdef UDP_SEND_BCAST
#define max_line 256
void udp_task(void *par)
{
udp_start = 1;
err_t err = ERR_OK;
u16_t len, port = 8004;
struct netconn *conn = NULL;
struct netbuf *buf = NULL;
void *data = NULL;
uint32_t cnt = 1;
uint32_t tmr_sec;
char line[max_line] = {0};
bool loop = true;

    total_task++;

    ets_printf("[%s] BROADCAST task started | FreeMem %u\n", TAGU, xPortGetFreeHeapSize());

    xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, false, true, portMAX_DELAY);


    while (loop && !restart_flag) {

        if (!udp_flag) { loop = false; break; }

        conn = netconn_new(NETCONN_UDP);
        if (!conn) {
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            loop = false;
        }

        tmr_sec = get_tmr(_1s);

        while (loop) {
            if (!udp_flag) { loop = false; break; }
            if (check_tmr(tmr_sec)) {
                buf = netbuf_new();
                if (buf) {
                    len = sprintf(line,"{\"DevID\":\"%08X\",\"Time\":%u,\"FreeMem\":%u,\"Ip\":\"%s\",\"To\":\"%s\",\"Pack\":%u}\r\n",
                                        cli_id,
                                        (uint32_t)time(NULL),
                                        xPortGetFreeHeapSize(),
                                        localip, ip4addr_ntoa(&bca.u_addr.ip4), cnt);
                    data = netbuf_alloc(buf, len);
                    if (data) {
                        memset((char *)data, 0, len);
                        memcpy((char *)data, line, len);
                        err = netconn_sendto(conn, buf, (const ip_addr_t *)&bca, port);
                        if (err != ERR_OK) {
                            ESP_LOGE(TAGU,"Sending '%.*s' error=%d '%s'", len, (char *)data, (int)err, lwip_strerr(err));
                        } else {
                            print_msg(1, TAGU, "%s", line);
                            cnt++;
                        }
                    }
                    netbuf_delete(buf); buf = NULL;
                }
                tmr_sec = get_tmr(_10s);//10 sec
            } else vTaskDelay(50 / portTICK_PERIOD_MS);
            if (restart_flag) { loop = false; break; }
        }

        if (conn) { netconn_delete(conn); conn = NULL; }

    }

    udp_start = 0;
    if (total_task) total_task--;
    ets_printf("[%s] BROADCAST task stoped | FreeMem %u\n", TAGU, xPortGetFreeHeapSize());

    vTaskDelete(NULL);
}
#endif
//--------------------------------------------------------------------------------------------------
void initialize_wifi(wifi_mode_t w_mode)
{

    wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
         if (w_mode == WIFI_MODE_STA) esp_netif_create_default_wifi_sta();
    else if (w_mode == WIFI_MODE_AP) esp_netif_create_default_wifi_ap();
    else {
        ets_printf("[%s] unknown wi-fi mode - %d | FreeMem %u\n", TAGW, w_mode, xPortGetFreeHeapSize());
        return;
    }

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(w_mode));


    switch ((uint8_t)w_mode) {
        case WIFI_MODE_STA :
        {
           ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID,    &event_handler, NULL));
           ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT,   IP_EVENT_STA_GOT_IP, &event_handler, NULL));

            wifi_config_t wifi_config;
            memset((uint8_t *)&wifi_config, 0, sizeof(wifi_config_t));
            if (wifi_param_ready) {
                memcpy(wifi_config.sta.ssid, work_ssid, strlen(work_ssid));
                memcpy(wifi_config.sta.password, work_pass, strlen(work_pass));
                ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
                ets_printf("[%s] WIFI_MODE - STA: '%s':'%s'\n",
                           TAGW, wifi_config.sta.ssid, wifi_config.sta.password);
            }
            ESP_ERROR_CHECK(esp_wifi_start());
        }
        break;
        case WIFI_MODE_AP:
        {
            ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));

            wifi_config_t wifi_configap;
            memset((uint8_t *)&wifi_configap, 0, sizeof(wifi_config_t));
            wifi_configap.ap.ssid_len = strlen(sta_mac);
            memcpy(wifi_configap.ap.ssid, sta_mac, wifi_configap.ap.ssid_len);
            memcpy(wifi_configap.ap.password, work_pass, strlen(work_pass));
            wifi_configap.ap.channel = 6;
            wifi_configap.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;//WIFI_AUTH_WPA_WPA2_PSK
            wifi_configap.ap.ssid_hidden = 0;
            wifi_configap.ap.max_connection = 4;
            wifi_configap.ap.beacon_interval = 100;
            ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_configap));
            ESP_ERROR_CHECK(esp_wifi_start());
            ets_printf("[%s] WIFI_MODE - AP: '%s':'%s'\n", TAGW, wifi_configap.ap.ssid, work_pass);
        }
        break;
    }
}
//********************************************************************************************************************
esp_err_t read_param(const char *param_name, void *param_data, size_t len)//, int8_t type)
{
nvs_handle mhd;

    esp_err_t err = nvs_open(STORAGE_NAMESPACE, NVS_READONLY, &mhd);
    if (err != ESP_OK) {
        ESP_LOGE(TAGN, "%s(%s): Error open '%s'", __func__, param_name, STORAGE_NAMESPACE);
    } else {//OK
        err = nvs_get_blob(mhd, param_name, param_data, &len);
        if (err != ESP_OK) {
            ESP_LOGE(TAGN, "%s: Error read '%s' from '%s'", __func__, param_name, STORAGE_NAMESPACE);
        }
        nvs_close(mhd);
    }

    return err;
}
//--------------------------------------------------------------------------------------------------
esp_err_t save_param(const char *param_name, void *param_data, size_t len)
{
nvs_handle mhd;

    esp_err_t err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &mhd);
    if (err != ESP_OK) {
        ESP_LOGE(TAGN, "%s(%s): Error open '%s'", __func__, param_name, STORAGE_NAMESPACE);
    } else {
        err = nvs_set_blob(mhd, param_name, (uint8_t *)param_data, len);
        if (err != ESP_OK) {
            ESP_LOGE(TAGN, "%s: Error save '%s' with len %u to '%s'", __func__, param_name, len, STORAGE_NAMESPACE);
        } else err = nvs_commit(mhd);
        nvs_close(mhd);
    }

    return err;
}
//--------------------------------------------------------------------------------------------------
uint32_t get_vcc()
{
    return ((uint32_t)(adc1_get_raw(ADC1_TEST_CHANNEL) * 0.8));
}
//--------------------------------------------------------------------------------------------------
float get_tChip()
{
    return (((temprature_sens_read() - 40) - 32) * 5/9);
}

//************************************************************************************************************

void app_main()
{

    total_task = 0;

    vTaskDelay(1000 / portTICK_RATE_MS);

    esp_err_t err = nvs_flash_init();
    if (err != ESP_OK) {
        ESP_LOGE(TAGN, "1: nvs_flash_init() ERROR (0x%x) !!!", err);
        nvs_flash_erase();
        err = nvs_flash_init();
        if (err != ESP_OK) {
            ESP_LOGE(TAGN, "2: nvs_flash_init() ERROR (0x%x) !!!", err);
            while (1);
        }
    }

    macs = (uint8_t *)calloc(1, 6);
    if (macs) {
        esp_efuse_mac_get_default(macs);
        sprintf(sta_mac, MACSTR, MAC2STR(macs));
        memcpy(&cli_id, &macs[2], 4);
        cli_id = ntohl(cli_id);
    }

    vTaskDelay(500 / portTICK_RATE_MS);

    ets_printf("\nApp version %s | MAC %s | SDK Version %s | FreeMem %u\n", Version, sta_mac, esp_get_idf_version(), xPortGetFreeHeapSize());

    //--------------------------------------------------------

    esp_log_level_set("wifi", ESP_LOG_WARN);

    //--------------------------------------------------------

    gpio_pad_select_gpio(GPIO_LOG_PIN);//white LED
    gpio_pad_pullup(GPIO_LOG_PIN);
    gpio_set_direction(GPIO_LOG_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_LOG_PIN, LED_OFF);
    //

    //--------------------------------------------------------

    bool rt = check_pin(GPIO_WIFI_PIN);//pin17
    if (!rt) ets_printf("[%s] === CHECK_WIFI_REWRITE_PIN %d LEVEL IS %d ===\n", TAGT, GPIO_WIFI_PIN, rt);

    //--------------------------------------------------------

    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &periodic_timer_callback,
        /* name is optional, but may help identify the timer when debugging */
        .name = "periodic"
    };
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));


    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, tperiod));//100000 us = 100 ms
    ets_printf("[%s] Started timer with period 100 ms, time since boot: %lld/%llu\n",
                    TAGT,
                    esp_timer_get_time(),
                    get_varta() * 100);

    //--------------------------------------------------------

    //CLI_ID
    err = read_param(PARAM_CLIID_NAME, (void *)&cli_id, sizeof(uint32_t));
    if (err != ESP_OK) save_param(PARAM_CLIID_NAME, (void *)&cli_id, sizeof(uint32_t));
    ets_printf("[%s] DEVICE_ID='%08X'\n", TAGT, cli_id);

#ifdef SET_SNTP
    //SNTP + TIME_ZONE
    memset(work_sntp, 0, sntp_srv_len);
    err = read_param(PARAM_SNTP_NAME, (void *)work_sntp, sntp_srv_len);
    if (err != ESP_OK) {
        memset(work_sntp, 0, sntp_srv_len);
        memcpy(work_sntp, sntp_server_def, strlen((char *)sntp_server_def));
        save_param(PARAM_SNTP_NAME, (void *)work_sntp, sntp_srv_len);
    }
    memset(time_zone, 0, sntp_srv_len);
    err = read_param(PARAM_TZONE_NAME, (void *)time_zone, sntp_srv_len);
    if (err != ESP_OK) {
        memset(time_zone, 0, sntp_srv_len);
        memcpy(time_zone, sntp_tzone_def, strlen((char *)sntp_tzone_def));
        save_param(PARAM_TZONE_NAME, (void *)time_zone, sntp_srv_len);
    }
    ets_printf("[%s] SNTP_SERVER '%s' TIME_ZONE '%s'\n", TAGT, work_sntp, time_zone);
#endif

    //WIFI
    //   MODE
    bool rt0 = check_pin(GPIO_WMODE_PIN);//pin16
    if (!rt0) ets_printf("[%s] === CHECK_WIFI_MODE_PIN %d LEVEL IS %d ===\n", TAGT, GPIO_WMODE_PIN, rt0);


    uint8_t wtmp;
    err = read_param(PARAM_WMODE_NAME, (void *)&wtmp, sizeof(uint8_t));
    if (rt0) {//set wifi_mode from flash
        if (err == ESP_OK) {
            wmode = wtmp;
        } else {
            wmode = WIFI_MODE_AP;
            save_param(PARAM_WMODE_NAME, (void *)&wmode, sizeof(uint8_t));
        }
    } else {//set AP wifi_mode
        wmode = WIFI_MODE_STA;
        if (err == ESP_OK) {
            if (wtmp != wmode) save_param(PARAM_WMODE_NAME, (void *)&wmode, sizeof(uint8_t));
        } else {
            save_param(PARAM_WMODE_NAME, (void *)&wmode, sizeof(uint8_t));
        }
#ifdef SET_CAN_DEV
        canMode = CAN_MODE_NO_ACK;
#endif
    }
/* Set STA mode !!!
    wmode = WIFI_MODE_STA;
    save_param(PARAM_WMODE_NAME, (void *)&wmode, sizeof(uint8_t));
*/
    ets_printf("[%s] WIFI_MODE (%d): %s\n", TAGT, wmode, wmode_name[wmode]);

#ifdef UDP_SEND_BCAST
    if (wmode == WIFI_MODE_STA) udp_flag = 1;
#endif

    //   SSID
    memset(work_ssid, 0, wifi_param_len);
    err = read_param(PARAM_SSID_NAME, (void *)work_ssid, wifi_param_len);
    if ((err != ESP_OK) || (!rt)) {
        memset(work_ssid,0,wifi_param_len);
        memcpy(work_ssid, EXAMPLE_WIFI_SSID, strlen(EXAMPLE_WIFI_SSID));
        save_param(PARAM_SSID_NAME, (void *)work_ssid, wifi_param_len);
    }
    //   KEY
    memset(work_pass, 0, wifi_param_len);
    err = read_param(PARAM_KEY_NAME, (void *)work_pass, wifi_param_len);
    if ((err != ESP_OK) || (!rt)) {
        memset(work_pass,0,wifi_param_len);
        memcpy(work_pass, EXAMPLE_WIFI_PASS, strlen(EXAMPLE_WIFI_PASS));
        save_param(PARAM_KEY_NAME, (void *)work_pass, wifi_param_len);
    }
    ets_printf("[%s] WIFI_STA_PARAM: '%s:%s'\n", TAGT, work_ssid, work_pass);

    wifi_param_ready = 1;


#ifdef SET_CAN_DEV
    err = read_param(PARAM_CAN_SPEED, (void *)&canSpeed, sizeof(uint16_t));
    if ((err != ESP_OK) || (!rt)) {
        canSpeed = 125;
        save_param(PARAM_CAN_SPEED, (void *)&canSpeed, sizeof(uint16_t));
    }
    ets_printf("[%s] CAN_SPEED: %u KHz\n", TAGT, canSpeed);
#endif


#ifdef SET_WS
    err = read_param(PARAM_WS_PORT, (void *)&ws_port, sizeof(uint16_t));
    if ((err != ESP_OK) || (!rt)) {
        ws_port = WS_PORT;
        save_param(PARAM_WS_PORT, (void *)&ws_port, sizeof(uint16_t));
    }
    ets_printf("[%s] WS_PORT: %u\n", TAGT, ws_port);
#endif


//******************************************************************************************************

    adc1_config_width(ADC_WIDTH_12Bit);
    adc1_config_channel_atten(ADC1_TEST_CHANNEL, ADC_ATTEN_11db);

    prn_mutex = xSemaphoreCreateMutex();

    initialize_wifi(wmode);

//******************************************************************************************************

#ifdef SET_SSD1306
    lcd_mutex = xSemaphoreCreateMutex();

    i2c_ssd1306_init();

    ssd1306_on(false);
//    vTaskDelay(500 / portTICK_RATE_MS);

    esp_err_t ssd_ok = ssd1306_init();
    if (ssd_ok == ESP_OK) ssd1306_pattern();
    //ssd1306_invert();
    ssd1306_clear();

    scr_ini_done = true;
    //
    char stk[128] = {0};
    struct tm *dtimka;
    int tu, tn;
    time_t dit_ct;
    uint32_t adc_tw = get_tmr(0);
#endif


#ifdef SET_SNTP
    if (wmode & 1) {// WIFI_MODE_STA) || WIFI_MODE_APSTA
        if (xTaskCreatePinnedToCore(&sntp_task, "sntp_task", STACK_SIZE_2K, work_sntp, 10, NULL, 0) != pdPASS) {//5,NULL,1
            ESP_LOGE(TAGS, "Create sntp_task failed | FreeMem %u", xPortGetFreeHeapSize());
        }// else vTaskDelay(250 / portTICK_RATE_MS);
    }
#endif


#ifdef SET_NET_LOG
    msgq = xQueueCreate(8, sizeof(s_net_msg));//create msg queue

    if (xTaskCreatePinnedToCore(&net_log_task, "net_log_task", 4*STACK_SIZE_1K, &net_log_port, 6, NULL, 1) != pdPASS) {//7,NULL,1
        ESP_LOGE(TAGS, "Create net_log_task failed | FreeMem %u", xPortGetFreeHeapSize());
    }// else vTaskDelay(250 / portTICK_RATE_MS);
#endif


#ifdef SET_CAN_DEV
    // Set filter config
    static const can_filter_config_t f_config = CAN_FILTER_CONFIG_ACCEPT_ALL();

    // Set timing config
    can_timing_config_t t_config;
    selectCanSpeed(&t_config, &canSpeed);//default speed = 125KBITS
    uint32_t cspeed = clkBaseKHz / t_config.brp / (1 + t_config.tseg_1 + t_config.tseg_2);//in KHz

    // Set global config
    static can_general_config_t g_config = CAN_GENERAL_CONFIG_DEFAULT(TX_GPIO_NUM, RX_GPIO_NUM, CAN_MODE_NO_ACK);
    if (canMode != g_config.mode) g_config.mode = canMode;

    if (can_driver_install(&g_config, &t_config, &f_config) == ESP_OK) {
        can_start();
        if (xTaskCreatePinnedToCore(&can_task, "can_task", 4*STACK_SIZE_1K, &cspeed, 8, NULL, 0) != pdPASS) {
            ESP_LOGE(TAGCAN, "Create can_task failed | FreeMem %u", xPortGetFreeHeapSize());
        }
        //vTaskDelay(500 / portTICK_RATE_MS);
    } else {
        ESP_LOGI(TAGCAN, "CAN Driver install Error. | FreeMem %u", xPortGetFreeHeapSize());
    }
#endif


#ifdef SET_WS
    wsq = xQueueCreate(4, sizeof(s_ws_msg));//create ws queue

    if (xTaskCreatePinnedToCore(&ws_task, "ws_task", 4*STACK_SIZE_1K, &ws_port, 7, NULL, 0) != pdPASS) {//8//10//7
        ESP_LOGE(TAGWS, "Create ws_task failed | FreeMem %u", xPortGetFreeHeapSize());
    }
#endif




    while (!restart_flag) {//main loop

#ifdef SET_SSD1306
        if (check_tmr(adc_tw)) {
            if (ssd_ok == ESP_OK) {
                dit_ct = time(NULL);
                dtimka = localtime(&dit_ct);
                sprintf(stk,"%02d.%02d %02d:%02d:%02d\n",
                                    dtimka->tm_mday, dtimka->tm_mon + 1,
                                    dtimka->tm_hour, dtimka->tm_min, dtimka->tm_sec);
                tu = strlen(localip);
                if ((tu > 0) && (tu <= 16)) {
                    tn = (16 - tu ) / 2;
                    if ((tn > 0) && (tn < 8)) sprintf(stk+strlen(stk),"%*.s",tn," ");
                    sprintf(stk+strlen(stk),"%s", localip);
                }
                ssd1306_text_xy(stk, 2, 1);
            }
            adc_tw = get_tmr(_1s);
        }
#endif

        if (wmode == WIFI_MODE_STA) {// WIFI_MODE_STA || WIFI_MODE_APSTA
#ifdef SET_SNTP
            if (sntp_go) {
                if (!sntp_start) {
                    sntp_go = 0;
                    if (xTaskCreatePinnedToCore(&sntp_task, "sntp_task", STACK_SIZE_2K, work_sntp, 5, NULL, 0)  != pdPASS) {//5//7 core=1
                        ESP_LOGE(TAGS, "Create sntp_task failed | FreeMem %u", xPortGetFreeHeapSize());
                    }
                    vTaskDelay(500 / portTICK_RATE_MS);
                } else vTaskDelay(50 / portTICK_RATE_MS);
            }
#endif

#ifdef UDP_SEND_BCAST
            if ((!udp_start) && (udp_flag == 1)) {
                if (xTaskCreatePinnedToCore(&udp_task, "disk_task", STACK_SIZE_1K5, NULL, 5, NULL, 0) != pdPASS) {//5,NULL,1)
                    ESP_LOGE(TAGU, "Create udp_task failed | FreeMem %u", xPortGetFreeHeapSize());
                }
                vTaskDelay(500 / portTICK_RATE_MS);
            }
#endif
        }

    }//while (!restart_flag)

#ifdef SET_CAN_DEV
    can_stop();
    can_driver_uninstall();
#endif

    uint8_t cnt = 30;
    print_msg(1, TAG, "Waiting for all task closed...%d sec.\n", cnt/10);
    while (total_task) {
        cnt--; if (!cnt) break;
        vTaskDelay(100 / portTICK_RATE_MS);
    }
    print_msg(1, TAG, "DONE (%d). Total unclosed task %d\n", cnt, total_task);

    if (macs) free(macs);

    vTaskDelay(1000 / portTICK_RATE_MS);

    ESP_ERROR_CHECK(esp_wifi_stop());
    ESP_ERROR_CHECK(esp_wifi_deinit());

    esp_restart();
}
