<?php
if (session_status() === PHP_SESSION_NONE) {
    session_start();
}

include_once "include/settings.php";
include_once "include/tgdb.php";    
include_once "include/page_top.php";

?>
<?php
    echo '<table style = "margin-bottom:0px;border:0; border-collapse:collapse; cellspacing:0; cellpadding:0; background-color:#f1f1f1;"><tr style = "border:none;background-color:#f1f1f1;">';
//    $tgselect = trim(getSVXTGSelect());
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
    }    
// wichtig, SVXLink muss für das Verzeichnis berechtigt  sein. Am besten auch noch chmod 755 setzen.
    $cmd = "perl " . DL3EL . "/get-monitor.pl v=0 r=1 \"TGNames:" . $tgnames . "\" " . $tgmons;
//    $cmd = "perl " . DL3EL . "/get-monitor.pl v=0 r=1 ". $tgmons . "\"TGNames:" . $tgnames . "\"";
    if ((defined ('debug')) && (debug)) echo "Aufruf: " . $cmd;
    echo "",exec($cmd, $output, $retval);
    echo "<tr> <td>&nbsp;</td></tr>";

?>
</table>
<?php include_once "include/page_bottom.php"; ?>
