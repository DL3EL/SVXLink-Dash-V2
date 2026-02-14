<?php
if (session_status() === PHP_SESSION_NONE) {
    session_start();
}
include_once "include/settings.php";
include_once "include/page_top.php";

    echo '<table style = "margin-bottom:0px;border:0; border-collapse:collapse; cellspacing:0; cellpadding:0; background-color:#f1f1f1;"><tr style = "border:none;background-color:#f1f1f1;">';
    echo '<td width="100px" valign="top" class="hide" style = "height:auto;border:0;background-color:#f1f1f1;">';
    echo '<div class="nav" style = "margin-bottom:1px;margin-top:1px;">'."\n";

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
    echo '<td valign="top"  style = "height:360px; width=620px; text-align: center; border:none;  background-color:#f1f1f1;">';
    echo '<div class="content">'."\n";
//    echo '<div class="left-half">';
    $call_script = $_GET['id'];
    if (isset($_GET['wid']) && !empty($_GET['wid'])) {
        // Parameter vorhanden und nicht leer
        $extern_width = $_GET['wid'];
    } else {
        if (defined('DL3EL_EXTERNWEB_WIDTH')) {
            $extern_width = DL3EL_EXTERNWEB_WIDTH;
        } else {
            $extern_width = 1500;
        }    
    }        
    echo '<object style="outline:none; height: 850px;width:' . $extern_width .'px;justify-content: left;" data=' . $call_script . '></object>';
//  echo '<object style="outline:none; height: 850px;width:1500px;justify-content: left;" data=http://openhamclock.com/></object>';
//	echo '<object style="outline:none; width:600px; height:900px; justify-content: left;" data=' . $call_script . '></object>';
    echo '</div>';
    // Live DB
    include_once "caller_svxmqtt.php"; 
    echo '</tr></table>';

echo ' <center>';
 include_once "include/page_bottom.php"; ?>

