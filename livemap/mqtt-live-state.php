<?php
// FM-Funknetz LiveMap
// Liest inkrementell die MQTT-Nachrichten aus der bestehenden mqtt.data.
// Es wird keine eigene MQTT-Verbindung aufgebaut.

declare(strict_types=1);

header('Content-Type: application/json; charset=utf-8');
header('Cache-Control: no-store, no-cache, must-revalidate, max-age=0');
header('Pragma: no-cache');

//$dataFile = __DIR__ . DIRECTORY_SEPARATOR . 'svx2mqtt' . DIRECTORY_SEPARATOR . 'mqtt.data';
$dataFile = __DIR__ . '/../svx2mqtt/mqtt.data';

function respond(int $status, array $payload): never
{
    http_response_code($status);
    echo json_encode($payload, JSON_UNESCAPED_SLASHES | JSON_UNESCAPED_UNICODE | JSON_INVALID_UTF8_SUBSTITUTE);
    exit;
}

function normalizeTgValue(string $value): string
{
    return preg_replace('/\\D+/', '', trim($value)) ?? '';
}

// Sprecher bleiben bis zu einem STOP-Ereignis oder MQTT-Neustart aktiv.


/**
 * Ermittelt den aktuellen Sprecherzustand wie index_neu.php durch
 * Rückwärtsauswertung der jüngsten MQTT-Ereignisse.
 * Pro Rufzeichen zählt ausschließlich das zeitlich jüngste START/STOP.
 */
function readRecentTalkers(string $file): array
{
    $handle = @fopen($file, 'rb');
    if ($handle === false) {
        return [];
    }

    $maxLines = 3000;
    $pos = fseek($handle, 0, SEEK_END) === 0 ? ftell($handle) : 0;
    $buffer = '';
    $linesNewestFirst = [];

    // In Blöcken rückwärts lesen; nur die jüngsten 3000 Logzeilen behalten.
    while ($pos > 0 && count($linesNewestFirst) < $maxLines) {
        $chunkSize = min($pos, 8192);
        $pos -= $chunkSize;
        if (fseek($handle, $pos) !== 0) {
            break;
        }
        $chunk = fread($handle, $chunkSize) . $buffer;
        $parts = explode("\n", $chunk);
        $buffer = array_shift($parts) ?? '';

        for ($i = count($parts) - 1; $i >= 0; $i--) {
            $line = trim($parts[$i]);
            if ($line === '') {
                continue;
            }
            $linesNewestFirst[] = $line;
            if (count($linesNewestFirst) >= $maxLines) {
                break 2;
            }
        }
    }
    fclose($handle);

    $talkers = [];
    $seenCalls = [];
    foreach ($linesNewestFirst as $line) {
        // MQTT-Neustart: ältere aktive Zustände dürfen nicht übernommen werden.
        if (str_contains($line, 'DL3EL: MQTT Start')) {
            break;
        }

        if (!str_contains($line, '/server/statethr/1:') && !str_contains($line, 'server/statethr/1:')) {
            continue;
        }
        if (!preg_match('/\^(\d+)\^\s*$/', $line, $tsMatch)) {
            continue;
        }
        $eventTs = (int)$tsMatch[1];
        $jsonStart = strpos($line, '{');
        $jsonEnd = strrpos($line, '}');
        if ($jsonStart === false || $jsonEnd === false || $jsonEnd < $jsonStart) {
            continue;
        }
        $message = json_decode(substr($line, $jsonStart, $jsonEnd - $jsonStart + 1), true);
        if (!is_array($message)) {
            continue;
        }

        $call = strtoupper(trim((string)($message['call'] ?? '')));
        $talk = strtolower(trim((string)($message['talk'] ?? '')));
        if ($call === '' || isset($seenCalls[$call]) || !in_array($talk, ['start', 'stop'], true)) {
            continue;
        }

        // Das jüngste Ereignis dieses Rufzeichens ist maßgeblich – auch STOP.
        $seenCalls[$call] = true;
        if ($talk !== 'start') {
            continue;
        }

        // Kein zeitliches Timeout: Das jüngste START bleibt aktiv, bis
        // ein STOP oder ein MQTT-Neustartmarker den Zustand beendet.
        if ($eventTs <= 0) {
            continue;
        }

        $tg = normalizeTgValue((string)($message['tg'] ?? ''));
        $talkers[$call] = [
            'call' => $call,
            'tg' => $tg,
            'startMs' => $eventTs * 1000,
            'lastSeenMs' => $eventTs * 1000
        ];
    }

    return $talkers;
}

/**
 * Verarbeitet genau eine vollständige Zeile aus mqtt.data.
 * Rückgabe true, wenn sich der für die LiveMap relevante Zustand geändert hat.
 */
