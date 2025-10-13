<?php
if (session_status() === PHP_SESSION_NONE) {
    session_start();
}
include_once "config.php";         
include_once "tools.php";        
include_once "functions.php";    
include_once "tgdb.php";    
?>
<span style = "font-weight: bold;font-size:14px;">SVXReflector Activity</span>
<fieldset style = " width:550px;box-shadow:5px 5px 20px #999;background-color:#e8e8e8e8;margin-top:10px;margin-left:0px;margin-right:0px;font-size:12px;border-top-left-radius: 10px; border-top-right-radius: 10px;border-bottom-left-radius: 10px; border-bottom-right-radius: 10px;">
  <form method="post">
  <table style = "margin-top:0px;">
    <tr height=25px>
      <th>Time (<?php echo date('T')?>)</th>
      <th width=100px>Callsign</th>
<?php
// prüfen ob tgdb.php aktualisiert werden muss
      $tgdb_File = DL3EL_BASE . "include/tgdb.php";
      if (file_exists($tgdb_File)) {
	$tgdb_File_save = DL3EL_BASE . "include/tgdb.php.save";
	$update_script = DL3EL_BASE . "include/tgdb_update.sh";
	$logfile = DL3EL_BASE . "include/tgdb_update.log";
	update_file($tgdb_File, $update_script, $logfile, 86400);
      } else {
	echo "TGDB: $tgdb_File does not exist<br>";
      }
// Suche Name zum Call in DMRIds.dat, prüfen ob id Datei vorhanden und Inhalt > 1MB, dann Überschrift einblenden
      if (file_exists("/var/lib/mmdvm/DMRIds.dat")) {
	$DMRIDFile = "/var/lib/mmdvm/DMRIds.dat";
      } else {
	$DMRIDFile = DL3EL . "/DMRIds.dat";
	$DMRIDFile_save = DL3EL . "/DMRIds.dat.save";
	$update_script = DL3EL .'/DMRID_update.sh ';
	$update_script = DL3EL . "/DMRID_update.sh";
	$logfile =  DL3EL .'/DMRID_update.log';
	update_file($DMRIDFile, $update_script, $logfile, 86400);
      }  
      $dmrIDline = file_get_contents($DMRIDFile);
      IF (strlen($dmrIDline) > 1000000) {
	echo '<th>Name</th>';
	$use_names = 1;
	$command = "sudo cp -p " . $DMRIDFile $DMRIDFile_save 2>&1";
	exec($command,$screen,$retval);
      } else {
	$use_names = 0;
      }
     
?>
      <th width=100px>TG #</th>
	<th width=30px> M </th>
	<th width=30px> A </th>
      <th>TG Name</th>
    </tr>
