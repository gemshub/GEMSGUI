echo off
rem Normal runs
rem gems2 -s c:\gems2test\program -u c:\gem2test

rem New file configuration if gems/profile dir changed
rem gems2 -c -s c:\gems2test\program -u c:\gems2test

rem New *.ini files - remake DOD and module dialog configs
rem In addition, does the same as  gems -c
gems2 -d -s c:\gemsetup\program -u c:\gemsetup > gems.out

rem Gives write access to database files in \DB.default\
rem For developers only! Also, does the same as gems -c
rem gems2 -f -c -s c:\gems2test\program -u c:\gems2test

