<?php
	// Determine current file name
	if(ereg("[^/]+$", $PHP_SELF, $regs))
		$CURRENTFILE = $regs[0];

	// Default value for title
	if( !isset($TITLE) )
	{
		if (isset($HEADLINE))
			$TITLE = "Seven Towers: $HEADLINE";
		else
			$TITLE = "Seven Towers";
	}

	// Default value for main template
	if( !isset($MAIN_TEMPLATE) )
		$MAIN_TEMPLATE = "main";

	// Default value for modification time
	if( !isset($MOD_DATE) )
		$MOD_DATE = date("Y-m-d", filemtime($CURRENTFILE));

	// Template stuff follows
	include("class.FastTemplate.php3");

	$tpl = new FastTemplate("./templates");

	$EXULT_LINK = extern_link("Exult", "http://exult.sourceforge.net/");

	// Support for outdated browsers (IE)
	if (false != stristr($_SERVER['HTTP_USER_AGENT'], "MSIE 6.0"))
		$tpl->assign("OUTDATED_BROWSER_CSS", '<link href="outdated.css" rel="stylesheet" type="text/css" />');
	else
		$tpl->assign("OUTDATED_BROWSER_CSS", '<!-- empty! -->');

	$tpl->assign(
		array(
			"URL"			=>	$PHP_SELF, 
			"MOD_DATE"		=>	$MOD_DATE, 
			"CURRENTFILE"	=>	$CURRENTFILE, 
			"TITLE"			=>	$TITLE, 
			"TITLE_IMAGE"	=>	image($TITLE, $TITLE_IMAGE)
		)
	);
	
	$tpl->define(
		array(
			"main"			=>	"$MAIN_TEMPLATE.tpl", 
			"empty_tpl"		=>	"empty.tpl", 
			"navbar"		=>	"navbar.tpl", 
			"section"		=>	"section.tpl", 
			"para"			=>	"paragraph.tpl", 
			"lpara"			=>	"logo_paragraph.tpl", 
			"box"			=>	"box.tpl", 
			"box_item"		=>	"box_item.tpl", 
			"secbar"		=>	"sectionbar.tpl", 
			"menu_button"	=>	"menu_button.tpl", 
			"menu_cur"		=>	"currpage.tpl"
		)
	);
	
	$tpl->define_dynamic("navbutton", "navbar");
	$tpl->define_dynamic("boxes", "box");
	
	include("content/navbar.dat");

	if (!$CUSTOM_PARSE )
	{
		include("content/$DATAFILE");
		empty_submenubar();
		$tpl->parse("MAIN", "main");
		$tpl->FastPrint();
		exit;
	}
	
	function image( $name, $file )
	{
		if ($file == "" || $file == ".png")
			return "[$name]";
		list($width, $height, $type, $attr) = getimagesize("images/$file");
		return "<img $attr alt=\"$name\" src=\"images/$file\"/>";
	}
	
	function local_link( $name, $url, $tip = null )
	{
		if (!($tip === null))
			$tip = " title=\"$tip\"";
		return "<a href=\"$url\"$tip>$name</a>";
	}
	
	function extern_link( $name, $url, $tip = null )
	{
		$img = image("Extern link", "externlink.png");
		if (!($tip === null))
			$tip = " title=\"$tip\"";
		return "<a href=\"$url\" class=\"offsite\"$tip>$name&nbsp;$img</a>";
	}
	
	function extern_logo_link( $name, $logo, $url )
	{
		$img = image("Extern link", "externlink.png");
		$logoimg = image("$name Logo", "$logo.png");
		return "<a href=\"$url\" class=\"offsite\">$logoimg$img</a>";
	}
	
	function quick_tip( $name, $tip )
	{
		return "<span class=\"quicktip\" title=\"$tip\">$name</span>";
	}
	
	function format_text( $text, $bold = false, $italic = false, $tip = null )
	{
		$font = "";
		if ($bold && $italic)
			$font = 'style="font-weight: bold; font-style: italic"';
		else if ($bold)
			$font = 'style="font-weight: bold;"';
		else if ($italic)
			$font = 'style="font-style: italic"';
		
		if (!($tip === null))
			$tip = "class=\"quicktip\" title=\"$tip\"";

		return "<span $font $tip>$text</span>";
	}

	function make_navbar()
	{
		global $tpl;
		
		// Write out the section
		$tpl->parse("NAVBAR", "navbar");

		// Close the nav bar
		$tpl->clear("ROWS");
	}

	function add_nav_button( $file, $desc, $icon )
	{
		global $tpl;
		
		$tpl->assign(
			array(
				"NAV_DEST"	=>	$file, 
				"NAV_TITLE"	=>	$desc, 
				"NAV_ICON"	=>	image($desc, $icon), 
				"NAV_CLASS"	=>	"navbar"
			)
		);
		
		global $HEADLINE;
		if ($desc == $HEADLINE)
			$tpl->parse("MENUITEM", "menu_cur");
		else
			$tpl->parse("MENUITEM", "menu_button");
		$tpl->parse("ROWS", ".navbutton");
	}

	function empty_submenubar()
	{
		global $tpl;
		
		$tpl->parse("SUBBAR", "empty_tpl");
	}

	function make_submenubar()
	{
		global $tpl;
		
		// Write out the section
		$tpl->parse("SUBBAR", ".secbar");

		// Close the nav bar
		$tpl->clear("SUBBUTTONS");
	}

	function add_sub_button( $target, $desc, $icon = null, $curr = false )
	{
		global $tpl;
		
		$tpl->assign(
			array(
				"NAV_DEST"	=>	$target, 
				"NAV_TITLE"	=>	$desc . "&nbsp;&nbsp;", 
				"NAV_ICON"	=>	($icon === null) ? "" : image($desc, $icon),
				"NAV_CLASS"	=>	"subsections"
			)
		);
		
		global $HEADLINE;
		if ($desc == $HEADLINE || $curr)
			$tpl->parse("SUBBUTTONS", ".menu_cur");
		else
			$tpl->parse("SUBBUTTONS", ".menu_button");
	}

	function make_anchor( $title, $anchor = null )
	{
		if (!($anchor === null))
			return "<a class=\"target\" id=\"$anchor\">$title</a>";
		else
			return $title;
	}
	
	function make_section( $title, $anchor = null )
	{
		global $tpl;
		$tpl->assign("DESC_TITLE", make_anchor($title, $anchor));
	}

	function close_section()
	{
		global $tpl;
		
		// Write out the section
		$tpl->parse("CONTENT", ".section");
		// Close the box text, in preparation for a possible new box
		$tpl->clear("DESCRIPTION_TEXT");
	}
	
	function add_text( $text, $showicon = false, $icon = null )
	{
		global $tpl;
		global $HEADLINE;
		
		$tpl->assign(
			array(
				"DESCRIPTION_PARA"	=>	"$text", 
				"ICON"				=>	image($HEADLINE, $icon)
			)
		);
		if ($showicon && !($icon === null))
			$tpl->parse("DESCRIPTION_TEXT", ".lpara");
		else
			$tpl->parse("DESCRIPTION_TEXT", ".para");
	}
	
	function make_box( $title, $text, $single = false )
	{
		global $tpl;
		
		$tpl->assign("BOXTITLE", $title);
		if (is_array($text))
			foreach($text as $para)
				add_box_text($para);
		else
			add_box_text($text);
		
		// Write out the section
		$tpl->parse("ROWS", ".boxes");
		// Close the box text, in preparation for a possible new box
		$tpl->clear("BOX_TEXT");
		
		if ($single)
			$tpl->parse("CONTENT", ".box");
	}

	function output_boxes()
	{
		global $tpl;
		// Write out the section
		$tpl->parse("CONTENT", ".box");
	}

	function add_box_text( $text )
	{
		global $tpl;
		$tpl->assign("BOX_LINE_TEXT", "$text");
		$tpl->parse("BOX_TEXT", ".box_item");
	}

    function randstr()
    {
    	$charset = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789';
    	$z = strlen($charset);
    	$s = '';
		for ($i = 0; $i < 32; $i++)
			$s .= $charset{mt_rand()%$z};
		return $s;
    }
?>
