<?php
	$HEADLINE = "Download";
	$DATAFILE = "download.dat";
	$CUSTOM_PARSE = true;

	include( "base.inc" ); 


	$tpl->define(
		array(
			download		=> "download.tpl",
			empty_tpl		=> "empty.tpl",
			section			=> "download_section.tpl",
			d_entry			=> "download_entry.tpl",
			s_entry			=> "snapshot_entry.tpl"
		)
	);

	// We seperate data and content. The content still is PHP code, but 
	// very easy to change and maintain
	include("content/download.dat");
	
	$tpl->parse(CONTENT, "download");
	add_topic_headline($HEADLINE);
	$tpl->parse(MAIN, "main");
	$tpl->FastPrint();
	exit;


	function begin_section( $section )
	{
		global $tpl;
		
		// Set the name of the current section
		$tpl->assign(
			array(
				SECTION_TITLE	=>	$section
			)
		);
	}

	function end_section()
	{
		global $tpl;
		
		// Write out the section
		$tpl->parse(DOWNLOAD_SECTIONS,".section");

		// Close the section entries list, in preparation for the new section
		$tpl->parse(SECTION_ENTRIES,"empty_tpl");
	}

	function add_extern_entry( $file, $desc, $size )
	{
		global $tpl;
		
		$tpl->assign(
			array(
				FILENAME	=>	$file,
				DESCRIPTION	=>	$desc,
				SIZE		=>	$size
			)
		);
		$tpl->parse(SECTION_ENTRIES,".d_entry");
	}

	function add_local_entry( $file, $desc )
	{
		global $tpl;
		
		$tpl->assign(
			array(
				FILENAME	=>	$file,
				DESCRIPTION	=>	$desc,
				FILE_TIME	=>	date("H:i",filemtime("snapshots/$file")),
				FILE_DATE	=>	date("Y-m-d",filemtime("snapshots/$file")),
				SIZE		=>	ceil(filesize("snapshots/$file")/1024)
			)
		);
		$tpl->parse(SECTION_ENTRIES,".s_entry");
	}
?>
