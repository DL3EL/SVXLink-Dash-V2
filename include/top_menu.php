<?php
if (session_status() === PHP_SESSION_NONE) {
    session_start();
}

    $knowledgeFile = DL3EL . "/knowledge";
    if (file_exists($knowledgeFile)) {
	$knowledge = trim(shell_exec('cat ' . $knowledgeFile));
    } else {
	if ((defined('DL3EL_EXPERT')) && (DL3EL_EXPERT === "yes")) {
	    $knowledge = "Expert";
	} else {
	    $knowledge = "Normal";
	}    
    }
    if (isset($_POST['btn_expert'])) {
	define("DL3EL_EXPERT", "yes");
	$knowledge = "Expert";
	$knowledge_new = "Expert >" . $knowledgeFile;
        shell_exec("echo $knowledge_new");
    }
    if (isset($_POST['btn_normal'])) {
	define("DL3EL_EXPERT", "no");
	$knowledge = "Normal";
	$knowledge_new = "Normal >" . $knowledgeFile;
	shell_exec("echo $knowledge_new");
    }
    if ($knowledge == "Expert") {
	$kn_exp = "<b>Expert</b>";
	$kn_nor = "Normal";
    } else {
	$kn_exp = "Expert";
	$kn_nor = "<b>Normal</b>";
    }
   if (empty($_SESSION['auth'])) {
      $_SESSION['auth'] = "UNAUTHORISED";
   }
?>
   <div id="display-links" align=right>
    <p style = "padding-right: 5px; text-align: right; color: #000000;">
    <a style = "color: black;">Display</a> |
<?php
    if ((defined('DL3EL_LIVEDB_AUTO')) && (DL3EL_LIVEDB_AUTO === "yes")) {
	include_once "include/browserdetect.php";
	//$mobile = 1;
	$show_livedb_inDB = $mobile;
    } else {
	    $show_livedb_inDB = 1;
    }    
// Hinweis zu nodes gestrichen noch ergänzen
    if ((defined('DL3EL_MENU_AUTH_ONLY')) && (DL3EL_MENU_AUTH_ONLY === "yes")) {
	$show_top_menu = 0;
	if ($_SESSION['auth'] === "AUTHORISED") {
	    $show_top_menu = 1;
	} else {
	    $show_top_menu = 0;
	}
    } else {    
	    $show_top_menu = 1;
    }
    if ($show_top_menu) {
	if ($show_livedb_inDB) {
	    echo '<a href="./index.php" style = "color: #000000;">Dashboard</a> | ';
	    echo '<a href="./svx2mqtt.php" style = "color: #0000ff;">LIVE DB</a> | ';
	} else {
	    echo '<a href="./index1.php" style = "color: #000000;">Dashboard</a> | ';
	}   
	echo '<a href="./tg.php" style = "color: #000000;">Talk Groups</a> | ';
	echo '<a href="./monitor.php" style = "color: crimson;" id="log">MonitorCalls | </a> ';
    } else {
	echo '<a href="./index.php" style = "color: #000000;">Dashboard</a> | ';
    }   

    if ((defined('MENUTOP')) && (MENUTOP === "FULL")) {
	$show_top_menu = 1;
    } else {    
	if ($_SESSION['auth'] === "AUTHORISED") {
	    $show_top_menu = 1;
	} else {
	    $show_top_menu = 0;
	}
    } 	
    if ($show_top_menu) {
	echo '<a href="./echolink_dl3el.php" style = "color: #0000ff;">Echolink</a> | ';
	echo '<a href="./relais.php" style = "color: #0000ff;">FM Relais</a> | ';
	if ((defined('DL3EL_DXCLUSTER')) && (DL3EL_DXCLUSTER === "yes")) {
	    echo '<a href="./dxcluster.php" style = "color: #0000ff;">DX</a> | ';
	    echo '<a href="https://openhamclock.com" style = "color: #0000ff;" target="OHC">OHC</a> | ';
	}    
	echo '<a href="./config_backup.php" style = "color: #0000ff;">Backup/Restore</a> | ';
	//<!--<a href="./dtmf.php" style = "color: #0000ff;">Dtmf</a> | -->
	//<!--<a href="./audio.php" style = "color: #0000ff;">Audio </a> | -->
    }
