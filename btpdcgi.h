#ifndef BTPDCGI_H
#define BTPDCGI_H

#include <sys/param.h>
#include <errno.h>
#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fetch.h>
#include <unistd.h>

#include "ClearSilver.h"

#include "btpd_if.h"
#include "metainfo.h"
#include "subr.h"
#include "benc.h"
#include "iobuf.h"

#define CONFFILE ETCDIR"/btpdcgi.conf"
#define TPLDIR SHAREDIR"/btpdcgi/templates"

extern const char *btpd_dir;
extern struct ipc *ipc;

void diemsg(const char *fmt, ...);
void btpd_connect(void);
enum ipc_err handle_ipc_res(enum ipc_err err, const char *cmd, const char *target);
char* tstate_str(enum ipc_tstate ts);
int torrent_spec(char *arg, struct ipc_torrent *tp);

void get_rate(long long rate, char **res);
void get_size(long long size, char **res);
void get_ratio(long long part, long long whole, char **res);
void get_percent(long long part, long long whole, char **res);

void cmd_add(CGI *cgi);
void cmd_del(CGI *cgi);
void cmd_list(CGI *cgi);
void cmd_kill(CGI *cgi);
void cmd_start(CGI *cgi);
void cmd_stop(CGI *cgi);

#endif
