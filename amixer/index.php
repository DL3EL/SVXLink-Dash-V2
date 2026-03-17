<?php
if (session_status() === PHP_SESSION_NONE) {
    session_start();
}

include "../include/settings.php";

///* running in ptt.html -> move to include functions.php
// Function to execute amixer command and retrieve output
function execute_amixer($command) {
    $output = [];
    if ((defined ('debug')) && (debug > 1)) echo "mixer cmd: $command, ANSWER:<br>";
    exec($command, $output);
    if ((defined ('debug')) && (debug > 2)) print_r($output);
    if ((defined ('debug')) && (debug > 2)) echo "<br>";
    return $output;
}
//*/
///* running in ptt.html
// Function to parse current values from amixer output
function parse_amixer_value($output) {
    $nn = 0;
    foreach ($output as $line) {
        if ((defined ('debug')) && (debug > 1)) echo "[$nn] parse_amixer, raw line [$line]<br>";
// for some reason on the shari, the output is ie 37,37, we need to delete the second value
        if (strpos($line, ": values=") !== false) {
            $value = trim(str_replace(": values=", "", $line));
            $poscomma = stripos($value,",");
            if ($poscomma > 0) {
                $value = substr($value,0,strlen($value)-$poscomma-1);
            }
            if ((defined ('debug')) && (debug > 1)) echo "[$nn] ende parse_amixer, values: $value [$line] (kommas was $poscomma)<br>------<br>";
            return $value;
        }
        ++$nn;
    }
    return null;
}
//*/
// Function to calculate percentage relative to maximum permitted value and return as whole number
function calculate_percentage($current_value, $max_value) {
    if (!is_numeric($current_value)) { $current_value = 0; }    
    if (!is_numeric($max_value)) { $max_value = 1; }    
    $perc = round(($current_value / $max_value) * 100);
    return round(($current_value / $max_value) * 100);
}


// Retrieve and parse current values using amixer cget
/*
function get_current_amixer_values() {
    if (defined('DL3EL_SC_STRING')) {
        $sc_port_cmp = DL3EL_SC_STRING;
    } else{    
        $sc_port_cmp = "Audio Device";
    }
//    $sc = 'aplay -l | grep "Audio Device"';
    $sc = 'aplay -l | grep "' . $sc_port_cmp . '"';

    $sc = substr(shell_exec($sc),5,1);
    if ((defined ('debug')) && (debug > 0)) echo "Karte gefunden: $sc<br>";
    $headphone_output = execute_amixer("sudo amixer -c" . $sc . " cget numid=6");
    $mic_output = execute_amixer("sudo amixer -c" . $sc . " cget numid=4");
    $capture_output = execute_amixer("sudo amixer -c" . $sc . " cget numid=8");
    $autogain_output = execute_amixer("sudo amixer -c" . $sc . " cget numid=9");
    $current_values = [
        'headphone' => parse_amixer_value($headphone_output),
        'mic' => parse_amixer_value($mic_output),
        'capture' => parse_amixer_value($capture_output),
        'autogain' => parse_amixer_value($autogain_output)
    ];

    if ($autogain_output) {
        foreach ($autogain_output as $line) {
            if (strpos($line, ": values=") !== false) {
                $current_values['autogain'] = trim(str_replace(": values=", "", $line));
                break;
            }
        }
       if ((defined ('debug')) && (debug > 0)) echo "Ende autogain values: " . $current_values['autogain'] . "<br>";
    }
    if ((defined ('debug')) && (debug > 1)) echo "CV: " . $current_values['headphone'] . "<br>";
    return $current_values;
}
*/
function get_current_amixer_values_tx($sc,$svxRadio) {
    if ((defined ('debug')) && (debug > 1)) echo "get_current_amixer_values_tx, für Karte: [$sc]<br>";
    $mic_output = execute_amixer("sudo amixer -c" . $sc . " cget numid=4");
    if ($svxRadio === "Elenata") {
        $headphone_output = execute_amixer("sudo amixer -c" . $sc . " cget numid=10");
        $capture_output = execute_amixer("sudo amixer -c" . $sc . " cget numid=2");
    } else {    
        $headphone_output = execute_amixer("sudo amixer -c" . $sc . " cget numid=6");
        $capture_output = execute_amixer("sudo amixer -c" . $sc . " cget numid=8");
    }    
    $autogain_output = execute_amixer("sudo amixer -c" . $sc . " cget numid=9 2>/dev/null");
    $current_values = [
        'headphone' => parse_amixer_value($headphone_output),
        'mic' => parse_amixer_value($mic_output),
        'capture' => parse_amixer_value($capture_output),
        'autogain' => parse_amixer_value($autogain_output)
    ];

    if ($autogain_output) {
        foreach ($autogain_output as $line) {
            if (strpos($line, ": values=") !== false) {
                $current_values['autogain'] = trim(str_replace(": values=", "", $line));
                break;
            }
        }
       if ((defined ('debug')) && (debug > 1)) echo "autogain values: " . $current_values['autogain'] . "<br>";
    } else {
       if ((defined ('debug')) && (debug > 1)) echo "no autogain values available<br>";
    }
    if ((defined ('debug')) && (debug > 1)) echo "<b>get_current_amixer_values_tx (Headphone, $sc): " . $current_values['headphone'] . "</b><br>";
    return $current_values;
}
function get_current_amixer_values_rx($sc) {
//    $sc = $sc_rx;
    if ((defined ('debug')) && (debug > 1)) echo "get_current_amixer_values_rx, für Karte:  $sc<br>";
    $mic_output_rx = execute_amixer("sudo amixer -c" . $sc . " cget numid=6");
    $current_values_rx = [
        'mic' => parse_amixer_value($mic_output_rx)
    ];

//    if ((defined ('debug')) && (debug > 1)) echo "CV: " . $current_values['headphone'] . "<br>";
    if ((defined ('debug')) && (debug > 1)) echo "<b>get_current_amixer_values_rx (Mike, $sc): " . $current_values_rx['mic'] . "</b><br>";
    return $current_values_rx;
}

