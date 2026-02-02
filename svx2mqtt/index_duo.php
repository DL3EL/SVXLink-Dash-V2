<!doctype html>
<html lang="de">
<head>
    <meta charset="utf-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1" />
    <title>Live Dash</title>
    <style>
        body {
            width:540px;
            background-color: #000000;
            margin-left: 20px;

        }

        table {
            width: 100%;
            border: 1px solid black;
            border-collapse: collapse;
        }

        th,
        td {
            border-bottom: 1px solid #374151;
        }

        th {
            text-align: left;
            //font-weight: bold;
            font-size: 12px;
            color: white;
            background-color: #0000FF;
        }

        td {
            text-align: middle;
            //font-weight: bold;
            font-size: 12px;
            color: black;
        }

        table tr:nth-child(even) {
            background: #e0e0e0;
            ;
        }

        table tr:nth-child(odd) {
            background: #f7f7f7;
            ;
        }

#activeTable {
        tr:nth-child(n+5) {
            display:none;
        }
}

#lhTable {
        tr:nth-child(n+70) {
            display:none;
        }
}

        button:disabled {
            opacity: .6;
            cursor: not-allowed;
        }

        #error {
            white-space: pre-wrap;
            color: #ef4444;
            font-size: 18px;
            margin-top: 8px;
        }
