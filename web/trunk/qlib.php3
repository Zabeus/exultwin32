<?php

$QLIB_INCLUDED = TRUE;

function qlib_error($errorMessage,$msgType)
{
/*	global $tpl;
	$tpl->assign(MESSAGE, $errorMessage);
	$tpl->parse(MAIN, ".message");
	$tpl->FastPrint();
*/
	echo($errorMessage);
	exit;
}

// qlib_fopen
//   opens a file $fileName and locks it
function qlib_fopen($fileName, $openMode, $sharing)
{
	global $langOpenError;
	$fp=fopen($fileName, $openMode);
	if ($fp<0) 
		qlib_error("Error opening file ".$fileName."!<br>Make sure writing is allowed in this directory.",2);

	if ($sharing==true)
		$op=1;
	else
		$op=2;

	if (!flock($fp,$op))
	{
		flock($fp,3);
		fclose($fp);
		$fp = -1;
		qlib_error("<i>flock</i> timeout",2);
		exit();
	}
	return $fp;
}


// qlib_fclose
//   unlocks the file $fp and closes it
function qlib_fclose($fp)
{
	flock($fp,3);
	fclose($fp);
}


//
// DB funcs
//

// qlib_DB_read
//   open a DB, and read it into an array of arrays.
//   Specify a list of keynames in the array $keynames.
function qlib_DB_read($filename, $seperator = ",")
{
	$fp = qlib_fopen($filename, "r", true);
	while (feof($fp) == 0)
	{
		$line = chop(fgets($fp,1000));
		// Ignore blank lines, and lines starting with '#' (comments)
		if( $line == "" or $line[0] == '#' )
			continue;
		$arr = split($seperator, $line);
		
		$keyArrayOfValueArrays[$arr[0]] = $arr;
	}
	qlib_fclose($fp);
	
	return $keyArrayOfValueArrays;
}

function qlib_DB_findKey($filename, $key, $seperator = ",")
{
	$fp = qlib_fopen($filename, "r", true);
	while (feof($fp) == 0)
	{
		$line = chop(fgets($fp,1000));
		// Ignore blank lines, and lines starting with '#' (comments)
		if( $line == "" or $line[0] == '#' )
			continue;
		$arr = split($seperator, $line);
		if( $arr[0] == $key )
		{
			qlib_fclose($fp);
			return $arr;
		}
	}
	qlib_fclose($fp);
	
	return array("");
}

function qlib_DB_updateKey($filename, $dataArray, $splitSeperator = ",", $joinSeperator = ",")
{
	$fp = qlib_fopen($filename, "r", false);
	while (feof($fp) == 0)
	{
		$line = chop(fgets($fp,1000));
		// Ignore blank lines, and lines starting with '#' (comments)
		if( $line == "" or $line[0] == '#' )
			continue;
		$arr = split($splitSeperator, $line);
		$keyArrayOfValueArrays[$arr[0]] = $arr;
	}
	fclose($fp);	// Close the file, but keep the lock!
	
	$keyArrayOfValueArrays[$dataArray[0]] = $dataArray;

	// Reopen the file, which is still locked, so no changes could
	// have happened in the meantime
	$fp = fopen($filename, "w+");
	while (list ($key, $valueArray) = each ($keyArrayOfValueArrays))
	{
		// Write it out entry by entry
		fputs($fp, join($joinSeperator, $valueArray) . "\n");
	}
	qlib_fclose($fp);
}

function qlib_DB_removeKey($filename, $key, $seperator = ",")
{
	$fp = qlib_fopen($filename, "r", false);
	while (feof($fp) == 0)
	{
		$line = chop(fgets($fp,1000));
		// Ignore blank lines, and lines starting with '#' (comments)
		if( $line == "" or $line[0] == '#' )
			continue;
		$arr = split($seperator, $line);
		$keyArrayOfValueArrays[$arr[0]] = $arr;
	}
	fclose($fp);	// Close the file, but keep the lock!
	
	// Remove the key
	unset($keyArrayOfValueArrays[$key]);

	// Reopen the file, which is still locked, so no changes could
	// have happened in the meantime
	$fp = fopen($filename, "w+");
	while (list ($key, $valueArray) = each ($keyArrayOfValueArrays))
	{
		// Write it out entry by entry
		fputs($fp, join($seperator, $valueArray) . "\n");
	}
	qlib_fclose($fp);
}


//
//
function qlib_filterUserText($str)
{
	$str = str_replace("\\'", "'", $str);
	$str = str_replace("\\\"", "\"", $str);
	$str = htmlentities($str);
	$str = str_replace("\n", "<br>", $str);
	$str = str_replace("$", "&#036;", $str);

	
	return $str;
}


?>