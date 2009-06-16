#include "btpdcgi.h"

struct item {
	unsigned num;
	char *name;
	char st;
	long long cgot, csize, totup;
	BTPDQ_ENTRY(item) entry;
};

struct items {
	int count;
	char **argv;
	int ntps;
	struct ipc_torrent *tps;
	BTPDQ_HEAD(item_tq, item) hd;
};

void
itm_insert(struct items *itms, struct item *itm)
{
	struct item *p;
	BTPDQ_FOREACH(p, &itms->hd, entry)
	if (strcmp(itm->name, p->name) < 0)
		break;
	if (p != NULL)
		BTPDQ_INSERT_BEFORE(p, itm, entry);
	else
		BTPDQ_INSERT_TAIL(&itms->hd, itm, entry);
}

static void
list_cb(int obji, enum ipc_err objerr, struct ipc_get_res *res, void *arg)
{
	struct items *itms = arg;
	struct item *itm = calloc(1, sizeof(*itm));
	if (objerr != IPC_OK)
		diemsg("list failed for '%s' (%s).\n", itms->argv[obji],
			ipc_strerror(objerr));
	itms->count++;
	itm->num = (unsigned)res[IPC_TVAL_NUM].v.num;
	itm->st = tstate_char(res[IPC_TVAL_STATE].v.num);
	if (res[IPC_TVAL_NAME].type == IPC_TYPE_ERR)
		asprintf(&itm->name, "%s", ipc_strerror(res[IPC_TVAL_NAME].v.num));
	else
		asprintf(&itm->name, "%.*s", (int)res[IPC_TVAL_NAME].v.str.l,
			res[IPC_TVAL_NAME].v.str.p);
	itm->totup = res[IPC_TVAL_TOTUP].v.num;
	itm->cgot = res[IPC_TVAL_CGOT].v.num;
	itm->csize = res[IPC_TVAL_CSIZE].v.num;
	itm_insert(itms, itm);
}

void
print_items(struct items* itms)
{
	struct item *p;
	BTPDQ_FOREACH(p, &itms->hd, entry) {
		printf("%-40.40s %4u %c. ", p->name, p->num, p->st);
		print_percent(p->cgot, p->csize);
		print_size(p->csize);
		print_ratio(p->totup, p->csize);
		printf("\n");
	}
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
	struct items itms;

	itms.ntps = 0;
	itms.tps = NULL;
	itms.count = 0;
	itms.argv = NULL;
	BTPDQ_INIT(&itms.hd);

	btpd_connect();
	code = btpd_tget_wc(ipc, IPC_TWC_ALL, keys, nkeys, list_cb, &itms);
	if (code != IPC_OK)
		diemsg("command failed (%s).\n", ipc_strerror(code));

	printf("%-40.40s  NUM ST   HAVE    SIZE   RATIO\n", "NAME");
	print_items(&itms);
}
