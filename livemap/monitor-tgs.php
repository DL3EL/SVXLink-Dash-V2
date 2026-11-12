<?php
// Version 20261112
// FM-Funknetz LiveMap - Monitor-Talkgroups dynamisch bereitstellen
// Wird von livemap.js alle 30 Sekunden im Hintergrund aufgerufen.

header('Content-Type: application/json; charset=utf-8');
header('Cache-Control: no-store, no-cache, must-revalidate, max-age=0');
header('Pragma: no-cache');

$configFile = '/etc/svxlink/svxlink.conf';

if (!is_readable($configFile)) {
    http_response_code(500);
    echo json_encode([
        'success' => false,
        'error' => 'svxlink.conf ist nicht lesbar.'
    ]);
    exit;
}

$configLines = file($configFile, FILE_IGNORE_NEW_LINES);
if ($configLines === false) {
    http_response_code(500);
    echo json_encode([
        'success' => false,
        'error' => 'svxlink.conf konnte nicht gelesen werden.'
    ]);
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

        // Nur Ziffern übernehmen; + und > sind reine Kennzeichnungen.
        foreach (explode(',', $tgMatch[1]) as $tgEntry) {
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
    echo json_encode([
        'success' => false,
        'error' => 'MONITOR_TGS wurde im Abschnitt [ReflectorLogic] nicht gefunden.'
    ]);
    exit;
}

$monitorTGs = array_values(array_unique($monitorTGs));

echo json_encode([
    'success' => true,
    'monitorTGs' => $monitorTGs
], JSON_UNESCAPED_SLASHES | JSON_UNESCAPED_UNICODE);
