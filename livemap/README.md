=======================================
== FM-Funknetz LiveMap für Dashboard ==
=======================================
// Version 20260914

Zweck:
------
Diese README dokumentiert die aktuelle Logik der FM-Funknetz LiveMap im Verzeichnis /var/www/html/TEST/livemap/.
Die Dokumentation beschreibt insbesondere die Logik für aktive Sender, aktive Talkgroups (TG), Empfänger, TG-Linien, Positionsdaten und den Umgang mit fehlenden Positionsdaten.

1. Verzeichnisstruktur:
-----------------------
Im Verzeichnis livemap müssen folgende Datein vorhanden sein:

	LiveMap
	   ├── 
       ├── README.md		Dieses Readme
       ├── fm-map-data.php	MQTT Positions-Daten vom Server holen
	   ├── fm-map.js		MQTT Daten Aufbereitung		
	   ├── fm-set-tg.php	TG Wechsel
	   ├── index.php		LiveMap Darstellung als Web Seite
	   ├── livemap.css		Farbdefinitionen für Kartendarstellung
	   ├── livemap.js		LiveMap Darstellung erstellen
	   └── livemap_ini.php	Benutzer definierte Farben

2. Grundprinzip:
----------------
Die LiveMap stellt den aktuellen Funkbetrieb auf einer Karte dar:
Aktive Sender → rote Marker
Empfänger → grüne Marker
TG-Linien → Verbindung vom Sender zu seinen Empfängern
Die LiveMap bildet die aktuellen MQTT-Zustände ab.

Zusätzliche Funktionen sind:
----------------------------
Ein Klick auf einen Marker für Sender oder Empfäner (rot oder grün) öffnet ein PopUp
	Ein Klick auf das PopUp führt zu einem TG Wechsel.
	Das PopUp schließt, wenn der Marker erlischt, kann mit Klick auf das X oben rechts 
	manuel geschlossen werden.

Eine Liste der "Aktiven Stationen"
	Ein Klick auf das Rufzeichen in der Liste führt zum TG Wechsel.
	Ein Klick auf die TG führt zum TG Wechsel.

3. MQTT-Datenquellen:
---------------------
Verwendete MQTT-Pfade:
/server/statethr/1
/server/state/activeTG/+
/server/statethr/1

Meldungen enthalten unter anderem:
	time
	talk
	call
	tg
	server

Bei talk = start wird der Sender als aktiv übernommen.
Bei talk = stop wird der Sender aus dem aktiven Zustand entfernt.

Beispiel:
start DB0AM-L TG: 263803
stop  DB0AM-L TG: 263803

4. Aktive Sender:
-------------------
Aktive Sender werden anhand der MQTT-Zustände geführt.
Intern werden unter anderem gespeichert:
	call
	tg
	startMs

Bei Neustart oder Verlassen der LiveMap (STRG + F5) werden alle gespeicherten Werte gelöscht.

Das Rufzeichen ist der eindeutige Schlüssel.
Ein Sender mit gültiger Position wird auf der Karte als roter Marker dargestellt.
In der linken Senderliste „Aktive Stationen“ erscheint das Rufzeichen und die zugehörige Talkgroup (TG). 
Die Rufzeichen werden aktuell schwarz dargestellt.
Die TGs werden wie ein Button farbig dargestellt. Die Farben entsprechen den aktuellen Linienfarben 
in der Karte. 
Die Farben werden aus einer Liste von 12 Farben erzeugt. In einer Reihenfolge:
	erste aktive TG → erste Farbe
	zweite aktive TG → zweite Farbe
	usw. 

5. Sender ohne Positionsdaten:
--------------------------------
Ein aktiver Sender ohne gültige Position wird nicht aus der Senderliste entfernt.
Das ist absichtlich so: Ein fehlender Positionsdatensatz bedeutet nicht automatisch, 
dass der Sender nicht aktiv ist.
Solche Sender erhalten in der Senderiste ein Warnsymbol. (gelbes Dreieck mit Ausrufezeichen)

Hinweis für den Benutzer:
-------------------------
Keine Positionsdaten für diesen Sender → Die Empfänger dieses Senders werden auf der Karte 
nicht angezeigt.

6. Entscheidende Regel für Empfänger:
-------------------------------------
Die wichtigste Regel der aktuellen LiveMap lautet:
Liefert ein aktiver Sender keine gültige Position, werden auch die von diesem Sender 
abgeleiteten Empfänger nicht auf der Karte angezeigt.

