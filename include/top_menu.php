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
   <div id="display-links" align=center>
    <p style = "padding-right: 5px; text-align: center; color: #000000;">
    <b>
    <a style = "color: black;">Display:</a>
    </b>
<?php
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
	echo '<a href="./index.php" style = "color: #000000;" target="_top">Dashboard</a> | ';
//	echo '<a href="./tg.php" style = "color: #000000;">Talk Groups</a> | ';
	echo '<a href="./caller.php?id=include/tg" style = "color: #000000;">Talk Groups</a> | ';
	if ((defined('DL3EL_LIVEDB')) && (DL3EL_LIVEDB === "top")) {
	    echo '<a href="./svx2mqtt.php" style = "color: #0000ff;">LIVE DB</a> | ';
	}    
	echo '<a href="./caller.php?id=monitor0" style = "color: crimson;" id="log" target="_top">MonitorCalls | </a> ';
//	echo '<a href="./monitor.php" style = "color: crimson;" id="log" target="_top">MonitorCalls0 | </a> ';
//	echo '<a href="./echolink_dl3el.php" style = "color: #0000ff;" target="_top">Echolink0</a> | ';
	echo '<a href="./caller.php?id=echolink_dl3el0" style = "color: #0000ff;" target="_top">Echolink</a> | ';
//	echo '<a href="./relais.php" style = "color: #0000ff;" target="_top">FM Relais</a> | ';
	echo '<a href="./caller.php?id=relais0" style = "color: #0000ff;" >FM Relais</a> | ';

	if ((defined('DL3EL_DXCLUSTER')) && (DL3EL_DXCLUSTER === "yes")) {
	    echo '<a href="./caller.php?id=dxcluster" style = "color: #0000ff;" target="_top">DX</a> | ';
	}    
	if (!defined('DL3EL_OPENHAMCLOCK')) {
	    echo '<a href="./caller_extern.php?id=https://openhamclock.com" style = "color: #0000ff;">OHC | </a> ';
	} else {
	    if (DL3EL_OPENHAMCLOCK !== "no") {
		if ((defined('DL3EL_OPENHAMCLOCK_TAB')) && (DL3EL_OPENHAMCLOCK_TAB === "new")) {
		    echo '<a href="' . DL3EL_OPENHAMCLOCK . '" style = "color: #0000ff;" target="OHC" rel="noopener noreferrer">OHC | </a> ';
		} else{
		    echo '<a href="./caller_extern.php?id=' . DL3EL_OPENHAMCLOCK . '&wid=new" style = "color: #0000ff;">OHC | </a> ';
		}    
	    }
	}    
	if (defined('DL3EL_OPENWEBRX')) {
	    echo '<a href="./caller_extern.php?id=' . DL3EL_OPENWEBRX . ' style = "color: #0000ff;"  Starget="_top">OWR | </a> ';
	}    
	if ((defined('DL3EL_FMFUNKNETZ')) && (DL3EL_FMFUNKNETZ === "yes")) {
	    echo '<a href="./caller_extern.php?id=https://stream.fm-funknetz.de" style = "color: #0000ff;"  target="_top">Voice FMN | </a> ';
	}    
	//<!--<a href="./dtmf.php" style = "color: #0000ff;">Dtmf</a> | -->
	//<!--<a href="./audio.php" style = "color: #0000ff;">Audio </a> | -->
	if (file_exists('/opt/svxlink_mqtt/svxlink_mqtt_controller.py')) {
            $cubeip = str_replace(' ', '<br />', exec('hostname -I | awk \'{print $1}\''));
	    $cubeip = "http://" . $cubeip . ":8081";
	    echo '<a href="./caller_extern.php?id=' . $cubeip . '" style = "color: #0000ff;"  Starget="_top">SVXCube | </a> ';
	} else {
	    if (defined('DL3EL_SVXCUBE'))  {
		echo '<a href="./caller_extern.php?id=' . DL3EL_SVXCUBE . ' style = "color: #0000ff;"  Starget="_top">SVXCube | </a> ';
	    }    
	}    
	echo '<a href="./config_backup.php" style = "color: #000000;" target="_top">Backup/Restore</a> ';
    } else {
	echo '<a href="./index.php" style = "color: #000000;" target="_top">Dashboard</a> ';
	if ((defined('DL3EL_DXCLUSTER')) && (DL3EL_DXCLUSTER === "yes")) {
	    echo '| <a href="./caller.php?id=dxcluster" style = "color: #0000ff;" target="_top">DX</a> ';
	}    
    }   
