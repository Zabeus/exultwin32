<?php
	include_once('geshi.php');
	$linelen = 80;
	if (!isset($outmode))
		$outmode = 0;
	$tpl->define(
		array(
			"any"		=>	"anytext.tpl"
			)
		);
	
    function anchor($text, $out = false)
    {
    	global $outmode;
        if (!$outmode)
    	{
    		$anchor = make_anchor("", $text);
        	if ($out)
        		out($anchor);
        	else
    			return $anchor;
    	}
    }
    
    function add_link($text, $url)
    {
    	global $outmode;
        if (!$outmode)
    		return local_link($text, $url);
    	else if ($outmode == 2)
    		return "<$text>";
    	else
    		return $text;
    }
    
    function add_tip($text, $tip)
    {
    	global $outmode;
        if (!$outmode)
    		return quick_tip($text, $tip);
    	else
    		return $text;
    }
    
    function parse_code($source, $lines = false)
    {
    	$geshi = new GeSHi($source, 'ucc');
    	$geshi->enable_classes();
    	if ($lines)
    	{
    		$geshi->enable_line_numbers(GESHI_FANCY_LINE_NUMBERS);
    		// Blargh... needed for XHTML compliance, though:
        	$geshi->set_header_type(GESHI_HEADER_DIV);
    	}
    	return $geshi->parse_code();
    }
    
    function code_block($source, $block = true, $lines = false, $offset = 0, $out = true)
    {
    	global $outmode, $linelen;
        // Tabs in <pre> tags looks awful, so we convert to spaces.
        $source = str_replace("\t", "    ", $source);
        if (!$outmode)
    	{
    		$left = $offset/2 . "em";
    		$code = parse_code($source, $lines);
    		if ($block)
        		$code = "<div class=\"codeblock\" style=\"margin-left: $left\">$code</div>";
        	else
        		$code = "<div class=\"bigcodeblock\" style=\"margin-left: $left\">$code</div>";
    	}
    	else if ($outmode == 2)
    	{
			$len = $linelen - $offset;
    		$txt = explode("\n", $source);
    		$pad = "// > " . str_pad("", $offset);
    		if (is_array($txt))
    		{
    			foreach ($txt as $i => $line)
            		$txt[$i] = $pad . $line;
    			$txt = implode("\n", $txt);
    		}
    		else
    			$txt = $pad . $txt;
    		$code = "// Definitions:\n$txt\n//\n";
		}
    	else
    	{
    		if ($out)
    		{
    			$len = $linelen - $offset;
        		$txt = explode("\n", $source);
        		$pad = str_pad("", $offset);
        		if (is_array($txt))
        		{
        			foreach ($txt as $i => $line)
                		$txt[$i] = wordwrap($line, $len, "\n$pad" . str_pad("", 8));
        			$txt = $pad . implode("\n$pad", $txt);
        		}
        		else
        			$txt = $pad . wordwrap($txt, $len, "\n$pad" . str_pad("", 8));
        		$code = "$txt\n\n";
        	}
        	else
        		$code = $source;
		}
    	if ($out)
    		out($code);
    	else
			return $code;
    }

    function inline_code($source)
    {
    	global $outmode;
    	if (!$outmode)
    	{
    		ob_start();
    		$ret = geshi_highlight($source, 'ucc');
    		$code = ob_get_contents();
			ob_end_clean();
    	}
    	else if ($outmode == 2)
    	{
    		$file1 = "usecode/intrinsics/$source.dat";
    		$file2 = str_replace("UI_", "", $file1);
    		if (file_exists($file1))
    			$code = "<UI_$source>";
    		else if (file_exists($file2))
    			$code = "<$source>";
	    	else
	    		$code = $source;
    	}
    	else
    		$code = $source;
		return $code;
    }

    function code_file($file, $block = true, $lines = false, $offset = 0, $out = true)
    {
    	global $outmode;
		$source = file_get_contents("usecode/$file");
    	$code = code_block($source, $block, $lines, $offset, $out);
    	if ($out)
    		out($code);
    	else
			return $code;
    }
    
    function join_images($images, $out = true)
    {
    	global $outmode;
    	if (!$outmode)
        		$imgs = implode("", $images);
            else
            	$imgs = "[" . implode(", ", $images) . "]";
    	if ($out)
    		out($imgs);
    	else
			return $imgs;
    }
    
    function make_header($text, $level = 2, $out = true, $anchor = "")
    {
    	global $outmode, $linelen;
    	if ($outmode != 2 && $level)
    		$text = strtoupper($text);
    	if (!$outmode)
    	{
    		if ($level == 2)
    			$tag = "h2";
    		else if ($level == 1)
    			$tag = "h3";
    		else
    			$tag = "h4";
    		$header = "<$tag>$anchor$text</$tag>";
    	}
    	else if ($outmode == 2)
    	{
    		if ($level == 2)
    			$header = "// Title: $text\n";
    		else if ($level == 1)
    			$header = "\n\n// Function: $text\n";
    		else
    			$header = "// $text:\n";
    	}
    	else
    	{
    		if ($level)
    		{
	    		if ($level == 2)
	    			$head = "=";
	    		else
	    			$head = "-";
	    		$header = str_pad("", $linelen, $head) . "\n$text\n" . str_pad("", $linelen, $head) . "\n\n";
	    	}
    		else
	    		$header = "  $text:\n";
    	}
    	if ($out)
    		out($header);
    	else
			return $header;
    }
    
    function bold($text)
    {
    	global $outmode;
    	if (!$outmode)
    		return format_text($text, true, false);
    	else
    		return "*$text*";
    }

    function italic($text)
    {
    	global $outmode;
    	if (!$outmode)
    		return format_text($text, false, true);
    	else if ($outmode == 2)
    		return "*$text*";
    	else
    		return "/$text/";
    }

    function text($text, $offset = 4, $out = true)
    {
    	global $outmode, $linelen;
    	if (!$outmode)
    	{
    		$left = $offset/2 . "em";
    		if (is_array($text))
    		{
    			$outp = "";
    			foreach($text as $line)
    				$outp .= "<p style=\"margin-left: $left\">$line</p>";
    			$txt = $outp;
    		}
    		else
    			$txt = "<p style=\"margin-left: $left\">$text</p>";
    	}
    	else if ($outmode == 2)
    	{
    		if (is_array($text))
    		{
    			foreach($text as $line)
    			{
            		$linetxt = wordwrap($line, $linelen-$offset-3);
            		$txt .= "// " . str_pad("", $offset) . 
        					str_replace("\n", "\n// " . str_pad("", $offset), $linetxt);
        			$txt .= "\n//\n";
    			}
    		}
    		else
    		{
        		$txt = wordwrap($text, $linelen-$offset-3);
        		$txt = "// " . str_pad("", $offset) . 
    					str_replace("\n", "\n// " . str_pad("", $offset), $txt);
    			$txt .= "\n//\n";
    		}
    	}
    	else
    	{
    		if (is_array($text))
    		{
    			foreach($text as $line)
    			{
            		$linetxt = wordwrap($line, $linelen-$offset);
            		$txt .= str_pad("", $offset) . 
        					str_replace("\n", "\n" . str_pad("", $offset), $linetxt);
        			$txt .= "\n\n";
    			}
    		}
    		else
    		{
        		$txt = wordwrap($text, $linelen-$offset);
        		$txt = str_pad("", $offset) . 
    					str_replace("\n", "\n" . str_pad("", $offset), $txt);
    			$txt .= "\n\n";
    		}
    	}
    	if ($out)
    		out($txt);
    	else
			return $txt;
    }
    
    function add_image($img, $name, $out = true)
    {
    	global $outmode;
    	if (!$outmode)
    		$imgout = image($name, $img);
    	else
    		$imgout = $name;
    	if ($out)
    		out($imgout);
    	else
			return $imgout;
    }
    
    function right_align($para, $text, $out = true)
    {
    	global $outmode, $linelen;
    	if (!$outmode)
    	{
    		if ($out)
    			$tag = "p";
    		else
    			$tag = "span";
    		$txt = "<$tag class=\"right\">$text</$tag>$para";
    	}
    	else if ($outmode == 2)
    	{
    		$namemap = array(
				"[BG]" => "Please note this is a *Black Gate only* intrinsic.",
				"[SI]" => "Please note this is a *Serpent Isle only* intrinsic.",
				"[Exult]" => "Please note this is an *Exult specific* intrinsic.",
				"[Exult: BG]" => "Please note this is a *Black Gate only, Exult specific* intrinsic.",
				"[Exult: SI]" => "Please note this is a *Serpent Isle only, Exult specific* intrinsic.",
				"[BG, Exult]" => "Please note that in the original usecode, this was a *Black Gate only* intrinsic, but Exult has since implemented it in its Serpent Isle support.",
				"[SI, Exult]" => "Please note that in the original usecode, this was a *Serpent Isle only* intrinsic, but Exult has since implemented it in its Black Gate support."
				);
			if (array_key_exists($text, $namemap))
				$name = $namemap[$text];
			else
				$name = "This is an error; go complain to Marzo about it.";
    		$txt = "// " . wordwrap("$name", $linelen - 3, "\n// ") . "\n//\n$para";
		}
    	else
    	{
    		$txt = explode("\n", $para);
    		$mlen = $linelen-strlen($text)-8;
    		if (is_array($txt))
    			$fline = $txt[0];
        	else
    			$fline = $txt;
    		if (strlen($fline) > $mlen)
    		{
    			$lines = wordwrap($fline, $mlen, "\n" . str_pad("", 8));
    			$lines = explode("\n", $lines);
				$lines[0] = str_pad($lines[0], $linelen-strlen($text)) . $text;
				$fline = implode("\n", $lines);
    		}
    		else
				$fline = str_pad($fline, $linelen-strlen($text)) . $text;
			
    		if (is_array($txt))
    		{
    			$txt[0] = $fline;
    			foreach ($txt as $i => $line)
    			{
    				if ($i > 0)
						$txt[$i] = wordwrap($line, $linelen, "\n" . str_pad("", 8));
    			}
    			$txt = implode("\n", $txt);
    		}
    		else
    			$txt = $fline;
    		$txt .= ($out ? "\n\n" : "");
    	}
    	if ($out)
    		out($txt);
    	else
			return $txt;
    }
    
    function add_list($items, $bullet = false, $offset = 8, $out = true)
    {
    	global $outmode, $linelen;
    	if (!$outmode)
    	{
    		$tag = $bullet ? "ul" : "ol";
			$type = ($bullet === -1) ? "; list-style-type: none" : "";
    		$left = $offset/4 . "em";
    		$list = "<$tag  style=\"margin-left: $left$type\">";
    		foreach($items as $line)
    			$list .= "<li>$line</li>";
    		$list .= "</$tag>";
    	}
    	else if ($outmode == 2)
    	{
    		$ident = $bullet ? (($bullet === -1) ? 3 : 3) : 7;
    		$img = ($bullet === -1) ? "// * " : "// * ";
    		foreach($items as $j => $line)
    		{
    			$num = $j + 1;
    			$indicator = str_pad($bullet ? $img : "// $num - ", $ident);
    			$lineout = "$indicator$line\n";
        		$list .= $lineout;
    		}
            $list .= "//\n";
    	}
    	else
    	{
    		$ident = $bullet ? (($bullet === -1) ? 0 : 2) : 6;
    		$img = ($bullet === -1) ? "" : "*";
    		foreach($items as $j => $line)
    		{
    			$num = $j + 1;
    			$indicator = str_pad($bullet ? $img : "($num)", $ident);
    			$lineout = wordwrap($indicator . $line, $linelen-$ident-$offset);
        		$list .= str_pad("", $offset) . 
    					str_replace("\n", "\n" . str_pad("", $offset+$ident), $lineout) . "\n";
    		}
            $list .= "\n";
    	}
    	if ($out)
    		out($list);
    	else
			return $list;
    }
    
    function add_2dlist($items, $cols = 4, $group = 2, $offset = 8, $out = true)
    {
    	global $outmode, $linelen;
    	if (!$outmode)
    	{
    		// Much easier
    		$lastindex = count($items) - 1;
    		$lastmember = $group - 1;
    		$cols += $group;
    		$left = $offset/2 . "em";
    		$list = "<table style=\"margin-left: $left\" border=\"1\"><tbody><tr>";
    		foreach($items as $j => $item)
    		{
    			if ($j > 0 && $j != $lastindex && $j % $cols == 0)
    				$list .= "</tr><tr>";
    			if ($j % $group == 0)
	    			$list .= "<td>";
	    		else if ($group != 1)
	    			$list .= " ";
    			$list .= $item;
    			if ($j % $group == $lastmember)
	    			$list .= "</td>";
    		}
    		$list .= "</tr></tbody></table>";
    	}
    	else if ($outmode == 2)
    	{
    		// Moderate difficulty.
    		$lastcol = $group - 1;
    		foreach($items as $j => $item)
    		{
    			$col = $j % $group;
    			if ($col == 0)
                	$list .= "// $item -";
				else
					$list .= " $item";	// Needs work.
				if ($col == $lastcol)
					$list .= "\n";
    		}
            $list .= "//\n";
    	}
    	else
    	{
    		// Much harder
    		// Determine the number of spaces that we'll need to align the table.
    		for ($i = 0; $i < $cols; $i++)
    			$lens[] = 0;
    		foreach($items as $j => $item)
    		{
    			$col = $j % $cols;
    			if (strlen($item) > $lens[$col])
    				$lens[$col] = strlen($item);
    		}
    		$list = str_pad("", $offset);
    		$lastcol = $cols - 1;
    		foreach($items as $j => $item)
    		{
    			$col = $j % $cols;
    			if ($j > 0 && $j != $lastindex && $col == 0)
    				$list .= "\n" . str_pad("", $offset);
    			if ($col < $lastcol)
					$list .= str_pad($item, $lens[$col]+4);
                else
                	$list .= $item;
    		}
            $list .= "\n\n";
    	}
    	if ($out)
    		out($list);
    	else
			return $list;
    }
    
    function table($rows, $offset = 8, $border = 1, $out = true)
    {
    	global $outmode, $linelen;
    	if (!$outmode)
    	{
    		// Much easier
    		$left = $offset/2 . "em";
    		$table = "<table style=\"margin-left: $left\" border=\"$border\"><tbody>";
    		foreach($rows as $row)
    		{
    			$table .= "<tr>";
    			foreach($row as $col)
        			$table .= "<td>$col</td>";
    			$table .= "</tr>";
    		}
    		$table .= "</tbody></table>";
    	}
    	else if ($outmode == 2)
    	{
    		// Moderate difficulty.
    		$lastcol = count($rows[0]) - 1;
    		foreach($rows as $i => $row)
    		{
    			if ($i != 0)
    			{
    				foreach($row as $col => $item)
    				{
    					$coltxt = preg_replace('/\s+=\s+/', ' - ', $item);
		    			$table .= "// $coltxt\n";
    				}
    			}
    		}
            $table .= "//\n";
    	}
    	else
    	{
    		// Much harder
    		$cols = count($rows[0]);
    		// Determine the number of spaces that we'll need to align the table.
    		for ($i = 0; $i < $cols; $i++)
    			$lens[] = 0;
    		$cols -= 1;
    		foreach($rows as $row)
    			foreach($row as $j => $col)
        			if (strlen($col) > $lens[$j] && $j < $cols)
        				$lens[$j] = strlen($col);
        	foreach($lens as $len)
        		$lastpos += $len + 4;
        	$lastpos -= $lens[$cols] + 4;
    		// Not output the table.
    		foreach($rows as $row)
    		{
				$table .= str_pad("", $offset);
    			foreach($row as $j => $col)
    			{
    				if ($j < $cols)
        				$table .= str_pad($col, $lens[$j]+4);
					else
					{
						$lines = wordwrap($col, $linelen - $lastpos - $offset, "\n" . str_pad("", $lastpos + $offset));
						$table .= $lines;
					}
				}
				$table .= "\n";
			}
            $table .= "\n";
    	}
    	if ($out)
    		out($table);
    	else
			return $table;
    }
     
    function add_keylist($items, $offset = 8, $out = true)
    {
    	global $outmode, $linelen;
    	if (!$outmode)
    	{
    		// Much easier
    		$left = $offset/2 . "em";
    		$list = "<table style=\"margin-left: $left\" border=\"$border\"><tbody>";
    		foreach($items as $i => $col)
    		{
    			if ($i % 2 == 0)
	    			$list .= "<tr>";
       			$list .= "<td>$col</td>";
    			if ($i % 2 == 1)
	    			$list .= "</tr>";
    		}
    		$list .= "</tbody></table>";
    	}
    	else if ($outmode == 2)
    		$list = add_2dlist($items, 2, 2, $offset, false);
    	else
    	{
    		// Much harder
    		// Determine the number of spaces that we'll need to align the table.
    		for ($i = 0; $i < 2; $i++)
    			$lens[] = 0;
    		foreach($items as $j => $item)
    		{
    			$col = $j % 2;
    			if (strlen($item) > $lens[$col])
    				$lens[$col] = strlen($item);
    		}
    		$len0 = $lens[0] + 4;
    		$len1 = $offset + $len0;
    		$off0 = str_pad("", $offset);
    		$off1 = "\n" . str_pad("", $len1);
    		$list = "";
    		foreach($items as $j => $item)
    		{
    			$col = $j % 2;
    			if ($col == 0)
					$list .= $off0 . str_pad($item, $len0);
                else
                	$list .= wordwrap($item, $linelen - $len1, $off1) . "\n";
    		}
    		if ($out)
            	$list .= "\n";
    	}
    	if ($out)
    		out($list);
    	else
			return $list;
    }
   
    function add_box($rows, $offset = 4, $out = true)
    {
    	global $outmode, $linelen;
    	if (!$outmode)
    	{
    		$left = $offset/2 . "em";
    		$box = "<table style=\"margin-left: $left\" border=\"1\"><tbody>";
    		foreach($rows as $row)
    			$box .= "<tr><td>$row</td></tr>";
    		$box .= "</tbody></table>";
    	}
    	else if ($outmode == 2)
    	{
    		$header = "// $rows[0]:\n// ";
    		$lines = array_slice($rows, 1);
    		$text = implode("\n", $lines);
    		$box = str_pad("", $offset) . wordwrap($text, $linelen-$offset-3);
    		$box = $header . str_replace("\n", "\n// " . str_pad("", $offset), $box);
			$box .= "\n//\n";
    	}
    	else
    	{
    		$header = str_pad($rows[0], strlen($rows[0])+2, " ", STR_PAD_BOTH);
    		$header = str_pad($header, 25, "*", STR_PAD_BOTH) . "\n";
    		$footer = str_pad("END " . $rows[0], strlen($rows[0])+6, " ", STR_PAD_BOTH);
    		$footer = str_pad($footer, 25, "*", STR_PAD_BOTH);
    		$lines = array_slice($rows, 1);
    		$text = implode("\n", $lines);
    		$box = $header . wordwrap($text, $linelen-$offset) . "\n" . $footer;
    		$box = str_pad("", $offset) . 
					str_replace("\n", "\n" . str_pad("", $offset), $box);
			$box .= "\n\n";
    	}
    	if ($out)
    		out($box);
    	else
			return $box;
    }
    
    function out($text)
    {
    	global $tpl;
    	$tpl->assign("TEXT", $text);
    	$tpl->parse("CONTENT", ".any");
    }
?>