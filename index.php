<?php
if (session_status() === PHP_SESSION_NONE) {
    session_start();
}

    $base_dir_def = '<?php define("DL3EL_BASE", "' . __DIR__ . '/");?>' . "\n";;
    $base_dir_dl3el_def = '<?php define("DL3EL", "' . __DIR__ . '/dl3el");?>' . "\n";;
    $base_dir_file = __DIR__ . "/include/basedir.php";
    if (!$handle = fopen($base_dir_file, 'w')) {
//        die("Cannot open file ($base_dir_file) for writing. Check file permissions.");
//        echo("Cannot open file ($base_dir_file) for writing. Check file permissions. ");
//        echo("find " . __DIR__ . " ! -exec sudo chown svxlink:svxlink {} +");
        exec("find " . __DIR__ . " ! -exec sudo chown svxlink:svxlink {} +");
        if (!$handle = fopen($base_dir_file, 'w')) {
            die("Cannot open file ($base_dir_file) for writing. Check file permissions.");
        }
    }
    fwrite($handle, $base_dir_def);
    fwrite($handle, $base_dir_dl3el_def);
    fclose($handle);

if ((!file_exists('include/config.inc.php')) && (file_exists('include/config.inc.php.example')) ) {
    copy('include/config.inc.php.example', 'include/config.inc.php');
    sleep(3); 
}

if ((!file_exists('include/config.php')) && (file_exists('include/config.php.example')) ) {
    copy('include/config.php.example', 'include/config.php');
    sleep(3); 
}

if ((!file_exists('include/buttons.php')) && (file_exists('include/buttons.php.example')) ) {
    copy('include/buttons.php.example', 'include/buttons.php');
    sleep(3); 
}

include_once "include/settings.php";
include_once "include/functions.php";

if ((defined('DL3EL_NOAUTH')) && (DL3EL_NOAUTH === "yes")) {
// always stay logged on
    $_SESSION['auth'] = "AUTHORISED";
} else {
   if (empty($_SESSION['auth'])) {
      $_SESSION['auth'] = "UNAUTHORISED";
   }
}    

if ((defined('DL3EL_SC_CHANGE')) && (DL3EL_SC_CHANGE === "yes")) {
    $svxConfigFile = SVXCONFPATH.SVXCONFIG;
    if (fopen($svxConfigFile,'r')) {
        $svxconfig = parse_ini_file($svxConfigFile,true,INI_SCANNER_RAW); 
        if (defined('DL3EL_SC_STRING')) {
            $sc_port_cmp = DL3EL_SC_STRING;
        } else {    
            $sc_port_cmp = "Audio Device";
        }
        $sc_port_linux = 'aplay -l | grep "' . $sc_port_cmp . '"';
        $sc = 'aplay -l | grep "Audio Device"';
        $sc = substr(shell_exec($sc),5,1);
         $sc_port_name = $svxconfig['SimplexLogic']['RX']; 
         $sc_port = substr($svxconfig[$sc_port_name]['AUDIO_DEV'],12,5); 
         if ($sc != $sc_port) {
            echo "<b>Soundcard&nbsp;mismatch:<br>Card:" . $sc . "/" . substr($svxconfig[$sc_port_name]['AUDIO_DEV'],5,8) . "</b>, will be changed";
            $sc_port_raw = $svxconfig[$sc_port_name]['AUDIO_DEV']; 
            echo "Data old: " . $sc_port_raw;
            $sc_port_new = substr($svxconfig[$sc_port_name]['AUDIO_DEV'],0,12) . $sc; 
            echo ", will be changed to Data new: " . $sc_port_new . "<br>";
            $content = file_get_contents($svxConfigFile);
            $backup_filename = $svxConfigFile . "." . date("YmdHis");
            exec('sudo cp -p ' . $svxConfigFile . ' ' . $backup_filename);
            $content = str_replace($sc_port_raw,$sc_port_new,$content); 
            file_put_contents($svxConfigFile, $content);
            echo "done, now restarting svxlink..<br>";
            sleep(1);
            exec('sudo systemctl restart svxlink 2>&1', $screen, $retval);
            if ($retval === 0) {
                echo "SVXLink sucessfull restartet, please reload page";
            } else {
                echo "SVXLink restart failure, check log";
            }
         }   
    }
}

include_once "include/page_top.php";

