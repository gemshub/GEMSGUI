echo off
echo Setting up environment for Qt usage - see qtenv2.bat in mingw48 bin subfolder
set PATH=C:\Qt\5.4\mingw491_32\bin;C:\Qt\Tools\mingw491_32\bin;%PATH%
windeployqt %1
