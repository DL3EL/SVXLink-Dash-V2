<?php
if (session_status() === PHP_SESSION_NONE) {
    session_start();
}
// 1. Bereich für die APRS Daten (Block-Element)
echo '<div id="aprs_data_container" style="display: block; min-height: 20px;">';
  // Optional: Hier könntest du svxaprs.php direkt einmal inkludieren, 
  // damit beim ersten Laden keine Verzögerung entsteht.
  include "include/svxaprs.php"; 
echo '</div>';

// 2. Ein Trenner oder Abstandshalter (Optional)
//echo '<hr style="margin: 20px 0;">';

// 3. Das Formular (Ebenfalls als Block oder in eigenem Div)
echo '<div id="aprs_form_container" style="margin-top: 10px;">';
    echo '<form method="post" style="display: inline;">';
      echo ' Nachricht an <input type="text" name="aprs_call" value="DL3EL" required>';
      echo '&nbsp;&nbsp;';
      echo '<input type="text" style="width:300px" name="aprs_msg" value="Text" required>';
      echo '&nbsp;&nbsp;';
      echo '<input type="submit" name="senden" value="senden">';  
    echo '</form>';
echo '</div>';
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

// 4. Das Script (unverändert, zielt auf den Container oben)
$rate = 10000;
echo '<script type="text/javascript">
function reloadaprs(){
    $("#aprs_data_container").load("include/svxaprs.php", function(){ 
        setTimeout(reloadaprs, ' . $rate . '); 
    });
}
// Wir starten den Timer, aber laden nicht sofort neu, 
// da wir oben per "include" die Daten schon einmalig geladen haben.
setTimeout(reloadaprs, ' . $rate . ');
</script>';
        include_once "caller_svxmqtt.php"; 
    echo '</tr></table>';
include_once "include/page_bottom.php"; 
?>

