echo off
echo Setting up environment for Qt usage - see qtenv2.bat in mingw48 bin subfolder
set PATH=C:\Qt5\5.3\mingw482_32\bin;C:\Qt5\Tools\mingw482_32\bin;%PATH%
windeployqt %1
