# GEM-Selektor v.3

Welcome to GEM-Selektor v3 program!

GEM-Selektor v.3 - the interactive package for thermodynamic modelling of aquatic (geo)chemical systems by Gibbs Energy Minimization using a built-in GEMS3K chemical solver.


## How to run GEM-Selektor

    * To run GEM-Selektor application for the first time double click to execute `rungems3.bat`. After the first run you can always start GEM-Selektor using its shortcut or `gem-selektor.exe` from `Gems3-app\bin`.

    !!! warning

        ![Windows run anyway](selektor_run_anyway.png "Unknown Publisher message in Windows"){ width="200" align=left }
        
        * If windows will show you a warning message **Windows protected your PC**, click "More info" and **Run anyway**.

    !!! warning
        * If you get an error `The application was unable to start correctly` that VCRUNTIME140_1.dll is missing from your computer. You have to download and install the latest Microsoft Visual C++ Redistributable version for the appropriate architecture, e.g., X86_64 for Intel, AMD PCs. [Visual C++ Redistributable Download](https://learn.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist?view=msvc-170#latest-microsoft-visual-c-redistributable-version)

        After installing the latest Microsoft Visual C++ Redistributable, try launching GEMS again.  

    !!! info "Pin to taskbar / shortcut"
        Once GEMS is running you can pin the program to the Task bar by right-clicking on its icon (on the task bar) and chose pin to task bar. 
        
        After you close GEMS, the script should create a Start Menu and Desktop shortcut. 

    !!! note "Important program folders Location"  
        To avoid permission issues in Windows 10/11, the recommended installation target is the user’s home folder:  
        **Recommended program location:** `C:\Users\<myuser>\GEMS<version>` <br>
        **Default resources and thermodynamic database location:** <br>`C:\Users\<myuser>\GEMS<version>\Gems3-app\Resources\DB.default` <br>
        **Default user projects location:** `C:\Users\<myuser>\Library\Gems3\projects`  

    * Alternatively, open a windows terminal, change directory to the right path and execute:

    ```bash
    rungems3.bat
    ```

    * For more details about command line parameters, see into `rungems3.bat`. Edit the file `rungems3.bat` (with any simple text editor) in order to ensure that GEMS3 command line parameters point to correct locations of the program resources and of modeling projects.
    Then leave one of the launch commands (they all contain gem-selektor.exe) without "rem" at the beginning, and close other commands with "rem". Save the batch file. Now you can start GEM-Selektor by typing "rungems3.bat" in the command line. 
    

## Briefly about GEM-Selektor v.3

Distributed "as is" by the Laboratory for Waste Management (LES) of the Paul Scherrer Institute (PSI) with two purposes:

* to promote the GEM method and software into research community;

* to gather the users feedback - vital for making the software more functional and reliable.

Permission to use the GEM-Selektor software is hereby granted free of charge for educational and research purposes, subject to acceptance of Terms and Conditions of Use. In particular, in any publication of your results obtained using the GEM-Selektor code, please, cite the web page (http://gems.web.psi.ch) and the following papers:

* Kulik D.A., Wagner T., Dmytrieva S.V., Kosakowski G., Hingerl F.F., Chudnenko K.V., Berner U. (2013): GEM-Selektor geochemical modeling package: revised algorithm and GEMS3K numerical kernel for coupled simulation codes. Computational Geosciences 17, 1-24.

* Wagner T., Kulik D.A., Hingerl F.F., Dmytrieva S.V. (2012): GEM-Selektor geochemical modeling package: TSolMod library and data interface for multicomponent phase models. Canadian Mineralogist 50, 1173-1195.

DISCLAIMER: thermodynamic data bases in test modeling projects provided in this installation are provisional and undocumented. The Development Team accepts no liability of any kind for any losses or inconveniences that may result from use of the data provided in modeling projects for testing and educational purposes. No guarantee is given that the input data in test projects are, in fact, accurate. 

**Enjoy GEM-Selektor!**

(c) 2003-2025 GEMS Development Team

gems2.support@psi.ch

http://gems.web.psi.ch
 

