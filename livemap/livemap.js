// Version 20260915

"use strict";

// ============================================================
// FM-Funknetz LiveMap
// ============================================================

// ------------------------------------------------------------
// Leaflet Karte
// ------------------------------------------------------------
const map =
    L.map(
        "map"
    ).setView(
        [51.1657, 10.4515], 6    // zentraler Mittelpunkt der Karte und default Zoomfaktor
    );

L.tileLayer(
    "https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png",
    {
        maxZoom: 19,
        attribution:
            "&copy; OpenStreetMap contributors"
    }
).addTo(map);

// ============================================================
// Positionsdaten Variablen Definition
// ============================================================

// Call -> Positionsdaten
const nodePositions = new Map();

// Aktuell sichtbare Sender Marker rot
const talkerMarkers = new Map();

// Akuell sichtbare Zuhörer Marker grün
const receiverMarkers = new Map();

// TG Linien Sender -> Empfänger
const tgLines = new Map();

// Gemeinsamer Layer für Empfänger und TG Linien
const tgNetworkLayer =
    L.layerGroup()
        .addTo(
            map
        );

// Aktive Empfänger je Talkgroup TG -> Set(Call)
const activeTG = new Map();

// TG Netzwerk Darstellung
let showTGNetwork = true;

// ------------------------------------------------------------
// Positionsdaten laden
//
// Quelle:
// ../livemap/fm-map-data.php
// ------------------------------------------------------------
async function loadNodePositions() {
    console.log(
        "Lade Positionsdaten..."
    );

    try {
        const response =
            await fetch(
                "fm-map-data.php",
                {
                    cache: "no-store"
                }
            );

        if (!response.ok) {
            throw new Error(
                "HTTP " +
                response.status
            );
        }

        const data =
            await response.json();

        if (!Array.isArray(data)) {
            console.error(
                "Positionsdaten haben ungültiges Format"
            );
            return;
        }

        // Alte Positionsdaten löschen.
        nodePositions.clear();
        let validPositions = 0;
        data.forEach(
            function (node) {
                if (
                    !node ||
                    typeof node !== "object"
                ) {
                    return;
                }

                // ------------------------------------------------
                // Rufzeichen
                // ------------------------------------------------
                const call =
                    String(
                        node._call ??
                        node.Call ??
                        node.call ??
                        node.callsign ??
                        ""
                    )
                        .trim()
                        .toUpperCase();

                // ------------------------------------------------
                // Latitude
                // ------------------------------------------------
                const lat =
                    parseFloat(
                        String(
                            node.LAT ??
                            node.lat ??
                            node.latitude ??
                            ""
                        )
                            .replace(",", ".")
                    );

                // ------------------------------------------------
                // Longitude
                // ------------------------------------------------
                const lon =
                    parseFloat(
                        String(
                            node.LONG ??
                            node.lon ??
                            node.lng ??
                            node.longitude ??
                            ""
                        )
                            .replace(",", ".")
                    );

                // ------------------------------------------------
                // Nur gültige Datensätze übernehmen
                // ------------------------------------------------
                if (
                    !call ||
                    !Number.isFinite(lat) ||
                    !Number.isFinite(lon)
                ) {
                    return;
                }

                // Plausibilitätsprüfung.
                if (
                    lat < -90 ||
                    lat > 90 ||
                    lon < -180 ||
                    lon > 180
                ) {
                    return;
                }

                // ------------------------------------------------
                // 0/0 ist keine sinnvolle Stationsposition.
                // Solche fehlerhaften SVXLink-Konfigurationen
                // werden genauso behandelt wie eine fehlende
                // Position: Die Station wird nicht als Marker
                // dargestellt und nicht für TG-Linien verwendet.
                // ------------------------------------------------
                if (lat === 0 && lon === 0) {
                    console.log(
                        "Ungültige Position 0/0 - Station übersprungen:",
                        call
                    );
                    return;
                }

                // ------------------------------------------------
                // Position speichern
                // ------------------------------------------------
                nodePositions.set(
                    call,
                    {
                        call: call,
                        lat: lat,
                        lon: lon,
                        data: node
                    }
                );
                validPositions++;
            }
        );

        console.log(
            "Positionsdaten geladen:",
            validPositions
        );

        console.log(
            "Beispiel Positionen:",
            Array.from(
                nodePositions.values()
            ).slice(0, 5)
        );

        // Positionsdaten wurden komplett neu geladen.
        // Deshalb Markerbestand sofort gegen den aktuellen
        // active/activeTG-Zustand synchronisieren.
        updateTalkerMarkers();
        updateTGLines();
        renderTalkers();
    }
    catch (error) {
        console.error(
            "Fehler beim Laden der Positionsdaten:",
            error
        );
    }
}

