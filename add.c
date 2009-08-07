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

	if((dest = fopen(path, "w")) == NULL) {
		diemsg("Can not open '%s' in writing mode: %s\n", path, strerror(errno));
	}

	for(;;) {
		if((r = fread(buf, 1, sizeof buf, src)) < 1)
			break;
		if ((w = fwrite(buf, 1, r, dest)) != r)
			diemsg("fwrite() : fatal error\n");
	}

	if(ferror(src))
		diemsg("Can not read uploaded file.\n");
	if(ferror(dest))
		diemsg("Can not write to the '%s' file.\n", path);

	fclose(src);
	fclose(dest);
	return 0;
}

int
fetch_file(char *url, char *tdir, char **tpath)
{
	FILE *fremote, *flocal;
	char *tname, buf[1024];
	size_t r;

	if((tname = strrchr(url, '/')) == NULL) {
		diemsg("Can not get the file name from the URL '%s'\n", url);
	}
	asprintf(tpath, "%s/%s", tdir, ++tname);

	/*
	 * fetch(3) use this environment variable and add its value to the HTTP
	 * request headers. Take care of user privacy and remove it.
	 */
	unsetenv("HTTP_REFERER");

	if((fremote = fetchGetURL(url, "dvv")) == NULL)
		diemsg("Error while initiating connection to server: %s\n", fetchLastErrString);

	if((flocal = fopen(*tpath, "w")) == NULL)
		diemsg("Can not open '%s' in writing mode: %s\n", tpath, strerror(errno));

	for(;;) {
		if((r = fread(buf, 1, sizeof buf, fremote)) < 1)
			break;
		if((fwrite(buf, 1, r, flocal)) != r)
			break;
	}

	if(ferror(fremote))
		diemsg("Error while reading file from server: %s\n", fetchLastErrString);
	if(ferror(flocal))
		diemsg("Error while writing to local file: %s\n", strerror(errno));

	fclose(fremote);
	fclose(flocal);
	return 0;
}

void cmd_add(CGI *cgi)
{
	int topdir, start;
	size_t dirlen = 0;
	char *cdir, *tdir, *tname, *fname, *torrent;
	FILE *tf;
	NEOERR *err;

	/* Check if the form has been submited */
	cgi_parse(cgi);
	if(hdf_get_value(cgi->hdf, "Query.submited", NULL) == NULL) {
		if((err = cgi_display(cgi, "add.cs"))) {
			cgi_neo_error(cgi, err);
		}
		return;
	}

	/* Init */
	GET_OR_DIE(cdir, "content_dir");
	GET_OR_DIE(tdir, "torrents_dir");
	dirlen = strlen(cdir);
	torrent = NULL;

	/* Get some params from the form */
	start = hdf_get_int_value(cgi->hdf, "Query.start", 0);
	topdir = hdf_get_int_value(cgi->hdf, "Query.topdir", 0);
	tname = hdf_get_value(cgi->hdf, "Query.name", NULL);

	/* Copy the uploaded file (if any) to torrents_dir */
	fname = hdf_get_value(cgi->hdf, "Query.torrent_file", NULL);
	if(fname != NULL && strcmp(fname, "") != 0) {
		tf = cgi_filehandle(cgi, "torrent_file");
		if(tf != NULL) {
			asprintf(&torrent, "%s/%s", tdir, fname);
			copy_file(tf, torrent);
		} else {
			diemsg("cgi_filehandle(): fatal error\n");
		}
	}

	/* Fetch the URL file (if any) to torrents_dir */
	fname = hdf_get_value(cgi->hdf, "Query.torrent_url", NULL);
	if(fname != NULL && strcmp(fname, "") != 0) {
		fetch_file(fname, tdir, &torrent);
	}

	/* Check if we have a torrent file */
	if(torrent == NULL) {
		diemsg("Error: no torrent specified\n");
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
	iobuf_write(&iob, cdir, dirlen);
	if (topdir && !mi_simple(mi)) {
		size_t tdlen;
		const char *td = benc_dget_mem(benc_dget_dct(mi, "info"), "name", &tdlen);
		iobuf_swrite(&iob, "/");
		iobuf_write(&iob, td, tdlen);
	}
	iobuf_swrite(&iob, "\0");
	if((errno = make_abs_path(iob.buf, dpath)) != 0)
		diemsg("make_abs_path '%s' failed (%s).\n", dpath, strerror(errno));
	code = btpd_add(ipc, mi, mi_size, dpath, tname);
	if(code == 0 && start) {
		struct ipc_torrent tspec;
		tspec.by_hash = 1;
		mi_info_hash(mi, tspec.u.hash);
		code = btpd_start(ipc, &tspec);
	}
	if (code != IPC_OK)
		diemsg("command failed (%s).\n", ipc_strerror(code));

	if((err = cgi_display(cgi, "add.cs"))) {
		cgi_neo_error(cgi, err);
	}
	return;
}
