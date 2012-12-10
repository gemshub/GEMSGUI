//-------------------------------------------------------------------
// $Id: v_file.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of TFile class
//
// Copyright (C) 1996-2001 S.Dmytriyeva
// Uses  gstring class (C) A.Rysin 1999
//
// This file is part of the GEM-Selektor GUI library which uses the
// Qt v.4 cross-platform App & UI framework (http://qt.nokia.com)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the terms of GEMS3 Development
// Quality Assurance Licence (GEMS3.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#include <cstdlib>
#include <cstdio>
#include <iostream>
using namespace std;

#ifdef __unix
#include <unistd.h>
#else
#include <io.h>
#endif

#include "v_file.h"
#include "v_user.h"
#include "gdatastream.h"


//----------------------------------------------------------
// TFile
//----------------------------------------------------------

char TFile::vv[9]="GEMBASE1";
char TFile::pa[9]="01041999";

void
TFile::write( fstream& out_stream )
{
//    os.write( Keywd, MAX_FILENAME_LEN );
    out_stream << Keywd << "  "; // endl;
    int ln = Path.length() + 1;
//    os.write((const char*)&ln, sizeof ln);
//    os.write(Path.c_str(), ln);
    out_stream << ln << ' '; // endl;
    out_stream << Path.c_str() << endl;

    ErrorIf( out_stream.bad(), GetKeywd(),
             "Error writing TFile to configurator.");
}

void *
TFile::read( fstream& in_stream )
{
//    is.read( Keywd, MAX_FILENAME_LEN );
// we should put length here !!
    in_stream >> Keywd;
    int ln;
//    is.read((char*)&ln, sizeof ln);
    in_stream >> ln;
    vstr sss(ln);
//    is.read(sss.p, ln);
    in_stream >> sss.p;  // without .p does not compile in BCC32 5.1
      sss.p[ln] = '\0';

    Path = sss;
    u_splitpath(Path, dir, name, ext);

    ErrorIf( in_stream.bad(), GetKeywd(),
             "Error reading TFile from configurator.");
    return this;
}

// configuration to file
void TFile::toCFG(fstream& fcfg)
{
    write(fcfg);
}

TFile::TFile(fstream& fcfg):
        status( NOT_OPEN ),
	f(*new GemDataStream())
{
    read(fcfg);
    Makepath();
}

TFile::TFile(const gstring& fName,
             const gstring& fExt, const gstring& fDir):
        status( NOT_OPEN ),
	f(*new GemDataStream())
{
    newFile( fName, fExt, fDir );
    makeKeyword();
}

TFile::TFile(const gstring& path):
        status( NOT_OPEN ),
        Path(path),
	f(*new GemDataStream())
{
    u_splitpath(Path, dir, name, ext);
    makeKeyword();
}

TFile::~TFile()
{
    if( status != NOT_OPEN )
        Close();
    delete &f;
}

void
TFile::makeKeyword()
{
    gstring key;
    if( name.empty() )
        return;

    // ignoring version and after
    // get version
    gstring fname = name;
    size_t pos1 = fname.find(".ver");
    if( pos1 != gstring::npos )
       fname = fname.substr(0, pos1 );

    key = gstring(fname, 0, 2);
    size_t npos = 0;
    size_t npos2 = fname.find(".", npos);
    while( npos2 != gstring::npos )
    {   npos = npos2+1;
        key += gstring(fname, npos, 2);
        npos2 = fname.find(".", npos);
    }
    key += gstring(fname, npos+2);

    strncpy( Keywd, key.c_str(), MAX_FILENAME_LEN);
    Keywd[MAX_FILENAME_LEN]='\0';
}

/*
void TFile::check()
{
    ErrorIf( !status || !f.good(), GetKeywd(),
             "Can't open file...");
}
*/
// Set new full file name. Old file closed.
void TFile::newFile( const gstring& newName, const gstring& newExt,
                     const gstring& newDir )
{
    if( status != NOT_OPEN )
        Close();

    dir = newDir;
    ext = newExt;
    name = newName;

    Makepath();
}

//Get full name of file
void TFile::Makepath()
{
    Path = u_makepath( dir, name, ext );
}

//Check for existence of file
bool TFile::Exist()
{
    if( Test() )
        return true;  // file alredy open
    if( Path.empty() )
        Makepath();

    if( access( Path.c_str(), 0 ) )
        return false;
    else  return true;
}

// Open file in special mode
void TFile::Open( FileStatus mode )
{
    if( mode == status )
        return;
    if( status != NOT_OPEN )
        Close();
    if( mode == NOT_OPEN )
        return;
    if( !Exist() && !(mode == WRITE_T || mode == WRITE_B ||
                      mode == APPEND_T || mode == APPEND_B) )
        Error( GetPath(),"File not found.");
#ifdef __unix
    if( mode== UPDATE_DBV && access( Path.c_str(), W_OK ) != 0)
        mode = RDONLY_DBV;
#else
if( mode== UPDATE_DBV && access( Path.c_str(), 02 ) != 0)
    mode = RDONLY_DBV;
#endif

    /*   if( mode == WRITE_T  || mode == WRITE_B )
          if( !vfQuestion( "Question", "Write over previous file (Y) or skip (N)?" ) )
                 Error( GetKeywd(),"Cansel of open file.");
    */
// it seems like latest libstdc++ specs does not enforce
// state and pointer reset :(
// we have to do it on our own
    f.clear();
    f.open( Path.c_str(), mode );

    ErrorIf( !f.good(), GetPath(), "File open error.");

    if( (mode & ios::out) || (mode & ios::app) )
	f.seekp(0, ios::beg);
    if( mode & ios::in )
	f.seekg(0, ios::beg);

    status = mode;
}

void TFile::OpenFromFileName( const gstring& filename, FileStatus mode )
{
    Close();
    remove( GetPath().c_str() );
    rename(  filename.c_str(), GetPath().c_str());
    Open(mode);
}

/*
// check file in special mode
bool TFile::CheckOverwrite( FileStatus mode )
{
    if( Exist() && (mode == WRITE_T || mode == WRITE_B) )
        return true;
    return false;
}
*/

//Close file
void TFile::Close()
{
    f.close();
    status = NOT_OPEN;
}

// file names constants

const char * PDB_EXT = "pdb";
const char * NDX_EXT = "ndx";

//--------------------- End of v_file.cpp ---------------------------

