#include "btpdcgi.h"

/* Im bored to repeat this 2 lines... */
#define HDF_SET_BUF(name) hdf_set_value(node, name, buf); free(buf);

static void
list_cb(int obji, enum ipc_err objerr, struct ipc_get_res *res, void *arg)
{
	CGI *cgi = arg;
	HDF *node;
	char *buf;

	if (objerr != IPC_OK)
		diemsg("list_cb failed (%s).\n", ipc_strerror(objerr));

	/* Get an HDF node for the current torrent */
	asprintf(&buf, "torrents.%i", (unsigned)res[IPC_TVAL_NUM].v.num);
	hdf_get_node(cgi->hdf, buf, &node);
	free(buf);

	/* Fill it */
	hdf_set_int_value(node, "num", (unsigned)res[IPC_TVAL_NUM].v.num);
	hdf_set_int_value(node, "peers", res[IPC_TVAL_PCOUNT].v.num);
	hdf_set_value(node, "st", tstate_str(res[IPC_TVAL_STATE].v.num));

	get_size(res[IPC_TVAL_CSIZE].v.num, &buf);
	HDF_SET_BUF("size");

	get_size(res[IPC_TVAL_CGOT].v.num, &buf);
	HDF_SET_BUF("downloaded_size");

	get_percent(res[IPC_TVAL_CGOT].v.num, res[IPC_TVAL_CSIZE].v.num, &buf);
	HDF_SET_BUF("percent");

	get_rate(res[IPC_TVAL_RATEDWN].v.num, &buf);
	HDF_SET_BUF("rate_down");

	get_rate(res[IPC_TVAL_RATEUP].v.num, &buf);
	HDF_SET_BUF("rate_up");

	get_percent(res[IPC_TVAL_PCSEEN].v.num, res[IPC_TVAL_PCCOUNT].v.num, &buf);
	HDF_SET_BUF("available");

	get_ratio(res[IPC_TVAL_TOTUP].v.num, res[IPC_TVAL_CSIZE].v.num, &buf);
	HDF_SET_BUF("ratio");

	if (res[IPC_TVAL_NAME].type == IPC_TYPE_ERR)
		asprintf(&buf, "%s", ipc_strerror(res[IPC_TVAL_NAME].v.num));
	else
		asprintf(&buf, "%.*s", (int)res[IPC_TVAL_NAME].v.str.l, res[IPC_TVAL_NAME].v.str.p);
	HDF_SET_BUF("name");
}

static enum ipc_tval keys[] = {
	IPC_TVAL_NUM,
	IPC_TVAL_STATE,
	IPC_TVAL_NAME,
	IPC_TVAL_PCOUNT,
	IPC_TVAL_TRGOOD,
	IPC_TVAL_PCCOUNT,
	IPC_TVAL_PCSEEN,
	IPC_TVAL_SESSUP,
	IPC_TVAL_SESSDWN,
	IPC_TVAL_TOTUP,
	IPC_TVAL_RATEUP,
	IPC_TVAL_RATEDWN,
	IPC_TVAL_CGOT,
	IPC_TVAL_CSIZE
};

void
cmd_list(CGI *cgi)
{
	enum ipc_err code;
	size_t nkeys = sizeof(keys) / sizeof(keys[0]);
	NEOERR *err;

	btpd_connect();
	code = btpd_tget_wc(ipc, IPC_TWC_ALL, keys, nkeys, list_cb, cgi);
	if (code != IPC_OK)
		diemsg("command failed (%s).\n", ipc_strerror(code));

	/* Ensure that the template now that the cmd is "list" */
	hdf_set_value(cgi->hdf, "Query.cmd", "list");

	if((err = cgi_display(cgi, "list.cs"))) {
		cgi_neo_error(cgi, err);
	}
}
