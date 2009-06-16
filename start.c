#include "btpdcgi.h"

void
cmd_start(CGI *cgi)
{
	struct ipc_torrent t;
	char *torrent;

	btpd_connect();
	if (torrent_spec(torrent, &t))
		handle_ipc_res(btpd_start(ipc, &t), "start", torrent);
}
