#!/bin/bash

# Pfade ermitteln
INSTALL_DIR=$(pwd)
SCRIPT_PATH="$INSTALL_DIR/ptt_listener.py"
SERVICE_FILE="/etc/systemd/system/ptt.service"

echo "--- QX-18 Hardware PTT Installation ---"

# 1. Abhängigkeiten prüfen/installieren
echo "Prüfe Abhängigkeiten..."
sudo apt-get update && sudo apt-get install -y python3-evdev

# 2. Statusdatei vorbereiten
echo "Bereite Statusdatei vor..."
touch "$INSTALL_DIR/ptt_status.txt"
chmod 666 "$INSTALL_DIR/ptt_status.txt"

# 3. Systemd Service Datei erstellen
echo "Erstelle Systemd-Service..."
sudo bash -c "cat <<EOM > $SERVICE_FILE
[Unit]
Description=SvxLink Hardware PTT Listener (QX-18)
After=network.target

[Service]
ExecStart=/usr/bin/python3 $SCRIPT_PATH
WorkingDirectory=$INSTALL_DIR
StandardOutput=inherit
StandardError=inherit
Restart=always
User=root

[Install]
WantedBy=multi-user.target
EOM"

# 4. Dienst aktivieren
echo "Starte Dienst..."
sudo systemctl daemon-reload
sudo systemctl enable ptt.service
sudo systemctl restart ptt.service

echo "---------------------------------------"
echo "Installation abgeschlossen!"
echo "Status prüfen mit: systemctl status ptt.service"
