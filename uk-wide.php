<?php
if (session_status() === PHP_SESSION_NONE) { session_start(); }

if ($_SESSION['auth'] !== 'AUTHORISED') {
    echo "Nicht autorisiert";
    return;
}

// Pfad zur Zwischendatei
$cacheFile = __DIR__ . "/dl3el/uk_cache.json";
$currentTime = time();
$dateStr = date("H:i:s");

// 1. Daten vom Web laden (UK-Wide JSON)
$url = "https://ukwide.svxlink.net/status.php?json";
$jsonRaw = @file_get_contents($url);
$webData = json_decode($jsonRaw, true);
$webNodes = isset($webData['nodes']) ? $webData['nodes'] : [];

// 2. Cache laden
$cache = file_exists($cacheFile) ? json_decode(file_get_contents($cacheFile), true) : [];

$active_rows = "";
$history_rows = "";
$svxmqtt_color_active = "background-color: green;";
$svxmqtt_color_passive = "background-color: blue;";

// 3. Web-Daten verarbeiten
if (is_array($webNodes) && count($webNodes) > 0) {
    foreach ($webNodes as $nodeName => $info) {
        if (isset($info['hidden']) && $info['hidden'] === true) continue;

        $isTalking = isset($info['isTalker']) && $info['isTalker'] === true;
        $tg = (isset($info['monitoredTGs']) && !empty($info['monitoredTGs'])) ? $info['monitoredTGs'][0] : "---";
        $safeName = htmlspecialchars($nodeName);

        if ($isTalking) {
            if (!isset($cache[$nodeName]['start_ts']) || $cache[$nodeName]['talking'] === false) {
                $cache[$nodeName]['start_ts'] = $currentTime;
                $cache[$nodeName]['start_str'] = $dateStr;
            }
            $cache[$nodeName]['last_end'] = $dateStr; 
            $cache[$nodeName]['talking'] = true;
            $cache[$nodeName]['tg'] = $tg;

            $duration = $currentTime - $cache[$nodeName]['start_ts'];
            $active_rows .= "<tr style='height:28px; border-bottom: 1px solid #ddd; background-color: rgba(40, 167, 69, 0.2);' class='ukw-talking' data-start='".$cache[$nodeName]['start_ts']."'>"
                          . "<td style='width:35%;'>&nbsp;&nbsp;<b>$safeName</b></td>"
                          . "<td style='width:20%;'><button class='ukw-btn-active' style='$svxmqtt_color_active'>$tg</button></td>"
                          . "<td style='width:20%; text-align:center;'>".$cache[$nodeName]['start_str']."</td>"
                          . "<td style='width:25%; font-weight:bold; color:green;' class='ukw-duration'>".$duration."s</td>"
                          . "</tr>";
        } else {
            if (isset($cache[$nodeName])) {
                $cache[$nodeName]['talking'] = false;
            }
        }
    }
    // Cache nur speichern, wenn wir auch valide Daten vom Web bekommen haben
    file_put_contents($cacheFile, json_encode($cache));
}

// 4. Inaktive Liste aus Cache bauen
if (!empty($cache)) {
    uasort($cache, function($a, $b) {
        return strcmp($b['last_end'] ?? '', $a['last_end'] ?? '');
    });

    foreach ($cache as $nodeName => $cData) {
        if (isset($cData['talking']) && $cData['talking'] === false) {
            $safeName = htmlspecialchars($nodeName);
            $history_rows .= "<tr style='height:28px; border-bottom: 1px solid #ddd;'>"
                           . "<td style='width:35%;'>&nbsp;&nbsp;<b>$safeName</b></td>"
                           . "<td style='width:20%;'><button class='ukw-btn-active' style='$svxmqtt_color_passive'>".($cData['tg'] ?? '---')."</button></td>"
                           . "<td style='width:20%; text-align:center; color:#666;'>".($cData['last_end'] ?? '--:--')."</td>"
                           . "<td style='width:25%;'></td>"
                           . "</tr>";
        }
    }
}
?>

<style>
.ukw-table-container a.ukw-link:link, .ukw-table-container a.ukw-link:visited { color: black; text-decoration: underline; }
.ukw-table-container a.ukw-link:hover { color: #009900; }
.ukw-btn-active { border: none; border-radius: 8px; width: 85px; color: white; font-weight: bold; font-size: 14px; height: 24px; cursor: pointer; }
.ukw-table { width: 600px; text-align: left; border-collapse: collapse; background-color: #f1f1f1; font-family: Arial, sans-serif; font-size: 14px; border: 1px solid #ccc; }
</style>

<div class="ukw-table-container">
    <div style="font-weight:bold; margin-bottom:5px; font-family: Arial, sans-serif;">
        <a class="ukw-link" href="https://ukwide.svxlink.net/status.php" target="ukw">UK-Wide Status</a> 
        <span id="ukw-clock" style="font-weight:normal; font-size:13px; margin-left:10px; color: #555;">
            <?php echo $dateStr; ?>
        </span>
    </div>
    
    <table class="ukw-table">
        <thead>
            <tr style="height:30px; background-color: #e2e2e2; border-bottom: 2px solid #bbb;">
                <th style="width:35%;">&nbsp;&nbsp;Node</th>
                <th style="width:20%;">TG</th>
                <th style="width:20%; text-align:center;">Start/Ende</th>
                <th style="width:25%;">Dauer</th>
            </tr>
        </thead>
        <tbody>
            <?php echo $active_rows ?: "<tr><td colspan='4' style='text-align:center; padding:10px; color:#777; font-style:italic;'>Kein Funkbetrieb</td></tr>"; ?>
            
            <tr style="background-color:#ddd;">
                <td colspan="4" style="padding:4px; font-size:11px; color:#444;">&nbsp;<b>Zuletzt aktiv (lokaler Cache)</b></td>
            </tr>
            
            <?php echo $history_rows; ?>
        </tbody>
    </table>
</div>

<script>
// Funktion für das Live-Update (Uhr und Sekunden)
(function() {
    if (window.ukwInterval) clearInterval(window.ukwInterval);
    
    window.ukwInterval = setInterval(function() {
        const now = new Date();
        
        // Uhrzeit-Anzeige oben
        const clock = document.getElementById('ukw-clock');
        if (clock) clock.innerText = now.toLocaleTimeString('de-DE');

        // Sekunden-Counter für Aktive
        const nowTs = Math.floor(now.getTime() / 1000);
        document.querySelectorAll('.ukw-talking').forEach(function(row) {
            const start = parseInt(row.getAttribute('data-start'));
            const durationCell = row.querySelector('.ukw-duration');
            if (durationCell) durationCell.innerText = (nowTs - start) + 's';
        });
    }, 1000);
})();
</script>
