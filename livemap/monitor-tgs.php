<?php
// Version 20261112_1400
// FM-Funknetz LiveMap - Monitor-Talkgroups nur bei Dateiänderung neu einlesen.

header('Content-Type: application/json; charset=utf-8');
header('Cache-Control: no-store, no-cache, must-revalidate, max-age=0');
header('Pragma: no-cache');

$configFile = '/etc/svxlink/svxlink.conf';

if (!is_readable($configFile)) {
    http_response_code(500);
    echo json_encode(['success' => false, 'error' => 'svxlink.conf ist nicht lesbar.']);
    exit;
}

// Schnelle Prüfung: filemtime() liest nicht den Inhalt der Konfigurationsdatei.
$currentMtime = @filemtime($configFile);
if ($currentMtime === false) {
    http_response_code(500);
    echo json_encode(['success' => false, 'error' => 'Änderungszeitpunkt von svxlink.conf konnte nicht ermittelt werden.']);
    exit;
}

$clientMtime = isset($_GET['mtime']) ? (int)$_GET['mtime'] : -1;

// Datei unverändert: sofort antworten, ohne sie zu öffnen/einzulesen.
if ($clientMtime === (int)$currentMtime) {
    echo json_encode([
        'success' => true,
        'changed' => false,
        'mtime' => (int)$currentMtime
    ]);
    exit;
}

// Erst bei geändertem Zeitstempel die Konfiguration tatsächlich einlesen.
$configLines = file($configFile, FILE_IGNORE_NEW_LINES);
if ($configLines === false) {
    http_response_code(500);
    echo json_encode(['success' => false, 'error' => 'svxlink.conf konnte nicht gelesen werden.']);
    exit;
}

$inReflectorLogic = false;
$monitorTGsFound = false;
$monitorTGs = [];

foreach ($configLines as $configLine) {
    $line = trim($configLine);

    if ($line === '' || str_starts_with($line, '#') || str_starts_with($line, ';')) {
        continue;
    }

    if (preg_match('/^\[([^]]+)\]$/', $line, $sectionMatch)) {
        $inReflectorLogic = (strcasecmp(trim($sectionMatch[1]), 'ReflectorLogic') === 0);
        continue;
    }

    if ($inReflectorLogic && preg_match('/^MONITOR_TGS\s*=\s*(.*)$/i', $line, $tgMatch)) {
        $monitorTGsFound = true;
        foreach (explode(',', $tgMatch[1]) as $tgEntry) {
            // Nur Ziffern behalten; TGs als Strings behandeln (auch > 5 Stellen).
            $tg = preg_replace('/\D+/', '', trim($tgEntry));
            if ($tg !== '') {
                $monitorTGs[] = $tg;
            }
        }
        break;
    }
}

if (!$monitorTGsFound) {
    http_response_code(500);
    echo json_encode(['success' => false, 'error' => 'MONITOR_TGS wurde im Abschnitt [ReflectorLogic] nicht gefunden.']);
    exit;
}

$monitorTGs = array_values(array_unique($monitorTGs));

// Während des Lesens könnte die Datei erneut geändert worden sein.
// In diesem Fall den alten Zeitstempel zurückgeben, damit beim nächsten
// Intervall sicher erneut geprüft und eingelesen wird.
$mtimeAfterRead = @filemtime($configFile);
if ($mtimeAfterRead === false || (int)$mtimeAfterRead !== (int)$currentMtime) {
    http_response_code(503);
    echo json_encode(['success' => false, 'error' => 'svxlink.conf wurde während des Einlesens geändert.']);
    exit;
}

echo json_encode([
    'success' => true,
    'changed' => true,
    'mtime' => (int)$currentMtime,
    'monitorTGs' => $monitorTGs
], JSON_UNESCAPED_SLASHES | JSON_UNESCAPED_UNICODE);
?>
