//-------------------------------------------------------------------
// $Id$
//
// Declaration of TConfig
//
// Copyright (C) 1996-2001 A.Rysin
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

#ifndef __config_h_
#define __config_h_

#include <fstream>
#include "gstring.h"
using namespace std;

/*
  errors:  (output # of line)
  	- Duplicated names
   - Wrong number of arguments
   - Identifier is too long
   - Line is too long
   - Wrong chars in identifier
*/

class TConfig
{
    const size_t token_len;
    char space;	// '"' for gstrings w/spaces
    //  gstring section;
    int sec_beg;
    int ssec_beg;

    fstream ini;
//    ifstream ini;
    int nLine;
    gstring line;
    unsigned valPos;
    char style;  // '=' or SPACE as delimiter

    bool getLine();
    bool findParam(const char* str);
    int getName();
    gstring getToken();
    void writegstring(gstring&);
    gstring readSectionName();
    gstring readSubSectionName();
    void reset(int p=0)
    {
        ini.clear();
        ini.seekg(p);
        nLine=0;
    }	// ! if(p!=0) nLine=?

public:

    TConfig(const char *fname, char style='=', int tok_ln=55);

    gstring GetFirstSubSection();
    gstring GetNextSubSection();
    bool SetSection(const gstring& s);
    gstring getFirst();
    gstring getNext();

    // get current values ( after getNext() )
    // no errors are verified
    int getcInt();
    bool getcStr(gstring& str);
    bool getcVals(int n, int vals[]);
    bool getcStrings(int n, gstring strs[]);

    int getInt(const char* par, int def=0);
    bool getStr(const char* par, gstring& str, char* def=0);
    bool getVals(const char* par, int n, int vals[], int defs[]=0);
    bool getStrings(const char* par, int n, gstring strs[], char* str[]=0);

    struct EBadSection
        {}
    ;
};


#endif      // __config_h


