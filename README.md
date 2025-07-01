# GEM-Selektor graphical user interface (GEMSGUI)

This repository contains the source code and default resources for [GEM-Selektor](https://gemshub.github.io/site/) with graphical user interface (GEMSGUI)- the interactive package for thermodynamic modelling of aquatic (geo)chemical systems by Gibbs Energy Minimization using the GEM-Standalone ![Conda Version](https://img.shields.io/conda/v/conda-forge/gems3k?label=gems3k) chemical solver.

[![ci-build](https://github.com/gemshub/GEMSGUI/actions/workflows/ci-build.yml/badge.svg)](https://github.com/gemshub/GEMSGUI/actions/workflows/ci-build.yml)

- [Building using Conda](#building-using-conda)
- [How to build on Linux or Mac OS](#how-to-build-the-gem-selektor-application-using-qt-on-linux-or-mac-os)
- [How to run the application](#how-to-run-the-application)
- [How to build and run GEM-Selektor application using Qt Creator](#how-to-build-and-run-gem-selektor-application-using-qt-creator)
- [How to use the application](#how-to-use-gem-selektor-application)


## Briefly about GEM-Selektor v.3 

Distributed "as is" by the Laboratory for Waste Management (LES) of the Paul Scherrer Institute (PSI) with two purposes:

* to promote the GEM method and software into research community;

* to gather the users feedback - vital for making the software more functional and reliable.

Permission to use the GEM-Selektor software is hereby granted free of charge for educational and research purposes, subject to acceptance of Terms and Conditions of Use. In particular, in any publication of your results obtained using the GEM-Selektor code, please, cite the web page (http://gems.web.psi.ch) and the following papers: 

* Kulik D.A., Wagner T., Dmytrieva S.V., Kosakowski G., Hingerl F.F., Chudnenko K.V., Berner U. (2013): GEM-Selektor geochemical modeling package: revised algorithm and GEMS3K numerical kernel for coupled simulation codes. Computational Geosciences 17, 1-24.

* Wagner T., Kulik D.A., Hingerl F.F., Dmytrieva S.V. (2012): GEM-Selektor geochemical modeling package: TSolMod library and data interface for multicomponent phase models. Canadian Mineralogist 50, 1173-1195.

Qt widget application written in C++ and [Qt6](https://www.qt.io/).

Latest version: ![GitHub Tag](https://img.shields.io/github/v/tag/gemshub/GEMSGUI?label=GEMSGUI)

GEM-Selektor graphical user interface (GEMSGUI) is available under GNU General Public License (GPL) v3.

## Build and run GEMSGUI from source code

### Clone (download) the GEMSGUI source code

* Open a terminal and in the command line run:

```sh
git clone https://github.com/gemshub/GEMSGUI.git

```

### Building using Conda

GEM-Selektor can be easily built using [Conda](https://conda.io/docs/) package manager. Conda can be installed from [Miniconda](https://conda.io/miniconda.html) or [Miniforge](https://github.com/conda-forge/miniforge/releases).
Before building `GEMSGUI`, make sure you have the `conda-devenv` tool installed. Otherwise, run the following command in a terminal (assuming here that `conda` is already installed!):

```sh
conda install conda-devenv
```

We will use this tool now to create the `GEMSGUI` conda environment which contains all the dependencies for building `GEMSGUI` code. In the terminal, do:

```sh
cd path/to/GEMSGUI
conda devenv
```

Once this step is successfully completed, you can activate the `GEMSGUI` conda environment as follows:

```sh
conda activate GEMSGUI
```

Now it's time to use `cmake` to configure and build the GEM-Selektor application:

```sh
cmake -S . -B build
cmake --build build --parallel
```

This step will produce `gem-selector` application inside the created directory `GEMSGUI/build/App`:

Now you can run the [GEMSGUI](#how-to-run-the-gem-selektor-application) application


### How to build the GEM-Selektor application using Qt on Linux or Mac OS

* Make sure that [Qt6](https://www.qt.io/try-qt) is installed in your system.

* Make sure you have g++, cmake and git installed. If not, install them.  On Ubuntu linux terminal, this can be done using the following commands:

```sh
#!bash
sudo apt-get install g++ cmake git libssl-dev libtool byacc flex
```

For Mac OSX, make sure you have Homebrew installed (see [Homebrew web site](http://brew.sh) ).

* Install Dependencies

```sh
#!bash
cd ~/GEMSGUI
sudo ./install-dependencies.sh
```

* Then navigate to the directory where this README.md file is located and type in terminal:

```sh
mkdir -p build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=<deploy_path>  -DCMAKE_PREFIX_PATH=/home/<you>/Qt/<version>/gcc_64
make -j 4
```

### How to run the application

* To run Qt GEM-Selektor application, execute the following (or use *GEMSGUI/gems3gui.pro* with QtCreator):

```sh
./GEMSGUI/build/App/gem-selektor
```

* For more details about command line parameters, see into *GEMSGUI/Resources/rungems3.sh* or *GEMSGUI/Resources/rungems3.bat*

```sh
#  Change the path to the actual location of gem-selektor executable and Resources
cd ./Gems3-app/bin

# 1. First launch with default location of modeling projects (usually done by the installer)
# ./gem-selektor -d > gems3.log

# 2. Normal runs in default locations (retains all settings from previous session)
./gem-selektor > gems3.log
# or
# ./gem-selektor -s . -u \home\<you>\Library\Gems3 > gems3.log

# 3. New file configuration if project subfolder(s) were added/removed to/from
#   /projects or if /projects are not in the default location
# ./gem-selektor -c -s <Path_for_Resources> -u <My_GEMS_Projects_Location>\Gems3 > gems3.log

# 4. New \data\*.ini files - remake DOD and module dialog configurators
# Otherwise, does the same as  gems3 -c
# ./gem-selektor.exe -d -s . -u \home\<USER>\Library\Gems3 > gems3.log

# 5. Add -f after gem-selektor for a write access to database files in \DB.default\
# (for developers only!)
```

### How to build and run GEM-Selektor application using Qt Creator

Mace sure you have [Qt6](https://www.qt.io/try-qt) with Wt Creator installed. Start Qt Creator and open `gems3gui.pro` (using prebuilt gems3k) or `gems3gui_with_gems3k_source.pro` (for building together with gems3k code).

Select the desired build and run locations. Copy folder `Resources` to the build location.

In the Qt project Run settings in the command line arguments add e.g., `-d -f -s /home/<user>/path/to/build-gems3gui-Desktop_Qt`. Help on the arguments can be found in `Resources/doc/html/gems_setup.html`


## How to use GEM-Selektor application

For information on using  application take a look into the https://gemshub.github.io/site/.



