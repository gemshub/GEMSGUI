echo off

echo Setting up environment for Qt usage - see qtenv2.bat
PATH=C:\Qt\5.11.2\mingw53_32\bin;C:\Qt\Tools\mingw530_32\bin;%PATH%
windeployqt %1
