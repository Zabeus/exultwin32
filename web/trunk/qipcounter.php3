<?php

if( ! $QLIB_INCLUDED )
	include("qlib.php3");


////////////////
//
// Counter
//
////////////////
class Counter {
	// Counter name:
	var $name;
	
	// Counter data file:
	var $counterFile;

	// Array containing the counter data
	var $counterArray;

	function Counter($counterName, $counterFile = "count.qip") {
		$this->name = trim($counterName);
		$this->counterFile = $counterFile;
		$this->counterArray = array();
	}

	function readCounterFile($fp)
	{
		while (feof($fp) == 0)
		{
			$line = chop(fgets($fp,1000));

			list($key, $value) = split("=", $line);
			
			$this->counterArray[trim($key)] = trim($value);
		}
	} // readcounterfile


	function writeCounterFile($fp)
	{
		reset ($this->counterArray);
		while (list($key, $value) = each ($this->counterArray))
		{
			fwrite ($fp, $key."=".$value."\n");
		} // while
	} // writecounterfile


	function set ($newvalue)
	{
		// Read in the current counter value.
		$fp = qlib_fopen($this->counterFile, "r+", false);
		if ($fp < 0)
			return;
		$this->readCounterFile($fp);

		// Set the new value.
		$this->counterArray[$this->name] = $newvalue;
		
		// Write it out again.
		ftruncate($fp, 0);
		$this->writeCounterFile($fp);
		qlib_fclose($fp);

		return ($newvalue);
	} // set


	function get()
	{
		// Read in all counter values
		$fp = qlib_fopen($this->counterFile, "r", true);
		$this->readCounterFile($fp);	
		qlib_fclose($fp);

		// Check whether this counter already existed and if yes return its value; if not, return 0.
		if (isset($this->counterArray[$this->name]))
			return ($this->counterArray[$this->name]);
		else
			return ($this->counterArray[$this->name] = 0);

	} // get

	function inc()
	{
		// Read in the current counter value.
		$fp = qlib_fopen($this->counterFile, "r+", false);
		if ($fp < 0)
			return;
		$this->readCounterFile($fp);

		if (isset($this->counterArray[$this->name]))
			$this->counterArray[$this->name]++;
		else
			$this->counterArray[$this->name] = 1;

		// Write it out again.
		ftruncate($fp, 0);
		$this->writeCounterFile($fp);
		qlib_fclose($fp);

		return ($this->counterArray[$this->name]);
	} // inc
}


////////////////
//
// PageCounter
//
////////////////
class PageCounter extends Counter {
	// IP data file:
	var $ipFile;

	// Array storing the ip data
	var $ipArray;


	function PageCounter($counterFile = "count.qip", $ipFile = "ipdata.qip") {
		global $PHP_SELF;
		
		$this->Counter($PHP_SELF, $counterFile);

		$this->ipFile = $ipFile;
		$this->ipArray = array();
		
	}
	
	function getAsHTML($asXHTML = false)
	{
		$html = "";
	
		// Size of your digit images:
		$width = "15";
		$height = "13";

		// Extention of digits image files (.jpg or .gif):
		$dig_ext = ".gif";

		// Your root URL:
		//
		// (for example, http://members.host.com/~MyDir/ or
		// http://www.mydomain.com/). Include the slash at the end!
		//$root_url = "http://www.abi-lahnstein.de/";

		// Digits directory:
		//
		// (do not include slash in the begining but put it at the end!)
		$dig_dir = "images/digits/";
		
		$count = $this->get();
		for($i=0; $i<strlen($count); $i++) {
			$num=substr($count,$i,1);
			$html .= "<img src=\"$root_url$dig_dir$dig_b$num$dig_e$dig_ext\" width=\"$width\" height=\"$height\" alt=\"$num\"";
			if ($asXHTML)
				$html .= " />";
			else
				$html .= ">";
		}
		
		return $html;
	}
	
	function getAsXHTML()
	{
		return $this->getAsHTML(true);
	}

	function printIt()
	{
		echo ($this->getAsHTML());
	}

	function readIPAddressFile($fp)
	{
		while (feof($fp) == 0)
		{
			$line = chop(fgets($fp,1000));

			list($key, $value) = split("=", $line);
			
			$this->ipArray[trim($key)] = trim($value);
		}
	} // readIPAddressFile


	function writeIPAddressFile($fp)
	{
		reset ($this->ipArray);
		while (list($key, $value) = each ($this->ipArray))
		{
			fwrite ($fp, $key."=".$value."\n");
		} // while
		return (0);
	} // writeIPAddressFile


	function checkIP ($ip)
	{
		// Read in the current counter value.
		$fp = qlib_fopen($this->ipFile, "r", true);
		$this->readIPAddressFile($fp);
		qlib_fclose($fp);

		// Set the new value.
		if( $ip != $this->ipArray[$this->name] )
		{
			$this->ipArray[$this->name] = $ip;
			
			// Write the IP file out again.
			$fp = qlib_fopen($this->ipFile, "w", false);
			$this->writeIPAddressFile($fp);
			qlib_fclose($fp);
			
			return true;
		}
	
		return false;
	} // checkIP
}

?>