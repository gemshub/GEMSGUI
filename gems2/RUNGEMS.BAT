echo off
rem Normal runs
rem gems2 -s c:\gems2test\program -u c:\gem2test

rem New file configuration if gems/profile dir changed
rem gems2 -c -s c:\gems2test\program -u c:\gems2test

rem New *.ini files - remake DOD and module dialog configs
rem In addition, does the same as  gems -c
rem gems2 -d -s c:\gems2test\program -u c:\gems2test > gems2.out

rem Gives write access to database files in \DB.default\
rem For developers only! Also, does the same as gems -c
rem gems2 -f -c -s c:\gems2test\program -u c:\gems2test > gems.out 2> gems.err
rem gems2 -c -s c:\gems2work\program -u c:\gems2work > gems.out 2> gems.err
rem gems2 -c -s c:\gems2test\program -u c:\gems2test > gems.out 2> gems.err
rem gems2.exe -d -s e:\gems2-try\program -u e:\gems2-try > gems.log 
 cd c:\gems2test_dt
 c:\gems2\gems2.exe -c -s c:\gems2test_dt\program -u c:\gems2test_dt > gems.log
rem cd c:\gems2test_dt 
rem c:\gems2\gems2.exe -d -s c:\gems2test_dt\program -u c:\gems2test_dt > gems.log