// ============================================================
// Position einer Station suchen
// ============================================================
function findNodePosition(call) {
    const searchCall =
        String(call || "")
            .trim()
            .toUpperCase();

    if (!searchCall) {
        return null;
    }

    // --------------------------------------------------------
    // 1. Exakte Übereinstimmung
    // --------------------------------------------------------
    if (
        nodePositions.has(searchCall)
    ) {
        return nodePositions.get(
            searchCall
        );
    }

    // --------------------------------------------------------
    // 2. Basis-Call ermitteln
    // DL2NJM-DS -> DL2NJM
    // --------------------------------------------------------
    const baseCall =
        searchCall
            .split("-")[0];

    // Direkter Basis-Call.
    if (
        nodePositions.has(baseCall)
    ) {

        console.log(
            "Basis-Call Position:",
            searchCall,
            "->",
            baseCall
        );

        return nodePositions.get(
            baseCall
        );
    }

    // --------------------------------------------------------
    // 3. Station mit gleichem Basis-Call suchen
    //
    // Beispiel:
    //
    // DL2NJM-DS
    // DL2NJM-HS
    // DL2NJM-BS
    // --------------------------------------------------------
    for (
        const [
            positionCall,
            position
        ]
        of nodePositions
    ) {

        if (
            positionCall
                .split("-")[0] ===
            baseCall
        ) {

            console.log(
                "Basis-Call Zuordnung:",
                searchCall,
                "->",
                positionCall
            );

            return position;
        }
    }

    console.warn(
        "Keine Position gefunden:",
        searchCall
    );
    return null;
}

// ============================================================
// Marker Icon Sender
// ============================================================
function createTalkerIcon() {
    return L.divIcon(
        {
            className:
                "talker-marker",
            html:
                '<div class="talker-marker-inner"></div>',
            iconSize:
                [18, 18],
            iconAnchor:
                [9, 9]
        }
    );
}

// ============================================================
// TG-Wechsel über Empfänger-Popup
// ------------------------------------------------------------
// Globaler Capture-Handler für dynamisch erzeugte Leaflet-Popups.
// ============================================================
document.addEventListener(
    "click",
    function (event) {
        const actionElement =
            event.target &&
            event.target.closest(
                ".receiver-popup-action"
            );

        if (!actionElement) {
            return;
        }

        const tg =
            actionElement.dataset.tg;

        if (!tg) {
            return;
        }

        event.preventDefault();
        event.stopPropagation();
        event.stopImmediatePropagation();

        changeTG(tg);
    },
    true
);

// ============================================================
// Marker Icon Empfänger
// ============================================================
function createReceiverIcon() {
    return L.divIcon(
        {
            className:
                "receiver-marker",
            html:
                '<div class="receiver-marker-inner"></div>',
            iconSize:
                [18, 18],
            iconAnchor:
                [9, 9]
        }
    );
}

// ============================================================
// TG wechseln
// ============================================================
async function changeTG(tg) {
    if (!tg) {
        console.warn(
            "TG-Wechsel: keine TG vorhanden"
        );
        return;
    }

    console.log(
        "TG-Wechsel angefordert:",
        tg
    );

    try {
        const formData =
            new URLSearchParams();
        formData.append(
            "dtmfsvx",
            String(tg)
        );

        const response =
            await fetch(
//                "/TEST/livemap/fm-set-tg.php",
                "fm-set-tg.php",
                {
                    method:
                        "POST",
                    headers:
                    {
                        "Content-Type":
                            "application/x-www-form-urlencoded"
                    },
                    body:
                        formData.toString()
                }
            );

        if (!response.ok) {
            throw new Error(
                "HTTP Fehler: " +
                response.status
            );
        }

        const result =
            await response.json();

        console.log(
            "TG-Wechsel Antwort:",
            result
        );

        if (!result.ok) {
            console.warn(
                "TG-Wechsel wurde abgelehnt:",
                result.message
            );
        }
    }
    catch (error) {
        console.error(
            "TG-Wechsel fehlgeschlagen:",
            error
        );
    }
}

// ============================================================
// Aktive Empfänger einer Talkgroup aktualisieren
// ============================================================
function updateActiveTG(
    tg,
    calls
) {
    const talkgroup =
        String(tg || "")
            .trim();

    if (!talkgroup) {
        return;
    }

    const receiverSet =
        new Set();

    if (
        Array.isArray(calls)
    ) {
        calls.forEach(
            function (call) {
                const normalizedCall =
                    String(call || "")
                        .trim()
                        .toUpperCase();
                if (normalizedCall) {
                    receiverSet.add(
                        normalizedCall
                    );
                }
            }
        );
    }

    // Empfänger für diese TG speichern
    activeTG.set(
        talkgroup,
        receiverSet
    );

    console.log(
        "ACTIVE TG UPDATE:",
        talkgroup,
        "RX:",
        Array.from(receiverSet)
    );

    // --------------------------------------------------------
    // Empfänger-Marker sofort mit dem neuen activeTG-Zustand
    // synchronisieren.
    //
    // Wichtig:
    // Ein activeTG-MQTT-Update kann Empfänger hinzufügen oder
    // entfernen, ohne dass gleichzeitig eine neue Talker-
    // Nachricht eintrifft. Deshalb reicht updateTGLines()
    // alleine hier nicht aus.
    // --------------------------------------------------------
    updateTalkerMarkers();

    // TG Linien aktualisieren
    updateTGLines();

    // Linke Liste ebenfalls sofort aktualisieren.
    renderTalkers();
}


