#ifndef BTPDCGI_H
#define BTPDCGI_H

#include <errno.h>
#include <getopt.h>
#include <inttypes.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "ClearSilver.h"

#include "btpd_if.h"
#include "metainfo.h"
#include "subr.h"
#include "benc.h"
#include "iobuf.h"
#include "queue.h"

#define CONFFILE ETCDIR"/btpdcgi.conf"
#define TPLDIR SHAREDIR"/btpdcgi/templates"
#define TPLLIST TPLDIR"/list.cs"
#define TPLADD TPLDIR"/add.cs"

extern const char *btpd_dir;
extern struct ipc *ipc;

void diemsg(const char *fmt, ...);
void btpd_connect(void);
enum ipc_err handle_ipc_res(enum ipc_err err, const char *cmd, const char *target);
char tstate_char(enum ipc_tstate ts);
int torrent_spec(char *arg, struct ipc_torrent *tp);

void print_rate(long long rate);
void print_size(long long size);
void print_ratio(long long part, long long whole);
void print_percent(long long part, long long whole);

void cmd_add(CGI *cgi);
void cmd_del(CGI *cgi);
void cmd_list(CGI *cgi);
void cmd_stat(CGI *cgi);
void cmd_kill(CGI *cgi);
void cmd_start(CGI *cgi);
void cmd_stop(CGI *cgi);

#endif
