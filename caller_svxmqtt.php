<?php
    if (((defined('DL3EL_LIVEDB_AUTO')) && (DL3EL_LIVEDB_AUTO === "yes")) || ((defined('DL3EL_LIVEDB')) && (DL3EL_LIVEDB === "yes"))) {
    // Live DB
        echo '<td valign="top" style = "border:none;">';
//		echo '<object style="outline:none; width:500px; height:1412px; justify-content: left;" data=svx2mqtt/index_duo.php></object>';
		echo '<object style="outline:none; width:500px; height:850px; justify-content: left;" data=svx2mqtt/index_duo.php></object>';
        echo '</td>';
    }    
?>