<?php
$i = 0;
for ($i = 0;  ($i <= 20); $i++) { //Last 20 calls
  if (isset($lastHeard[$i])) {
    $listElem = $lastHeard[$i];
    if ( $listElem[1] ) {
//echo "Timestamp: $listElem[0] Länge " . strlen($listElem[0]) . "<br>";		  
      if (isset($svxconfig['GLOBAL']['TIMESTAMP_FORMAT'])) {

//        $local_time = substr($listElem[0],-16); }
	$local_time = substr($listElem[0],-strlen($listElem[0])); 
      } else {
	$local_time = substr($listElem[0],-8); 
      }
	$local_time = substr($listElem[0],-8); 
        //$local_time = date("%e F Y", strtotime('2010-01-08'))
      echo"<tr height=24px style=\"font-size:12.5px;>\">";
      echo"<td align=\"center\"> $local_time </td>";
      if ($listElem[3] == "OFF" ) {$bgcolor="";}
//      if ($listElem[3] == "ON" ) {$bgcolor=""; $tximg="<img src=images/tx.gif height=21 alt='TXing' title='TXing' style=\"vertical-align: middle;\">";}
      if ($listElem[3] == "ON" ) {$bgcolor="style=\"background-color:#00FF00;\"";}
      $ref = substr($listElem[1],0,3);
      $call=$listElem[1];
      $ssid = strpos($listElem[1],"-");
      if ((!preg_match('/[A-Za-z].*[0-9]|[0-9].*[A-Za-z]/', $listElem[1]) or $ref=="XLX" or $ref=="YSF" or $ref=="M17" or substr($listElem[1],0,3)=="TG" )) {
	echo "<td $bgcolor align='left' valign='middle' class=mh_call>&nbsp;&nbsp;<b>$listElem[1]</b>&nbsp;</td>";
      } else {
	if ($ssid){
	  $call = substr($listElem[1],0,$ssid);
	}
	echo "<td $bgcolor align=\"left\">&nbsp;&nbsp;<a href=\"http://www.qrz.com/db/".$call."\" target=\"_blank\" class=\"qrz_link\"><b>$listElem[1]</b></a>&nbsp;</td>";
      }
//////////////////// NEU //////////////Add Name ///////////////////////////////////////////////////////////////////////////////////////
      if ($use_names) {
	//Aufbereitung LastCall zur Suche in DMRIds.dat (nntfernt Bindestrich)
	$string = $call;
	$position = strpos($string, "-");		// Stelle des "-" zurück
	if ($position !== false) {			// Prüft, ob das Zeichen gefunden wurde
	  $call = substr($string, 0, $position);}	//schneidet ab "-" ab

	  // Suche Name zum Call in DMRIds.da
	  $arr2 = $call;
	  $pos = strpos($dmrIDline, $arr2." ");
	  if ($pos != false) {
	    $name = substr($dmrIDline, ($pos + strlen($arr2." ")));
	    $name = ltrim($name, " ");
	    $x = strpos($name, "\n");
	    $y = strpos($name, " ");
	    $name = rtrim($name, " ");
	    if ($x < $y) {
	      $name = substr($name, 0, $x);
	      echo "<td align=\"left\" style=\"font-weight:bold;color:#464646;\">&nbsp;<b>".$name."&nbsp;</b></td>";
	    } else {
	      $name = substr($name, 0, $y);
	      echo "<td align=\"left\" style=\"font-weight:bold;color:#464646;\">&nbsp;<b>".$name."&nbsp;</b></td>";
	    }
	  } else {
	    echo "<td align=\"left\" style=\"font-weight:bold;color:#464646;\">&nbsp;<b>&nbsp;</b></td>";
/*
	    // Check Relais Calls
	    $string = $call;
	    $position = strpos($string, "0");       	// Stelle der "0" im Rufzeichen
	    if ($position !== false) {              	// Prüft, ob die "0" gefunden wurde
	      // echo "$call ist Relais $string ($name)<br>";
	      $relais = substr($string, 0, $position);
	    }	//schneidet ab der "0" ab
	    if ($relais === "DB") {
	      echo "<td align=\"center\" style=\"font-size:12px;font-weight:bold;color:#005500;\"><b>&nbsp;</b></td>";
	    } elseif ($relais === "DM") {
		echo "<td align=\"center\" style=\"font-size:12px;font-weight:bold;color:#005500;\"><b>&nbsp;</b></td>";
	    } else {echo "<td align=\"left\" style=\"font-weight:bold;color:red;\"><b>&nbsp;&nbsp;</b></td>";}
*/
	  }
	}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////      //echo "<td align=\"left\">&nbsp;<span style=\"color:#b5651d;font-weight:bold;\">$listElem[2]</span></td>";
      $tgnumber = substr($listElem[2],3);
      $name=isset($tgdb_array[$tgnumber])? $tgdb_array[$tgnumber] : '';
      echo "<td align=\"left\">&nbsp;<span style=\"color:#b5651d;font-weight:bold;\">$tgnumber</span></td>";
      echo "<td><button type=submit id=jumptoM name=jmptoM class=monitor_id value=\"$listElem[2]\"><i class=\"material-icons\"style=\"font-size:12px;\">$listElem[3]</i></button></td>";
      //echo "<td onlick='monitorTmpTG(".$tgnumber.")'> M </a></td>";
      //echo "<td><button> T </button></td>";
      echo "<td><button type=submit id=jumptoA name=jmptoA class=active_id value=\"$listElem[2]\"><i class=\"material-icons\"style=\"font-size:12px;\">$listElem[4]</i></button></td>";
      //$tgnumber = substr($listElem[2],3);
      //$name=$tgdb_array[$tgnumber];

      if ( $name==""){ $name ="------";}
      if ( $tgnumber>=400 and $tgnumber<= 499){ $name ="AUTO QSY";}
      echo "<td style=\"font-weight:bold;color:#464646;\">&nbsp;<b>".$name."</b></td>";
      echo"</tr>\n";
    }
  }
}
?>
  </table></form>
</fieldset>
