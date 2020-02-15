#include "hdr.h"

#ifdef SET_WS

#include "main.h"

//#include "includes.h"


//------------------------------------------------------------------------------------------------------------

const char *TAGWS = "WS";
uint8_t ws_start = 0;

static bool WS_conn = false;

const char WS_sec_WS_keys[] = "Sec-WebSocket-Key:";
const char WS_sec_conKey[] = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
const char WS_srv_hs[] ="HTTP/1.1 101 Switching Protocols \r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: %.*s\r\n\r\n";


//------------------------------------------------------------------------------------------------------------


static const unsigned char base64_table[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/**
 * base64_encode - Base64 encode
 * @src: Data to be encoded
 * @len: Length of the data to be encoded
 * @out_len: Pointer to output length variable, or %NULL if not used
 * Returns: Allocated buffer of out_len bytes of encoded data,
 * or %NULL on failure
 *
 * Caller is responsible for freeing the returned buffer. Returned buffer is
 * nul terminated to make it easier to use as a C string. The nul terminator is
 * not included in out_len.
 */
unsigned char *base64_encode(const unsigned char *src, size_t len, size_t *out_len)
{
unsigned char *out, *pos;
const unsigned char *end, *in;
size_t olen;
int line_len;

	olen = len * 4 / 3 + 4; /* 3-byte blocks to 4-byte */
	olen += olen / 72; /* line feeds */
	olen++; /* nul termination */
	if (olen < len)
		return NULL; /* integer overflow */
	out = (unsigned char *)calloc(1, olen);
	if (out == NULL)
		return NULL;

	end = src + len;
	in = src;
	pos = out;
	line_len = 0;
	while (end - in >= 3) {
		*pos++ = base64_table[in[0] >> 2];
		*pos++ = base64_table[((in[0] & 0x03) << 4) | (in[1] >> 4)];
		*pos++ = base64_table[((in[1] & 0x0f) << 2) | (in[2] >> 6)];
		*pos++ = base64_table[in[2] & 0x3f];
		in += 3;
		line_len += 4;
		if (line_len >= 72) {
			*pos++ = '\n';
			line_len = 0;
		}
	}

	if (end - in) {
		*pos++ = base64_table[in[0] >> 2];
		if (end - in == 1) {
			*pos++ = base64_table[(in[0] & 0x03) << 4];
			*pos++ = '=';
		} else {
			*pos++ = base64_table[((in[0] & 0x03) << 4) |
					      (in[1] >> 4)];
			*pos++ = base64_table[(in[1] & 0x0f) << 2];
		}
		*pos++ = '=';
		line_len += 4;
	}

	if (line_len)
		*pos++ = '\n';

	*pos = '\0';
	if (out_len)
		*out_len = pos - out;
	return out;
}


/**
 * base64_decode - Base64 decode
 * @src: Data to be decoded
 * @len: Length of the data to be decoded
 * @out_len: Pointer to output length variable
 * Returns: Allocated buffer of out_len bytes of decoded data,
 * or %NULL on failure
 *
 * Caller is responsible for freeing the returned buffer.
 */
unsigned char *base64_decode(const unsigned char *src, size_t len,
			      size_t *out_len)
{
	unsigned char dtable[256], *out, *pos, block[4], tmp;
	size_t i, count, olen;
	int pad = 0;

	memset(dtable, 0x80, 256);
	for (i = 0; i < sizeof(base64_table) - 1; i++)
		dtable[base64_table[i]] = (unsigned char) i;
	dtable['='] = 0;

	count = 0;
	for (i = 0; i < len; i++) {
		if (dtable[src[i]] != 0x80)
			count++;
	}

	if (count == 0 || count % 4)
		return NULL;

	olen = count / 4 * 3;
	pos = out = (unsigned char *)calloc(1, olen);
	if (out == NULL)
		return NULL;

	count = 0;
	for (i = 0; i < len; i++) {
		tmp = dtable[src[i]];
		if (tmp == 0x80)
			continue;

		if (src[i] == '=')
			pad++;
		block[count] = tmp;
		count++;
		if (count == 4) {
			*pos++ = (block[0] << 2) | (block[1] >> 4);
			*pos++ = (block[1] << 4) | (block[2] >> 2);
			*pos++ = (block[2] << 6) | block[3];
			count = 0;
			if (pad) {
				if (pad == 1)
					pos--;
				else if (pad == 2)
					pos -= 2;
				else {
					/* Invalid padding */
					free(out);
					return NULL;
				}
				break;
			}
		}
	}

	*out_len = pos - out;
	return out;
}



//------------------------------------------------------------------------------------------------------------
int ws_get_socket_error_code(int socket)
{
int ret = 0;
socklen_t optlen = sizeof(int);

    if (getsockopt(socket, SOL_SOCKET, SO_ERROR, &ret, &optlen) == -1) {
        print_msg(1, TAGWS, "getsockopt for socket %d failed\n", socket);
        ret = -1;
    }

    return ret;
}
//------------------------------------------------------------------------------------------------------------
void ws_show_socket_error_reason(int socket)
{
int err = ws_get_socket_error_code(socket);

    print_msg(1, TAGWS, "Socket %d error %d %s\n", socket, err, strerror(err));

}
//------------------------------------------------------------------------------------------------------------
int ws_create_tcp_server(u16_t prt)
{
int soc = -1, err = 0;

    soc = socket(AF_INET, SOCK_STREAM, 0);
    if (soc >= 0) {
        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(prt);
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        if (!bind(soc, (struct sockaddr*)&server_addr, sizeof(server_addr))) {
            if (listen(soc, 1)) err = 1;//(soc , 5)
        } else err = 1;

        if (err) {
            ws_show_socket_error_reason(soc);
            close(soc); soc = -1;
        }
    }

    return soc;
}
//------------------------------------------------------------------------------------------------------------
err_t WS_write_data(int sc, char *p_data, size_t length)
{
    if (!WS_conn) return ERR_CONN;

    if (length > MAX_FRAME_LEN) return ERR_VAL;

    size_t ofs = 0, tot = 0, len;
    err_t result = ERR_VAL;

    WS_frame_header_t hdr;
    hdr.opcode = WS_OP_TXT; //4bit
    hdr.reserved = 0;       //3bit
    hdr.FIN = 1;            //1bit
    hdr.mask = 0;           //1bit

    unsigned short dl = length;
    dl = htons(dl);

    if (length >= WS_STD_LEN2) {
        hdr.payload_length = WS_STD_LEN2;
        ofs = sizeof(unsigned short);
    } else hdr.payload_length = length;

    len = length + sizeof(WS_frame_header_t) + ofs;
    char *buf = (char *)calloc(1, len);
    if (buf) {
        memcpy(buf + tot, &hdr, sizeof(WS_frame_header_t));
        tot += sizeof(WS_frame_header_t);
        if (ofs) {
            memcpy(buf + tot, &dl, ofs);
            tot += ofs;
        }
        memcpy(buf + tot, p_data, length);
        if (send(sc, buf, len, 0) == len) result = ERR_OK; else result = ERR_VAL;

#ifdef WS_PRN
        buf_prn_hex(buf, len);

        print_msg(0, NULL, "  %s: hdr: 0x%02X 0x%02X ", __func__, *(unsigned char *)(buf), *(unsigned char *)(buf+1));
        if (ofs) print_msg(0, NULL, "0x%02X 0x%02X ", *(unsigned char *)(buf+2), *(unsigned char *)(buf+3));
        print_msg(0, NULL, "data: %s\n", p_data);
#endif
        free(buf);
    }

    return result;
}
//------------------------------------------------------------------------------------------------------------
void do_ack(int skt, char *ibuf)
{

    char *stx = (char *)calloc(1, strlen(ibuf) + 64);
    if (stx) {
        print_msg(1, TAGWS, "Recv. data (%d bytes) from ws_client:%s\n", strlen(ibuf), ibuf);
        free(stx);
    }

}
//------------------------------------------------------------------------------------------------------------
void buf_prn_hex(char *bf, int len, bool fl)
{
    if ((len <= 0) || !bf) return;

    char *st = (char *)calloc(1, ((len*4) + 64));
    if (st) {
        sprintf(st, "buf(%d): ", len);
        for (int i = 0; i < len; i++) sprintf(st+strlen(st),"%02X ", *(unsigned char *)(bf + i));
        print_msg(1, TAGWS, "%s\n", st);
        free(st);
    }
    if (fl) {
        if (len >= sizeof(WS_frame_header_t)) {
            st = (char *)calloc(1, 128);
            if (st) {
                WS_frame_header_t *hdr = (WS_frame_header_t *)bf;
                sprintf(st, "hdr(%d): FIN:1=%d reserved:3=%d opcode:%d=%d , mask:1=%d len:7=%d\n",
                            sizeof(WS_frame_header_t),
                            hdr->FIN, hdr->reserved, WS_MASK_L, hdr->opcode,
                            hdr->mask, hdr->payload_length);
                print_msg(1, TAGWS, st);
                free(st);
            }
        }
    }
}
//------------------------------------------------------------------------------------------------------------
static void ws_server_netconn_serv(int sc)
{
char *p_buf = NULL;
uint16_t i;
char *p_payload = NULL;
WS_frame_header_t *p_frame_hdr;
int len = 0, total = 0;
TickType_t tcikl = 0;


    char *p_SHA1_Inp    = heap_caps_malloc(WS_CLIENT_KEY_L + sizeof(WS_sec_conKey), MALLOC_CAP_8BIT);//allocate memory for SHA1 input
    char *p_SHA1_result = heap_caps_malloc(SHA1_RES_L, MALLOC_CAP_8BIT);//allocate memory for SHA1 result
    char *buf = (char *)calloc(1, BUF_SIZE);

    if (p_SHA1_Inp && p_SHA1_result && buf) {
        //receive handshake request
        tcikl = get_tmr(WAIT_DATA_WS*_1s);
        while ( total < sizeof(WS_sec_conKey) ) {
            if (restart_flag) goto done;
            len = recv(sc, buf + total, BUF_SIZE - total - 1, 0);
            if (len > 0) {
                if (!total) tcikl = get_tmr(WAIT_DATA_WS);
                total += len;
            }
            if (total) {
                if (check_tmr(tcikl)) break;
            }
        }
#ifdef WS_PRN
        print_msg(1, TAGWS, "Receive handshake request:\n");
        print_msg(0, TAGWS, buf);
#endif
        //write static key into SHA1 Input
        for (i = 0; i < sizeof(WS_sec_conKey); i++) p_SHA1_Inp[i + WS_CLIENT_KEY_L] = WS_sec_conKey[i];
        p_buf = strstr(buf, WS_sec_WS_keys);//find Client Sec-WebSocket-Key:
        if (p_buf) {
            for (i = 0; i < WS_CLIENT_KEY_L; i++) p_SHA1_Inp[i] = *(p_buf + sizeof(WS_sec_WS_keys) + i);//get Client Key
            esp_sha(SHA1, (unsigned char *)p_SHA1_Inp, strlen(p_SHA1_Inp), (unsigned char *)p_SHA1_result);// calculate hash
            p_buf = (char*)base64_encode((unsigned char *)p_SHA1_result, SHA1_RES_L, (size_t *)&i);//hex to base64
            p_payload = heap_caps_malloc(sizeof(WS_srv_hs) + i - WS_SPRINTF_ARG_L, MALLOC_CAP_8BIT);//allocate memory for handshake
            if (p_payload) {
                sprintf(p_payload, WS_srv_hs, i - 1, p_buf);//prepare handshake
                send(sc, p_payload, strlen(p_payload), 0);//send handshake
#ifdef WS_PRN
                print_msg(1, TAGWS, "Send handshake:\n");
                print_msg(0, TAGWS, p_payload);
#endif
                heap_caps_free(p_payload); p_payload=NULL;
                WS_conn = true;
                while (!restart_flag) {
                    total = len = 0;
                    memset(buf,0,BUF_SIZE);
                    tcikl = get_tmr(WAIT_DATA_WS);
                    while (total < sizeof(WS_frame_header_t)) {
                        if (restart_flag) goto done;
                        len = recv(sc, buf + total, BUF_SIZE - total - 1, 0);
                        if (len > 0) {
                            if (!total) tcikl = get_tmr(WAIT_DATA_WS);
                            total += len;
                        }
                        if (total) {
                            if (check_tmr(tcikl)) break;
                        }
                    }
#ifdef WS_PRN
                    buf_prn_hex(buf, sizeof(WS_frame_header_t), true);
#endif
                    p_frame_hdr = (WS_frame_header_t *)buf;
                    if (p_frame_hdr->opcode == WS_OP_CLS) break;//check if clients wants to close the connection
                    if (p_frame_hdr->payload_length <= WS_STD_LEN) {
                        p_buf = (char *)&buf[sizeof(WS_frame_header_t)];
                        if (p_frame_hdr->mask) {
                            p_payload = heap_caps_malloc(p_frame_hdr->payload_length + 1, MALLOC_CAP_8BIT);
                            if (p_payload) {
                                //decode playload
                                for (i = 0; i < p_frame_hdr->payload_length; i++) p_payload[i] = (p_buf + WS_MASK_L)[i] ^ p_buf[i % WS_MASK_L];
                                p_payload[p_frame_hdr->payload_length] = 0;
                            }
                        } else p_payload = p_buf;//content is not masked
                        if ((p_payload) && (p_frame_hdr->opcode == WS_OP_TXT)) do_ack(sc, p_payload);
                        if ((p_payload) && (p_payload != p_buf)) {
                            heap_caps_free(p_payload);
                            p_payload = NULL;
                        }
                    }
                    vTaskDelay(10 / portTICK_RATE_MS);
                }
            }
        }
    }

done:

    WS_conn = false;
    if (p_payload) heap_caps_free(p_payload);
    if (buf) free(buf);
    if (p_SHA1_Inp) heap_caps_free(p_SHA1_Inp);
    if (p_SHA1_result) heap_caps_free(p_SHA1_result);

}
//------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------
void ws_task(void *arg)
{
ws_start = 1;
total_task++;

int srv = -1, cli = -1;
struct sockaddr_in client_addr;
unsigned int socklen = sizeof(client_addr);


    uint16_t wp = *(u16_t *)arg;

    ets_printf("[%s] Start WebSocket task (port=%u)| FreeMem %u\n", TAGWS, wp, xPortGetFreeHeapSize());

    xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, false, true, portMAX_DELAY);

    srv = ws_create_tcp_server(wp);
    if (srv >= 0) {
        fcntl(srv, F_SETFL, (fcntl(srv, F_GETFL, 0)) | O_NONBLOCK);
        if (!setDateTimeOK) ets_printf("[%s] Wait new web_socket client... | FreeMem %u\n", TAGWS, xPortGetFreeHeapSize());
                       else print_msg(1, TAGWS, "Wait new web_socket client... | FreeMem %u\n", xPortGetFreeHeapSize());
        while (!restart_flag) {
            cli = accept(srv, (struct sockaddr*)&client_addr, &socklen);
            if (cli > 0) {
                print_msg(1, TAGWS, "ws_client %s:%u (soc=%u) online | FreeMem %u\n",
                                (char *)inet_ntoa(client_addr.sin_addr),
                                htons(client_addr.sin_port),
                                cli,
                                xPortGetFreeHeapSize());
                fcntl(cli, F_SETFL, (fcntl(cli, F_GETFL, 0)) | O_NONBLOCK);
                ws_server_netconn_serv(cli);
                print_msg(1, TAGWS, "Closed connection (%s:%u soc=%u) | FreeMem %u\n",
                                (char *)inet_ntoa(client_addr.sin_addr),
                                htons(client_addr.sin_port),
                                cli,
                                xPortGetFreeHeapSize());
                close(cli);
                cli = -1;
            }
        }
    } else {
        print_msg(1, TAGWS, "ERROR create_tcp_server(%u)=%d\n", wp, srv);
    }


    print_msg(1, TAGWS, "WebSocket task stop | FreeMem %u\n", xPortGetFreeHeapSize());

    if (total_task) total_task--;
    ws_start = 0;

    vTaskDelete(NULL);
}
//------------------------------------------------------------------------------------------------------------
#endif
