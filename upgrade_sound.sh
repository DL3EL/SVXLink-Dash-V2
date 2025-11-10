#!/bin/bash
# This script is used to upgrade the system for the Permissions required for file handling.

# Define the sudoers file, the source file, the script file, and the config file
SUDOERS_FILE="/etc/sudoers.d/svxlink"
SOURCE_FILE="www-data.sudoers"
SCRIPT_FILE=$(basename "$0")
CONFIG_FILE="include/config.inc.php"
# Function to display an info message using whiptail
show_info() {
  whiptail --title "Information" --msgbox "$1" 8 78
}



# Check if the source file exists
if [ ! -f "$SOURCE_FILE" ]; then
  whiptail --title "Error" --msgbox "Source file $SOURCE_FILE does not exist. Exiting." 8 78
  exit 1
fi

# ==============================
# Node.js, npm, and svxlink-node.service setup
# ==============================
CURRENT_USER=$(whoami)
show_info "working as user $CURRENT_USER"

# Check if Node.js is installed for the pi user
# looks, that it doesn't work, skip the user, look for everyone
# if ! sudo -u "$CURRENT_USER" command -v node >/dev/null 2>&1; then
if ! command -v node >/dev/null 2>&1; then
    show_info "Node.js not found. Installing Node.js and npm as $CURRENT_USER..."
    sudo apt update
    sudo apt install -y nodejs npm
else
    show_info "Node.js is already installed: $(node -v)"
fi

# Check if npm is installed for the pi user
CURRENT_USER=$(whoami)
if ! command -v npm >/dev/null 2>&1; then
    show_info "npm not found. Installing npm "
    sudo apt install -y npm
else
    show_info "npm is already installed: $(sudo -u $CURRENT_USER npm -v)"
fi

# Ensure webserver user (svxlink) can run npm-installed scripts if needed
sudo chown -R $CURRENT_USER:$CURRENT_USER /home/$CURRENT_USER/.npm*
sudo chmod -R 755 /home/$CURRENT_USER/.npm-global

# Ensure ws module is installed for svxlink user in scripts folder
# SCRIPT_DIR="/var/www/html/FM-Funknetz/scripts"
SCRIPT_DIR=$(pwd)/scripts
if [ ! -d "$SCRIPT_DIR/node_modules/ws" ]; then
    show_info "Installing ws Node module for svxlink user..."
    sudo -u svxlink bash -c "cd $SCRIPT_DIR && npm install ws"
else
    show_info "Node module ws is already installed."
fi

# Create the systemd service only if it doesn't exist
SERVICE_FILE="/etc/systemd/system/svxlink-node.service"
if [ ! -f "$SERVICE_FILE" ]; then
    show_info "Creating svxlink-node.service..."
    sudo tee "$SERVICE_FILE" > /dev/null <<EOL
[Unit]
Description=SVXLink Node.js Server
After=network.target

[Service]
# Send logs directly to journald instead of syslog or files
StandardOutput=journal
StandardError=journal

# Ensure service restarts even after journal restarts or SIGHUPs
Restart=always
RestartSec=5

# Allow clean reloads (optional, useful if you add reload scripts later)
ExecReload=/bin/kill -HUP $MAINPID

# Give the process a few seconds to shut down gracefully
TimeoutStopSec=10
Type=simple
User=svxlink
Group=svxlink
ExecStart=/usr/bin/node $SCRIPT_DIR/server.js
Environment=NODE_ENV=production

[Install]
WantedBy=multi-user.target
EOL

    show_info "Reloading systemd, enabling, and starting svxlink-node.service..."
    sudo systemctl daemon-reload
    sudo systemctl enable svxlink-node.service
    sudo systemctl start svxlink-node.service
else
    show_info "svxlink-node.service already exists."
    # Optional: restart it to ensure it's running
    sudo systemctl restart svxlink-node.service
fi

# Verify service status
sudo systemctl is-active --quiet svxlink-node.service && show_info "svxlink-node.service is running." || show_info "svxlink-node.service is not running!"