// ============================================================
// Sender Marker aktualisieren
// ============================================================
function updateTalkerMarkers() {

    // --------------------------------------------------------
    // Ohne Positionsdaten keine Marker
    // --------------------------------------------------------
    if (nodePositions.size === 0) {

        console.log(
            "Positionsdaten noch nicht geladen"
        );
        return;
    }

    // --------------------------------------------------------
    // Aktuell aktive Calls sammeln
    //
    // Wichtig:
    // Ein Call darf nur EINEN Marker besitzen.
    // --------------------------------------------------------
    const currentTalkers =
        new Set();

    // --------------------------------------------------------
    // Aktive Sender durchgehen
    // --------------------------------------------------------
    active.forEach(
        function (
            talker,
            key
        ) {
            const call =
                String(
                    talker.call ||
                    key ||
                    ""
                )
                    .trim()
                    .toUpperCase();

            if (!call) {
                return;
            }

            // ------------------------------------------------
            // Call als aktuell aktiv merken
            // ------------------------------------------------
            currentTalkers.add(
                call
            );

            // ------------------------------------------------
            // Falls dieser Call vorher als Empfänger angezeigt
            // wurde, Empfänger-Marker entfernen.
            // ------------------------------------------------
            if (
                receiverMarkers.has(
                    call
                )
            ) {
                const receiverMarker =
                    receiverMarkers.get(
                        call
                    );
                receiverMarker.remove();
                receiverMarkers.delete(
                    call
                );

                console.log(
                    "Empfänger wird Sender - Empfänger Marker entfernt:",
                    call
                );
            }

            // ------------------------------------------------
            // Position suchen
            // ------------------------------------------------
            const position =
                findNodePosition(
                    call
                );

            // Keine Position vorhanden.
            // Dann kann kein Marker dargestellt werden.
            // ------------------------------------------------
            if (!position) {

                console.log(
                    "Keine Position für Sender:",
                    call
                );
                return;
            }


            // ------------------------------------------------
            // Marker existiert bereits
            // ------------------------------------------------
            if (
                talkerMarkers.has(
                    call
                )
            ) {
                const marker =
                    talkerMarkers.get(
                        call
                    );

                // Position aktualisieren
                marker.setLatLng(
                    [
                        position.lat,
                        position.lon
                    ]
                );

                // Popup aktualisieren

                marker.setPopupContent(
                    `
    <div class="talker-popup talker-popup-action">
        <b>${call}</b>
        <br>
        TG: ${talker.tg}
        <br>
        <span class="talker-popup-hint">
            ▶ Klicken zum TG-Wechsel
        </span>
    </div>
    `
                );

                marker._talkerTG =
                    String(
                        talker.tg || ""
                    ).trim();
                return
            }

            // ------------------------------------------------
            // Neuer Marker
            // ------------------------------------------------
            console.log(
                "Neuer Sender Marker:",
                call,
                "TG:",
                talker.tg,
                "Position:",
                position.lat,
                position.lon
            );

            const marker =
                L.marker(
                    [
                        position.lat,
                        position.lon
                    ],
                    {
                        icon:
                            createTalkerIcon(),
                        zIndexOffset: 1000
                    }
                );

            // ------------------------------------------------
            // Popup-Inhalt
            // ------------------------------------------------
            function updateTalkerPopup() {

                marker.bindPopup(
                    `
        <div class="talker-popup talker-popup-action">
            <b>${call}</b>
            <br>
            TG: ${talker.tg}
            <br>
            <span class="talker-popup-hint">
                ▶ Klicken zum TG-Wechsel
            </span>
        </div>
        `
                );

                marker._talkerTG =
                    String(
                        talker.tg || ""
                    ).trim();

                marker.on(
                    "popupopen",
                    function (event) {

                        const popupElement =
                            event.popup.getElement();

                        if (!popupElement) {
                            return;
                        }

                        const actionElement =
                            popupElement.querySelector(
                                ".talker-popup-action"
                            );

                        if (!actionElement) {
                            return;
                        }

                        if (
                            actionElement.dataset.tgBound === "1"
                        ) {
                            return;
                        }

                        actionElement.dataset.tgBound = "1";

                        actionElement.addEventListener(
                            "click",
                            function (clickEvent) {

                                clickEvent.stopPropagation();

                                console.log(
                                    "TG-Wechsel über Sender-Popup:",
                                    call,
                                    "TG:",
                                    marker._talkerTG
                                );

                                changeTG(
                                    marker._talkerTG
                                );
                            }
                        );
                    }
                );
            }

            updateTalkerPopup();

            // ------------------------------------------------
            // Marker auf Karte
            // ------------------------------------------------
            marker.addTo(
                map
            );

            // ------------------------------------------------
            // Marker eindeutig speichern
            // ------------------------------------------------
            talkerMarkers.set(
                call,
                marker
            );
        }
    );

    // --------------------------------------------------------
    // EMPFÄNGER: INKREMENTELLER VOLL-SYNC
    //
    // Der Soll-Zustand wird weiterhin aus ALLEN aktuell aktiven
    // Talkern und deren activeTG-Daten berechnet. Es werden aber
    // nur Marker geändert, die tatsächlich hinzugefügt, geändert
    // oder entfernt werden müssen. Dadurch entsteht bei jedem
    // MQTT-Ereignis kein komplettes Löschen/Neuanlegen mehr.
    // --------------------------------------------------------
    const desiredReceivers = new Map();

    console.log(
        "Empfänger-Sync aktive Talker:",
        Array.from(active.values()).map(function (talker) {
            return String(talker.call || "").trim().toUpperCase() +
                " -> TG " + String(talker.tg || "").trim();
        })
    );

    console.log(
        "Empfänger-Sync verwendete TGs:",
        Array.from(new Set(Array.from(active.values()).map(function (talker) {
            return String(talker.tg || "").trim();
        }).filter(Boolean)))
    );

    active.forEach(function (talker) {
        const tg = String(talker.tg || "").trim();
        if (!tg) return;

        const receivers = activeTG.get(tg);
        if (!receivers || receivers.size === 0) return;

        // Empfänger nur dann anzeigen, wenn der zugehörige Sprecher
        // selbst eine gültige Position liefert. Ohne Sprecherposition
        // kann auch keine TG-Linie zum Sprecher gezeichnet werden.
        const speakerPosition = findNodePosition(
            String(talker.call || '').trim().toUpperCase()
        );

        if (!speakerPosition) {
            console.log(
                "Empfänger-Sync: TG übersprungen - keine Sprecherposition:",
                String(talker.call || '').trim().toUpperCase(),
                "TG:",
                tg,
                "Empfänger:",
                Array.from(receivers)
            );
            return;
        }

        receivers.forEach(function (receiverCall) {
            const call = String(receiverCall || "").trim().toUpperCase();
            if (!call) return;

            // Aktiver Sender niemals zusätzlich als Empfänger anzeigen.
            if (currentTalkers.has(call)) return;

            // Ein Empfänger kann in mehreren TGs vorkommen. Für den
            // einzelnen Marker verwenden wir die zuerst gefundene TG.
            if (!desiredReceivers.has(call)) {
                desiredReceivers.set(call, tg);
            }
        });
    });

    // Nicht mehr benötigte Empfänger entfernen.
    receiverMarkers.forEach(function (marker, call) {
        if (!desiredReceivers.has(call)) {
            marker.remove();
            receiverMarkers.delete(call);
            console.log(
                "Empfänger-Sync: Marker entfernt:",
                call
            );
        }
    });

    // Benötigte Empfänger hinzufügen bzw. vorhandene Marker aktualisieren.
    desiredReceivers.forEach(function (tg, call) {
        const position = findNodePosition(call);

        if (!position) {
            console.log(
                "Empfänger-Sync: keine Position:",
                call
            );
            return;
        }

        let marker = receiverMarkers.get(call);

        if (!marker) {
            console.log(
                "Neuer Empfänger Marker:",
                call,
                "TG:",
                tg,
                "Position:",
                position.lat,
                position.lon
            );

            marker = L.marker(
                [position.lat, position.lon],
                {
                    icon: createReceiverIcon(),
                    zIndexOffset: 100
                }
            );

            marker._receiverTG = tg;

            marker.bindPopup(
                `
    <div class="receiver-popup receiver-popup-action" data-tg="${tg}">
        <b>${call}</b>
        <br>
        TG: ${tg}
        <br>
        <span class="receiver-popup-hint">
            ▶ Klicken zum TG-Wechsel
        </span>
    </div>
    `
            );

            receiverMarkers.set(call, marker);

            if (showTGNetwork) {
                marker.addTo(tgNetworkLayer);
            }
        }
        else {
            // Position und TG des bestehenden Markers aktualisieren,
            // ohne ihn aus der Karte zu entfernen.
            marker.setLatLng([position.lat, position.lon]);
            marker._receiverTG = tg;

            marker.setPopupContent(
                `
    <div class="receiver-popup receiver-popup-action" data-tg="${tg}">
        <b>${call}</b>
        <br>
        TG: ${tg}
        <br>
        <span class="receiver-popup-hint">
            ▶ Klicken zum TG-Wechsel
        </span>
    </div>
    `
            );

            if (showTGNetwork && !tgNetworkLayer.hasLayer(marker)) {
                marker.addTo(tgNetworkLayer);
            }
        }
    });

    console.log(
        "Empfänger-Sync abgeschlossen:",
        "Marker=",
        receiverMarkers.size,
        "Soll=",
        desiredReceivers.size,
        "Calls=",
        Array.from(receiverMarkers.keys())
    );

    // --------------------------------------------------------
    // Nicht mehr aktive Sender entfernen
    // --------------------------------------------------------
    talkerMarkers.forEach(
        function (
            marker,
            call
        ) {
            if (
                !currentTalkers.has(
                    call
                )
            ) {

                console.log(
                    "Sender Marker entfernen:",
                    call
                );
                marker.remove();
                talkerMarkers.delete(
                    call
                );
            }
        }
    );
}

