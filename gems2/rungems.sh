#!/bin/sh

if [ "`uname`" == "Darwin" ]; then 
exec `pwd`/gems2.app/Contents/MacOS/gems2 $@
else
#exec ./gems2 -c  -s ./shared -u ./shared
exec ./gems2 $@
fi

