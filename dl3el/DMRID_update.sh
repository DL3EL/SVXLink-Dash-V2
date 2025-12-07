#!/bin/bash
SCRIPT_DIR=$(dirname "$(realpath $0)")
DMRIDFILE="$SCRIPT_DIR/"DMRIds.dat1
DMRIDFILEok="$SCRIPT_DIR/"DMRIds.dat
RADIOIDFILE="$SCRIPT_DIR/"radioid.dat
cd "$SCRIPT_DIR/"
echo "Downloading DMR database..."
curl --fail -o ${RADIOIDFILE} 'https://database.radioid.net/static/user.csv' 
echo "Converting usr.bin to DMRids.dat (saving space)"
awk -F, '{print $1" "$2" "$3}' $RADIOIDFILE > $DMRIDFILE
chown svxlink:svxlink $DMRIDFILE 
rm $DMRIDFILEok 
mv $DMRIDFILE $DMRIDFILEok 
echo "fertig"


