#ifndef __TCP_SRV_H__
#define __TCP_SRV_H__

#include "hdr.h"

#ifdef SET_NET_LOG
    typedef struct {
        char *msg;
    } s_net_msg;

    extern int tcpCli;

    extern int putMsg(char *st);
    extern void net_log_task(void *arg);
#endif

#endif