/**
 * Extrahiert min und max Werte aus der amixer Ausgabe
 * @param string $input Die Roh-Ausgabe von shell_exec
 * @return array|null Array mit ['min', 'max'] oder null bei Fehler
 */
function getAmixerRange($input) {
    // Sucht nach 'min=' gefolgt von Zahlen und 'max=' gefolgt von Zahlen
    // \d+ steht für eine oder mehrere Ziffern
    if (preg_match('/min=(\d+),max=(\d+)/', $input, $matches)) {
        return [
            'min' => (int)$matches[1],
            'max' => (int)$matches[2]
        ];
    }
    return null;
}

// PHP Code starts here
    $svxRadio = "";
    $sc_port_cmp = "Audio Device";
    if (defined('DL3EL_RADIO')) {
        $svxRadio = DL3EL_RADIO;
    }
    if (defined('DL3EL_SC_STRING')) {
        $sc_port_cmp = DL3EL_SC_STRING;
    }

/// SC & MIke Info from svxlink.conf & system, skip var from config.php
    $svxConfigFile = SVXCONFPATH."/".SVXCONFIG;
    if (fopen($svxConfigFile,'r')) {
        $svxconfig = parse_ini_file($svxConfigFile,true,INI_SCANNER_RAW); 
        $sc_port_rxname = $svxconfig['SimplexLogic']['RX']; 
        $sc_port_rx = $svxconfig[$sc_port_rxname]['AUDIO_DEV']; 
        $sc_port_txname = $svxconfig['SimplexLogic']['TX']; 
// hier noch auf Multi Tx erweitern
        $sc_port_tx = $svxconfig["Tx1"]['AUDIO_DEV']; 
        $card_start = strpos($sc_port_tx, 'CARD=')+5;
        if ($card_start !== false) {
            $card_end = strpos($sc_port_tx, ',DEV');
            $card_string = substr($sc_port_tx, $card_start, $card_end - $card_start);
            $sc_tx_cmp = $card_string . " \[";
            $sc = 'aplay -l | grep "' . $sc_tx_cmp . '"';
            $sc_tx = substr(shell_exec($sc),5,1);
            if ((defined ('debug')) && (debug > 1)) echo "TX (Lautsprecher): $sc_tx_cmp, Card:[$sc_tx] ($sc) <br>SVXLink: [" . $sc_port_tx . "]<br>";
        }
        $card_start = strpos($sc_port_rx, 'CARD=')+5;
        if ($card_start !== false) {
            $card_end = strpos($sc_port_rx, ',DEV');
            $card_string = substr($sc_port_rx, $card_start, $card_end - $card_start);
            $sc_rx_name = $card_string;
            $sc_rx_cmp = $card_string . " \[";
            $sc = 'aplay -l | grep "' . $sc_rx_cmp . '"';
            $sc_rx = substr(shell_exec($sc),5,1);
            if ((defined ('debug')) && (debug > 1)) echo "RX (Mikrofon): $sc_rx_cmp, Card:[$sc_rx] ($sc) <br>SVXLink: [" . $sc_port_rx . "]<br>";
        }
        $spkrismike = "";
        if ($sc_port_rx === $sc_port_tx) {
            $spkrismike = "/Mikrofon";
            if ((defined ('debug')) && (debug > 1)) echo "TX: [$sc_port_tx] == RX:[$sc_port_rx]<br>";
        } else {   
//                if ($sc_port_rx !== $sc_port_tx) {
            if ((defined ('debug')) && (debug > 1)) echo "TX: [$sc_port_tx] != RX:[$sc_port_rx]<br>";
            $sc_mike_linux = 'aplay -l | grep "' . $sc_rx_cmp . '"';
            $sc_mike_linux = shell_exec($sc_mike_linux);
            $max_values_qx18 = 147;
            if ((defined ('debug')) && (debug > 1)) echo "Found extra Mike [$sc_mike_linux]<br>";
        }    
    }    