// ============================================================
// Feste Farben für Talkgroups aus livemap_ini.php 
// und default Werte, wenn livemap_ini.php fehlt 
// bzw. fehlerhaft ist
// ============================================================
const tgColors =
    Array.isArray(window.livemapConfig) &&
        window.livemapConfig.length > 0
        ? window.livemapConfig
        : [
		    "#8A2BE2",       // 01 - Blau Violett
			"#1874CD",       // 02 - Kräftiges Blau
			"#458B74",       // 03 - Aquamarine
			"#8B4513",       // 04 - Schokolade
			"#CD2626",       // 05 - Feuer Rot
			"#CD0000",       // 06 - Dunkles Rot
			"#f4511e",       // 07 - Rotorange
			"#EEB422",       // 08 - Gold
			"#006400",       // 09 - Dunkelgrün
			"#8B636C",       // 10 - Pink
			"#5C5C5C",       // 11 - Grau
			"#CD5555"        // 12 - Inian Rot
        ];

// ============================================================
// Farbe für Talkgroup bestimmen
// ============================================================
function getTGColor(tg) {
    const text =
        String(tg || "");
    let hash =
        0;

    for (
        let i = 0;
        i < text.length;
        i++
    ) {
        hash =
            text.charCodeAt(i) +
            (
                (
                    hash << 5
                )
                -
                hash
            );
    }

    const index =
        Math.abs(
            hash
        )
        %
        tgColors.length;
    return tgColors[
        index
    ];

}

