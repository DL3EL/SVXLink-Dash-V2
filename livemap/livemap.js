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
        [51.1657, 10.4515], 6
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
// Positionsdaten
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

        // Falls bereits Sender aktiv sind,
        // Marker jetzt erzeugen.
        updateTalkerMarkers();
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
                "/TEST/livemap/fm-set-tg.php",
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

    // TG Linien aktualisieren
    updateTGLines();
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
                    <div class="talker-popup">
                        <b>${call}</b>
                        <br>
                        TG: ${talker.tg}
                    </div>
                    `
                );
                return;
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
            // Popup
            // ------------------------------------------------
            marker.bindPopup(
                `
                <div class="talker-popup">
                    <b>${call}</b>
                    <br>
                    TG: ${talker.tg}
                </div>
                `
            );

            // ------------------------------------------------
            // Doppelklick -> TG wechseln
            // ------------------------------------------------
            marker.on(
                "dblclick",
                function (event) {
                    L.DomEvent.stopPropagation(
                        event
                    );

                    console.log(
                        "Marker Doppelklick:",
                        call,
                        "TG:",
                        talker.tg
                    );
                    changeTG(
                        talker.tg
                    );
                }
            );

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
    // Aktive Empfänger der aktuell belegten Talkgroups
    // --------------------------------------------------------
    const currentReceivers =
        new Set();
    active.forEach(
        function (
            talker
        ) {
            const tg =
                String(
                    talker.tg || ""
                )
                    .trim();
            if (!tg) {
                return;
            }

            const receivers =
                activeTG.get(
                    tg
                );

            if (!receivers) {
                return;
            }

            receivers.forEach(
                function (receiverCall) {
                    const call =
                        String(
                            receiverCall || ""
                        )
                            .trim()
                            .toUpperCase();

                    if (!call) {
                        return;
                    }

                    // Sender nicht zusätzlich
                    // als Empfänger markieren
                    if (
                        currentTalkers.has(
                            call
                        )
                    ) {
                        return;
                    }

                    currentReceivers.add(
                        call
                    );

                    // Position suchen
                    const position =
                        findNodePosition(
                            call
                        );

                    if (!position) {
                        return;
                    }

                    // Marker existiert bereits
                    if (
                        receiverMarkers.has(
                            call
                        )
                    ) {
                        const marker =
                            receiverMarkers.get(
                                call
                            );
                        marker.setLatLng(
                            [
                                position.lat,
                                position.lon
                            ]
                        );
                        marker.setPopupContent(
                            `
                            <div class="receiver-popup">
                                <b>${call}</b>
                                <br>
                                TG: ${tg}
                            </div>
                            `
                        );
                        return;
                    }

                    // ------------------------------------------------
                    // Neuer Empfänger Marker
                    // ------------------------------------------------
                    console.log(
                        "Neuer Empfänger Marker:",
                        call,
                        "TG:",
                        tg,
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
                                    createReceiverIcon(),
                                zIndexOffset: 100
                            }
                        );

                    marker.bindPopup(
                        `
                        <div class="receiver-popup">
                            <b>${call}</b>
                            <br>
                            TG: ${tg}
                        </div>
                        `
                    );

					marker.on(
						"dblclick",
						function (event) {

							L.DomEvent.stopPropagation(
								event
							);

							changeTG(
								tg
							);

						}
					);

						receiverMarkers.set(
							call,
							marker
						);

						marker.addTo(
						tgNetworkLayer
					);
                }
            );
        }
    );

    // --------------------------------------------------------
    // Nicht mehr aktive Empfänger entfernen
    // --------------------------------------------------------
    receiverMarkers.forEach(
        function (
            marker,
            call
        ) {

            // Ein Sender darf keinen Empfänger-Marker besitzen.
            if (
                currentTalkers.has(
                    call
                )
            ) {

                console.log(
                    "Empfänger Marker entfernen, da jetzt Sender:",
                    call
                );
                marker.remove();
                receiverMarkers.delete(
                    call
                );
                return;
            }

            // Nicht mehr aktiver Empfänger.
            if (
                !currentReceivers.has(
                    call
                )
            ) {
                console.log(
                    "Empfänger Marker entfernen:",
                    call
                );
                marker.remove();
                receiverMarkers.delete(
                    call
                );
            }
        }
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
// Feste Farben für Talkgroups
// ============================================================

const tgColors = [

    "#e53935",   // Rot
    "#1565c0",   // Kräftiges Blau
    "#43a047",   // Grün
    "#fb8c00",   // Orange
    "#8e24aa",   // Violett
    "#00838f",   // Dunkles Cyan
    "#f4511e",   // Rotorange
    "#c62828",   // Dunkelrot
    "#7cb342",   // Olivgrün
    "#d81b60",   // Pink
    "#6d4c41",   // Braun
    "#455a64"    // Blau-Grau

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

            tgNetworkLayer.addTo(
                map
            );

        }

        else {

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
    // Aktive Empfänger je Talkgroup
    // --------------------------------------------------------
    client.subscribe(
        "/server/state/activeTG/+",
        {
            qos:
                0
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

                // --------------------------------------------------------
                // Debug: Liste und Marker vergleichen
                // --------------------------------------------------------

                console.log(
                    "AKTIVE CALLS:",
                    activeTalkers.size,
                    Array.from(activeTalkers.keys())
                );
                console.log(
                    "MARKER:",
                    talkerMarkers.size,
                    Array.from(talkerMarkers.keys())
                );

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
// Klick auf TG
// --------------------------------------------------------

tg.addEventListener(

    "click",

    function(
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