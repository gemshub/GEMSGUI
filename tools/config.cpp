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
#include <sstream>
#include <fstream>

#include "config.h"
#include "GEMS3K/v_detail.h"
#include "GEMS3K/v_service.h"

TConfig::TConfig( const std::string&fname, char st ):
    space(' '),
    style( st ),
    sec_beg( 0 ),
    ssec_beg( 0 ),
    cur_pos(0),
    ini_data()
{
    // read all file
    std::ifstream ini(fname);
    ErrorIf( !ini.good(), "Read ini file", "File open error...  "+fname );
    std::stringstream buffer;
    buffer << ini.rdbuf();
    ini_data = buffer.str();
}

std::string TConfig::get_next_line()
{
    std::string ln;

    auto next_n = ini_data.find( new_line, cur_pos );

    if(next_n == std::string::npos )
    {
        ln = ini_data.substr(cur_pos);
        cur_pos = ini_data.length();
    }
    else
    {
        ln = ini_data.substr(cur_pos, next_n-cur_pos);
        cur_pos = next_n+new_line.length();
    }
    return ln;
}


bool TConfig::getLine()
{
    line = "";

    if( cur_pos >= ini_data.length() )
        return false;

    while( cur_pos < ini_data.length() )
    {
        line = get_next_line();
        strip( line );
        // empty lines and comments are cut out
        if( line.empty() )
            continue;
        if( line[0] != ';' && line[0] != '#' )
            break;
    }
    return true;
}

// sets Section name for getFirst(), getNext(), get...()
bool TConfig::SetSection( const std::string& sec_name )
{
    if( !sec_name.empty() )
    {
        std::string ss = "["+sec_name+"]";
        auto sec_pos = ini_data.find(ss);
        if(  sec_pos!= std::string::npos )
        {
            sec_beg = sec_pos+ss.length();
            ssec_beg = 0;
            cur_pos = sec_beg;
            return true;
        }
        return false;
    }
    sec_beg = 0;
    ssec_beg = 0;
    return true;
}

std::string TConfig::readSectionName()
{
    while( 1 )
    {
        if( line[0]=='[' )
            break;
        // get line after 'cause it's already read by readSection()
        if( !getLine() )
            return "";
    }
    size_t ii;
    for( ii = 0; ii<line.length(); ii++ )
        if( line[ii] == ']' )
            break;
    if( ii == line.length() )
        throw EBadSection();

    sec_beg = cur_pos;
    ssec_beg = 0;
    std::string sec(line, 1, ii-1);
    strip(sec);
    return sec;
}

std::string TConfig::readSubSectionName()
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
    size_t ii;
    for( ii = 0; ii<line.length(); ii++ )
        if( line[ii] == '>' )
            break;
    if( ii == line.length() )
        throw EBadSection();

    ssec_beg = cur_pos;
    std::string sec(line, 1, ii-1);
    strip(sec);
    return sec;
}

std::string TConfig::GetFirstSubSection()
{
    reset( sec_beg );
    if( !getLine() )
        return "";
    return GetNextSubSection();
}

std::string TConfig::GetNextSubSection()
{
    return readSubSectionName();
}


std::string TConfig::getFirst()
{
    reset( ((ssec_beg!=0) ? ssec_beg : sec_beg) );
    return getNext();
}

std::string TConfig::getNext()
{
    if( !getLine() )
        return "";
    if( sec_beg != 0 && (line[0]=='[' || line[0]=='<') )		// section break
        return "";

    size_t pos = getName();
    if( pos != 0 )
        return std::string(line,0,pos);
    return "";
}

size_t TConfig::getName()
{
    size_t ii, jj;
    for( ii=0; ii<line.length(); ii++ )
    {
        if( !isalnum(line[ii]) && line[ii]!='_' && line[ii]!='~')
            break;

        if( ii >= token_len )
            return val_pos=0;
    }

    for( jj=ii; jj<line.length(); jj++ )
        if( line[jj] != style && !IsSpace(line[jj]) )
            break;
    val_pos = jj;

    return ii;
}

bool TConfig::findParam(const char* par)
{
    // speedup if getNext was previous &
    // intruding loopback if repeated!!!
    //  int pos = getName();
    //  if( pos != 0  &&  string(line,pos) == par )
    //    return true;
    std::string s;
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

std::string TConfig::getToken()
{
    auto pos = val_pos;
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

    for( val_pos = pos; val_pos<line.length(); val_pos++ )
        if( line[val_pos] == '"' ||
                (space==' ' && IsSpace(line[val_pos]) ) )
            break;

    if( val_pos==line.length() && space=='"')	// new line within quotes
    {
        std::string line2 = get_next_line();
        if( cur_pos ==  ini_data.length() )
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
        val_pos++;
        return std::string(line, pos, val_pos-pos-1);
    }

    return std::string(line, pos, val_pos-pos);
}


// simple versions ( after getNext() )
//--- Integer (32bit)
int
TConfig::getcInt()
{
    std::string val(line,val_pos,std::string::npos);

    return atoi(val.c_str());
}

//--- simple string ----
bool
TConfig::getcStr(std::string& str)
{
    str = getToken();

    return true;
}

bool
TConfig::getcVals(int n, int vals[])
{
    for( int ii=0; ii<n; ii++ )
    {
        std::string st = getToken();
        vals[ii] = atoi(st.c_str());
    }

    return true;
}

bool
TConfig::getcStrings(int n, std::string strs[])
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

    std::string val(line, val_pos, std::string::npos);

    return atoi(val.c_str());
}

//--- simple string ----
bool TConfig::getStr(const char* par, std::string& str, char* def)
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

bool TConfig::getVals(const char* par, int n, int vals[], int defs[])
{
    if( !findParam(par) )
    {
        for( int ii=0; ii<n; ii++ )
            vals[ii] = (defs)?defs[ii]:0;
        return false;
    }

    for( int ii=0; ii<n; ii++ )
    {
        std::string st = getToken();
        vals[ii] = atoi(st.c_str());
    }

    return true;
}

bool TConfig::getStrings(const char* par, int n, std::string strs[], char* defs[])
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



