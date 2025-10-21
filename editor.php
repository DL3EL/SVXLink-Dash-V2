<?php
if (session_status() === PHP_SESSION_NONE) {
    session_start();
}
$progname = basename($_SERVER['SCRIPT_FILENAME'],".php");
include "include/settings.php";
//include_once "include/config.inc.php";
include_once "include/functions.php";
include_once "include/page_top.php";
?>
<?php
if($_GET['id']=="log") {
    $edit_file = "log/index.php";
}
if($_GET['id']=="svxlink") {
    $edit_file = "svxlink/index.php"; 
}
if($_GET['id']=="amixer") {
    $edit_file = "amixer/index.php"; 
}
elseif($_GET['id']=="echolink"){
    $edit_file = "echolink/index.php";
}
elseif($_GET['id']=="metarinfo"){
    $edit_file = "metarinfo/index.php";
}
elseif($_GET['id']=="nodeInfo"){
    file_backup("/etc/svxlink/","node_info.json");
    $edit_file="nodeInfo/index.php";
}
if($_GET['id']=="logic") {
    $edit_file = "logic/index.php"; 
}
if($_GET['id']=="power") {
    $edit_file = "power/index.php";
}
if($_GET['id']=="msg") {
    $edit_file = "aprs/index.php";
}
// check if form has been submitted
//$filename = by choice;





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
    include_once "include/status.php";
    echo '</div>'."\n";
    echo '</div>'."\n";
    echo '</td>'."\n";
    echo '<td valign="top"  style = "height:595px; width=620px; text-align: center; border:none;  background-color:#f1f1f1;">';	


if($_SESSION['auth'] === 'AUTHORISED') {
echo '<iframe height="100%" id="editIframe" src="' . $edit_file . '" width="620px" height="495px" title="EDIT"></iframe>';

    echo '</td>';
   }   else {
        echo '<h1 id="power" style = "color:#00aee8;font: 18pt arial, sans-serif;font-weight:bold; text-shadow: 0.25px 0.25px gray;">You are not yet authorised.</h1>';
        echo '</td>';    
    }
?>
</tr></table>
<?php include_once "include/page_bottom.php"; ?>
