
## GEM-Selektor v.3

Welcome to GEM-Selektor v3 program!

GEM-Selektor v.3 - the interactive package for thermodynamic modelling of aquatic (geo)chemical systems by Gibbs Energy Minimization using a built-in GEMS3K chemical solver.

### Briefly about GEM-Selektor v.3

Distributed "as is" by the Laboratory for Waste Management (LES) of the Paul Scherrer Institute (PSI) with two purposes:

* to promote the GEM method and software into research community;

* to gather the users feedback - vital for making the software more functional and reliable.

Permission to use the GEM-Selektor software is hereby granted free of charge for educational and research purposes, subject to acceptance of Terms and Conditions of Use. In particular, in any publication of your results obtained using the GEM-Selektor code, please, cite the web page (http://gems.web.psi.ch) and the following papers:

* Kulik D.A., Wagner T., Dmytrieva S.V., Kosakowski G., Hingerl F.F., Chudnenko K.V., Berner U. (2013): GEM-Selektor geochemical modeling package: revised algorithm and GEMS3K numerical kernel for coupled simulation codes. Computational Geosciences 17, 1-24.

* Wagner T., Kulik D.A., Hingerl F.F., Dmytrieva S.V. (2012): GEM-Selektor geochemical modeling package: TSolMod library and data interface for multicomponent phase models. Canadian Mineralogist 50, 1173-1195.

Version: currently 3.10.0.

### How to run GEM-Selektor

Before you can start GEM-Selektor from command line for the first time, please edit the file rungems3.bat(.sh) (with any simple text editor) in order to ensure that GEMS3 command line parameters point to correct locations of the program resources and of modeling projects.

Suppose that you have installed GEMS3 in "D:\MyWork\Modeling\GEMS3.10.0". Open the rungems3.bat file in text editor; the file begins with the following lines:

	echo off
        rem  Change the path to the actual location of gem-selektor executable and Resources
        cd ./Gems3-app/bin
	...

Now, edit the cd (change directory) command argument in the third line to point to the actual location: 

	echo off
	rem  Change the path to the actual location of GEMS3 executable and Resources
        cd D:\MyWork\Modeling\Gems3-app\bin
	...

Then leave one of the launch commands (they all contain gem-selektor.exe) without "rem" at the beginning, and close other commands with "rem". Save the batch file. Now you can start GEM-Selektor by typing "rungems3.bat" in the command line. 

To create a Windows desktop shortcut to launch GEM-Selektor from this location, right-click on the desktop; choose "New" "Shortcut", and set there the path to gem-selektor.exe file: "D:\MyWork\Modeling\Gems3-app\bin\gem-selektor.exe -c", and finish. 
To add the icon and command line parameters, right-click on the new shortcut icon, select "Properties", append the command-line parameters (such as in the rungems3.bat) into "Target" field, then click "Change icon", and browse to
"D:\MyWork\Modeling\Gems3-app\Resources\Gems3.ico". You should see now the GEMS icon; click "Ok". The shortcut will now appear as GEMS icon, double-click to start the program. 

DISCLAIMER: thermodynamic data bases in test modeling projects provided in this installation are provisional and undocumented. The Development Team accepts no liability of any kind for any losses or inconveniences that may result from use of the data provided in modeling projects for testing and educational purposes. No guarantee is given that the input data in test projects are, in fact, accurate. 

Enjoy GEM-Selektor!

(c) 2003,2025 GEMS Development Team

gems2.support@psi.ch

http://gems.web.psi.ch
 