if ((file_exists('/usr/bin/dvs')) && (defined('DL3EL'))) {
    $dmr_support = "1";
    include_once "dvs_code.php";
} else {
    $dmr_support = "0";
    $dmrtg = "no DMR";
}    
// for debug, switch off dmr_support
//    $dmr_support = "0";
if ((defined ('debug')) && (debug > 0)) echo "DVS Support: $dmr_support";
$refchg = DL3EL.'Reflector1.conf';
if ((defined ('debug')) && (debug > 0)) echo "Auth:" . $_SESSION['auth'] . "<br>";
if ($_SESSION['auth'] === "AUTHORISED") {
    if (file_exists(DL3EL.'/Reflector1.conf')) {
        include_once "ref_change.php";
    }
}   

?>

<?php
    echo '<table style = "margin-right:20px;border:none;border-collapse:collapse;background-color:#f1f1f1;">';
	echo '<tr style = "border:none;background-color:#f1f1f1;">';
    echo '<td width="200px" valign="top" class="hide" style = "height:auto;border:none;background-color:#f1f1f1;">';
    echo '<div class="nav" style = "margin-bottom:1px;margin-top:10px;">'."\n";
    echo '<script type="text/javascript">'."\n";
    echo 'function reloadStatusInfo(){'."\n";
    echo '$("#statusInfo").load("include/status.php",function(){ setTimeout(reloadStatusInfo,3000) });'."\n";
    echo '}'."\n";
    echo 'setTimeout(reloadStatusInfo,3000);'."\n";
    echo '$(window).trigger(\'resize\');'."\n";
    echo '</script>'."\n";
    echo '<div id="statusInfo" style = "margin-bottom:30px;">'."\n";
    include_once "include/status.php";
    echo '</div>'."\n";
    echo '</div>'."\n";
    echo '</td>'."\n";

    echo '<td valign="top" style = "height:auto;border:none;  background-color:#f1f1f1;">';
    echo '<div class="content">'."\n";
    echo '<script type="text/javascript">'."\n";

    echo 'function reloadLastHeard(){'."\n";
    echo '  $("#LastHeard").load("include/lh.php",function(){ setTimeout(reloadLastHeard,3000) });'."\n";
    echo '}'."\n";
    echo 'setTimeout(reloadLastHeard,3000);'."\n";

    if ($dmr_support == "1") {
        echo 'function reloadLastHeardDMR(){'."\n";
        echo '  $("#LastHeardDMR").load("/DVSwitch/include/lh.php",function(){ setTimeout(reloadLastHeardDMR,3000) });'."\n";
        echo '}'."\n";
        echo 'setTimeout(reloadLastHeardDMR,3000);'."\n";
    }
// --- NEU: Reload für die aktive TG Teilnehmer ---
    $mqtt_script = shell_exec("pgrep fmn-mqtt.pl");
    if ((strlen($mqtt_script)) || (file_exists("/usr/local/bin/mqtt-simple"))) {
      $mqtt = 1;
      echo 'function reloadActiveTGCalls(){'."\n";
      echo '  $("#ActiveTGCalls").load("active_tg_calls.php", function(){ setTimeout(reloadActiveTGCalls, 30000) });'."\n";
      echo '}'."\n";
      echo 'setTimeout(reloadActiveTGCalls, 3000);'."\n";
    } else {
      $mqtt = 0;
    } 
// ------------------------------------------------
    echo '$(window).trigger(\'resize\');'."\n";

    echo '</script>'."\n";
    echo '<center><div id="LastHeard" style = "margin-bottom:30px;">'."\n";
    include_once "include/lh.php";
    echo '</div></center>'."\n";

    if ($dmr_support == "1") {
        echo '<center><div id="LastHeardDMR" style = "margin-bottom:30px;">'."\n";
        echo '</div></center>'."\n";
    }    
// --- NEU: Container für die Anzeige ---
      if ($mqtt) {
        echo '<center><div id="ActiveTGCalls" style = "margin-bottom:30px;">'."\n";
        include_once "active_tg_calls.php"; // Initiales Laden beim Seitenaufruf
        echo '</div></center>'."\n";
      }  
// --------------------------------------    echo "<br />\n";
    echo '</td>';
    // Live DB
    include_once "caller_svxmqtt.php"; 
    echo '</tr></table>';
?>

<?php
include_once "include/page_bottom.php";
    ?>

