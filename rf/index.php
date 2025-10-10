<?php
if (session_status() === PHP_SESSION_NONE) {
    session_start();
}
include_once "../include/config.php";
?>
<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="UTF-8">
    <link href="../css/css.php" type="text/css" rel="stylesheet" />
<style type="text/css">
body {
  background-color: #eee;
  font-size: 18px;
  font-family: Arial;
  font-weight: 300;
  margin: 2em auto;
  max-width: 40em;
  line-height: 1.5;
  color: #444;
  padding: 0 0.5em;
}
h1, h2, h3 {
  line-height: 1.2;
}
a {
  color: #607d8b;
}
.highlighter-rouge {
  background-color: #fff;
  border: 1px solid #ccc;
  border-radius: .2em;
  font-size: .8em;
  overflow-x: auto;
  padding: .2em .4em;
}
pre {
  margin: 0;
  padding: .6em;
  overflow-x: auto;
}

#player {
    position:relative;
    width:205px;
    overflow: hidden;
    direction: ltl;
}

textarea {
    background-color: #111;
    border: 1px solid #000;
    color: #ffffff;
    padding: 1px;
    font-family: courier new;
    font-size:10px;
}

</style>
</head>
<body style = "background-color: #e1e1e1;font: 11pt arial, sans-serif;">
<script src="web-audio-peak-meter.js"></script>
<center>
<fieldset style = "border:#3083b8 2px groove;box-shadow:5px 5px 20px #999; background-color:#f1f1f1; width:555px;margin-top:15px;margin-left:0px;margin-right:5px;font-size:13px;border-top-left-radius: 10px; border-top-right-radius: 10px;border-bottom-left-radius: 10px; border-bottom-right-radius: 10px;">
<div style = "padding:0px;width:550px;background-image: linear-gradient(to bottom, #e9e9e9 50%, #bcbaba 100%);border-radius: 10px;-moz-border-radius:10px;-webkit-border-radius:10px;border: 1px solid LightGrey;margin-left:0px; margin-right:0px;margin-top:4px;margin-bottom:0px;line-height:1.6;white-space:normal;">
<center>
<h1 id="web-audio-peak-meters" style = "color:#00aee8;font: 18pt arial, sans-serif;font-weight:bold; text-shadow: 0.25px 0.25px gray;">RF Module Configurator (<?php echo DL3EL_RADIO; ?>)</h1>

<?php 

//if ($_SERVER["REQUEST_METHOD"] == "POST") {
//  if (empty($_POST["ssid"])) {
//     echo "Name is required";
//  } else {
//    $ssid = $_POST["ssid"]);
//  }
//}}


// load the connlist
$retval = null;
$conns = null;

// find the gateway
//$ipgw = null;

if (defined('DL3EL_RADIO')) {
  if (($DL3EL_RADIO == "Shari") || ($DL3EL_RADIO == "RFGuru")) {
    $svxRadio = DL3EL_RADIO;
  } else {
    echo "No supported Radio <br>";
    $svxRadio = "";
  }  
} else {
   echo "No Radio <br>";
   exit();
}

$RfConfFile = DL3EL . '/sa818/sa818.json';

