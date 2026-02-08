<?php
if (session_status() === PHP_SESSION_NONE) {
    session_start();
}
include "include/settings.php";
include_once "include/page_top.php";

    $call_script = $_GET['id'] . ".php";
/*    
    if($_GET['id']=="monitor") {
        $call_script = "monitor.php";
    }
    if($_GET['id']=="tg") {
        $call_script = "include/tg.php";
    }
    if($_GET['id']=="echolink_dl3el") {
        $call_script = "echolink_dl3el.php";
    }
    if($_GET['id']=="relais") {
        $call_script = "relais.php";
    }
    if($_GET['id']=="dxcluster") {
        $call_script = "dxcluster.php";
    }
    if($_GET['id']=="openhamclock") {
        $call_script = "openhamclock.php";
    }
    if($_GET['id']=="aprs") {
        $call_script = "aprs.php";
    }
*/
    echo '<table style = "margin-bottom:0px;border:0; border-collapse:collapse; cellspacing:0; cellpadding:0; background-color:#f1f1f1;"><tr style = "border:none;background-color:#f1f1f1;">';
    echo '<td width="200px" valign="top" class="hide" style = "height:auto;border:0;background-color:#f1f1f1;">';
    echo '<div class="nav" style = "margin-bottom:1px;margin-top:10px;">'."\n";

    echo '<script type="text/javascript">'."\n";
    echo 'function reloadStatusInfo(){'."\n";
    echo '  $("#statusInfo").load("include/status.php",function(){ setTimeout(reloadStatusInfo,3000) });'."\n";
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

    echo '<center><div id="Relais" style = "margin-bottom:30px;">'."\n";
//    include "relais.php";
    include $call_script;
    echo '</div></center>'."\n";
    echo '</td>';
    include_once "caller_svxmqtt.php"; 
/*
    // Live DB
    echo '<td valign="top" style = "border:none;">';
//		echo '<object style="outline:none; width:500px; height:1412px; justify-content: left;" data=svx2mqtt/index_duo.php></object>';
		echo '<object style="outline:none; width:600px; height:900px; justify-content: left;" data=svx2mqtt/index_duo.php></object>';
    echo '</td>';
*/
    echo '</tr></table>';
include_once "include/page_bottom.php"; 
?>
