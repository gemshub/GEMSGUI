//-------------------------------------------------------------------
// $Id: config.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of TConfig
//
// Copyright (C) 1996-2001 A.Rysin

//
// This file is part of the GEM-Selektor GUI library which uses the
// Qt v.4 cross-platform App & UI framework (https://qt.io/download-open-source)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the GPL v.3 license

//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#ifndef CONFIG_H_
#define CONFIG_H_

#include <string>

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
    const size_t token_len = 55;
    const std::string new_line ="\n";

    bool getLine();
    bool findParam(const char* str);
    size_t getName();

    std::string getToken();
    //std::string readSectionName();
    std::string readSubSectionName();

    void reset(size_t p=0)
    {
        cur_pos=p;
    }

public:

    TConfig( const std::string&fname, char style='=');

    std::string GetFirstSubSection();
    std::string GetNextSubSection();
    bool SetSection(const std::string& s);
    std::string getFirst();
    std::string getNext();

    // get current values ( after getNext() )
    // no errors are verified
    int getcInt();
    bool getcStr(std::string& str);
    bool getcVals(int n, int vals[]);
    bool getcStrings(int n, std::string strs[]);

    int getInt(const char* par, int def=0);
    bool getStr(const char* par, std::string& str, char* def=nullptr);
    bool getVals(const char* par, int n, int vals[], int defs[]=nullptr);
    bool getStrings(const char* par, int n, std::string strs[], char* str[]=nullptr);

    struct EBadSection
        {}
    ;

    std::string readSectionName();
protected:

    char space;	// '"' for strings w/spaces
    char style;  // '=' or SPACE as delimiter

    //  string section;
    size_t sec_beg;
    size_t ssec_beg;
    size_t cur_pos = 0;
    std::string line;
    size_t val_pos;

    std::string ini_data;

    std::string get_next_line();

    bool IsSpace(char ch)
    {
        return ( (ch == ' ') || (ch == '\t') );
    }
};


#endif      // CONFIG_H_


