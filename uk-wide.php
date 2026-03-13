<?php
if (session_status() === PHP_SESSION_NONE) { session_start(); }

if ($_SESSION['auth'] !== 'AUTHORISED') {
    echo "Nicht autorisiert";
    return;
}

// 1. Abhängigkeiten laden
if (!isset($svx_include)) {
    include_once "include/settings.php";
}
include_once "include/settings.php";

/**
 * Funktion um die UK-TG-Datenbank isoliert zu laden
 */
function loadUkTgDatabase() {
    $file = "include/tgdb_uk.php";
    if (!file_exists($file)) return [];
    
    return (function($file) {
        include $file;
        $vars = get_defined_vars();
        foreach ($vars as $k => $v) {
            if ($k !== 'file' && is_array($v)) return $v;
        }
        return [];
    })($file);
}

$uk_tgs = loadUkTgDatabase();
$cacheFile = __DIR__ . "/dl3el/uk_cache.json";
$currentTime = time();
$dateStr = date("H:i:s");
$refLog = isset($reflectorlogic1) ? $reflectorlogic1 : "ReflectorLogic";

// 2. Daten vom Web laden
$url = "https://ukwide.svxlink.net/status.php?json";
$jsonRaw = @file_get_contents($url);
$webData = json_decode($jsonRaw, true);
$webNodes = isset($webData['nodes']) ? $webData['nodes'] : [];

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
        
        $tgName = "";
        if (isset($uk_tgs[$tg])) {
            $tgName = $uk_tgs[$tg];
        } elseif (isset($uk_tgs[(int)$tg])) {
            $tgName = $uk_tgs[(int)$tg];
        }
        
        $displayTG = ($tgName !== "") ? "<span>$tg</span><br><small>" . htmlspecialchars($tgName) . "</small>" : "<span>$tg</span>";
        $safeName = htmlspecialchars($nodeName);

        if ($isTalking) {
            if (!isset($cache[$nodeName]['start_ts']) || (isset($cache[$nodeName]['talking']) && $cache[$nodeName]['talking'] === false)) {
                $cache[$nodeName]['start_ts'] = $currentTime;
                $cache[$nodeName]['start_str'] = $dateStr;
            }
            $cache[$nodeName]['last_end'] = $dateStr; 
            $cache[$nodeName]['talking'] = true;
            $cache[$nodeName]['tg'] = $tg;
            $cache[$nodeName]['tgName'] = $tgName;

            $duration = $currentTime - $cache[$nodeName]['start_ts'];
            $active_rows .= "<tr style='height:40px; border-bottom: 1px solid #ddd; background-color: rgba(40, 167, 69, 0.2);' class='ukw-talking' data-start='".$cache[$nodeName]['start_ts']."'>"
                          . "<td style='width:25%;'>&nbsp;&nbsp;<b>$safeName</b></td>"
                          . "<td style='width:35%; padding:4px;'><button type='submit' name='jmptoA' value='$tg' class='ukw-btn-active' style='$svxmqtt_color_active;'>$displayTG</button></td>"
                          . "<td style='width:20%; text-align:center;'>".$cache[$nodeName]['start_str']."</td>"
                          . "<td style='width:20%; font-weight:bold; color:green;' class='ukw-duration'>".$duration."s</td>"
                          . "</tr>";
        } else {
            if (isset($cache[$nodeName])) {
                $cache[$nodeName]['talking'] = false;
            }
        }
    }
    file_put_contents($cacheFile, json_encode($cache));
}

// 4. Inaktive Liste aus Cache
if (!empty($cache)) {
    uasort($cache, function($a, $b) {
        return strcmp($b['last_end'] ?? '', $a['last_end'] ?? '');
    });

    foreach ($cache as $nodeName => $cData) {
        if (isset($cData['talking']) && $cData['talking'] === false) {
            $safeName = htmlspecialchars($nodeName);
            $tgVal = ($cData['tg'] ?? '---');
            $tgName = ($cData['tgName'] ?? '');
            $displayTG = ($tgName !== "") ? "<span>$tgVal</span><br><small>" . htmlspecialchars($tgName) . "</small>" : "<span>$tgVal</span>";

            $history_rows .= "<tr style='height:40px; border-bottom: 1px solid #ddd;'>"
                           . "<td style='width:25%;'>&nbsp;&nbsp;<b>$safeName</b></td>"
                           . "<td style='width:35%; padding:4px;'><button type='submit' name='jmptoA' value='$tgVal' class='ukw-btn-active' style='$svxmqtt_color_passive;'>$displayTG</button></td>"
                           . "<td style='width:20%; text-align:center; color:#666;'>".($cData['last_end'] ?? '--:--')."</td>"
                           . "<td style='width:20%;'></td>"
                           . "</tr>";
        }
    }
}
?>

<style>
.ukw-table-container a.ukw-link:link, .ukw-table-container a.ukw-link:visited { color: black; text-decoration: underline; }
.ukw-table-container a.ukw-link:hover { color: #009900; }
.ukw-btn-active { border: none; border-radius: 8px; width: 100%; max-width: 170px; color: white; font-weight: bold; font-size: 14px; cursor: pointer; height: auto; line-height: 1.1; padding: 5px 0; }
.ukw-table { width: 600px; text-align: left; border-collapse: collapse; background-color: #f1f1f1; font-family: Arial, sans-serif; font-size: 14px; border: 1px solid #ccc; }
.ukw-table small { font-size: 10px; font-weight: normal; opacity: 0.95; display: block; margin-top: 2px; line-height: 1; }
.ukw-table span { font-size: 14px; }
</style>

<div class="ukw-table-container" id="ukw-wrapper">
    <div style="font-weight:bold; margin-bottom:5px; font-family: Arial, sans-serif;">
        <a class="ukw-link" href="https://ukwide.svxlink.net/status.php" target="_blank">UK-Wide Status</a> 
        <span id="ukw-clock" style="font-weight:normal; font-size:13px; margin-left:10px; color: #555;">
            <?php echo $dateStr; ?>
        </span>
    </div>
    
    <form id="ukw-tg-form" method="post" action="index.php">
        <input type="hidden" name="ref" value="<?php echo $refLog; ?>">
        <table class="ukw-table">
            <thead>
                <tr style="height:30px; background-color: #e2e2e2; border-bottom: 2px solid #bbb;">
                    <th style="width:25%;">&nbsp;&nbsp;Node</th><th style="width:35%;">TG / Name</th><th style="width:20%; text-align:center;">Start/Ende</th><th style="width:20%;">Dauer</th>
                </tr>
            </thead>
            <tbody>
                <?php echo $active_rows ?: "<tr><td colspan='4' style='text-align:center; padding:15px; color:#777;'>Kein Funkbetrieb</td></tr>"; ?>
                <tr style="background-color:#ddd;"><td colspan="4" style="padding:4px; font-size:11px; color:#444;">&nbsp;<b>Zuletzt aktive Nodes</b></td></tr>
                <?php echo $history_rows; ?>
            </tbody>
        </table>
    </form>
</div>

<script>
(function() {
    // 1. Timer für Uhrzeit und Dauer
    if (window.ukwInterval) clearInterval(window.ukwInterval);
    window.ukwInterval = setInterval(function() {
        const now = new Date();
        const clock = document.getElementById('ukw-clock');
        if (clock) clock.innerText = now.toLocaleTimeString('de-DE');
        const nowTs = Math.floor(now.getTime() / 1000);
        document.querySelectorAll('.ukw-talking').forEach(function(row) {
            const start = parseInt(row.
