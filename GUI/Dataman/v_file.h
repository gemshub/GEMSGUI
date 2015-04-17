//-------------------------------------------------------------------
// $Id: v_file.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of TFile class and some filename related constants
//
// Copyright (C) 1996-2013 S.Dmytriyeva, D.Kulik
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

#ifndef _v_file_h_
#define _v_file_h_

#include <fstream>
#include "verror.h"

typedef ios::openmode FileStatus;

const int 	MAX_FILENAME_LEN = 20;

static const FileStatus  NOT_OPEN = FileStatus(0),    // no file  opend
    RDONLY_T = ios::in/*|ios::nocreate*/,               //0x01, !
    RDONLY_B = ios::in/*|ios::nocreate*/|ios::binary,   //0x81,
    WRITE_T =  ios::out,                            //0x02,
    WRITE_B =  ios::out|ios::binary,                //0x82,
    UPDATE_T = ios::in|ios::out,                    //0x03,
    UPDATE_B = ios::in|ios::out|ios::binary,        //0x83,
    APPEND_T = /*ios::in|*/ios::out|ios::app,           //0x09, !
    APPEND_B = /*ios::in|*/ios::out|ios::app|ios::binary, //0x84,
    RDONLY_DBV = ios::in/*|ios::nocreate*/|ios::binary, //0x05, !
    UPDATE_DBV = ios::in|ios::out|ios::binary;       // 0x83 !


/// Base class for file manipulation
class TFile
{

protected:

    char Keywd[MAX_FILENAME_LEN+1];
    bool  isopened;    // true, if file is opened
    string dir;
    string name;
    string ext;
    string Path;

    virtual void makeKeyword();
    virtual void write( fstream& );
    virtual void read( fstream& );

public:

    //  TFile();
    TFile(const string& fName,
          const string& fExt, const string& fDir );
    TFile(const string& path);
    TFile(fstream& fcfg);
    virtual ~TFile() {}

    void toCFG(fstream& fcfg);

    //--- Selectors
    const char* GetKeywd() const
    {
        return Keywd;
    }
    bool Test() const
    {
        return isopened;
    }

    const string& Name() const
    {
        return name;
    }
    const string& Ext() const
    {
        return ext;
    }
    const string& Dir() const
    {
        return dir;
    }
    const string& GetPath() const
    {
        return Path;
    }

    //--- Manipulation
    void Makepath();
    bool Exist();
    virtual void Open() = 0;
    virtual void Close() = 0;
    void OpenFromFileName( const string& filename );

};

#endif // _v_file_h
