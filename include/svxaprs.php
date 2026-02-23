<?php

if (session_status() === PHP_SESSION_NONE) {
    session_start();
}

include_once "settings.php";
echo '<fieldset style = " width:550px;background-color:#f1f1f1;margin-top:15px;margin-left:10px;margin-right:10px;font-size:14px; border:none;">';
//echo '<span><b>APRS Monitor</b><br></span>';
echo '<span><b>APRS Monitor</b></span>'; // <br> ist hier nicht mehr nötig


// last 1000 Spots
//$url="https://dxc.jo30.de/dxcache/spots";
    $aprs_datei = DL3EL . "/aprs-is.txt";
    $msg_datei = DL3EL . "/aprs-is.msg";

    if (file_exists($msg_datei)) {
	$result  = file($msg_datei);
	$aprs_msg = array_reverse($result);
	$aprs_msg_avail = 1;
    } else {
	$aprs_msg_avail = 0;
    }

    date_default_timezone_set('Europe/Berlin');
    if (file_exists($msg_datei  . ".neu")) {
	unlink($msg_datei . ".neu");
    }

    if ($aprs_msg_avail)  {
//	echo '<BR>';
//	echo '<table style = "margin-bottom:0px;border:0; border-collapse:collapse; cellspacing:0; background-color:#f1f1f1;">';
//	echo '<table style = "margin-bottom:0px;border:0; border-collapse:collapse; cellspacing:0; cellpadding:0; background-color:#f1f1f1;"><tr style = "border:none;background-color:#f1f1f1;">';
//	echo '<table style="margin-top: 15px;margin-left:10px;margin-right:20px; border-collapse: collapse; background-color: #f1f1f1; ...">';
	echo '<table style=" border-collapse: collapse; background-color: #f1f1f1; ...">';
	echo '<thead>';
	echo '<tr style = "border:none;background-color:#f1f1f1;">';
	echo "<th><b>From</b></th><th><b>Type</b></th><th><b>Messages (" . date("H:i:s") .")</b></th><th><b>Zeit</b></th></tr>";
	echo '</thead>';

	$nn = 0;
	while ($nn < 10) { 
	    $line = explode("^", $aprs_msg[$nn]);
	    $msg = $line[2];
	    $ll = 40;
	    if (strlen($msg) > $ll) {
		$msg_array = explode(" ",$msg);
		$msgn = "";
		foreach ($msg_array as $msg_word) {
		    if (strlen($msg_word) > $ll) {
			$msg_word = "";
		    }
		    $msgn = $msgn . $msg_word . " ";
		    if (strlen($msgn) > $ll) {
			$msgn = $msgn . "<br>";
			$ll = $ll + 40;
		    }
		}
		$msg = $msgn;
	    } 
	    $llm = strlen($aprs_msg[$nn]);
	    if ($llm) {
//		echo "<tr><td>" . $line[0] . "</td><td>" . $line[1] . "</td><td>" . $msg . "</td><td>". $line[4] . "</td><td>" .  $line[3] . "</td></tr>";
		echo "<tr><td>" . $line[0] . "</td><td>" . $line[1] . "</td><td>" . $msg . "</td><td>". $line[4] . "</td></tr>";
	    }
	    ++$nn;
    }
//	echo "</td></Tr>";
    echo '</table>';
    echo '<br><br>';
    }    
    echo '<table style = "margin-bottom:0px;border:0; border-collapse:collapse; cellspacing:0; cellpadding:0; background-color:#f1f1f1;">';
    echo '<tr style = "border:none;background-color:#f1f1f1;">';
//    echo "<tr><td><b>Call</b></td><td><b>Locator</b></td><td><b>APRS Info letzter Update (" . date("H:i:s") .")</b></td><td><b></b></td><td><b>Zeit</b></td></tr>";
    echo "<tr><th><b>Call</b></th><th><b>Locator</b></th><th><b>APRS Info letzter Update (" . date("H:i:s") .")</b></th><th><b></b></th><th><b>Zeit</b></th></tr>";
