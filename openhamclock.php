<?php
if (session_status() === PHP_SESSION_NONE) {
    session_start();
}
include_once "include/settings.php";
include_once "include/page_top.php";
?>

<?php
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
    include "include/status.php";
    echo '</div>'."\n";
    echo '</div>'."\n";
    echo '</td>'."\n";
    echo '<td valign="top"  style = "height:500px;text-align: left; border:none;  background-color:#f1f1f1;">';
    echo '<div class="content">'."\n";
//    echo '<p align="left">';
//    echo '<iframe src="http://192.168.241.99:3030/"  style = "width:1615px; height:690px"></iframe>';
    echo '<iframe src="http://openhamclock.com/"  style="display: block; margin-left: 0; margin-right: 20%;width:60%; height:800px"></iframe>';
//    echo '<iframe src="https://www.beiderwieden.de/imode/index.html"  style = "width:100%; height:690px"></iframe>';
//    echo '</p>';
    echo '</td></tr></table>';
?>
<?php 
// include_once "include/page_bottom.php"; ?>
