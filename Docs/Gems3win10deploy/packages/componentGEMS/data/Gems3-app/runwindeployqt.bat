echo off
echo Setting up environment for Qt usage - see qtenv2.bat    
PATH=C:\Qt\6.2.2\msvc2019_64\bin;C:\Qt\Tools\CMake_64\bin;%PATH%
C:\Qt\6.2.2\msvc2019_64\bin\windeployqt --release --no-translations --json %1
