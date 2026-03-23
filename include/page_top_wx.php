<?php
$wx_file = DL3EL . '/aprs-is.wx';
if (defined ('SVXNAME')) {
    $wx_file = '/var/www/html/dl3el/aprs-is.wx';
}    

$best_station = null;
$wx_stn_found = 0;
if (file_exists($wx_file)) {
    $lines = array_reverse(file($wx_file, FILE_IGNORE_NEW_LINES | FILE_SKIP_EMPTY_LINES));
    $stations = [];

    foreach ($lines as $line) {
        // Extrahiere Call und den Rest
        if (preg_match('/\]\^(.*?)\^(.*)$/', $line, $matches)) {
            $call = $matches[1];
            $content = $matches[2];
            $time = substr($line,12,5);
            // Falls wir diese Station in diesem Durchlauf (rückwärts = aktuellster Stand) noch nicht haben
            if (!isset($stations[$call])) {
                $parts = explode('^', $content);
                $data = ['call' => $call, 'score' => 0, 'dist' => 9999, 'display' => ""];
                $info = [];

                foreach ($parts as $p) {
                    $p = trim($p);
                    if (empty($p) || $p == "undef°") continue;

                    // Score berechnen (Qualität) und Distanz extrahieren
                    if (strpos($p, 'km') !== false) {
                        $data['dist'] = (float)$p;
                    } else {
                        $data['score']++; // Jeder Wetter-Datenpunkt erhöht die Qualität
                        $info[] = $p;
                    }
                }
                $data['display'] = implode(' | ', $info);
                $data['time'] = $time;
                $stations[$call] = $data;
                if ((defined ('DL3EL_WX_STN')) && (DL3EL_WX_STN === $call)) {
                    if ((defined ('debug')) && (debug > 0)) echo "WX_STN:" . DL3EL_WX_STN . " " . $call . "<br>";;
                    $wx_stn_found = 1;
                    break;
                }

            }
        }
    }
    if (!$wx_stn_found) {
        // Sortierung: Erst nach höchstem Score (viele Daten), dann nach kleinster Distanz
        usort($stations, function($a, $b) {
            if ($b['score'] === $a['score']) {
                return $a['dist'] <=> $b['dist'];
            }
            return $b['score'] <=> $a['score'];
        });

        if (count($stations) > 0) {
            $best = $stations[0];
            $wx_string = "<span style='color:#FFD700; font-weight:bold;'>WX ({$best['call']}):</span> {$best['display']} ({$best['dist']}km) " . $best['time'] . "h)";
        }
    } else {
        $best = $stations[$call];
        $wx_string = "<span style='color:#FFD700; font-weight:bold;'>WX ({$best['call']}):</span> {$best['display']} ({$best['dist']}km, " . $best['time'] . "h)";
    }        
} else {
    if ((defined ('debug')) && (debug > 0)) echo "no WX available";
}    
?>

<center>
    <?php if (isset($wx_string)): ?>
        <p style="margin-top:0px;margin-bottom:5px; font-size: 13px; color: white; background: rgba(0,0,0,0.2); padding: 5px; border-radius: 5px; display: inline-block;">
            <?php echo $wx_string; 
            ?>

<?php
/*
echo "<span style='display:inline-block; transform:rotate(45deg);'>&uarr;</span> (Nord-Ost)";
echo "<span style='display:inline-block; transform:rotate(180deg);'>&uarr;</span> (Süden)";
*/
?>
        </p>
    <?php endif; ?>
</center>
