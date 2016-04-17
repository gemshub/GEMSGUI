# GEM-Selektor v.3 #

This GEMS3GUI repository contains the source code and default resources for GEM-Selektor v.3 - the interactive package for thermodynamic modelling of aquatic (geo)chemical systems by Gibbs Energy Minimization using a built-in GEMS3K chemical solver.

## Briefly about GEM-Selektor v.3 ##

Distributed "as is" by the Laboratory for Waste Management (LES) of the Paul Scherrer Institute (PSI) with two purposes:

* to promote the GEM method and software into research community;
* to gather the users feedback - vital for making the software more functional and reliable.

Permission to use the GEM-Selektor software is hereby granted free of charge for educational and research purposes, subject to acceptance of Terms and Conditions of Use. In particular, in any publication of your results obtained using the GEM-Selektor code, please, cite the web page (http://gems.web.psi.ch) and the following papers: 

* Kulik D.A., Wagner T., Dmytrieva S.V., Kosakowski G., Hingerl F.F., Chudnenko K.V., Berner U. (2013): GEM-Selektor geochemical modeling package: revised algorithm and GEMS3K numerical kernel for coupled simulation codes. Computational Geosciences 17, 1-24.
* Wagner T., Kulik D.A., Hingerl F.F., Dmytrieva S.V. (2012): GEM-Selektor geochemical modeling package: TSolMod library and data interface for multicomponent phase models. Canadian Mineralogist 50, 1173-1195.

Version: currently 3.3.3.

Learn more about GEM-Selektor from http://gems.web.psi.ch 

## How to clone (download) the GEMS3GUI source code and build it on linux ##

* In your home directory, make a folder named e.g. ~/gitGEMS3 with a subfolder ~/gitGEMS3/gems3gui.
* Change into ~/gitGEMS3/gems3gui and clone this repository from https://<you>@bitbucket.org/gems4/gems3gui.git using a preinstalled free git client SourceTree or SmartGit (the best way on Windows). 
* Alternatively on Mac OS X or linux, open a terminal, cd ~/gitGEMS3/gems3gui and type in the command line (do not forget a period):
~~~
git clone https://<you>@bitbucket.org/gems4/gems3gui.git . 
~~~
* To switch to another branch (e.g. devEJDB), use a git client or open a terminal, cd to ~/gitGEMS3/gems3gui, and type in the command line
~~~
git checkout -b branches/devEJDB --track origin/branches/devEJDB
git pull origin branches/devEJDB
~~~
To switch back to trunk, type
~~~
git checkout trunk
~~~

* Now cd back to ~/gitGEMS3 and make a subfolder ~/gitGEMS3/standalone then clone the GEMS3K repository by running
~~~
cd standalone
git clone https://<you>@bitbucket.org/gems4/gems3k.git . 
~~~ 

* This will download the trunk branch of the GEMS3K code. To switch (if necessary) to a different branch, do:
~~~
git checkout -b branches/devEJDB --track origin/branches/devEJDB
git pull origin branches/devEJDB
~~~ 

* Finally, cd back to ~/gitGEMS3 and create there empty build directories /build-release and /build-debug.
* We assume that both subfolders /gems3gui and /standalone are under git control and are checked out to "trunk" branch.

### Building and deploying GEM-Selektor v.3 ###

This deployment process is based on the "gemsdeployqtapp.sh" script, which is an augmented and extended "deployqtapp.sh" script from
https://github.com/goblincoding/qtscripts by goblincoding, see also http://goblincoding.com/2013/11/07/deploying-qt-5-applications-on-ubuntu-12-04/  

1. Make sure that Qt5 SDK is installed in the system or as ~/Qt in your home directory.

On latest versions of Linux, the complete Qt5 is available in the system. On Ubuntu 15.10, it can be installed using a command:
~~~
sudo apt-get install qt5-default qttools5-dev
~~~
It may be necessary to install in addition:
~~~
sudo apt-get install libqt5svg5 libqt5help5
~~~

Now we assume that the Qt library is installed locally as "/home/you/Qt/5.3/src" and "/home/you/Qt/5.3/gcc_64" (or gcc_32). If not yet done, copy recursively the "/Docs/build-release" directory from the  folder where this file is located to the folder in which the "/gems-gui" folder is located. Typically, you should have the folders at the same level, as shown below.
~~~
~/gitGEMS3
    /gems3gui
    /standalone
    /build-release
    /build-debug
~~~
We assume that both subfolders /gems3gui and /standalone are under git control and are checked out to "trunk" branch.

2. Make sure that you have built (with the QtCreator) a release variant of GEM-Selektor v.3 in "/home/you/DevGEMS/devPhase/build-release", where an executable script "gemsdeployqtapp.sh" and several other files are located; if the script is not executable, open a terminal, cd to that directory, and run 
~~~
chmod u+x ./gemsdeployqtapp.sh.
~~~

3. Open a Linux terminal, cd to "/home/you/DevGEMS/devPhase/build-release", and execute the script "gemsdeployqtapp.sh":
~~~
./gemsdeployqtapp.sh gems3
~~~

This will create an archive "Gems3-app.tgz". Optionally, to investigate the script output, run
~~~
$ ./gemsdeployqtapp.sh gems3 > ./output.txt
~~~

4. Rename the file "Gems3-app.tgz" to "Gems3.3-<rev1>.<rev2>-linux-x86-64.tgz", for instance "Gems3.3-2558.960-linux-x86-64.tgz". It can now be provided. 

5. For installation steps, see INSTALL-linux.txt. Install and test if it works. If gems3 does not start, try the following. In terminal, cd to "/Gems3-app/platforms" and run there the script "fixdep.sh" with superuser rights:
~~~
$ sudo ./fixdep.sh
~~~

This script needs superuser because it can automatically download the necessary libraries for your platform.       

### Mac OSX ###

This deployment process is based on the "macdeployqt" utility, located in ~/Qt/5.3/clang_64/bin/ folder.

1. Make sure that the Qt5 SDK is installed as ~/Qt or in similar folder. We assume that the Qt library is located in ~/Qt/5.3/clang_64/bin/ and Qt Tools (QtCreator etc.) are located in ~/Qt/.

2. If not yet done, find ~/DevGEMS/trunk/gems-gui/Docs/mac-build-release directory and copy it recursively into ~/Applications, then rename it to ~/Applications/devPhase. Dive into the ~/Applications/trunk/gems3.app/Contents and refresh recursively the /Resources subfolder contents by copying it from ~/DevGEMS/trunk/gems-gui/Resources.

3. In the ~/Applications/trunk folder, find the "runmacdeployqt.bat" file and edit it to make sure that it contains the correct full path to the "macdeployqt" utility, e.g. Users/<you>/Qt/5.3/clang_64/bin/macdeployqt. From the latter folder, copy the actual files "qcollectiongenerator", "qhelpconverter" and "qhelpgenerator" (needed to refresh the help database) to ~/Applications/devPhase/gems3.app/Contents/MacOS.

4. Using QtCreator, build the actual release variant of gems3 executable. Typically, you should have something like "~/DevGEMS/trunk/gems-gui"; "~/DevGEMS/trunk/standalone"; and "~/DevGEMS/trunk/build-release" folders at the same level. Find gems3 in /gems3.app in the latter folder and copy it into ~/Applications/devPhase/gems3.app/Contents/MacOS.

5. Open terminal, cd to ~/Applications/trunk and run the command:
~~~
$ ./runmacdeployqt.sh
~~~
This will find and copy all dependence files (Qt libraries, plugins, etc.) into ~/Applications/trunk/gems3.app, and also will create a gems3.dmg image file of the GEM-Selektor installer.

6. To make sure that the GEM-Selektor runs correctly: in terminal, cd to ~ (your user home directory) and start the program with the command:
~~~
$ exec /Users/<you>/Applications/trunk/gems3.app/Contents/MacOS/gems3 -d
~~~
Upon the first start, the gems3 program will create a ~/Library/Gems3/projects folder, in which the test projects are located (automatically copying from the .../gems3.app/Contents/Resources/projects).
 
7. To complete creating a new GEM-Selektor installer file for Mac OS X: cd to ~/Applications/devPhase/ folder and delete the gems3.dmg file. Then, to create the installer, run again the command
~~~
$ ./runmacdeployqt.sh
~~~
This will create the file ~/Applications/devPhase/gems3.dmg. Rename it according to the following pattern:
~~~
Gems3.2-<rev1>.<rev2>-OSX-x86-64.dmg
~~~
where put the same actual revision numbers for GEMS GUI (rev1) and GEMS3K (rev2) parts, as used in the built code. The dmg file can now
be distributed.

8. For installation steps, see INSTALL-macosx.txt. Install and test if it works.


### Windows 7,8,10 ###

This deployment process is based on the "windeployqt.exe" utility, which is an augmented "windeployqt.exe" binary by lkc, taken from
https://github.com/lkc/windeployqt.

1. Make sure that the Qt5 SDK is installed as C:\Qt or in similar folder. We assume that the Qt library is located in C:\Qt5\5.3\mingw482_32\bin and Qt Tools are located in C:\Qt5\Tools\mingw482_32\bin (or similar, depending on the current Qt version, MinGW version and their locations).  

2. If not yet done, go into the .\Docs\win-build-release directory and copy recursively the folder \GEMS3DEPLOY to C:\ drive. Dive into the  C:\GEMS3DEPLOY\Gems3-app folder. Refresh recursively the \Resources subfolder (from .gems-gui\Resources in GEM-Selektor source code tree). 

3. In the C:\GEMS3DEPLOY\Gems3-app folder, edit the "runwindeployqt.bat" file to make sure that it contains the correct paths to 
C:\Qt5\5.3\mingw482_32\bin and C:\Qt5\Tools\mingw482_32\bin directories (modify to actual ones, if necessary). Copy (or refresh) the files 
qcollectiongenerator.exe, qhelpconverter.exe and qhelpgenerator.exe from C:\Qt5\5.3\mingw482_32\bin to C:\GEMS3DEPLOY\Gems3-app folder.   

4. Using QtCreator, build the actual release variant of gems3.exe. Typically, you should have something like "C:\DevGEMS\trunk\gems-gui";
"C:\DevGEMS\trunk\standalone"; and "C:\DevGEMS\trunk\build-release" folders at the same level. Find gems3.exe in the latter folder and copy it into the C:\GEMS3DEPLOY\Gems3-app folder.  

5. In command-line prompt window (terminal), cd to C:\GEMS3DEPLOY\Gems3-app and run the command:
~~~
runwindeployqt.bat gems3.exe
~~~
This will find and copy all dependence files (Qt libraries, plugins, runtime MinGW libraries, locale files, etc.) into C:\GEMS3DEPLOY\Gems3-app folder. 

6. To test that GEM-Selektor runs correctly: cd to C:\GEMS3DEPLOY and check that the batch file "rungems3.bat" contains correct path to the folder where the gems3.exe file is located (e.g. C:\GEMS3DEPLOY\Gems3-app). Edit, if necessary (see also Readme3.txt). Then start GEM-Selektor by executing the rungems3.bat file. Upon the first start, the program will create a C:\GEMS3DEPLOY\Library\Gems3\projects folder, in which the test projects are located (by copying them from C:\GEMS3DEPLOY\Gems3-app\Resources\projects).  
 
7. To create a new GEM-Selektor installer file for Windows 7 or Windows 8:

If not yet done, download and install 7-zip v.9.20 (http://www.7-zip.org/). Start it and select the C:\GEMS3DEPLOY folder. Add it contents (i.e. Gems3-app folder and a few files) recursively to the archive (to be created e.g. in C:\TEMP folder) "C:\TEMP\GEMS3DEPLOY.7z". 

If not yet done, download and install 7zip-sfx-maker v.3.3 utility from http://sourceforge.net/projects/sfx-maker/ . Also download ResourceHacker v.3.6 from http://www.angusj.com/resourcehacker/; install it into "C:\Program Files (x86)\7-Zip SFX Maker\Resource Hacker" folder. 

Start the 7zip-sfx-maker as an administrator and click on "Load Settings..." button. Select the file ".\Docs\win-build-release\Self-extractor-GEMS3-settings.xml" (the path is shown relative to the folder in which this file DeployOnWindows.txt is located). Then, using "+" button or "Files" "Add 7z file...", select the file "C:\TEMP\GEMS3DEPLOY.7z" prepared previously. Click through tabs "Dialogs", "Tasks", "Metadata" and edit what is necessary, e.g. revisionand version numbers, date of production, etc. When done, click on "Save 
settings..." button to save settings to the file ".\Docs\win-build-release\Self-extractor-GEMS3-settings.xml", which is under version control. Then click on the "Make SFX" button to create the self-installer file. This will create a file C:\TEMP\GEMS3DEPLOY.sfx.exe. Rename it according to the following pattern:
~~~
C:\TEMP\gems3.2-<rev1>.<rev2>-win32-install.sfx.exe 
~~~
where put the same actual revision numbers for GEMS GUI (rev1) and GEMS3K (rev2) parts, as used in "Self-extractor-GEMS3-settings.xml" 
file. The self-extracting installer can now be distributed. 

8. For installation steps, see INSTALL-windows.txt. Install and test if it works.


### For details on how to install the GEM-Selektor v3 code package: ### 

Please, consult this web page:

http://gems.web.psi.ch/GEMS3/techinfo.html

TBD
* Summary of set up
* Configuration
* Dependencies
* Database configuration
* How to run tests
* Deployment instructions

### Contribution guidelines ###

* Writing tests
* Code review
* Other guidelines