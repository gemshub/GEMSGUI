echo off

echo Setting up environment for Qt usage (MSVC14-Visual Studio 2019 pro) - see qtenv2.bat
rem PATH=C:\Qt\5.15.2\mingw81_64\bin;C:\Qt\Tools\mingw810_64\bin;%PATH%
set VCINSTALLDIR=C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\
PATH=C:\Qt\5.15.2\msvc2015_64\bin;%PATH%
windeployqt %1
