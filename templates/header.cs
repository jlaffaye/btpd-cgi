<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<title>BTPDcgi - <?cs alt:Query.cmd ?>list<?cs /alt ?></title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<?cs if:Query.cmd == "list" ?>
	<meta http-equiv="refresh" content="<?cs alt:refresh_interval ?>15<?cs /alt ?>" />
<?cs /if ?>
<link href="./style.css" rel="stylesheet" type="text/css" />
</head>
<body>

<div id="header">
	<div id="title">BTPDcgi</div>
	<div id="right">
		<div id="loggedin">Logged in as <em><?cs alt:CGI.RemoteUser ?>anonymous<?cs /alt ?></em></div>
		<div id="menu">
			<a href="<?cs var:CGI.ScriptName ?>?cmd=add">Add torrent</a> |
			<a href="<?cs var:CGI.ScriptName ?>">List torrent</a>
		</div>
	</div>
</div>

<div id="content">
