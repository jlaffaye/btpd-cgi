#include "btpdcgi.h"

static struct {
	const char *name;
	void (*fun)(CGI *);
} cmd_table[] = {
	{ "add", cmd_add },
	{ "del", cmd_del },
	{ "kill", cmd_kill },
	{ "list", cmd_list },
	{ "start", cmd_start },
	{ "stop", cmd_stop },
	{ "stat", cmd_stat }
};

int ncmds = sizeof(cmd_table) / sizeof(cmd_table[0]);

int
main()
{
	char *cmd;
	CGI *cgi;

	cgi_init(&cgi, NULL);
	cmd = hdf_get_value(cgi->hdf, "Query.cmd", "list");

	if(btpd_dir == NULL)
		if((btpd_dir = find_btpd_dir()) == NULL)
			diemsg("cannot find the btpd directory.\n");

	for(int i = 0 ; i < ncmds ; i++)
		if(strcmp(cmd_table[i].name, cmd) == 0)
			cmd_table[i].fun(cgi);

	cgi_destroy(&cgi);
	return 0;
}
