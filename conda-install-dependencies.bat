
mkdir tmp_velo
cd tmp_velo

echo
echo ******                    ******
echo ****** Compiling ChemicalFun ******
echo ******                    ******
echo

echo git clone ChemicalFun...
git clone https://bitbucket.org/gems4/chemicalfun.git
cd chemicalfun

echo "Configuring..."
cmake -DCMAKE_BUILD_TYPE=Release -DCHEMICALFUN_BUILD_EXAMPLES=OFF -DCHEMICALFUN_BUILD_TESTS=OFF -DCHEMICALFUN_BUILD_PYTHON=OFF -DCMAKE_INSTALL_PREFIX:PATH="%CONDA_PREFIX%\Library" -A x64 -S . -B build
echo "Building..."
cmake --build build --target install  --config Release
cd ..

echo
echo ******                    ******
echo ****** Compiling thermofun ******
echo ******                    ******
echo

echo git clone thermofun...
git git clone https://bitbucket.org/gems4/thermofun.git
cd thermofun

echo "Configuring..."
cmake -DCMAKE_BUILD_TYPE=Release -DTFUN_BUILD_TESTS=OFF -DTFUN_BUILD_PYTHON=OFF -DCMAKE_INSTALL_PREFIX:PATH="%CONDA_PREFIX%\Library" -A x64 -S . -B build
echo "Building..."
cmake --build build --target install  --config Release
cd ..

echo
echo ******                    ******
echo ****** Compiling gems3k ******
echo ******                    ******
echo

echo git clone gems3k...
git clone https://github.com/sdmytrievs/GEMS3K.git
cd GEMS3K

echo "Configuring..."
cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_GEM2MT=OFF -DCMAKE_INSTALL_PREFIX:PATH="%CONDA_PREFIX%\Library" -A x64 -S . -B build
echo "Building..."
cmake --build build --target install  --config Release

cd ..\..

REM Housekeeping
rd /s /q tmp_velo