function processMqttDataLine(string $line, array &$state): bool
{
    if (!preg_match('/^\\[[^\\]]+\\]\\s+([^:]+):\\s*(.*)$/', rtrim($line, "\r\n"), $lineMatch)) {
        return false;
    }

    $topic = trim($lineMatch[1]);
    $payloadAndTimestamp = $lineMatch[2];

    // Format am Zeilenende: <Payload>^<Unix-Zeitstempel>^
    if (!preg_match('/^(.*)\\^(\\d+)\\^\\s*$/s', $payloadAndTimestamp, $payloadMatch)) {
        return false;
    }

    $payload = trim($payloadMatch[1]);
    $eventTimeMs = ((int)$payloadMatch[2]) * 1000;

    // Der Perl-Empfänger schreibt bei jedem Programmstart einen Marker
    // "DL3EL: MQTT Start". Damit werden Sprecher entfernt, deren STOP-
    // Meldung durch einen vorherigen Empfänger-Neustart fehlen könnte.
    if ($topic === 'DL3EL' && str_starts_with($payload, 'MQTT Start')) {
        if ($state['talkers'] !== []) {
            $state['talkers'] = [];
            return true;
        }
        return false;
    }

    // --------------------------------------------------------
    // Aktive Empfänger je Talkgroup
    // --------------------------------------------------------
    $activePrefix = '/server/state/activeTG/';
    if (str_starts_with($topic, $activePrefix)) {
        $tg = normalizeTgValue(substr($topic, strlen($activePrefix)));
        if ($tg === '') {
            return false;
        }

        if ($payload === '') {
            if (array_key_exists($tg, $state['activeTG'])) {
                unset($state['activeTG'][$tg]);
                return true;
            }
            return false;
        }

        $calls = json_decode($payload, true);
        if (!is_array($calls)) {
            return false;
        }

        $normalizedCalls = [];
        foreach ($calls as $call) {
            if (!is_string($call) && !is_numeric($call)) {
                continue;
            }
            $normalizedCall = strtoupper(trim((string)$call));
            if ($normalizedCall !== '') {
                $normalizedCalls[$normalizedCall] = true;
            }
        }
        $normalizedCalls = array_keys($normalizedCalls);
        sort($normalizedCalls, SORT_STRING);

        $oldCalls = $state['activeTG'][$tg] ?? null;
        if ($oldCalls === $normalizedCalls) {
            return false;
        }

        $state['activeTG'][$tg] = $normalizedCalls;
        return true;
    }

    // --------------------------------------------------------
    // Aktive Sprecher
    // --------------------------------------------------------
    if ($topic !== '/server/statethr/1' && $topic !== 'server/statethr/1') {
        return false;
    }

    $message = json_decode($payload, true);
    if (!is_array($message)) {
        return false;
    }

    $call = strtoupper(trim((string)($message['call'] ?? '')));
    $talk = strtolower(trim((string)($message['talk'] ?? '')));
    $tg = normalizeTgValue((string)($message['tg'] ?? ''));

    if ($call === '') {
        return false;
    }

    if ($talk === 'start') {
        $activityTimeMs = $eventTimeMs > 0 ? $eventTimeMs : (int)(microtime(true) * 1000);

        if (!isset($state['talkers'][$call])) {
            $state['talkers'][$call] = [
                'call' => $call,
                'tg' => $tg,
                'startMs' => $activityTimeMs,
                'lastSeenMs' => $activityTimeMs
            ];
            return true;
        }

        // Ein erneutes START aktualisiert die letzte Aktivität, aber nicht
        // den Beginn der laufenden Aussendung.
        $changed = false;
        if ($activityTimeMs > (int)($state['talkers'][$call]['lastSeenMs'] ?? 0)) {
            $state['talkers'][$call]['lastSeenMs'] = $activityTimeMs;
            $changed = true;
        }
        if ($tg !== '' && $state['talkers'][$call]['tg'] !== $tg) {
            $state['talkers'][$call]['tg'] = $tg;
            $changed = true;
        }
        return $changed;
    }

    if ($talk === 'stop' && isset($state['talkers'][$call])) {
        unset($state['talkers'][$call]);
        return true;
    }

    return false;
}

if (!is_file($dataFile) || !is_readable($dataFile)) {
    respond(500, [
        'success' => false,
        'error' => 'mqtt.data ist nicht vorhanden oder für www-data nicht lesbar.',
        'file' => $dataFile
    ]);
}

$stateKey = hash('sha256', $dataFile);
$stateFile = rtrim(sys_get_temp_dir(), DIRECTORY_SEPARATOR) . DIRECTORY_SEPARATOR . 'fm-livemap-mqtt-' . $stateKey . '.json';
$lockFile = $stateFile . '.lock';
$lockHandle = @fopen($lockFile, 'c');
if ($lockHandle === false || !flock($lockHandle, LOCK_EX)) {
    if (is_resource($lockHandle)) {
        fclose($lockHandle);
    }
    respond(500, ['success' => false, 'error' => 'LiveMap-Daten können momentan nicht gesperrt werden.']);
}

