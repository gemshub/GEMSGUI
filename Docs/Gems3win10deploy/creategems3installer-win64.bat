echo off
rem Edit the name of installer file to be generated, Gems3.9.z-xxxxxxx.yyyyyyy-win64-qtinstall.exe
rem by replacing xxxxxxx with commit # of gems3gui and yyyyyyy with commit # of gems3k repositories.
rem Then generate the installer by opening a command-line terminal from this directory and executing the following command:
C:\Qt\Tools\QtInstallerFramework\4.2\bin\binarycreator.exe --offline-only -c config/config.xml -p packages Gems3.9.6-2ed66a4.9a8c970-win64-qt6-install.exe

