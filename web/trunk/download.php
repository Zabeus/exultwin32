<?php
	$HEADLINE = "Download";
	$DATAFILE = "download.dat";
	$CUSTOM_PARSE = true;

	include( "base.inc" ); 


	$tpl->define(
		array(
			download		=> "download.tpl",
			d_entry			=> "download_entry.tpl",
			s_entry			=> "snapshot_entry.tpl"
		)
	);

	// dirty trick to seperate data and content a bit ;)
	include("content/download.dat");
	
	$tpl->parse(CONTENT, "download");
	add_topic_headline($HEADLINE);
	$tpl->parse(MAIN, "main");
	$tpl->FastPrint();
	exit;


	function add_download( $file, $desc, $size )
	{
		global $tpl;
		
		$tpl->assign(
			array(
				FILENAME	=>	$file,
				DESCRIPTION	=>	$desc,
				SIZE		=>	$size
			)
		);
		$tpl->parse(DOWNLOAD_ENTRIES,".d_entry");
	}

	function add_download2( $file, $desc, $size )
	{
		global $tpl;
		
		$tpl->assign(
			array(
				FILENAME	=>	$file,
				DESCRIPTION	=>	$desc,
				SIZE		=>	$size
			)
		);
		$tpl->parse(DOWNLOAD_ENTRIES2,".d_entry");
	}

	function add_snapshot( $file, $desc )
	{
		global $tpl;
		
		$tpl->assign(
			array(
				FILENAME	=>	$file,
				DESCRIPTION	=>	$desc,
				FILE_TIME	=>	date("H:i",filemtime("snapshots/$file")),
				FILE_DATE	=>	date("Y-m-d",filemtime("snapshots/$file")),
				SIZE		=>	ceil(filesize("snapshots/$file")/1000)
			)
		);
		$tpl->parse(SNAPSHOT_ENTRIES,".s_entry");
	}

		function add_pentagram( $file, $desc )
	{
		global $tpl;
		
		$tpl->assign(
			array(
				FILENAME	=>	$file,
				DESCRIPTION	=>	$desc,
				FILE_TIME	=>	date("H:i",filemtime("snapshots/$file")),
				FILE_DATE	=>	date("Y-m-d",filemtime("snapshots/$file")),
				SIZE		=>	ceil(filesize("snapshots/$file")/1000)
			)
		);
		$tpl->parse(PENTAGRAM_ENTRIES,".s_entry");
	}
	
	function add_datafile( $file, $desc, $size )
	{
		global $tpl;
		
		$tpl->assign(
			array(
				FILENAME	=>	$file,
				DESCRIPTION	=>	$desc,
				SIZE		=>	$size
			)
		);
		$tpl->parse(DATA_ENTRIES,".d_entry");
	}

?>
