<?php
if ($_SESSION['auth'] !== "AUTHORISED") {
    return;
}

// Erkennung: Wird das Script via AJAX (jQuery .load) aufgerufen?
$isAjax = (!empty($_SERVER['HTTP_X_REQUESTED_WITH']) && strtolower($_SERVER['HTTP_X_REQUESTED_WITH']) == 'xmlhttprequest');

// --- NUR FÜR DEN NEUEN UK-ZWEIG ---
if ($fmnetwork === "uk.wide.svxlink.uk") {
    
    // Beim ersten Laden (include) müssen wir die Tabellenzelle öffnen
    if (!$isAjax) {
        echo '<td valign="top" style="border:none; width:600px;">';
        echo '<div id="svxmqtt" style="margin-bottom:30px;">' . "\n";
    }

    // Das eigentliche Script einbinden
    $svx_include = 1;
    include "uk-wide.php";

    // Beim ersten Laden den JS-Reload-Code ausgeben
    if (!$isAjax) {
        $rate = 2000;
        $call_script = "uk-wide.php";
        echo '<script type="text/javascript">' . "\n";
        echo 'function reloadCurrPage(){' . "\n";
        echo '  $("#svxmqtt").load("' . $call_script . '", function(){ setTimeout(reloadCurrPage, ' . $rate . ') });' . "\n";
        echo '}' . "\n";
        echo 'setTimeout(reloadCurrPage, ' . $rate . ');' . "\n";
        echo '$(window).trigger(\'resize\');' . "\n";
        echo '</script>' . "\n";
        echo '</div>'; // div schließen
        echo '</td>';  // td schließen
    }

} else {
    // --- RESTLICHER CODE (FM-FUNKNETZ) BLEIBT UNVERÄNDERT ---
    // (Hier folgt dein originaler Code für das Standard-Netz)
    if ((!defined('DL3EL_LIVEDB')) || ((defined('DL3EL_LIVEDB')) && ((DL3EL_LIVEDB === "right") || (DL3EL_LIVEDB === "yes")))) {
        echo '<td valign="top" style = "border:none;">';
        
        $mqtt_data = DL3EL_BASE . "svx2mqtt/mqtt.data";
        $mqtt = file_exists("/usr/local/bin/mqtt-simple") ? 1 : 0;
        if (file_exists($mqtt_data) && (time() - filemtime($mqtt_data) < 15)) $mqtt = 1;
        if (defined('DL3EL_MQTT')) $mqtt = (DL3EL_MQTT === "no") ? 0 : 1;

        if ($mqtt) {
            echo '<div id="svxmqtt" style = "margin-bottom:30px;">'."\n";
            $svx_include = 1;
            include "svx2mqtt/index_neu.php";      
            $rate = 1000;
            $call_script = "svx2mqtt/index_neu.php";      
            echo '<script type="text/javascript">'."\n";
            echo 'function reloadCurrPage(){'."\n";
            echo '  $("#svxmqtt").load("' . $call_script . '",function(){ setTimeout(reloadCurrPage,' . $rate . ') });'."\n";
            echo '}'."\n";
            echo 'setTimeout(reloadCurrPage,' . $rate . ');'."\n";
            echo '$(window).trigger(\'resize\');'."\n";
            echo '</script>'."\n";
            echo '</div>';
        } else {
            echo '<object style="outline:none; width:500px; height:850px; justify-content: left;" data=svx2mqtt/index_duo.php></object>';
        }
        echo '</td>';
    }   
}
?>
