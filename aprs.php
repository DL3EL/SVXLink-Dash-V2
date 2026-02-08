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
    echo '<div class="content">'."\n";
    echo '<script type="text/javascript">'."\n";

    echo 'function reloadSVXDXC(){'."\n";
    echo '  $("#svxdxc").load("include/svxaprs.php",function(){ setTimeout(reloadSVXDXC,5000) });'."\n";
    echo '}'."\n";
    echo 'setTimeout(reloadSVXDXC,5000);'."\n";

    echo '$(window).trigger(\'resize\');'."\n";
    echo '</script>'."\n";
    echo '<div id="svxaprs" style = "margin-bottom:30px;align:left">'."\n";
    include_once "include/svxaprs.php";

    echo '<form method="post">';
      echo ' Nachricht an <input type="text" id="aprs" name="aprs_call" value="DL3EL" required>';
      echo '&nbsp;&nbsp;';
      echo '<input type="text" style = "width:300px" id="aprs" name="aprs_msg" value="Text" required>';
      echo '&nbsp;&nbsp;';
      echo '<input type="submit" name="senden" value="senden">';  
    echo '</form>';
    // Save / Reload on submit//
    if ((isset($_POST['senden'])) || (isset($_POST['cancel']))) {
        $aprs_call = $_POST['aprs_call'];
        if ($aprs_call !== "Call") {
          $aprs_msg = $_POST['aprs_msg'];
          $aprs_tx = $aprs_call . "^" .  $aprs_msg;
          $file = DL3EL . "/aprs-tx.msg";
          file_put_contents($file, $aprs_tx);
          $logtext =  "APRS Nachricht \"" . $aprs_msg . "\" an " . $aprs_call . " gesendet\n";
          addsvxlog($logtext);
        }  
    }
    echo '</div>'."\n";
    echo '</td></tr>';
    echo '</table>';

?>
<?php include_once "include/page_bottom.php"; ?>