if (fopen($RfConfFile,'r'))
{
    $filedata = file_get_contents($RfConfFile);
    $RfData = json_decode($filedata,true);
    if (debug) echo "Data from File: ";
    if (debug) print_r($RfData);
    if (debug) echo "<br>";
    list($txctcss, $rxctcss) = explode(",", $RfData['ctcss']);
    $RfData['txctcss'] = $txctcss;
    $RfData['rxctcss'] = $rxctcss;
    $RfData['txfreq'] = $RfData['freq'];
    if ($RfData['offset'] === 0) {
	$RfData['rxfreq'] = $RfData['txfreq'];
    } else {
	$RfData['rxfreq'] = $RfData['txfreq'] + $RfData['offset'];
    }   
// SA818 works only in simplex mode, RX has to be equal to TX	
    $RfData['rxfreq'] = $RfData['txfreq'];

//  print_r($RfData);
} else {
    $RfData['txctcss'] = "";
    $RfData['rxctcss'] = "";
    $RfData['txfreq'] = "";
    $RfData['offset'] = 0;
    $RfData['port'] = "";
}    
    $radioport = $RfData['port'];

    $command = "perl " . DL3EL . "/sa818/get_shari_hf_data.pl r=1 d=" . DL3EL . " p=" . $radioport;
    if (debug) echo "ShariCall: $command<br>";
    exec($command, $output, $retval);
    $tx = "";
    $rx = "";
    $txctcss = "";
    $squelch = "";
    $rxctcss = "";
    $bandwidth = "";
    $offset = 0;

    list($tx, $rx, $txctcss, $squelch, $rxctcss, $bandwidth) = explode(",", $output[0]);
    if (debug) echo "current data from shari: TX$tx, RX$rx, $txctcss, SQ: $squelch, $rxctcss, BW: $bandwidth <br>";
	
    if ($tx !== "") {
	$RfData['txfreq'] = $tx;
	if ($tx !== $RfData['freq']) {
	    echo "different freq, save the corret one<br>";
	}	
    }    
    if ($rx !== "") {
	$RfData['rxfreq'] = $rx;
    }    
    if ($rxctcss !== "") {
	if ($rxctcss !== $RfData['rxctcss']) {
	    echo "different rxctcss, Shari: $rxctcss, sa818.json: " . $RfData['rxctcss'] . ", save the correct one<br>";
	}	
	$RfData['rxctcss'] = $rxctcss;
    }    
    if ($txctcss !== "") {
	if ($txctcss !== $RfData['txctcss']) {
	    echo "different txctcss, Shari: $txctcss, sa818.json: " . $RfData['txctcss'] . ", save the correct one<br>";
	}	
	$RfData['txctcss'] = $txctcss;
    }    
    if ($squelch !== "") {
	if ($squelch !== $RfData['squelch']) {
	    echo "different squelch, Shari: $squelch, sa818.json: " . $RfData['squelch'] . ", save the correct one<br>";
	}	
	$RfData['squelch'] = $squelch;
    }    
    if ($bandwidth !== "") {
	if ($bandwidth !== $RfData['bw']) {
	    echo "different bandwidth, Shari: $bandwidth, sa818.json: " . $RfData['bw'] . ", save the correct one<br>";
	}	
	$RfData['bw'] = $bandwidth;
    }    
//        print_r($RfData);

$device = device_detection($radioport);
    if (debug) print_r($device);
$screen[0] = "Welcome to SA818 RF MODULE configuration tool.";
$screen[1] = "Please use buttons for actions.";
$screen[2] = "Actions are limited to section data only.";
$screen[3] = "";


if (isset($_POST['btnDetect']))
    {

        $retval = null;
        $screen_top = null;
	$screen = null;
	$screen_small = null;
	
        $port = $_POST['port'];

	$command_top = "ls -1 /dev/ttyS* /dev/ttyUSB* 2>&1";
	exec($command_top,$screen_top,$retval);
	
	//print_r($screen_top); 
	//print_r("<br>");
	$retval = null;
	
	$i = 0;
	foreach ($screen_top as $port_test)
	{ 
		$screen[$i] = "Detection for:" .$port_test; 
		$command = "python3 sa818.py --port \"" .$port_test. "\" version 2>&1";
        	exec($command,$screen_small,$retval);
		
		//print_r($screen_small);
		if (!$retval)
		{
			$port = $port_test;
			$screen[$i] = $screen[$i] . " BINGO !"; 
		}
		$i = $i+1;
	};

}



if (isset($_POST['btnVersion']))
    {
        $retval = null;
        $screen = null;

        $port = $_POST['port'];
        $port = $device[$port];
	if (debug) echo "selected Port: $port<br>";  
        $command = "python3 sa818.py --port \"" .$port. "\" version 2>&1";

	if (!$retval) exec($command,$screen,$retval);
//	echo "<br>SHARI: $retval";
  	//if ($retval) echo("NOK");
	if (!$retval) {
		$RfData['port']=$port;
		$jsonRfData = json_encode($RfData);
        	file_put_contents("sa818.json", $jsonRfData ,FILE_USE_INCLUDE_PATH);
                //archive the current config
//                exec('sudo cp /opt/sa818/sa818.json /opt/sa818/sa818.json.' .date("YmdThis") ,$screen,$retval);
                exec('sudo cp ' . DL3EL . '/sa818/sa818.json ' . DL3EL . '/sa818/sa818.json.' .date("YmdThis") ,$screen,$retval);
                //move generated file to current config
//                exec('sudo mv sa818.json /opt/sa818/sa818.json', $screen, $retval);
                exec('sudo mv sa818.json ' . DL3EL . '/sa818/sa818.json', $screen, $retval);
	}
}



