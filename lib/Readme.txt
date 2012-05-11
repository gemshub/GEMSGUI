Win32: for GEMS developers without Qt Enterprise license

Place in this directory: 

qtmt301.lib 
qtmain.lib   - binary Qt Toolkit libraries for Win32 (for linking only)

gemviz.lib  - compiled from /vizor source code tree using Qt headers - 
              must be provided by Qt-licensed developers; 
              used for linking only.

gemsel.lib  - compiled by all GEMS developers from /mods source tree 
              for linking with gemviz.lib and qtmt301.dll 

To re-compile from /mods, first delete gemsel.lib

To re-compile from /vizor, delete gemviz.lib (Qt-licensed developers only)

To run gems.exe, qtmt301.dll and cc3250mt.dll files are necessary
(can be placed in the same directory with gems2.exe).


