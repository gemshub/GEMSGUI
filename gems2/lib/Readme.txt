Place in this directory: 

qt.lib      - a binary Qt Toolkit library for Win32 (for linking only)

gemviz.lib  - compiled from /vizor source code tree using Qt headers by 
              Qt-licensed developers; 
              used for linking only by scientists without Qt licenses;

gemsel.lib  - compiled by all GEMS developers from /mods source tree 
              for linking with gemviz.lib and qt.lib 

To re-compile from /mods, first delete gemsel.lib
To re-compile from /vizor, delete gemviz.lib (for Qt-licensed developers)
