<?php
$wx_file = DL3EL . '/aprs-is.wx';
if (defined ('SVXNAME')) {
    $wx_file = '/var/www/html/dl3el/aprs-is.wx';
}    

$best_station = null;
$wx_stn_found = 0;
$now = time(); // Aktueller Zeitpunkt

if (file_exists($wx_file)) {
    $lines = array_reverse(file($wx_file, FILE_IGNORE_NEW_LINES | FILE_SKIP_EMPTY_LINES));
    $stations = [];

    foreach ($lines as $line) {
        // 1. Zeitstempel der Zeile extrahieren (Format: 25.03.2026 00:13:35)
        // Wir nehmen den gesamten Inhalt der ersten Klammer [ ]
        if (preg_match('/^\[(.*?)\]/', $line, $timeMatches)) {
            $lineTime = strtotime($timeMatches[1]);
        
            // 2. Prüfen: Wenn die Zeile älter als 20 Min (1200 Sek) ist -> ignorieren
            if ($now - $lineTime > 1200) {
                continue; 
            }
        }
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

                $ztime = 0;
                foreach ($parts as $p) {
                    $p = trim($p);
                    if (empty($p) || $p == "undef°") continue;

                    // Score berechnen (Qualität) und Distanz extrahieren
                    if (strpos($p, 'km') !== false) {
                        $data['dist'] = (float)$p;
                    } elseif (strpos($p, 'z') !== false) {
                        $ztime = $p;
                    } elseif (strpos($p, 'span') !== false) {
                        # do not count wind
                        $info[] = $p;
                    } else {
                        $data['score']++; // Jeder Wetter-Datenpunkt erhöht die Qualität
                        $info[] = $p;
                    }
                }
                if ((defined ('debug')) && (debug > 0)) echo "WX found: $call [" . $content . "], Score:" . $data['score'] . " <br>";;
                $data['display'] = implode(' | ', $info);
                $data['time'] = $time;
                $data['ztime'] = $ztime;
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
/*alt
        // Sortierung: Erst nach höchstem Score (viele Daten), dann nach kleinster Distanz
        usort($stations, function($a, $b) {
            if ($b['score'] === $a['score']) {
                return $a['dist'] <=> $b['dist'];
            }
            return $b['score'] <=> $a['score'];
        });
*/
        // Sortierung: Innerhalb von 10km nach Score, danach nur nach Entfernung
usort($stations, function($a, $b) {
    $radius = 10; // 10 km Grenze

    // Fall 1: Beide Stationen sind innerhalb von 10km
    // -> Hier gewinnt der höhere Score
    if ($a['dist'] <= $radius && $b['dist'] <= $radius) {
        if ($b['score'] === $a['score']) {
            return $a['dist'] <=> $b['dist']; // Bei gleichem Score: Nähere gewinnt
        }
        return $b['score'] <=> $a['score'];
    }

    // Fall 2: Eine Station ist nah (<=10km), die andere weit weg (>10km)
    // -> Die nähere Station gewinnt immer (unabhängig vom Score)
    if ($a['dist'] <= $radius && $b['dist'] > $radius) return -1;
    if ($a['dist'] > $radius && $b['dist'] <= $radius) return 1;

    // Fall 3: Beide sind außerhalb von 10km
    // -> Hier gewinnt die absolut kleinere Entfernung
    return $a['dist'] <=> $b['dist'];
});
        if (count($stations) > 0) {
            $best = $stations[0];
            $ztime = "";
            if ((defined ('debug')) && (debug > 0)) echo "# WX found:" . count($stations) . ", selected:{$best['call']}, with Score:{$best['score']}<br>";
            if (isset($best['ztime']) && !empty($best['ztime'])) {
                $ztime = ", " . $best['ztime'];
            }    
            $wx_string = "<span style='color:#FFD700; font-weight:bold;'>WX ({$best['call']}{$ztime}):</span> {$best['display']} ({$best['dist']}km, " . $best['time'] . "h)";
        } else {
          if ((defined ('debug')) && (debug > 0)) echo "no WX available";
        }   
    } else {
        $best = $stations[$call];
        $ztime = "";
        if (isset($best['ztime']) && !empty($best['ztime'])) {
            $ztime = ", " . $best['ztime'];
        }    
        $wx_string = "<span style='color:#FFD700; font-weight:bold;'>WX ({$best['call']}{$ztime}):</span> {$best['display']} ({$best['dist']}km, " . $best['time'] . "h)";
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
