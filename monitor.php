<?php
if (session_status() === PHP_SESSION_NONE) {
    session_start();
}
include_once "include/settings.php";
include_once "include/tgdb.php";    

// FIX: Variable setzen, damit index_neu.php weiß, dass settings geladen sind
$svx_include = true;

$svxConfigFile = SVXCONFPATH . "/" . SVXCONFIG;

if (file_exists($svxConfigFile)) {
    $svxconfig = parse_ini_file($svxConfigFile, true, INI_SCANNER_RAW); 
}

echo '<table style="margin-bottom:0px;border:0; border-collapse:collapse; background-color:#f1f1f1;"><tr style="border:none;background-color:#f1f1f1;">';

if ($_SESSION['auth'] !== 'AUTHORISED') {
    echo '<button id="n/a" style="margin-right:90px; width:150px; color:white; background-color:red; border-color:transparent; border-radius:8px; height:40px; font-size:16px;"><b>nicht angemeldet</b></button>';
    return;
}

// --- Logik zum Sammeln der TGs aus der Config ---
$tgmon = explode(",", $svxconfig['ReflectorLogic']['MONITOR_TGS'] ?? "");
$monitored_list = [];

foreach ($tgmon as $key) {
    $tg = trim(str_replace(" ", "", $key), "+");
    if (!empty($tg)) $monitored_list[] = $tg;
}

if (defined('DL3EL')) {
    $svxStatusFile = DL3EL . "/tg_status";
    if (file_exists($svxStatusFile)) {
        $svxdata = trim(file_get_contents($svxStatusFile));
        if (!empty($svxdata)) $monitored_list[] = $svxdata;
    }
}

// Dubletten entfernen
$monitored_list = array_unique($monitored_list);

// --- DER ERSATZ FÜR DEN PERL-AUFRUF (Der Alles-Fresser) ---
$file = DL3EL_BASE . "svx2mqtt/mqtt.data"; // Pfad ggf. anpassen (mqtt.data oder activeTGs.data)
$call_tab_pr = [];

// Funktion zum effizienten Lesen der letzten Zustände
function get_tg_status_efficient($filename, $limit, &$call_tab_pr) {
    if (!file_exists($filename)) return;
    $handle = fopen($filename, 'r');
    fseek($handle, 0, SEEK_END);
    $pos = ftell($handle);
    $buffer_size = 4096;
    $line_count = 0;
    $leftover = '';

    while ($pos > 0 && $line_count < $limit) {
        $read_size = min($pos, $buffer_size);
        $pos -= $read_size;
        fseek($handle, $pos);
        $chunk = fread($handle, $read_size) . $leftover;
        $lines = explode("\n", $chunk);
        $leftover = array_shift($lines);
        
        for ($i = count($lines) - 1; $i >= 0; $i--) {
            $line = trim($lines[$i]);
            if (empty($line) || !str_contains($line, "/server/state/")) continue;

            $tg_start = strpos($line, 'TG/') + 3;
            $tg_end = strpos($line, ':', $tg_start);
            if ($tg_start === false || $tg_end === false) continue;
            
            $tg = substr($line, $tg_start, $tg_end - $tg_start);

            if (!isset($call_tab_pr[$tg])) {
                $b_open = strpos($line, '[', $tg_end);
                $b_close = strpos($line, ']', $b_open);
                if ($b_open !== false && $b_close !== false) {
                    $content = str_replace('"', '', substr($line, $b_open + 1, $b_close - $b_open - 1));
                    $call_tab_pr[$tg] = array_filter(array_map('trim', explode(",", $content)));
                } else {
                    $call_tab_pr[$tg] = [];
                }
                $line_count++;
            }
            if ($line_count >= $limit) break;
        }
    }
    fclose($handle);
}

// Daten einlesen (wir scannen die letzten 2000 Zeilen nach den TGs)
get_tg_status_efficient($file, 2000, $call_tab_pr);


// ... (Die get_tg_status_efficient Funktion und Vorbereitung bleibt identisch)

// 1. Gesamtüberschrift (Bleibt als TD, da es die Hauptüberschrift der Seite ist)
date_default_timezone_set('Europe/Berlin');
$update_time = date("H:i:s");
echo "<tr><td colspan='2' style='padding: 10px 5px;'>";
echo "<span style='font-size: 18px; font-weight: bold; color: #333;'>Teilnehmer pro abonnierter TG</span> ";
echo "<span style='font-size: 14px; color: #666;'>(Stand: $update_time Uhr)</span>";
echo "</td></tr>";

// --- BILDCHIRMAUSGABE ---
foreach ($monitored_list as $tg) {
    $name = trim(isset($tgdb_array[$tg]) ? $tgdb_array[$tg] : '---');
    $calls = isset($call_tab_pr[$tg]) ? $call_tab_pr[$tg] : [];
    $anzahl = count($calls);

    // --- ÄNDERUNG: Nutzt jetzt TH für die Talkgroup-Überschrift ---
    echo "<tr><th colspan='2' style='padding: 5px 10px; text-align: left;'>";
    echo "<div style='display: flex; justify-content: space-between; align-items: center;'>";
    
    // Der Button-Link
    printf(
        '<button type="submit" id="jmptoA" name="jmptoA" class="active_id" value="%s" 
         style="cursor:pointer; background:none; border:1px solid rgba(255,255,255,0.3); color:inherit; border-radius:4px; padding:2px 8px; font-weight:bold;">
         %s (%s)
         </button>',
        $tg, 
        $tg, 
        $name
    );

    echo "<span style='font-weight: normal; font-size: 0.9em; opacity: 0.8;'>($anzahl Teilnehmer)</span>";
    echo "</div>";
    echo "</th></tr>";

    // Teilnehmerliste
    echo "<tr><td colspan='2' style='padding: 10px 15px; background-color: #f9f9f9; border-left: 1px solid #ddd; border-right: 1px solid #ddd;'>";
    
    if ($anzahl > 0) {
        $linked_calls = array_map(function($call) {
            $url = "https://dashboard.fm-funknetz.de/node.php?call=" . urlencode($call);
            return '<a class="hreflink" href="' . $url . '" target="_blank">' . htmlspecialchars($call) . '</a>';
        }, $calls);

        $zeilen_stuecke = array_chunk($linked_calls, 10);
        
        foreach ($zeilen_stuecke as $index => $stueck) {
            echo implode(", ", $stueck);
            if ($index < count($zeilen_stuecke) - 1) {
                echo ",<br>";
            }
        }
    } else {
        echo "<em>keine Teilnehmer aktiv</em>";
    }

    echo "</td></tr>";
    
    // Abschlusskante und Abstand
    echo "<tr><td colspan='2' style='height: 1px; background-color: #ddd; border-bottom: 1px solid #ddd;'></td></tr>";
    echo "<tr><td colspan='2' style='height: 10px;'></td></tr>";
}

echo "<tr><td colspan='2'>&nbsp;</td></tr>";
?>
</table>
