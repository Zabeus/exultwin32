<?php
	$TITLE = "Exult - About Us";
	$HEADLINE = "Exult Team";
	$CUSTOM_PARSE = true;

	$MOD_DATE = max(filemtime("content/about.dat"),filemtime("content/irc.dat"));
	$MOD_DATE = date("Y-m-d",$MOD_DATE);

	include( "base.inc" ); 


	add_topic("Exult Team", join("", file ("content/about.dat")));
	add_topic("Official IRC channel", join("", file ("content/irc.dat")));
	$tpl->parse(MAIN, "main");
	$tpl->FastPrint();
	exit;
?>