Ein Empfänger darf nur angezeigt werden, wenn:
	der Sender aktuell aktiv ist,
	der Sender einer aktiven TG zugeordnet ist,
	für diese TG Empfänger vorhanden sind,
	der Sender selbst eine gültige Position besitzt.

Beispiel:
	Sender: DB0QH
	TG:       26349
	Position: nicht vorhanden
Auch wenn activeTG/26349 Empfänger enthält, werden diese Empfänger nicht als grüne Marker
in der Karte angezeigt!!!

7. Warum diese Regel notwendig ist:
-----------------------------------
Die TG-Linie benötigt eine gültige Position des Senders.
Ohne Senderposition kann keine eindeutige Verbindung
Sender → Empfänger gezeichnet werden.

8. Empfänger-Synchronisierung:
------------------------------
Empfänger werden aus den aktuell aktiven TG-Zuständen abgeleitet.
Bei jeder Aktualisierung wird geprüft:
	Welche Empfänger müssen vorhanden sein?
	Welche vorhandenen Marker sind noch gültig?
	Welche Marker müssen neu angelegt werden?
	Welche Marker müssen entfernt werden?
	
Dadurch werden unnötige Marker-Neuanlagen vermieden.

9. TG-Linien:
-------------
Für aktive TG-Verbindungen werden Linien zwischen Sender und Empfänger angezeigt, 
sofern die TG-Netzwerkdarstellung aktiviert ist. Die Farben der Linien können in der 
Datei: livemap_ini.php angepasst werden. Dabei ist darauf zu achten, dass die PHP Syntax
genau eingehalten wird und Farben im HEX Format #ffff00 z.B. für gelb bestimmt werden.
Fehlt die Datei livemap_ini.php oder ist sie syntaktisch fehlerhaft werden Default-Farben angezeigt,
damit die LiveMap funktionstüchtig bleibt. In der Datei ist eine Anleitung enthalten zur
zur erfolgreichen Ändereung von Linien- und Markerfarben (default rot und grün).

	Grundstruktur:
	--------------
	aktiver Sender
		│
		└── aktive TG
             │
             ├── Empfänger 1
             ├── Empfänger 2
			 ...
             └── Empfänger x

Die Linie wird nur erzeugt, wenn die Position des Senders bekannt ist.
Damit stimmen Empfänger- und Linienlogik überein.

Default Farben der Linien und Marker:
-------------------------------------
Marker:
-------
Sender:   $LIVEMAP_TALKER_COLOR = "#ff0000"; //rot
Empfänger:  $LIVEMAP_RECEIVER_COLOR = "#3cb371"; //grün

