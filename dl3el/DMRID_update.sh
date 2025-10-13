#!/bin/bash
SCRIPT_DIR=$(dirname "$(realpath $0)")
DMRIDFILE="$SCRIPT_DIR/"DMRIds.dat
RADIOIDFILE="$SCRIPT_DIR/"radioid.dat
cd "$SCRIPT_DIR/"
echo "Downloading DMR database..."
sudo curl --fail -o ${RADIOIDFILE} 'https://database.radioid.net/static/user.csv' 
echo "Converting usr.bin to DMRids.dat (saving space)"
awk -F, '{print $1" "$2" "$3}' $RADIOIDFILE > $DMRIDFILE
sudo chown svxlink:svxlink $DMRIDFILE 
echo "fertig"


