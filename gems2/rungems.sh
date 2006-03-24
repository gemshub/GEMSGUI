#!/bin/sh

if [ "`uname`" == "Darwin" ]; then 
exec `pwd`/gems2.app/Contents/MacOS/gems2 $@
else
#exec ./gems2 -d -f -s /home/gems/work/gems2/shared -u  /home/gems/work/gems2/shared
#exec ./gems2 -d -f -s ./shared -u ./shared
exec ./gems2 -c $@
fi

