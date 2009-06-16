#include "btpdcgi.h"

void
cmd_stop(CGI *cgi)
{
	int all = 0;
	struct ipc_torrent t;
	char *torrent;

	btpd_connect();
	if (all) {
		enum ipc_err code = btpd_stop_all(ipc);
		if (code != IPC_OK)
			diemsg("command failed (%s).\n", ipc_strerror(code));
	} else {
		if (torrent_spec(torrent, &t))
			handle_ipc_res(btpd_stop(ipc, &t), "stop", torrent);
	}
}