?>
<?php
	if ((defined('DL3EL_VERSION')) && (strncmp(DL3EL_VERSION, "develop", 7) === 0)) {
//		echo '<a href="./audio.php" style = "color: #0000ff;"> Audio </a> | ';
		echo '<a href="./wifi.php" style = "color: #0000ff;">Wifi</a> | ';
		echo '<a href="./network.php" style = "color: #0000ff;">Network</a> | ';
	}	


	if ($_SESSION['auth'] === "AUTHORISED") {
	    echo '<a href="./editor.php?id=log" style = "color: crimson;" id="log"> Log | </a> ';
	}
?>
<?php
	if ((defined('DL3EL_NOAUTH')) && (DL3EL_NOAUTH === "yes")) {
	    echo ' ';
            $_SESSION['auth'] = "AUTHORISED";
	    $show_auth = 1;
	} else {    
	    if ((defined('MENUTOPAUTH')) && (MENUTOPAUTH === "no")) {
		$show_auth = 0;
		// do not show Authorise
	    } else {
		$show_auth = 1;
	    }
	    if ((defined ('debug')) && (debug > 0)) echo "<br>ShowAUTH: $show_auth";
	    $ip = isset($_SERVER['REMOTE_ADDR'])? $_SERVER['REMOTE_ADDR'] : '0';;
	    if ((defined ('debug')) && (debug > 0)) echo "<br>RemoteIP: $ip";
	    $net1= cidr_match($ip,"192.168.0.0/16");
	    $net2= cidr_match($ip,"44.128.0.0/10");
	    $net3= cidr_match($ip,"44.149.160.87/32");
	    $net4= cidr_match($ip,"44.149.162.105/32");
	    if ($net1 == TRUE || $net2 == TRUE) {
		$show_auth = 1;
	    } 
	    if (defined ('DL3EL_MASTER_IP')) {
		$master_ip_array = explode(",", DL3EL_MASTER_IP);
		$nn = 0;
		foreach ($master_ip_array as $master_ip) {
		    if ((defined ('debug')) && (debug > 0)) echo "<br>IP($nn):$master_ip ($ip)";
		    if (cidr_match($ip,$master_ip)) {
			$_SESSION['auth'] = "AUTHORISED";
			$show_auth = 1;
			break;
		    }
		    ++$nn;
		}    
	    }
	}
	if ($show_auth) {
	    echo '<a href="./authorise.php" style = "color: crimson;">Authorise</a>';
	}
	if ((defined('DL3EL_VERSION')) && (strncmp(DL3EL_VERSION, "develop", 7) === 0) && (file_exists('/usr/bin/dvs'))) {
	    echo '<a href="/DVSwitch/index.php" style = "color: #0000ff;"> | DVS</a></p>';
	}
	if ((file_exists('/var/log/svxreflector')) || (file_exists('/var/log/svxreflector.log'))) {
	    echo '<a href="/" style = "color: #0000ff;">| Reflector</a></p>';
	}
	echo '</div>';
	echo '<div id="full-edit-links"  align=right>';

   if ($_SESSION['auth'] === "AUTHORISED") {

	echo '<form method="post" action="' . htmlspecialchars($_SERVER["PHP_SELF"]) .'"> ';
        list($version, $rest) = explode(" ", $dbversion);

        $dbversionFile = DL3EL . "/dbversion.upd";
        if (file_exists($dbversionFile)) {
	    $content = file_get_contents($dbversionFile);
	    if ($content === "update") {
		$version = $version . " Update available";
	    }
	}    
	echo '<a href="./edit.php?file=info" style = "text-align: left; border-radius:8px; color:white;border-color:transparent; background-color:blue; font-size:14px;" id="info">&nbsp;&nbsp;Neues in ' . $version . '&nbsp;&nbsp;</a>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;';
	echo '<button name="btn_expert" type="submit" >' . $kn_exp . '</button>';
	echo '<button name="btn_normal" type="submit" >' . $kn_nor . '</button>';
	ECHO '&nbsp;&nbsp;&nbsp;&nbsp;';
	if ($knowledge == "Expert") {
	    echo '<a style = "padding-right: 5px; text-align: right; color: #000000;" <a style = "color: black;"><b>Full</b> Edit</a> | ';
	    echo '<a href="./edit.php?file=' . SVXCONFPATH . SVXCONFIG . '" style = "color: black;" id="svxlink">SVXLink</a> | ';
	    if ((file_exists('/var/log/svxreflector')) || (file_exists('/var/log/svxreflector.log'))) {
		echo '<a href="./edit.php?file=' . SVXCONFPATH . 'svxreflector.conf" style = "color: black;" id="svxlink">SVXRef</a> | ';
	    }
	    echo '<a href="./edit.php?file=' . MODULEPATH . ECHOLINKCONFIG . '" style = "color: black;" id="echolink">EchoLink</a> | ';
	    echo '<a href="./edit.php?file=' . MODULEPATH . METARINFO . '" style = "color: black;" id="metarinfo">MetarInfo</a> | ';
	    echo '<a href="./edit.php?file=' . SVXCONFPATH . 'node_info.json" style = "color: black;" id="nodeInfo">NodeInfo</a> | ';
	    if ((defined('DL3EL_APRS_MSG')) && (DL3EL_APRS_MSG === "yes")) {
		echo '<a href="./edit.php?file=' . DL3EL . '/aprs-is-msg.conf" style = "color: black;" id="aprs">aprs.conf</a> | ';
	    }
	    echo '<a href="./edit.php?file=' . DL3EL_BASE . 'include/config.php" style = "color: black;" id="configphp">config.php</a> | ';
	    if ((file_exists('/etc/default/shellinabox')) && ((defined('DL3EL_SSH')) && (strncmp(DL3EL_SSH, "yes", 3) === 0))) {
		$getPortCommand = "grep -m 1 'SHELLINABOX_PORT=' /etc/default/shellinabox | awk -F '=' '/SHELLINABOX_PORT=/ {print $2}'";
		$shellPort = exec($getPortCommand);    
		if ($shellPort == 4200) {
		    echo '<a href="./ssh.php" style = "color: black;">SSH</a> | ';
		}
	    }	
//	    echo '<a href="./editor.php?id=amixer" style = "color: black;" id="amixer">Amixer</a> | ';
	    if ((defined('TclVoiceMail')) && (strncmp(TclVoiceMail, "no", 1) !== 0)) {
		echo '<a href="./edit.php?file=TclVoiceMail.conf" style = "color: black;" id="tclvoicemail">TclVoiceMail</a> | ';
	    }
	} else {
	    echo '<a style = "padding-right: 5px; text-align: right; color: #000000;" <a style = "color: black;">Edit</a> | ';
	    echo '<a href="./editor.php?id=svxlink" style = "color: black;" id="svxlink">SVXLink</a> | ';
	    echo '<a href="./editor.php?id=echolink" style = "color: black;" id="echolink">EchoLink</a> | ';
	    echo '<a href="./editor.php?id=metarinfo" style = "color: black;" id="metarinfo">MetarInfo</a> | ';
	    echo '<a href="./editor.php?id=nodeInfo" style = "color: black;" id="nodeInfo">NodeInfo</a> | ';
	    if ((defined('TclVoiceMail')) && (strncmp(TclVoiceMail, "no", 1) !== 0)) {
		echo '<a href="./editor.php?id=tclvoicemail" style = "color: black;" id="tclvoicemail">TclVoiceMail</a> | ';
	    }
	}	
	if (defined('DL3EL_RADIO')) {
	    $svxRadio = DL3EL_RADIO;
	    if (($svxRadio == "Shari") || ($svxRadio == "RFGuru")) {
		echo '<a href="./editor.php?id=amixer" style = "color: black;" id="amixer">AMixer</a> | ';
		echo '<a href="./rf.php" style = "color: black;" id="radio">Radio</a> | ';
	    }
	    if ($svxRadio == "SC (no Radio)") {
		echo '<a href="./editor.php?id=amixer" style = "color: black;" id="amixer">AMixer</a> | ';
	    }
	}
	echo '<a href="./editor.php?id=power" style = "color: green;">Power</a></p>';
	echo '<a style = "padding-right: 5px; text-align: right; color: #000000;" <a style = "color: black;"><b>Display</b></a> | ';
	if ((defined('DL3EL_APRS_MSG')) && (DL3EL_APRS_MSG === "yes")) {
	    echo '<a href="./aprs.php" style = "color: #0000ff;">APRS Monitor</a> | ';
	    echo '<a href="./edit.php?file=loga" style = "color: black;" id="log">APRS Log</a> | ';
//	    echo '<a href="./edit.php?file=msg" style = "color: black;" id="msg">APRS MSG</a> ';
	    if (file_exists(DL3EL.'/aprs-is-mqtt.msg')) {
		echo '<a href="./edit.php?file=logb" style = "color: black;" id="log">APRS MQTT | </a> ';
	    }
	}
	if ((file_exists('/var/log/svxreflector')) || (file_exists('/var/log/svxreflector.log'))) {
	    echo '<a href="./edit.php?file=log" style = "color: black;" id="log">L-Log</a> | ';
	    echo '<a href="./edit.php?file=ref" style = "color: black;" id="log">R-Log</a>';
	} else {
	    echo '<a href="./edit.php?file=log" style = "color: black;" id="log">SVX Log</a>';
	}
	if (file_exists(DL3EL.'/Reflector1.conf')) {
	    echo '<a style = "padding-right: 5px; text-align: right; color: #000000;" <a style = "color: black;"> | <b>Full</b> Edit</a> | ';
	    echo '<a href="./edit.php?file=' . DL3EL . '/Reflector1.conf" style = "color: black;" id="reflector1">Reflector1.conf</a>';
	}
	if (file_exists(DL3EL.'/Reflector2.conf')) {
	    echo ' | <a href="./edit.php?file=' . DL3EL . '/Reflector2.conf" style = "color: black;" id="reflector1">Reflector2.conf</a>';
	}
	if (file_exists(DL3EL.'/Reflector3.conf')) {
	    echo ' | <a href="./edit.php?file=' . DL3EL . '/Reflector3.conf" style = "color: black;" id="reflector1">Reflector3.conf</a>';
	}
	if (file_exists(DL3EL.'/Reflector4.conf')) {
	    echo ' | <a href="./edit.php?file=' . DL3EL . '/Reflector4.conf" style = "color: black;" id="reflector1">Reflector4.conf</a>';
	}
        if ((defined('DL3EL_GIT_UPDATE')) && (DL3EL_GIT_UPDATE === "nocheck") && (defined('DL3EL_VERSION')) && (DL3EL_VERSION === "develop")) { 
            if (file_exists('./statistic.php')) {
		echo ' | <a href="./statistic.php" style = "color: black;" id="log">Statistic</a> ';
            }
        }
	$tgdb_File = DL3EL_BASE . "include/tgdb.php";
	if (filesize($tgdb_File)  <= 1000) {
	    echo ' | <a href="./include/tgdb_upd.php" style = "color: black;" id="upd">Update TGDB</a> ';
	}
	echo '</p>';
}


    echo '</div>';
    echo '<div id="display-links" align=center>';