try {
    $handle = @fopen($dataFile, 'rb');
    if ($handle === false) {
        respond(500, ['success' => false, 'error' => 'mqtt.data konnte nicht geöffnet werden.']);
    }

    $fileStat = fstat($handle);
    if (!is_array($fileStat)) {
        fclose($handle);
        respond(500, ['success' => false, 'error' => 'Dateistatus von mqtt.data konnte nicht ermittelt werden.']);
    }

    $inode = (string)($fileStat['ino'] ?? '0');
    $fileSize = (int)($fileStat['size'] ?? 0);

    $state = null;
    if (is_file($stateFile)) {
        $cachedJson = @file_get_contents($stateFile);
        if (is_string($cachedJson)) {
            $cachedState = json_decode($cachedJson, true);
            if (
                is_array($cachedState) &&
                isset($cachedState['inode'], $cachedState['offset'], $cachedState['version']) &&
                isset($cachedState['talkers'], $cachedState['activeTG']) &&
                is_array($cachedState['talkers']) &&
                is_array($cachedState['activeTG']) &&
                (string)$cachedState['inode'] === $inode &&
                (int)$cachedState['offset'] <= $fileSize
            ) {
                $state = $cachedState;
            }
        }
    }

    // Ältere Cache-Versionen enthalten noch kein lastSeenMs. In diesem
    // Fall verwenden wir die bisherige Startzeit als letzten Aktivitätszeitpunkt.
    if ($state !== null) {
        foreach ($state['talkers'] as &$talker) {
            if (!isset($talker['lastSeenMs'])) {
                $talker['lastSeenMs'] = (int)($talker['startMs'] ?? 0);
            }
        }
        unset($talker);
    }

    // Erster Aufruf, beschädigter Cache, Dateirotation oder Truncate:
    // den Zustand einmalig aus der vorhandenen Datei rekonstruieren.
    $cacheWasRebuilt = ($state === null);
    if ($state === null) {
        $state = [
            'inode' => $inode,
            'offset' => 0,
            'version' => 0,
            'talkers' => [],
            'activeTG' => []
        ];
    }

    if (fseek($handle, (int)$state['offset']) !== 0) {
        fclose($handle);
        respond(500, ['success' => false, 'error' => 'Dateiposition in mqtt.data konnte nicht gesetzt werden.']);
    }

    $stateChanged = false;
    while (!feof($handle)) {
        $lineStart = ftell($handle);
        $line = fgets($handle);
        if ($line === false) {
            break;
        }

        // Nur vollständige Zeilen verarbeiten. Eine gerade geschriebene
        // letzte Zeile bleibt für den nächsten Abruf stehen.
        if ($line === '' || substr($line, -1) !== "\n") {
            fseek($handle, $lineStart);
            break;
        }

        if (processMqttDataLine($line, $state)) {
            $stateChanged = true;
        }
    }

    $newOffset = ftell($handle);
    fclose($handle);

    // Sprecherbestand unabhängig vom Cache wie in index_neu.php aus den
    // jüngsten Ereignissen rückwärts rekonstruieren. Das verhindert, dass
    // ein älteres START ein neueres STOP wieder als aktiv erscheinen lässt.
    $talkersBeforeScan = $state['talkers'];
    $state['talkers'] = readRecentTalkers($dataFile);
    if ($state['talkers'] !== $talkersBeforeScan) {
        $stateChanged = true;
    }

    $oldOffset = (int)$state['offset'];
    $state['inode'] = $inode;
    $state['offset'] = (int)$newOffset;
    if ($stateChanged) {
        $state['version'] = (int)$state['version'] + 1;
    }

    // Den kleinen Zwischenstand nur schreiben, wenn neue vollständige
    // Logzeilen verarbeitet wurden oder der Cache neu aufgebaut werden musste.
    if ($cacheWasRebuilt || (int)$newOffset !== $oldOffset || $stateChanged) {
        $encodedState = json_encode($state, JSON_UNESCAPED_SLASHES | JSON_UNESCAPED_UNICODE | JSON_INVALID_UTF8_SUBSTITUTE);
        if ($encodedState === false || @file_put_contents($stateFile, $encodedState, LOCK_EX) === false) {
            respond(500, ['success' => false, 'error' => 'Der LiveMap-Zwischenstand konnte nicht gespeichert werden.']);
        }
    }

    $talkers = array_values($state['talkers']);
    usort($talkers, static function (array $a, array $b): int {
        $timeCompare = ((int)$a['startMs']) <=> ((int)$b['startMs']);
        return $timeCompare !== 0 ? $timeCompare : strcmp((string)$a['call'], (string)$b['call']);
    });

    // Inhaltlicher Fingerabdruck: bleibt stabil, solange Sprecher und
    // Empfängerlisten gleich bleiben – auch wenn der Cache neu aufgebaut wird.
    $revisionState = [
        'talkers' => $talkers,
        'activeTG' => $state['activeTG']
    ];
    ksort($revisionState['activeTG'], SORT_STRING);
    $revisionJson = json_encode($revisionState, JSON_UNESCAPED_SLASHES | JSON_UNESCAPED_UNICODE | JSON_INVALID_UTF8_SUBSTITUTE);
    $revision = $inode . ':' . hash('sha256', $revisionJson === false ? '' : $revisionJson);

    respond(200, [
        'success' => true,
        'revision' => $revision,
        'talkers' => $talkers,
        'activeTG' => (object)$state['activeTG'],
        'offset' => (int)$state['offset']
    ]);
} finally {
    flock($lockHandle, LOCK_UN);
    fclose($lockHandle);
}
