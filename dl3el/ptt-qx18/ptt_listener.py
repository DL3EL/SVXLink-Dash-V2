import evdev
from evdev import ecodes
import requests
import time

# Konfiguration
DEVICE_NAME = "Jieli Technology UACDemoV1.0 Keyboard"
URL = "http://localhost/executePTT.php"

def find_device():
    while True:
        try:
            devices = [evdev.InputDevice(p) for p in evdev.list_devices()]
            for d in devices:
                if DEVICE_NAME == d.name:
                    return d
        except Exception:
            pass
        time.sleep(2)

def run_listener():
    try:
        device = find_device()
        print(f"PTT Modus (Tastbetrieb) aktiv auf: {device.name}")
        
        # Exklusiv greifen, um F2-Eingaben im System zu unterdrücken
        try:
            device.grab()
        except:
            pass

        for event in device.read_loop():
            if event.type == ecodes.EV_KEY and event.code == 60: # 60 = F2
                
                if event.value == 1: # TASTE GEDRÜCKT
                    print("PTT AKTIV (Senden...)")
                    try:
                        with open("ptt_status.txt", "w") as f: f.write("ON")
                        requests.post(URL, data={'command': 'first'}, timeout=1)
                    except Exception as e:
                        print(f"Fehler: {e}")
                
                elif event.value == 0: # TASTE LOSGELASSEN
                    print("PTT AUS (Hören...)")
                    try:
                        with open("ptt_status.txt", "w") as f: f.write("OFF")
                        requests.post(URL, data={'command': 'second'}, timeout=1)
                    except Exception as e:
                        print(f"Fehler: {e}")
                
                # event.value == 2 (Repeat) wird einfach ignoriert

    except Exception as e:
        print(f"Verbindung verloren: {e}")
        time.sleep(2)

if __name__ == "__main__":
    while True:
        run_listener()
        
