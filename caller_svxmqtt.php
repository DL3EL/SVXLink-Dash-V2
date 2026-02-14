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
//		echo '<object style="outline:none; width:500px; height:1412px; justify-content: left;" data=svx2mqtt/index_duo.php></object>';
		echo '<object style="outline:none; width:500px; height:850px; justify-content: left;" data=svx2mqtt/index_duo.php></object>';
        echo '<center><small>Live Daten vom FM-Funknetz.de<br>basierend auf dem MQTT Livedashboard von DJ1JAY<br>(integriert von Frank, DL4EM)</small></center>';
        echo '</td>';
    }   
   
?>
