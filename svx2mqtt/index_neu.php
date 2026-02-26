<style>
a.hreflink:link,
a.hreflink:visited {
    color: black;
    text-decoration: underline;
}

a.hreflink:hover {
    color: #009900;	
    text-decoration: underline;
}
</style>

<?php
if (session_status() === PHP_SESSION_NONE) {
    session_start();
}

if (!isset($svx_include)) {
    include_once "../include/settings.php";
    include_once "../include/tgdb.php";    
}

if ($_SESSION['auth'] !== 'AUTHORISED') {
    echo '<button id="n/a" style="margin-right:90px; width:150px; color:white; background-color:red; border-color:transparent; border-radius:8px; height:40px; font-size:16px;"><b>nicht angemeldet</b></button>';
    return;
}

// MQTT Check (unverändert)
$mqtt_script = shell_exec("pgrep fmn-mqtt.pl");
if (!strlen($mqtt_script)) {
    start_mqtt();
}

// Farben definieren (unverändert)
$svxmqtt_color_active = defined('SVXMQTT_COLOR_active') ? "style=cursor:pointer;border:none;border-radius:8px;width:85px;background-color:" . SVXMQTT_COLOR_active . ";color:white;font-weight:bold;font-size:14px;" : "green";
$svxmqtt_color_passive = defined('SVXMQTT_COLOR_passive') ? "style=cursor:pointer;border:none;border-radius:8px;width:85px;background-color:" . SVXMQTT_COLOR_passive . ";color:white;font-weight:bold;font-size:14px;" : "blue";
$svxmqtt_color_friends = defined('SVXMQTT_COLOR_friends') ? "background-color:" . SVXMQTT_COLOR_friends . ";" : "green";

$file = DL3EL_BASE . "svx2mqtt/mqtt.data";
$friend_call_array = defined('DL3EL_FRIENDS') ? explode(",", DL3EL_FRIENDS) : [0];

$array = [];
$array_time = [];
$loginz = 0;
$mqtt_start_ts = 0;

// --- OPTIMIERTER DATEIZUGRIFF (Rückwärts lesen) ---
if (file_exists($file)) {
    $handle = fopen($file, 'r');
    fseek($handle, 0, SEEK_END);
    $pos = ftell($handle);
    $lines_to_read = 3000;
    $lines_found = 0;
    $buffer = '';
    
    // Wir lesen in 4KB Blöcken rückwärts
    while ($pos > 0 && $lines_found < $lines_to_read) {
        $seek = min($pos, 4096);
        $pos -= $seek;
        fseek($handle, $pos);
        $chunk = fread($handle, $seek) . $buffer;
        $lines = explode("\n", $chunk);
        $buffer = array_shift($lines); // Der Anfang des Blocks könnte eine unvollständige Zeile sein
        
        // Zeilen von unten nach oben verarbeiten
        for ($i = count($lines) - 1; $i >= 0; $i--) {
            if ($lines_found >= $lines_to_read) break;
            $line = trim($lines[$i]);
            if (empty($line)) continue;
            
            // --- DEINE ORIGINAL LOGIK ---
            if (str_contains($line, "server/statethr/1")) {
                $timestamp_start = strpos($line, '^');
                $timestamp = ($timestamp_start !== false) ? trim(substr($line, $timestamp_start), "\^") : 0;
                
                $json_start = strpos($line, '{');
                if ($json_start !== false) {
                    $json_end = strpos($line, '}') + 1;
                    $json_string = substr($line, $json_start, $json_end - $json_start);
                    $line_e = json_decode($json_string, true);
                    
                    if ($line_e) {
                        $time = $line_e['time'];
                        $talk_state = $line_e['talk'];
                        $call = $line_e['call'];
                        $tg = $line_e['tg'];
                        
                        // Nur verarbeiten, wenn dieses Call-Ereignis noch nicht finalisiert wurde
                        // Da wir rückwärts lesen, ignorieren wir Dubletten
                        if (!isset($array[$call]) && !isset($array[$call . "^" . $timestamp])) {
                            $array[$call]["call"] = $call;
                            $array[$call]["talk_state"] = $talk_state;
                            $tgnumber = $tg;
                            $name = trim(isset($tgdb_array[$tgnumber]) ? $tgdb_array[$tgnumber] : '---');

                            if ($talk_state === "start") {
                                $array[$call]["time_1"] = $time;
                                $array[$call]["time_2"] = (time() - $timestamp) . "s";
                                $array[$call]["talker"] = "1";
                                $array[$call]["ts-start"] = $timestamp;
                                $array[$call]["tg"] = sprintf("&nbsp;<button style='%s' type=submit id=jumptoA name=jmptoA class=active_id value='%s'>%s</button>", $svxmqtt_color_active, $tg, $tg);
                                $array[$call]["tgn"] = $name;
                            } else {
                                // Stop-Event gefunden: Da wir rückwärts lesen, suchen wir das Start-Event (falls vorhanden) 
                                // In der "Rückwärtslogik" ist ein Stop-Event eigentlich das Erste, was wir sehen.
                                // Um dein Layout-System beizubehalten, speichern wir es temporär.
                                $unique_call = $call . "^" . $timestamp;
                                $array[$unique_call]["talker"] = "0";
                                $array[$unique_call]["time_2"] = $time;
                                $array[$unique_call]["ts-stop"] = $timestamp;
                                $array[$unique_call]["tgn"] = $name;
                                $array[$unique_call]["tg"] = sprintf("&nbsp;<button style='%s' type=submit id=jumptoA name=jmptoA class=active_id value='%s'>%s</button>", $svxmqtt_color_passive, $tg, $tg);
                                
                                // Da wir nicht wissen, wann der Start war (weil wir rückwärts lesen),
                                // behelfen wir uns hier mit einer vereinfachten Zeitschätzung oder 
                                // warten, bis das 'start' im Loop auftaucht (komplexer).
                                // Einfachheitshalber nutzen wir hier deine bestehende Struktur:
                                $array[$unique_call]["time_1"] = $time; // Dummy, da Startzeit rückwärts später käme
                                $array[$unique_call]["ts-start"] = $timestamp; 
                            }
                        }
                    }
                }
            } elseif (str_contains($line, "/server/state/loginz: ")) {
                if ($loginz == 0) { // Nur das aktuellste
                    $loginz_start = strpos($line, ': ') + 2;
                    $loginz_end = strpos($line, '^');
                    $loginz = substr($line, $loginz_start, $loginz_end - $loginz_start);
                }
            } elseif (str_contains($line, "DL3EL: MQTT Start")) {
                if ($mqtt_start_ts == 0) {
                    $mqtt_start = strpos($line, 't^') + 2;
                    $mqtt_start_ts = trim(substr($line, $mqtt_start), "\^");
                }
            }
            $lines_found++;
        }
    }
    fclose($handle);
}

