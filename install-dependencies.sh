#!/bin/bash
# Installing dependencies needed to build on (k)ubuntu linux 20.04

if [ "$(uname)" == "Darwin" ]; then

    # Do under Mac OS X platform
    #Needs Xcode and ArangoDB server locally installed
    brew upgrade
    brew install cmake
    #brew install spdlog

    EXTN=dylib

# spdlog 
# if no spdlog installed in /usr/local/lib/ (/usr/local/include/spdlog)
test -d /usr/local/include/spdlog || {

        # Building spdlog library
        mkdir -p ~/code && \
                cd ~/code && \
                git clone https://github.com/gabime/spdlog -b v1.10.0
        cd spdlog && mkdir -p build && \
                cd build && \
                cmake .. -DCMAKE_CXX_FLAGS=-fPIC && \
                make
        sudo make install

        # Removing generated build files
        cd ~ && \
                 rm -rf ~/code
}

elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then

    #Needs gcc v.5 or higher and ArangoDB server locally installed
    sudo apt-get update
    sudo apt-get install libspdlog-dev

    EXTN=so
fi

# Uncomment what is necessary to reinstall by force 
#sudo rm -f /usr/local/include/nlohmann/json.hpp
#sudo rm -rf /usr/local/include/eigen3/Eigen
#sudo rm -rf /usr/local/include/pybind11
#sudo rm -rf /usr/local/include/spdlog
#sudo rm -f /usr/local/lib/libChemicalFun.$EXTN
#sudo rm -f /usr/local/lib/libThermoFun.$EXTN

threads=3
workfolder=${PWD}
BRANCH_TFUN=master
BUILD_TYPE=Release

# nlohmann/json
test -f /usr/local/include/nlohmann/json.hpp || {

	# Building json library
	mkdir -p ~/code && \
        cd ~/code && \
        git clone https://github.com/nlohmann/json.git && \
        cd json && \
        mkdir -p build && \
        cd build && \
        cmake .. -DJSON_BuildTests=OFF && \
        make && \
        sudo make install

	# Removing generated build files
	cd ~ && \
        rm -rf ~/code
}


#Pybind11
test -d /usr/local/include/pybind11 || {

        # Building pybind11 library
        mkdir -p ~/code && \
        cd ~/code && \
        git clone https://github.com/pybind/pybind11.git && \
        cd pybind11 && \
        mkdir -p build && \
        cd build && \
        cmake .. -DPYBIND11_TEST=OFF && \
        make && \
        sudo make install

        # Removing generated build files
        cd ~ && \
        rm -rf ~/code
}

# Eigen
# Eigen3 math library (added for building and installing xGEMS)
# if not installed in /usr/local/include/eigen3)
test -d /usr/local/include/eigen3/Eigen || {

       # Downloading and unpacking eigen3 source code into ~/code/eigen
       mkdir -p ~/code && \
       cd ~/code && \
       git clone https://gitlab.com/libeigen/eigen.git -b 3.4.0 && \
       cd eigen && \
       mkdir -p build && \
       cd build && \
       cmake .. && \
       sudo make install

       # Removing generated build files
       cd ~ && \
       rm -rf ~/code
}

# ChemicalFun library
# if no ChemicalFun installed in /usr/local/lib/ (/usr/local/include/ChemicalFun)
test -f /usr/local/lib/libChemicalFun.$EXTN || {

        # Building chemicalfun library
        mkdir -p ~/code && \
        cd ~/code && \
        git clone https://bitbucket.org/gems4/chemicalfun.git -b $BRANCH_TFUN  && \
        cd chemicalfun && \
        mkdir -p build && \
        cd build && \
        cmake .. -DCMAKE_CXX_FLAGS=-fPIC -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DCHEMICALFUN_BUILD_PYTHON=OFF && \
        make -j $threads && \
        sudo make install

        # Removing generated build files
        cd ~ && \
        rm -rf ~/code
}


# ThermoFun library 
# if no ThermoFun installed in /usr/local/lib/libThermoFun.a (/usr/local/include/ThermoFun)
test -f /usr/local/lib/libThermoFun.$EXTN || {

	# Building thermofun library
	mkdir -p ~/code && \
        cd ~/code && \
        git clone https://github.com/thermohub/thermofun.git -b $BRANCH_TFUN && \
        cd thermofun && \
        mkdir -p build && \
        cd build && \
        cmake .. -DCMAKE_CXX_FLAGS=-fPIC -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DTFUN_BUILD_PYTHON=OFF && \
        make -j $threads && \
        sudo make install

	# Removing generated build files
	cd ~ && \
        rm -rf ~/code
}

if [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
   sudo ldconfig
fi
# on MacOS, this gives a syntax error
