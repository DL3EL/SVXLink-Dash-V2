import evdev
import select

# Alle verfügbaren Eingabegeräte öffnen
devices = [evdev.InputDevice(path) for path in evdev.list_devices()]
devs = {dev.fd: dev for dev in devices}

print("Scanner aktiv. Bitte Taste am Mikrofon drücken...")
for d in devices:
    print(f"Prüfe Gerät: {d.name} auf {d.path}")

while True:
    # Wartet auf Input von irgendeinem der Geräte
    r, w, x = select.select(devs, [], [])
    for fd in r:
        for event in devs[fd].read():
            if event.type == 1: # EV_KEY
                print(f"TREFFER! Gerät: {devs[fd].name} | Code: {event.code} | Wert: {event.value}")
