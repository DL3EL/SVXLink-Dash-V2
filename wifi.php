<?php
if (session_status() === PHP_SESSION_NONE) {
    session_start();
}
include_once "include/settings.php";
//include_once "include/editor.php";
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
    include_once "include/status.php";
    echo '</div>'."\n";
    echo '</div>'."\n";
    echo '</td>'."\n";

    echo '<td valign="center" style = "height:455px;width:550px; border:none; background-color:#f1f1f1;">';
    echo '<iframe src="./wifi"  width="615px" height="450px" style = "background-color:#f1f1f1;"></iframe>';
    echo '</td>';
 
?>
</tr></table>
<?php include_once "include/page_bottom.php"; ?>
