<?php
	// Determine current file name
	if(ereg("[^/]+$", $PHP_SELF, $regs))
		$CURRENTFILE = $regs[0];


	// Counter stuff:
	include("qipcounter.php3");
	$myCounter = new PageCounter("../cgi-bin/count.qip", "../cgi-bin/ipdata.qip");
	if( $myCounter->checkIP($REMOTE_ADDR) )
		$myCounter->inc();
	if( $XHTML )
		$counterHTML = $myCounter->getAsXHTML();
	else
		$counterHTML = $myCounter->getAsHTML();

	// Default value for title
	if( !isset($TITLE) )
		$TITLE = "Exult - $HEADLINE";

	// Default value for main template
	if( !isset($MAIN_TEMPLATE) )
		$MAIN_TEMPLATE = "main";

	// Content is by default loaded from a data file
	if( !isset($CONTENT) && isset($DATAFILE) )
	{
		$CONTENT = join("", file ("content/$DATAFILE"));
		if( !isset($MOD_DATE) )
			$MOD_DATE = date("Y-m-d",filemtime("content/$DATAFILE"));
	}
	
	// Default value for modification time
	if( !isset($MOD_DATE) )
		$MOD_DATE = date("Y-m-d",filemtime($CURRENTFILE));

	if( $XHTML )
		$MAIN_TEMPLATE .= "_xhtml";



	// Template stuff follows
	include("class.FastTemplate.php3");

	$tpl = new FastTemplate("./templates");

	$tpl->define(
		array(
			main			=> "$MAIN_TEMPLATE.tpl",
			topic			=> "topic.tpl"
		)
	);
	
	$tpl->assign(
		array(
			URL			=>	$PHP_SELF,
			COUNTER		=>	$counterHTML,
			MOD_DATE	=>	$MOD_DATE,
			CURRENTFILE	=>	$CURRENTFILE,

			TITLE		=>	$TITLE
		)
	);
	
	
	if (!$CUSTOM_PARSE )
	{
		add_topic($HEADLINE,$CONTENT);
		$tpl->parse(MAIN, "main");
		$tpl->FastPrint();
		exit;
	}

	function add_topic_headline( $headline )
	{
		global $tpl;
		
		$tpl->assign(
			array(
				HEADLINE	=>	$headline
			)
		);
		$tpl->parse(TOPICS,".topic");
	}

	function add_topic( $headline, $content )
	{
		global $tpl;
		
		$tpl->assign(
			array(
				HEADLINE	=>	$headline,
				CONTENT		=>	$content
			)
		);
		$tpl->parse(TOPICS,".topic");
	}
?>
