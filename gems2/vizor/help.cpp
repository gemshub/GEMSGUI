//-------------------------------------------------------------------
// $Id$
//
// Implementation of Helper class
//
// Copyright (C) 1996-2001  A.Rysin, S.Dmytriyeva
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
//    <ref> should be in format:
//    "objectlabel[indexN][indexM]", if sizeN >1 and sizeM > 1
//    "objectlabel[][indexM]", if sizeN > 1 and sizeM > 1
//    "objectlabel[index]", if sizeN >1 or sizeM > 1
//    "objectlabel", if sizeN == 1 and sizeM == 1
//
//    Fixed by DS 17/02/2005
bool
Helper::checkForRef(const gstring& file, gstring& ref)
{
    gstring fname = path;
    fname += file;

    ifstream is(fname.c_str());
    if( !is.good() )
        throw FileError();

    gstring line;
    gstring ref1 = ref;

    size_t pos_name;
    size_t  pos_1 = ref1.find('[');
    size_t  pos_2 = ref1.rfind("[");
    size_t  pos_12 = ref1.find(']');
    size_t  pos_22 = ref1.rfind("]");
    if( pos_2 == pos_1 )
           pos_2 = pos_22 = gstring::npos;

    gstring o_name = ">";
            o_name += ref1.substr(0, pos_1 );
    gstring o_name2 = "";
    if( pos_2 != gstring::npos  )
    {
        o_name2 = o_name;
        o_name2 += "[]";
        o_name2 += ref1.substr( pos_2, pos_22-pos_2 );
    }
    gstring name = o_name;
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
               continue;
        posf += 8;

        if( !o_name2.empty() )
          if( line.find( o_name2 ) != gstring::npos )
          {  // include line such: ">keywd[][m]"
            ref = line.substr( posf );
            ref = ref.substr( 0, ref.find('\"') );
            return true;
          }
ID:
        pos_name = line.find( o_name );
        if( pos_name != gstring::npos )
        {

           size_t poss = o_name.length();
                  poss+= pos_name;
           if( line[poss] !=  '<'&&
               line[poss] !=  '[' )
             continue; // finding full name, not subtring
           ref = line.substr( posf );
           ref = ref.substr( 0, ref.find('\"') );
           if( pos_1 == gstring::npos )
            { // all reference finding
               return true;
            }
            else
            { // add index
              o_name += ref1.substr( pos_1, pos_12-pos_1+1);
              pos_1 = pos_2;
              pos_12 = pos_22;
              pos_2 = pos_22 = gstring::npos;
              goto ID;
            }
         }
        else
           if( !ref.empty() )
           {
              pos_name = line.find( name );
              if( pos_name == gstring::npos ) // next object, return nearest
                   return true;
           }

      }
      return false;
}

/* Sveta 16/02/2005
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
*/
// ----------------- End of help.cpp -----------------------

