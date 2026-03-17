<style>
a.hreflink:link,
a.hreflink:visited {
    color: black;
    text-decoration: underline;
}

a.hreflink:hover {
    color: #009900;	
    text-decoration: underline;
}
</style>
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
    include_once DL3EL_BASE . "/include/select_menu.php";    
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
	echo '<a class="hreflink" href="./index.php" target="_top">Dashboard</a> | ';
//	echo '<a href="./tg.php" style = "color: #000000;">Talk Groups</a> | ';
	echo '<a class="hreflink" href="./caller.php?id=include/tg" ">Talk Groups</a> | ';


	if ((defined('DL3EL_LIVEDB')) && (DL3EL_LIVEDB === "top")) {
	    if (defined('DL3EL_MQTT')) {
		if (DL3EL_MQTT === "no") {
		    echo '<a class="hreflink" href="./svx2mqtt.php">LIVE DB</a> | ';
		} else {
		    echo '| <a class="hreflink" href="./caller.php?id=svx2mqtt/index_neu&refresh=1">LIVE DB</a> ';
		}    
	    } else {
		echo '<a class="hreflink" href="./svx2mqtt.php">LIVE DB</a> | ';
	    }
	}    
//	if ((defined('DL3EL_MONREFRESH')) && (DL3EL_MONREFRESH > "4")) {
//	if ($fmnetwork === "uk.wide.svxlink.uk") {
	if (check_network($fmnetwork,"uk.wide.svxlink.uk")) {
	    echo '<a class="hreflink" href="./caller_extern.php?id=https://ukwide.svxlink.net">UK Monitor | </a> ';
	} else {
	    if (file_exists("/usr/local/bin/mqtt-simple")) {
//	    echo '<a href="./caller.php?id=monitor0&refresh=' . DL3EL_MONREFRESH . '" style = "color: crimson;" id="log" target="_top">MonitorCalls | </a> ';
		echo '<a class="hreflink" href="./caller.php?id=monitor&refresh=15" style = "color: crimson;" id="log" target="_top">MonitorCalls | </a> ';
	    } else {
		echo '<a class="hreflink" href="./caller.php?id=monitor0" style = "color: crimson;" id="log" target="_top">MonitorCalls | </a> ';
	    }
	}    
	
//	echo '<a href="./monitor.php" style = "color: crimson;" id="log" target="_top">MonitorCalls0 | </a> ';
//	echo '<a href="./echolink_dl3el.php" style = "color: #0000ff;" target="_top">Echolink0</a> | ';
	echo '<a class="hreflink" href="./caller.php?id=echolink_dl3el0" >Echolink</a> | ';
//	echo '<a href="./relais.php" style = "color: #0000ff;" target="_top">FM Relais</a> | ';
	echo '<a class="hreflink" href="./caller.php?id=relais0" " >FM Relais</a> | ';

	if ((defined('DL3EL_DXCLUSTER')) && (DL3EL_DXCLUSTER === "yes")) {
//	    echo '<a href="./caller.php?id=dxcluster" style = "color: #0000ff;" target="_top">DX</a> | ';
	    echo '<a class="hreflink" href="./caller.php?id=include/svxdxc&refresh=15" >DX</a> | ';
////	    echo '<a href="./caller_refresh.php?id=include/svxdxc" style = "color: #0000ff;" target="_top">DX_R</a> | ';
	}    
	if (!defined('DL3EL_OPENHAMCLOCK')) {
	    $openhamclock = "https://openhamclock.com/";
	} else {    
	    $openhamclock = DL3EL_OPENHAMCLOCK;
	}

	if ($openhamclock !== "no") {
	    if ((defined('DL3EL_OPENHAMCLOCK_TAB')) && (DL3EL_OPENHAMCLOCK_TAB === "new")) {
		echo '<a class="hreflink" href="' . $openhamclock . '" target="OHC" rel="noopener noreferrer">OHC | </a> ';
	    } else{
		echo '<a class="hreflink" href="./caller_extern.php?id=' . $openhamclock . '" ">OHC | </a> ';
	    }    
	}    
	if (defined('DL3EL_OPENWEBRX')) {
	    echo '<a class="hreflink" href="./caller_extern.php?id=' . DL3EL_OPENWEBRX . '" >OWR | </a> ';
	}    
	if ((defined('DL3EL_FMFUNKNETZ')) && (DL3EL_FMFUNKNETZ === "yes")) {
	    echo '<a class="hreflink" href="./caller_extern.php?id=https://stream.fm-funknetz.de" >Voice FMN | </a> ';
	}    
