<?php
// DL3EL_LIVEDB === "right" LiveDB rechts
// DL3EL_LIVEDB === "top" LiveDB über Menu oben
// DL3EL_LIVEDB undiefined: LiveDB rechts

	if ($_SESSION['auth'] !== "AUTHORISED") {
	    if ((defined ('debug')) && (debug > 0)) echo "not authorized<br>";
	    return;
    }
    if ((!defined('DL3EL_LIVEDB')) || ((defined('DL3EL_LIVEDB')) && ((DL3EL_LIVEDB === "right") || (DL3EL_LIVEDB === "yes")))) {
    // Live DB
        echo '<td valign="top" style = "border:none;">';
//		echo '<object style="outline:none; width:500px; height:850px; justify-content: left;" data=svx2mqtt/index_duo.php></object>';
//		echo '<object style="outline:none; width:500px; height:850px; justify-content: left;" data=svx2mqtt/index_neu.php></object>';
// neu, mqtt
        if (file_exists($mqtt_data)) {
            $delta = time() - filemtime($mqtt_data);
            if ($delta < 15) {
// something received, heartbeat every 11s, dl3el mqtt ok
                $mqtt = 1;
            } else {
// nothing received within 14s, dl3el mqtt not ok
                $mqtt = 0;
            }
        } else {
                $mqtt = 0;
        }    
        if (defined('DL3EL_MQTT')) {
            if (DL3EL_MQTT === "no") {
                $mqtt = 0;
            } else {
                $mqtt = 1;
            }    
        }
        if ($mqtt) {
            echo '<div id="svxmqtt" style = "margin-bottom:30px;">'."\n";
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

        echo '<center><small>Live Daten vom FM-Funknetz.de<br>basierend auf dem MQTT Livedashboard von DJ1JAY<br>(integriert von Frank, DL4EM)</small></center>';
        echo '</td>';
    }   
   
?>
