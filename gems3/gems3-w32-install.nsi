; gems300rc0-w32-install.nsi
; requires NullSoft NSIS v.1.98 or later to be installed on your PC!
;
; This script will install GEM-Selektor program package 
; and database into a directory that the user selects

; The name of the installer
Name "GEM-Selektor v3"
Caption "GEM-Selektor Package"
BrandingText "(C) 2000,2010, GEM-Selector Development Team (PSI,ETHZ)"
Icon "C:\GEMS3inst\program\img\gems3.ico"
WindowIcon on

; The file to write
; OutFile "gems-w32-install.exe"
OutFile "gems300rc0-1640-w32-install.exe"

; The default installation directory
InstallDir $PROGRAMFILES\GEMS3
; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM SOFTWARE\GEMS3 "Install_Dir"

;License text
LicenseText "Version to be installed: 3.0.0-PSI r1640/450 $\nLICENSE and DISCLAIMER"
LicenseData "C:\GEMS3inst\program\doc\txt\TermsOfUse.txt"

; The text to prompt the user to enter a directory
ComponentText "The GEM-Selektor v.3.0-PSI package is about to be installed on your PC. (De)select optional components that you want to install, and hit 'Next' to continue."
; The text to prompt the user to enter a directory
DirText "At best, choose something like D:\MyGEMS3 to make finding your \projects easier. Attention: hit 'Install' only after backup of your earlier GEMS installation directory."

; The stuff to install
Section "GEMS3 (required)"
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  ; Put file there
  File "C:\GEMS3inst\readme3.txt"
  File "C:\GEMS3inst\rungems3.bat"
  CreateDirectory "$INSTDIR\program"
  SetOutPath $INSTDIR\program
  Delete "$INSTDIR\program\*.dll"
 ; Put file there
    File "C:\GEMS3inst\program\gems3.exe"
    File "C:\GEMS3inst\program\gems3.ico"
 ; Set the right version of *.dll below
    File "C:\GEMS3inst\program\*.dll"
    File "C:\GEMS3inst\program\assistant.exe"
 ;   File "C:\GEMS3inst\program\assistant_adp.exe"
    File "C:\GEMS3inst\program\readme.txt"
  CreateDirectory "$INSTDIR\program\codecs"
  SetOutPath "$INSTDIR\program\codecs"
    File "C:\GEMS3inst\program\codecs\*.*"
  CreateDirectory "$INSTDIR\program\iconengines"
  SetOutPath "$INSTDIR\program\iconengines"
    File "C:\GEMS3inst\program\iconengines\*.*"
  CreateDirectory "$INSTDIR\program\imageformats"
  SetOutPath "$INSTDIR\program\imageformats"
    File "C:\GEMS3inst\program\imageformats\*.*"
  CreateDirectory "$INSTDIR\program\sqldrivers"
  SetOutPath "$INSTDIR\program\sqldrivers"
    File "C:\GEMS3inst\program\sqldrivers\*.*"
  CreateDirectory "$INSTDIR\program\data"
  SetOutPath "$INSTDIR\program\data"
    File "C:\GEMS3inst\program\data\units.ini"
    File "C:\GEMS3inst\program\data\vis_cn.ini"
    File "C:\GEMS3inst\program\data\vis_od.ini"
    File "C:\GEMS3inst\program\data\viz_cndoc.txt"
  CreateDirectory "$INSTDIR\program\visor.data"
  SetOutPath "$INSTDIR\program\visor.data"
    File "C:\GEMS3inst\program\visor.data\visor.dat"
    File "C:\GEMS3inst\program\visor.data\visobj.dat"
  CreateDirectory "$INSTDIR\program\img"
  SetOutPath $INSTDIR\program\img
   Delete "$INSTDIR\program\img\*.*"
    File "C:\GEMS3inst\program\img\*.*"
  CreateDirectory "$INSTDIR\program\DB.default"
  SetOutPath "$INSTDIR\program\DB.default"