.small { font-size:14px; color:black; }

    .row { display:flex; gap:8px; align-items:center; flex-wrap:wrap; }
    .pill { padding:4px 10px; border-radius:999px; background:#ffffff; font-size:12px; border:1px solid #374151; }
    .ok { color:#10b981; }
    .warn { color:#f59e0b; }
    .bad { color:#ef4444; }
//    main { padding:16px; max-width:1200px; margin:0 auto; }
    .grid { display:grid; gap:12px; grid-template-columns: 1fr 1fr; }
    @media (max-width: 900px) { .grid { grid-template-columns: 1fr; } }
    .card { background:#edf0f5; border:1px solid #0f1011; border-radius:12px; overflow:hidden; }
    .card h2 { margin:0; font-size:15px; padding:10px 12px; border-bottom:1px solid #0f1011; }
    .card .body { padding:12px; }
    .tag { font-size:14px; background:blue; border:1px solid #374151; color:#ffffff; padding:2px 6px; border-radius:999px; }
//    .call { font-weight:bold; }
    .mono { font-family: ui-monospace, SFMono-Regular, Menlo, Monaco, Consolas, "Liberation Mono", "Courier New", monospace; }
    button:disabled { opacity:.6; cursor:not-allowed; }
    #error { white-space:pre-wrap; color:#ef4444; font-size:12px; margin-top:8px; }




    </style>
    <script src="https://unpkg.com/mqtt/dist/mqtt.min.js"></script>
</head>

<?php
// Farben:
include_once  '../include/settings.php';

if (defined ('SVXMQTT_COLOR_active')) {
  $svxmqtt_color_active = "style=cursor:pointer;border:none;border-radius:8px;width:85px;background-color:" . SVXMQTT_COLOR_active . ";color:white;font-weight:bold;font-size:14px;";
} else {
  $SVXMQTT_COLOR1_active = "style=cursor:pointer;border:none;border-radius:8px;width:85px;background-color:blue;color:white;font-weight:bold;font-size:14px;";
  $SVXMQTT_COLOR2_active = "style=cursor:pointer;border:none;border-radius:8px;width:85px;background-color:green;color:white;font-weight:bold;font-size:14px;";
  $SVXMQTT_COLOR3_active = "style=cursor:pointer;border:none;border-radius:8px;width:85px;background-color:silver;color:black;font-weight:bold;font-size:14px;";
  $svxmqtt_color_active = $SVXMQTT_COLOR2_active;
}

if (defined ('SVXMQTT_COLOR_passive')) {
  $svxmqtt_color_passive = "style=cursor:pointer;border:none;border-radius:8px;width:85px;background-color:" . SVXMQTT_COLOR_passive . ";color:white;font-weight:bold;font-size:14px;";
} else {
  $SVXMQTT_COLOR1_passive = "style=cursor:pointer;border:none;border-radius:8px;width:85px;background-color:blue;color:white;font-weight:bold;font-size:14px;";
  $SVXMQTT_COLOR2_passive = "style=cursor:pointer;border:none;border-radius:8px;width:85px;background-color:green;color:white;font-weight:bold;font-size:14px;";
  $SVXMQTT_COLOR3_passive = "style=cursor:pointer;border:none;border-radius:8px;width:85px;background-color:silver;color:black;font-weight:bold;font-size:14px;";
  $svxmqtt_color_passive = $SVXMQTT_COLOR3_passive;
}
?>

<body style="background:#e1e1e1;">
<form method="post">
<fieldset style = "box-shadow:5px 5px 20px #999; background-color:#f1f1f1; height:1380px; margin-top:15px; font-size:13px; border-radius: 10px;">
     <fieldset style="color:white; background-color:#f1f1f1; height:240px; font-size:14px; border:none; border-radius:10px;">
        <header>
            <center>
                <h1 style="background-color:#fififi; color:#0000ff; font:18pt arial, sans-serif;font-weight:bold;">Live Dashboard</h1>
            </center>
        </header>

            <main>
                <div>
                    <section>
                        <h3 style="color:#000000">Live</h3>
                        <div>
                            <button id="clients" style="margin-right:30px;margin-bottom:10px;color:black; font-size:14px;border-radius:10px;background-color:transparent;">Verbundene Clients: —</button>
                            <span id="conn" style="color:green; font-size:14px; font-weight:bold;">Getrennt</span>
                            <div style="color:#909090;font-size:20px, font-weight:bold;" id="activeEmpty">Funkstille .... Niemand spricht... aber wirklich niemand.</div>
                            <button hidden id="btnConnect">Verbinden</button>
                            <button hidden id="btnDisconnect" disabled>Trennen</button>
                            <button hidden id="btnTGConn" enabled>TGConn</button>

                            <fieldset style = "margin-left:2px;border:none;">
                            <table id="activeTable">
                                <thead>
                                    <tr style="height:24px;">
                                        <th style="width:22%;">&nbsp;&nbsp;Call</th>
                                        <th style="width:12%">TG</th>
                                        <th style="width:40%">TG Name</th>
                                        <th style="width:12%">Zeit</th>
                                        <th style="width:13%">Dauer</th>
                                    </tr>
                                </thead>
                                <tbody></tbody>
                            </table>
                            </fieldset>
                            <div id="error"></div>
                        </div>
                    </section>
    </fieldset>
    <fieldset style="color:white; background-color:#f1f1f1; height:1500; font-size:14px; border:none; border-radius:10px;">
        <form method="post">
                    <section>
                        <h3 style="color:#000000;">Zuletzt Aktiv</h3>
                        <div style="max-height:70vh; overflow:auto;">
                            <fieldset style = "height:1010px;box-shadow:2px 2px 10px #999;background-color:#e8e8e8;font-size:12px;border-radius: 10px;">
                            <table id="lhTable">
                                <thead>
                                    <tr style="height:24px;">
                                        <th style="width:22%">&nbsp;&nbsp;Call</th>
                                        <th style="width:12%">TG</th>
                                        <th style="width:40%">TG Name</th>
                                        <th style="width:12%">Zeit</th>
                                        <th style="width:13%">Dauer</th>
                                    </tr>
                                </thead>
                                <tbody></tbody>
                            </table>
		            </fieldset>
                        </div>
                    </section>
    </fieldset>
                </div>
            </main>

            <script>
                const WS_URL = "wss://status.thueringen.link/mqtt";
                const TOPIC = "/server/statethr/1"; // {"time":"18:09:43","talk":"start|stop","call":"DK0GH-HS","tg":"37574"}
                const TGDB_URL = "https://nc1.fm-funknetz.de/dashtt/tgdb_proxy.php"; // CORS-ok plaintext
                const CLIENTS_TOPIC = "/server/state/loginz"; // Zahl erwartet

                const elConn = document.getElementById("conn");
                const tgConn = document.getElementById("btnTGConn");
                const elBtnC = document.getElementById("btnConnect");
                const elBtnD = document.getElementById("btnDisconnect");
                const elClients = document.getElementById("clients");
                const elErr = document.getElementById("error");
                const elActiveEmpty = document.getElementById("activeEmpty");
                const elActiveTable = document.getElementById("activeTable");
                const elActiveBody = elActiveTable.querySelector("tbody");
                const elLHBody = document.getElementById("lhTable").querySelector("tbody");

                let client = null;
                const active = new Map();   // call -> { call, tg, startMs }
                const lastHeard = [];       // { call, tg, endMs, durationSec }
                const tgMap = new Map();    // id -> name

                function setState(txt, cls) {
                    elConn.textContent = txt;
                    elConn.className = "pill " + (cls || "");
                    elBtnC.disabled = (txt !== "&nbsp;Getrennt" && txt !== "&nbsp;Fehler");
                    elBtnD.disabled = (txt !== "&nbsp;Verbunden");
                }

                // Load TG DB (supports php-array style and line formats)
                async function loadTGDB() {
                    try {
                        const resp = await fetch(TGDB_URL, { cache: "no-store" });
                        const text = await resp.text();
                        parseTGDB(text);
                        render();
                    } catch (e) {
                        console.warn("TGDB fetch failed:", e);
                    }
                }

                function parseTGDB(text) {
                    tgMap.clear();
                    // PHP array style: '123' => 'TG123 Foo Bar', ...
                    const phpRe = /'(\d+)'\s*=>\s*'([^']*)'/g;
                    let m, found = false;
                    while ((m = phpRe.exec(text)) !== null) {
                        found = true;
                        const id = m[1].trim();
                        const raw = m[2].trim();
                        tgMap.set(id, normalizeName(id, raw));
                    }
                    if (found) return;

                    // Fallback: line lists
                    const lines = text.split(/\r?\n/);
                    for (let line of lines) {
                        line = line.trim();
                        if (!line || line.startsWith("//") || line.startsWith("#") || line.startsWith(";")) continue;
                        let parts = line.split(/\s*[,;\t]\s*/);
                        if (parts.length === 1) {
                            const mm = line.match(/^(\d+)\s+(.+)$/);
                            if (mm) parts = [mm[1], mm[2]];
                        }
                        if (parts.length >= 2) {
                            const id = parts[0].trim();
                            const raw = parts.slice(1).join(" ").trim();
                            if (/^\d+$/.test(id) && raw) {
                                tgMap.set(id, normalizeName(id, raw));
                            }
                        }
                    }
                }

                function normalizeName(id, raw) {
                    const esc = id.replace(/[-\/\\^$*+?.()|[\]{}]/g, "\\$&");
                    const reA = new RegExp("^TG\s*" + esc + "\s*", "i");
                    let name = raw.replace(reA, "").trim();
                    const reB = new RegExp("^TG\s*0*" + esc + "\b\s*-?\s*", "i");
                    name = name.replace(reB, "").trim();
                    if (!name) name = raw;
                    return name;
                }

                function tgName(id) {
                    const s = String(id || "");
                    return tgMap.get(s) || "";
                }

                function connectTG() {
                }

                function connect() {
                    if (client) try { client.end(true); } catch (e) { }
                    elErr.textContent = "";
                    setState("Verbinden…", "warn");
                    client = mqtt.connect(WS_URL, {
                        reconnectPeriod: 3000,
                        keepalive: 30,
                        clean: true,
                        clientId: "fm-talkers-" + Math.random().toString(16).slice(2, 10),
                    });

                    client.on("connect", () => {
                        setState("Verbunden", "ok");
                        client.subscribe([TOPIC, CLIENTS_TOPIC], { qos: 0 }, (err) => {
                            if (err) elErr.textContent = "Subscribe-Fehler: " + (err.message || String(err));
                        });
                    });
                    client.on("reconnect", () => setState("Verbinden…", "warn"));
                    client.on("close", () => setState("Getrennt"));
                    client.on("error", (err) => {
                        setState("Fehler", "bad");
                        elErr.textContent = (err && err.message) ? err.message : String(err || "Unbekannter Fehler");
                    });

                    client.on("message", (topic, payload) => {
                        try {
                            if (topic === CLIENTS_TOPIC) {
                                const t = (typeof payload === "string") ? payload : new TextDecoder().decode(payload);
                                const n = Number.parseInt(String(t).trim(), 10);
                                if (!Number.isNaN(n)) elClients.textContent = "Verbundene Clients: " + n;
                                return;
                            }
                            const text = (typeof payload === "string") ? payload : new TextDecoder().decode(payload);
                            const msg = parseMsg(text);
                            if (!msg) return;

                            const now = Date.now();
                            const id = String(msg.call || "").trim();
                            const tg = String(msg.tg || "").trim();
                            const kind = String(msg.talk || "").toLowerCase();

                            if (!id) return;
                            if (kind === "start") {
                                if (!active.has(id)) {
                                    active.set(id, { call: id, tg, startMs: now });
                                } else {
                                    const a = active.get(id);
                                    a.tg = tg || a.tg;
                                }
                            } else if (kind === "stop") {
                                if (active.has(id)) {
                                    const a = active.get(id);
                                    active.delete(id);
                                    const durSec = Math.max(0, Math.round((now - a.startMs) / 1000));
                                    lastHeard.unshift({ call: a.call, tg: a.tg || tg, endMs: now, durationSec: durSec });
                                    while (lastHeard.length > 100) lastHeard.pop();
                                }
                            }

                            render();
                        } catch (e) {
                            elErr.textContent = "Parsing-Fehler: " + (e && e.message ? e.message : String(e));
                        }
                    });
                }

                function disconnect() {
                    try { client && client.end(true); } catch (e) { }
                    client = null;
                    setState("Getrennt");
                }

                function parseMsg(text) {
                    const clean = text.replace(/[\u0000-\u0008\u000B\u000C\u000E-\u001F]/g, "");
                    try { return JSON.parse(clean); } catch { return null; }
                }

                function render() {

                    // Active
                    const arr = Array.from(active.values()).sort((a, b) => a.startMs - b.startMs);
                    elActiveBody.innerHTML = "";
                    if (!arr.length) {
                        elActiveEmpty.style.display = "block";
                        elActiveTable.style.display = "none";
                    } else {
                        elActiveEmpty.style.display = "none";
                        elActiveTable.style.display = "table";
                        const now = Date.now();
                        for (const a of arr) {
                            const durSec = Math.max(0, Math.round((now - a.startMs) / 1000));
                            const group = tgName(a.tg);
                            const tr = document.createElement("tr");
                            tr.innerHTML = `

<td style="color:#0065ff;font-weight:bold;">&nbsp;${escapeHtml(a.call)}</td>
<!--<td align=\"left\"><a href=\"https://dashboard.fm-funknetz.de/node.php?call=".$escapeHtml(a.call)}."\" target=\"_blank\" class=\"qrz_link\">${escapeHtml(a.call)}</a></td>" -->

<?php $tg_a = '${escapeHtml(a.tg || "—")}' ?>
<td><button
 style=cursor:pointer;border-radius:8px;width:85px;<?php echo $svxmqtt_color_active; ?>;font-weight:bold;font-size:14px;
 type=submit
 id=dtmfsvx
 name=dtmfsvx
 class=active_id
 value="${escapeHtml(a.tg || "—")}"><?php echo $tg_a; ?>
</button></td>

<td>&nbsp;${escapeHtml(group || "—")}</td>
<td>${new Date(a.startMs).toLocaleTimeString()}</td>
<td>${durSec}s</td>
`;
                            elActiveBody.appendChild(tr);
                        }
                    }

                    // LastHeard
                    elLHBody.innerHTML = "";
                    for (const h of lastHeard) {
                        const group = tgName(h.tg);
                        const tr = document.createElement("tr");
                        tgConn.textContent = h.tg;
                        tr.innerHTML = `
<td style="color:#0065ff;font-weight:bold;">&nbsp;${escapeHtml(h.call)}</td>

<?php $tg_h = '${escapeHtml(h.tg || "—")}' ?>
<td><button
 style=cursor:pointer;border-radius:8px;width:85px;<?php echo $svxmqtt_color_passive; ?>;font-weight:bold;font-size:14px;
 type=submit
 id=dtmfsvx
 name=dtmfsvx
 class=active_id
 value="${escapeHtml(h.tg || "—")}"><?php echo $tg_h; ?>
</button></td>

<td>&nbsp;${escapeHtml(group || "—")}</td>
<td>${new Date(h.endMs).toLocaleTimeString()}</td>
<td>${h.durationSec}s</td>
`;
                        elLHBody.appendChild(tr);
                    }
                }

                function escapeHtml(s) {
                    return String(s ?? "").replace(/[&<>"]/g, c => ({ '&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;' }[c]));
                }

                // Update durations every second
                setInterval(() => { if (client) render(); }, 1000);

                // TGDB refresh every 10 minutes
                loadTGDB();
                setInterval(loadTGDB, 10 * 60 * 1000);

                // Wire buttons and autoconnect
                document.getElementById("btnTGConn").addEventListener("click", connectTG);
                document.getElementById("btnConnect").addEventListener("click", connect);
                document.getElementById("btnDisconnect").addEventListener("click", disconnect);
                connect();
            </script>
        </form>

        <?php
        // Definition Button TG Wechsel
        if (isset($_POST["dtmfsvx"])) {
            $exec = "echo '*91" . $_POST['dtmfsvx'] . "#' > /tmp/dtmf_svx";	//FM Funknetz TG Link Ausführung
            exec($exec, $output);
            echo "<meta http-equiv='refresh' content='0'>";
        }
        ?>

<center><small><br>FM-Funknetz.de - basierend auf dem MQTT Livedashboard von DJ1JAY (integriert von Frank, DL4EM)</small></center>

</fieldset>
    <!-- SIMPLE Button 
    <center>
    <a href="../index.php"><button class="white"
    style="border:none; border-radius:10px; margin-top:10px; color:white; background-color:#0000ff; height: 35px; width:180px; font-siz>
    button name="button0">simple</button></a>
    </center>
-->
</body>
</html>
