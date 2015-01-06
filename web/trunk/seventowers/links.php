<?php
	function add_logo_link( $name, $url, $logo, $desc )
	{
		global $tpl;
		
		$tpl->assign("EXTERN_LINK", extern_logo_link($name, $logo, $url) );
		$tpl->assign("LINK_DESC", $desc);
		$tpl->parse("LINKLIST",".e_link");
	}
	
	function add_link( $name, $url, $tip, $desc )
	{
		global $tpl;
		
		$tpl->assign("EXTERN_LINK", extern_link($name, $url, $tip) );
		$tpl->assign("LINK_DESC", $desc);
		$tpl->parse("LINKLIST",".e_link");
	}

	function make_links_section( $section_data )
	{
		global $tpl;
		
		$tpl->assign("DESC_TITLE", make_anchor($section_data["NAME"], $section_data["TARGET"]));

		foreach( $section_data["LINKS"] as $link )
		{
			if ($link["LINK_TYPE"] == 0)
				add_link(
						$link["LINK_NAME"], $link["LINK_URL"],
						$link["LINK_TIP"], $link["LINK_DESC"]
					);
			else
				add_logo_link(
						$link["LINK_NAME"], $link["LINK_URL"],
						$link["LINK_LOGO"], $link["LINK_DESC"]
					);
		}
		$tpl->assign("ICON", image( "Up arrow", "uparrow.png"));
		// Write out the section
		$tpl->parse("ROWS", ".linksection");
		// Close the section, in preparation for a possible new section
		$tpl->clear("LINKLIST");
	}
	
	$HEADLINE = "Links";
	$TITLE_IMAGE = "linkstitle.png";
	$CUSTOM_PARSE = true;

	include( "base.inc" );

	$tpl->define(
		array(
			"links"			=>	"linkslist.tpl",
			"e_link"		=>	"extern_link.tpl",
		)
	);
	
	$tpl->define_dynamic("linksection", "links");
	
	include("content/links.dat");

	$tpl->parse("CONTENT", "links");
	$tpl->parse("MAIN", "main");
	$tpl->FastPrint();
?>
