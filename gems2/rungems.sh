#!/bin/sh

if [ "`uname`" == "Darwin" ]; then 
exec `pwd`/gems2.app/Contents/MacOS/gems2 -s ~/Gems2Lin/program -u ~/Gems2Lin 
else
exec ./gems2 -s ~/Gems2Lin/program -u ~/Gems2Lin
fi

