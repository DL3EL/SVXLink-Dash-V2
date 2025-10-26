<?php
if (session_status() === PHP_SESSION_NONE) {
    session_start();
}
include_once "include/settings.php";
include_once "include/page_top.php";
?>

<?php
    echo '<table style = "margin-bottom:0px;border:0; border-collapse:collapse; cellspacing:0; cellpadding:0; background-color:#f1f1f1;"><tr style = "border:none;background-color:#f1f1f1;">';
    echo '<td width="200px" valign="top" class="hide" style = "height:auto;border:0;background-color:#f1f1f1;">';
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
    echo '<td valign="top"  style = "height:500px; width=620px; text-align: center; border:none;  background-color:#f1f1f1;">';
    echo '<div class="content">'."\n";
    echo '<iframe src="svx2mqtt/index.php"  style = "width:615px; height:690px"></iframe>';
    echo '</td></tr></table>';
?>
<?php include_once "include/page_bottom.php"; ?>