// ============================================================
// TG Verbindungslinien aktualisieren
// ============================================================
function updateTGLines() {

    // --------------------------------------------------------
    // TG-Netzwerk deaktiviert:
    // Keine Linien erzeugen oder aktualisieren.
    // Der Checkbox-Handler entfernt vorhandene Linien vollständig.
    // --------------------------------------------------------
    if (!showTGNetwork) {
        return;
    }

    // --------------------------------------------------------
    // Aktuell benötigte Linien
    // --------------------------------------------------------
    const currentLines =
        new Set();

    // --------------------------------------------------------
    // Alle aktiven Sender durchgehen
    // --------------------------------------------------------
    active.forEach(
        function (
            speaker,
            speakerCall
        ) {
            const tg =
                String(
                    speaker.tg || ""
                )
                    .trim();

            if (!tg) {
                return;
            }

            // ------------------------------------------------
            // Empfänger dieser TG holen
            // ------------------------------------------------
            const receivers =
                activeTG.get(
                    tg
                );

            if (
                !receivers ||
                receivers.size === 0
            ) {
                return;
            }

            // ------------------------------------------------
            // Senderposition
            // ------------------------------------------------
            const speakerPosition =
                findNodePosition(
                    speakerCall
                );

            if (!speakerPosition) {
                return;
            }

            // ------------------------------------------------
            // Alle Empfänger durchgehen
            // ------------------------------------------------
            receivers.forEach(
                function (
                    receiverCall
                ) {
                    // Sender nicht mit sich selbst verbinden
                    if (
                        receiverCall === speakerCall
                    ) {
                        return;
                    }

                    if (
                        active.has(receiverCall)
                    ) {
                        return;
                    }

                    const receiverPosition =
                        findNodePosition(
                            receiverCall
                        );

                    if (!receiverPosition) {
                        return;
                    }

                    // ----------------------------------------
                    // Eindeutigen Linien-Key erzeugen
                    // ----------------------------------------
                    const lineKey =
                        speakerCall +
                        "|" +
                        receiverCall;

                    currentLines.add(
                        lineKey
                    );

                    // ----------------------------------------
                    // Linie bereits vorhanden?
                    // ----------------------------------------
                    if (
                        !tgLines.has(
                            lineKey
                        )
                    ) {

                        console.log(
                            "Neue TG Linie:",
                            speakerCall,
                            "->",
                            receiverCall,
                            "TG:",
                            tg
                        );

                        const line =
                            L.polyline(
                                [
                                    [
                                        speakerPosition.lat,
                                        speakerPosition.lon
                                    ],
                                    [
                                        receiverPosition.lat,
                                        receiverPosition.lon
                                    ]
                                ],
                                {
                                    color:
                                        getTGColor(
                                            tg
                                        ),

                                    weight:
                                        3,
                                    opacity:
                                        0.85,
                                    dashArray:
                                        "8,5",
                                    lineCap:
                                        "round",
                                    lineJoin:
                                        "round",
                                    interactive:
                                        false
                                }
                            );

                        tgLines.set(
                            lineKey,
                            line
                        );

                        line.addTo(
                            tgNetworkLayer
                        );
                    }

                    else {
                        const line =
                            tgLines.get(
                                lineKey
                            );

                        line.setLatLngs(
                            [
                                [
                                    speakerPosition.lat,
                                    speakerPosition.lon
                                ],
                                [
                                    receiverPosition.lat,
                                    receiverPosition.lon
                                ]
                            ]
                        );
                    }
                }
            );
        }
    );

    // --------------------------------------------------------
    // Nicht mehr benötigte Linien entfernen
    // --------------------------------------------------------
    tgLines.forEach(
        function (
            line,
            lineKey
        ) {

            if (
                !currentLines.has(
                    lineKey
                )
            ) {
                console.log(
                    "TG Linie entfernen:",
                    lineKey
                );
                line.remove();
                tgLines.delete(
                    lineKey
                );
            }
        }
    );
}

