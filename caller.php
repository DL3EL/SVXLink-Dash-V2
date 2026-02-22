<?php
if (session_status() === PHP_SESSION_NONE) {
    session_start();
}
include_once "include/settings.php";
include_once "include/page_top.php";

///// aus index.php
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
////
/*
    echo '<table style = "margin-bottom:0px;border:0; border-collapse:collapse; cellspacing:0; cellpadding:0; background-color:#f1f1f1;">';
    echo '<tr style = "border:none;background-color:#f1f1f1;">';
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
*/    

    echo '<div class="content">'."\n";
    echo '<td valign="top" style = "margin-right=10px;height:auto;border:none;  background-color:#f1f1f1;">';
    echo '<div id="content" style = "margin-bottom:00px;">'."\n";
//echo '<fieldset style = " width:550px;background-color:#f1f1f1;margin-top:15px;margin-left:10px;margin-right:20px;font-size:14px; border:none;">';

    $call_script = $_GET['id'] . ".php";
    if ($call_script === "amixer/index.php") {
        echo '<object style="outline:none; width:600px; height:900px; justify-content: left;" data=' . $call_script . '></object>';
    } else {    
        include $call_script;
    }
//
    if (isset($_GET['refresh']) && !empty($_GET['refresh'])) {
        $rate = $_GET['refresh'] * 1000;
        echo '<script type="text/javascript">'."\n";
        echo 'function reloadCurrPage(){'."\n";
        echo '  $("#content").load("' . $call_script . '",function(){ setTimeout(reloadCurrPage,' . $rate . ') });'."\n";
        echo '}'."\n";
        echo 'setTimeout(reloadCurrPage,' . $rate . ');'."\n";
        echo '$(window).trigger(\'resize\');'."\n";
        echo '</script>'."\n";
    }    
//	echo '<object style="outline:none; width:600px; height:900px; justify-content: left;" data=' . $call_script . '></object>';
//
//echo '</fieldset>';
    echo '</div></center>'."\n";
    echo '</td>';
    include_once "caller_svxmqtt.php"; 
    echo '</tr></table>';
include_once "include/page_bottom.php"; 
?>
