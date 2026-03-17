SCRIPT_DIR=$(dirname "$(realpath $0)")
SOUNDSTATE_save="$SCRIPT_DIR/"asound.state
# Wenn $1 leer ist, wird "Device" verwendet
CARD=${1:-Device}
ALSACTL_PATH="/usr/sbin/alsactl"
sudo alsactl store "$CARD"
# sudo alsactl store Device
sudo cp /var/lib/alsa/asound.state $SOUNDSTATE_save

