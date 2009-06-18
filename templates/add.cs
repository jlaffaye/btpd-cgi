<?cs include:"header.cs" ?>
<div id="add-box">
<form id="upload" method="post" enctype="multipart/form-data" action="">
<p>Select the torrent you wish to add.<br />
<input type="file" name="torrent" size="50" /><br /><br />
<input type="checkbox" name="start" />Automatically start the torrent.<br />
<input type="submit" value="Add Torrent" /></p>
</form>
</div>
<?cs include:"footer.cs" ?>
