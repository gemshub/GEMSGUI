Welcome to PM2SEL  v.0.11a  utility program!


General
-------

PM2SEL (PMatch - to  SELektor-A) is an interactive utility program. 
It is intended to facilitate data conversions between thermodynamic 
databases such as NAGRA (Pearson and others, 1991, 1992) maintained 
by the PMATCH program (Pearson and others, 1989, 1993), and Selektor-A
a program for thermodynamic modeling by Gibbs energy minimization
(Kulik and others, 1997).


1.  	System requirements

PM2SEL v.0.11a  will work normally on a PC 386SX or higher, with at 
least 2 Mbyte of extended RAM, EGA or standard VGA monitor, at least
1 MByte space on hard disk, under MS-DOS version 5.0 or higher, or 
under DOS mode of Windows-95.  This version of PM2SEL can not use 
mouse or other pointing devices.


2.  	Installation

PM2SEL v.0.1a comes on one 3.5" diskette. This diskette contains 
this file (README.TXT) and a directory PM2SEL with an executable 
file PM2SEL v.0.1a (pm2s.exe),configuration file (pm2s.cfg),  
redirection file (pm2s_app.red); a subdirectory for binary database 
files (DATABASE) and a subdirectory for backup text format files 
or PMATCH backup files (TXTFILES).

The installation procedure requires the following steps:

a) Copy the directory tree PM2SEL from the floppy disc to your 
computer hard disk (use any file manager like Norton Commander, 
Windows Explorer etc.).
b) If necessary, change all paths in the redirection file 
(pm_appl.red) to your hard disk (if not C:) using any standard 
text editor. This file must contain full paths for work
subdirectories of PM2SEL program. 

Example: the directory tree is copied from diskette into drive D:, 
directory \MY_DATA

Old contents of  pm2s_app.red:

4
s_txt    = C:\PM2SEL\txtfiles
a_base  =  C:\PM2SEL\database
s_binexe = C:\PM2SEL
a_       = a:

After editing, new contents of this file should be:

4
s_txt    = D:\MY_DATA\PM2SEL\txtfiles
a_base  =  D:\MY_DATA\PM2SEL\database
s_binexe = D:\MY_DATA\PM2SEL
a_       = a:

Warning: long directory names (>8 characters) can not be
used, even when launching PM2SEL from Windows/95 or 
Windows/NT. 



3.  	Starting and Quitting the Program


To start PM2SEL from the MS DOS prompt (or DOS mode of Windows 95/98):

Change to \PM2SEL directory (for instance, type command 
cd\MY_DATA\PM2SEL) ; 

At the DOS propmpt D:\MY_DATA\PM2SEL\>,  type: pm2s  and press Enter.

To start from Windows/NT, create a batch file with the following 
contents:

echo off
forcedos PM2SEL

and put it in the ~\PM2SEL\ directory. Then execute this .bat file. 

In a few seconds, a message beginning with words  «Welcome to PM2SEL 
utility program,...» will appear on screen. Press «Y»  to continue 
work with PM2SEL utility. In a few more seconds, the main screenform 
for operation of PM2SEL utility program will appear. See Section 3 
of user's manual for explanations how to activate screenforms and 
access the data.

If the program did not run correctly, the most probable reason is  
that the EMS mode access to extended memory is disabled. Check 
config.sys file in C:\ root directory. Switch off the NOEMS mode 
(if it is present) from the parameters of EMM86 or equivalent 
extended memory driver command in config.sys or autoexec.bat files. 
After editing the files, reboot the computer to take changes into 
effect and try to start PM2SEL again.

To quit PM2SEL, select  «Exit Program» item  on main screenform or 
press Alt-X  key combination. In the last case,confirm quitting by 
pressing «Y» in  a  dialog box with a message:
        
         "Confirm end of work and exit "

After quitting PM2SEL program session, the new configuration is saved
automatically. If the program does not start again, the most probable 
reason is that the pm2s.cfg file is corrupt. Restore it from a copy of 
the initial .cfg file on floppy disc. 



4.	How to run examples described in Appendix B:

I. Master species data example (pmat_mst.bac):


1. Select «Dependent Components (Thermochemical)» item  on main 
screenform and press Enter.

2. Press F9 shortcut key. This will display a menu of database 
maintenance functions. Highlight  "import" action with the cursor and 
press Enter to execute it.

3. A message will appear in the dialog box:

          Import data from PMATCH backup format (Y)
            or Selektor-A txt-format (N)? (C -cancel) 

Press "Y"  to proceed.

4. The program will prompt you to type the name of PMATCH backup text 
file from which the data will be imported; the following string 
will appear:  

 C:\PM2SEL\txtfiles\*.txt

Change it to C:\PM2SEL\txtfiles\pmat_mst.bac and press Enter.

5. Next, you will be prompted to edit a DCOMP record key template: 
 
"a:*:*:na1:"

Do not edit it, just press Enter. 

6. A message will appear in the dialog box:
   
    Would you like to import all records (Y)
             or check/edit each one (N)?

Press "Y" if you wish to use All-at-once Mode, or "N" to use 
Stepwise Mode.
  


II. Non-master species data example (pmat_rcc.bac):
    should be performed only after the file pmat_mst.bac 
    has been imported into PM2SEL binary database.

1. Select «Dependent Components (Reaction-defined)» item  on main 
screenform and press Enter.

2. Press F9 shortcut key. This will display a menu of database 
maintenance functions. Highlight  "import" action with the cursor and 
press Enter to execute it.

3. A message will appear in the dialog box:

          Import data from PMATCH backup format (Y)
            or Selektor-A txt-format (N)? (C -cancel) 

Press "Y"  to proceed.

4. The program will prompt you to type the name of PMATCH backup text 
file from which the data will be imported, displaying a line

 C:\PM2SEL\txtfiles\*.txt

Change it to  C:\PM2SEL\txtfiles\pmat_rcc.bac  and press Enter.

5. You will be prompted to edit a REACDC record key template. 
 
"a:*:*:na1:"

Do not edit it, just press Enter. 

6. A message will appear in the dialog box:
   
    Would you like to import all records (Y)
             or check/edit each one (N)?

Press "Y" if you wish to use All-at-once Mode, or "N" to use 
Stepwise Mode.
  

-----------------------------
May 3, 1999

Addition:  In the F9 functions menu, a new line "rename" 
has been added. This function enables easy changing of record
keys, retaining the contents. 

Warning: The "rename" function should be used with great care 
for ICOMP and DCOMP records, as chemical formulae refer to 
ICOMP record keys, and REACDC data refer to DCOMP record keys. 





