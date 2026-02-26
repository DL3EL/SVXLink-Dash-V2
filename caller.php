<?php
if (session_status() === PHP_SESSION_NONE) {
    session_start();
}
include_once "include/settings.php";
include_once "include/page_top.php";

echo '<table style="margin-right:20px; border:none; border-collapse:collapse; background-color:#f1f1f1;">';
echo '<tr style="border:none; background-color:#f1f1f1;">';

// LINKER BEREICH (Status)
echo '<td width="200px" valign="top" class="hide" style="height:auto; border:none; background-color:#f1f1f1; width:100%;">';
    echo '<div id="statusInfo" style="margin-bottom:30px;">';
        include_once "include/status.php";
    echo '</div>';
    
    // Script für Status (Intervall)
    echo '<script type="text/javascript">
        function reloadStatusInfo(){
            $("#statusInfo").load("include/status.php", function(){ 
                setTimeout(reloadStatusInfo, 3000); 
            });
        }
        setTimeout(reloadStatusInfo, 3000);
    </script>';
echo '</td>';

// RECHTER BEREICH (Content)
echo '<td valign="top" style="height:auto; border:none; background-color:#f1f1f1;">';
    
    // Ein Wrapper-Div innerhalb der Zelle sorgt für die Ränder (10px links und rechts)
    echo '<div style="padding-left:10px; padding-right:10px; width: auto;">';
        
        // Dies ist der Ziel-Container für den Reload
//        echo '<div id="content_area" style="display: block;">';

echo '<div id="content_area">';

            if (isset($_GET['file']) && !empty($_GET['file'])) {
                $file = $_GET['file']; 
                $call_script = "./edit/index.php?file=" . $file;
                echo '<object style="outline:none; width:950px; height:900px;" data="' . $call_script . '"></object>';
            } else {    
                $call_script = $_GET['id'] . ".php";
                if (($call_script === "amixer/index.php") || ($call_script === "power/index.php")) {
                    echo '<object style="outline:none; width:600px; height:900px;" data="' . $call_script . '"></object>';
                } else {    
                    // Falls das inkludierte File selbst Tabellen hat, 
                    // verhindert das umschließende Div, dass diese am Rand kleben
    $id = htmlspecialchars($_GET['id']);
    $call_script = $id . ".php";
    include $call_script;
//                    include $call_script;
                }
            }
        echo '</div>'; // Ende #content_area
        
    echo '</div>'; // Ende Padding-Wrapper
echo '</td>';


echo '</td>';
// --- 3. GANZ RECHTER BEREICH (MQTT / Caller) ---
echo '<td valign="top" style="width:250px; border:none; background-color:#f1f1f1; padding-left:10px;">';
    include_once "caller_svxmqtt.php"; 
echo '</td>';
echo '</tr>';
echo '</table>';

// ... (nach den schließenden </td> Tags, ganz am Ende vor page_bottom)
if (isset($_GET['refresh']) && !empty($_GET['refresh'])) {
    $rate = intval($_GET['refresh']) * 1000;
    $ajax_url = $id . ".php"; 

    echo '<script type="text/javascript">
        function reloadCurrPage(){
            console.log("Starte AJAX Update für: ' . $ajax_url . '");
            $.ajax({
                url: "' . $ajax_url . '",
                cache: false,
                success: function(data){
                    $("#content_area").html(data);
                    console.log("Update erfolgreich um: " + new Date().toLocaleTimeString());
                },
                complete: function() {
                    setTimeout(reloadCurrPage, ' . $rate . ');
                }
            });
        }
        $(document).ready(function() {
            setTimeout(reloadCurrPage, ' . $rate . ');
        });
    </script>';
}

include_once "include/page_bottom.php"; 
?>
