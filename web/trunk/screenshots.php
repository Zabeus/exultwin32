<?php
	$HEADLINE = "Screenshots";
	$DATAFILE = "screenshots.dat";
	$CUSTOM_PARSE = true;

	include( "base.inc" ); 



	$tpl->define(
		array(
			screenshots		=> "screenshots.tpl",
			entry			=> "screenshot.tpl"
		)
	);

	
	// dirty trick to seperate data and content a bit ;)
	include("content/screenshots.dat");
	
	$tpl->parse(CONTENT, "screenshots");
	add_topic_headline($HEADLINE);
	$tpl->parse(MAIN, "main");
	$tpl->FastPrint();
	exit;


	function add_screenshot( $file, $width, $height, $desc )
	{
		global $tpl, $picture_count;
		
		$tpl->assign(
			array(
				FILENAME	=>	$file,
				WIDTH		=>	$width,
				HEIGHT		=>	$height,
				DESCRIPTION	=>	$desc
			)
		);
		$tpl->parse(ENTRIES,".entry");
	}
?>