?>
<?php
	if ((defined('DL3EL_VERSION')) && (strncmp(DL3EL_VERSION, "develop", 7) === 0)) {
//		echo '<a href="./audio.php" style = "color: #0000ff;"> Audio </a> | ';
		echo '<a href="./wifi.php" style = "color: #000000;" target="_top">| Wifi</a> | ';
		echo '<a href="./network.php" style = "color: #000000;" target="_top">Network</a> ';
	}	


	if ($_SESSION['auth'] === "AUTHORISED") {
	    echo '<a href="./editor.php?id=log" style = "color: crimson;" id="log">| Log </a> ';
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
	    echo '<a href="./authorise.php" style = "color: crimson;"> | Authorise</a>';
	}
	if ((defined('DL3EL_VERSION')) && (strncmp(DL3EL_VERSION, "develop", 7) === 0) && (file_exists('/usr/bin/dvs'))) {
	    echo '<a href="/DVSwitch/index.php" style = "color: #0000ff;"> | DVS</a></p>';
	}
	if ((file_exists('/var/log/svxreflector')) || (file_exists('/var/log/svxreflector.log'))) {
	    echo '<a href="/" style = "color: #0000ff;">| Reflector</a></p>';
	}
	echo '</div>';
	echo '<div id="full-edit-links"  align=center>';

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
//	echo '<a href="./edit.php?file=info" style = "text-align: left; border-radius:8px; color:white;border-color:transparent; background-color:blue; font-size:14px;" id="info">&nbsp;&nbsp;Neues in ' . $version . '&nbsp;&nbsp;</a>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;';
	echo '<a href="./caller_extern.php?id=http://relais.dl3el.de/FM-Funknetz/hilfe.html&wid=950" style = "text-align: left; border-radius:8px; color:white;border-color:transparent; background-color:blue; font-size:14px;" id="info">&nbsp;&nbsp;Hilfe für ' . $version . '&nbsp;&nbsp;</a>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;';
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
	echo '<a style = "padding-right: 5px; text-align: right; color: #000000;" <a style = "color: black;"><b>Display:</b></a> ';
