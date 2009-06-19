#include "btpdcgi.h"

#define GET_OR_DIE(var, name) \
	var = hdf_get_value(cgi->hdf, name, NULL); \
	if(var == NULL) \
		diemsg("The '%s' config variable is needed.\n", name);

int
copy_file(FILE *src, char *path)
{
	FILE *dest;
	char buf[2048];
	size_t r,w;

	dest = fopen(path, "w+");

	for(;;) {
		if((r = fread(buf, 1, sizeof buf, src)) < 1)
			break;
		if ((w = fwrite(buf, 1, r, dest)) != r)
			diemsg("fwrite() : fatal error\n");
	}

	if (ferror(src))
		diemsg("Can not read src file.\n");
	if (ferror(dest))
		diemsg("Can not write dest file.\n");

	fclose(src);
	fclose(dest);
	return 0;
}

void cmd_add(CGI *cgi)
{
	int topdir, start;
	size_t dirlen = 0;
	char *dir = NULL, *name = NULL, *tdir = NULL;
	FILE *tf;
	char *torrent = NULL;

	/* Check if the form has been submited */
	cgi_parse(cgi);
	if(hdf_get_value(cgi->hdf, "Query.submited", NULL) == NULL) {
		cgi_display(cgi, "add.cs");
		return;
	}

	/* Get some params from the config file */
	GET_OR_DIE(dir, "content_dir");
	GET_OR_DIE(tdir, "torrents_dir");
	dirlen = strlen(dir);

	/* Get some params form the form */
	start = hdf_get_int_value(cgi->hdf, "Query.start", 0);
	topdir = hdf_get_int_value(cgi->hdf, "Query.topdir", 0);
	name = hdf_get_value(cgi->hdf, "Query.name", NULL);

	/* Copy the torrent file to torrents_dir */
	tf = cgi_filehandle(cgi, "torrent_file");
	if(tf != NULL) {
		asprintf(&torrent, "%s/%s", tdir, hdf_get_value(cgi->hdf, "Query.torrent_file", "default"));
		copy_file(tf, torrent);
	}

	/* We have all the infos, let's add the torrent */
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

	cgi_display(cgi, "add.cs");
	return;
}