// FM-Funknetz WebChat
	if ((defined('DL3EL_WEBCHAT')) && (DL3EL_WEBCHAT === "no")) {
	    // do not display Chat in Menu
	} else {
	    echo '<a class="hreflink" href="./caller_extern.php?id=https://chat.fm-funknetz.de/index.php?call=' . $callsign . '">FMN WebChat | </a> ';
	}    
	//<!--<a href="./dtmf.php" style = "color: #0000ff;">Dtmf</a> | -->
	//<!--<a href="./audio.php" style = "color: #0000ff;">Audio </a> | -->
	if (file_exists('/opt/svxlink_mqtt/svxlink_mqtt_controller.py')) {
            $cubeip = str_replace(' ', '<br />', exec('hostname -I | awk \'{print $1}\''));
	    $cubeip = "http://" . $cubeip . ":8081";
	    echo '<a class="hreflink" href="./caller_extern.php?id=' . $cubeip . '" >SVXCube | </a> ';
	} else {
	    if (defined('DL3EL_SVXCUBE'))  {
		echo '<a class="hreflink" href="./caller_extern.php?id=' . DL3EL_SVXCUBE . '" >SVXCube | </a> ';
	    }    
	}    
	if ((defined('DL3EL_QSOLINK')) && (DL3EL_QSOLINK === "yes")) {
	    echo '<a class="hreflink" href="./caller_extern.php?id=https://qsolink.websdrbordeaux.fr/dashboard.html">QSOLinkDB | </a> ';
	}
	echo '<a class="hreflink" href="./config_backup.php" >Backup/Restore</a> ';
    } else {
	echo '<a class="hreflink" href="./index.php" >Dashboard</a> ';
	if ((defined('DL3EL_DXCLUSTER')) && (DL3EL_DXCLUSTER === "yes")) {
//	    echo '| <a href="./caller.php?id=dxcluster" style = "color: #0000ff;" target="_top">DX</a> ';
	    echo '| <a class="hreflink" href="./caller.php?id=include/svxdxc&refresh=15" >DX</a> | ';
	}    
    }   
