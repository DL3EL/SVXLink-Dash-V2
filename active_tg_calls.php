<?php
if (session_status() === PHP_SESSION_NONE) { session_start(); }
include_once "include/settings.php";
include_once "include/tgdb.php";

// Zeitzone setzen für den Zeitstempel
date_default_timezone_set('Europe/Berlin');
$update_time = date("H:i:s");

// 1. Aktive TG ermitteln
$svxStatusFile = DL3EL . "/tg_status";
if (!file_exists($svxStatusFile)) return;
$active_tg = trim(file_get_contents($svxStatusFile));
if (empty($active_tg)) return;

// 2. Daten sammeln
$file = DL3EL_BASE . "svx2mqtt/mqtt.data"; 
$all_calls = [];

if (file_exists($file)) {
    $data = shell_exec("tail -c 25600 " . escapeshellarg($file));
    $lines = explode("\n", $data);

    $pattern_active = "/activeTG/" . $active_tg . ":";
    $pattern_monitor = "/monitoredTG/" . $active_tg . ":";
    
    $found_active = false;
    $found_monitor = false;

    for ($i = count($lines) - 1; $i >= 0; $i--) {
        $is_active = (strpos($lines[$i], $pattern_active) !== false);
        $is_monitor = (strpos($lines[$i], $pattern_monitor) !== false);

        if (($is_active && !$found_active) || ($is_monitor && !$found_monitor)) {
            $line = $lines[$i];
            $start = strpos($line, '[');
            $end = strrpos($line, ']'); 

            if ($start !== false && $end !== false) {
                $content = substr($line, $start + 1, $end - $start - 1);
                preg_match_all('/"([^"]+)"/', $content, $matches);
                
                if (!empty($matches[1])) {
                    foreach ($matches[1] as $call) {
                        $all_calls[] = trim($call);
                    }
                }
                
                if ($is_active) $found_active = true;
                if ($is_monitor) $found_monitor = true;
            }
        }
        if ($found_active && $found_monitor) break;
    }
}

$all_calls = array_unique($all_calls);
natcasesort($all_calls);
$anzahl = count($all_calls);

// --- 3. AUSGABE ---
$name = isset($tgdb_array[$active_tg]) ? $tgdb_array[$active_tg] : '---';

echo '<table style="width: 100%; max-width: 650px; margin: 0 auto; border-collapse: collapse; border: none;">';
    echo '<thead>';
        echo '<tr>';
            // Hier nutzen wir dein CSS-definiertes <th>
            echo '<th style="text-align: left; padding: 8px 10px;">';
                echo '<div style="display: flex; justify-content: space-between; align-items: center; font-weight: bold;">';
                    echo '<span>TG ' . $active_tg . ' (' . $name . ')</span>';
                    echo '<span style="font-weight: normal; font-size: 0.85em;">' . $anzahl . ' Calls | Stand: ' . $update_time . '</span>';
                echo '</div>';
            echo '</th>';
        echo '</tr>';
    echo '</thead>';
    echo '<tbody>';
        echo '<tr>';
            echo '<td style="background-color: #f9f9f9; border: 1px solid #ccc; padding: 12px; text-align: left; line-height: 1.7;">';

            if ($anzahl > 0) {
                $linked_calls = array_map(function($c) {
                    $url = "https://dashboard.fm-funknetz.de/node.php?call=" . urlencode($c);
                    return '<a class="hreflink" style="white-space: nowrap; text-decoration: none; font-weight: 500;" href="' . $url . '" target="_blank">' . htmlspecialchars($c) . '</a>';
                }, $all_calls);

                $chunks = array_chunk($linked_calls, 8);
                $rows = [];
                foreach ($chunks as $chunk) {
                    $rows[] = implode(", ", $chunk);
                }
                echo implode(",<br>", $rows);
            } else {
                echo '<em style="color: #999;">Keine Teilnehmer aktiv</em>';
            }

            echo '</td>';
        echo '</tr>';
    echo '</tbody>';
echo '</table>';

?>
