<?php
if (session_status() === PHP_SESSION_NONE) {
    session_start();
}
include_once "config.php";         
include_once "tools.php";        
include_once "functions.php";

?>
<?php
   if ((defined ('DL3EL_APRS_MSG')) && (DL3EL_APRS_MSG === "yes")) {
      $aprs_script = shell_exec("pgrep aprs-is-msg.pl");
      if (!strlen($aprs_script)) {
         $debug = "";
         if ((defined ('debug')) && (debug > 0)) $debug = "v=" . debug . " ";
         $dbversionFile = DL3EL . "/dbversion";
         $dbversion = file_get_contents($dbversionFile);
         if (file_exists('/etc/systemd/system/svxlink-node.service')) {
            $dbversion = $dbversion . "(s)(a)";
         }  
         $dbversion = " db=\"" . $dbversion . "\"";
         /* Radio */
         $radioinfo = "";
         $radioqrg = "";
         $radioctcss = "";
         if (defined('DL3EL_RADIO')) {
            $svxRadio = DL3EL_RADIO;
            if (($svxRadio == "Shari") || ($svxRadio == "RFGuru")) {
               $RfConfFile = DL3EL . '/sa818/sa818.json';
               if (fopen($RfConfFile,'r')) {
                  $filedata = file_get_contents($RfConfFile);
                  $RfData = json_decode($filedata,true);
//                  $radioqrg = " qrg=\"" . $RfData['rxfreq'] . "\"";
//                  $radioctcss = " rct=\"" . $RfData['rxctcss'] . "\"";
                  $radioinfo = " ri=\"(" . $RfData['rxfreq'] . "MHz, T" . $RfData['rxctcss'] . ")\"";
               } 
            } else {
               $radioinfo = " ri=\"(" . $svxRadio . ")\"";
            }   
         }
//         $cmd = DL3EL . "/aprs-is-msg.pl " . $debug . "c=" . $callsign . $dbversion . $radioqrg . $radioctcss . " >/dev/null &";
         $cmd = DL3EL . "/aprs-is-msg.pl " . $debug . "c=" . $callsign . $dbversion . $radioinfo . " >/dev/null &";
         echo "Starting APRS " . $debug;
         exec($cmd, $output, $retval);
         $logtext =  "APRS Dienst gestartet " . $cmd . " /" . $debug . "\n";
         addsvxlog($logtext);
      } else {
         if ((defined ('debug')) && (debug > 0)) echo "APRS: [" . $aprs_script . "]<br>";
      }   
      $aprsmsg = DL3EL . "/aprs-is.msg.neu";
      if (file_exists($aprsmsg)) {
         // Erweitertes CSS f r maximale Aufmerksamkeit
         echo '<style>
            .blink-msg {
                color: blue;
                font-weight: bold;
                text-decoration: none;
                padding: 2px 5px;
                border-radius: 3px;
                animation: attention 0.8s linear infinite;
            }
            @keyframes attention {
                0% { color: blue; background-color: transparent; }
                50% { color: white; background-color: red; }
                100% { color: blue; background-color: transparent; }
            }
         </style>';
         echo '<a href="./aprs.php" class="blink-msg" id="msg">!!! Neue APRS Nachricht !!!</a><br>';
         echo '<a href="./caller.php?id=aprs" class="blink-msg" id="msg">!!! Neue APRS Nachricht !!!</a><br>';
      }   
	} 
?>

<div style = "width:180px;"><span style = "font-weight: bold;font-size:14px;">SVXLink Info</span></div>
<fieldset style = "width:170px;background-color:#e8e8e8e8;margin-top:6px;;margin-bottom:0px;margin-left:0px;margin-right:3px;font-size:12px;border-top-left-radius: 10px; border-top-right-radius: 10px;border-bottom-left-radius: 10px; border-bottom-right-radius: 10px;">
<?php

