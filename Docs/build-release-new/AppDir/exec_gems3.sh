#!/bin/bash
appname=gems3
dirname=`dirname $0`
if [ "${dirname:0:1}" != "/" ]; then
   dirname=$PWD/$dirname
fi
export LD_LIBRARY_PATH=$dirname/libs
export QT_QPA_FONTDIR=$dirname/fonts
cd $dirname
./usr/bin/$appname $*
