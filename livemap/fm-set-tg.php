<?php
// Version V20260914

header('Content-Type: application/json; charset=utf-8');
$tg = '';
if (isset($_POST['dtmfsvx'])) {
    $tg = preg_replace(
        '/[^0-9]/',
        '',
        $_POST['dtmfsvx']
    );
}

if ($tg === '') {
    echo json_encode([
        'ok' => false,
        'message' => 'Keine gültige TG'
    ]);
    exit;
}

// SVXLink auf gewünschte TG wechseln.
exec(
    "echo '*91{$tg}#' > /tmp/dtmf_svx"
);
echo json_encode([
    'ok' => true,
    'tg' => $tg
]);

?>