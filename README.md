# GEM-Selektor v.3 #

This GEMSGUI repository contains the source code and default resources for GEM-Selektor v.3 client in GEMS-Reaktoro benchmarking project
GEMSGUI is an interactive package for thermodynamic modelling of aquatic (geo)chemical systems by Gibbs Energy Minimization using a standalone server - either GEMS3K chemical solver or alternatively the rkt4gems chemical solver based on Reaktoro 2 framework.

## Briefly about GEM-Selektor v.3 ##

Distributed "as is" by the Laboratory for Waste Management (LES) of the Paul Scherrer Institute (PSI) with two purposes:

* to promote the GEM method and software into research community;
* to gather the users feedback - vital for making the software more functional and reliable.

Permission to use the GEM-Selektor software is hereby granted free of charge for educational and research purposes, subject to acceptance of Terms and Conditions of Use. In particular, in any publication of your results obtained using the GEM-Selektor code, please, cite the web page (http://gems.web.psi.ch) and the following papers: 

* Kulik D.A., Wagner T., Dmytrieva S.V., Kosakowski G., Hingerl F.F., Chudnenko K.V., Berner U. (2013): GEM-Selektor geochemical modeling package: revised algorithm and GEMS3K numerical kernel for coupled simulation codes. Computational Geosciences 17, 1-24.
* Wagner T., Kulik D.A., Hingerl F.F., Dmytrieva S.V. (2012): GEM-Selektor geochemical modeling package: TSolMod library and data interface for multicomponent phase models. Canadian Mineralogist 50, 1173-1195.

Version: currently 3.9.5.

Learn more about GEM-Selektor from http://gems.web.psi.ch 

## Instructions on how to build and set up GEMS-Reaktoro benchmarking tools on linux (e.g. ubuntu 20.04) ##

Create a folder to keep repositories and builds, e.g. 

```
#!bash
mkdir /home/<your_user>/git/GEMS-Reaktoro
cd /home/<your_user>/git/GEMS-Reaktoro
```

1. To build and install ThermoFun library (used by GEMS3K and Reaktoro):


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

2. To build and install GEMS3K library (version working with ThermoFun):


```
cd /home/<your_user>/git/GEMS-Reaktoro
git clone https://<your_github_token>@github.com/gemshub/GEMS3K.git && cd GEMS3K
./install.sh
```

Type your password if/when prompted (installs in /usr/local/).

3. To build and start the gems3k-server of GEMS-Reaktoro (from git branch "rkt-server"):


```
cd /home/<your_user>/git/GEMS-Reaktoro
git clone https://<your_github_token>@github.com/gemshub/gemsreaktoro.git -b rkt-server
cd gemsreaktoro
```

First, install ZeroMQ library for (remote) communication between client and servers


```
sudo ./install-dependencies.sh

```

Then make and install gems3k-server:


```
mkdir -p build && cd build && cmake .. && make

```

To start the server, open a separate terminal window or tab, and execute:

```
cd ~/git/GEMS-Reaktoro/gemsreaktoro/build/bin
./gems3k-server

```

The gems3k-server runs in the background and listens to the client that can run on another computer or in a different process environment on this machine.
To stop the server (in terminal where it was started): enter Ctrl-C.
   
4. To run the standalone batch test gems3k-client with the data from example1:

Open a separate terminal window or tab, and execute:


```
cd ~/git/GEMS-Reaktoro/gemsreaktoro/build/bin
./gems3k-client example1/example1-dat.lst example1/example1-dbr.lst

```

The gems3k-client connects to the gems3k-server running on another computer or in a different process on this machine. It sends the input data for GEMS3K in ZeroMQ messages, the server calculates the GEM task and sends the results back to the client in a ZeroMQ message, then the standalone client prints the results and stops.
   
#### *Alternatively, one can build and run the gems3k-server and gems3k-client using Qt projects.*
 
##### How exactly? What Qt vestion and platform should be installed? 
   
5. To build and run the gems3gui graphical client (branch rkt-server)

Install the Qt6.4 framework in your home directory (download the open-source web installer from https://qt.io, start it and choose for installation:

In Qt 6.4.0: Desktop gcc 64-bit; Qt5 Compatibility Module; in Additional Libraries: Qt 3D, Qt Charts, Qt Data Visualization, Qt Image Formats, Qt Multimedia, Qt PDF, Qt WebEngine. 
In Developer and Designer Tools: Qt Design Studio 3.6.0; Qt Installer Framework 4.4; CMake 3.23.2; Ninja 1.10.2.

Start installation and check if Qt Designer launches normally.

Also install linux packages providing libGL (to prevent linking errors when building GUI applications):

```
sudo apt-get update
sudo apt-get install libglu1-mesa-dev freeglut3-dev mesa-common-dev
sudo apt install --reinstall libgl-dev
```

Now, clone the GEMSGUI Git repository. 
Please, change the path to Qt toolkit to the actual one in the cmake command below:

```
git clone https://<your_github_token>@github.com/gemshub/GEMSGUI.git -b rkt-server
cd GEMSGUI
mkdir -p build && cd build
cmake .. -DCMAKE_PREFIX_PATH=/home/<your_user>/Qt/6.4.0/gcc_64 && make
```
To start the gems3gui GUI client, check the path to folder containing Resources (after the -s key of gems3kgui-app), and then execute:

```
cd App
./gems3kgui-app -d -s /home/<your_user>/git/GEMS-Reaktoro/GEMSGUI
```
< Why gems3kgui-app and not gemsgui-app or gemsguiclient-app? >
< Shall the last command also start gems3k-server and rkt4gems-server? >

Note that the resources folder, e.g. /resources/projects/, should contain GEMS projects folders similar to those in Library/Gems3/projects for GEM-Selektor installation. 

GEMS3K and rkt4gems servers must be run in different processes or on different computers
   
#  Or as usual use gems3gui.pro file. ?????
#  Building in Qt Creator using gems3gui.pro does not produce the same libraries and gems3kgui-app as cmake build!   

6. To build and start the rkt4gems server (branch rkt-server)

```
git clone https://<your_github_token>@github.com/gemshub/rkt4gems.git -b rkt-server
```

Then build rkt4gems as described in README.md in this folder (needs Python3 and miniconda installed). 

Sidebar: To install miniconda, perform steps described in
https://www.how2shout.com/linux/how-to-install-miniconda-on-ubuntu-20-04-lts-focal/

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

Briefly, to build rkt4gems:

```
cd ~/git/GEMS-Reaktoro/rkt4gems/
conda devenv
conda activate rkt4gems
cmake -S . -B build
cmake --build build --parallel
```

```
cd gemsreaktoro/build/bin
./rkt4gems-client example1/example1-dat.lst example1/example1-dbr.lst
```

To test rkt-server you can use ```./gems3k-client example1/example1-dat.lst example1/example1-dbr.lst```
Client sends to the server the same files as into  ```rkt4gems/resources/``` and gets back the result dbr json string.

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
