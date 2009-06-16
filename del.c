#include "btpdcgi.h"

void
cmd_del(CGI *cgi)
{
	char *torrent = NULL;
	struct ipc_torrent t;

	btpd_connect();
	if (torrent_spec(torrent, &t))
		handle_ipc_res(btpd_del(ipc, &t), "del", torrent);
}