//	echo '<a href="./caller_extern.php?id=http://relais.dl3el.de/FM-Funknetz/hilfe.html style = "color: #0000ff;">Hilfe</a> | ';
	if ((defined('DL3EL_APRS_MSG')) && (DL3EL_APRS_MSG === "yes")) {
//	    echo '<a href="./aprs.php" style = "color: #0000ff;">APRS Monitor</a> | ';
	    echo '<a href="./caller.php?id=aprs" style = "color: black;">APRS Monitor</a> | ';
	    echo '<a href="./edit.php?file=loga" style = "color: black;" id="log">APRS Log</a> | ';
//	    echo '<a href="./edit.php?file=msg" style = "color: black;" id="msg">APRS MSG</a> ';
	    if (file_exists(DL3EL.'/aprs-is-mqtt.msg')) {
		echo '<a href="./edit.php?file=logb" style = "color: black;" id="log">APRS MQTT | </a> ';
	    }
	}
	if ((file_exists('/var/log/svxreflector')) || (file_exists('/var/log/svxreflector.log'))) {
	    echo '<a href="./edit.php?file=log" style = "color: black;" id="log">L-Log</a> | ';
	    echo '<a href="./edit.php?file=ref" style = "color: black;" id="log">R-Log</a> ';
	} else {
	    echo '<a href="./edit.php?file=log" style = "color: black;" id="log">SVX Log</a> ';
	}
	if (defined('DL3EL_EXTERN1'))  {
	    if (defined('DL3EL_EXTERN1_NAME'))  {
		$linktext = DL3EL_EXTERN1_NAME;
	    } else {
		$linktext = DL3EL_EXTERN1;
	    }
//	    echo '| <a href="./caller_extern.php?id=' . DL3EL_EXTERN1 . ' style = "color: black;">' . $linktext . '</a> ';
	    if ((defined('DL3EL_EXTERN1_TAB')) && (DL3EL_EXTERN1_TAB === "new")) {
		    echo '| <a href="' . DL3EL_EXTERN1 . '" style = "color: black;" target="extern1" rel="noopener noreferrer">' . $linktext . '</a> ';
		} else{
		    echo '| <a href="./caller_extern.php?id=' . DL3EL_EXTERN1 . ' style = "color: black;">' . $linktext . '</a> ';
		}    
	}    
	if (defined('DL3EL_EXTERN2'))  {
	    if (defined('DL3EL_EXTERN2_NAME'))  {
		$linktext = DL3EL_EXTERN2_NAME;
	    } else {
		$linktext = DL3EL_EXTERN2;
	    }
//	    echo '| <a href="./caller_extern.php?id=' . DL3EL_EXTERN2 . ' style = "color: black;">' . $linktext . '</a> ';
	    if ((defined('DL3EL_EXTERN2_TAB')) && (DL3EL_EXTERN2_TAB === "new")) {
		    echo '| <a href="' . DL3EL_EXTERN2 . '" style = "color: black;" target="extern2" rel="noopener noreferrer">' . $linktext . '</a> ';
		} else{
		    echo '| <a href="./caller_extern.php?id=' . DL3EL_EXTERN2 . ' style = "color: black;">' . $linktext . '</a> ';
		}    
	}    
	if (defined('DL3EL_EXTERN3'))  {
	    if (defined('DL3EL_EXTERN3_NAME'))  {
		$linktext = DL3EL_EXTERN3_NAME;
	    } else {
		$linktext = DL3EL_EXTERN3;
	    }
//	    echo '| <a href="./caller_extern.php?id=' . DL3EL_EXTERN3 . ' style = "color: #000000;">' . $linktext . '</a> ';
	    if ((defined('DL3EL_EXTERN3_TAB')) && (DL3EL_EXTERN3_TAB === "new")) {
		    echo '| <a href="' . DL3EL_EXTERN3 . '" style = "color: black;" target="extern3" rel="noopener noreferrer">' . $linktext . '</a> ';
		} else{
		    echo '| <a href="./caller_extern.php?id=' . DL3EL_EXTERN3 . ' style = "color: black;">' . $linktext . '</a> ';
		}    
	}    
	if (defined('DL3EL_EXTERN4'))  {
	    if (defined('DL3EL_EXTERN4_NAME'))  {
		$linktext = DL3EL_EXTERN4_NAME;
	    } else {
		$linktext = DL3EL_EXTERN4;
	    }
//	    echo '| <a href="./caller_extern.php?id=' . DL3EL_EXTERN4 . ' style = "color: black;">' . $linktext . '</a> ';
	    if ((defined('DL3EL_EXTERN4_TAB')) && (DL3EL_EXTERN4_TAB === "new")) {
		    echo '| <a href="' . DL3EL_EXTERN4 . '" style = "color: black;" target="extern4" rel="noopener noreferrer">' . $linktext . '</a> ';
		} else{
		    echo '| <a href="./caller_extern.php?id=' . DL3EL_EXTERN4 . ' style = "color: black;">' . $linktext . '</a> ';
		}    
	}    
	if (file_exists(DL3EL.'/Reflector1.conf')) {
	    echo '<a style = "padding-right: 5px; text-align: right; color: #000000;" <a style = "color: black;"> | <b>Full</b> Edit</a> | ';
	    echo '<a href="./edit.php?file=' . DL3EL . '/Reflector1.conf" style = "color: black;" id="reflector1">Reflector1.conf</a> ';
	}
	if (file_exists(DL3EL.'/Reflector2.conf')) {
	    echo '| <a href="./edit.php?file=' . DL3EL . '/Reflector2.conf" style = "color: black;" id="reflector1">Reflector2.conf</a> ';
	}
	if (file_exists(DL3EL.'/Reflector3.conf')) {
	    echo '| <a href="./edit.php?file=' . DL3EL . '/Reflector3.conf" style = "color: black;" id="reflector1">Reflector3.conf</a> ';
	}
	if (file_exists(DL3EL.'/Reflector4.conf')) {
	    echo '| <a href="./edit.php?file=' . DL3EL . '/Reflector4.conf" style = "color: black;" id="reflector1">Reflector4.conf</a> ';
	}
        if ((defined('DL3EL_GIT_UPDATE')) && (DL3EL_GIT_UPDATE === "nocheck") && (defined('DL3EL_VERSION')) && (DL3EL_VERSION === "develop")) { 
            if (file_exists('./statistic.php')) {
		echo '| <a href="./statistic.php" style = "color: black;" id="log">Statistic</a> ';
            }
        }
	$tgdb_File = DL3EL_BASE . "include/tgdb.php";
	if (filesize($tgdb_File)  <= 1000) {
	    echo '| <a href="./include/tgdb_upd.php" style = "color: black;" id="upd">Update TGDB</a> ';
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

