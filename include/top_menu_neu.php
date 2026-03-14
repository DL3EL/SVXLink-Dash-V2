<?php
if (session_status() === PHP_SESSION_NONE) {
    session_start();
    include_once 'settings.php';
    include_once 'basedir.php';
    include_once 'config.php';
}

//////////////////////////////////////////////////////////////
// TOP MENÜ EIN- & AUSBLENDEN
//////////////////////////////////////////////////////////////
$show_top_menu = 0;

if((defined('MENUTOP') && MENUTOP==="FULL") || $_SESSION['auth']==="AUTHORISED"){
    $show_top_menu = 1;
}
//$show_top_menu = 0;
if($show_top_menu == 1) {

//////////////////////////////////////////////////////////////
// ALLGEMEINE LOGIK
//////////////////////////////////////////////////////////////
if (!defined('MENUBUTTON'))
    define("MENUBUTTON", "no button");

$knowledgeFile = DL3EL . "/knowledge";

if (empty($_SESSION['auth'])) {
    $_SESSION['auth'] = "UNAUTHORISED";
}

// Mobile Dashboard
$mobil = false;
if (file_exists(DL3EL_BASE . "/mobile/index.php")) {
    $mobil = true;
}

// UK oder FM Funknetz
$check = function_exists('check_network')
    ? check_network($fmnetwork, "uk.wide.svxlink.uk")
    : $fmnetwork === "uk.wide.svxlink.uk";
$monuknet = $check;
$monfmnet = !$check;

// SVX Cube
if (file_exists('/opt/svxlink_mqtt/svxlink_mqtt_controller.py')) {
    $cubeip = str_replace(' ', '<br />', exec('hostname -I | awk \'{print $1}\''));
	$cubeip = "http://" . $cubeip . ":8081";
    $cube01 = true;
	} else {
	    if (defined('DL3EL_SVXCUBE'))  {
	$cube02 = true;
	    }    
	}

// DVS
	if ((defined('DL3EL_VERSION')) && (strncmp(DL3EL_VERSION, "develop", 7) === 0) && (file_exists('/usr/bin/dvs'))) {
		$dvs = true;
	}
	
// Reflector
	if ((file_exists('/var/log/svxreflector')) || (file_exists('/var/log/svxreflector.log'))) {
		$local_ref = true;
	}

	if ((file_exists('/var/log/svxreflector')) || (file_exists('/var/log/svxreflector.log'))) {
		$l_log = true;
		$r_log = true;
	} 

// Shell
	if ((file_exists('/etc/default/shellinabox')) && ((defined('DL3EL_SSH')) && (strncmp(DL3EL_SSH, "yes", 3) === 0))) {
		$getPortCommand = "grep -m 1 'SHELLINABOX_PORT=' /etc/default/shellinabox | awk -F '=' '/SHELLINABOX_PORT=/ {print $2}'";
		$shellPort = exec($getPortCommand);    
		if ($shellPort == 4200) {
			$shell_box = true;
		    //echo '<a class="hreflink" href="./ssh.php" >SSH</a> | ';
		}
	}	

// TclVoiceMail Config
	if ((defined('TclVoiceMail')) && (strncmp(TclVoiceMail, "no", 1) !== 0)) {
		//echo '<a class="hreflink" href="./caller.php?file=TclVoiceMail.conf" id="tclvoicemail">TclVoiceMail</a> | ';
		$voice_mail = true;
	    }
 
//////////////////////////////////////////////////////////////
// ROUTING FUNKTION
//////////////////////////////////////////////////////////////
function route($post, $routes)
{
    if (!isset($_POST[$post]))
        return;
    $val = $_POST[$post];
    if (isset($routes[$val])) {
        $target = $routes[$val];
        if (is_callable($target)) {
            $target();
        } else {
            header("Location: $target");
        }
        exit;
    }
}

//////////////////////////////////////////////////////////////
// OPTIONEN AUS CONFIG.PHP
//////////////////////////////////////////////////////////////
$clock =   defined('DL3EL_OPENHAMCLOCK') && DL3EL_OPENHAMCLOCK == "yes";
$cluster = defined('DL3EL_DXCLUSTER') && DL3EL_DXCLUSTER == "yes";
$owrx =    defined('DL3EL_OPENWEBRX');
$livedb =  defined('DL3EL_LIVEDB') && DL3EL_LIVEDB == "top";
$shari =   defined('DL3EL_RADIO') && DL3EL_RADIO == "Shari";
$guru =    defined('DL3EL_RADIO') && DL3EL_RADIO == "RFGuru";

$livedb01 = false;
$livedb02 = false;
$livedb03 = false;
if (defined('DL3EL_LIVEDB') && DL3EL_LIVEDB === "top") {
    if (!defined('DL3EL_MQTT')) {
        $livedb03 = true;
    } elseif (DL3EL_MQTT === "no") {
        $livedb01 = true;
    } else {
        $livedb02 = true;
    }
}

$reflector =
    defined("DL3EL_REF1_BUTTON") ||
    defined("DL3EL_REF2_BUTTON") ||
    defined("DL3EL_REF3_BUTTON") ||
    defined("DL3EL_REF4_BUTTON");

if (!defined('SHOWPTT')) {
    define("SHOWPTT", "TRUE");
}
if (!defined('MENUBUTTON')) {
    define("MENUBUTTON", "no button");
}
if (!defined('DL3EL_RADIO')) {
    define("DL3EL_RADIO", "no Radio");
}
if (!defined('DL3EL_LIVEDB_AUTO')) {
    define("DL3EL_LIVEDB_AUTO", "yes");
}

//////////////////////////////////////////////////////////////
// NORMAL / EXPERT MODUS
//////////////////////////////////////////////////////////////
$knowledge = "Normal";
if (isset($_POST['btn_expert'])) {
    $knowledge = "Expert";
    file_put_contents($knowledgeFile, $knowledge);
} elseif (isset($_POST['btn_normal'])) {
    $knowledge = "Normal";
    file_put_contents($knowledgeFile, $knowledge);
} elseif (file_exists($knowledgeFile)) {
    $knowledge = file_get_contents($knowledgeFile);
}

$expert = ($knowledge === "Expert");
$normal = !$expert;
$kn_exp = $expert ? "<b>EXPERT</b>" : "EXPERT";
$kn_nor = $normal ? "<b>NORMAL</b>" : "NORMAL";
$kn_exp_col = $expert ? "green" : "";
$kn_nor_col = $normal ? "green" : "";

//////////////////////////////////////////////////////////////
// DISPLAY ROUTING
//////////////////////////////////////////////////////////////
// Inline Links
$displayRoutes = [
    2 => './caller.php?id=include/tg',
    3 => file_exists("/usr/local/bin/mqtt-simple")
        ? './caller.php?id=monitor&refresh=15'
        : './caller.php?id=monitor0',
    4 => './caller.php?id=echolink_dl3el0',
    5 => './caller.php?id=relais0',
    6 => './caller.php?id=include/svxdxc&refresh=15',
    7 => './caller.php?id=aprs',
    8 => './caller_extern.php?id=https://websdr.z-05.de/#freq=144800000',
    9 => './caller_extern.php?id=https://stream.fm-funknetz.de',
    10 => './caller_extern.php?id=https://qsolink.websdrbordeaux.fr/dashboard.html',
    11 => 'https://openhamclock.com/',
    12 => './caller_extern.php?id=https://chat.fm-funknetz.de/index.php?call=' . $callsign,
    13 => './svx2mqtt.php',
    14 => './caller.php?id=svx2mqtt/index_neu&refresh=1',	
    15 => './svx2mqtt.php',
	16 => './caller_extern.php?id=https://ukwide.svxlink.net'
	
];
// derzeit maimal 30 Links Inline möglich

// derzeit max 10 externe Links im neuen TAB
$max_links = 10;
for ($i = 1; $i < $max_links + 1; $i++) {
    $const = "DL3EL_EXTERN" . $i;
    if (defined($const)) {
        $displayRoutes[30 + $i] = './caller_extern.php?id=' . constant($const);
        // Links erhalten hier die Positionen 31-41
    }
}
route("display", $displayRoutes);

//////////////////////////////////////////////////////////////
// SYSTEM ROUTING
//////////////////////////////////////////////////////////////
$systemRoutes = [
    4 => './mobile/index.php',
    3 => './config_backup.php',
	2 => "./caller_extern.php?id=" . $cubeip,
	5 => "./caller_extern.php?id=" . DL3EL_SVXCUBE,
	6 => "/DVSwitch/index.php",
	7 => "./caller.php?id=/",
	8 => "./ssh.php",
    1 => function () {
        file_put_contents("dl3el/menu", "classic");
        header("Location: ./index.php");
    }
	
];
if ((defined ('debug')) && (debug > 0)) echo "SVXCube: " . "./caller_extern.php?id=" . DL3EL_SVXCUBE;
route("svxsystem", $systemRoutes);

//////////////////////////////////////////////////////////////
// EXPERT EDIT ROUTING
//////////////////////////////////////////////////////////////
$expertRoutes = [
    1 => './caller.php?file=' . SVXCONFPATH . 'svxlink.conf',
    2 => './caller.php?file=' . MODULEPATH . 'ModuleEchoLink.conf',
    3 => './caller.php?file=' . MODULEPATH . 'ModuleMetarInfo.conf',
    4 => './caller.php?file=' . DL3EL . '/aprs-is-msg.conf',
    5 => './caller.php?id=amixer/index',
    6 => './rf.php',
    7 => './caller.php?file=' . SVXCONFPATH . 'node_info.json',
    8 => './caller.php?file=' . DL3EL_BASE . 'include/config.php',
    9 => './wifi.php',
	10 => './caller.php?file=TclVoiceMail.conf" id="tclvoicemail"'
];

if (
    in_array(DL3EL_RADIO, ["Shari", "RFGuru"]) ||
    str_starts_with(DL3EL_RADIO, "SC ")
) {
    $expertRoutes[5] = './caller.php?id=amixer/index';
}
route("expertedit", $expertRoutes);

//////////////////////////////////////////////////////////////
// NORMAL EDIT ROUTING
//////////////////////////////////////////////////////////////
$normalRoutes = [
    1 => './editor.php?id=svxlink',
    2 => './editor.php?id=echolink',
    3 => './editor.php?id=metarinfo',
    4 => './caller.php?id=amixer/index',
    5 => './rf.php',
    6 => './editor.php?id=nodeInfo'
];

if (
    in_array(DL3EL_RADIO, ["Shari", "RFGuru"]) ||
    str_starts_with(DL3EL_RADIO, "SC ")
) {
    $expertRoutes[4] = './caller.php?id=amixer/index';
}
route("normaledit", $normalRoutes);

//////////////////////////////////////////////////////////////
// LOG ROUTING
//////////////////////////////////////////////////////////////
$logRoutes = [
    1 => './editor.php?id=log',
    2 => './caller.php?file=log',
    3 => './caller.php?file=loga',
    4 => './caller.php?file=log" id="log"',	
    5 => './caller.php?file=ref" id="log'	
];
route("logdat", $logRoutes);

//////////////////////////////////////////////////////////////
// REFLECTOR CONFIG
//////////////////////////////////////////////////////////////
if (isset($_POST['refconf'])) {
    $val = (int) $_POST['refconf'];
    if ($val >= 1 && $val <= 4) {
        $file = DL3EL . "/Reflector{$val}.conf";
        header("Location: ./caller.php?file=" . urlencode($file));
        exit;
    }
}

//////////////////////////////////////////////////////////////
// MENÜ AUSGABE
//////////////////////////////////////////////////////////////
echo "<br>";

//////////////////////////////////////////////////////////////
// HILFE BUTTON
//////////////////////////////////////////////////////////////
list($version, $rest) = explode(" ", $dbversion);
$dbversionFile = DL3EL . "/dbversion.upd";
if (file_exists($dbversionFile)) {
    if (file_get_contents($dbversionFile) == "update") {
        $version .= " Update available";
    }
}
echo '<a href="./caller_extern.php?id=http://relais.dl3el.de/FM-Funknetz/hilfe.html&wid=950"
style="text-align:left;margin-right:20px;border-radius:8px;color:white;
border-color:transparent;background-color:blue;font-size:20px;"
id="info">Hilfe für ' . $version . '</a>';

//////////////////////////////////////////////////////////////
// DASHBOARD
//////////////////////////////////////////////////////////////
echo '<a href="./index.php"
style="margin-right:10px;font-size:18px;color:#CD6600"
target="_top">Dashboard</a>';
echo '<form method="post" action="" style="display:inline;">';

//////////////////////////////////////////////////////////////
// DISPLAY MENU
//////////////////////////////////////////////////////////////
echo '<select style="font-size:16px;" name="display" onchange="this.form.submit()">';
echo '<option value="" hidden disabled selected>DISPLAY</option>';
echo '<option value="2">Talk Groups</option>';
if ($monfmnet == "true")  echo '<option value="3">Monitor Calls</option>';
if ($monuknet == "true") echo '<option value="16">UK Monitor</option>';
echo '<option value="4">Echolink</option>';
echo '<option value="5">FM Relais</option>';
if ($cluster) echo '<option value="6">DX Cluster</option>';
echo '<option value="7">APRS Monitor</option>';
if ($owrx)    echo '<option value="8">OWR</option>';
echo '<option value="9">Voice FMN</option>';
echo '<option value="10">QSO Cockpit</option>';
if ($clock)   echo '<option value="11">OHC</option>';
echo '<option value="12">Web Chat</option>';
if ($livedb01)  echo '<option value="13">Live DB</option>';
if ($livedb02)  echo '<option value="14">Live DB</option>';
if ($livedb03)  echo '<option value="15">Live DB</option>';

// derzeit max 10 externe Links definiert in DISPLAY ROUTING
for ($i = 1; $i < $max_links + 1; $i++) {
    $link = "DL3EL_EXTERN" . $i;
    $name = "DL3EL_EXTERN" . $i . "_NAME";
    $tab = "DL3EL_EXTERN" . $i . "_TAB";
    if (defined($link) && (!defined($tab) || constant($tab) != "new")) {
        $text = defined($name) ? constant($name) : "Ext " . $i;
        echo "<option value='" . (30 + $i) . "'>$text</option>";
    }
}
echo '</select>';

//////////////////////////////////////////////////////////////
// EXT LINKS MENU
//////////////////////////////////////////////////////////////
$ext_link_menu = false;

// derzeit max 10 externe Links definiert in DISPLAY ROUTING
for ($i = 1; $i < $max_links + 1; $i++) {
    $tab = "DL3EL_EXTERN" . $i . "_TAB";
    if (defined($tab) && constant($tab) == "new") {
        $ext_link_menu = true;
    }
}

if ($ext_link_menu) {
    echo '<select style="font-size:16px;" onchange="sopen(this)">';
    echo '<option value="" hidden disabled selected>EXT LINKS</option>';

// derzeit max 10 externe Links definiert in DISPLAY ROUTING
    for ($i = 1; $i < $max_links + 1; $i++) {
        $link = "DL3EL_EXTERN" . $i;
        $name = "DL3EL_EXTERN" . $i . "_NAME";
        $tab = "DL3EL_EXTERN" . $i . "_TAB";
        if (defined($tab) && constant($tab) == "new" && defined($link)) {
            $url = constant($link);
            $text = defined($name) ? constant($name) : "Ext " . $i;
            echo "<option value=\"$url\">$text</option>";
        }
    }
    echo '</select>';
    echo '<script>
function sopen(select){
if(select.value!=""){
window.open(select.value,"_blank");
select.selectedIndex=0;
}
}
</script>';

}

//////////////////////////////////////////////////////////////
// SYSTEM MENU
//////////////////////////////////////////////////////////////
echo '<select style="font-size:16px;" name="svxsystem" onchange="this.form.submit()">';
echo '<option value="" hidden disabled selected>SYSTEM</option>';
echo '<option value="1">Classic Menü</option>';
echo '<option value="3">Backup & Restore</option>';
if ($cube01 == true) echo '<option value="2">SVX Cube</option>';
if ($cube02 == true) echo '<option value="5">SVX Cube</option>';
if ($mobil == true) echo '<option value="4">Mobile DB</option>';
if ($dvs == true) echo '<option value="6">DVS</option>';
if ($local_ref == true) echo '<option value="7">Reflector</option>';
if ($shell_box == true) echo '<option value="8">SSH</option>';
echo '</select>';

//////////////////////////////////////////////////////////////
// EXPERT EDIT MENU
//////////////////////////////////////////////////////////////
if ($expert) {
    echo '<select style="font-size:16px;" name="expertedit" onchange="this.form.submit()">';
    echo '<option value="" hidden disabled selected>EXPERT EDIT</option>';
    echo '<option value="1">SVXLink Conf</option>';
    echo '<option value="2">Echolink Conf</option>';
    echo '<option value="3">Metar Info Conf</option>';
    echo '<option value="4">APRS Conf</option>';
    echo '<option value="5">AMixer</option>';
    if ($shari) echo '<option value="6">Shari Conf</option>';
    if ($guru)  echo '<option value="6">Guru Conf</option>';
    echo '<option value="7">Node Info</option>';
    echo '<option value="8">config.php</option>';
    echo '<option value="9">Wifi</option>';
	if ($voice_mail) echo '<option value="10">TclVoiceMail</option>';
    echo '</select>';
}

//////////////////////////////////////////////////////////////
// NORMAL EDIT MENU
//////////////////////////////////////////////////////////////
if ($normal) {
    echo '<select style="font-size:16px;" name="normaledit" onchange="this.form.submit()">';
    echo '<option value="" hidden disabled selected>NORMAL EDIT</option>';
    echo '<option value="1">SVXLink Conf</option>';
    echo '<option value="2">Echolink Conf</option>';
    echo '<option value="3">Metar Info Conf</option>';
    echo '<option value="4">AMixer</option>';
    if ($shari) echo '<option value="5">Shari Conf</option>';
    if ($guru)  echo '<option value="5">Guru Conf</option>';
    echo '<option value="6">Node Info</option>';
    echo '</select>';
}

//////////////////////////////////////////////////////////////
// REFLECTOR MENU
//////////////////////////////////////////////////////////////
if ($reflector) {
    echo '<select style="font-size:16px;" name="refconf" onchange="this.form.submit()">';
    echo '<option value="" hidden disabled selected>REF CONF</option>';
    if (defined("DL3EL_REF1_BUTTON"))
        echo "<option value='1'>" . DL3EL_REF1_BUTTON . "</option>";
    if (defined("DL3EL_REF2_BUTTON"))
        echo "<option value='2'>" . DL3EL_REF2_BUTTON . "</option>";
    if (defined("DL3EL_REF3_BUTTON"))
        echo "<option value='3'>" . DL3EL_REF3_BUTTON . "</option>";
    if (defined("DL3EL_REF4_BUTTON"))
        echo "<option value='4'>" . DL3EL_REF4_BUTTON . "</option>";
    echo '</select>';
}

//////////////////////////////////////////////////////////////
// LOG FILES MENU
//////////////////////////////////////////////////////////////
echo '<select style="font-size:16px;" name="logdat" onchange="this.form.submit()">';
echo '<option value="" hidden disabled selected>LOG FILES</option>';
echo '<option value="1">SVXLink Live Log</option>';
echo '<option value="2">SVXLink Log</option>';
echo '<option value="3">APRS Log</option>';
if ($l_log) echo '<option value="4">L-Log</option>';
if ($r_log) echo '<option value="5">R-Log</option>';
echo '</select>';

//////////////////////////////////////////////////////////////
// EXPERT / NORMAL BUTTONS
//////////////////////////////////////////////////////////////
if (defined('DL3EL_EXPERT') && DL3EL_EXPERT == "yes") {
    echo '<button style="color:' . $kn_exp_col . ';font-size:16px;" name="btn_expert">' . $kn_exp . '</button>';
    echo '<button style="color:' . $kn_nor_col . ';font-size:16px;" name="btn_normal">' . $kn_nor . '</button>';
}

//////////////////////////////////////////////////////////////
// AUTHORISE + POWER BUTTONS
//////////////////////////////////////////////////////////////
echo '<a href="./authorise.php" style="color:crimson;margin-left:10px;font-size:18px;">Authorise</a>';
echo '<a href="./caller.php?id=power/index" style="color:green;margin-left:10px;font-size:18px;">Power</a>';
echo '</form>';
echo "<br>";
echo '<div id="display-links" align="center"></div>';

$tgdb_File = DL3EL_BASE . "include/tgdb.php";
if (filesize($tgdb_File)  <= 1000) {
    echo '<a style="color:red;" class="hreflink" href="./include/tgdb_upd.php" id="upd">Update TGDB</a> ';
}

}

if($show_top_menu != 1) {
echo '<span style="color:#CD6600;font-size:16px;font-weight:bold;">Display:&nbsp;&nbsp;</span>';
echo '<a style="color:#000000;font-size:16px;font-weight:bold;" class="hreflink" href="./index.php" target="_top">Dashboard</a> | ';
echo '<a style="color:#000000;font-size:16px;font-weight:bold;"class="hreflink" href="./caller.php?id=include/tg" ">Talk Groups</a> | ';
echo '<a style="color:#000000;font-size:16px;font-weight:bold;"class="hreflink" href="./caller.php?id=include/svxdxc&refresh=15" >DX</a> | ';
}



?>
