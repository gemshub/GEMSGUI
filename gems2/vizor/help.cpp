//-------------------------------------------------------------------
// $Id$
//
// Implementation of Helper class
//
// Copyright (C) 1996-2001  A.Rysin
// Uses  gstring class (C) A.Rysin 1999
//
// This file is part of the GEM-Vizor library which uses the
// Qt v.2.x GUI Toolkit (Troll Tech AS, http://www.trolltech.com)
// according to the Qt Duo Commercial license
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL)
//
// See http://les.web.psi.ch/Software/GEMS-PSI/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------
#include <fstream>
using namespace std;

#include "help.h"
#include "v_user.h"

Helper::Helper(const gstring& dir):
        path(dir)
{}

// searches for reference <ref> in the index <file>
//    returns true if exact match is found
//    returns false if not found and sets <ref> to HTML file
//    name where full description is to be browsed
//    ( null - if no match found )
//    <ref> should be in format "objectlabel_cellnum"
//    Fixed by DAK 05.01.01, 16.01.01 
bool
Helper::checkForRef(const gstring& file, gstring& ref)
{
    gstring fname = path;
    fname += file;

    ifstream is(fname.c_str());
    if( !is.good() )
        throw FileError();

    gstring ref1 = ref;
    gstring line;
    gstring nearest;
    gstring sBase("#");
    sBase += gstring(ref, 0, ref.rfind("_"));
    /// do smth if no "_" in <ref>
    gstring sExact("#");
    sExact += ref;
    sExact += "\">";
    ref = "";

    /// should be case insensitive  !!!
    while(1)
    {
        u_getline(is, line, '\n');
        if( is.eof() )
            break;
        if( !is.good() )
            throw FileError();

        size_t posf = line.find("a href=\"");  // 08.01.01
        if( posf == gstring::npos )
//        {   Netscape Composer produces lowercase <a href !
//           posf = line.find("A HREF=\"");
//           if( posf == gstring::npos )
               continue;
//        }
        posf += 8;
        gstring sBaseT = sBase + "_";    // Bugfix 16.01.01
        size_t pos = line.find( sBaseT );
        if( pos != gstring::npos )
        {
          ref = gstring(line, posf, pos-posf);  // File name
          if( line.find(sExact) != gstring::npos )
          { // Trying exact match
             ref += "#";
             ref += ref1;
             return true;
          }
          else continue;  // Try next line
        }
        else {  // Try without '_' but with '\"'
          sBaseT = sBase + "\"";
          pos = line.find( sBaseT );
          if( pos != gstring::npos )
          {
// Filename is found - take it to ref
            ref = gstring(line, posf, pos-posf);
            ref += sBase;
            return false;
          }
          else continue;  // Try next line 
       }
    }
//  ref = "";    Only file found, but not the object 
    return false;
}

// ----------------- End of help.cpp -----------------------

