#!/bin/bash
#
# DK: gemsdeployqtapp.sh - a modified/extended deployqtapp.sh from W.Hallatt 
#
#================================================================================
# Copyright (c) 2012 - 2013 by William Hallatt.
# 
# This script is free software: you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# This script is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with
# this script (GNUGPL.txt).  If not, see
#
#                    <http://www.gnu.org/licenses/>
# 
# Should you wish to contact me for whatever reason, please do so via:
#
#                 <http://www.goblincoding.com/contact>
# 
#================================================================================

# Copy your executable as well as this script to the directory where you 
# want to create the final tar ball.  To execute, simply pass the name of the
# executable as command line parameter to this script.
#
# Worth noting is that I built Qt from source myself, you may or may not need 
# additional plugins, etc and different or additional directory structures and
# will have to edit this script to suit your needs!

if [ $# -ne 1 ]
then
        echo "Usage: $0 <executable name>"
        exit 1
fi

executable=$1

# Obtain the Linux flavour and version.
distro=`lsb_release -d | awk '{print $2$3$4}' | sed 's/\./_/g'`

# Create the directory that will be tarred up for distribution.
etardir=`echo $executable"_"$distro | awk '{print tolower($0)}'`
tardir=Gems3-app
gemsdistro=Gems3.6.0-d4802726.b4d97076-linux-x86-64-ubuntu.tgz
mkdir $tardir
echo "Created tar ball directory: "$tardir

# Copy executable across.
chmod u+x $executable
cp $executable $tardir
echo "Copied executable "$executable" to "$tardir

# Create the libs directory.
libsdir=$PWD/$tardir/libs
mkdir $libsdir 
echo "Created libs directory: "$libsdir

# Copy all dependencies across to the tar directory.
echo "Copying dependencies..."

for dep in `ldd ./$executable | awk '{print $3}' | grep -v "("`
do
  cp $dep $libsdir
  echo "Copied dependency "$dep" to "$libsdir
done

dep=$HOME/Qt/5.12.5/gcc_64/lib/libQt5Concurrent.so.5
cp $dep $libsdir
dep=$HOME/Qt/5.12.5/gcc_64/lib/libQt5DBus.so.5
cp $dep $libsdir
dep=$HOME/Qt/5.12.5/gcc_64/lib/libQt5Xml.so.5
cp $dep $libsdir
dep=$HOME/Qt/5.12.5/gcc_64/lib/libQt5Sql.so.5
cp $dep $libsdir
dep=$HOME/Qt/5.12.5/gcc_64/lib/libQt5Charts.so.5
cp $dep $libsdir
dep=$HOME/Qt/5.12.5/gcc_64/lib/libQt5Help.so.5
cp $dep $libsdir
dep=$HOME/Qt/5.12.5/gcc_64/lib/libQt5X11Extras.so.5
cp $dep $libsdir

# Create the fonts directory and copy fonts across. You
# will obviously need to assign the directory path leading
# to your fonts to "fontdir", e.g. /home/you/qt/lib/fonts
#qtfontsdir=$HOME/Qt/5.12.5/Src/qtbase/lib/fonts
#fontsdir=$PWD/$tardir/fonts
#mkdir $fontsdir
#echo "Created fonts directory: "$fontsdir" copying fonts..."
#cp -r $qtfontsdir/* $fontsdir

# You will need to change this to point to wherever libqxcb.so lives on your PC.
qtplatformplugin=$HOME/Qt/5.12.5/gcc_64/plugins/platforms/*.so
qtplatformplugindir=$tardir/platforms
mkdir $qtplatformplugindir
echo "Created platforms directory: "$qtplatformplugindir
cp $qtplatformplugin $qtplatformplugindir
echo "Copied platforms "$qtplatformplugin" to "$qtplatformplugindir

# Create the script to fix xcb dependencies.
fixscript=$qtplatformplugindir/fixdep.sh
echo "Creating fixdep script: "$fixscript

echo "#!/bin/sh" >> $fixscript
echo "" >> $fixscript
echo "#================================================================================" >> $fixscript
echo "# Copyright (c) 2012 - 2013 by William Hallatt." >> $fixscript
echo "#" >> $fixscript
echo "# This script is free software: you can redistribute it and/or modify it under" >> $fixscript
echo "# the terms of the GNU General Public License as published by the Free Software" >> $fixscript
echo "# Foundation, either version 3 of the License, or (at your option) any later" >> $fixscript
echo "# version." >> $fixscript
echo "#" >> $fixscript
echo "# This script is distributed in the hope that it will be useful, but WITHOUT" >> $fixscript
echo "# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS" >> $fixscript
echo "# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details." >> $fixscript
echo "#" >> $fixscript
echo "# You should have received a copy of the GNU General Public License along with" >> $fixscript
echo "# this script (GNUGPL.txt).  If not, see" >> $fixscript
echo "#" >> $fixscript
echo "#                    <http://www.gnu.org/licenses/>" >> $fixscript
echo "#" >> $fixscript
echo "# Should you wish to contact me for whatever reason, please do so via:" >> $fixscript
echo "#" >> $fixscript
echo "#                 <http://www.goblincoding.com/contact>" >> $fixscript
echo "#" >> $fixscript
echo "#================================================================================" >> $fixscript
echo "" >> $fixscript
echo "# This script will install all \"not found\" dependencies for ALL libraries " >> $fixscript
echo "# that exist within the directory from where it is executed. " >> $fixscript
echo "" >> $fixscript
echo "# All of the packages thus installed can be removed through running the generated " >> $fixscript
echo "# \"removedep.sh\" script if you wish to undo these changes." >> $fixscript
echo "" >> $fixscript
echo "removescript=removedep.sh" >> $fixscript 
echo "> \$removescript" >> $fixscript
echo "echo \"#!/bin/sh\" >> \$removescript" >> $fixscript
echo "" >> $fixscript
echo "# For each library in this directory..." >> $fixscript
echo "for lib in \`ls -F | grep -v @ | grep *.so\`" >> $fixscript
echo "do" >> $fixscript
echo "  # Determine the dependencies, find only those that are not on the system, exclude all Qt5 libraries," >> $fixscript
echo "  # print the name of the library (not full path) and extract the package name from the .so name itself." >> $fixscript
echo "  lddquery=\`ldd \$lib | grep -i \"not found\" | grep -i -v qt5 | awk '{print \$1}' | sed 's/\\(^.*\\)\\.so.*\$/\\1/'\`" >> $fixscript
echo "  #echo \$lddquery" >> $fixscript
echo "" >> $fixscript
echo "  echo \"Installing dependencies for library: \"\$lib" >> $fixscript
echo "  #echo \$lib" >> $fixscript
echo "" >> $fixscript
echo "  for dep in \$lddquery" >> $fixscript
echo "  do" >> $fixscript
echo "    echo \"Found dependency: \"\$dep" >> $fixscript
echo "" >> $fixscript
echo "    # Query apt-cache for the relevant package, excluding dev and dbg versions." >> $fixscript
echo "    packagequery=\`apt-cache search \$dep | grep -v dev | grep -v dbg | awk '{print \$1}'\`" >> $fixscript
echo "    echo \"Querying package list for: \"$packagequery" >> $fixscript
echo "" >> $fixscript
echo "    sudo apt-get install \$packagequery" >> $fixscript
echo "    echo \"Installing: \"\$packagequery" >> $fixscript
echo "    echo \"sudo apt-get remove \$packagequery\" >> \$removescript" >> $fixscript 
echo "  done" >> $fixscript
echo "done" >> $fixscript
echo "" >> $fixscript
echo "chmod u+x \$removescript" >> $fixscript

chmod u+x $fixscript

# Edit this script to add whatever other additional plugins your application
# requires. Fixed to skip debug variants of plugins
qtsqliteplugin=$HOME/Qt/5.12.5/gcc_64/plugins/sqldrivers/*.so
qtsqliteplugindir=$tardir/sqldrivers
mkdir $qtsqliteplugindir
echo "Created sql driver directory: "$qtsqliteplugindir
cp $qtsqliteplugin $qtsqliteplugindir
echo "Copied "$qtsqliteplugin" to "$qtsqliteplugindir

qtbearerplugin=$HOME/Qt/5.12.5/gcc_64/plugins/bearer/*.so
qtbearerplugindir=$tardir/bearer
mkdir $qtbearerplugindir
echo "Created bearer plugin directory: "$qtbearerplugindir
cp $qtbearerplugin $qtbearerplugindir
echo "Copied "$qtbearerplugin" to "$qtbearerplugindir

qticonenginesplugin=$HOME/Qt/5.12.5/gcc_64/plugins/iconengines/*.so
qticonenginesplugindir=$tardir/iconengines
mkdir $qticonenginesplugindir
echo "Created iconengines plugin directory: "$qticonenginesplugindir
cp $qticonenginesplugin $qticonenginesplugindir
echo "Copied "$qticonenginesplugin" to "$qticonenginesplugindir

qtprintsupportplugin=$HOME/Qt/5.12.5/gcc_64/plugins/printsupport/*.so
qtprintsupportplugindir=$tardir/printsupport
mkdir $qtprintsupportplugindir
echo "Created printsupport driver directory: "$qtprintsupportplugindir
cp $qtprintsupportplugin $qtprintsupportplugindir
echo "Copied "$qtprintsupportplugin" to "$qtprintsupportplugindir

qtimageformatsplugin=$HOME/Qt/5.12.5/gcc_64/plugins/imageformats/*.so
qtimageformatsplugindir=$tardir/imageformats
mkdir $qtimageformatsplugindir
echo "Created imageformats plugin directory: "$qtimageformatsplugindir
cp $qtimageformatsplugin $qtimageformatsplugindir
echo "Copied "$qtimageformatsplugin" to "$qtimageformatsplugindir

$qtstylesplugin=$HOME/Qt/5.12.5/gcc_64/plugins/styles/*.so
$qtstylesplugindir=$tardir/styles
$mkdir $qtstylesplugindir
$echo "Created styles plugin directory: "$qtstylesplugindir
$cp $qtstylesplugin $qtstylesplugindir
$echo "Copied "$qtstylesplugin" to "$qtstylesplugindir

# Create the run script.
execscript=$tardir/"run$executable.sh"
#echo "Created run script: "$execscript
#echo "#!/bin/sh" >> $execscript
#echo "export LD_LIBRARY_PATH=\`pwd\`/libs" >> $execscript
#echo "export QT_QPA_FONTDIR=\`pwd\`/fonts" >> $execscript
#echo "./$executable \`$\`*" >> $execscript

#echo "#!/bin/bash" >> $execscript
#echo "appname=gems3" >> $execscript
#echo "dirname=\`dirname $0\`" >> $execscript
#echo "if [ \"${dirname:0:1}\" != \"/\" ]; then" >> $execscript
#echo "  dirname=$PWD/$dirname" >> $execscript
#echo "fi" >> $execscript
#echo "export LD_LIBRARY_PATH=$dirname/libs" >> $execscript
#echo "export QT_QPA_FONTDIR=$dirname/fonts" >> $execscript
#echo "cd $dirname" >> $execscript
#echo "./$appname $*" >> $execscript

# Make executable.
#chmod u+x $execscript

# Create the Resources directory and copy Resources across.
# You will obviously need to assign the directory path leading
# to GEM-Selektor Resources e.g. /home/you/DevGEMS/trunk/gems-gui/Resources
gresourcesdir=$HOME/gitGEMS3/build-release/Resources
resourcesdir=$tardir/Resources
mkdir $resourcesdir
echo "Created Resources directory: "$resourcesdir", copying ..."
cp -r $gresourcesdir/* $resourcesdir
echo "Copied Resources directory from "$gresourcesdir" to "$resourcesdir

#Copying executables for generating GEM-Selektor help database
qtqcollectiongenerator=$HOME/Qt/5.12.5/gcc_64/bin/qcollectiongenerator
cp $qtqcollectiongenerator $tardir
echo "Copied executable "$qtqcollectiongenerator" to "$tardir
qtqhelpgenerator=$HOME/Qt/5.12.5/gcc_64/bin/qhelpgenerator
cp $qtqhelpgenerator $tardir
echo "Copied executable "$qtqhelpgenerator" to "$tardir

# Copying minor files needed to complete installation and start GEMS3
gbuilddir=$HOME/gitGEMS3/build-release
cp -r $gbuilddir/ToDesktop $tardir/ToDesktop
cp $gbuilddir/gems3.png $tardir/gems3.png
cp $gbuilddir/qtconf $tardir/qtconf
cp $gbuilddir/INSTALL.linux.txt $tardir/INSTALL.linux.txt
cp $gbuilddir/exec_gems3.sh $execscript
# qtconf=$tardir/qt.conf
# echo [Paths] >> $qtconf
echo "Copied run script: "$execscript 
chmod u+x $execscript

# Create a README
echo "Creating README..."

readme=$tardir/README
echo "================================================================================" >> $readme
echo "GEM-Selektor v.3.6.0 $etardir bult on linux $distro" >> $readme
echo "Please, read INSTALL.linux.txt file first, and do things as suggested there." >> $readme
echo "To have projects in /home/you/Library/Gems3 (default location," >> $readme 
echo "  launch once $executable via" >> $readme
echo "" >> $readme
echo "                 $execscript -d" >> $readme
echo "" >> $readme
echo "If you run into any problem regarding dependencies, all you need to do is to" >> $readme
echo "run " >> $readme
echo "                 $fixscript " >> $readme
echo "" >> $readme
echo "in order to automatically resolve dependencies on your behalf " >> $readme
echo "(note that you will need administrator privileges for this as this script will" >> $readme
echo "download the necessary libraries for your platform). " >> $readme
echo "" >> $readme
echo "Should you wish to contact us for any reason, please do so via Contacts in:" >> $readme
echo "" >> $readme
echo "                 <http://gems.web.psi.ch/termsofuse>" >> $readme
echo "" >> $readme
echo "================================================================================" >> $readme

echo "Creating tarball..."
tar -zcvf $gemsdistro $tardir

echo "Cleaning up..."
rm -rf $tardir
echo "Done!"

