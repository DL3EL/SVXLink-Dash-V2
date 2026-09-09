(function () {
    'use strict';
    console.log(
        'FM MQTT startet...'
    );
    if (
        typeof mqtt === 'undefined'
    ) {
        console.error(
            'MQTT.js wurde nicht geladen.'
        );
        return;
    }

    // Letzten bekannten MQTT-Zustand
    // aller Stationen speichern.
    //
    // Dadurch gehen Nachrichten nicht verloren,
    // wenn die Leaflet-Karte noch aufgebaut wird.
    const pendingNodes = new Map();

    // ------------------------------------------------------------
    // Status an die Karte übergeben
    // ------------------------------------------------------------
    function sendToMap(call, data) {
        const key =
            String(call || '')
                .trim()
                .toUpperCase();
        if (!key) {
            return false;
        }
        pendingNodes.set(
            key,
            data
        );
        if (
            window.FMMap &&
            typeof window.FMMap.updateNodeStatus ===
            'function'
        ) {
            const result =
                window.FMMap.updateNodeStatus(
                    key,
                    data
                );
            if (result === true) {
                pendingNodes.delete(
                    key
                );
                return true;
            }
        }
        return false;
    }

    // ------------------------------------------------------------
    // MQTT Verbindung
    // ------------------------------------------------------------
    const client =
        mqtt.connect(
            'wss://status.thueringen.link/mqtt',
            {
                reconnectPeriod: 5000,
                keepalive: 30,
                clean: true,
                clientId:
                    'svx-dashboard-' +
                    Math.random()
                        .toString(16)
                        .substring(2, 10)
            }
        );

    // ------------------------------------------------------------
    // Verbindung hergestellt
    // ------------------------------------------------------------
    client.on(
        'connect',
        function () {
            console.log(
                '********************************'
            );
            console.log(
                'MQTT VERBUNDEN'
            );
            console.log(
                '********************************'
            );


            // Node-Zustände
            client.subscribe(
                '/server/state/nodes/+',
                {
                    qos: 0
                },
                function (error) {
                    if (error) {
                        console.error(
                            'MQTT Subscribe Fehler:',
                            error
                        );
                    }
                    else {
                        console.log(
                            'MQTT Subscribe OK:',
                            '/server/state/nodes/+'
                        );
                    }
                }
            );


            // Echter Live-Talker
            client.subscribe(
                '/server/statethr/1',
                {
                    qos: 0
                },
                function (error) {
                    if (error) {
                        console.error(
                            'MQTT Subscribe Fehler:',
                            error
                        );
                    }
                    else {
                        console.log(
                            'MQTT Subscribe OK:',
                            '/server/statethr/1'
                        );
                    }
                }
            );
        }
    );
    // Activ TG
    client.subscribe(
        '/server/state/activeTG/+',
        {
            qos: 0
        },
        function (error) {
            if (error) {
                console.error(
                    'MQTT Subscribe Fehler activeTG:',
                    error
                );
            }
            else {
                console.log(
                    'MQTT Subscribe OK:',
                    '/server/state/activeTG/+'
                );
            }
        }
    );


    // ------------------------------------------------------------
    // MQTT Nachrichten
    // ------------------------------------------------------------
    client.on(
        'message',
        function (
            topic,
            payload
        ) {
            const text =
                payload.toString();

            // ====================================================
            // AKTIVE EMPFÄNGER EINER TALKGROUP
            // ====================================================
            if (
                topic.startsWith(
                    '/server/state/activeTG/'
                )
            ) {
                const tg =
                    topic
                        .substring(
                            '/server/state/activeTG/'.length
                        )
                        .replace(/\/$/, '')
                        .trim();
                if (!tg) {
                    return;
                }
                let list;
                try {
                    list =
                        JSON.parse(text);
                }
                catch (error) {
                    console.error(
                        'activeTG JSON Fehler:',
                        error
                    );
                    return;
                }
                if (
                    !Array.isArray(list)
                ) {
                    return;
                }
                const calls =
                    list
                        .map(
                            function (call) {
                                return String(
                                    call || ''
                                )
                                    .trim()
                                    .toUpperCase();
                            }
                        )
                        .filter(Boolean);
                console.log(
                    'ACTIVE TG:',
                    tg,
                    'RX:',
                    calls.length
                );
                if (
                    window.FMMap &&
                    typeof
                    window.FMMap.updateActiveTG ===
                    'function'
                ) {
                    window.FMMap.updateActiveTG(
                        tg,
                        calls
                    );
                }
                return;
            }

            // ====================================================
            // ECHTES LIVE-TALKER-EVENT
            // ====================================================
            if (
                topic ===
                '/server/statethr/1'
            ) {
                let data;
                try {
                    data =
                        JSON.parse(text);
                }
                catch (error) {
                    console.error(
                        'statethr JSON Fehler:',
                        error
                    );
                    return;
                }
                const call =
                    String(
                        data.call || ''
                    )
                        .trim()
                        .toUpperCase();
                if (!call) {
                    return;
                }

                // start = TX beginnt
                // stop  = TX endet
                let isTalker;
                if (
                    data.talk === 'start'
                ) {
                    isTalker = true;
                }
                else if (
                    data.talk === 'stop'
                ) {
                    isTalker = false;
                }
                else {
                    console.log(
                        'Unbekanntes talk:',
                        data.talk
                    );
                    return;
                }
                const liveData = {
                    call: call,
                    isTalker: isTalker,
                    liveTalk: true,
                    tg:
                        data.tg !== undefined
                            ? data.tg
                            : 0,
                    time:
                        data.time || '',
                    server:
                        data.server || ''
                };
                const result =
                    sendToMap(
                        call,
                        liveData
                    );
                if (call === 'DL4EM-L') {
                    console.log(
                        '### DL4EM-L MAP RESULT ###',
                        result
                    );
                }
                return;
            }

            // ====================================================
            // PERIODISCHER NODE-STATUS
            // ====================================================
            const prefix =
                '/server/state/nodes/';
            if (
                !topic.startsWith(prefix)
            ) {
                return;
            }
            const call =
                topic
                    .substring(
                        prefix.length
                    )
                    .trim()
                    .toUpperCase();
            if (!call) {
                return;
            }
            let data;
            try {
                data =
                    JSON.parse(text);
            }
            catch (error) {
                console.error(
                    'MQTT Node JSON Fehler:',
                    error
                );
                return;
            }
            if (call === 'DL4EM-L') {
                console.log(
                    '### DL4EM-L NODE ###',
                    data
                );
            }

            // Periodischen Node-Zustand speichern
            sendToMap(
                call,
                {
                    ...data,

                    liveTalk: false
                }
            );
        }
    );

    // ------------------------------------------------------------
    // Reconnect
    // ------------------------------------------------------------
    client.on(
        'reconnect',
        function () {
            console.log(
                'MQTT: neuer Verbindungsversuch...'
            );
        }
    );

    // ------------------------------------------------------------
    // Verbindung geschlossen
    // ------------------------------------------------------------
    client.on(
        'close',
        function () {
            console.log(
                'MQTT: Verbindung geschlossen.'
            );
        }
    );

    // ------------------------------------------------------------
    // Offline
    // ------------------------------------------------------------
    client.on(
        'offline',
        function () {
            console.log(
                'MQTT: offline.'
            );
        }
    );

    // ------------------------------------------------------------
    // Fehler
    // ------------------------------------------------------------
    client.on(
        'error',
        function (error) {
            console.error(
                'MQTT FEHLER:',
                error
            );
        }
    );
})();