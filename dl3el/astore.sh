SCRIPT_DIR=$(dirname "$(realpath $0)")
SOUNDSTATE_save="$SCRIPT_DIR/"asound.state
sudo alsactl store Device
sudo cp /var/lib/alsa/asound.state $SOUNDSTATE_save

