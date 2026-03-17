SCRIPT_DIR=$(dirname "$(realpath $0)")
SOUNDSTATE_save="$SCRIPT_DIR/"asound.state
sudo cp $SOUNDSTATE_save /var/lib/alsa/asound.state
CARD=${1:-Device}
ALSACTL_PATH="/usr/sbin/alsactl"
##sudo cp /home/pi/SVXLink/shari/asound.state.good /var/lib/alsa/asound.state
# sudo alsactl restore Device
sudo alsactl restore "$CARD"
