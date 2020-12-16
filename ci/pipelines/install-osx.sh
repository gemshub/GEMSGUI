if [ ! -f $HOME/miniconda/bin/conda ]; then
    echo "Downloading and installing miniconda"
    wget -O miniconda.sh https://repo.continuum.io/miniconda/Miniconda3-latest-MacOSX-x86_64.sh
    rm -rf $HOME/miniconda
    bash miniconda.sh -b -p $HOME/miniconda
fi
if [ ! -f $HOME/miniconda/bin/conda ]; then
    echo ERROR: conda was not installed.
    exit 1
fi
bash $HOME/miniconda/etc/profile.d/conda.sh
export PATH=$HOME/miniconda/bin/:$PATH
conda config --set always_yes yes --set changeps1 no
conda config --add channels conda-forge
conda install conda-devenv
conda update -q conda
conda info -a
conda devenv
source activate GEMSGUI
mkdir build
cd build
# Configure step
cmake -GNinja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_LIBDIR=lib \
    ..
if [ $? -eq 0 ]
then
    echo "The cmake step ran OK"
else
    echo "The cmake step failed" >&2
    exit 1
fi
ninja install
if [ $? -eq 0 ]
then
    echo "The make step ran OK"
else
    echo "The make step failed" >&2
    exit 1
fi
conda list
