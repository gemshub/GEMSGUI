#!/bin/sh

if [ "`uname`" == "Darwin" ]; then 
exec `pwd`/gems2.app/Contents/MacOS/gems2 $@
else
#exec ./gems2 -c -s ~/Gems2Data/program -u ~/Gems2Data
exec ./gems2 -c $@
fi

