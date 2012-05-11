//-------------------------------------------------------------------
// $Id: config.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of TConfig
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
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#include <stdlib.h>
#include <ctype.h>

#include "config.h"
#include "v_user.h"

TConfig::TConfig(const char *fname, char st, int tok_ln):
        token_len(tok_ln),
        space(' '),
        ini(fname),
        style(st)
{
    if( !ini.good() )
    {
//     cout << "TTTTTTTTTTTT1";
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
        //    line.strip(/*gstring::Both*/);
        StripLine( line );
        // empty lines and comments are cut out
        if( line.empty() )
            continue;
        if( line[0] != ';' && line[0] != '#' )
            break;
    }
    return true;
}

gstring
TConfig::readSectionName()
{
    int cpos;
    while( 1 )
    {
        if( !getLine() )
            return "";
        if( line[0]=='[' )
            break;
    }
    cpos = ini.tellg();
    size_t ii;
    for( ii = 0; ii<line.length(); ii++ )
        if( line[ii] == ']' )
            break;
    if( ii == line.length() )
        throw EBadSection();

    sec_beg = cpos;
    ssec_beg = 0;
    gstring sec(line, 1, ii-1);
    StripLine(sec);
    return sec;
}

gstring
TConfig::readSubSectionName()
{
    int cpos;
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
    cpos = ini.tellg();
    size_t ii;
    for( ii = 0; ii<line.length(); ii++ )
        if( line[ii] == '>' )
            break;
    if( ii == line.length() )
        throw EBadSection();

    ssec_beg = cpos;
    gstring sec(line, 1, ii-1);
    StripLine(sec);
    return sec;
}

gstring
TConfig::GetFirstSubSection()
{
    reset(sec_beg);
    if( !getLine() )
        return "";
    return GetNextSubSection();
}

gstring
TConfig::GetNextSubSection()
{
    return readSubSectionName();
}

int
TConfig::getName()
{
    size_t ii;
    for( ii=0; ii<line.length(); ii++ )
    {
        if( !isalnum(line[ii]) && line[ii]!='_' && line[ii]!='~')
            break;

        if( ii >= token_len )
            return valPos=0;
    }

    unsigned jj;
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
    //  if( pos != 0  &&  gstring(line,pos) == par )
    //    return true;
    gstring s;
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
TConfig::SetSection(const gstring& s)
{
    reset();
    if( !s.empty() )
    {
        gstring ss;
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

gstring
TConfig::getFirst()
{
    reset( ((ssec_beg!=0) ? ssec_beg : sec_beg) );

    return getNext();
}

gstring
TConfig::getNext()
{
    if( !getLine() )
        return "";
    if( sec_beg != 0 && (line[0]=='[' || line[0]=='<') )		// section break
        return "";

    int pos = getName();
    if( pos != 0 )
        return gstring(line,0,pos);
    return "";
}

gstring
TConfig::getToken()
{
    unsigned pos = valPos;
    // searching for non-space
    for( ; IsSpace(line[pos]); pos++ )
        if( pos == line.length()-1 )
            return "";		// over the end of line

    if( line[pos] == '"' )
        space = '"',
                pos++;

CONT_SEARCH:
    for( valPos = pos; valPos<line.length(); valPos++ )
        if( line[valPos] == '"' ||
                (space==' ' && IsSpace(line[valPos]) ) )
            break;

    if( valPos==line.length() && space=='"')	// new line within quotes
    {
        gstring line2;
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
        return gstring(line, pos, valPos-pos-1);
    }

    return gstring(line, pos, valPos-pos);
}


// simple versions ( after getNext() )
//--- Integer (32bit)
int
TConfig::getcInt()
{
    gstring val(line,valPos,gstring::npos);

    return atoi(val.c_str());
}

//--- simple gstring ----
bool
TConfig::getcStr(gstring& str)
{
    str = getToken();

    return true;
}

bool
TConfig::getcVals(int n, int vals[])
{
    for( int ii=0; ii<n; ii++ )
    {
        gstring st = getToken();
        vals[ii] = atoi(st.c_str());
    }

    return true;
}

bool
TConfig::getcStrings(int n, gstring strs[])
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

    gstring val(line, valPos, gstring::npos);

    return atoi(val.c_str());
}

//--- simple gstring ----
bool
TConfig::getStr(const char* par, gstring& str, char* def)
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
        gstring st = getToken();
        vals[ii] = atoi(st.c_str());
    }

    return true;
}

bool
TConfig::getStrings(const char* par, int n, gstring strs[], char* defs[])
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