// $sc_rx_cmp Mikrofon     svxlink rx, Index: $sc_rx
// $sc_tx_cmp Lautsprecher svxlink tx, Index: $sc_tx   
if ((defined ('debug')) && (debug > 0)) echo "<b>Radio $svxRadio:<br>RX (Mikrofon): $sc_rx_cmp [Index: $sc_rx sc_rx]<br>";
if ((defined ('debug')) && (debug > 0)) echo "TX (Lautsprecher): $sc_tx_cmp [Index: $sc_tx sc_tx] $sc_port_cmp: </b><br>";
///
// Maximum permitted values based on numid
// Todo: erlaubte Werte ermitteln
/*
 show all numids: sudo amixer -c1 controls
 show all numids: sudo amixer -c1 contents
*/
    if ($svxRadio === "Elenata") {
/*
        numid=10,iface=MIXER,name='Lineout Playback Volume'
        ; type=INTEGER,access=rw---R--,values=2,min=0,max=31,step=0
        : values=22,22
        | dBscale-min=-15.50dB,step=0.50dB,mute=0
        numid=2,iface=MIXER,name='Capture Volume'
        ; type=INTEGER,access=rw------,values=2,min=0,max=15,step=0
        : values=12,12
 
*/      
        $max_values = [
            'headphone' => 31, // numid=10
            'mic' => 32, // numid=4
            'capture' => 15 // numid=2
        ];
if ((defined ('debug')) && (debug > 0)) {
    echo "<br>MaxValues alt:<br>";
    print_r($max_values);
//    echo "<br>Gefundener Minimalwert LineOut 10: " . $range_lo['min'] . "<br>";
//    echo "Gefundener Maximalwert LineOut 10: " . $range_lo['max'] . "<br>";
//    echo "Gefundener Minimalwert Mike 2: " . $range_mi['min'] . "<br>";
//    echo "Gefundener Maximalwert Mike 2: " . $range_mi['max'] . "<br>";
}
    $output_lo = shell_exec("amixer -c" . $sc_tx ." cget numid=10");
    $range_lo = getAmixerRange($output_lo);
    $output_mi = shell_exec("amixer -c" . $sc_tx ." cget numid=4");
    $range_mi = getAmixerRange($output_mi);
    $output_cp = shell_exec("amixer -c" . $sc_tx ." cget numid=2");
    $range_cp = getAmixerRange($output_cp);
        $max_values = [
            'headphone' => $range_lo['max'], // numid=10
            'mic' => $range_mi['max'], // numid=4
            'capture' => $range_cp['max'] // numid=2
        ];
if ((defined ('debug')) && (debug > 0)) {
    echo "<br>MaxValues neu:<br>";
    print_r($max_values);
}
    } else {
        /*
        numid=4,iface=MIXER,name='Mic Playback Volume'
        ; type=INTEGER,access=rw---R--,values=1,min=0,max=31,step=0
        : values=24
        | dBminmax-min=-23.00dB,max=8.00dB
        numid=6,iface=MIXER,name='Speaker Playback Volume'
        ; type=INTEGER,access=rw---R--,values=2,min=0,max=37,step=0
        : values=23,23
        | dBminmax-min=-37.00dB,max=0.00dB
        numid=8,iface=MIXER,name='Mic Capture Volume'
        ; type=INTEGER,access=rw---R--,values=1,min=0,max=35,step=0
        : values=12
        | dBminmax-min=-12.00dB,max=23.00dB
        */
        $max_values = [
//        'headphone' => 151, // numid=6
        'headphone' => 45, // numid=6
        'mic' => 37, // numid=4
        'capture' => 35 // numid=8
        ];
 if ((defined ('debug')) && (debug > 0)) {
   echo "<br>MaxValues alt:<br>";
    print_r($max_values);
 }   
    $output_hp = shell_exec("amixer -c" . $sc_tx ." cget numid=6");
    $range_hp = getAmixerRange($output_hp);
    if ($sc_port_rx === $sc_port_tx) {
        $output_mi = shell_exec("amixer -c" . $sc_tx ." cget numid=4");
    } else {
        $output_mi = shell_exec("amixer -c" . $sc_rx ." cget numid=6");
    }        
    $range_mi = getAmixerRange($output_mi);
    $output_cp = shell_exec("amixer -c" . $sc_tx ." cget numid=8");
    $range_cp = getAmixerRange($output_cp);

        $max_values = [
            'headphone' => $range_hp['max'], // numid=10
            'mic' => $range_mi['max'], // numid=4
            'capture' => $range_cp['max'] // numid=2
        ];
 if ((defined ('debug')) && (debug > 0)) {
   echo "<br>MaxValues neu:<br>";
    print_r($max_values);
    echo "<br>Gefundener Minimalwert HeadPhone 6: " . $range_hp['min'] . "<br>";
    echo "Gefundener Maximalwert HeadPhone 6: " . $range_hp['max'] . "<br>";
    echo "Gefundener Minimalwert Mike 4: " . $range_mi['min'] . "<br>";
    echo "Gefundener Maximalwert Mike 4: " . $range_mi['max'] . "<br>";
    echo "Gefundener Minimalwert Capture 8: " . $range_cp['min'] . "<br>";
    echo "Gefundener Maximalwert Capture 8: " . $range_cp['max'] . "<br>";
}
}
// Get current values from amixer
    $current_values = get_current_amixer_values_tx($sc_tx,$svxRadio);
    if ($sc_tx !== $sc_rx) {
        $current_values_rx = get_current_amixer_values_rx($sc_rx);
    }    
    $current_autogain = $current_values['autogain'];
