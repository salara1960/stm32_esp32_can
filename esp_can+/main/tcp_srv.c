#include "hdr.h"

#ifdef SET_NET_LOG

#include "main.h"


//------------------------------------------------------------------------------------------------------------

int tcpCli = -1;
static const char *TAGLOG = "LOG";
static char log_cli_ip_addr[32] = {0};
//------------------------------------------------------------------------------------------------------------
int get_socket_error_code(int socket)
{
int res;
socklen_t optlen = sizeof(int);

    if (getsockopt(socket, SOL_SOCKET, SO_ERROR, &res, &optlen) == -1) {
        ESP_LOGE(TAGLOG, "getsockopt for socket %d failed", socket);
        res = -1;
    }
    return res;
}
//------------------------------------------------------------------------------------------------------------
void show_socket_error_reason(int socket)
{
int err = get_socket_error_code(socket);

    print_msg(1, TAGLOG, "Socket %d error %d '%s'\n", socket, err, strerror(err));
}
//------------------------------------------------------------------------------------------------------------
void net_log_close(int *cli)
{
    if (*cli < 0) return;

    shutdown(*cli, 0);
    close(*cli);
    *cli = -1;
}
//------------------------------------------------------------------------------------------------------------
int create_tcp_server(u16_t prt)
{
int soc = -1, err = 0;

    soc = socket(AF_INET, SOCK_STREAM, 0);
    if (soc >= 0) {
        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(prt);
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        if (!bind(soc, (struct sockaddr*)&server_addr, sizeof(server_addr))) {
            if (listen(soc, 1)) err = 1;
        } else err = 1;

        if (err) {
            show_socket_error_reason(soc);
            net_log_close(&soc);
        }
    }

    return soc;
}
//------------------------------------------------------------------------------------------------------------
int sendMsg(int *cli)
{
s_net_msg evt;
int ret = -1, s = *cli;

    if (s < 0) return ret;

    if (xQueueReceive(msgq, &evt, (TickType_t)0) == pdTRUE) {//data for printing present !
        if (evt.msg != NULL) {
            int len = strlen(evt.msg);
            if (len) {
                ret = send(s, evt.msg, len, 0);
                if (ret < 0) {
                    show_socket_error_reason(s);
                    net_log_close(&s);
                    *cli = s;
                }
            }
            free(evt.msg);
        }
    } else {// no data for printing
        char sym;
        if (!recv(s, &sym, 1, O_NONBLOCK)) {//client offline check
            ret = -1;
            show_socket_error_reason(s);
            net_log_close(&s);
            *cli = s;
        } else ret = 0;
    }

    return ret;
}
//------------------------------------------------------------------------------------------------------------
int putMsg(char *st)
{
int ret = -1;

    if (!st) return ret;

    int len = strlen(st); if (!len) return ret;

    s_net_msg evt;
    evt.msg = (char *)calloc(1, len + 1);
    if (evt.msg) {
        memcpy(evt.msg, st, len);
        if (xQueueSend(msgq, (void *)&evt, (TickType_t)0) != pdPASS) {
            ESP_LOGE(TAGLOG, "Error while sending to msg queue");
            free(evt.msg);
        } else ret = 0;
    }

    return ret;
}
//------------------------------------------------------------------------------------------------------------
void net_log_task(void *arg)
{
total_task++;

int srv = -1, res = 0, dl = 0;
struct sockaddr_in client_addr;
unsigned int socklen = sizeof(client_addr);
char stx[64];

    uint16_t tp = *(uint16_t *)arg;

    ets_printf("[%s] Start NetLogServer task (port=%u)| FreeMem %u\n", TAGLOG, tp, xPortGetFreeHeapSize());

    if (wmode == WIFI_MODE_STA) xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, false, true, portMAX_DELAY);


    srv = create_tcp_server(tp);
    if (srv >= 0) {
        fcntl(srv, F_SETFL, (fcntl(srv, F_GETFL, 0)) | O_NONBLOCK);
        gpio_set_level(GPIO_LOG_PIN, LED_ON);//0 //led ON
        ets_printf("[%s] Wait new log_client... | FreeMem %u\n", TAGLOG, xPortGetFreeHeapSize());
        while (!restart_flag) {
            tcpCli = accept(srv, (struct sockaddr*)&client_addr, &socklen);
            if (tcpCli >= 0) {
                fcntl(tcpCli, F_SETFL, (fcntl(tcpCli, F_GETFL, 0)) | O_NONBLOCK);
                strcpy(log_cli_ip_addr, (char *)inet_ntoa(client_addr.sin_addr));
                //gpio_set_level(GPIO_LOG_PIN, LED_ON);//0 //led ON
                print_msg(1, TAGLOG, "New log_client %s:%u (soc=%u) online | FreeMem %u\n",
                           log_cli_ip_addr,
                           htons(client_addr.sin_port),
                           tcpCli,
                           xPortGetFreeHeapSize());
#ifdef SET_SSD1306
                ssd1306_clear_lines(7, 2);
                dl = sprintf(stx, "LOG client adr:");
                ssd1306_text_xy(stx, ssd1306_calcx(dl), 7);
                dl = strlen(log_cli_ip_addr);
                ssd1306_text_xy(log_cli_ip_addr, ssd1306_calcx(dl), 8);
#endif
                while (res >= 0) {
                    res = sendMsg(&tcpCli);
                    vTaskDelay(100 / portTICK_PERIOD_MS);
                }
                res = 0;
                //gpio_set_level(GPIO_LOG_PIN, LED_OFF);//1 //led OFF

                print_msg(1, TAGLOG, "Closed connection. Wait new tcp client... | FreeMem %u\n", xPortGetFreeHeapSize());
                memset(log_cli_ip_addr, 0, sizeof(log_cli_ip_addr));
#ifdef SET_SSD1306
                ssd1306_clear_lines(7, 2);
#endif
            }
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    } else ESP_LOGE(TAGLOG, "ERROR create tcp server(%u)=%d", tp, srv);


    net_log_close(&tcpCli);
    net_log_close(&srv);

    ets_printf("[%s] NetLogServer task stop | FreeMem %u%s\n", TAGLOG, xPortGetFreeHeapSize());

    if (total_task) total_task--;

    vTaskDelete(NULL);
}
//------------------------------------------------------------------------------------------------------------

#endif
