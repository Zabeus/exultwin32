<?
	$TITLE = "Exult";
	$HEADLINE = "What is it?";
	$CUSTOM_PARSE = true;

	$MOD_DATE = max(filemtime("content/intro.dat"),filemtime("content/news.dat"));
	$MOD_DATE = date("Y-m-d",$MOD_DATE);

	include( "base.inc" ); 


	add_topic("What is it?", join("", file ("content/intro.dat")));
	add_topic("News", join("", file ("content/news.dat")));
	$tpl->parse(MAIN, "main");
	$tpl->FastPrint();
	exit;
?>