echo off

echo Setting up environment for Qt usage - see qtenv2.bat
PATH=C:\Qt\5.12.7\mingw73_64\bin;C:\Qt\Tools\mingw730_64\bin;%PATH%
windeployqt %1
