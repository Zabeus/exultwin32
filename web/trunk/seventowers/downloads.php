<?php
	$PAGES = array(
    	array(
    		"DAT"	=>	"exultmods.dat",
    		"IMG"	=>	"exultmodstitle.png",
      		"HEADS"	=>	array(
        		"Exult Mods",
        		"Black Gate Keyring",
        		"Serpent Isle Fixes",
        		"Avatar Pack",
    		)
		),
		array(
    		"DAT"	=>	"usecode.dat",
    		"IMG"	=>	"usecodetitle.png",
    		"HEADS"	=>	array(
        		"Usecode C",
        		"Tutorials",
        		"Reference",
        		"Syntax Highlighters",
    		)
		),
		array(
    		"DAT"	=>	"ultimaicons.dat",
    		"IMG"	=>	"ultimaicons.png",
    		"HEADS"	=>	array(
        		"Ultima Icons"
    		)
		),
  );

	// Silently correct errors.
	if( isset($_REQUEST["section"]) && is_numeric($_REQUEST["section"]) )
		$section = intval($_REQUEST["section"]);
	else
		$section = 0;

	// Silently correct errors.
	if( isset($_REQUEST["page"]) && is_numeric($_REQUEST["page"]) )
		$page = intval($_REQUEST["page"]);
	else
		$page = 0;
	
	$FILEINFO = $PAGES[$section];
	
	$HEADS = $FILEINFO["HEADS"];
	$DATAFILE = $FILEINFO["DAT"];
	$TITLE_IMAGE = $FILEINFO["IMG"];

	$HEADLINE = $HEADS[$page];

	$CUSTOM_PARSE = true;
	$SUBMENUFILE = "submenubar.dat";

	include( "base.inc" );

	include( "download.inc" );
?>