// ============================================================
// Live-Zustand bei MQTT-Verbindungsabbruch zurücksetzen
// ============================================================
function clearLiveMapState(reason) {

    console.log(
        "LiveMap Zustand wird zurückgesetzt:",
        reason || "unbekannt"
    );

    // Aktive Senderdaten löschen
    active.clear();

    // Aktive Empfänger je TG löschen
    activeTG.clear();

    // Alle Sender-Marker entfernen
    talkerMarkers.forEach(
        function (marker) {
            marker.remove();
        }
    );
    talkerMarkers.clear();

    // Alle Empfänger-Marker entfernen
    receiverMarkers.forEach(
        function (marker) {
            marker.remove();
        }
    );
    receiverMarkers.clear();

    // Sicherheitshalber den gemeinsamen Layer vollständig leeren
    tgNetworkLayer.clearLayers();

    // Alle gespeicherten TG-Linien löschen
    tgLines.clear();

    // Linke Liste ebenfalls sofort auf Funkstille setzen
    renderTalkers();

    console.log(
        "LiveMap Zustand zurückgesetzt:",
        "Sender=", active.size,
        "Empfänger=", receiverMarkers.size,
        "TG=", activeTG.size,
        "Linien=", tgLines.size
    );
}

// ============================================================
// MQTT Konfiguration
// ============================================================
const WS_URL =
    "wss://status.thueringen.link/mqtt";

const TOPIC =
    "/server/statethr/1";

// ============================================================
// HTML Elemente
// ============================================================
// Call Anzeige
const talkerEmpty =
    document.getElementById(
        "talkerEmpty"
    );

// Sprecher Liste
const talkerList =
    document.getElementById(
        "talkerList"
    );

// Netzwerk Anzeige	
const toggleTGNetwork =
    document.getElementById(
        "toggleTGNetwork"
    );

// ============================================================
// Darstellung TG Netzwerk
// ============================================================
toggleTGNetwork.addEventListener(
    "change",
    function () {
        showTGNetwork =
            toggleTGNetwork.checked;

        console.log(
            "TG Netzwerk Anzeige:",
            showTGNetwork
                ?
                "EIN"
                :
                "AUS"
        );

        if (showTGNetwork) {
            // Layer wieder sichtbar machen.
            tgNetworkLayer.addTo(
                map
            );
            // Aktuellen Zustand der Marker und Linien
            // vollständig neu aufbauen.
            updateTalkerMarkers();
            updateTGLines();
        }

        else {
            // ----------------------------------------------------
            // Beim Ausschalten den gemeinsamen Layer vollständig
            // leeren. Das ist wichtig, weil darin sowohl TG-Linien
            // als auch Empfänger-Marker liegen.
            //
            // Nur den Layer von der Karte zu entfernen reicht nicht:
            // Marker können sonst weiterhin im Layer gespeichert
            // bleiben und beim sofortigen Wiedereinschalten zusammen
            // mit bereits nicht mehr gültigen Objekten erscheinen.
            // ----------------------------------------------------
            tgLines.clear();
            tgNetworkLayer.clearLayers();

            // Layer aus der Karte entfernen.
            map.removeLayer(
                tgNetworkLayer
            );
        }
    }
);

// ============================================================
// Aktive Sender
//
// call ->
//
// {
//     call,
//     tg,
//     startMs
// }
// ============================================================
const active =
    new Map();

// MQTT Client
let client = null;

