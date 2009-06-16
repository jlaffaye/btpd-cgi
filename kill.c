#include "btpdcgi.h"

void
cmd_kill(CGI *cgi)
{
	enum ipc_err code;

	btpd_connect();
	if ((code = btpd_die(ipc)) != 0)
		diemsg("command failed (%s).\n", ipc_strerror(code));
}
