After installation, you will find the GEMS entry with GEM-Selektor sub-entry in Windows applications menu.
Click on GEM-Selektor to start the program for the first time. 
If it does not start and shows errors about missing DLL files, this means that MSVC runtime dependencies 
were not (yet) installed in your Windows system. To install them, open your web browser and browse to

https://support.microsoft.com/en-ie/help/2977003/the-latest-supported-visual-c-downloads

From the appearing page, download the  x64: vc_redist.x64.exe  file using the following link:

https://aka.ms/vs/16/release/vc_redist.x64.exe

Start this file in your user Downloads folder and follow the instructions to install the runtime libraries. 
After this installation is finished, the GEM-Selektor code should start normally. 

If you have copied some of your old modelling projects to C:\Users\your_user\GEMS3\Library\GEMS3\projects folder, 
but do not see them when you start GEM-Selektor code, exit GEM-Selektor and start it once using the batch file
 
C:\Users\your_user\GEMS3\rungems3.bat

The GEM-Selektor code starts and you should now see your added modelling projects. Any next time you can start
GEM-Selektor as usual from Windows Start Applications menu.

   