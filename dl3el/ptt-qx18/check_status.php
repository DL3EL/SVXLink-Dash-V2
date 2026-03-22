<?php
header('Content-Type: text/plain');
// Wir bleiben beim absoluten Pfad via __DIR__
//$status_file = __DIR__ . "/ptt_status.txt";
$status_file = "/tmp/ptt_status.txt";

if (file_exists($status_file)) {
    echo trim(file_get_contents($status_file));
} else {
    echo "OFF";
}
?>
