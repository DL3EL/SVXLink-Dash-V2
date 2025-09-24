#!/bin/bash

# Utility to download the latest DMR database.

cd ~
# use correct directory 
DMRIDFILE="/var/www/html/dl3el/DMRIds.dat"
RADIOIDFILE="/var/www/html/dl3el/radioid.dat"
echo "Downloading DMR database..."
#sudo curl --fail -o ${RADIOIDFILE} 'https://database.radioid.net/static/user.csv'

echo "Converting usr.bin to DMRids.dat (saving space)"
awk -F, '{print $1" "$2" "$3}' $RADIOIDFILE > $DMRIDFILE

sudo chown svxlink:svxlink $DMRIDFILE
echo "fertig"
exit