if ((defined ('debug')) && (debug > 0)) {
    echo "<b>Spkr: ";
    print_r($current_values);
    if ($sc_tx !== $sc_rx) {
        echo "<br>Mike: ";
        print_r($current_values_rx);
    }    
    echo "</b><br>";
}    
?>

<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <link href="../css/css.php" type="text/css" rel="stylesheet" />
 <script type="text/javascript">
        function reloadPage() {
            window.location.href = window.location.pathname + "?reloaded=true";
        }
    </script>
<script src="https://code.jquery.com/jquery-3.6.0.min.js"></script>
</head>

<body style="background-color: #e1e1e1;font: 11pt arial, sans-serif;">
    <center>
    <h1 id="svxlink" style="color:#00aee8;font: 18pt arial, sans-serif;font-weight:bold; text-shadow: 0.25px 0.25px gray;">Audio Configurator</h1>
    <h3 style="color:#00aee8;font: 12pt arial, sans-serif;font-weight:bold; text-shadow: 0.25px 0.25px gray;">AMixer settings</h3>
        <?php 
        ?>
    <!-- HTML Form to adjust ALSA settings -->
    <form method="post" action="<?php echo htmlspecialchars($_SERVER["PHP_SELF"]); ?>" onsubmit="reloadPage()">
        <h3 style="color:#00aee8;font: 12pt arial, sans-serif;font-weight:bold; text-shadow: 0.25px 0.25px gray;">Lautsprecher - TX Level</h3>
            <label for="headphone">Set for 65 (0-100):</label>
            <input type="number" id="headphone" name="headphone" min="0" max="100" value="<?php echo htmlspecialchars(calculate_percentage($current_values['headphone'], $max_values['headphone'])); ?>" required>%
            <br>
        <?php 
        if ($sc_tx !== $sc_rx) {
            echo '<h3 style="color:#00aee8;font: 12pt arial, sans-serif;font-weight:bold; text-shadow: 0.25px 0.25px gray;">ext. Mikrofon - ' . $sc_rx_name . '</h3>';
            echo '<label for="mic">(0-100): Set to 50</label>';
            echo '<input type="number" id="mic" name="mic" min="0" max="100" value="';
//            echo htmlspecialchars(calculate_percentage($current_values_rx['mic'], $max_values['mic']));
            echo htmlspecialchars(calculate_percentage($current_values_rx['mic'], $max_values_qx18));
            echo '" required>%';
            echo '<br>';
        } else {    
            echo '<h3 style="color:#00aee8;font: 12pt arial, sans-serif;font-weight:bold; text-shadow: 0.25px 0.25px gray;">Audio Capture - RX Levels</h3>';
            echo '<label for="capture">(0-100) Set for 25:</label>';
            echo '<input type="number" id="capture" name="capture" min="0" max="200" value="';
            echo htmlspecialchars(calculate_percentage($current_values['capture'], $max_values['capture'])); 
            echo '" required>%';
            echo '<br>';
            echo '<h3 style="color:#00aee8;font: 12pt arial, sans-serif;font-weight:bold; text-shadow: 0.25px 0.25px gray;">Auto Gain</h3>';
            echo '<label for="autogain">Set to OFF for optimum control</label>';
            echo '<select id="autogain" name="autogain" required>%';
            echo '<option value="0"'; 
            if ($current_autogain === '0' || $current_autogain === 'off')  echo 'selected';
            echo '>Off</option>';
            echo '<option value="1"';
            if ($current_autogain === '1' || $current_autogain === 'on')  echo 'selected';
            echo '>On</option>';
        }
        ?>
        </select>

        <?php 
                echo "<br><br>Lautsprecher$spkrismike Konfiguration: <b>$sc_port_tx</b>, SC-Index:$sc_tx<br>";
                if ($sc_port_rx !== $sc_port_tx) {
                    echo "ext. Mikrofon Konfiguration: <b>$sc_port_rx</b>, SC-Index:$sc_rx<br>";
                }
        ?>
        <br>
        <button type="submit">Apply Settings</button>
        <?php 