if (isset($_POST['btnRadio']))
    {

        $retval = null;
        $screen = null;
        $port = $_POST['port'];
        $port = $device[$port];
	if (debug) echo "selected Radio Port: $port<br>";  
	$freq = $_POST['freq'];
	$rxfreq = $_POST['rxfreq'];
	$txfreq = $_POST['txfreq'];
// SA818 works only in simplex mode, RX has to be equal to TX	
	$txfreq = $rxfreq;
	$squelch = $_POST['squelch'];
	$ctcss = $_POST['ctcss'];
	$rxctcss = $_POST['rxctcss'];
	$txctcss = $_POST['txctcss'];
	$tail = $_POST['tail'];
	$bw = $_POST['bw'];
	$ctcss = $txctcss;
	if ($rxctcss !== "") {
		$ctcss = $txctcss . "," . $rxctcss;
	}
#        $command = "python3 sa818.py --port \"" .$port. "\" radio --frequency \"" .$freq. "\" --offset \"" .$offset. "\" --squelch \"" .$squelch. "\" --ctcss \"" .$ctcss. "\" --close-tail \"" .$tail. "\" 2>&1";
#        $command = "python3 sa818.py --port \"" .$port. "\" radio --frequency \"" .$freq. "\" --offset \"" .$offset. "\" --squelch \"" .$squelch. "\" --ctcss \"" .$ctcss. "\" --tail \"" .$tail. "\" --bw \"" .$bw. "\" 2>&1";
        $command = "python3 sa818.py --port \"" .$port. "\" radio --frequency \"" .$rxfreq. "\" --txfrequency \"" .$txfreq. "\" --squelch \"" .$squelch. "\" --ctcss \"" .$ctcss. "\" --tail \"" .$tail. "\" --bw \"" .$bw. "\" 2>&1";
//echo $command;
        if (!$retval) exec($command,$screen,$retval);

	if (!$retval) {
//                $RfData['port']=$port;$RfData['rxfreq']=$rxfreq;$RfData['txfreq']=$txfreq;$RfData['offset']=$offset;$RfData['squelch']=$squelch;$RfData['ctcss']=$ctcss;$RfData['tail']=$tail;
                $RfData['port']=$port;$RfData['rxfreq']=$rxfreq;$RfData['txfreq']=$txfreq;$RfData['squelch']=$squelch;$RfData['ctcss']=$ctcss;$RfData['tail']=$tail;
                $jsonRfData = json_encode($RfData);
                file_put_contents("sa818.json", $jsonRfData ,FILE_USE_INCLUDE_PATH);
                //archive the current config
//                exec('sudo cp /opt/sa818/sa818.json /opt/sa818/sa818.json.' .date("YmdThis") ,$screen,$retval);
                exec('sudo cp ' . DL3EL . '/sa818/sa818.json ' . DL3EL . '/sa818/sa818.json.' .date("YmdThis") ,$screen,$retval);
                //move generated file to current config
//                exec('sudo mv sa818.json /opt/sa818/sa818.json', $screen, $retval);
                exec('sudo mv sa818.json ' . DL3EL . '/sa818/sa818.json', $screen, $retval);
        }
}

if (isset($_POST['btnFilters']))
    {

	$retval = null;
        $screen = null;
	$port = $_POST['port'];
        $port = $device[$port];
	if (debug) echo "selected Radio Port: $port<br>";  
        $fEmph = $_POST['fEmph'];
        $fLow = $_POST['fLow'];
        $fHigh = $_POST['fHigh'];

        $command = "python3 sa818.py --port \"" .$port. "\" filters  --emphasis \"" .$fEmph. "\" --lowpass \"" .$fLow. "\" --highpass \"" .$fHigh. "\" 2>&1";
        if (!$retval) exec($command,$screen,$retval);
	        if (!$retval) {
                $RfData['port']=$port;$RfData['fEmph']=$fEmph; $RfData['fLow']=$fLow;$RfData['fHigh']=$fHigh;
                $jsonRfData = json_encode($RfData);
                file_put_contents("sa818.json", $jsonRfData ,FILE_USE_INCLUDE_PATH);
                //archive the current config
//                exec('sudo cp /opt/sa818/sa818.json /opt/sa818/sa818.json.' .date("YmdThis") ,$screen,$retval);
                exec('sudo cp ' . DL3EL . '/sa818/sa818.json ' . DL3EL . '/sa818/sa818.json.' .date("YmdThis") ,$screen,$retval);
                //move generated file to current config
//                exec('sudo mv sa818.json /opt/sa818/sa818.json', $screen, $retval);
                exec('sudo mv sa818.json ' . DL3EL . '/sa818/sa818.json', $screen, $retval);
        }

}




