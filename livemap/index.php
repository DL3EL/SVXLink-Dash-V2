<?php
// Version V20260915_1640
// Farbpallette abrufen

if (!file_exists(__DIR__ . '/livemap_ini.php') && file_exists(__DIR__ . '/livemap_ini.php.example')) {
    copy(__DIR__ . '/livemap_ini.php.example', __DIR__ . '/livemap_ini.php');
    sleep(3); 
}

if (file_exists(__DIR__ . "/livemap_ini.php")) {
    require_once __DIR__ . "/livemap_ini.php";
} else {
    // Sicherheitsnetz: Falls auch die .example fehlte, stürzt das Skript kontrolliert ab
    die("Fehler: Die Konfigurationsdatei 'livemap_ini.php' fehlt und konnte nicht erstellt werden.");
}
?>


<!DOCTYPE html>
<html lang="de">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>FM-Funknetz LiveMap</title>
    
    <!-- Leaflet CSS -->
    <link rel="stylesheet" href="https://unpkg.com/leaflet@1.9.4/dist/leaflet.css">
    
    <!-- Eigene CSS -->
    <link rel="stylesheet" href="livemap.css">

    <!-- Hier werden die PHP-Variablen sicher an das CSS übergeben -->
    <style>
        :root {
				--talker-color: <?php echo $LIVEMAP_TALKER_COLOR; ?>;
				--receiver-color: <?php echo $LIVEMAP_RECEIVER_COLOR; ?>;
        }
    </style>
</head>

<body>
    <!-- Gesamte Karte -->
    <div id="map" data-darkreader-ignore>

        <!-- Linke Live-Liste -->
        <div id="talkerPanel">
            <div class="panelHeader">Aktive Stationen</div>
            <div id="talkerEmpty" class="emptyMessage">Funkstille...</div>
            <div id="talkerList"></div>
        </div>

        <!-- Darstellung Menü -->
        <div id="displayPanel">
            <div class="displayHeader">Darstellung</div>
            <label class="displayOption">
                <input type="checkbox" id="toggleTGNetwork" checked>
                <span>TG Netzwerk anzeigen</span>
            </label>
        </div>
    </div>

    <!-- Leaflet JS -->
    <script src="https://unpkg.com/leaflet@1.9.4/dist/leaflet.js"></script>

    <!-- MQTT.js -->
    <script src="https://unpkg.com/mqtt/dist/mqtt.min.js"></script>

    <!-- Eigene Logik -->
	<script>
		window.livemapConfig = <?php
			echo json_encode(
				$LIVEMAP_COLORS,
				JSON_UNESCAPED_SLASHES |
				JSON_UNESCAPED_UNICODE
        );
    ?>;
	</script>
    <script src="livemap.js"></script>

    <p style="font-size:8px; color:gray; margin:0;">@DL4EM</p>
</body>

</html>