;   Delete "$INSTDIR\program\DB.default\*.*"
    File "C:\GEMS3inst\program\DB.default\*.ndx"
    File "C:\GEMS3inst\program\DB.default\*.pdb"
  CreateDirectory "$INSTDIR\program\doc"
  CreateDirectory "$INSTDIR\program\doc\html"
  CreateDirectory "$INSTDIR\program\doc\pdf"
  CreateDirectory "$INSTDIR\program\doc\txt"
  SetOutPath $INSTDIR\program\doc\txt
   Delete "$INSTDIR\program\doc\txt\*.*" 
    File "C:\GEMS3inst\program\doc\txt\*.txt"
  SetOutPath $INSTDIR\program\doc\html
   Delete "$INSTDIR\program\doc\html\*.*"
    File "C:\GEMS3inst\program\doc\html\*.html"
    File "C:\GEMS3inst\program\doc\html\*.png"
    File "C:\GEMS3inst\program\doc\html\*.q*"
    File "C:\GEMS3inst\program\doc\html\prefDialog.jpg"
  SetOutPath $INSTDIR\program\doc\pdf
   Delete "$INSTDIR\program\doc\pdf\*.*"
    File "C:\GEMS3inst\program\doc\pdf\*.pdf"
  SetOutPath $INSTDIR
  CreateDirectory "$INSTDIR\projects"
  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\GEMS3 "Install_Dir" "$INSTDIR"
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\GEMS3" "DisplayName" "GEMS3 (remove only)"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\GEMS3" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteUninstaller "uninstall.exe"
SectionEnd

; optional section
Section "Start Menu Shortcuts"
  CreateDirectory "$SMPROGRAMS\GEM-Selektor v3-PSI-ETHZ"
  CreateShortCut "$SMPROGRAMS\GEM-Selektor v3-PSI-ETHZ\Uninstall GEMS3.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\GEM-Selektor v3-PSI-ETHZ\Run GEMS3.lnk" "$INSTDIR\program\gems3.exe" '-c -s "$INSTDIR\program" -u "$INSTDIR"' "$INSTDIR\program\img\gems3.ico" 0
  CreateShortCut "$SMPROGRAMS\GEM-Selektor v3-PSI-ETHZ\Run GEMS3 with new projects.lnk" "$INSTDIR\program\gems3.exe" '-d -s "$INSTDIR\program" -u "$INSTDIR"' "$INSTDIR\program\img\gems3.ico" 0
  CreateShortCut "$DESKTOP\Run GEMS3.lnk" "$INSTDIR\program\gems3.exe" '-c -s "$INSTDIR\program" -u "$INSTDIR"' "$INSTDIR\program\img\gems3.ico" 0
SectionEnd

; optional section
Section "Test Modelling Projects"
  ; Set output path to the installation directory.
;  CreateDirectory "$INSTDIR\projects"
  SetOutPath $INSTDIR\projects
  ; Put files here 
ifFileExists "$INSTDIR\projects\*.conf" ConfSkip 0
    File "C:\GEMS3inst\projects\*.conf"
  ; Will only be copied during the first installation!
ConfSkip:
  CreateDirectory "$INSTDIR\projects\TryNPTDB"
  SetOutPath $INSTDIR\projects\TryNPTDB
    File "C:\GEMS3inst\projects\TryNPTDB\*.pdb"
    File "C:\GEMS3inst\projects\TryNPTDB\*.ndx"
  CreateDirectory "$INSTDIR\projects\Kaolinite"
  SetOutPath $INSTDIR\projects\Kaolinite
    File "C:\GEMS3inst\projects\Kaolinite\*.pdb"
    File "C:\GEMS3inst\projects\Kaolinite\*.ndx"
  CreateDirectory "$INSTDIR\projects\AragCalc"
  SetOutPath $INSTDIR\projects\AragCalc
    File "C:\GEMS3inst\projects\AragCalc\*.pdb"
    File "C:\GEMS3inst\projects\AragCalc\*.ndx"
  CreateDirectory "$INSTDIR\projects\Ca-Sr-CO3"
  SetOutPath $INSTDIR\projects\Ca-Sr-CO3
    File "C:\GEMS3inst\projects\Ca-Sr-CO3\*.pdb"
    File "C:\GEMS3inst\projects\Ca-Sr-CO3\*.ndx"
  CreateDirectory "$INSTDIR\projects\Kyanite"
  SetOutPath $INSTDIR\projects\Kyanite
    File "C:\GEMS3inst\projects\Kyanite\*.pdb"
    File "C:\GEMS3inst\projects\Kyanite\*.ndx"
