#include "btpdcgi.h"

const char *btpd_dir;
struct ipc *ipc;

void
diemsg(const char *fmt, ...)
{
	va_list ap;
	printf("Content-Type: text/plain\n\n"); /* Make sure we are out of headers */
	va_start(ap, fmt);
	vfprintf(stdout, fmt, ap);
	va_end(ap);
	exit(1);
}

void
btpd_connect(void)
{
	if((errno = ipc_open(btpd_dir, &ipc)) != 0)
		diemsg("cannot open connection to btpd in %s (%s).\n", btpd_dir, strerror(errno));
}

enum ipc_err
handle_ipc_res(enum ipc_err code, const char *cmd, const char *target)
{
	switch (code) {
	case IPC_OK:
		break;
	case IPC_COMMERR:
		diemsg("error in communication with btpd.\n");
	default:
		printf("btpdcgi: %s '%s': %s.\n", cmd, target, ipc_strerror(code));
	}
	return code;
}

void
get_percent(long long part, long long whole, char **res)
{
	asprintf(res, "%.1f%%", floor(1000.0 * part / whole) / 10);
}

void
get_rate(long long rate, char **res)
{
	if (rate >= 999.995 * (1 << 10))
		asprintf(res, "%.2fMB/s", (double)rate / (1 << 20));
	else
		asprintf(res, "%.2fkB/s", (double)rate / (1 << 10));
}

void
get_size(long long size, char **res)
{
	if(size >= 999.995 * (1 << 20))
		asprintf(res, "%.2fGB", (double)size / (1 << 30));
	else
		asprintf(res, "%.2fMB", (double)size / (1 << 20));
}

void
get_ratio(long long part, long long whole, char **res)
{
	asprintf(res, "%.2f", (double)part / whole);
}

char*
tstate_str(enum ipc_tstate ts)
{
	switch (ts) {
	case IPC_TSTATE_INACTIVE:
		return "inactive";
	case IPC_TSTATE_START:
		return "start";
	case IPC_TSTATE_STOP:
		return "stop";
	case IPC_TSTATE_LEECH:
		return "leech";
	case IPC_TSTATE_SEED:
		return "seed";
	}
	return "n/a";
	diemsg("unrecognized torrent state.\n");
}

int
torrent_spec(char *arg, struct ipc_torrent *tp)
{
	char *p;
	tp->u.num = strtoul(arg, &p, 10);
	if (*p == '\0') {
		tp->by_hash = 0;
		return 1;
	}
	if ((p = mi_load(arg, NULL)) == NULL) {
		printf("btcli: bad torrent '%s' (%s).\n", arg, strerror(errno));
		return 0;
	}
	tp->by_hash = 1;
	mi_info_hash(p, tp->u.hash);
	free(p);
	return 1;
}
