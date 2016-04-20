rem  Change the path to the actual location of GEMS3 executable and Resources
cd C:\GEMS3MOD\Gems3-app

rem 1. First launch with default location of modeling projects (usually done by the installer)
rem gems3.exe -d > gems3.log

rem 2. Normal runs in default locations (retains all settings from previous session)
gems3.exe -s . -u ..\Library\Gems3 > gems3.log
rem or 
rem gems3.exe > gems3.log

rem 2a. The above is equivalent to
rem gems3.exe -s C:\GEMS3MOD\Gems3-app -u ..\Library\Gems3 > gems3.log

rem 3. New file configuration if project subfolder(s) were added/removed to/from
rem   /projects or if /projects are not in the default location
rem gems3.exe -c -s . -u G:\My_GEMS_Projects_Location\Gems3 > gems3.log

rem 4. New \data\*.ini files - remake DOD and module dialog configurators
rem Otherwise, does the same as  gems3 -c
rem gems3.exe -d -s . -u ..\Library\Gems3 > gems3.log

rem 5. Add -f after gems3.exe for a write access to database files in \DB.default\    
rem (for developers only!)

rem 6. Create on desktop a shortcut to C:\GEMS3MOD\Gems3-app\gems3.exe -c
rem   in shortcut, select the icon C:\GEMS3MOD\Gems3-app\Resources\Gems3.ico