<?php
if (session_status() === PHP_SESSION_NONE) {
    session_start();
include_once 'settings.php';	
}

/*
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
*/   
   
///////////////////// Klappleiste ////////////////////////////////////
echo '<br>';
// 1. Display Funktionen
function display($display) {
    return $display;
}
    if (isset($_POST['display'])) {
    $call_display = display($_POST['display']);
}

// 2. System Funktionen
function svxsystem($svxsystem) {
    return $svxsystem;
}
    if (isset($_POST['svxsystem'])) {
    $call_svxsystem = svxsystem($_POST['svxsystem']);
}

// 3. Expert Edit Funktionen
function expertedit($expertedit) {
    return $expertedit;
}
    if (isset($_POST['expertedit'])) {
    $call_expertedit = expertedit($_POST['expertedit']);
}

// 4. Normal Edit Funktionen
function normaledit($normaledit) {
    return $normaledit;
}
    if (isset($_POST['normaledit'])) {
    $call_normaledit = normaledit($_POST['normaledit']);
}

// 5. Refector Config Funktionen
function refconf($refconf) {
    return $refconf;
}
    if (isset($_POST['refconf'])) {
    $call_refconf = refconf($_POST['refconf']);
}

// 6. Log Files Funktionen
function logdat($logdat) {
    return $logdat;
}
    if (isset($_POST['logdat'])) {
    $call_logdat = refconf($_POST['logdat']);
}

// Hilfe & Info Button
	echo '<form method="post" action="' . htmlspecialchars($_SERVER["PHP_SELF"]) .'"> ';
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
?>

<!-- ///////////////////// Klappleiste Definitionen //////////////////////////////////// -->

<form method="post" action="">

<!-- Buttons 1 Display Funktionen -->
<select style="font-size:16px;" name="display" onchange="this.form.submit()">
  <option style="color:gray;" value="0">DISPLAY</option>
  <option value="1">Dashboard</option>
  <option value="2">Talk Groups</option>
  <option value="3">Monitor Calls</option>
  <option value="4">Echolink</option>
  <option value="5">FM Relais</option>
  <option value="6">DX Cluster</option>
  <option value="7">OpenWebRx</option>
  <option value="8">Voice FMN</option>
  <option value="9">APRS Monitor</option>
<!--  <option value="10">Live Log View</option> -->
</select>
<noscript><input type="submit" value="send"></noscript>

<!-- Buttons 2 System Eintellunegen -->
<select style="font-size:16px;" name="svxsystem" onchange="this.form.submit()">
  <option style="color:gray;" value="0">SYSTEM</option>
  <option value="1">Wifi</option>
  <option value="2">Network</option>
<!--  <option value="3">Backup & Restore</option> -->
</select>
<noscript><input type="submit" value="send"></noscript>

<!-- Buttons 3 Expert Edit -->
<select style="font-size:16px;" name="expertedit" onchange="this.form.submit()">
  <option style="color:gray;" value="0">EXPERT EDIT</option>
  <option value="1">SVXLink Conf</option>
  <option value="2">Echolink Conf</option>
  <option value="3">Metar Info Conf</option>
  <option value="4">APRS Conf</option>
  <option value="5">AMixer</option>
  <option value="6">Radio</option>
  <option value="7">Node Info</option>
  <option value="8">config.php</option>
</select>
<noscript><input type="submit" value="send"></noscript>

<!-- Buttons 4 Normal Edit -->
<select style="font-size:16px;" name="normaledit" onchange="this.form.submit()">
  <option style="color:gray;" value="0">NORMAL EDIT</option>
  <option value="1">SVXLink Conf</option>
  <option value="2">Echolink Conf</option>
  <option value="3">Metar Info Conf</option>
  <option value="4">AMixer</option>
  <option value="5">Radio</option>
  <option value="6">Node Info</option>
<!--  <option value="7">TCL Voice Mail</option> -->
  
