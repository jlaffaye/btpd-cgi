#include "btpdcgi.h"

void cmd_add(CGI *cgi)
{
	int topdir = 0, start = 1;
	size_t dirlen = 0;
	char *dir = NULL, *name = NULL;
	char *torrent = NULL;

	btpd_connect();
	char *mi;
	size_t mi_size;
	enum ipc_err code;
	char dpath[PATH_MAX];
	struct iobuf iob;

	if((mi = mi_load(torrent, &mi_size)) == NULL)
		diemsg("error loading '%s' (%s).\n", torrent, strerror(errno));

	iob = iobuf_init(PATH_MAX);
	iobuf_write(&iob, dir, dirlen);
	if (topdir && !mi_simple(mi)) {
		size_t tdlen;
		const char *td = benc_dget_mem(benc_dget_dct(mi, "info"), "name", &tdlen);
		iobuf_swrite(&iob, "/");
		iobuf_write(&iob, td, tdlen);
	}
	iobuf_swrite(&iob, "\0");
	if((errno = make_abs_path(iob.buf, dpath)) != 0)
		diemsg("make_abs_path '%s' failed (%s).\n", dpath, strerror(errno));
	code = btpd_add(ipc, mi, mi_size, dpath, name);
	if(code == 0 && start) {
	struct ipc_torrent tspec;
	tspec.by_hash = 1;
	mi_info_hash(mi, tspec.u.hash);
	code = btpd_start(ipc, &tspec);
	}
	if (code != IPC_OK)
		diemsg("command failed (%s).\n", ipc_strerror(code));
	return;
}
