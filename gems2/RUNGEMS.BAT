echo off
rem Normal runs
rem gems -s c:\gemtutor\program -u c:\gemtutor

rem New file configuration if gems/profile dir changed
rem gems -c -s c:\gemtutor\program -u c:\gemtutor

rem New *.ini files - remake DOD and module dialog configs
rem In addition, does the same as  gems -c
rem gems -d -s c:\gemtutor\program -u c:\gemtutor > gems.out

rem Gives write access to database files in \DB.default\
rem For developers only! Also, does the same as gems -c
gems -f -c -s c:\gemtutor\program -u c:\gemtutor
