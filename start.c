#include "btpdcgi.h"

void
cmd_start(CGI *cgi)
{
	struct ipc_torrent t;
	char *torrent;

	torrent = hdf_get_value(cgi->hdf, "Query.torrent", "None");

	btpd_connect();
	if (torrent_spec(torrent, &t))
		handle_ipc_res(btpd_start(ipc, &t), "start", torrent);

	cgi_redirect(cgi, "%s", hdf_get_value(cgi->hdf, "CGI.ScriptName", ""));
}
