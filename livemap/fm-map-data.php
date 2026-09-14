<?php
// Version V20260914

header('Content-Type: application/json; charset=utf-8');
$urls = array(
    'https://dashboard.fm-funknetz.de/reflector1.json',
    'https://dashboard.fm-funknetz.de/reflector2.json'
);
$nodes = array();
foreach ($urls as $url) {
    $context = stream_context_create(array(
        'http' => array(
            'method'  => 'GET',
            'timeout' => 20,
            'header'  => "User-Agent: FM-LiveMap/1.0\r\n"
        ),
        'ssl' => array(
            'verify_peer'      => true,
            'verify_peer_name' => true
        )
    ));
    $data = @file_get_contents($url, false, $context);
    if ($data === false) {
        continue;
    }
    $json = json_decode($data, true);
    if (!is_array($json)) {
        continue;
    }
    if (!isset($json['nodes']) || !is_array($json['nodes'])) {
        continue;
    }
    foreach ($json['nodes'] as $call => $node) {
        $call = trim((string)$call);
        if ($call === '') {
            continue;
        }
        if (!is_array($node)) {
            continue;
        }
        $node['_call'] = $call;

        // Wenn ein Node in beiden Reflectoren vorkommt,
        // wird der zweite Datensatz übernommen.
        $nodes[$call] = $node;
    }
}
echo json_encode(
    array_values($nodes),
    JSON_UNESCAPED_UNICODE | JSON_UNESCAPED_SLASHES
);
?>