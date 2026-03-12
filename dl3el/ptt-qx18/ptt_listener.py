import evdev
from evdev import ecodes
import time
import os

# --- KONFIGURATION ---
DEVICE_NAME = "Jieli Technology UACDemoV1.0 Keyboard"
SQL_FILE = "/tmp/sql"
PTT_KEY = ecodes.KEY_F2

def run_listener():
    try:
        devices = [evdev.InputDevice(p) for p in evdev.list_devices()]
        device = next(d for d in devices if d.name == DEVICE_NAME)
        
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
                        else:
                            # Wir schreiben einfach Z und ein Newline
                            os.write(fd, b"Z\n")
                        
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
    