//    echo '<a href="./edit.php?file=info" style = "border-radius:8px; color:white;border-color:transparent; background-color:blue; font-size:14px;" id="info">&nbsp;&nbsp;Neues in der Version ' . $dbversion . '&nbsp;&nbsp;</a>';
    echo '</div>';

    echo '</form>';

//include_once('parse_svxconf.php');


/*if (fopen($svxConfigFile,'r'))
{

  $svxconfig = parse_ini_file($svxConfigFile,true,INI_SCANNER_RAW);
  $logics = explode(",",$svxconfig['GLOBAL']['LOGICS']);
  foreach ($logics as $key) {
	if ($key == "SimplexLogic") $isSimplex = true;
	if ($key == "RepeaterLogic") $isRepeater = true; 
  };
  $logics = explode(",",$svxconfig['GLOBAL']['LOGICS']);
  if ($isSimplex) $modules = explode(",",str_replace('Module','',$svxconfig['SimplexLogic']['MODULES']));
  if ($isRepeater) $modules = explode(",",str_replace('Module','',$svxconfig['RepeaterLogic']['MODULES']));
  foreach ($modules as $key){
	if ($key == "EchoLink") $isEchoLink = true;
 }
 */
 //if ($isEchoLink==true) {echo ' <a href="/echolink.php" style = "color: #0000ff;">EchoLink</a> |';};
//$globalRf = $svxconfig['GLOBAL']['RF_MODULE'];

/*if ($globalRf <> "No")
{
	echo'	<a href="/rf.php" style = "color: #0000ff;"> Rf</a> |';
}
}*/
?>

