<?php
include_once "include/settings.php";
include_once "include/tgdb.php";    
    $svxConfigFile = SVXCONFPATH . "/" . SVXCONFIG;
    if (fopen($svxConfigFile,'r')) {
      $svxconfig = parse_ini_file($svxConfigFile,true,INI_SCANNER_RAW); 
    }
    echo '<table style = "margin-bottom:0px;border:0; border-collapse:collapse; cellspacing:0; cellpadding:0; background-color:#f1f1f1;"><tr style = "border:none;background-color:#f1f1f1;">';
    if ($_SESSION['auth'] !== 'AUTHORISED') {
        echo '<button id="n/a" style = "margin-right:90px; width:150px; color:white; background-color:red; border-color:transparent; border-radius:8px; height:40px; font-size:16px;"><b>nicht angemeldet</b></button>';
        return;
    }
    $tgmon = explode(",",$svxconfig['ReflectorLogic']['MONITOR_TGS']);
    $tgmons = "";
    $tgnames = "";
    foreach ($tgmon as $key) {
        $key = str_replace(" ","",$key);
        $tgmons = $tgmons . $key . " ";
        $tg = trim($key,"+");
        if (isset($tgdb_array[$tg])) {
            $tgn = str_replace("`","'",$tgdb_array[$tg]);
            $tgnames = $tgnames . $tg . "^" . $tgn . ";";
        } else {
            $tgnames = $tgnames . $tg . "^??;";
        }
    }
    if (defined('DL3EL')) {
        $svxStatusFile = DL3EL . "/tg_status";
        $svxdata = shell_exec('cat ' . $svxStatusFile);
        $tgmons = $tgmons . $svxdata . " ";
        $tg = trim($svxdata);
        if (isset($tgdb_array[$tg])) {
            $tgn = str_replace("`","'",$tgdb_array[$tg]);
            $tgnames = $tgnames . $tg . "^" . $tgn . ";";
        } else {
            $tgnames = $tgnames . $tg . "^??;";
        }
    }    
// wichtig, SVXLink muss für das Verzeichnis berechtigt  sein. Am besten auch noch chmod 755 setzen.
    $cmd = "perl " . DL3EL . "/get-monitor.pl v=0 r=1 \"TGNames:" . $tgnames . "\" " . $tgmons;
//    $cmd = "perl " . DL3EL . "/get-monitor.pl v=0 r=1 ". $tgmons . "\"TGNames:" . $tgnames . "\"";
    if ((defined ('debug')) && (debug)) echo "Aufruf: " . $cmd;
    echo "",exec($cmd, $output, $retval);
    echo "<tr> <td>&nbsp;</td></tr>";

?>
</table>