</select>
<noscript><input type="submit" value="send"></noscript>

<!-- Buttons 5 Reflector Config-->
<select style="font-size:16px;" name="refconf" onchange="this.form.submit()">
  <option style="color:gray;" value="0">REF CONF</option>
  <option value="1">REF 1</option>
  <option value="2">Ref 2</option>
  <option value="3">Ref 3</option>
  <option value="4">Ref 4</option>
</select>
<noscript><input type="submit" value="send"></noscript>

<!-- Buttons 6 Log Dateien -->
<select style="font-size:16px;" name="logdat" onchange="this.form.submit()">
  <option style="color:gray;" value="0">LOG FILES</option>
  <option value="1">SVXLink Live Log</option>
  <option value="2">SVXLink Log</option>
  <option value="3">APRS Log</option>
</select>
<noscript><input type="submit" value="send"></noscript>

<?php
//Backup & Restore Button
echo '<a href="./config_backup.php" style = "margin-left:10px;font-size:18px;color:#cd6600;" target="_top">Backup</a> ';

// Button Authorise
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
if ($show_auth) {
    echo '<a href="./authorise.php" style="color:crimson;margin-left:10px;font-size:18px;">Authorise</a>';
}

// Button Power	
echo '<a style="color:green;margin-left:10px;font-size:18px;" href="./editor.php?id=power">Power</a>';
echo '</form>';
echo '<br>';
echo '<div id="display-links" align=center>';
	
///////////////////// Klappleiste Funktionen ////////////////////////////////////
///////////////////// DISPLAY ///////////////////////////////////////
// Dashboard
if ($call_display == 1) {
include "./index.php";
}

// Talk Groups
if ($call_display == 2) {
$_GET['id'] = 'include/tg';
include 'caller.php';
}

// Monitor Calls
if ($call_display == 3) {
$_GET['id'] = 'monitor0';
include 'caller.php';
}

// Echolink
if ($call_display == 4) {
$_GET['id'] = 'echolink_dl3el0';
include 'caller.php';
}

// FM Relais
if ($call_display == 5) {
$_GET['id'] = 'relais0';
include 'caller.php';
}

// DX Cluster
/// Problem Timer ///
if ($call_display == 6) {
if ((defined('DL3EL_DXCLUSTER')) && (DL3EL_DXCLUSTER === "yes")) {
	$_GET['id'] = "dxcluster";
//	$file_ref = "include/svxdxc&refresh=15";
//	$_GET['id'] = $file_ref;
//	$_GET['refresh'] = "15";
	include './caller.php';
	}
}

// OpenWebRx
if ($call_display == 7) {
	if (defined('DL3EL_OPENWEBRX')) {
	$_GET['id'] = 'https://websdr.z-05.de/#freq=144800000,mod=empty,secondary_mod=packet,sql=-150';
	include 'caller_extern.php';	
	}    
}

// FM Funknetz Stream
if ($call_display == 8) {
	if ((defined('DL3EL_FMFUNKNETZ')) && (DL3EL_FMFUNKNETZ === "yes")) {
		$_GET['id'] = 'https://stream.fm-funknetz.de';
		include 'caller_extern.php';	
	}    
}

// APRS Monitor
if ($call_display == 9) {
	$_GET['id'] = 'aprs';
	include 'caller.php';
}

///////////////////// SYSTEM ///////////////////////////////////////
// Wifi 
if ($call_svxsystem == 1) {
include "./wifi.php";	
}

// Network 
if ($call_svxsystem == 2) {
include "./network.php";	
}

/*
// Backup & Restore 
if ($call_svxsystem == 3) {
echo '<object style="outline:none; width:750; height:780;" data="http://./config_backup.php"></object>';	
}
*/

///////////////////// EXPERT EDIT ///////////////////////////////////////

