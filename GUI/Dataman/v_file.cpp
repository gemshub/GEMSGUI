//-------------------------------------------------------------------
// $Id: v_file.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of TFile class
//
// Copyright (C) 1996-2001 S.Dmytriyeva
// Uses  string class (C) A.Rysin 1999
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
#include <unistd.h>
#include "v_file.h"
#include "v_user.h"

//----------------------------------------------------------
// TFile
//----------------------------------------------------------

void TFile::write( fstream& out_stream )
{
    out_stream << Keywd << "  "; // endl;
    int ln = Path.length() + 1;
    out_stream << ln << ' '; // endl;
    out_stream << Path << endl;
    ErrorIf( out_stream.bad(), GetKeywd(),
             "Error writing TFile to configurator.");
}

void TFile::read( fstream& in_stream )
{
    in_stream >> Keywd;
    int ln;
    in_stream >> ln;
    in_stream >> Path;
    u_splitpath(Path, dir, name, ext);

    ErrorIf( in_stream.bad(), GetKeywd(),
             "Error reading TFile from configurator.");
}

// configuration to file
void TFile::toCFG(fstream& fcfg)
{
    write(fcfg);
}

TFile::TFile(fstream& fcfg):
    isopened( false )
{
    read(fcfg);
    //Makepath();
}

TFile::TFile(const string& fName, const string& fExt, const string& fDir):
    isopened( false )
{
    dir = fDir;
    ext = fExt;
    name = fName;

    Makepath();
    makeKeyword();
}

TFile::TFile(const string& path):
        isopened( false ),
        Path(path)
{
    u_splitpath(Path, dir, name, ext);
    makeKeyword();
}


// For all files first simbols for name
void TFile::makeKeyword()
{
    if( name.empty() )
        return;
    strncpy( Keywd, name.c_str(), MAX_FILENAME_LEN);
    Keywd[MAX_FILENAME_LEN]='\0';
}


/// Get full name of file
void TFile::Makepath()
{
    Path = u_makepath( dir, name, ext );
}

/// Check for existence of file
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

/// Open file in special mode
void TFile::OpenFromFileName( const string& filename )
{
    Close();
    remove( GetPath().c_str() );
    rename(  filename.c_str(), GetPath().c_str());
    Open();
}


//--------------------- End of v_file.cpp ---------------------------

