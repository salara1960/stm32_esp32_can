#include "js.h"

#define cmd_name_all 9
const char *CtlName[] = {
    "auth",     //0//{"auth":"hash"} - hash md5_from_key_word
    "udp",      //1//{"udp":"on"} , {"udp":"off"}
    "sntp",     //2//{"sntp":"on"}
    "sntp_srv", //3//{"sntp_srv":"ip_ntp_server"}
    "time_zone",//4//{"time_zone":"UTC+02:00"}
    "restart",  //5//{"restart":"on"}
    "time",     //6//{"time":"1579692184"} , {"time":1579692184}
    "can_speed",//7//{"can_speed":250}, {"can_speed":"250"}
    "get"       //8//{"get":"status"},{"get":"wifi"},{"get":"sntp_srv"},{"get":"time_zone"},
                   //{"get":"ftp_srv"},{"get":"ftp_user"},{"get":"log_port"},{"get":"version"},
                   //{"gps_srv":"192.168.0.201:9090"},//{"gps_info":{...}}
};

const char *isNone  = "???";
const char *isOn  = "on";
const char *isOff = "off";


#define sub_cmd_name_all 7
const char *SubCtlName[] = {
    "status",   //0
    "wifi",     //1//{"wifi":"ssid:password"}
    "sntp_srv", //2//{"sntp_srv":"2.ru.pool.ntp.org"}
    "time_zone",//3//{"time_zone":"EET-2"}
    "can_speed",//4//{"can_speed":"250"}
    "log_port", //5//{"log_port":"8008"}
    "version"   //6//{"version":"4.2 (22.01.2020)"}
};
//------------------------------------------------------------------------------------------
char *get_json_str(cJSON *tp)
{
    if (tp->type != cJSON_String) return NULL;

    char *st = cJSON_Print(tp);
    if (st) {
        if (*st == '"') {
            int len = strlen(st);
            memcpy(st, st + 1, len - 1);
            *(st + len - 2) = '\0';
        }
    }

    return st;
}
//------------------------------------------------------------------------------------------
int parser_json_str(const char *st, bool *au, const char *str_md5, uint8_t *rst)
{
int yes = -1, subc = 0;
uint8_t done = 0, ind_c = 255, rcpu = 0;
int k, i, val_bin = -1;


    cJSON *obj = cJSON_Parse(st);
    if (obj) {
        cJSON *tmp = NULL;
        char *val = NULL;
        for (i = 0; i < cmd_name_all; i++) {
            tmp = cJSON_GetObjectItem(obj, CtlName[i]);
            if (tmp) {
                ind_c = i;
                switch (tmp->type) {
                    case cJSON_Number:
                        val_bin = tmp->valueint;
                    break;
                    case cJSON_String:
                        if (val) { free(val); val = NULL; }
                        val = get_json_str(tmp);
                    break;
                        default : break;
                }
                done = 0;
                if ((val) || (val_bin != -1)) {
                    switch (ind_c) {
                        case iCTRL_AUTH://0://auth
                            if (val) {
                                done = 1;
                                if ((au) && (str_md5)) {
                                    if (!strcmp(val, str_md5)) {
                                        yes = 0;
                                        *au = done;
                                    }
                                }
                            }
                        break;
#ifdef UDP_SEND_BCAST
                        case iCTRL_UDP://1://udp
                            if (val) {
                                if (au) {
                                    if (*au) {
                                        if (!strcmp(val, isOn)) {
                                            udp_flag = 1;
                                            yes = 0;
                                        } else if (!strcmp(val, isOff)) {
                                            udp_flag = 0;
                                            yes = 0;
                                        }
                                    }
                                }
                                done = 1;
                            }
                        break;
#endif
#ifdef SET_SNTP
                        case iCTRL_SNTP://2://sntp
                            if (val) {
                                if (au) {
                                    if (*au) {
                                        if (!strcmp(val, isOn)) {
                                            sntp_go = 1;
                                            yes = 0;
                                        }
                                    }
                                }
                                done = 1;
                            }
                        break;
                        case iCTRL_SNTP_SRV://3://sntp_srv
                            if (val) {
                                if (au) {
                                    if (*au) {
                                        k = strlen(val); if (k > sntp_srv_len - 1) k = sntp_srv_len - 1;
                                        memset(work_sntp, 0, sntp_srv_len);
                                        strncpy(work_sntp, val, k);
                                        save_param(PARAM_SNTP_NAME, (void *)work_sntp, sntp_srv_len);
                                        sntp_go = 1;
                                        yes = 0;
                                    }
                                }
                                done = 1;
                            }
                        break;
                        case iCTRL_TIME_ZONE://4://time_zone
                            if (val) {
                                if (au) {
                                    if (*au) {
                                        if (strcmp(val, time_zone)) {
                                            k = strlen(val);
                                            if ((k < sntp_srv_len) && (k > 0)) {
                                                memset(time_zone, 0, sntp_srv_len);
                                                strncpy(time_zone, val, k);
                                                save_param(PARAM_TZONE_NAME, (void *)time_zone, sntp_srv_len);
                                                sntp_go = 1;
                                            }
                                        }
                                        yes = 0;
                                    }
                                }
                                done = 1;
                            }
                        break;
#endif
                        case iCTRL_RESTART://5://restart
                            if (val) {
                                if (au) {
                                    if (*au) {
                                        if (!strcmp(val, isOn)) {
                                            rcpu = 1;
                                            yes = 0;
                                        }
                                    }
                                }
                                done = 1;
                                if (rst) *rst = rcpu;
                            }
                        break;
                        case iCTRL_TIME://6://time
                            if (au) {
                                if (*au) {
                                    k = -1;
                                    if (val) k = atoi(val); else if (val_bin != -1) k = val_bin;
                                    if (k > 0) {
                                        SNTP_SET_SYSTEM_TIME_US( (time_t)k, 0 );
                                        yes = 0;
                                    }
                                }
                            }
                            done = 1;
                        break;
#ifdef SET_CAN_DEV
                        case iCTRL_CAN_SPEED://7://{"can_speed":250}, {"can_speed":"250"}
                            if (au) {
                                if (*au) {
                                    k = -1;
                                    if (val) k = atoi(val); else if (val_bin != -1) k = val_bin;
                                    if (k > 0) {
                                        canSpeed = (uint16_t)k;
                                        save_param(PARAM_CAN_SPEED, (void *)&canSpeed, sizeof(uint16_t));
                                        yes = 0;
                                    }
                                }
                            }
                            done = 1;
                        break;
#endif
                        case iCTRL_GET://10://{"get":"status"}
                            if (val) {
                                if (au) {
                                    if (*au) {
                                        for (k = 0; k < sub_cmd_name_all; k++) {
                                            if (!strcmp(val, SubCtlName[k])) {
                                                subc = k;
                                                yes = 0;
                                                break;
                                            }
                                        }
                                    }
                                }
                                done = 1;
                            }
                        break;

                    }//switch (ind_c)
                }//if ((val) || (val_bin != -1))
                if (val) free(val);
                val = NULL;
                val_bin = -1;
                if (done) break;
            }//if (tmp)
        }//for
        cJSON_Delete(obj);
    }//if (obj)

    if (yes != -1) { yes = ind_c; yes |= (subc << 16); }

    return yes;
}
//------------------------------------------------------------------------------------------------
int mkAck(char *tbuf, int ictrl, int sctrl, bool *au)
{
int len = 0;
char ts[64];

    if (!tbuf || (ictrl == -1)) return len;


    if (*au == false) {
        len = sprintf(tbuf, "{\"status\":\"You are NOT auth. client, bye\"}");
    } else {
        if (ictrl != iCTRL_GET) {
            len = sprintf(tbuf, "{\"DevID\":\"%08X\",\"Time\":%u,\"FreeMem\":%u}",
                            cli_id, (uint32_t)time(NULL), xPortGetFreeHeapSize());
        } else {
            sprintf(tbuf, "{\"DevID\":\"%08X\",\"Time\":%u,\"FreeMem\":%u",
                            cli_id, (uint32_t)time(NULL), xPortGetFreeHeapSize());
            switch (sctrl) {
                //case sCTRL_STATUS://"status",
                case sCTRL_WIFI://1//"wifi",     //{"wifi":"ssid:password"}
                    memset(ts, 0, sizeof(ts));
                    if (read_param(PARAM_SSID_NAME, (void *)ts, wifi_param_len) != ESP_OK) strcpy(ts, isNone);
                    sprintf(tbuf+strlen(tbuf), ",\"%s\":\"%s", SubCtlName[sctrl], ts);
                    memset(ts, 0, sizeof(ts));
                    if (read_param(PARAM_KEY_NAME, (void *)ts, wifi_param_len) != ESP_OK) strcpy(ts, isNone);
                    sprintf(tbuf+strlen(tbuf), ":%s\"", ts);
                break;
#ifdef SET_SNTP
                case sCTRL_SNTP_SRV://2//"sntp_srv", //{"sntp_srv":"2.ru.pool.ntp.org"}
                    memset(ts, 0, sizeof(ts));
                    if (read_param(PARAM_SNTP_NAME, (void *)ts, sntp_srv_len) != ESP_OK) strcpy(ts, isNone);
                    sprintf(tbuf+strlen(tbuf), ",\"%s\":\"%s\"", SubCtlName[sctrl], ts);
                break;
                case sCTRL_TIME_ZONE://3//"time_zone",//{"time_zone":"UTC+02:00"}
                    if (read_param(PARAM_TZONE_NAME, (void *)ts, sntp_srv_len) != ESP_OK) strcpy(ts, isNone);
                    sprintf(tbuf+strlen(tbuf), ",\"%s\":\"%s\"", SubCtlName[sctrl], ts);
                break;
#endif
#ifdef SET_CAN_DEV
                case sCTRL_CAN_SPEED://4//"can_speed", //{"can_speed":"250"}
                    sprintf(tbuf+strlen(tbuf), ",\"%s\":%u", SubCtlName[sctrl], canSpeed);
                break;
#endif
#ifdef SET_NET_LOG
                case sCTRL_LOG_PORT://5//"log_port", //{"log_port":"8008"}
                    sprintf(tbuf+strlen(tbuf), ",\"%s\":%u", SubCtlName[sctrl], net_log_port);
                break;
#endif
                case sCTRL_VERSION://6//"version"   //{"version":"4.2 (22.01.2020)"}
                    sprintf(tbuf+strlen(tbuf), ",\"%s\":\"%s\"", SubCtlName[sctrl], Version);
                break;
            }
            strcat(tbuf, "}");
            len = strlen(tbuf);
        }
    }

    return len;
}
//------------------------------------------------------------------------------------------------------------

