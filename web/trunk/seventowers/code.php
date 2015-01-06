<?php
	$HEADLINE = "Usecode document";

	// Silently correct errors.
    $img_regexp = "/^[A-Za-z0-9_-]+\.png$/";
	if( isset($_REQUEST["TITLE_IMAGE"])
			&& preg_match($img_regexp, $_REQUEST["TITLE_IMAGE"]) )
		$TITLE_IMAGE = $_REQUEST["TITLE_IMAGE"];
	else
		$TITLE_IMAGE = "usecodetitle.png";

	// Silently correct errors.
    $dat_regexp = "/^[A-Za-z0-9_-]+\.dat$/";
	if( isset($_REQUEST["DATAFILE"])
			&& preg_match($dat_regexp, $_REQUEST["DATAFILE"]) )
		$DATAFILE = $_REQUEST["DATAFILE"];
	else
		$DATAFILE = "exult_intrinsics.dat";

	// Silently correct errors.
	if( !isset($_REQUEST["OUTPUT"]) )
		$OUTPUT = "html";
	else
		$OUTPUT = $_REQUEST["OUTPUT"];

	// Silently correct errors.
	if( isset($_REQUEST["TYPE"]) && is_numeric($_REQUEST["TYPE"]) )
		$TYPE = intval($_REQUEST["TYPE"]);
	else
		$TYPE = 0;

	/* ++++ These vars must be filled from the calling page
		set_time_limit(0);
		$TITLE_IMAGE = "usecodetitle.png";
		$DATAFILE = "exult_intrinsics.dat";
		$OUTPUT = "naturaldocs";
		$TYPE = 0;
		// For testing:
		set_include_path("D:\AmazingDocs\Misc\HomePage\V4 PHP");
	//*/
	
	$CUSTOM_PARSE = true;
	
	$modelist = array ("html" => 0, "text" => 1, "naturaldocs" => 2);
	
	if (array_key_exists($OUTPUT, $modelist))
		$outmode = $modelist[$OUTPUT];
	else
		$outmode = 0;

	$reference_mode = false;
	
	if ($outmode == 2)
		$reverse_titles = true;
	else
		$reverse_titles = false;

	include( "base.inc" );
	include( "code.inc" );
	include( "usecode/$DATAFILE" );
	
	if ($outmode == 0)
	{
		empty_submenubar();
		$tpl->parse("MAIN", "main");
		$tpl->FastPrint();
	}
	else
	{
		$file = basename($DATAFILE, ".dat");
		$disptype = ($TYPE == 1 ? "attachment" : "inline");
		$content = html_entity_decode($tpl->fetch("CONTENT"));
		$length = strlen($content);
		header("Content-Disposition: $disptype; filename=$file.txt");
		header("Accept-Ranges: bytes");
		header("Content-Length: $length");
		header("Content-type: text/plain");
		print($content);
	}
?>