; More test projects to come here!  
MessageBox MB_YESNO "Please, click YES to check if GEMS can start Ok and refresh its configuration files. This is especially recommended if you install GEMS in this directory for the first time." IDNO ExecSkipped 
  Exec '"$INSTDIR\program\gems3.exe" -d -s "$INSTDIR\program" -u "$INSTDIR"'
ExecSkipped:
;
  SetOutPath $INSTDIR
SectionEnd

; uninstall stuff
UninstallText "This will uninstall the GEM-Selektor v.3.0-PSI package. Your own modelling project subdirectories will not be removed. Hit 'Uninstall' to continue."

; special uninstall section.
Section "Uninstall"
; remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\GEMS3"
  DeleteRegKey HKLM SOFTWARE\GEMS3
; remove files
  Delete "$INSTDIR\*.*"
  Delete "$INSTDIR\program\data\*.*"
  Delete "$INSTDIR\program\visor.data\*.*"
  Delete "$INSTDIR\program\img\*.*"
  Delete "$INSTDIR\program\DB.default\*.*"
  Delete "$INSTDIR\program\*.*" 
  Delete "$INSTDIR\program\doc\txt\*.*"
  Delete "$INSTDIR\program\doc\html\*.*"
  Delete "$INSTDIR\program\doc\pdf\*.*"
   Delete "$INSTDIR\program\codecs\*.*" 
   Delete "$INSTDIR\program\iconengines\*.*"
   Delete "$INSTDIR\program\imageformats\*.*"
   Delete "$INSTDIR\program\sqldrivers\*.*"
;
  Delete "$INSTDIR\projects\*.*" 
  Delete "$INSTDIR\projects\TryNPTDB\*.*" 
;  Delete "$INSTDIR\projects\Kaolinite\*.*" 
;  Delete "$INSTDIR\projects\Solvus2\*.*" 
;  Delete "$INSTDIR\projects\AragCalc\*.*"
;  Delete "$INSTDIR\projects\Ca-Sr-CO3\*.*"
;  Delete "$INSTDIR\projects\Kyanite\*.*"
;
; MUST REMOVE UNINSTALLER, too
  Delete $INSTDIR\uninstall.exe
; remove shortcuts, if any.
  Delete "$SMPROGRAMS\GEM-Selektor\*.*"
  Delete "$DESKTOP\Run GEMS3.lnk"
; remove directories used.
  RMDir "$SMPROGRAMS\GEM-Selektor"
  RMDir "$INSTDIR\program\data"
  RMDir "$INSTDIR\program\visor.data"
  RMDir "$INSTDIR\program\img"
  RMDir "$INSTDIR\program\DB.default"
  RMDir "$INSTDIR\program\doc\html"
  RMDir "$INSTDIR\program\doc\pdf"
  RMDir "$INSTDIR\program\doc\txt"
  RMDir "$INSTDIR\program\doc"
   RMDir "$INSTDIR\program\codecs" 
   RMDir "$INSTDIR\program\iconengines"
   RMDir "$INSTDIR\program\imageformats"
   RMDir "$INSTDIR\program\sqldrivers"
  RMDir "$INSTDIR\program" 
;
  RMDir "$INSTDIR\projects\TryNPTDB" 
;  RMDir "$INSTDIR\projects\Kaolinite" 
;  RMDir "$INSTDIR\projects\Solvus2" 
;  RMDir "$INSTDIR\projects\AragCalc" 
;  RMDir "$INSTDIR\projects\Ca-Sr-CO3" 
;  RMDir "$INSTDIR\projects"
;
;  RMDir "$INSTDIR"
SectionEnd

; eof
