import sys
import evdev
from evdev import ecodes
import time
import os

# --- KONFIGURATION ---
# sudo pkill -f ptt_listener.py
DEVICE_NAME = "Jieli Technology UACDemoV1.0 Keyboard"
DEFAULT_DEVICE_PART = "UACDemoV1.0 K"
DEVICE_PART = sys.argv[1] if len(sys.argv) > 1 else DEFAULT_DEVICE_PART
SQL_FILE = "/tmp/sql"
PTT_KEY = ecodes.KEY_F2

def run_listener():
    try:
        devices = [evdev.InputDevice(p) for p in evdev.list_devices()]
#        print(f"Gefundene Geräte: {len(devices)}", flush=True)
        
#        for d in devices:
#            print(f"Gerät im System: {d.name}", flush=True)
#        device = next(d for d in devices if d.name == DEVICE_NAME)
        device = next((d for d in devices if DEVICE_PART in d.name), None)

        
        device.grab()
        print(f"Verbunden: {device.name}")

        last_state = None

        for event in device.read_loop():
            if event.type == ecodes.EV_KEY and event.code == PTT_KEY:
                if event.value == 1:
                    new_val = "O"
                elif event.value == 0:
                    new_val = "Z"
                else:
                    continue

                if new_val != last_state:
                    try:
                        # Wir öffnen ohne O_SYNC, da PTYs das nicht können
                        fd = os.open(SQL_FILE, os.O_WRONLY | os.O_NOCTTY | os.O_NONBLOCK)
                        
                        if new_val == "O":
                            os.write(fd, b"O\n")
                            with open("/tmp/ptt_status.txt", "w") as f: f.write("ON")
                        else:
                            # Wir schreiben einfach Z und ein Newline
                            os.write(fd, b"Z\n")
                            with open("/tmp/ptt_status.txt", "w") as f: f.write("OFF")
                        
                        os.close(fd)
                        print(f"PTT -> {new_val}")
                        
                    except Exception as e:
                        print(f"PTY-Fehler: {e}")
                    
                    last_state = new_val

    except StopIteration:
        print(f"Gerät nicht gefunden...")
        time.sleep(5)
        run_listener()
    except Exception as e:
        print(f"Kritischer Fehler: {e}")
        time.sleep(2)
        run_listener()

if __name__ == "__main__":
    print("--- SvxLink PTT Controller (Clean Version) ---")
    run_listener()
    
