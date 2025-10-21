<?php
if (session_status() === PHP_SESSION_NONE) {
    session_start();
}
$progname = basename($_SERVER['SCRIPT_FILENAME'],".php");
include_once "include/settings.php";
include_once "include/page_top.php";
?>
<?php
    echo '<table style = "margin-bottom:0px;border:0; border-collapse:collapse; cellspacing:0; cellpadding:0; background-color:#f1f1f1;"><tr style = "border:none;background-color:#f1f1f1;">';
//    echo '<td width="200px" valign="top" class="hide" style = "height:auto;border:0;background-color:#f1f1f1;">';
    echo '<div class="nav" style = "margin-bottom:1px;margin-top:1px;">'."\n";

    echo '</div>'."\n";
    echo '</td>'."\n";

$file = $_GET['file']; 
    echo '<iframe src="./edit/index.php?file=' . $file . '"  style = "width:900px; height:690px"></iframe>';
    echo "<tr> <td>&nbsp;</td></tr>";
?>
</table>
<?php include_once "include/page_bottom.php"; ?>