// --- TRANSFORMATION (Mit Fix für Undefined Keys) ---
foreach ($array as $call_key => $data) {
    // Falls ein Eintrag unvollständig ist, Standardwerte setzen
    $ts_start   = $data['ts-start'] ?? 0;
    $act        = $data['talker'] ?? "0";
    $tg_html    = $data['tg'] ?? "&nbsp;";
    $tg_name    = $data['tgn'] ?? "---";
    $t_1        = $data['time_1'] ?? "--:--";
    $t_2        = $data['time_2'] ?? "--:--";

// --- FILTER: Zeile nur verarbeiten, wenn TG UND Zeit vorhanden sind ---
    if (($tg_html === "&nbsp;") || ($t_1 === "--:--")) {
        continue; 
    }
    $clean_call = (strpos($call_key, '^') !== false) 
                  ? substr($call_key, 0, strpos($call_key, '^')) 
                  : $call_key;

    // Friend check
    $color_friend = "";
    if (defined('DL3EL_FRIENDS')) {
        foreach ($friend_call_array as $fcall) {
            if (!empty(trim($fcall)) && str_starts_with($clean_call, trim($fcall))) {
                $color_friend = "background-color:" . SVXMQTT_COLOR_friends . ";";
                break;
            }
        }
    }

    // Zeile zusammenbauen (unter Verwendung der sicheren Variablen)
    $line_content = "<tr style='height:24px;" . $color_friend . "'>"
                  . "<td>" . htmlspecialchars($clean_call) . "&nbsp;</td>"
                  . "<td>" . $tg_html . "&nbsp;</td>"
                  . "<td>" . htmlspecialchars($tg_name) . "&nbsp;</td>"
                  . "<td>" . htmlspecialchars($t_1) . "&nbsp;</td>"
                  . "<td>" . htmlspecialchars($t_2) . "&nbsp;</td>"
                  . "</tr>";
    
    // Filter für MQTT Start (wie im Original)
    if (!($act == "1" && $ts_start < $mqtt_start_ts)) {
        $array_time[$ts_start] = $act . $line_content;
    }
}

krsort($array_time);

// --- BILDSCHIRMAUSGABE ---
echo '<a class="hreflink" href="https://dashboard.fm-funknetz.de/" style="font-weight:bold; font-size:15px;" target="fmn">FM-Funknetz Live Activity</a>';
echo '<form method="post">';
echo '<table style="width: 500px; text-align: left; margin-top: 5px; margin-bottom:0px; border:0; border-collapse:collapse; background-color:#f1f1f1;">';
echo '<thead><tr style="height:24px;">';
echo '<th style="width:22%;">&nbsp;&nbsp;Call</th><th style="width:12%">TG</th><th style="width:40%">TG Name&nbsp;&nbsp;(Logins:' . htmlspecialchars($loginz) . ')</th><th style="width:12%">von</th><th style="width:13%">bis/Dauer&nbsp;</th>';
echo '</tr></thead>';

// Aktive Sprecher
foreach ($array_time as $line) {
    if (substr($line, 0, 1) === "1") echo substr($line, 1);
}

echo "<tr><td colspan='5'>&nbsp;</td></tr><tr><td colspan='2'>&nbsp;</td><td colspan='3'><b>zuletzt aktiv</b></td></tr>";

// Letzte Aktivitäten (Historie)
$nn = 0;
foreach ($array_time as $line) {
    if (substr($line, 0, 1) === "0") {
        echo substr($line, 1);
        if (++$nn > 30) break;
    }
}
echo "</table></form>";
?>