// SVXLink Config
if ($call_expertedit == 1) {
// . SVXCONFPATH . SVXCONFIG . muss überall umgesetzt werden
$file_edit="/etc/svxlink/svxlink.conf";
$_GET['file'] = $file_edit;
include './edit.php';  
}

// Echolink Config
if ($call_expertedit == 2) {

$file_edit="/etc/svxlink/svxlink.d/ModuleEchoLink.conf";
$_GET['file'] = $file_edit;
include './edit.php';  
}

// MetarInfo Config
if ($call_expertedit == 3) {

$file_edit="/etc/svxlink/svxlink.d/ModuleMetarInfo.conf";
$_GET['file'] = $file_edit;
include './edit.php';  
}

// APRS Config
if ($call_expertedit == 4) {

$file_edit="/var/www/html/TEST/dl3el/aprs-is-msg.conf";
$_GET['file'] = $file_edit;
include './edit.php';  
}

// AMixer
if ($call_expertedit == 5) {
$_GET['id'] = 'amixer/index';
include './caller.php';  
}

// Radio
if ($call_expertedit == 6) {
include './rf.php';  
}

// Node Info
if ($call_expertedit == 7) {

$file_edit="/etc/svxlink/node_info.json";
$_GET['file'] = $file_edit;
include './edit.php';  

// config.php
$file_edit=DL3EL_BASE . "include/config.pgp";
$_GET['file'] = $file_edit;
include './edit.php';  

//echo '<a href="./edit.php?file=' . DL3EL_BASE . 'include/config.php" style = "color: black;" id="configphp">config.php</a> | ';

}

///////////////////// NORMAL EDIT ///////////////////////////////////////

//SVXLink Conig
if ($call_normaledit == 1) {
	$_GET['id'] = 'svxlink';
	include './editor.php';  
}

// Echolink
if ($call_normaledit == 2) {
	$_GET['id'] = 'echolink';
	include './editor.php';  
}

// Metar Info
if ($call_normaledit == 3) {
	$_GET['id'] = 'amixer/index';
	include './caller.php';  
}

// AMixer
if ($call_normaledit == 4) {
	$_GET['id'] = 'amixer/index';
	include './caller.php';  
}

// Radio
if ($call_normaledit == 5) {
	include './rf.php'; 
}

// Node Info
if ($call_normaledit == 6) {
	$_GET['id'] = 'nodeInfo';
	include './editor.php';  
}

// TCL Voice Mail
//if ($call_normaledit == 7) {
//	$_GET['id'] = 'tclvoicemail';
//	include './editor.php';  
//}

/////////////////// Reflektor Edit //////////////////////////////////////

// REF 1
if ($call_refconf == 1) {
$file_edit="/var/www/html/TEST/dl3el/Reflector1.conf";
$_GET['file'] = $file_edit;
include './edit.php';
}

// REF 2
if ($call_refconf == 2) {
$file_edit="/var/www/html/TEST/dl3el/Reflector2.conf";
$_GET['file'] = $file_edit;
include './edit.php';
}

// REF 3
if ($call_refconf == 3) {
$file_edit="/var/www/html/TEST/dl3el/Reflector3.conf";
$_GET['file'] = $file_edit;
include './edit.php';
}

// REF 4
if ($call_refconf == 4) {
$file_edit="/var/www/html/TEST/dl3el/Reflector4.conf";
$_GET['file'] = $file_edit;
include './edit.php';
}

////////////////// Anzeige Log Files  //////////////////////////////////////

// SVXLink Live Log
if ($call_logdat == 1) {
	if ($_SESSION['auth'] === "AUTHORISED") {
	$_GET['id'] = 'log';
	include './editor.php';
	}	
}

// SVXLink Log
if ($call_logdat == 2) {
	$file_show="log";
	$_GET['file'] = $file_show;
	include_once './edit.php';
}

// APRS Log
if ($call_logdat == 3) {
	$file_show="loga";
	$_GET['file'] = $file_show;
	include_once './edit.php';
}


?>