// ============================================================
// MQTT Verbindung
// ============================================================
function connectMQTT() {
    console.log(
        "MQTT Verbindung wird aufgebaut..."
    );
    client =
        mqtt.connect(
            WS_URL,
            {
                reconnectPeriod:
                    3000,
                keepalive:
                    30,
                clean:
                    true,
                clientId:
                    "fm-livemap-" +
                    Math.random()
                        .toString(16)
                        .slice(2, 10)
            }
        );

    // --------------------------------------------------------
    // MQTT verbunden
    // --------------------------------------------------------
    client.on(
        "connect",
        function () {

            console.log(
                "MQTT verbunden"
            );

            // ------------------------------------------------
            // Sprach-/Talker-Status
            // ------------------------------------------------
            client.subscribe(
                TOPIC,
                {
                    qos: 0
                },
                function (err) {
                    if (err) {
                        console.error(
                            "MQTT Subscribe Fehler:",
                            err
                        );
                        return;
                    }

                    console.log(
                        "MQTT Topic abonniert:",
                        TOPIC
                    );
                }
            );

            // ------------------------------------------------
            // Aktive Empfänger je Talkgroup
            // ------------------------------------------------
            client.subscribe(
                "/server/state/activeTG/+",
                {
                    qos: 0
                },
                function (err) {

                    if (err) {
                        console.error(
                            "MQTT Subscribe Fehler activeTG:",
                            err
                        );
                        return;
                    }

                    console.log(
                        "MQTT Topic abonniert:",
                        "/server/state/activeTG/+"
                    );
                }
            );
        }
    );



    // --------------------------------------------------------
    // Reconnect
    // --------------------------------------------------------
    client.on(
        "reconnect",
        function () {

            console.log(
                "MQTT reconnect..."
            );
        }
    );

    // --------------------------------------------------------
    // Verbindung geschlossen
    // --------------------------------------------------------
    client.on(
        "close",
        function () {

            console.log(
                "MQTT Verbindung geschlossen"
            );

            // Nach einem Verbindungsabbruch dürfen keine alten
            // Sender-, Empfänger- oder TG-Zustände sichtbar bleiben.
            // MQTT verbindet sich anschließend automatisch neu.
            clearLiveMapState(
                "MQTT close"
            );
        }
    );

    // --------------------------------------------------------
    // MQTT Fehler
    // --------------------------------------------------------
    client.on(
        "error",
        function (err) {

            console.error(
                "MQTT Fehler:",
                err
            );
        }
    );



    // --------------------------------------------------------
    // MQTT Nachricht
    // --------------------------------------------------------
    client.on(
        "message",
        function (
            topic,
            payload
        ) {

            console.log(
                "MQTT TOPIC:",
                topic
            );

            try {
                // =================================================
                // Aktive Empfänger einer Talkgroup
                // =================================================
                if (
                    topic.startsWith(
                        "/server/state/activeTG/"
                    )
                ) {
                    const text =
                        typeof payload === "string"
                            ?
                            payload
                            :
                            new TextDecoder()
                                .decode(payload);

                    console.log(
                        "ACTIVE TG ERKANNT:",
                        topic,
                        "PAYLOAD:",
                        text
                    );

                    const activeTGNumber =
                        topic
                            .substring(
                                "/server/state/activeTG/".length
                            )
                            .replace(
                                /\/$/,
                                ""
                            )
                            .trim();

                    // Eine leere Payload bedeutet: diesen TG-Zustand löschen.
                    // Der Broker kann beim Zurücksetzen eines activeTG-Topics
                    // eine wirklich leere Payload liefern und nicht [].
                    if (text.trim() === "") {
                        console.log(
                            "ACTIVE TG LEER - Zustand gelöscht:",
                            activeTGNumber
                        );
                        activeTG.delete(
                            activeTGNumber
                        );
                        updateTalkerMarkers();
                        updateTGLines();
                        renderTalkers();
                        return;
                    }
                    try {
                        const calls =
                            JSON.parse(
                                text
                            );

                        console.log(
                            "ACTIVE TG MQTT:",
                            activeTGNumber,
                            calls
                        );

                        updateActiveTG(
                            activeTGNumber,
                            calls
                        );

                        // TG Linien aktualisieren
                        updateTGLines();
                    }

                    catch (error) {
                        console.error(
                            "ACTIVE TG JSON Fehler:",
                            error
                        );
                    }
                    return;
                }

                // =================================================
                // Nur Live-Talker Topic weiterverarbeiten
                // =================================================
                if (
                    topic !== TOPIC
                ) {
                    return;
                }

                // =================================================
                // Live-Talker Nachricht
                // =================================================
                const text =
                    typeof payload === "string"
                        ?
                        payload
                        :
                        new TextDecoder()
                            .decode(payload);
                const msg =
                    parseMsg(
                        text
                    );

                if (!msg) {
                    return;
                }

                console.log(
                    "MQTT RAW MSG:",
                    msg
                );

                // ------------------------------------------------
                // Nachricht auswerten
                // ------------------------------------------------
                const now =
                    Date.now();
                const id =
                    String(
                        msg.call ||
                        ""
                    )
                        .trim()
                        .toUpperCase();
                const tg =
                    String(
                        msg.tg ||
                        ""
                    )
                        .trim();
                const kind =
                    String(
                        msg.talk ||
                        ""
                    )
                        .toLowerCase();

                if (!id) {
                    return;
                }

                console.log(
                    "MQTT:",
                    kind,
                    id,
                    "TG:",
                    tg
                );

                // =================================================
                // START
                // =================================================
                if (
                    kind === "start"
                ) {

                    if (
                        !active.has(id)
                    ) {
                        active.set(
                            id,
                            {
                                call:
                                    id,
                                tg:
                                    tg,
                                startMs:
                                    now
                            }
                        );

                        console.log(
                            "START:",
                            id,
                            "TG:",
                            tg
                        );
                    }

                    else {
                        const a =
                            active.get(
                                id
                            );
                        a.tg =
                            tg ||
                            a.tg;
                    }
                }

                // =================================================
                // STOP
                // =================================================
                else if (
                    kind === "stop"
                ) {
                    if (
                        active.has(id)
                    ) {
                        active.delete(
                            id
                        );

                        console.log(
                            "STOP:",
                            id
                        );
                    }
                }

                // =================================================
                // Anzeige aktualisieren
                // =================================================
                updateTalkerMarkers();
                updateTGLines();
                renderTalkers();

            }
            catch (err) {

                console.error(
                    "MQTT Parsing Fehler:",
                    err
                );
            }
        }
    );
}

// ============================================================
// Sender-Sicherheitsbereinigung 30 Sekunden
// ============================================================
setInterval(
    cleanupStaleTalkers,
    30 * 1000
);

// ============================================================
// MQTT Nachricht parsen
// ============================================================
function parseMsg(text) {
    const clean =
        text.replace(
            /[\u0000-\u0008\u000B\u000C\u000E-\u001F]/g,
            ""
        );
    try {
        return JSON.parse(
            clean
        );
    }
    catch {
        return null;
    }
}

