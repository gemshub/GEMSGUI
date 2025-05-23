echo off
rem  Change the path to the actual location of GEMS3 executable and Resources
rem cd .\Gems3-app

rem First launch with default location of modeling projects
 .\Gems3-app\gems3.exe -d > gems3.log 

rem Normal runs in default locations
rem gems3.exe -s . -u ..\Library\Gems3 > gems3.log

rem New file configuration if project subfolder(s) were added/removed to/from 
rem   /projects that are not in the default location 
rem gems3.exe -c -s . -u c:\OtherLocation\gems3test > gems3.log

rem New \data\*.ini files - remake DOD and module dialog configurators
rem Otherwise, does the same as  gems3 -c
rem gems3.exe -d -s . -u ..\Library\Gems3 > gems3.log

rem Add -f after gems3.exe for a write access to database files in \DB.default\     
rem (for developers only!) 

rem Create on desktop a shortcut to C:\GEMS340\Gems3-app\gems3.exe -c 
rem   in shortcut, select the icon C:\GEMS340\Gems3-app\Resources\Gems3.ico