<?php
if (session_status() === PHP_SESSION_NONE) {
	session_start();
	include_once 'settings.php';
	include_once 'basedir.php';
	include_once 'config.php';
}

// Option Relais Dashboard
$public_relais = false;

// User Grundeinstellung			
$normal = true;				// Option Normal User
$expert = false;			// Option Expert User

// Option Develop
if (DL3EL_VERSION == "develop") {
	$develop = true;
} else {
	$develop = false;
}

// Option Reflector Config
$reflector = true;

// Option DX Cluster
if (DL3EL_DXCLUSTER == "yes") {
	$cluster = true;
}

// Option OpenWebRx
if (defined('DL3EL_OPENWEBRX')) {
	$owrx = true;
}

// Extert & Normal Button Logig
if (DL3EL_EXPERT == "yes") {
	$knowledgeFile = DL3EL . "/knowledge";
	if (file_exists($knowledgeFile)) {
		$knowledge = file_get_contents($knowledgeFile);
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
		file_put_contents($knowledgeFile, $knowledge);
	}

	if (isset($_POST['btn_normal'])) {
		define("DL3EL_EXPERT", "no");
		$knowledge = "Normal";
		file_put_contents($knowledgeFile, $knowledge);
	}

	if ($knowledge == "Expert") {
		$expert = true;
		$normal = false;
		$kn_exp = "<b>EXPERT</b>";
		$kn_exp_col = "green";
		$kn_nor = "NORMAL";
	} else {
		$expert = false;
		$normal = true;
		$kn_exp = "EXPERT";
		$kn_nor = "<b>NORMAL</b>";
		$kn_nor_col = "green";
	}
} else {
	$normal = true;
}


if (empty($_SESSION['auth'])) {
	$_SESSION['auth'] = "UNAUTHORISED";
}

///////////////////// Drop Down Menu ///////////////////////////////////////////////
echo '<br>';
// 1. Display Funktionen
function display($display)
{
	return $display;
}
if (isset($_POST['display'])) {
	$call_display = display($_POST['display']);
}

// 2. System Funktionen
function svxsystem($svxsystem)
{
	return $svxsystem;
}
if (isset($_POST['svxsystem'])) {
	$call_svxsystem = svxsystem($_POST['svxsystem']);
}

// 3. Expert Edit Funktionen
function expertedit($expertedit)
{
	return $expertedit;
}
if (isset($_POST['expertedit'])) {
	$call_expertedit = expertedit($_POST['expertedit']);
}

// 4. Normal Edit Funktionen
function normaledit($normaledit)
{
	return $normaledit;
}
if (isset($_POST['normaledit'])) {
	$call_normaledit = normaledit($_POST['normaledit']);
}

// 5. Refector Config Funktionen
function refconf($refconf)
{
	return $refconf;
}
if (isset($_POST['refconf'])) {
	$call_refconf = refconf($_POST['refconf']);
}

// 6. Log Files Funktionen
function logdat($logdat)
{
	return $logdat;
}
if (isset($_POST['logdat'])) {
	$call_logdat = refconf($_POST['logdat']);
}


