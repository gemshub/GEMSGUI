rem  Change the path to the actual location of gem-selektor executable and Resources
cd ./Gems3-app/bin
set PATH=%CD%

rem Force Qt to use the platform plugin bundled with this app (Gems3-app\lib\qt6\plugins\platforms
rem - the same lib\qt6\plugins layout the Linux build uses). Without this, a
rem QT_QPA_PLATFORM_PLUGIN_PATH or QT_PLUGIN_PATH already set on this machine (e.g. left over from
rem an unrelated Anaconda/PyQt/Qt Creator install) can override Qt's normal bundled-app lookup, and
rem gem-selektor.exe aborts immediately with:
rem   qt.qpa.plugin: Could not find the Qt platform plugin "windows" in ""
set "QT_QPA_PLATFORM_PLUGIN_PATH=%CD%\..\lib\qt6\plugins\platforms"
set "QT_PLUGIN_PATH=%CD%\..\lib\qt6\plugins"

rem 1. First launch with default location of modeling projects (usually done by the installer)
rem gem-selektor.exe -d > gems3.log

rem 2. Normal runs in default locations (retains all settings from previous session)
rem gem-selektor.exe  > gems3.log
rem or
rem gem-selektor.exe -s . -u C:\Users\<USER>\Library\Gems3 > gems3.log

rem 3. New file configuration if project subfolder(s) were added/removed to/from
rem   /projects or if /projects are not in the default location
gem-selektor.exe -c > gems3.log
rem or
rem gem-selektor.exe -c -s <Path_to_Resources> -u G:\My_GEMS_Projects_Location\Gems3 > gems3.log

rem 4. New \data\*.ini files - remake DOD and module dialog configurators
rem Otherwise, does the same as  gems3 -c
rem gem-selektor.exe -d -s . -u C:\Users\<USER>\Library\Gems3 > gems3.log

rem 5. Add -f after gem-selektor.exe for a write access to database files in \DB.default\
rem (for developers only!)

rem 6. Create on desktop a shortcut

@echo off
setlocal
set "scriptPath=%~dp0"
rem Shortcuts target rungems3.bat itself (not gem-selektor.exe directly) so that every
rem launch - not just the very first one - goes through the QT_QPA_PLATFORM_PLUGIN_PATH fix
rem above. Pointing a shortcut straight at the exe would skip that fix and could reproduce
rem the "Could not find the Qt platform plugin" crash on machines with a conflicting Qt env var.
set "targetPath=%scriptPath%rungems3.bat"
set "workingDir=%scriptPath%"
set "exePath=%scriptPath%Gems3-app\bin\gem-selektor.exe"
set "iconPath=%scriptPath%Gems3-app\bin\gem-selektor.ico"
set "startMenuShortcut=%APPDATA%\Microsoft\Windows\Start Menu\Programs\gems-selektor.lnk"

REM Write the helper VBScript to %TEMP%, not the app's own folder: the app can be installed
REM somewhere read-only (e.g. under Program Files), and %TEMP% is always per-user writable.
set "vbsPath=%TEMP%\gemsgui-create-shortcut.vbs"

REM Check if required files exist
IF NOT EXIST "%exePath%" (
    echo ERROR: Executable not found: "%exePath%"
    exit /b
)

IF NOT EXIST "%iconPath%" (
    echo ERROR: Icon file not found: "%iconPath%"
    exit /b
)

REM Create Start Menu shortcut
echo Creating Start Menu shortcut...
echo Set oWS = CreateObject("WScript.Shell") > "%vbsPath%"
echo Set oLink = oWS.CreateShortcut("%startMenuShortcut%") >> "%vbsPath%"
echo oLink.TargetPath = "%targetPath%" >> "%vbsPath%"
echo oLink.WorkingDirectory = "%workingDir%" >> "%vbsPath%"
echo oLink.IconLocation = "%iconPath%" >> "%vbsPath%"
echo oLink.Save >> "%vbsPath%"
cscript //nologo "%vbsPath%"
set "cscriptRC=%ERRORLEVEL%"
del "%vbsPath%" >nul 2>&1
IF "%cscriptRC%"=="0" (
    echo Shortcut created successfully in Start Menu.
) ELSE (
    echo WARNING: Could not create the Start Menu shortcut - Windows Script Host may be disabled
    echo or blocked by policy on this machine. You can still start GEM-Selektor by running
    echo rungems3.bat directly, or by right-clicking rungems3.bat and choosing "Pin to Start"/
    echo "Pin to taskbar".
)

REM Create Desktop shortcut
echo Creating Desktop shortcut...
echo Set oWS = CreateObject("WScript.Shell") > "%vbsPath%"
echo strDesktop = oWS.SpecialFolders("Desktop") >> "%vbsPath%"
echo Set oLink = oWS.CreateShortcut(strDesktop ^& "\gems-selektor.lnk") >> "%vbsPath%"
echo oLink.TargetPath = "%targetPath%" >> "%vbsPath%"
echo oLink.WorkingDirectory = "%workingDir%" >> "%vbsPath%"
echo oLink.IconLocation = "%iconPath%" >> "%vbsPath%"
echo oLink.Save >> "%vbsPath%"
cscript //nologo "%vbsPath%"
set "cscriptRC=%ERRORLEVEL%"
del "%vbsPath%" >nul 2>&1
IF "%cscriptRC%"=="0" (
    echo Shortcut created successfully on Desktop.
) ELSE (
    echo WARNING: Could not create the Desktop shortcut - Windows Script Host may be disabled
    echo or blocked by policy on this machine. You can still start GEM-Selektor by running
    echo rungems3.bat directly.
)

endlocal
