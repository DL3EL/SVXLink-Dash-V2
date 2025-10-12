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
<!---
<script src="web-audio-peak-meter.js"></script>
-->
<center>
<fieldset style = "border:#3083b8 2px groove;box-shadow:5px 5px 20px #999; background-color:#f1f1f1; width:555px;margin-top:15px;margin-left:0px;margin-right:5px;font-size:13px;border-top-left-radius: 10px; border-top-right-radius: 10px;border-bottom-left-radius: 10px; border-bottom-right-radius: 10px;">
<div style = "padding:0px;width:550px;background-image: linear-gradient(to bottom, #e9e9e9 50%, #bcbaba 100%);border-radius: 10px;-moz-border-radius:10px;-webkit-border-radius:10px;border: 1px solid LightGrey;margin-left:0px; margin-right:0px;margin-top:4px;margin-bottom:0px;line-height:1.6;white-space:normal;">
<center>
<?php 
if (defined('DL3EL_RADIO')) {
  if ((DL3EL_RADIO == "Shari") || (DL3EL_RADIO == "RFGuru")) {
    $svxRadio = DL3EL_RADIO;
  } else {
    echo "No supported Radio defined (valid: Shari/RFGuru)<br>";
    $svxRadio = "";
  }  
} else {
   echo "No Radio defined (valid: Shari/RFGuru)<br>";
   $svxRadio = "";
}
?>
<h1 id="web-audio-peak-meters" style = "color:#00aee8;font: 18pt arial, sans-serif;font-weight:bold; text-shadow: 0.25px 0.25px gray;">RF Module Configurator (<?php echo $svxRadio; ?>)</h1>

