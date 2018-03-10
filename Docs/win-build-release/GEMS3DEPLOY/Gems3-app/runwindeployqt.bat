echo off
echo Setting up environment for Qt usage - see qtenv2.bat in mingw48 bin subfolder
set PATH=C:\Qt\5.10.1\mingw53_32\bin;C:\Qt\Tools\mingw530_32\bin;%PATH%
windeployqt %1