////////////// Option Dashboard für Relais eingeschränktes Menu ///////////////////////////////////////////////////////////////////////////////////
if ($public_relais == true) {
	// Dashboard Button
	echo '<span style="font-size:18px;font-weight:bold;">Display:&nbsp;&nbsp;&nbsp;<span>';
	echo '<a href="./index.php" style="margin-right:10px;font-size:18px;color:#CD6600" target="_top">Dashboard></a>| ';
	echo '<a href="./caller.php?id=include/svxdxc&refresh=15" style = "font-size:18px;color: #0000ff;" target="_top">&nbsp;DX</a>';
} else {

////////////// Option Standart Dashboard mit Drop Down Menu ////////////////////////////////////////////////////////////////////////////////////////
// Help & Info Button
	echo '<form method="post" action="' . htmlspecialchars($_SERVER["PHP_SELF"]) . '"> ';
	list($version, $rest) = explode(" ", $dbversion);

	$dbversionFile = DL3EL . "/dbversion.upd";
	if (file_exists($dbversionFile)) {
		$content = file_get_contents($dbversionFile);
		if ($content === "update") {
			$version = $version . " Update available";
		}
	}
	echo '<a href="./caller_extern.php?id=http://relais.dl3el.de/FM-Funknetz/hilfe.html&wid=950"
                        style="text-align:left;margin-right:20px;border-radius:8px;color:white;
                        border-color:transparent;background-color:blue;font-size:20px;"
                        id="info">Hilfe für ' . $version . '</a>';

	// Dashboard return Button
	echo '<a href="./index.php" style="margin-right:10px;font-size:18px;color:#CD6600" target="_top">Dashboard</a>';

/////////////////////// Drop Down Menu Optionen ////////////////////////////////////
	echo '<form method="post" action="">';

	// Buttons 1 Display Funktionen
	echo '<select style="font-size:16px;" name="display" onchange="this.form.submit()">';
	echo '<option value="" hidden disabled selected>DISPLAY</option>';
	echo '<option value="2">Talk Groups</option>';
	echo '<option value="3">Monitor Calls</option>';
	echo '<option value="4">Echolink</option>';
	echo '<option value="5">FM Relais</option>';
	if ($cluster == true) {
		echo '<option value="6">DX Cluster</option>';
	}
	echo '<option value="9">APRS Monitor</option>';

	if ($owrx == true) {
		echo '<option value="7">OpenWebRx</option>';
	}
	echo '<option value="8">Voice FMN</option>';

	// Extern 1
	if (defined('DL3EL_EXTERN1')) {
		if (defined('DL3EL_EXTERN1_NAME')) {
			$linktext = DL3EL_EXTERN1_NAME;
		} else {
			$linktext = DL3EL_EXTERN1;
		}
		echo "<option value='10'>$linktext</option>";
	}

	// Extern 2
	if (defined('DL3EL_EXTERN2')) {
		if (defined('DL3EL_EXTERN2_NAME')) {
			$linktext = DL3EL_EXTERN2_NAME;
		} else {
			$linktext = DL3EL_EXTERN2;
		}
		echo "<option value='11'>$linktext</option>";
	}

	// Extern 3
	if (defined('DL3EL_EXTERN3')) {
		if (defined('DL3EL_EXTERN3_NAME')) {
			$linktext = DL3EL_EXTERN3_NAME;
		} else {
			$linktext = DL3EL_EXTERN3;
		}
		echo "<option value='12'>$linktext</option>";
	}

	// Extern 4
	if (defined('DL3EL_EXTERN4')) {
		if (defined('DL3EL_EXTERN4_NAME')) {
			$linktext = DL3EL_EXTERN4_NAME;
		} else {
			$linktext = DL3EL_EXTERN4;
		}
		echo "<option value='13'>$linktext</option>";
	}

	echo '</select>';

	// Buttons 2 System Eintellunegen
	echo '<select style="font-size:16px;" name="svxsystem" onchange="this.form.submit()">';
	echo '<option value="" hidden disabled selected>SYSTEM</option>';
	echo '<option value="1">Wifi</option>';
	if ($develop == true) {
		echo '<option value="2">Network</option>';
	}
	echo '<option value="3">Backup & Restore</option>';
	echo '</select>';

	// Buttons 3 Expert Edit
	if ($expert == true) {
		echo '<select style="font-size:16px;" name="expertedit" onchange="this.form.submit()">';
		echo '<option value="" hidden disabled selected>EXPERT EDIT</option>';
		echo '<option value="1">SVXLink Conf</option>';
		echo '<option value="2">Echolink Conf</option>';
		echo '<option value="3">Metar Info Conf</option>';
		echo '<option value="4">APRS Conf</option>';
		echo '<option value="5">AMixer</option>';
		echo '<option value="6">Radio</option>';
		echo '<option value="7">Node Info</option>';
		echo '<option value="8">config.php</option>';
		echo '</select>';
	}

	// Buttons 4 Normal Edit
	if ($normal == true) {
		echo '<select style="font-size:16px;" name="normaledit" onchange="this.form.submit()">';
		echo '<option value="" hidden disabled selected>NORMAL EDIT</option>';
		echo '<option value="1">SVXLink Conf</option>';
		echo '<option value="2">Echolink Conf</option>';
		echo '<option value="3">Metar Info Conf</option>';
		echo '<option value="4">AMixer</option>';
		echo '<option value="5">Radio</option>';
		echo 'option value="6">Node Info</option>';
		//  echo '<option value="7">TCL Voice Mail</option>';
		echo '</select>';
	}

	// Buttons 5 Reflector Config
	if ($reflector == true) {
		echo '<select style="font-size:16px;" name="refconf" onchange="this.form.submit()">';
		echo '<option value="" hidden disabled selected>REF CONF</option>';
		echo '<option value="1">REF 1</option>';
		echo '<option value="2">Ref 2</option>';
		echo '<option value="3">Ref 3</option>';
		echo '<option value="4">Ref 4</option>';
		echo '</select>';
	}

	// Buttons 6 Log Dateien
	echo '<select style="font-size:16px;" name="logdat" onchange="this.form.submit()">';
	echo '<option value="" hidden disabled selected>LOG FILES</option>';
	echo '<option value="1">SVXLink Live Log</option>';
	echo '<option value="2">SVXLink Log</option>';
	echo '<option value="3">APRS Log</option>';
	echo '</select>';

	echo '<noscript><input type="submit" value="send"></noscript>';

	// Button Authorise
	if ((defined('MENUTOPAUTH')) && (MENUTOPAUTH === "no")) {
		$show_auth = 0;
		// do not show Authorise
	} else {
		$show_auth = 1;
	}
	if ((defined('debug')) && (debug > 0))
		echo "<br>ShowAUTH: $show_auth";
	$ip = isset($_SERVER['REMOTE_ADDR']) ? $_SERVER['REMOTE_ADDR'] : '0';
	;
	if ((defined('debug')) && (debug > 0))
		echo "<br>RemoteIP: $ip";
	if (defined('DL3EL_MASTER_IP')) {
		$master_ip_array = explode(",", DL3EL_MASTER_IP);
		$nn = 0;
		foreach ($master_ip_array as $master_ip) {
			if ((defined('debug')) && (debug > 0))
				echo "<br>IP($nn):$master_ip ($ip)";
			if (cidr_match($ip, $master_ip)) {
				$_SESSION['auth'] = "AUTHORISED";
				$show_auth = 1;
				break;
			}
			++$nn;
		}
	}

	// Normal / Expert Button

	if (DL3EL_EXPERT == "yes") {
		echo '<button style="color:' . $kn_exp_col . ';font-size:16px;" name="btn_expert" type="submit" >' . $kn_exp . '</button>';
		echo '<button style="color:' . $kn_nor_col . ';font-size:16px;" name="btn_normal" type="submit" >' . $kn_nor . '</button>';
	}

	// Auth Button
	if ($show_auth) {
		echo '<a href="./authorise.php" style="color:crimson;margin-left:10px;font-size:18px;">Authorise</a>';
	}

	// Button Power
//	echo '<a style="color:green;margin-left:10px;font-size:18px;" href="./editor.php?id=power">Power</a>';
	echo '<a style="color:green;margin-left:10px;font-size:18px;" href="./caller.php?id=power/index" >Power</a>';
	echo '</form>';
	echo '<br>';
	echo '<div id="display-links" align=center>';

//////////////////// Drop Down Menu Funktionen ////////////////////////////////////
///////////////////// DISPLAY ///////////////////////////////////////
/*
	// Dashboard
	if ($call_display == 1) {
		$site = './index.php';
		echo '<meta http-equiv="refresh" content="0;url=' . htmlspecialchars($site) . '">';
	}
*/

	// Talk Groups
	if ($call_display == 2) {
		$site = './caller.php?id=include/tg';
		echo '<meta http-equiv="refresh" content="0;url=' . htmlspecialchars($site) . '">';
	}

	// Monitor Calls
	if ($call_display == 3) {
		$site = './caller.php?id=monitor0';
		echo '<meta http-equiv="refresh" content="0;url=' . htmlspecialchars($site) . '">';
	}

	// Echolink
	if ($call_display == 4) {
		$site = './caller.php?id=echolink_dl3el0';
		echo '<meta http-equiv="refresh" content="0;url=' . htmlspecialchars($site) . '">';
	}

	// FM Relais
	if ($call_display == 5) {
		$site = './caller.php?id=relais0';
		echo '<meta http-equiv="refresh" content="0;url=' . htmlspecialchars($site) . '">';
	}

	// DX Cluster
	if ($call_display == 6) {
		$site = './caller.php?id=include/svxdxc&refresh=15';
		echo '<meta http-equiv="refresh" content="0;url=' . htmlspecialchars($site) . '">';
	}

	// OpenWebRx
	if ($call_display == 7) {
		$site = './caller_extern.php?id=https://websdr.z-05.de/#freq=144800000,mod=empty,secondary_mod=packet,sql=-150';
		echo '<meta http-equiv="refresh" content="0;url=' . htmlspecialchars($site) . '">';
	}

	// FM Funknetz Stream
	if ($call_display == 8) {
		$site = './caller_extern.php?id=https://stream.fm-funknetz.de';
		echo '<meta http-equiv="refresh" content="0;url=' . htmlspecialchars($site) . '">';
	}

	// APRS Monitor
	if ($call_display == 9) {
		$site = './caller.php?id=aprs';
		echo '<meta http-equiv="refresh" content="0;url=' . htmlspecialchars($site) . '">';
	}

	// Extern 1
	if ($call_display == 10) {
		if (defined('DL3EL_EXTERN1')) {
			$site = './caller_extern.php?id=' . DL3EL_EXTERN1 . '';
			echo '<meta http-equiv="refresh" content="0;url=' . htmlspecialchars($site) . '">';
		}
	}

	// Extern 2
	if ($call_display == 11) {
		if (defined('DL3EL_EXTERN2')) {
			$site = './caller_extern.php?id=' . DL3EL_EXTERN2 . '';
			echo '<meta http-equiv="refresh" content="0;url=' . htmlspecialchars($site) . '">';
		}
	}

	// Extern 3
	if ($call_display == 12) {
		if (defined('DL3EL_EXTERN3')) {
			$site = './caller_extern.php?id=' . DL3EL_EXTERN3 . '';
			echo '<meta http-equiv="refresh" content="0;url=' . htmlspecialchars($site) . '">';
		}
	}

	// Extern 4
	if ($call_display == 13) {
		if (defined('DL3EL_EXTERN4')) {
			$site = './caller_extern.php?id=' . DL3EL_EXTERN4 . '';
			echo '<meta http-equiv="refresh" content="0;url=' . htmlspecialchars($site) . '">';
		}
	}

///////////////////// SYSTEM ///////////////////////////////////////
	// Wifi
	if ($call_svxsystem == 1) {
		$site = './wifi.php';
		echo '<meta http-equiv="refresh" content="0;url=' . htmlspecialchars($site) . '">';
	}

	// Network
	if ($call_svxsystem == 2) {
		$site = './network.php';
		echo '<meta http-equiv="refresh" content="0;url=' . htmlspecialchars($site) . '">';
	}

	// Backup & Restore
	if ($call_svxsystem == 3) {
		$site = './config_backup.php';
		echo '<meta http-equiv="refresh" content="0;url=' . htmlspecialchars($site) . '">';
	}

///////////////////// EXPERT EDIT ///////////////////////////////////////
	// SVXLink Config
	if ($call_expertedit == 1) {
		$file_path = SVXCONFPATH;
		$file_edit = $file_path . 'svxlink.conf';
		$site = "edit.php?file=$file_edit";
		echo '<meta http-equiv="refresh" content="0;url=' . htmlspecialchars($site) . '">';
	}

	// Echolink Config
	if ($call_expertedit == 2) {
		$file_path = MODULEPATH;
		$file_edit = $file_path . 'ModuleEchoLink.conf';
		$site = "edit.php?file=$file_edit";
		echo '<meta http-equiv="refresh" content="0;url=' . htmlspecialchars($site) . '">';
	}

	// MetarInfo Config
	if ($call_expertedit == 3) {
		$file_path = MODULEPATH;
		$file_edit = $file_path . 'ModuleMetarInfo.conf';
		$site = "edit.php?file=$file_edit";
		echo '<meta http-equiv="refresh" content="0;url=' . htmlspecialchars($site) . '">';
	}

	// APRS Config
	if ($call_expertedit == 4) {
		$file_path = DL3EL;
		$file_edit = $file_path . '/aprs-is-msg.conf';
		$site = "edit.php?file=$file_edit";
		echo '<meta http-equiv="refresh" content="0;url=' . htmlspecialchars($site) . '">';
	}

	// AMixer
	if ($call_expertedit == 5) {
		$site = './caller.php?id=amixer/index';
		echo '<meta http-equiv="refresh" content="0;url=' . htmlspecialchars($site) . '">';
	}

	// Radio
	if ($call_expertedit == 6) {
		$site = './rf.php';
		echo '<meta http-equiv="refresh" content="0;url=' . htmlspecialchars($site) . '">';
	}

	// Node Info
	if ($call_expertedit == 7) {
		$file_path = SVXCONFPATH;
		$file_edit = $file_path . 'node_info.json';
		$site = "edit.php?file=$file_edit";
		echo '<meta http-equiv="refresh" content="0;url=' . htmlspecialchars($site) . '">';
	}

	// config.php
	if ($call_expertedit == 8) {
		$file_path = DL3EL_BASE;
		$file_edit = $file_path . 'include/config.php';
		$site = "edit.php?file=$file_edit";
		echo '<meta http-equiv="refresh" content="0;url=' . htmlspecialchars($site) . '">';
	}

///////////////////// NORMAL EDIT ///////////////////////////////////////
	//SVXLink Conig
	if ($call_normaledit == 1) {
		$site = './editor.php?id=svxlink';
		echo '<meta http-equiv="refresh" content="0;url=' . htmlspecialchars($site) . '">';
	}

	// Echolink
	if ($call_normaledit == 2) {
		$site = './editor.php?id=echolink';
		echo '<meta http-equiv="refresh" content="0;url=' . htmlspecialchars($site) . '">';
	}

	// Metar Info
	if ($call_normaledit == 3) {
		$site = './editor.php?id=metarinfo';
		echo '<meta http-equiv="refresh" content="0;url=' . htmlspecialchars($site) . '">';
	}

	// AMixer
	if ($call_normaledit == 4) {
		$site = './caller.php?id=amixer/index';
		echo '<meta http-equiv="refresh" content="0;url=' . htmlspecialchars($site) . '">';
	}

	// Radio
	if ($call_normaledit == 5) {
		$site = './rf.php';
		echo '<meta http-equiv="refresh" content="0;url=' . htmlspecialchars($site) . '">';
	}

	// Node Info
	if ($call_normaledit == 6) {
		$site = './editor.php?id=nodeInfo';
		echo '<meta http-equiv="refresh" content="0;url=' . htmlspecialchars($site) . '">';
	}

/////////////////// Reflektor Edit //////////////////////////////////////
	// REF 1
	if ($call_refconf == 1) {
		$file_path = DL3EL;
		$file_edit = $file_path . '/Reflector1.conf';
		$site = "edit.php?file=$file_edit";
		echo '<meta http-equiv="refresh" content="0;url=' . htmlspecialchars($site) . '">';
	}

	// REF 2
	if ($call_refconf == 2) {
		$file_path = DL3EL;
		$file_edit = $file_path . '/Reflector2.conf';
		$site = "edit.php?file=$file_edit";
		echo '<meta http-equiv="refresh" content="0;url=' . htmlspecialchars($site) . '">';
	}

	// REF 3
	if ($call_refconf == 3) {
		$file_path = DL3EL;
		$file_edit = $file_path . '/Reflector3.conf';
		$site = "edit.php?file=$file_edit";
		echo '<meta http-equiv="refresh" content="0;url=' . htmlspecialchars($site) . '">';
	}

	// REF 4
	if ($call_refconf == 4) {
		$file_path = DL3EL;
		$file_edit = $file_path . '/Reflector4.conf';
		$site = "edit.php?file=$file_edit";
		echo '<meta http-equiv="refresh" content="0;url=' . htmlspecialchars($site) . '">';
	}

////////////////// Anzeige Log Files  //////////////////////////////////////
	// SVXLink Live Log
	if ($call_logdat == 1) {
		$site = './editor.php?id=log';
		echo '<meta http-equiv="refresh" content="0;url=' . htmlspecialchars($site) . '">';
	}

	// SVXLink Log
	if ($call_logdat == 2) {
		$site = 'edit.php?file=log';
		echo '<meta http-equiv="refresh" content="0;url=' . htmlspecialchars($site) . '">';
	}

	// APRS Log
	if ($call_logdat == 3) {
		$site = 'edit.php?file=loga';
		echo '<meta http-equiv="refresh" content="0;url=' . htmlspecialchars($site) . '">';
	}
}

?>
