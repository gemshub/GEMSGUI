#!/bin/bash

# Uncomment what is necessary to reinstall by force 
rm -f ${CONDA_PREFIX}/bin/gems3k-server

BRANCH_JSON=master

# if no gemsreaktoro installed in ${CONDA_PREFIX}/include/gemsreaktoro
test -f ${CONDA_PREFIX}/bin/gems3k-server || {

	# Building jsonio library
	mkdir -p ~/code && \
		cd ~/code && \
                git clone https://github.com/gemshub/gemsreaktoro.git && \
                cd gemsreaktoro && \
		mkdir -p build && \
		cd build && \
                cmake .. -DCMAKE_CXX_FLAGS=-fPIC -DCMAKE_BUILD_TYPE=Release -DBuildClientExample=OFF -DBuildGUI=OFF && \
		make  && \
		make install

	# Removing generated build files
	cd ~ && \
		 rm -rf ~/code
}

