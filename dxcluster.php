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
    echo '<script type="text/javascript">'."\n";

    echo 'function reloadSVXDXC(){'."\n";
    echo '  $("#svxdxc").load("include/svxdxc.php",function(){ setTimeout(reloadSVXDXC,90000) });'."\n";
    echo '}'."\n";
    echo 'setTimeout(reloadSVXDXC,15000);'."\n";

    echo '$(window).trigger(\'resize\');'."\n";
    echo '</script>'."\n";
    echo '<center><div id="LastHeard" style = "margin-bottom:30px;">'."\n";
    echo '</div></center>'."\n";
    echo "<br />\n";
        echo '<center><div id="svxdxc" style = "margin-bottom:30px;">'."\n";
        include_once "include/svxdxc.php";
        echo '</div></center>'."\n";
    echo '</td></tr></table>';
?>
<?php include_once "include/page_bottom.php"; ?>
