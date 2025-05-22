
mkdir tmp_velo
cd tmp_velo

echo
echo ******                    ******
echo ****** Compiling gems3k ******
echo ******                    ******
echo

echo git clone gems3k...
git clone https://github.com/sdmytrievs/GEMS3K.git -b no_client_mode
cd GEMS3K

echo "Configuring..."
cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_GEM2MT=OFF  -DBUILD_SOLMOD=OFF -DCMAKE_INSTALL_PREFIX:PATH="%CONDA_PREFIX%\Library" -A x64 -S . -B build
echo "Building..."
cmake --build build --target install  --config Release

cd ..\..

REM Housekeeping
rd /s /q tmp_velo
