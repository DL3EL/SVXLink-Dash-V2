<?php
if (session_status() === PHP_SESSION_NONE) {
    session_start();
}
    echo '<div id="content" style = "margin-bottom:30px;">'."\n";
    $call_script = "include/svxaprs.php";
    include $call_script;
    $rate = 10000;
    echo '<script type="text/javascript">'."\n";
    echo 'function reloadaprs(){'."\n";
    echo '  $("#content").load("' . $call_script . '",function(){ setTimeout(reloadaprs,' . $rate . ') });'."\n";
    echo '}'."\n";
    echo 'setTimeout(reloadaprs,' . $rate . ');'."\n";
    echo '$(window).trigger(\'resize\');'."\n";
    echo '</script>'."\n";
        
    echo '</div>'."\n";

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
    echo '</td>';
        include_once "caller_svxmqtt.php"; 
    echo '</tr></table>';
include_once "include/page_bottom.php"; 
?>

