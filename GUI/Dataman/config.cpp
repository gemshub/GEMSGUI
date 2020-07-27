//-------------------------------------------------------------------
// $Id: config.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of TConfig
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

#include <cstdlib>
#include <cctype>

#include "config.h"
#include "v_user.h"

TConfig::TConfig(const char *fname, char st, const size_t tok_ln):
        token_len(tok_ln),
        space(' '),
        ini(fname),
        style(st)
{
    if( !ini.good() )
    {
     Error( fname, "Error opening file!");
    }
    sec_beg = ssec_beg = 0;
}


bool
TConfig::getLine()
{
    while( 1 )
    {
        u_getline(ini, line, '\n');
        if( ini.eof() )
            return false;
        ErrorIf( !ini.good(), "Config", "Error reading file!");
        //  readline(ini,line);
        nLine++;
        //    line.strip(/*string::Both*/);
        StripLine( line );
        // empty lines and comments are cut out
        if( line.empty() )
            continue;
        if( line[0] != ';' && line[0] != '#' )
            break;
    }
    return true;
}

string TConfig::readSectionName()
{
    //int cpos;
    while( 1 )
    {
        if( !getLine() )
            return "";
        if( line[0]=='[' )
            break;
    }
    auto cpos = ini.tellg();
    size_t ii;
    for( ii = 0; ii<line.length(); ii++ )
        if( line[ii] == ']' )
            break;
    if( ii == line.length() )
        throw EBadSection();

    sec_beg = cpos;
    ssec_beg = 0;
    string sec(line, 1, ii-1);
    StripLine(sec);
    return sec;
}

string
TConfig::readSubSectionName()
{
    //int cpos;
    while( 1 )
    {
        if( line[0]=='[' )
            return "";
        if( line[0]=='<' )
            break;
        // get line after 'cause it's already read by readSection()
        if( !getLine() )
            return "";
    }
    auto cpos = ini.tellg();
    size_t ii;
    for( ii = 0; ii<line.length(); ii++ )
        if( line[ii] == '>' )
            break;
    if( ii == line.length() )
        throw EBadSection();

    ssec_beg = cpos;
    string sec(line, 1, ii-1);
    StripLine(sec);
    return sec;
}

string
TConfig::GetFirstSubSection()
{
    reset(sec_beg);
    if( !getLine() )
        return "";
    return GetNextSubSection();
}

string
TConfig::GetNextSubSection()
{
    return readSubSectionName();
}

size_t
TConfig::getName()
{
    size_t ii, jj;
    for( ii=0; ii<line.length(); ii++ )
    {
        if( !isalnum(line[ii]) && line[ii]!='_' && line[ii]!='~')
            break;

        if( ii >= token_len )
            return valPos=0;
    }

    for( jj=ii; jj<line.length(); jj++ )
        if( line[jj] != style && !IsSpace(line[jj]) )
            break;
    valPos = jj;

    return ii;
}

bool
TConfig::findParam(const char* par)
{
    // speedup if getNext was previous &
    // intruding loopback if repeated!!!
    //  int pos = getName();
    //  if( pos != 0  &&  string(line,pos) == par )
    //    return true;
    string s;
    do
    {
        s = getNext();
        if( s == par )
            return true;
    }
    while( !s.empty() );

    s = getFirst();
    if( s == par )
        return true;
    while( !s.empty() )
    {
        s = getNext();
        if( s == par )
            return true;
    }

    return false;
}


// sets Section name for getFirst(), getNext(), get...()
bool
TConfig::SetSection(const string& s)
{
    reset();
    if( !s.empty() )
    {
        string ss;
        do
        {
            ss = readSectionName();
            if( ss.empty() )
                return false;
        }
        while( ss != s );
    }
    ssec_beg = 0;
    return true;
}

string
TConfig::getFirst()
{
    reset( ((ssec_beg!=0) ? ssec_beg : sec_beg) );

    return getNext();
}

string
TConfig::getNext()
{
    if( !getLine() )
        return "";
    if( sec_beg != 0 && (line[0]=='[' || line[0]=='<') )		// section break
        return "";

    size_t pos = getName();
    if( pos != 0 )
        return string(line,0,pos);
    return "";
}

string
TConfig::getToken()
{
    unsigned pos = valPos;
    // searching for non-space
    for( ; IsSpace(line[pos]); pos++ )
        if( pos == line.length()-1 )
            return "";		// over the end of line

    if( line[pos] == '"' )
    {
        space = '"';
        pos++;
    }

CONT_SEARCH:
    for( valPos = pos; valPos<line.length(); valPos++ )
        if( line[valPos] == '"' ||
                (space==' ' && IsSpace(line[valPos]) ) )
            break;

    if( valPos==line.length() && space=='"')	// new line within quotes
    {
        string line2;
        u_getline(ini, line2, '\n');
        if( ini.eof() )
            goto BREAK;
        // we need '\n' after each line that placed in "..."
        line +=  '\n';
        line += line2;
        goto CONT_SEARCH;
    }
BREAK:
    if( space == '"' )
    {
        space = ' ';
        valPos++;
        return string(line, pos, valPos-pos-1);
    }

    return string(line, pos, valPos-pos);
}


// simple versions ( after getNext() )
//--- Integer (32bit)
int
TConfig::getcInt()
{
    string val(line,valPos,string::npos);

    return atoi(val.c_str());
}

//--- simple string ----
bool
TConfig::getcStr(string& str)
{
    str = getToken();

    return true;
}

bool
TConfig::getcVals(int n, int vals[])
{
    for( int ii=0; ii<n; ii++ )
    {
        string st = getToken();
        vals[ii] = atoi(st.c_str());
    }

    return true;
}

bool
TConfig::getcStrings(int n, string strs[])
{
    for( int ii=0; ii<n; ii++ )
        strs[ii] = getToken();

    return true;
}


// full versions
//--- Integer (32bit)
int
TConfig::getInt(const char* par, int def)
{
    if( !findParam(par) )
        return def;

    string val(line, valPos, string::npos);

    return atoi(val.c_str());
}

//--- simple string ----
bool
TConfig::getStr(const char* par, string& str, char* def)
{
    if( !findParam(par) )
    {
        if( def )
            str = def;
        else
            str = "";
        return false;
    }

    str = getToken();

    return true;
}

bool
TConfig::getVals(const char* par, int n, int vals[], int defs[])
{
    if( !findParam(par) )
    {
        for( int ii=0; ii<n; ii++ )
            vals[ii] = (defs)?defs[ii]:0;
        return false;
    }

    for( int ii=0; ii<n; ii++ )
    {
        string st = getToken();
        vals[ii] = atoi(st.c_str());
    }

    return true;
}

bool
TConfig::getStrings(const char* par, int n, string strs[], char* defs[])
{
    if( !findParam(par) )
    {
        for( int ii=0; ii<n; ii++ )
            if( defs )
                strs[ii] = defs[ii];
            else
                strs[ii] = "";
        return false;
    }

    for( int ii=0; ii<n; ii++ )
        strs[ii] = getToken();

    return true;
}
//--------------------- End of config.cpp ---------------------------

