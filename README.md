## GEM-Selektor v.3 for GEMS-Reaktoro project #

This GEMSGUI repository contains the source code and default resources for GEM-Selektor v.3 client in GEMS-Reaktoro benchmarking project
GEMSGUI is an interactive package for thermodynamic modelling of aquatic (geo)chemical systems by Gibbs Energy Minimization using a standalone server - either GEMS3K chemical solver or alternatively the rkt4gems chemical solver based on Reaktoro 2 framework.

### Briefly about GEM-Selektor v.3 ##

Distributed "as is" by the Laboratory for Waste Management (LES) of the Paul Scherrer Institute (PSI) with two purposes:

* to promote the GEM method and software into research community;
* to gather the users feedback - vital for making the software more functional and reliable.

Permission to use the GEM-Selektor software is hereby granted free of charge for educational and research purposes, subject to acceptance of Terms and Conditions of Use. In particular, in any publication of your results obtained using the GEM-Selektor code, please, cite the web page (http://gems.web.psi.ch) and the following papers:

* Kulik D.A., Wagner T., Dmytrieva S.V., Kosakowski G., Hingerl F.F., Chudnenko K.V., Berner U. (2013): GEM-Selektor geochemical modeling package: revised algorithm and GEMS3K numerical kernel for coupled simulation codes. Computational Geosciences 17, 1-24.
* Wagner T., Kulik D.A., Hingerl F.F., Dmytrieva S.V. (2012): GEM-Selektor geochemical modeling package: TSolMod library and data interface for multicomponent phase models. Canadian Mineralogist 50, 1173-1195.

Version: currently 3.9.5.

Learn more about GEM-Selektor from http://gems.web.psi.ch

## Instructions on how to build and set up GEMS-Reaktoro benchmarking tools on linux (e.g. ubuntu 20.04) ##

1. Install Python and miniconda (if not done already), e.g. performing steps described in

https://www.how2shout.com/linux/how-to-install-miniconda-on-ubuntu-20-04-lts-focal/

Check in terminal whether conda and python are already present (if yes then just versions of both will be reported in response):

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
git clone https://<your_github_token>@github.com/gemshub/rkt4gems.git -b rkt-server
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
example1
```

TBD
Add how to build and launch the rkt4gems server


3. To build and install the GEMS3K library (version working with ThermoFun):

Check the conda environment (activate if needed), and install chemicalfun and gems3k from conda forge

```
# conda activate rkt4gems
conda install chemicalfun -c conda-forge
conda install gems3k -c conda-forge
```

4. To build and start the gems3k-server of GEMS-Reaktoro (from git branch "rkt-server"):

```
cd /home/<your_user>/git/GEMS-Reaktoro
git clone https://<your_github_token>@github.com/gemshub/gemsreaktoro.git -b rkt-server
cd gemsreaktoro
mkdir -p build && cd build && cmake .. && make
```

To start the server, open a separate terminal window or tab, and execute:

```
cd ~/git/GEMS-Reaktoro/gemsreaktoro/build/bin
./gems3k-server

```

The gems3k-server runs in the background and listens to the client that can run on another computer or in a different process environment on this machine.
To stop the server (in terminal where it was started): enter Ctrl-C.

To run the standalone batch test gems3k-client with the data from example1:

Open a separate terminal window or tab, and execute:

```
cd ~/git/GEMS-Reaktoro/gemsreaktoro/build/bin
./gems3k-client example1/example1-dat.lst example1/example1-dbr.lst

```

The gems3k-client connects to the gems3k-server running on another computer or in a different process on this machine. It sends the input data for GEMS3K in ZeroMQ messages, the server calculates the GEM task and sends the results back to the client in a ZeroMQ message, then the standalone client prints the results and stops.


#### *Alternatively, one can build and run the gems3k-server and gems3k-client using Qt projects.*

##### How exactly? What Qt vestion and platform should be installed?

5. To build and run the gemsgui-app graphical client (branch rkt-server)

Install Qt5.15.4 from conda forge. Also install packages providing libGL (to prevent linking errors like "fatal error: GL/gl.h: No such file or directory" when building Qt GUI applications):

```
conda install -c conda-forge freeglut gsl glew glfw mesalib mesa-libgl-devel-cos7-x86_64 qt
```

Now, clone the GEMSGUI Git repository.

```
git clone https://<your_github_token>@github.com/gemshub/GEMSGUI.git -b rkt-server
cd GEMSGUI
mkdir -p build && cd build
cmake .. && make
```

To start the gems3gui GUI client, check the path to folder containing Resources (after the -s key of gemsgui-app), and then execute it:

```
cd App
./gemsgui-app -d -s /home/<your_user>/git/GEMS-Reaktoro/GEMSGUI
```

Note that the resources folder, e.g. /resources/projects/, should contain GEMS projects folders similar to those in Library/Gems3/projects for GEM-Selektor installation.

GEMS3K-server and rkt4gems-server must be run in different processes or on different computers.


##### Reached this point o.k.
GEMSGUI starts but shows all icons in low resolution, crashes when the calculation of the first equilibrium in the Kaolinite project is running. Also fails upon attempt to export the system in Fun-JSON format.

< To Svitlana: Please, finalise gemsreaktoro launcher so that it starts gems3k-server, rkt4gems-server and gemsgui-app, and kills them when shutdown of gemsreaktoro is performed>


6. To test rkt4gems-server (TBD)
```

To test rkt-server you can use ```./gems3k-client example1/example1-dat.lst example1/example1-dbr.lst```
Client sends to the server the same files as into  ```rkt4gems/resources/``` and gets back the result dbr json string.


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