// Soundcard?
                if (substr($svxRadio,0,3) === "SC ") {
                    echo '<br>';
                    echo '<button name="btnsavshari" type="submit" class="green" style = "height:30px; width:400px; font-size:12px;">Save SoundCard Settings</button>';
                    echo '<br>';
                    echo '<button name="btnrstshari" type="submit" class="green" style = "height:30px; width:400px; font-size:12px;">Restore SoundCard Settings</button>';
                }    
                if (($svxRadio == "Shari") || ($svxRadio == "RFGuru") || ($svxRadio == "Elenata")) {
                    echo '<br>';
                    echo '<button name="btnsavshari" type="submit" class="green" style = "height:30px; width:400px; font-size:12px;">Save ' . $svxRadio . ' Sound Settings</button>';
                    echo '<button name="btnrstshari" type="submit" class="green" style = "height:30px; width:400px; font-size:12px;">Restore ' . $svxRadio . ' Sound Settings</button>';
                }    
            echo '<br>';
            echo '<button id=jumptoAm name=jmptoAm type="submit" class="green" style = "height:30px; width:200px; font-size:12px;" value=\"9990\">Parrot 9990 (Modulations Test)</button>';
            echo '<button id=jumptoAm name=jmptoAm type="submit" class="green" style = "height:30px; width:200px; font-size:12px;" value=\"9998\">Parrot 9998 (Modulations Test)</button>';
        ?>
        <input type="hidden" name="form_submitted" value="1">
    </form>
    </center>

    <?php
    if ($_SERVER["REQUEST_METHOD"] == "POST" && isset($_POST['form_submitted'])) {
        if (isset($_POST['headphone'])) {
            $headphone_percentage = intval($_POST['headphone']);
            $headphone_value = ($headphone_percentage / 100) * $max_values['headphone'];
            echo "Card: $sc $headphone_value<br>";
            if ($svxRadio === "Elenata") {
                exec("sudo amixer -c" . $sc_tx . " cset numid=10 " . escapeshellarg($headphone_value));
            } else {    
                exec("sudo amixer -c" . $sc_tx . " cset numid=6 " . escapeshellarg($headphone_value));
            }    
        }

        if (isset($_POST['mic'])) {
            if ($sc_tx === $sc_rx) {
                $mic_percentage = intval($_POST['mic']);
                $mic_value = ($mic_percentage / 100) * $max_values['mic'];
                $sc = $sc_tx;
                exec("sudo amixer -c" . $sc . " cset numid=4 " . escapeshellarg($mic_value));
            } else {
                $mic_percentage = intval($_POST['mic']);
                $mic_value = ($mic_percentage / 100) * $max_values_qx18;
                $sc = $sc_rx;
                exec("sudo amixer -c" . $sc . " cset numid=6 " . escapeshellarg($mic_value));
            }                    
        }

        if (isset($_POST['capture'])) {
            $capture_percentage = intval($_POST['capture']);
            $capture_value = ($capture_percentage / 100) * $max_values['capture'];
//            exec("sudo amixer -c" . $sc_tx . " cset numid=8 " . escapeshellarg($capture_value));
            if ($svxRadio === "Elenata") {
                exec("sudo amixer -c" . $sc_tx . " cset numid=2 " . escapeshellarg($capture_value));
            } else {    
                exec("sudo amixer -c" . $sc_tx . " cset numid=8 " . escapeshellarg($capture_value));
            }    
        }
        if ((defined ('debug')) && (debug > 0)) echo "vor autogain values: $autogain<br>";
        if (isset($_POST['autogain'])) {
            $autogain = $_POST['autogain'] === '1' ? '1' : '0';
            if ((defined ('debug')) && (debug > 0)) echo "Ende autogain values: $autogain<br>";
            exec("sudo amixer -c" . $sc_tx. " cset numid=9 " . escapeshellarg($autogain));
        }
        if (isset($_POST['btnsavshari']))
            {
// wichttig: damit das funktioniert, muss mit visudo folgendes eingetragen werden
// svxlink        ALL=(ALL) NOPASSWD: /usr/sbin/alsactl
/*
            $retval = null;
            $screen = null;
            $command = DL3EL . "/astore.sh  2>&1";
            exec($command,$screen,$retval);
*/
//$sc_rx_cmp
// $sc_rx_cmp Mikrofon     svxlink rx, Index: $sc_rx
// $sc_tx_cmp LAutsprecher svxlink tx, Index: $sc_tx   

            $command = DL3EL . "/astore.sh " . $sc_tx_cmp . " 2>&1";
            $output = shell_exec($command);
            if ((defined ('debug')) && (debug > 0)) { 
                $logtext = "AudioSave: " . $command . "\n[" . $output . "]";
                addsvxlog($logtext);
            }    
            if ($sc_tx !== $sc_rx) {
                $command = DL3EL . "/astore.sh " . $sc_rx_cmp . " 2>&1";
                $output = shell_exec($command);
                if ((defined ('debug')) && (debug > 0)) { 
                    $logtext = "AudioSave: " . $command . "\n[" . $output . "]";
                    addsvxlog($logtext);
                }    
            }
        }
        if (isset($_POST['btnrstshari']))
            {
// wichttig: damit das funktioniert, muss mit visudo folgendes eingetragen werden
// svxlink        ALL=(ALL) NOPASSWD: /usr/sbin/alsactl
/*
            $retval = null;
            $screen = null;
            $command = DL3EL . "/arestore.sh  2>&1";
            exec($command,$screen,$retval);
*/
            $command = DL3EL . "/arestore.sh " . $sc_tx_cmp . " 2>&1";
            $output = shell_exec($command);
            if ((defined ('debug')) && (debug > 0)) { 
                $logtext = "AudioSave: " . $command . "\n[" . $output . "]";
                addsvxlog($logtext);
            }    
            if ($sc_tx !== $sc_rx) {
                $command = DL3EL . "/arestore.sh " . $sc_rx_cmp . " 2>&1";
                $output = shell_exec($command);
                if ((defined ('debug')) && (debug > 0)) { 
                    $logtext = "AudioSave: " . $command . "\n[" . $output . "]";
                    addsvxlog($logtext);
                }    
            }
        }

        // Refresh the page to show updated values
        echo "<script type='text/javascript'>
        reloadPage();
        </script>";    

        if (isset($_POST["jmptoAm"])) {
            $exec= "echo '*91" . $_POST['jmptoAm'] . "#' > /tmp/dtmf_svx";
            exec($exec,$output);
            echo "<meta http-equiv='refresh' content='0'>";
            $logtext = "DTMF via Dashboard: "  . $_POST['jmptoAm'];
        }    
    

    }
    echo '<div id="content" style = "margin-bottom:30px;">'."\n";
    $call_script = "parrot_mqtt.php";
    $rate = 1000;
    echo '<script type="text/javascript">'."\n";
    echo 'function reloadmqtt(){'."\n";
    echo '  $("#content").load("' . $call_script . '",function(){ setTimeout(reloadmqtt,' . $rate . ') });'."\n";
    echo '}'."\n";
    echo 'setTimeout(reloadmqtt,' . $rate . ');'."\n";
    echo '$(window).trigger(\'resize\');'."\n";
    echo '</script>'."\n";
        
    echo '</div>'."\n";
    ?>
</body>

</html>