?>
<?php
	if (file_exists(DL3EL_BASE . "/mobile/index.php")) {
	    echo '| <a class="hreflink" href="./mobile/index.php" >mobile</a> ';
	}    

	if ($_SESSION['auth'] === "AUTHORISED") {
	    echo '<a class="hreflink" href="./editor.php?id=log" id="log">| Log </a> ';
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
	    echo '<a class="hreflink" href="./authorise.php" > | Authorise</a>';
	}
	if ((defined('DL3EL_VERSION')) && (strncmp(DL3EL_VERSION, "develop", 7) === 0) && (file_exists('/usr/bin/dvs'))) {
	    echo '<a class="hreflink" href="/DVSwitch/index.php" > | DVS</a></p>';
	}
	if ((file_exists('/var/log/svxreflector')) || (file_exists('/var/log/svxreflector.log'))) {
//	    echo '<a href="/" style = "color: #0000ff;">| Reflector</a></p>';
	    echo '<a class="hreflink" href="./caller.php?id=/" >| Reflector</a></p>';
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
//	if ((!defined('DL4EM_TOPMENU')) || ((defined('DL4EM_TOPMENU')) && (DL4EM_TOPMENU === "yes"))) {
	if ((defined('DL4EM_TOPMENU')) && (DL4EM_TOPMENU === "yes")) {
	    echo '&nbsp;<button name="btn_dropdown" type="submit" >' . $menu_dropdown . '</button>';
	}    
	ECHO '&nbsp;&nbsp;&nbsp;&nbsp;';
	if ($knowledge == "Expert") {
	    echo '<a style = "padding-right: 5px; text-align: right; color: #000000;" <a style = "color: black;"><b>Full</b> Edit</a> | ';
	    echo '<a class="hreflink" href="./caller.php?file=' . SVXCONFPATH . SVXCONFIG . '" id="svxlink">SVXLink</a> | ';
	    if ((file_exists('/var/log/svxreflector')) || (file_exists('/var/log/svxreflector.log'))) {
		echo '<a class="hreflink" href="./caller.php?file=' . SVXCONFPATH . 'svxreflector.conf" id="svxlink">SVXRef</a> | ';
	    }
	    echo '<a class="hreflink" href="./caller.php?file=' . MODULEPATH . ECHOLINKCONFIG . '" id="echolink">EchoLink</a> | ';
	    echo '<a class="hreflink" href="./caller.php?file=' . MODULEPATH . METARINFO . '" id="metarinfo">MetarInfo</a> | ';
	    echo '<a class="hreflink" href="./caller.php?file=' . SVXCONFPATH . 'node_info.json" class="hreflink" id="nodeInfo">NodeInfo</a> | ';
	    if ((defined('DL3EL_APRS_MSG')) && (DL3EL_APRS_MSG === "yes")) {
		echo '<a class="hreflink" href="./caller.php?file=' . DL3EL . '/aprs-is-msg.conf" id="aprs">aprs.conf</a> | ';
	    }
	    echo '<a class="hreflink" href="./caller.php?file=' . DL3EL_BASE . 'include/config.php" id="configphp">config.php</a> | ';
	    echo '<a class="hreflink" href="./wifi.php" >Wifi</a> | ';
	    if ((defined('DL3EL_VERSION')) && (DL3EL_VERSION === "develop")) {
//		echo '<a href="./audio.php" style = "color: #0000ff;"> Audio </a> | ';
		echo '<a class="hreflink" href="./network.php" >Network</a> |  ';
	    }	
	    if ((file_exists('/etc/default/shellinabox')) && ((defined('DL3EL_SSH')) && (strncmp(DL3EL_SSH, "yes", 3) === 0))) {
		$getPortCommand = "grep -m 1 'SHELLINABOX_PORT=' /etc/default/shellinabox | awk -F '=' '/SHELLINABOX_PORT=/ {print $2}'";
		$shellPort = exec($getPortCommand);    
		if ($shellPort == 4200) {
		    echo '<a class="hreflink" href="./ssh.php" >SSH</a> | ';
		}
	    }	
//	    echo '<a href="./editor.php?id=amixer" style = "color: black;" id="amixer">Amixer</a> | ';
	    if ((defined('TclVoiceMail')) && (strncmp(TclVoiceMail, "no", 1) !== 0)) {
		echo '<a class="hreflink" href="./caller.php?file=TclVoiceMail.conf" id="tclvoicemail">TclVoiceMail</a> | ';
	    }
	} else {
	    echo '<a style = "padding-right: 5px; text-align: right; <a style = "color: black;">Edit</a> | ';
	    echo '<a class="hreflink" href="./editor.php?id=svxlink" id="svxlink">SVXLink</a> | ';
	    echo '<a class="hreflink" href="./editor.php?id=echolink" id="echolink">EchoLink</a> | ';
	    echo '<a class="hreflink" href="./editor.php?id=metarinfo" id="metarinfo">MetarInfo</a> | ';
	    echo '<a class="hreflink" href="./editor.php?id=nodeInfo" id="nodeInfo">NodeInfo</a> | ';
	    if ((defined('TclVoiceMail')) && (strncmp(TclVoiceMail, "no", 1) !== 0)) {
		echo '<a class="hreflink" href="./editor.php?id=tclvoicemail" id="tclvoicemail">TclVoiceMail</a> | ';
	    }
	}	
	if (defined('DL3EL_RADIO')) {
	    $svxRadio = DL3EL_RADIO;
	    if (($svxRadio == "Shari") || ($svxRadio == "RFGuru") || ($svxRadio == "Elenata")) {
		echo '<a class="hreflink" href="./caller.php?id=amixer/index" id="amixer">AMixer</a> | ';
//		echo '<a href="./editor.php?id=amixer" style = "color: black;" id="amixer">AMixer</a> | ';
		echo '<a class="hreflink" href="./rf.php" id="radio">Radio</a> | ';
	    }
// Soundcard?
	    if (substr($svxRadio,0,3) === "SC ") {
//		echo '<a href="./ameditor.php?id=amixer" style = "color: black;" id="amixer">1AMixer</a> | ';
//		echo '<a href="./caller.php?id=amixer/index&refresh=5" style = "color: black;" id="amixer">2AMixer</a> | ';
		echo '<a class="hreflink" href="./caller.php?id=amixer/index" id="amixer">AMixer</a> | ';
	    }
	}
//	echo '<a class="hreflink" href="./editor.php?id=power" style = "color: green;">Power</a></p>';
	echo '<a class="hreflink" href="./caller.php?id=power/index" >Power</a></p>';
//	echo '<a href="./caller_extern.php?id=http://relais.dl3el.de/FM-Funknetz/hilfe.html style = "color: #0000ff;">Hilfe</a> | ';
	if ((defined('DL3EL_APRS_MSG')) && (DL3EL_APRS_MSG === "yes")) {
	    echo '<a class="hreflink" href="./caller.php?id=aprs0" >APRS Monitor</a> ';
	    echo '<a style = "padding-right: 5px; text-align: right; " <a style = "color: black;"> <b>Display Logs:</b></a> ';
	    echo '<a class="hreflink" href="./caller.php?file=loga" id="log">APRS Log</a> | ';
//	    echo '<a href="./caller.php?file=msg" style = "color: black;" id="msg">APRS MSG</a> ';
	    if (file_exists(DL3EL.'/aprs-is-mqtt.msg')) {
		echo '<a class="hreflink" class="hreflink" href="./caller.php?file=logb" id="log">APRS MQTT | </a> ';
	    }
	} else {
	    echo '<a style = "padding-right: 5px; text-align: right; " <a style = "color: black;"> <b>Display Log:</b></a> ';
	}    
	if ((file_exists('/var/log/svxreflector')) || (file_exists('/var/log/svxreflector.log'))) {
	    echo '<a class="hreflink" href="./caller.php?file=log" id="log">L-Log</a> | ';
	    echo '<a class="hreflink" href="./caller.php?file=ref" id="log">R-Log</a> ';
	} else {
	    echo '<a class="hreflink" href="./caller.php?file=log" id="log">SVX Log</a> ';
	}
////// externe Links
	echo '<a style = "padding-right: 5px; text-align: right; " <a style = "color: black;">&nbsp; <b>External:</b></a> ';
	$max_links = 4;
	for ($i = 1; $i < $max_links+1; $i++) {
	    $link = "DL3EL_EXTERN{$i}";
	    $name = "DL3EL_EXTERN{$i}_NAME";
	    $tab = "DL3EL_EXTERN{$i}_TAB";
	    if (defined($link)) {
		$linkurl = constant($link);
		if (defined($name))  {
		    $linktext = constant($name);
		} else {
		    $linktext = $linkurl;
		}    
		if ((defined ('debug')) && (debug > 0)) echo "($i) L:$linkurl LN:$linktext<br>";
		if ((defined($tab)) && (constant($tab) === "new")) {
		    echo '<a class="hreflink" href="' . $linkurl . '" target="extern1" rel="noopener noreferrer">' . $linktext . '</a> ';
		} else{
		    echo '<a class="hreflink" href="./caller_extern.php?id=' . $linkurl . '" >' . $linktext . '</a> ';
		}    
		if ($i < $max_links) {
		    echo "|";
		}    
	    }
	}
///// Profile / reflektoren
	$refnr = 1;
	while ($refnr < 6) {
	    $ref_conf = DL3EL.'/Reflector' . $refnr . '.conf';
	    if (file_exists($ref_conf)) {
		if ($refnr === 1) {
		    echo '&nbsp;<a style = "padding-right: 5px; text-align: right; color: #000000;" <a style = "color: black;"> <b>Full</b> Edit:</a> ';
		} else {
		    echo '|';
		}   
		echo ' <a class="hreflink" href="./caller.php?file=' . $ref_conf . '" id="reflector1">Reflector' . $refnr . '.conf</a> ';
	    }
	    ++$refnr;
	}
        if ((defined('DL3EL_GIT_UPDATE')) && (DL3EL_GIT_UPDATE === "nocheck") && (defined('DL3EL_VERSION')) && (DL3EL_VERSION === "develop")) { 
            if (file_exists('./statistic.php')) {
		echo '| <a class="hreflink" href="./statistic.php" id="log">Statistic</a> ';
            }
        }
	$tgdb_File = DL3EL_BASE . "include/tgdb.php";
	if (filesize($tgdb_File)  <= 1000) {
	    echo '| <a class="hreflink" href="./include/tgdb_upd.php" id="upd">Update TGDB</a> ';
	}
	echo '</p>';
} else {
    	echo '<a class="hreflink" href="./caller_extern.php?id=http://relais.dl3el.de/FM-Funknetz/hilfe.html&wid=950&noauth" style = "text-align: left; border-radius:8px; color:white;border-color:transparent; background-color:blue; font-size:14px;" id="info">&nbsp;&nbsp;Hilfe &nbsp;&nbsp;</a>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;';
}


    echo '</div>';
    echo '<div id="display-links" align=center>';
//    echo '<a href="./edit.php?file=info" style = "border-radius:8px; color:white;border-color:transparent; background-color:blue; font-size:14px;" id="info">&nbsp;&nbsp;Neues in der Version ' . $dbversion . '&nbsp;&nbsp;</a>';
    echo '</div>';

    echo '</form>';


?>

