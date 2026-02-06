#!/bin/bash
SCRIPT_DIR=$(dirname "$(realpath $0)")
DMRIDFILE="$SCRIPT_DIR/"DMRIds.dat1
DMRIDFILEok="$SCRIPT_DIR/"DMRIds.dat
RADIOIDFILE="$SCRIPT_DIR/"radioid.dat
cd "$SCRIPT_DIR/"
echo "Downloading DMR database..."
curl --fail -o ${DMRIDFILE} 'http://192.68.17.16/md380/UserInfo/user.fm' 
chown svxlink:svxlink $DMRIDFILE 
rm $DMRIDFILEok 
mv $DMRIDFILE $DMRIDFILEok 
echo "fertig"