<?php 
$RfConfFile = DL3EL . '/sa818/sa818.json';
if (fopen($RfConfFile,'r')) {
    $filedata = file_get_contents($RfConfFile);
    $RfData = json_decode($filedata,true);

    if ((defined ('debug')) && (debug > 10)) echo "Data from File: ";
    if ((defined ('debug')) && (debug > 10)) print_r($RfData);
    if ((defined ('debug')) && (debug > 10)) echo "<br>";
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

    if ((defined ('debug')) && (debug > 0)) echo "sa818.jason:<br>";
    if ((defined ('debug')) && (debug > 0)) print_r($RfData);
    $radioport = $RfData['port'];
    if (substr($radioport,0,8) !== "/dev/tty") {
// saved port not valid      
      echo "saved port not valid: $radioport<br>";
      $RfData['port'] = "";
    } else {  
// check if saved data match radio data
      $command = "perl " . DL3EL . "/sa818/get_shari_hf_data.pl r=1 d=" . DL3EL . " p=" . $radioport;
      if ((defined ('debug')) && (debug > 10)) echo "ShariCall: $command<br>";
      exec($command, $output, $retval);
      $tx = "";
      $rx = "";
      $txctcss = "";
      $squelch = "";
      $rxctcss = "";
      $bandwidth = "";
      $offset = 0;

      list($tx, $rx, $txctcss, $squelch, $rxctcss, $bandwidth) = explode(",", $output[0]);
      if ((defined ('debug')) && (debug > 0)) echo "current data from shari: TX$tx, RX$rx, $txctcss, SQ: $squelch, $rxctcss, BW: $bandwidth <br>";

      if ($tx !== "") {
	$RfData['txfreq'] = $tx;
	if ($tx !== $RfData['freq']) {
	    echo "different freq, save the corret one<br>";
	}	
      }    
      if ($rx !== "") {
	$RfData['rxfreq'] = $rx;
      }    
      if (($rxctcss !== "")  && ($rxctcss !== "None")) {
	if ($rxctcss !== $RfData['rxctcss']) {
	    echo "different rxctcss, Shari: $rxctcss, sa818.json: " . $RfData['rxctcss'] . ", save the correct one<br>";
	}	
	$RfData['rxctcss'] = $rxctcss;
      }    
      if (($txctcss !== "")  && ($txctcss !== "None")) {
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
    }
} else {
// no config File, try to find the radio
    $device = device_detection("-9");
    echo "found device?<br>";
    print_r($device);
    $radioport = $device[0];
    if (substr($radioport,0,8) == "/dev/tty") {
      echo "found it!!: [$radioport]<br>";
      $RfData['port'] = $radioport;
      $command = "perl " . DL3EL . "/sa818/get_shari_hf_data.pl r=1 d=" . DL3EL . " p=" . $radioport;
      if ((defined ('debug')) && (debug > 0)) echo "ShariCall: $command<br>";
      exec($command, $output, $retval);
      $tx = "";
      $rx = "";
      $rxctcss = "";
      $txctcss = "";
      $squelch = 1;
      $rxctcss = "";
      $bandwidth = "";
      $offset = 0;
      $volume = 6;
      $fEmph = "disable";
      $fLow = "disable";
      $fHigh = "enable";
      $tail = "close";
      list($tx, $rx, $txctcss, $squelch, $rxctcss, $bandwidth) = explode(",", $output[0]);
      if ((defined ('debug')) && (debug > 0)) echo "current data from shari: TX$tx, RX$rx, $txctcss, SQ: $squelch, $rxctcss, BW: $bandwidth <br>";
      $RfData['freq'] = $tx;
      $RfData['txfreq'] = $tx;
      $RfData['rxfreq'] = $rx;
      $RfData['rxctcss'] = $rxctcss;
      $RfData['txctcss'] = $txctcss;
      $ctcss = $txctcss;
      if ($rxctcss !== "") {
	$ctcss = $txctcss . "," . $rxctcss;
      }
      $RfData['squelch'] = $squelch;
      $RfData['bw'] = $bandwidth;
      $RfData['fEmph'] = $fEmph;
      $RfData['fLow'] = $fLow;
      $RfData['fHigh'] = $fHigh;
      $RfData['ctcss']=$ctcss;
      $RfData['tail']=$tail;
      $RfData['offset']=$offset;
      $RfData['volume']=$volume;

      if ((defined ('debug')) && (debug > 0)) print_r($RfData);
// save init config file
      $jsonRfData = json_encode($RfData);
      file_put_contents("sa818.json", $jsonRfData ,FILE_USE_INCLUDE_PATH);
      //move generated file to current config
      exec('sudo mv sa818.json ' . DL3EL . '/sa818/sa818.json', $screen, $retval);
    } else {  
      $port = substr($radioport,0,8);
      echo "not found!!: [$radioport]:[$port] <br>";
      $RfData['port'] = "";
      $RfData['txctcss'] = "67.0";
      $RfData['rxctcss'] = "67.0";
      $RfData['txfreq'] = "433.025";
      $RfData['freq'] = "433.025";
      $RfData['offset'] = 0;
      $RfData['fEmph'] = "disable";
      $RfData['fLow'] = "disable";
      $RfData['fHigh'] = "enable";
      $RfData['tail'] = "close";
      $RfData['bw'] = "1";
      $RfData['squelch'] = "1";
      $RfData['volume'] = "6";
    }  
}    
if ($RfData['port'] == "") {
  $RfData['port'] = ($svxRadio === "Shari")? "/dev/ttyUSB0" : "/dev/ttyS0";
  $radioport = $RfData['port'];
} else {
  $radioport = $RfData['port'];
}  
//
// test, hier was ungültiges eintragen und prüfen, ob etwas gültiges gefunden wird
// getestet wird, ob eine Frequezu zurückgeliefert wird
//$radioport = "/dev/ttyS0";
//
//        print_r($RfData);
// Devicetabelle mit verfügabren Ports füllen
$device[0] = $radioport;
if ((defined ('debug')) && (debug)) echo "device_detection with current port: $radioport<br>";
$device = device_detection($radioport);
if ((defined ('debug')) && (debug > 10)) print_r($device);
// Ports zählen
$pn = 0;
foreach ($device as $portnr) {
  ++$pn;
}
if ((defined ('debug')) && (debug)) echo "Port at start: $device[0] of $pn <br>";
$port = $device[0]; 

$screen[0] = "Welcome to SA818 RF MODULE configuration tool.";
$screen[1] = "Please use buttons for actions.";
$screen[2] = "Actions are limited to section data only.";
$screen[3] = "Port in use: " . $device[0] ;


if (isset($_POST['btnDetectn'])) {
  $device = device_detection($radioport);
}

if (isset($_POST['btnSave']))
    {

        $retval = null;
        $screen = null;
echo "btnSave: <br>";
        $port = $_POST['port'];
	if ((defined ('debug')) && (debug > 0)) echo "0 selected Port: $port<br>";  
        $port = $device[$port];
      if (debug) print_r($device);
	if ((defined ('debug')) && (debug > 0)) echo "1 selected Port: $port<br>";  
        $command = "python3 sa818.py --port \"" .$port. "\" version 2>&1";
	if ((defined ('debug')) && (debug > 0)) echo "testing with $command<br>";  

	exec($command,$screen,$retval);
	if (!$retval) {
	if ((defined ('debug')) && (debug > 0)) echo "selected Port works: $port<br>";  
//		$RfData['port']=$port;
		$jsonRfData = json_encode($RfData);
        	file_put_contents("sa818.json", $jsonRfData ,FILE_USE_INCLUDE_PATH);
                //archive the current config
                exec('sudo cp ' . DL3EL . '/sa818/sa818.json ' . DL3EL . '/sa818/sa818.json.' .date("YmdThis") ,$screen,$retval);
                //move generated file to current config
                exec('sudo mv sa818.json ' . DL3EL . '/sa818/sa818.json', $screen, $retval);
	}
}
if (isset($_POST['btnVersion']))
    {
        $retval = null;
        $screen = null;

        $port = $_POST['port'];
	if ((defined ('debug')) && (debug > 0)) echo "using port: $port<br>";  
	if ((defined ('debug')) && (debug > 10)) print_r($device);
        $command = "python3 sa818.py --port \"" .$port. "\" version 2>&1";

	if (!$retval) exec($command,$screen,$retval);
//	echo "<br>SHARI: $retval";
  	//if ($retval) echo("NOK");
/*
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
*/
}

if (isset($_POST['btnRadio']))
    {
        $retval = null;
        $screen = null;
	if ((defined ('debug')) && (debug > 10)) echo "selected Radio Port: $port<br>";  
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
	if ((defined ('debug')) && (debug > 10)) echo $command;
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
	echo "new Data saved to device on $port, please reload page<br>";  
}

if (isset($_POST['btnFilters']))
    {

	$retval = null;
        $screen = null;
	if ((defined ('debug')) && (debug > 10)) echo "selected Radio Port: $port<br>";  
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
	echo "new filter settings saved to device on $port, please reload page<br>";  

}




if (isset($_POST['btnVol']))
    {
	
	$retval = null;
        $screen = null;
	if ((defined ('debug')) && (debug > 10)) echo "selected Radio Port: $port<br>";  
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
	echo "new volume saved to device on $port, please reload page<br>";  
}


//load json

$rxfreq = $RfData['rxfreq'];$txfreq = $RfData['txfreq'];$ctcss=$RfData['ctcss'];$tail=$RfData['tail'];$squelch=$RfData['squelch'];
$fEmph = $RfData['fEmph'];$fLow=$RfData['fLow'];$fHigh=$RfData['fHigh'];
$volume = $RfData['volume'];
$tail = $RfData['tail'];
$bw = $RfData['bw'];

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

   <button name="btnDetectn" type="submit" class="red"style = "height:30px; width:105px; font-size:12px;">Detect</button> 
	Port: <input type "text" name="port" style = "width: 150px" value="<?php echo $port;?>"
</TD>
<td>
<button name="btnVersion" type="submit" class="red" style = "height:30px; width:105px; font-size:12px;">Get Version</button>
</TD></tr>
</form>
<tr>
<td> 

<?php
    if ($pn > 1) {
      $pn = 0;
      $port = $device[0];
      echo '<form method="post" action="">';
      echo '<select id="port" name="port" required>';
      foreach ($device as $portnr) {
	  $portLine = "<option value=" . $pn . ">" . $portnr . "</option><br>\n";
	  echo $portLine;
	  if (!$pn) {
	    echo "<option value=" . $pn . ">------</option><br>\n";
	  }  
	  ++$pn;
      }
      echo "</select>";
      echo " Einträge in Liste: $pn ";
      echo '</TD><TD><button type="submit" class="red" style = "height:30px; width:105px; font-size:12px;">save Port</button></TD>';
      echo '<input type="hidden" name="port_submitted" value="1">';
      echo "</form>&nbsp;&nbsp;";

      if ($_SERVER["REQUEST_METHOD"] == "POST" && isset($_POST['port_submitted'])) {
        if (isset($_POST['port'])) {
	  $port = $device[$_POST['port']];

	  if ((defined ('debug')) && (debug > 0)) echo "selected Port: $port <br>";  
		$RfData['port']=$port;
		echo "</tr><td>new Port $port, saving .. ";
		$jsonRfData = json_encode($RfData);
        	file_put_contents("sa818.json", $jsonRfData ,FILE_USE_INCLUDE_PATH);
                //archive the current config
                exec('sudo cp ' . DL3EL . '/sa818/sa818.json ' . DL3EL . '/sa818/sa818.json.' .date("YmdThis") ,$screen,$retval);
                //move generated file to current config
                exec('sudo mv sa818.json ' . DL3EL . '/sa818/sa818.json', $screen, $retval);
		echo "done, please reload page </td><td></td>";
	}
      } 
    }
?>
</tr>
</table>
<form method="post" action="<?php echo htmlspecialchars($_SERVER["PHP_SELF"]);?>"> 
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
	
	if ((defined ('debug')) && (debug>10)) echo "Output ls -1 /dev/ttyS* /dev/ttyUSB*<br>"; 
	if ((defined ('debug')) && (debug>10)) print_r($screen_top); 
	if ((defined ('debug')) && (debug>10)) echo "<br>";
	$retval = null;
	
	$i = 0;
	if ($radioport !== '') {
	  if ((defined ('debug')) && (debug>0)) echo " test port $radioport mit Version<br>"; 
	  if ($radioport == '-9') {
	    echo "trying to find the raido and stop on first hit<br>";
	  } else {
	    $command = "python3 sa818.py --port \"" .$radioport. "\" version 2>&1";
	    exec($command,$screen_small,$retval);
	    if (!$retval) {
	      $screen[0] = $radioport;
	      $i = 1;	
	    } else {
	      echo "given port is invalid<br>";
	      $radioport = "-1";
	    } 
	    if ((defined ('debug')) && (debug>10)) echo " get defined port from $command<br>"; 
	    if ((defined ('debug')) && (debug>10)) print_r($screen_small);
	    if ((defined ('debug')) && (debug>10)) echo "<br>";
	  }
	}    
	foreach ($screen_top as $port_test)
	{ 
		$screen[$i] = $port_test; 
		$command = "python3 sa818.py --port \"" .$port_test. "\" version 2>&1";
        	exec($command,$screen_small,$retval);
		
		if ((defined ('debug')) && (debug>10)) echo " ($i) Output from $command<br>"; 
		if ((defined ('debug')) && (debug>10)) print_r($screen_small);
		if ((defined ('debug')) && (debug>10)) echo "<br>";
		if (!$retval) {
		    if ((defined ('debug')) && (debug>10)) echo "Portfile: $PortFile adding: $screen[$i]<br>"; 
		    if ((defined ('debug')) && (debug>10)) echo "Dev ($i): " . $screen[$i] . "<br>";
		    if ($radioport == '-9') {
		      break;
		    } else { 
		      $i = $i+1;
		    }  
		}
	};
    if (($radioport == '-1') && (!$i)) {
      echo "no Radio found<br>";
      $screen[0] = "no Radio found";
    }  
    return($screen);
}
?>
