<?cs def:print_sizes(torrent) ?>
	<?cs var:torrent.downloaded_size ?> of <?cs var:torrent.size ?> [<?cs var:torrent.percent ?>]
<?cs /def ?>

<?cs include:"header.cs" ?>
<div class="torrent">
<?cs each:torrent = Torrents ?>
	<table cellspacing="0">
		<tr>
			<td class="title"><?cs var:torrent.name ?></td>
			<td rowspan="3" class="nav">
				<?cs if:torrent.st == "inactive" ?>
					<a href="<?cs var:CGI.ScriptName ?>?cmd=start&amp;torrent=<?cs var:torrent.num ?>">Start</a>
				<?cs else ?>
					<a href="<?cs var:CGI.ScriptName ?>?cmd=stop&amp;torrent=<?cs var:torrent.num ?>">Stop</a>
				<?cs /if ?>
				<br />
				<a href="<?cs var:CGI.ScriptName ?>?cmd=del&amp;torrent=<?cs var:torrent.num ?>">Delete</a>
			</td>
		</tr>
		<tr>
			<td>
				<div class="prog-bar">
					<div class="prog-bar-<?cs var:torrent.st ?>" style="width: <?cs var:torrent.percent ?>"></div>
				</div>
			</td>
		</tr>
		<tr>
			<td>
			<?cs if:torrent.st == "inactive" ?>
				Inactive - <?cs call:print_sizes(torrent) ?>
			<?cs /if ?>
			<?cs if:torrent.st == "leech" ?>
				Downloading from <?cs var:torrent.peers ?> peers - <?cs call:print_sizes(torrent) ?> -
				Down: <?cs var:torrent.rate_down ?>, Up: <?cs var:torrent.rate_up ?> - Available: <?cs var:torrent.available ?>
			<?cs /if ?>
			<?cs if:torrent.st == "seed" ?>
				Seeding to <?cs var:torrent.peers ?> - Up: <?cs var:torrent.rate_up ?> - Ratio: <?cs var:torrent.ratio ?>
			<?cs /if ?>
			<?cs if:torrent.st == "start" ?>
				Starting...
			<?cs /if ?>
			<?cs if:torrent.st == "stop" ?>
				Stoping...
			<?cs /if ?>
			</td>
		</tr>
	</table>
	<?cs /each ?>
</div>
<?cs include:"templates/footer.cs" ?>