if (isset($_POST['btnVol']))
    {
	
	 $retval = null;
        $screen = null;
        $port = $_POST['port'];
        $port = $device[$port];
	if (debug) echo "selected Radio Port: $port<br>";  
        $volume = $_POST['volume'];

        $command = "python3 sa818.py --port \"" .$port. "\" volume  --level \"" .$volume. "\" 2>&1";
        if (!$retval) exec($command,$screen,$retval);
                if (!$retval) {
                $RfData['volume']=$volume;
                $jsonRfData = json_encode($RfData);
                file_put_contents("sa818.json", $jsonRfData ,FILE_USE_INCLUDE_PATH);
                //archive the current config
//                exec('sudo cp /opt/sa818/sa818.json /opt/sa818/sa818.json.' .date("YmdThis") ,$screen,$retval);
                exec('sudo cp ' . DL3EL . '/sa818/sa818.json ' . DL3EL . '/sa818/sa818.json.' .date("YmdThis") ,$screen,$retval);
                //move generated file to current config
//                exec('sudo mv sa818.json /opt/sa818/sa818.json', $screen, $retval);
                exec('sudo mv sa818.json ' . DL3EL . '/sa818/sa818.json', $screen, $retval);
        }

}


//load json

$port = $RfData['port']; 
if (debug) echo "Port at start: $port <br>";
$rxfreq = $RfData['rxfreq'];$txfreq = $RfData['txfreq'];$ctcss=$RfData['ctcss'];$tail=$RfData['tail'];$squelch=$RfData['squelch'];
$fEmph = $RfData['fEmph'];$fLow=$RfData['fLow'];$fHigh=$RfData['fHigh'];
$volume = $RfData['volume'];
$tail = $RfData['tail'];
$bw = $RfData['bw'];

// default section
// port
#if ($port === "" || is_null($port)) $port = "/dev/ttyS1";
$svxRadio = DL3EL_RADIO;
if ($svxRadio == "Shari") {
  if ($port === "" || is_null($port)) $port = "/dev/ttyUSB0";
}  
if ($svxRadio == "RFGuru") {
  if ($port === "" || is_null($port)) $port = "/dev/ttyS0";
}  

if (debug) echo "Port after start: $port <br>";

//radio
if ($rxfreq === "" || is_null($freq)) $freq = "433.025";
if ($txfreq === "" || is_null($txfreq)) $txfreq = "433.025";
if ($offset === "" || is_null($offset)) $offset = "0.0";
if ($ctcss === "" || is_null($ctcss)) $ctcss = "94.8";
if ($tail === "" || is_null($tail)) $tail = "yes";
if ($squelch === "" || is_null($squelch)) $squelch = "1";
if ($bw === "" || is_null($bw)) $bw = "1";

//filter
if ($fEmph === "" || is_null($fEmph)) $fEmph = "enable";
if ($fLow === "" || is_null($fLow)) $fLow = "enable";
if ($fHigh === "" || is_null($fHigh)) $fHigh = "enable";

//
if ($volume === "" || is_null($volume)) $volume = "8";

?>

<form method="post" action="<?php echo htmlspecialchars($_SERVER["PHP_SELF"]);?>"> 
<DIV style = "height:77px">
<table>
	<tr>
	<th>Screen</th> 
	</tr>
<tr>
<td>
	<textarea name="scan" rows="4" cols="80"><?php 
			echo implode("\n",$screen); ?></textarea>

</td>



</tr>  
</table> 
</DIV>

<table>
        <tr>
        <th width = "380px">Port</th>
	<th width = "100px">Action</th>
        </tr>
<tr>
<td> 
<!--
   <button name="btnDetect" type="submit" class="red"style = "height:30px; width:105px; font-size:12px;">Detect</button> 
	Port: <input type "text" name="port" style = "width: 150px" value="<?php echo $port;?>"