Linien:
-------
$LIVEMAP_COLORS = [
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

10. PopUp:
---------
Bei einem Klick auf einen Marker in der Karte werden Informationen angezeigt bestehend aus:
	Call
	TG
	Hinweis zu TG Wechsel per Klick
	
Ein Klick in das PopUp führt zum TG Wechsel.

11. Checkbox FM Funknetz TG-Netzwerk ein-/ausschalten:
-----------------------------------------------------
Die Checkbox "Darstellung" steuert die Darstellung des TG-Netzwerks.
	Beim Ausschalten werden Empfänger-Marker und TG-Linien der Netzwerkdarstellung entfernt.
	Beim Einschalten wird der aktuelle Zustand neu aufgebaut.
		Alte TG-Linien dürfen dabei nicht zurückbleiben.

12. MQTT-Verbindungsabbruch:
----------------------------
Bei einem MQTT-close wird der komplette LiveMap-Zustand zurückgesetzt.
Dabei werden unter anderem gelöscht:
	active
	activeTG
	talkerMarkers
	receiverMarkers
	tgLines

Anschließend wird die Senderliste neu aufgebaut.
Damit verhindert die LiveMap, dass nach einem Verbindungsabbruch veraltete Zustände weiter 
angezeigt werden. Nach einer erneuten Verbindung kann der aktuelle Zustand wieder aus den 
MQTT-Meldungen aufgebaut werden.

13. Kein aggressives Zeit-Timeout:
----------------------------------
Die LiveMap verwendet kein kurzes Timeout, um aktive Sender nach beispielsweise 100 Sekunden 
automatisch zu löschen. Das wurde bewusst so entschieden.

Ein längeres QSO kann deutlich länger dauern. Ein willkürliches Timeout könnte deshalb 
einen noch laufenden Sender fälschlicherweise entfernen.
Maßgeblich sind grundsätzlich die MQTT-Zustände:
	start
	stop
	
14. Typische Zustände:
--------------------------
|  Sender mit Position |
--------------------------
MQTT start
    ↓
Sender aktiv
    ↓
Position vorhanden
    ↓
roter Sender-Marker
    ↓
aktive TG vorhanden
    ↓
Empfänger vorhanden
    ↓
grüne Empfänger-Marker
    ↓
TG-Linien möglich

--------------------------
| Sender ohne Position |
--------------------------
MQTT start
    ↓
Sender aktiv
    ↓
keine Position
    ↓
Sender bleibt in der Liste
    ↓
Warnsymbol
    ↓
keine Empfänger-Marker dieses Senders
    ↓
keine TG-Linie dieses Senders

-------------------
| Sender stoppt |
-------------------
MQTT stop
    ↓
Sender aus active entfernen
    ↓
Sender-Marker entfernen
    ↓
abgeleitete Empfänger aktualisieren
    ↓
zugehörige TG-Linien entfernen


15. Diagnose und Fehlersuche:
-----------------------------
Wichtige Meldungen in der Browser-Konsole sind unter anderem:
	START:
	STOP:
		für Senderzustände.

	Empfänger-Sync:
		für die Empfänger-Synchronisierung.

	RX DIAG - URSPRUNG:
		zeigt, aus welchem aktiven Sender/TG-Zusammenhang ein Empfänger abgeleitet wurde.

Die Schlüsselwörter z.B. "STOP:" können zur Filterung der Bowser-Console Ausgabe genutzt werden.

Beispiel:
Eingabe "RX DIAG - URSPRUNG:" im Filter der Bowser-Console führt zu fogender Ausgabe:
	RX DIAG - URSPRUNG: DB1VW-L
	MarkerTG: 26349
	Quellen: [{"speaker":"DB0QH","tg":"26349","speakerPosition":false}]

Hier ist erkennbar:
DB0QH
  └── TG 26349
       └── DB1VW-L
aber:
DB0QH → liefert keine Position ("speakerPosition":false)
Nach der aktuellen Logik: 
	DB1VW-L wird nicht als grüner Empfänger-Marker angezeigt.
	DB0QH wird nicht als roter Sender-Marker angezeigt.
	TG Linien werden nicht angezeigt.
	DB0QH wird mit Warnsymbol in der Liste "Aktive Stationen" angezeigt.
	TG Wechsel ist in der Liste "Aktive Stationen" möglich.

16. Ergebnis der aktuellen Logik:
---------------------------------
Die Karte folgt einer klaren Regel:
	Keine Senderposition → keine daraus abgeleiteten Empfänger auf der Karte.
	Gleichzeitig bleibt der Sender für den Benutzer sichtbar:
	Aktiv, aber ohne Positionsdaten → Senderliste + Warnsymbol.
	
Damit entstehen keine scheinbar zufälligen oder verwaisten grünen Marker.

17. LiveMap-Abgrenzung:
--------------------
Alle in dieser README beschriebenen Aspekte beziehen sich ausschließlich auf:
/var/www/html/<Dashboard>/livemap/
und darin enthaltene Dateien.

Verschiedene Anwendungen wie weitere Dashboards usw. können denselben MQTT-Broker 
bzw. MQTT-Datenstrom verwenden, sind aber hinsichtlich ihrer Dashboard- und 
Darstellungslogik getrennt.
Änderungen an der LiveMap dürfen keine Änderungen an anderen Anwendungen 
voraussetzen oder beeinflussen.

18. Aktueller Referenzstand:
----------------------------
Dieser Stand sollte als funktionierender Referenzstand betrachtet werden.
Vor zukünftigen Änderungen empfiehlt sich:
	aktuelle Dateien sichern,
	Änderungen möglichst klein halten,
	Browser-Konsole beobachten,
	Senderwechsel und TG-Wechsel testen,
	Empfänger auf verwaiste Marker prüfen,
	MQTT-Verbindungsabbruch und Wiederverbindung testen.

Besonders wichtig ist, dass die zentrale Regel erhalten bleibt:
----
Sender aktiv
+
Senderposition vorhanden
+
aktive TG
+
Empfänger vorhanden
=
Empfänger darf angezeigt werden

----
Ohne Senderposition:
Sender aktiv
+
keine Senderposition
=
keine Empfänger-Marker

19. Kurzfassung:
----------------
MQTT bestimmt den Funkzustand.
Positionsdaten bestimmen, ob dieser Zustand auf der Karte dargestellt werden kann.
Ein Sender ohne Position bleibt in der Liste sichtbar, aber seine Empfänger 
werden nicht angezeigt.
Damit ist die Darstellung der LiveMap logisch konsistent und verwaisten 
Empfänger-Marker werden vermieden.