/*
    while ($nn < 100) { 
	$line_neu[$nn] = $aprs_data[$nn]);
	++nn;
    }
    $parsedData = parseLinesToUniqueArray($line_neu);
*/
$file = $aprs_datei;
    if (file_exists($aprs_datei)) {
//	$result  = file($aprs_datei);
//	$aprs_data = array_reverse($result);
//	sort($aprs_data);
// 1. Daten holen (Eindeutigkeit basierend auf dem ersten Feld, z.B. DL1FX)
	$uniqueData = readLast100Unique($file);
	$aprs_data = printLogData($uniqueData);
	$nn = 0;
    } else {
	$nn = 30;
    }


// 2. Daten ausgeben
    while ($nn < 22) { 
	$line = explode("^", $aprs_data[$nn]);
	$msg = $line[3];
	    $ll = 35;
	    $llm = strlen($msg);
	    if (strlen($msg) > $ll) {
		$msg_array = explode(" ",$msg);
		$msgn = "";
		foreach ($msg_array as $msg_word) {
		    if (strlen($msg_word) > $ll) {
			$msg_word = "";
		    }
		    $msgn = $msgn . $msg_word . " ";
		    if (strlen($msgn) > $ll) {
			$msgn = $msgn . "<br>";
			$ll = $ll + 40;
		    }
		}
		$msg = $msgn;
		if ((defined ('debug')) && (debug > 0)) $msg = $msg . "<br>[" . $msgn . "]," . $llm;
	    } 
	echo "<tr><td>" . $line[1] . "</td><td>" . $line[2] . "</td><td>" . $msg . "</td><td>". $line[5] . "</td><td>" .  $line[4] . "</td></tr>";
	++$nn;
    }
    echo "</td></Tr>";
echo '<fieldset>';

    echo '</table>';

?>
</center>
<!-- </div><br></div>-->
<!-- </fieldset> -->


<?php

// Gemini
/**
 * Liest die letzten 100 Zeilen einer Datei rückwärts ein und filtert Dubletten.
 * * @param string $filename Pfad zur Datei
 * @return array Eindeutige Zeilen mit dem ersten Feld als Key
 */
function readLast100Unique($filename) {
    if (!file_exists($filename)) {
        return [];
    }
    // Die gesamte Datei in ein Array laden
    $lines = file($filename, FILE_IGNORE_NEW_LINES | FILE_SKIP_EMPTY_LINES);
    
    $anzahl_zeilen = count($lines); 
    if ($anzahl_zeilen > 100) {
	$anzahl_zeilen = 100;
    }
    $anzahl_zeilen = $anzahl_zeilen *-1;
    // Die letzten 100 Zeilen extrahieren
//    $last100 = array_slice($lines, -100);
    $last100 = array_slice($lines, $anzahl_zeilen);
    
    // Reihenfolge umkehren, damit wir mit der aktuellsten Zeile beginnen
    $last100 = array_reverse($last100);
    // nach Entfernung aufsteigend sortieren
    sort($last100);
    
    $result = [];
    
    foreach ($last100 as $line) {
        // Explode am Trennzeichen ^
        $fields = explode('^', $line);
// Key ist Call
        $key = $fields[1];
        
        // Wenn der Key noch nicht im Resultat ist, hinzufügen
        // Da wir rückwärts lesen, ignorieren wir automatisch ältere Einträge
        if (!isset($result[$key])) {
            $result[$key] = $line;
        }
    }
    
    return $result;
}

/**
 * Gibt das Array zeilenweise aus.
 * * @param array $data Das gefilterte Array
 */
function printLogData($data) {
    $nn = 0;
    foreach ($data as $line) {
//        echo $line . PHP_EOL;
//        echo $line . "<br>";
//        echo $line . "^\n";
	$aprs_data_neu[$nn] = $line . "^\n";
	++$nn;
    }
    return $aprs_data_neu;
}

?>
