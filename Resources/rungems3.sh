#  Change the path to the actual location of gem-selektor executable and Resources
cd ./Gems3-app/bin

# 1. First launch with default location of modeling projects (usually done by the installer)
# ./gem-selektor -d > gems3.log

# 2. Normal runs in default locations (retains all settings from previous session)
./gem-selektor > gems3.log
# or
# ./gem-selektor -s . -u \home\<USER>\Library\Gems3 > gems3.log

# 3. New file configuration if project subfolder(s) were added/removed to/from
#   /projects or if /projects are not in the default location
# ./gem-selektor -c -s <Path_for_Resources> -u <My_GEMS_Projects_Location>\Gems3 > gems3.log

# 4. New \data\*.ini files - remake DOD and module dialog configurators
# Otherwise, does the same as  gems3 -c
# ./gem-selektor.exe -d -s . -u \home\<USER>\Library\Gems3 > gems3.log

# 5. Add -f after gem-selektor for a write access to database files in \DB.default\
# (for developers only!)

