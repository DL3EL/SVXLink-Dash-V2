<?php
if (session_status() === PHP_SESSION_NONE) {
    session_start();
}

include_once "../include/settings.php";

// call to filter own data from mqtt
    $ccall = $callsign . " ";

// file with mqtt data

    $file = DL3EL_BASE . "svx2mqtt/mqtt.data";
    $content = file_get_contents($file);
    $zeilen_array = explode("\n", $content);
    $anzahl_zeilen = count($zeilen_array); 
    $nn = $anzahl_zeilen - 1000;
    $umgekehrte_zeilen = array_reverse($zeilen_array);
    $nn = 0;
    While ($nn < 1000) {
      $line = $umgekehrte_zeilen[$nn];
//echo "L: $line <br>";          
      if (strlen($line)) {
//[21.02.2026 08:08:15] /server/mqtt/parrot/9990/analysis: {"tg": 9990, "callsign": "DO3FHS-APP", "timestamp": 1771535167.418658, "summary": {"avg_rms_db": -28.4, "max_peak_db": 0.0, "min_rms_db": -49.3, "max_rms_db": -2.1, "dynamics_db": 47.2, "clip_percent": 12.6, "silence_percent": 2.9, "total_frames": 579, "active_frames": 562, "duration_s": 11.6}, "recommendation": {"rating": 3, "rating_text": "\ud83d\udfe0 Grenzwertig", "level_status": "borderline", "message": "Clipping etwas hoch (13%).", "action": "Optional: TX-Pegel leicht reduzieren", "avg_rms_db": -28.4, "max_peak_db": 0.0, "clip_percent": 12.6, "meter_position": 72.0}}^1771657695^
        if ((str_contains($line,"/server/mqtt/parrot/9990/analysis")) || (str_contains($line,"/server/mqtt/parrot/9998/analysis"))) {
//get timestamp (if we need it)
            $timestamp_start = strpos($line, '^');
            if ($timestamp_start !== false) {
                $timestamp = substr($line, $timestamp_start);
                $timestamp = trim($timestamp,"\^");
            }  
            // 1. Den JSON-Teil sauber ausschneiden (ab der Position nach dem Doppelpunkt)
            // Wir suchen die erste "{" Klammer
            $json_start = strpos($line, '{');
            if ($json_start !== false) {
                $json_end = strpos($line, '^');
                $json_string = substr($line, $json_start,$json_end-$json_start);
                $json_string = trim($json_string);
                // 2. Den String in ein PHP-Array umwandeln
                $line_e = json_decode($json_string, true,3);
                if ($line_e !== null && json_last_error() === JSON_ERROR_NONE) {
                    $tg = $line_e['tg'];
                    $call = $line_e['callsign'];
                    $fcall = substr($call,0,strlen($ccall)-1) . " ";
                    if ($ccall === $fcall) {
                        $timestamp = $line_e['timestamp'];
                        $summary = $line_e['summary'];
                        $recommendation = $line_e['recommendation'];
                        $ts = date("H:i:s",$timestamp);

                        echo "<br>Soundcheck für $call, Ergebnis von $ts: <b>$recommendation[rating_text]</b> (Papagei TG $tg)<br> ";
                        echo "Empfehlung: $recommendation[message]<br>";
                        echo "Empfehlung: $recommendation[action], Level-Status: $recommendation[level_status]<br><br>Details:";
                        echo '<table style = "text-align: left;"';
                        echo "<tr> <td>Ergebnis:</td><td>&nbsp;</td><td>&nbsp;</td><td>&nbsp;</td><td>&nbsp;</td><td></td><td></td><td>&nbsp;&nbsp;</td></tr>";
                        echo "<tr><td>avg_rms_db</td><td> $summary[avg_rms_db] </td><td>&nbsp;&nbsp;</td><td>max_peak_db</td><td>$summary[max_peak_db]</td><td>&nbsp;&nbsp;</td><td>max_rms_db</td><td>$summary[max_rms_db]</td></tr>";
                        echo "<tr><td>dynamics_db</td><td> $summary[dynamics_db] </td><td>&nbsp;&nbsp;</td><td>clip_percent</td><td>$summary[clip_percent]</td><td>&nbsp;&nbsp;</td><td>silence_percent</td><td>$summary[silence_percent]</td></tr>";
                        echo "<tr><td>total_frames</td><td> $summary[total_frames] </td><td>&nbsp;&nbsp;</td><td>active_frames</td><td>$summary[active_frames]</td><td>&nbsp;&nbsp;</td><td>duration_s</td><td>$summary[duration_s]</td></tr>";
                        echo "<tr> <td>Empfehlungen:</td><td>&nbsp;</td><td>&nbsp;</td><td>&nbsp;</td><td>&nbsp;</td><td></td><td></td><td>&nbsp;&nbsp;</td></tr>";
                        echo "<tr><td>avg_rms_db</td><td> $recommendation[avg_rms_db] </td><td>&nbsp;&nbsp;</td><td>max_peak_db</td><td> $recommendation[max_peak_db] </td><td>&nbsp;&nbsp;</td><td>&nbsp;&nbsp;</td><td>&nbsp;&nbsp;</td></tr>";
                        echo "<tr><td>clip_percent</td><td>$recommendation[clip_percent]</td><td>&nbsp;&nbsp;</td><td>meter_position</td><td>$recommendation[meter_position]</td><td>&nbsp;&nbsp;</td><td>&nbsp;&nbsp;</td><td>&nbsp;&nbsp;</td></tr>";
                        echo '</table';
                        break;
                    }
                }
            } else {
              echo "JSON $json_string JSON-Fehler: " . json_last_error_msg() . " im String: $json_string";
            }
        }
    }
    ++$nn;
}
/*
                        echo"$call 1: $tg $timestamp ($ts) $summary[avg_rms_db] $recommendation[rating] $recommendation[rating_text]<br>";
                        echo"$call 2: $summary[avg_rms_db] $summary[max_peak_db] $summary[min_rms_db] $summary[max_rms_db] $summary[dynamics_db] $summary[clip_percent] <br>";
                        echo"$call 3: $summary[silence_percent] $summary[total_frames] $summary[min_rms_db] $summary[active_frames] $summary[duration_s]<br>";
                        echo"$call 4: $recommendation[rating] $recommendation[rating_text] $recommendation[level_status] $recommendation[message] $recommendation[action]<br>";
                        echo"$call 5: $recommendation[avg_rms_db] $recommendation[max_peak_db] $recommendation[clip_percent] $recommendation[meter_position]<br><br>";
*/                        

?>
