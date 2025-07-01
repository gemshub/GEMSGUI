rem  Change the path to the actual location of gem-selektor executable and Resources
cd ./Gems3-app/bin

rem 1. First launch with default location of modeling projects (usually done by the installer)
rem gem-selektor.exe -d > gems3.log

rem 2. Normal runs in default locations (retains all settings from previous session)
gem-selektor.exe > gems3.log
rem or 
rem gem-selektor.exe -s . -u C:\Users\<USER>\Library\Gems3 > gems3.log

rem 3. New file configuration if project subfolder(s) were added/removed to/from
rem   /projects or if /projects are not in the default location
rem gem-selektor.exe -c -s <Path_to_Resources> -u G:\My_GEMS_Projects_Location\Gems3 > gems3.log

rem 4. New \data\*.ini files - remake DOD and module dialog configurators
rem Otherwise, does the same as  gems3 -c
rem gem-selektor.exe -d -s . -u C:\Users\<USER>\Library\Gems3 > gems3.log

rem 5. Add -f after gem-selektor.exe for a write access to database files in \DB.default\
rem (for developers only!)

rem 6. Create on desktop a shortcut to ~\Gems3-app\bin\gem-selektor.exe -c
rem   in shortcut, select the icon ~\Gems3-app\bin\gem-selektor.ico