--->
<?php

    echo '<form method="post" action="">';
    echo '<select id="port" name="port" required>';
    $pn = 0;
    $port = $device[0];
    foreach ($device as $portnr) {
	$portLine = "<option value=" . $pn . ">" . $portnr . "</option><br>\n";
	echo $portLine;
	++$pn;
    }
    echo "</select>";
    echo '<button type="submit">Port</button>';
    echo '<input type="hidden" name="port_submitted" value="1">';
    echo "</form>&nbsp;&nbsp;";

    if ($_SERVER["REQUEST_METHOD"] == "POST" && isset($_POST['port_submitted'])) {
        if (isset($_POST['port'])) {
	    $port = $device[$_POST['port']];
	    if (debug) echo "selected Port: $port<br>";  
	}
    }
?>
</TD>
<td>
<button name="btnVersion" type="submit" class="red" style = "height:30px; width:105px; font-size:12px;">Get Version</button>
</TD>
</tr>
</table>

<table>
        <tr>
        <th width = "380px">Radio</th>
        <th width = "100px">Action</th>
        </tr>
<tr>
<td>
   	RxTXFreq: <input type "text" name="rxfreq" style = "width: 60px" value="<?php echo $rxfreq;?>"><br>
<!--
   	TxFreq: <input type "text" name="txfreq" style = "width: 60px" value="<?php echo $txfreq;?>">
	Shift: <input type "text" name="offset" style = "width: 50px" value="<?php echo $offset;?>"> <br>
   	Ctcss: <input type "text" name="ctcss" style = "width: 50px" value="<?php echo $ctcss;?>">
--->
   	RXCtcss: <input type "text" name="rxctcss" style = "width: 50px" value="<?php echo $rxctcss;?>">
   	TXCtcss: <input type "text" name="txctcss" style = "width: 50px" value="<?php echo $txctcss;?>"><br>
	Squelch: <input type "text" name="squelch" style = "width: 50px" value="<?php echo $squelch;?>">
	Tail: <input type "text" name="tail" style = "width: 50px" value="<?php echo $tail;?>">
	Bandwidth: <input type "text" name="bw" style = "width: 50px" value="<?php echo $bw;?>">
</TD>
<td>
<button name="btnRadio" type="submit" class="red" style = "height:30px; width:105px; font-size:12px;">Set Radio</button>
</TD>
</tr>
</table>

<table>
        <tr>
        <th width = "380px">Enable Filters</th>
        <th width = "100px">Action</th>
        </tr>
<tr>
<td>
   Emphasis: <input type "text" name="fEmph" style = "width: 50px" value="<?php echo $fEmph;?>">
   Low: <input type "text" name="fLow" style = "width: 50px" value="<?php echo $fLow;?>">
   High: <input type "text" name="fHigh" style = "width: 50px" value="<?php echo $fHigh;?>">

</TD>
<td>
<button name="btnFilters" type="submit" class="red" style = "height:30px; width:105px; font-size:12px;">Set Filters</button>
</TD>
</tr>
</table>

<table>
        <tr>
        <th width = "380px">Volume</th>
        <th width = "100px">Action</th>
        </tr>
<tr>
<td>
   Volume: <input type "text" name="volume" style = "width: 50px" value="<?php echo $volume;?>">
</TD>
<td>
<button name="btnVol" type="submit" class="red" style = "height:30px; width:105px; font-size:12px;">Set Vol</button>
</TD>
</tr>
</table>

</form>

<p style = "margin: 0 auto;"></p>
<p style = "margin-bottom:-2px;"></p>

</body>
</html>
<?php

function device_detection($radioport) {
        $retval = null;
        $screen_top = null;
	$screen = null;
	$screen_small = null;
	
// RFGuru: /dev/ttyS*
// Shari: /dev/ttyUSB* 

	$command_top = "ls -1 /dev/ttyS* /dev/ttyUSB* 2>&1";
	exec($command_top,$screen_top,$retval);
	
	if (debug) print_r($screen_top); 
	if (debug) print_r("<br>");
	$retval = null;
	
	if ($radioport !== '') {
	    $command = "python3 sa818.py --port \"" .$radioport. "\" version 2>&1";
	    exec($command,$screen_small,$retval);
	    if (!$retval) {
		$screen[0] = $radioport;
		$i = 1;	
	    } else {
		$i = 0;
	    }
	}
	foreach ($screen_top as $port_test)
	{ 
		$screen[$i] = $port_test; 
		$command = "python3 sa818.py --port \"" .$port_test. "\" version 2>&1";
        	exec($command,$screen_small,$retval);
		
		if (debug) print_r($screen_small);
		if (!$retval)
		{
		    $i = $i+1;
		    if (debug) echo "Dev ($i): " . $screen[$i] . "<br>";
		}
	};
    return($screen);
}
?>
