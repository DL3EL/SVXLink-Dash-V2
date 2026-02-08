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
    include_once "include/status.php";
    echo '</div>'."\n";
    echo '</div>'."\n";
    echo '</td>'."\n";
    echo '<td valign="top"  style = "height:360px; width=620px; text-align: center; border:none;  background-color:#f1f1f1;">';
    echo '<div class="content">'."\n";
//    echo '<div class="left-half">';
    echo '<object style="outline:none; height: 800px;width:1500px;justify-content: left;" data=http://openhamclock.com/></object>';
    echo '</div>';
//    echo '</p>';
    // Live DB
    echo '<td valign="top" style = "border:none;">';
		echo '<object style="outline:none; width:500px; height:1412px; justify-content: left;" data=svx2mqtt/index_duo.php></object>';
    echo '</td></tr></table>';

echo ' <center>';
 include_once "include/page_bottom.php"; ?>
?>
