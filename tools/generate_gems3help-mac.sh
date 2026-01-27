cd tools
./gems3helpconfig-qhp-generator
$(brew --prefix qt)/bin/qhelpgenerator gems3helpconfig.qhcp -o Resources/help/gems3help.qhc
cp -r Resources/help ../../Resources
cd ..
#/usr/share/miniconda/envs/GEMSGUI/lib/qt6/qhelpgenerator


