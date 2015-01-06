<?php
	$HEADLINE = "News";
	$TITLE_IMAGE = "newstitle.png";
	$CUSTOM_PARSE = true;

	include( "base.inc" );

	if( isset($_REQUEST["showall"]) )
		$showall = true;
	else
		$showall = false;
	
	include("content/news.dat");

	output_boxes();
	$tpl->parse("MAIN", "main");
	$tpl->FastPrint();
?>