// ============================================================
// Veraltete Sender automatisch entfernen
// ============================================================
function cleanupStaleTalkers() {
    const now =
        Date.now();
    // Kein automatisches Löschen mehr nach 10/30 Sekunden.
    // Ein laufendes QSO kann deutlich länger dauern. Der MQTT-close-Reset
    // leert den Zustand bei Verbindungsverlust; ein normaler MQTT-stop
    // entfernt den Talker sofort.
    const maxAge =
        0;

    console.log(
        "CLEANUP:",
        new Date(now).toLocaleTimeString(),
        "aktive Sender:",
        active.size
    );

    active.forEach(
        function (
            talker,
            call
        ) {
            const age =
                now -
                Number(
                    talker.startMs || 0
                );

            console.log(
                "CLEANUP PRÜFUNG:",
                call,
                "Alter:",
                Math.round(age / 1000),
                "Sekunden"
            );
            // Absichtlich kein Timeout-Löschen.
            // Der ursprüngliche 10-Sekunden-Cleanup hat lange QSOs
            // fälschlich aus active entfernt und dadurch den Empfänger-
            // Zustand inkonsistent gemacht.
        }
    );
    updateTalkerMarkers();
    updateTGLines();
    renderTalkers();
}

// ============================================================
// Aktive Sender in linker Liste darstellen
// ============================================================
function renderTalkers() {
    const arr =
        Array
            .from(
                active.values()
            )
            .sort(
                function (
                    a,
                    b
                ) {

                    return (
                        a.startMs -
                        b.startMs
                    );
                }
            );

    // --------------------------------------------------------
    // Keine Sender
    // --------------------------------------------------------
    if (
        arr.length === 0
    ) {
        talkerEmpty.style.display =
            "block";
        talkerList.innerHTML =
            "";
        return;
    }

    // --------------------------------------------------------
    // Sender vorhanden
    // --------------------------------------------------------
    talkerEmpty.style.display =
        "none";
    talkerList.innerHTML =
        "";

    // --------------------------------------------------------
    // Sender erzeugen
    // --------------------------------------------------------
    arr.forEach(
        function (a) {
            const entry =
                document.createElement(
                    "div"
                );
            entry.className =
                "talkerEntry";

            // Rufzeichen
            const call =
                document.createElement(
                    "div"
                );

            call.className =
                "talkerCall";

            call.textContent =
                a.call;

            // --------------------------------------------------------
            // Warnhinweis bei aktivem Sprecher ohne Positionsdaten
            // --------------------------------------------------------
            const talkerPosition = findNodePosition(
                String(a.call || "").trim().toUpperCase()
            );

            if (!talkerPosition) {
                const warning =
                    document.createElement("span");
                warning.textContent = "!";
                warning.title =
                    "Keine Positionsdaten für diesen Sprecher. Empfänger dieses Sprechers werden nicht auf der Karte angezeigt.";
                warning.setAttribute(
                    "aria-label",
                    "Keine Positionsdaten für diesen Sprecher"
                );
                warning.style.display = "inline-flex";
                warning.style.alignItems = "center";
                warning.style.justifyContent = "center";
                warning.style.width = "16px";
                warning.style.height = "16px";
                warning.style.marginLeft = "6px";
                warning.style.background = "#ffd54f";
                warning.style.color = "#222222";
                warning.style.fontWeight = "700";
                warning.style.fontSize = "12px";
                warning.style.lineHeight = "16px";
                warning.style.clipPath = "polygon(50% 0%, 100% 100%, 0% 100%)";
                warning.style.paddingTop = "3px";
                warning.style.boxSizing = "border-box";
                warning.style.cursor = "help";
                call.appendChild(warning);
            }

            // --------------------------------------------------------
            // Klick auf Rufzeichen:
            // Talkgroup wechseln
            // --------------------------------------------------------
            call.addEventListener(
                "click",
                function (event) {
                    event.stopPropagation();

                    console.log(
                        "TG-Wechsel über Senderliste:",
                        a.call,
                        "TG:",
                        a.tg
                    );

                    if (a.tg) {
                        changeTG(
                            a.tg
                        );
                    }
                }
            );

            // Talkgroup
            const tg =
                document.createElement(
                    "div"
                );
            tg.className =
                "talkerTG";
            tg.textContent =
                a.tg ||
                "—";

            // --------------------------------------------------------
            // TG-Hintergrundfarbe = Farbe der TG-Verbindungslinien
            // --------------------------------------------------------
            if (a.tg) {
                tg.style.backgroundColor =
                    getTGColor(a.tg);
                // Weiße Schrift für gute Lesbarkeit
                tg.style.color =
                    "#ffffff";
            }

            // --------------------------------------------------------
            // Klick auf TG
            // --------------------------------------------------------
            tg.addEventListener(
                "click",

                function (
                    event
                ) {
                    event.stopPropagation();

                    if (
                        a.tg
                    ) {
                        changeTG(
                            a.tg
                        );
                    }
                }
            );
            entry.appendChild(
                call
            );
            entry.appendChild(
                tg
            );
            talkerList.appendChild(
                entry
            );
        }
    );
}


// ============================================================
// Schnittstelle für MQTT
// ============================================================
window.FMMap = {
    updateActiveTG:

        function (
            tg,
            calls
        ) {
            updateActiveTG(
                tg,
                calls
            );
        }
};

// ============================================================
// Initialisierung
// ============================================================

// Zuerst Positionsdaten laden.
loadNodePositions();

// MQTT Verbindung aufbauen.
connectMQTT();