if (isProcessRunning('svxlink')) {
   echo "<table style=\"margin-top:4px;margin-bottom:13px;\">\n";
   echo "<tr><th><span style=\"font-size:12px;\">Active Logics</span></th></tr>\n";
   if ((defined('SVXCONFIG')) && (defined('SVXCONFPATH'))) {
      $svxConfigFile = SVXCONFPATH . "/" . SVXCONFIG;}
   else {
      $svxConfigFile = SVXCONFPATH."/".SVXCONFIG;
   }
   if (fopen($svxConfigFile,'r')) {
      $svxconfig = parse_ini_file($svxConfigFile,true,INI_SCANNER_RAW); 
   }
   $callsign = isset($svxconfig['ReflectorLogic']['CALLSIGN']) ? $svxconfig['ReflectorLogic']['CALLSIGN'] : 'NoCall';     
   $check_logics = explode(",",$svxconfig['GLOBAL']['LOGICS']);
   
 // $inReflectorDefaultLang = explode(",", $svxconfig['ReflectorLogic']['DEFAULT_LANG']);
   $reflectorlogic1 = "";
   $reflectorlogic2 = "";
// we support tewo different reflectorlogis, there have to be matching .tcl files. The first reflectorlogis has to be "ReflectorLogic", the second one "ReflectorLogicxyz"
   foreach ($check_logics as $key) {
      echo "<tr><td style=\"background:#ffffed;\"><span style=\"color:#b5651d;font-weight: bold;\">".$key."</span></td></tr>";
      if (strncmp($key, "ReflectorLogic", 14) === 0) {
         if (strlen($key) == 14) {
            $reflectorlogic1 = $key;
         } else {
            $reflectorlogic2 = $key;
         }    
      }
   }
   echo "</table>\n";
   if ((defined ('debug')) && (debug > 0)) echo "R1:$reflectorlogic1 R2:$reflectorlogic2<br>";
   echo "<table style=\"margin-top:2px;margin-bottom:13px;\">\n";
   if (($check_logics[0]=="RepeaterLogic") && (isset($svxconfig['RepeaterLogic']['MODULES']))) { 
      $modules = explode(",",str_replace('Module','',$svxconfig['RepeaterLogic']['MODULES'])); 
   }
   if (($check_logics[0]=="SimplexLogic") && (isset($svxconfig['SimplexLogic']['MODULES']))) { 
      $modules = explode(",",str_replace('Module','',$svxconfig['SimplexLogic']['MODULES'])); 
   } else {
      //$modules=""; 
      $modecho = "False";
   }
//   $inReflectorDefaultLang = explode(",", $svxconfig[$reflectorlogic1]['DEFAULT_LANG']);

   if ($modules!="") {
      define("SVXMODULES",$modules);
      $admodules = getActiveModules();
      echo "<tr><th><span style=\"font-size:12px;\">Modules Loaded</span></th></tr>\n";
      foreach ($modules as $key) {
         if ($admodules[$key]=="On") {
            $activemod="<td style=\"background:MediumSeaGreen;color:#464646;font-weight: bold;\">";
         } else {
            $activemod="<td style=\"background:#ffffed;;color:#b5651d;font-weight: bold;\">";
         }
         echo "<tr>".$activemod."".$key."</td></tr>";

         if ($key=="EchoLink") {
            $modecho ="True";
         }
      }
      if ((defined ('DL3EL_APRS_MSG')) && (DL3EL_APRS_MSG === "yes")) {
         $aprs_script = shell_exec("pgrep aprs-is-msg.pl");
         if (strlen($aprs_script)) {
// process is running
            $aprs_login_ok =  DL3EL . "/aprs-login.ok";
            $aprs_login = "";
            if (file_exists($aprs_login_ok)) {
               $aprs_login = file_get_contents($aprs_login_ok);
               $aprs_login = "(" . $aprs_login . ")";
               $activemod="<td style=\"background:MediumSeaGreen;color:#464646;font-weight: bold;\">";
            } else {
// login was not successful
               $activemod="<td style=\"background:#ffffed;;color:#b5651d;font-weight: bold;\">";
            }
            echo "<tr>".$activemod."APRS " . $aprs_login . "</td></tr>";
         }
      }
   } else {
      echo "<tr><td style=\"background: #ffffed;\" ><span style=\"color:#b0b0b0;\"><b>No Modules</b></span></td></tr>";
   }
   echo "</table>\n";
//#### neu
if ((defined ('debug')) && (debug > 0)) echo "R1:$reflectorlogic1 R2:$reflectorlogic2<br>";
if ($reflectorlogic1 != "") {
// Achtung: wenn jemand in der svxlink.con nur HOSTS = schreibt, funktioniert das nicht.
   $fmnetwork_all = isset($svxconfig[$reflectorlogic1]['HOSTS']) ? $svxconfig[$reflectorlogic1]['HOSTS'] : $svxconfig[$reflectorlogic1]['DNS_DOMAIN'];    
   if (strlen($fmnetwork_all) > 23) {
      $fmnetwork_arry = explode(",", $fmnetwork_all);
      $fmnetwork1 = $fmnetwork_arry[0];
      if (str_ends_with($fmnetwork1,"hamnet.cloud")) {
         $fmnetwork1 = "HAMNet."  . substr($fmnetwork1,0,1);
      }    
      if (str_ends_with($fmnetwork1,"fm-funknetz.de:5308")) {
         $fmnetwork1 = substr($fmnetwork1,0,7);
      }    
      if (str_ends_with($fmnetwork1,"reflector-network.eu:5308")) {
         $fmnetwork1 = substr($fmnetwork1,0,11);
      }    
// shorter name
      if (strlen($fmnetwork1) > 23) {
         $fmnetwork_arry = explode(".", $fmnetwork1);
         $fmnetwork1 = $fmnetwork_arry[0];
      }
   } else {
      $fmnetwork1 = $fmnetwork_all;
   }   
//   
//   echo "<table  style=\"margin-bottom:13px;\"><tr><th>".$fmnetwork1."</th></tr><tr>";
   echo "<table><tr><th>".$fmnetwork1."</th></tr><tr>";
   $svxrstatus = getSVXRstatus($reflectorlogic1);
   echo "<tr>";
   if ($svxrstatus=="Connected") {
      echo "<td style=\"background:#c3e5cc;\"><div style=\"margin-top:2px;margin-bottom:2px;white-space:normal;color:#b44010;font-weight:bold;\">";
      echo $svxrstatus."</div>";
   }
   if ($svxrstatus=="Not connected") {
      echo "<td style=\"background:#ff9;\"><div style=\"margin-top:2px;margin-bottom:2px;color:#454545;font-weight:bold;\">";
      echo $svxrstatus."</div>";
   }
   if ($svxrstatus=="No status") {
      echo "<td style=\"background:#ffffed;\"><div style=\"margin-top:2px;margin-bottom:2px;color:#b0b0b0;font-weight:bold;\">"; 
      echo $svxrstatus."</div>";
   }
   echo "</td></tr>";
   echo "</table>\n";

   $tgtmp = trim(getSVXTGTMP($reflectorlogic1));
   echo "<table colspan=2 style=\"margin-top:4px;margin-bottom:13px;\">\n";
   $tgdefault = $svxconfig[$reflectorlogic1]['DEFAULT_TG'];
   $tgmon = explode(",",$svxconfig[$reflectorlogic1]['MONITOR_TGS']);
   echo "<tr><th width=50%>TG Default</th><td style=\"background:#ffffed;color:green;font-weight: bold;\">".$tgdefault."</td></tr>\n";
   echo "<tr><th width=50%>TG Monitor</th><td style=\"background:#ffffed;color:#b44010;font-weight: bold;\">";
   echo "<div style=\"white-space:normal;\">";
   foreach ($tgmon as $key) {
      echo $key." ";
   }
   echo "<span style=\"background: #ffffed;color:#0065ff;font-weight: bold;\">".$tgtmp."</span>";
   echo "</div></td></tr>\n";

   if (!isset($tgselect_a)) { $tgselect_a = "";}
   $tgselect = trim(getSVXTGSelect($reflectorlogic1));
   if ( $tgselect=="0") {
      $tgselect="";
      $svxTGact = DL3EL . "/tg_act";
      if (file_exists($svxTGact)) {
         $svxStatusFile = DL3EL . "/tg_status";
         file_put_contents($svxStatusFile, $tgdefault);
         unlink($svxTGact);
      }   
   } else {
      if (defined('DL3EL')) {
         $svxStatusFile = DL3EL . "/tg_status";
        $svxTGact = DL3EL . "/tg_act";
         if (!file_exists($svxTGact)) {
            file_put_contents($svxStatusFile, $tgselect);
            touch($svxTGact);
         }
      }      
   }
   echo "<tr><th width=50%>TG Active</th><td style=\"background: #ffffed;color:#0065ff;font-weight: bold;\">".$tgselect."</td></tr>\n";
   echo "</table>";

}
if ((defined ('debug')) && (debug > 0)) echo "R1:$reflectorlogic1 R2:$reflectorlogic2<br>";
if ($reflectorlogic2 != "") {
//   $fmnetwork2 = $svxconfig[$reflectorlogic2]['HOSTS'];     
   $fmnetwork_all = isset($svxconfig[$reflectorlogic2]['HOSTS']) ? $svxconfig[$reflectorlogic2]['HOSTS'] : $svxconfig[$reflectorlogic2]['DNS_DOMAIN'];    
   if (strlen($fmnetwork_all) > 23) {
      $fmnetwork_arry = explode(",", $fmnetwork_all);
      $fmnetwork2 = $fmnetwork_arry[0];
// shorter name
      if (strlen($fmnetwork2) > 23) {
         $fmnetwork2_arry = explode(".", $fmnetwork2);
         $fmnetwork2 = $fmnetwork2_arry[0];
      }
   } else {
      $fmnetwork2 = $fmnetwork_all;
   }   

//   echo "<table  style=\"margin-bottom:13px;\"><tr><th>".$fmnetwork2."</th></tr><tr>";
   echo "<table><tr><th>".$fmnetwork2."</th></tr><tr>";
   $svxrstatus = getSVXRstatus($reflectorlogic2);
   echo "<tr>";
   if ($svxrstatus=="Connected") {
      echo "<td style=\"background:#c3e5cc;\"><div style=\"margin-top:2px;margin-bottom:2px;white-space:normal;color:#b44010;font-weight:bold;\">";
      echo $svxrstatus."</div>";
   }
   if ($svxrstatus=="Not connected") {
      echo "<td style=\"background:#ff9;\"><div style=\"margin-top:2px;margin-bottom:2px;color:#454545;font-weight:bold;\">";
      echo $svxrstatus."</div>";
   }
   if ($svxrstatus=="No status") {
      echo "<td style=\"background:#ffffed;\"><div style=\"margin-top:2px;margin-bottom:2px;color:#b0b0b0;font-weight:bold;\">"; 
      echo $svxrstatus."</div>";
   }
   echo "</td></tr>";
   echo "</table>\n";
   $tgtmp = trim(getSVXTGTMP($reflectorlogic2));
   echo "<table colspan=2 style=\"margin-top:4px;margin-bottom:13px;\">\n";
   $tgdefault = $svxconfig[$reflectorlogic2]['DEFAULT_TG'];
   $tgmon = explode(",",$svxconfig[$reflectorlogic2]['MONITOR_TGS']);
   echo "<tr><th width=50%>TG Default</th><td style=\"background:#ffffed;color:green;font-weight: bold;\">".$tgdefault."</td></tr>\n";
   echo "<tr><th width=50%>TG Monitor</th><td style=\"background:#ffffed;color:#b44010;font-weight: bold;\">";
   echo "<div style=\"white-space:normal;\">";
   foreach ($tgmon as $key) {
      echo $key." ";
   }
   echo "<span style=\"background: #ffffed;color:#0065ff;font-weight: bold;\">".$tgtmp."</span>";
   echo "</div></td></tr>\n";

   $tgselect = trim(getSVXTGSelect($reflectorlogic2));
   if ( $tgselect=="0") {
      $tgselect="";
   }
   echo "<tr><th width=50%>TG Active</th><td style=\"background: #ffffed;color:#0065ff;font-weight: bold;\">".$tgselect."</td></tr>\n";
   echo "</table>";
}
   if (isset($svxconfig['Rx1']['PEAK_METER']) && ($svxconfig["Rx1"]["PEAK_METER"] =="1")) {
      $ispeak = true ;
   }   
   //echo "<table  style=\"margin-bottom:13px;\"><tr><th>Radio Status</th></tr><tr>";
   //echo getTXInfo();
   //if ($ispeak==true) echo getRXPeak();
   //echo "</table>\n";
   if (($check_logics[0]=="RepeaterLogic") && ($svxconfig['RepeaterLogic']['TX'] !== "NONE")) {
      echo "<table  style=\"margin-bottom:13px;\"><tr><th>Repeater Status</th></tr><tr>";
      echo getTXInfo();
      echo "</table>\n"; 
   }
   if (($check_logics[0]=="SimplexLogic") && ($svxconfig['SimplexLogic']['TX'] !== "NONE")) {
      echo "<table  style=\"margin-bottom:13px;\"><tr><th>Radio Status</th></tr><tr>";
      echo getTXInfo();
      echo "</table>\n"; 
   }

   if ($modecho=="True") {
      $echolog = getEchoLog();
      $echotxing = getEchoLinkTX();
      echo "<table style=\"margin-top:4px;margin-bottom:13px;\"><tr><th colspan=2 >EchoLink Users</th></tr><tr>";
      echo "<tr>";
      $timestamp = $svxconfig['GLOBAL']['TIMESTAMP_FORMAT'];    
      $users = getConnectedEcholink($echolog,$timestamp);
      if (count($users)!=0){
         echo "<td colspan=2 style=\"background:#f6f6bd;\"><div style=\"margin-top:4px;margin-bottom:4px;white-space:normal;color:#0065ff;font-weight: bold;\">";
         foreach ($users as $user) {
            echo "<a href=\"http://www.qrz.com/db/".$user."\" target=\"_blank\"><b>".str_replace("0","&Oslash;",$user)."</b></a> ";
         }
      } else { 
         echo "<td colspan=2 style=\"background:#ffffed;\"><div style=\"margin-top:4px;margin-bottom:4px;color:#b0b0b0;font-weight: bold;\">Not connected";
      }
      echo "</div></td></tr>";
      echo "<tr><th width=50%>TXing</th><td style=\"background:#ffffed;color:red;font-weight: bold;\">".$echotxing."</td></tr>";
      echo "</table>\n";

//      $svxEchoConfigFile = "/etc/svxlink/svxlink.d/ModuleEchoLink.conf";
      $svxEchoConfigFile = MODULEPATH . ECHOLINKCONFIG;
      if (fopen($svxEchoConfigFile,'r')) { 
         $svxconfig = custom_parse_ini_file($svxEchoConfigFile);
         $key = "PROXY_SERVER";
         $section = "ModuleEchoLink";
         $eproxyd = $svxconfig[$section][$key]['value'];
         $eproxyd_state = ($svxconfig[$section][$key]['active'])? 1 : 0;
         if ((defined ('debug')) && (debug > 0)) echo "EP0: $eproxyd ($eproxyd_state)<br>";
      } else {
         if ((defined ('debug')) && (debug > 0)) echo "$svxEchoConfigFile not found";
         $eproxyd= ""; 
      }
      if ((defined ('debug')) && (debug > 0)) echo "EP2: $eproxyd<br>";
         if (!$eproxyd_state) {
            $eproxy = "not in use";
         } else {
            $eproxy = getEchoLinkProxy();
         }
      if ($eproxy!="" and $eproxyd_state) {
         echo "<table style=\"margin-top:4px;margin-bottom:4px;\"><tr><th>EchoLink Proxy</th></tr><tr>"; 
         echo "<tr><td style=\"background:#ffffed;\">";
         echo "<div style=\"margin-top:2px;margin-bottom:2px;white-space:normal;color:black;font-weight:500;\">";
         if ($eproxy!="Access denied to proxy ") {
            echo $eproxy;
         } else { 
            echo "<div style=\"margin-top:2px;margin-bottom:2px;color:red;font-weight: bold;\">".$eproxy; 
         }
         echo "</div></td></tr>";
         echo "</table>\n";
      }
   }
   echo "<table style=\"margin-top:4px;margin-bottom:13px;\"><tr><th colspan=2 >Systeminfo</th></tr><tr>";
   echo "<td colspan=2 style=\"background:#ffffed;\"><div style=\"margin-top:4px;margin-bottom:4px;white-space:normal;color:#000000;font-weight: bold;\">"; 
   echo "Last Reboot<br>",exec('uptime -s');
   echo "</div></td></tr>";
   if ($check_logics[0] == "RepeaterLogic") {
      echo "<td colspan=2 style=\"background:#ffffed;\"><div style=\"margin-top:4px;margin-bottom:4px;white-space:normal;color:#0a7d29;font-weight: bold;\">";
      echo "Mode: duplex";
      echo "</div></td></tr>";
   }
   if ($check_logics[0] == "SimplexLogic") {
      echo "<td colspan=2 style=\"background:#ffffed;\"><div style=\"margin-top:4px;margin-bottom:4px;white-space:normal;color:#0a7d29;font-weight: bold;\">";
      if (defined('DL3EL_RADIO')) {
         $svxRadio = DL3EL_RADIO;
         if (($svxRadio == "Shari") || ($svxRadio == "RFGuru")) {
            $RfConfFile = DL3EL . '/sa818/sa818.json';
            if (fopen($RfConfFile,'r')) {
               $filedata = file_get_contents($RfConfFile);
               $RfData = json_decode($filedata,true);
               $radioport = $RfData['port'];
            } else {
              $radioport = "";
            }
            if ($radioport !== "") {
               $command = "perl " . DL3EL . "/sa818/get_shari_hf_data.pl d=" . DL3EL . " p=" . $radioport;
//               echo "QRG:",exec($command, $output, $retval);
               exec($command, $output, $retval);
               if (substr($output[0],0,1) === "/") {
                  echo "Radio is gone";
               } else {
                  echo "QRG:$output[0]";
               }   
            } else {
               echo " no Radio found";
            }   
               
         } else {
            echo " Radio: " . DL3EL_RADIO;
         }   
      }   
      echo "</div></td></tr>";    echo "</div></td></tr>";
   }  

 
   $ip = isset($_SERVER['REMOTE_ADDR'])? $_SERVER['REMOTE_ADDR'] : '0';;
   if ((defined ('debug')) && (debug > 0)) echo "REMOTE_ADDR: $_SERVER[REMOTE_ADDR]<br>$ip";
   //$net1= cidr_match($ip,"192.168.0.0/16");
   //$net2= cidr_match($ip,"44.128.0.0/10");

   echo "<table style=\"margin-top:4px;margin-bottom:13px;\"><tr><th colspan=2 >Editing</th></tr><tr>";
   echo "<td colspan=2 style=\"background:#ffffed;\"><div style=\"margin-top:4px;margin-bottom:4px;white-space:normal;color:#ff0000;font-weight: bold;\">";
   if ($_SESSION['auth'] === "UNAUTHORISED") {
      if ((defined('DL3EL_NOAUTH')) && (DL3EL_NOAUTH === "yes")) {
         echo "always logged in";
         $_SESSION['auth'] = "AUTHORISED";
      } else {
         echo "Not Authorised";
      }
   } else {
      echo "logged in";
   }   
//   $ip = "2003:fb:8734:5600:7d73:c34b:8b35:e900";
   if (strlen($ip) < 16) {
      echo '<span title="[IP Address of Browser Client]"> from<br>['. $ip . ']</span>';
   } else {   
      echo '<span title="[' . $ip .']"> from<br>IPV6</span>';
   }   
   if ((defined ('DL3EL_APRS_MSG')) && (DL3EL_APRS_MSG === "yes")) {
      $aprspos = DL3EL . "/aprs-follow.pos";
      if (file_exists($aprspos)) {
         $delta = time() - filemtime($aprspos);
//         if ($delta > 60) {
            $filepos = file_get_contents($aprspos);
            $position = explode("^",$filepos);
            echo "<br>QTH: " . $position[0] . " " . $position[1];
            if (isset($position[3])) {
               echo "<br>from: " . $position[3] . " (" . $position[2] . ")";
            }
//         }   
      }
   }      
   echo "</div></td></tr>";
   echo "</table>\n";
} else {
   echo "<span style=\"color:red;font-size:13.5px;font-weight: bold;\">SvxLink is not <br>running</span>";
}
?>
</fieldset>
