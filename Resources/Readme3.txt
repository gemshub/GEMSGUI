Welcome to GEM-Selektor v3 program!

Before you can start GEMS3 from command line for the first time, please edit the file rungems3.bat (with any simple text editor) in order to ensure that GEMS3 command line parameters point to correct locations of the program resources and of modeling projects.

Suppose that you have installed GEMS3 in "D:\MyWork\Modeling\". Open the rungems3.bat file in text editor; the file begins with the following lines:

	echo off
	rem  Change the path to the actual location of GEMS3 executable and Resources
	cd C:\Selektor\Gems3-app
	...

Now, edit the cd (change directory) command argument in the third line to point to the actual location: 

	echo off
	rem  Change the path to the actual location of GEMS3 executable and Resources
	cd D:\MyWork\Modeling\Gems3-app
	...

Then leave one of the launch commands (they all contain gems3.exe) without "rem" at the beginning, and close other commands with "rem". Save the batch file. Now you can start GEMS3 by typing "rungems.bat" in the command line. 

To create a Windows desktop shortcut to launch GEMS3 from this location, right-click on the desktop; choose "New" "Shortcut", and set there the path to gems3.exe file: "D:\MyWork\Modeling\Gems3-app\gems3.exe -c", and finish. To add the icon and command line parameters, right-click on the new shortcut icon, select "Properties", append the command-line parameters (such as in the rungems3.bat) into "Target" field, then click "Change icon", and browse to
"D:\MyWork\Modeling\Gems3-app\Resources\Gems3.ico". You should see now the GEMS icon; click "Ok". The shortcut will now appear as GEMS icon, double-click to start the program. 

DISCLAIMER: thermodynamic data bases in test modeling projects provided in this installation are provisional and undocumented. The Development Team accepts no liability of any kind for any losses or invonveniences that may result from use of the data provided in modeling projects for testing and educational purposes. No garantee is given that the input data in test projects are, in fact, accurate. 

Enjoy GEM-Selektor!

(c) 2003,2012 GEMS Development Team
gems2.support@psi.ch
http://gems.web.psi.ch
 

