## GEM-Selektor v.3 for GEMS-Reaktoro benchmarking project #

This GEMSGUI repository contains the source code and default resources for GEM-Selektor v.3 client in GEMS-Reaktoro benchmarking project
GEMSGUI is an interactive package for thermodynamic modelling of aquatic (geo)chemical systems by Gibbs Energy Minimization using a standalone server - either GEMS3K chemical solver or alternatively the rkt4gems chemical solver based on Reaktoro 2 framework.

### Briefly about GEM-Selektor v.3 ##

Distributed "as is" by the Laboratory for Waste Management (LES) of the Paul Scherrer Institute (PSI) with two purposes:

* to promote the GEM method and software into research community;

* to gather the users feedback - vital for making the software more functional and reliable.

Permission to use the GEM-Selektor software is hereby granted free of charge for educational and research purposes, subject to acceptance of Terms and Conditions of Use. In particular, in any publication of your results obtained using the GEM-Selektor code, please, cite the web page (http://gems.web.psi.ch) and the following papers: 

* Kulik D.A., Wagner T., Dmytrieva S.V., Kosakowski G., Hingerl F.F., Chudnenko K.V., Berner U. (2013): GEM-Selektor geochemical modeling package: revised algorithm and GEMS3K numerical kernel for coupled simulation codes. Computational Geosciences 17, 1-24.

* Wagner T., Kulik D.A., Hingerl F.F., Dmytrieva S.V. (2012): GEM-Selektor geochemical modeling package: TSolMod library and data interface for multicomponent phase models. Canadian Mineralogist 50, 1173-1195.


Latest version: ![GitHub Tag](https://img.shields.io/github/v/tag/gemshub/GEMSGUI?label=GEMSGUI)

## GEM-Selektor v.3 for GEMS-Reaktoro project #

This GEMSGUI repository contains the source code and default resources for GEM-Selektor v.3 client in GEMS-Reaktoro benchmarking project
GEMSGUI is an interactive package for thermodynamic modelling of aquatic (geo)chemical systems by Gibbs Energy Minimization using a standalone server - either GEMS3K chemical solver or alternatively the rkt4gems chemical solver based on Reaktoro 2 framework.

### Briefly about GEM-Selektor v.3 ###

Distributed "as is" by the Laboratory for Waste Management (LES) of the Paul Scherrer Institute (PSI) with two purposes:

* to promote the GEM method and software into research community;
* to gather the users feedback - vital for making the software more functional and reliable.

Permission to use the GEM-Selektor software is hereby granted free of charge for educational and research purposes, subject to acceptance of Terms and Conditions of Use. In particular, in any publication of your results obtained using the GEM-Selektor code, please, cite the web page (http://gems.web.psi.ch) and the following papers: 

* Kulik D.A., Wagner T., Dmytrieva S.V., Kosakowski G., Hingerl F.F., Chudnenko K.V., Berner U. (2013): GEM-Selektor geochemical modeling package: revised algorithm and GEMS3K numerical kernel for coupled simulation codes. Computational Geosciences 17, 1-24.
* Wagner T., Kulik D.A., Hingerl F.F., Dmytrieva S.V. (2012): GEM-Selektor geochemical modeling package: TSolMod library and data interface for multicomponent phase models. Canadian Mineralogist 50, 1173-1195.

Version: currently 3.9.5.

Learn more about GEM-Selektor from http://gems.web.psi.ch 

### Briefly abour Reaktoro v.2 ###

## Instructions on how to build and set up GEMS-Reaktoro benchmarking tools on linux (e.g. ubuntu 20.04) ##

1. Install Python and miniconda (if not done already), e.g. performing steps described in

https://www.how2shout.com/linux/how-to-install-miniconda-on-ubuntu-20-04-lts-focal/

Briefly: check in terminal whether conda and python are already present (if yes then just versions of both will be reported in response):

```
conda --v
python --version
```

If the latter command returns something like
```
Command 'python' not found, did you mean:
  command 'python3' from deb python3
  command 'python' from deb python-is-python3
```
then python3 is already installed; just link the command 'python' to 'python3' as follows:
```
sudo ln -s /usr/bin/python3 /usr/bin/python
```

To install python and miniconda, perform the following steps in (bash) terminal:

```
cd ~
sudo apt install python3
sudo apt update && sudo apt upgrade
wget https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh
bash Miniconda3-latest-Linux-x86_64.sh
# Press 'q', then type "Yes" Enter to accept the license, then "Yes" to activate conda on staring the terminal
# To deactivate the latest option, execute 'conda config --set auto_activate_base false'
conda --v
python --version
conda install conda-devenv -c conda-forge
```

Hint: if 'conda --v' still returns 'conda: command not found' then add the line below to ~/.bashrc and ~/.profile file, and then re-login (replace <your_username> with your real linux user name): 
```
export PATH="/home/<your_username>/miniconda/bin:$PATH"
```

2. To build and start the rkt4gems server (branch rkt-server)

Create a folder to keep repositories and builds, e.g. 

```
#!bash
mkdir /home/<your_user>/git/GEMS-Reaktoro
cd /home/<your_user>/git/GEMS-Reaktoro
```

Clone the rkt4gems repository:

```
git clone https://<your_github_token>@github.com/gemshub/rkt4gems.git
```

Then build rkt4gems as described in README.md in the rkt4gems folder. Briefly, to build rkt4gems:

```
cd ~/git/GEMS-Reaktoro/rkt4gems/
conda devenv
conda activate rkt4gems
cmake -S . -B build
cmake --build build --parallel
```

To build tests and examples:

~~~bash
cmake --build build --target tests
cmake --build build --target examples
~~~

To execute c++ example: 

```
cd build/examples/cpp  
./example1
```

To launch the rkt4gems server, open a separate terminal and run:

```
cd ~/git/GEMS-Reaktoro/rkt4gems/build/rkt-server
./rkt-server

```

The rkt-server runs in the background, by default binds to tcp socket 5570, and listens to the client that can even run on another computer or in a different process on this machine.
To stop the server (in terminal where it was started): enter Ctrl-C.

To test the server, first build gemsreaktoro (see Section 4).

3. To build and install the GEMS3K library (version working with ThermoFun):

Check the conda environment (activate if needed), and install chemicalfun and gems3k from conda forge

```
# conda activate rkt4gems
conda install spdlog chemicalfun gems3k -c conda-forge
```

4. To build and start the gems3k-server and test client:

```
cd /home/<your_user>/git/GEMS-Reaktoro
git clone https://<your_github_token>@github.com/gemshub/gemsreaktoro.git && cd gemsreaktoro
mkdir -p build && cd build && cmake .. && make
```

To start the server, open a separate terminal window or tab, and execute:

```
cd ~/git/GEMS-Reaktoro/gemsreaktoro/build/bin
./gems3k-server

```

The gems3k-server runs in the background, by default binds to tcp socket 5570 and listens to the client that can run in a different process on this machine or on another computer.
To stop the server (in terminal where it was started): enter Ctrl-C.

To run the standalone batch test gems3k-client with the data from example1:

Open a separate terminal window or tab, and execute using example1 or another example:

```
cd ~/git/GEMS-Reaktoro/gemsreaktoro/build/bin
./gems3k-client example1/example1-dat.lst example1/example1-dbr.lst

```

The gems3k-client connects to the gems3k-server running on another computer or in a different process on this machine. It sends the input data for GEMS3K in ZeroMQ messages, the server calculates the GEM task and sends the results back to the client in a ZeroMQ message, then the standalone client prints the results and stops.

5. To build and run the gem-selektor graphical client (branch rkt-server)

Install Qt5.15.4 from conda forge. Also install packages providing libGL (to prevent linking errors like "fatal error: GL/gl.h: No such file or directory" when building Qt GUI applications):

```
conda install freeglut gsl glew glfw mesalib mesa-libgl-devel-cos7-x86_64 qt -c conda-forge
```

Now, clone the GEMSGUI Git repository. 

```
git clone https://<your_github_token>@github.com/gemshub/GEMSGUI.git && cd GEMSGUI
mkdir -p build && cd build && cmake .. && make
```
To start the gem-selektor client, check the path to folder containing Resources (after the -s key of gem-selektor), and then execute it:

```
cd App
./gem-selektor -d -s /home/<your_user>/git/GEMS-Reaktoro/GEMSGUI -u /home/<your_user>/git/GEMS-Reaktoro/Data/Library/Gems3
```

Note that the resources folder, e.g. /resources/projects/, should contain GEMS projects folders similar to those in /home/<your_user>/GEMS-Reaktoro/Data/Library/Gems3/projects for GEM-Selektor installation. Note: The folder "Data" in GEMS-Reaktoro must be created once, before starting GEMSGUI first time.

gems3k-server and rkt-server must be run in different processes or on different computers. So far, they listen to the same tcp socket 5570. This means that either can be used, but not both for choice from GEMSGUI; to implement this, rkt-server must listen to a different socket, e.g. 5580.
   

### Testing protocol (18.Oct.2022)


1. GEMSGUI starts and produces the following output after single-system calculation (Kaolinite project, SysEq pHtitrKa:0:0:1:25:0:):

1.1. with gems3k-server running:

```
cd ~/git/GEMS-Reaktoro/GEMSGUI/build/App
 ./gem-selektor -d -s /home/kulik/git/GEMS-Reaktoro/GEMSGUI -u /home/kulik/git/GEMS-Reaktoro/GEMSGUI/build/Library/Gems3
[2022-10-18 11:42:58.127] [gems3gui] [info] Local    : /home/kulik/git/GEMS-Reaktoro/GEMSGUI/build
[2022-10-18 11:42:58.127] [gems3gui] [info] SysGEM   : /home/kulik/git/GEMS-Reaktoro/GEMSGUI/Resources/
[2022-10-18 11:42:58.127] [gems3gui] [info] UserGEM  : /home/kulik/git/GEMS-Reaktoro/GEMSGUI/build/Library/Gems3/
[2022-10-18 11:42:58.163] [gems3gui] [info] gems.cfg saved
[tnode] [info] Send system... Kaolinite G  pHtitrKa    0    0       1       25      0    type 3

```

***Comment:*** In the [info] level log, after "send system" there must be a confirmation that the result is received. Now it is not clear if this result has been received indeed.

The respective output from gems3k-server console:

```
~/git/GEMS-Reaktoro/gemsreaktoro/build/bin$ ./gems3k-server
Bind gems3k server to: "tcp://*:5570" ( used 5 threads )
ZeroMQ worker start... 0  0x563684582e20
ZeroMQ worker start... 2  0x563684584c80
ZeroMQ worker start... 1  0x563684583d50
ZeroMQ worker start... 4  0x563684586ae0
ZeroMQ worker start... 3  0x563684585bb0
System Kaolinite G  pHtitrKa    0    0       1       25      0
Execute... 0 NodeStatusCH = 2

```

1.2. with rkt-server running:

```
cd ~/git/GEMS-Reaktoro/GEMSGUI/build/App
 ./gem-selektor -d -s /home/kulik/git/GEMS-Reaktoro/GEMSGUI -u /home/kulik/git/GEMS-Reaktoro/GEMSGUI/build/Library/Gems3
[2022-10-18 11:42:58.127] [gems3gui] [info] Local    : /home/kulik/git/GEMS-Reaktoro/GEMSGUI/build
[2022-10-18 11:42:58.127] [gems3gui] [info] SysGEM   : /home/kulik/git/GEMS-Reaktoro/GEMSGUI/Resources/
[2022-10-18 11:42:58.127] [gems3gui] [info] UserGEM  : /home/kulik/git/GEMS-Reaktoro/GEMSGUI/build/Library/Gems3/
[2022-10-18 11:42:58.163] [gems3gui] [info] gems.cfg saved
[tnode] [info] Send system... Kaolinite G  pHtitrKa    0    0       1       25      0    type 3
[gems3k] [error]
********************************************************************
*** Error: Invalid symbol
*** Reason: Nit
*** Location: ...1152052/work/ThermoFun/Common/formuladata.cpp:251
********************************************************************

:
********************************************************************
*** Error: Invalid symbol
*** Reason: Nit
*** Location: ...1152052/work/ThermoFun/Common/formuladata.cpp:251
********************************************************************


[2022-10-18 11:43:28.711] [gems3gui] [error] error_IPN

```

The output from rkt-server console:

```
cd ~/git/GEMS-Reaktoro/rkt4gems/build/rkt-server
 ./rkt-server
Bind server to: "tcp://*:5570" ( used 5 threads )
ZeroMQ worker start... 0  0x5570f0a43d50
ZeroMQ worker start... 1  0x5570f0a44c50
ZeroMQ worker start... 2  0x5570f0a45b50
ZeroMQ worker start... 3  0x5570f0a46a50
ZeroMQ worker start... 4  0x5570f0a47950
System "Kaolinite G  pHtitrKa    0    0       1       25      0"
error occured during deserialize the data
System "Kaolinite G  pHtitrKa    0    0       1       25      0"
error occured during deserialize the data
System "Kaolinite G  pHtitrKa    0    0       1       25      0"
error occured during deserialize the data
...

```

1.3. The system was exported into example3 (all JSON incl FUN files); example4 (GEMS KV format + FUN JSON file), and added to examples in gemsreaktoro repository.

2. Testing with "primitive" gems3k-client using example 3:

```
cd ~/git/GEMS-Reaktoro/gemsreaktoro/build/bin
```

2.1. With gems3k-server running:

Client console:

```
 ./gems3k-client example3/example3-dat.lst example3/example3-dbr.lst
[tnode] [info] Calc ThermoEngine T: 298.15  P: 100000
[tnode] [info] Initialization of system Kaolinite G  pHtitrKa    0    0       1       25      0
[tnode] [debug] CheckMtparam T: 298.15 - 298.15  P: 1 - 1
Send all - receive all
Elapsed time in microseconds: 24288 μs
[tnode] [debug] CheckMtparam T: 298.15 - 298.15  P: 1 - 1
Send/receive asynchronous
Elapsed time in microseconds: 28789 μs
[tnode] [debug] CheckMtparam T: 298.15 - 298.15  P: 1 - 1
Send-receive one by other
Elapsed time in microseconds: 18791 μs

./gems3k-client example4/example4-dat.lst example4/example4-dbr.lst
[tnode] [info] Calc ThermoEngine T: 298.15  P: 100000
[tnode] [info] Initialization of system Kaolinite G  pHtitrKa    0    0       1       25      0
[tnode] [debug] CheckMtparam T: 298.15 - 298.15  P: 1 - 1
Send all - receive all
Elapsed time in microseconds: 22575 μs
[tnode] [debug] CheckMtparam T: 298.15 - 298.15  P: 1 - 1
Send/receive asynchronous
Elapsed time in microseconds: 31782 μs
[tnode] [debug] CheckMtparam T: 298.15 - 298.15  P: 1 - 1
Send-receive one by other
Elapsed time in microseconds: 18697 μs
```

Server console:

```
./gems3k-server
Bind gems3k server to: "tcp://*:5570" ( used 5 threads )
ZeroMQ worker start... 0  0x5603945dce20
ZeroMQ worker start... 1  0x5603945ddd50
ZeroMQ worker start... 4  0x5603945e0ae0
ZeroMQ worker start... 2  0x5603945dec80
ZeroMQ worker start... 3  0x5603945dfbb0
System Kaolinite G  pHtitrKa    0    0       1       25      0
Execute... 0 NodeStatusCH = 6
System Kaolinite G  pHtitrKa    0    0       1       25      0
Execute... 0 NodeStatusCH = 6
System Kaolinite G  pHtitrKa    0    0       1       25      0
Execute... 0 NodeStatusCH = 6
System Kaolinite G  pHtitrKa    0    0       1       25      0
Execute... 0 NodeStatusCH = 6
System Kaolinite G  pHtitrKa    0    0       1       25      0
Execute... 0 NodeStatusCH = 6
System Kaolinite G  pHtitrKa    0    0       1       25      0
Execute... 0 NodeStatusCH = 6
^C
```

Results generated in files "example3-dbr-0-0000.json-out.json" and "example4-dbr-0-0000.dat-out.json" are identical.

***Comment:*** the latter file name has a wrong extension. It is a KV format file with comments, and hence must have an extension "dat" but not "json".

2.2. With rkt-server running:

Client console:

```
 ./gems3k-client example3/example3-dat.lst example3/example3-dbr.lst
[tnode] [info] Calc ThermoEngine T: 298.15  P: 100000
[tnode] [info] Initialization of system Kaolinite G  pHtitrKa    0    0       1       25      0
Error -1:
********************************************************************
*** Error: Invalid symbol
*** Reason: Nit
*** Location: ...1152052/work/ThermoFun/Common/formuladata.cpp:251
********************************************************************


********************************************************************
*** Error: Invalid symbol
*** Reason: Nit
*** Location: ...1152052/work/ThermoFun/Common/formuladata.cpp:251
********************************************************************


Send all - receive all
Elapsed time in microseconds: 15255 μs

...

```
(more similar error messages, the same for example4).
***Comment:***  The element "Nit" is provided in FUN.json file and in example3 input composition.

Server console:

```
cd ~/git/GEMS-Reaktoro/rkt4gems/build/rkt-server
 ./rkt-server
Bind server to: "tcp://*:5570" ( used 5 threads )
ZeroMQ worker start... 0  0x5570f0a43d50
ZeroMQ worker start... 1  0x5570f0a44c50
ZeroMQ worker start... 2  0x5570f0a45b50
ZeroMQ worker start... 3  0x5570f0a46a50
ZeroMQ worker start... 4  0x5570f0a47950
System "Kaolinite G  pHtitrKa    0    0       1       25      0"
error occured during deserialize the data
System "Kaolinite G  pHtitrKa    0    0       1       25      0"
error occured during deserialize the data
System "Kaolinite G  pHtitrKa    0    0       1       25      0"
error occured during deserialize the data
...

```
(more similar error messages).


Conclusion 18.Oct.2022: Still not possible to compare the results between gems3k-server and rkt-server because of absence of results from rkt-server.



ToDo:

< To Svitlana: Please, finalise the gemsreaktoro launcher so that it starts gems3k-server, rkt4gems-server and gem-selektor, and kills them when a shutdown of gemsreaktoro is performed>

< To Svitlana and Allan: Please, check that the JSON lists for elements (ICNL), species (DCNL) and phases (PHNL) are read in correct order into rkt-server from "-dch.json" file>








## Trash bin (keeping for a few weeks just for the case)

####  Or as usual use gems3gui.pro file. ?????
####  Building in Qt Creator using gems3gui.pro does not produce the same libraries and gems3kgui-app as cmake build!


Install the Qt6.4 framework in your home directory (download the open-source web installer from https://qt.io, start it and choose for installation:

In Qt 6.4.0: Desktop gcc 64-bit; Qt5 Compatibility Module; in Additional Libraries: Qt 3D, Qt Charts, Qt Data Visualization, Qt Image Formats, Qt Multimedia, Qt PDF, Qt WebEngine. 
In Developer and Designer Tools: Qt Design Studio 3.6.0; Qt Installer Framework 4.4; CMake 3.23.2; Ninja 1.10.2.

Start installation and check if Qt Designer launches normally.


2. To build and install ThermoFun library (used by GEMS3K and Reaktoro):


```
git clone https://<your_github_token>@github.com/thermohub/thermofun.git && cd thermofun
```

The thermofun library uses eigen3 linear algebra header, pybind11 for Python interfaces, spdlog for logging errors, nlohmann/json.hpp as third-party dependence to parse database files in JSON format, and chemicalfun for parsing chemical formulae and generating or parsing reactions. To install these dependencies in the system on ubuntu 20.04 linux or similar (if not done before):


```
sudo apt-get install libeigen3-dev
sudo ln -s /usr/include/eigen3/Eigen /usr/include/Eigen

sudo apt-get install pybind11-dev
sudo apt-get install libspdlog-dev
sudo ./install-dependencies.sh
```

#### ToDo: add install of libeigen3-dev, pybind11-dev and libspdlog-dev to the install-dependencies.sh script

To compile thermofun library and install it with headers in the system (/usr/local):


```
mkdir build && cd build && cmake .. && make
sudo make install
```




>  7. Alternative installation from Conda Forge
   
> *TBD*
> *For installation using Conda see instructions in readme.md*
> *All above applications can be installed using Conda too*




TBD

2. Start the QtCreator (if Qt5 is in the system, type "qtcreator" in terminal and press Enter) and build a release variant of GEM-Selektor v.3 in "/home/you/DevGEMS/devPhase/build-release", where an executable script "gemsdeployqtapp.sh" and several other files are located; if the script is not executable, open a terminal, cd to that directory, and run 
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

4. Rename the file "Gems3-app.tgz" to "Gems3.5-<rev1>.<rev2>-linux-x86-64.tgz", for instance "Gems3.5-2558.960-linux-x86-64.tgz". It can now be provided. 

5. For installation steps, see INSTALL-linux.txt. Install and test if it works. If gems3 does not start, try the following. In terminal, cd to "/Gems3-app/platforms" and run there the script "fixdep.sh" with superuser rights:
~~~
$ sudo ./fixdep.sh
~~~

This script needs superuser because it can automatically download the necessary libraries for your platform.       


```
sudo ./install-dependencies.sh

```

Then make and install gems3k-server:


```
mkdir -p build && cd build && cmake .. && make

```


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

