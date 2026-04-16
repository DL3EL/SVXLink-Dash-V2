<?php
header('Content-Type: text/plain');
// Wir bleiben beim absoluten Pfad via __DIR__
//$status_file = __DIR__ . "/ptt_status.txt";
$status_file = "/tmp/ptt_status.txt";
$rxtx_state = "/tmp/svxlink_last_state.txt";
if (file_exists($rxtx_state)) { 
    $content = file_get_contents($rxtx_state);
    if (strpos($content, "OPEN")) {
        echo "ON";
    } else {  
        echo "OFF";
    }    
} else {
    if (file_exists($status_file)) {
        echo trim(file_get_contents($status_file));
    } else {
        echo "OFF";
    }
}    
?>
