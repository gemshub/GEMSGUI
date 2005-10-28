//-------------------------------------------------------------------
// $Id$
//
// Declaration of TFile class and some filename related constants
//
// Copyright (C) 1996-2000 S.Dmytriyeva, D.Kulik
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

#ifndef _v_file_h_
#define _v_file_h_

#include <fstream>

#include "gstring.h"
#include "v_user.h"
#include "v_dbm.h"

class GemDataStream;

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


class TFile
{
    static  char vv[9];
    static  char pa[9];

    char Keywd[MAX_FILENAME_LEN+1];
    FileStatus  status;
    gstring dir;
    gstring name;
    gstring ext;
    gstring Path;

    void makeKeyword();

protected:
    virtual void write( fstream& );
    virtual void *read( fstream& );
//    void check();

public:
    GemDataStream& f;

    //  TFile();
    TFile(const gstring& fName,
          const gstring& fExt, const gstring& fDir );
    TFile(const gstring& path);
    TFile(fstream& fcfg);
    virtual ~TFile();
    void toCFG(fstream& fcfg);

    //--- Selectors
    const char* GetKeywd() const
    {
        return Keywd;
    }
    FileStatus Test() const
    {
        return status;
    }
    const gstring& Name() const
    {
        return name;
    }
    const gstring& Ext() const
    {
        return ext;
    }
    const gstring& Dir() const
    {
        return dir;
    }
    const gstring& GetPath() const
    {
        return Path;
    }

    //--- Manipulation
    void newFile( const gstring& newName, const gstring& newExt,
                  const gstring& newDir );
    void Makepath();
    bool Exist();
//    bool CheckOverwrite(FileStatus mode);
    virtual void Open( FileStatus mode );
    virtual void Close();
    void OpenFromFileName( const gstring& filename, FileStatus mode );
    //  void Reopen(const char* file, FileStatus mode);

    static const char* VV()
    {
        return vv;
    }
    static const char* PA()
    {
        return pa;
    }
};

// file names constants

extern const char * PDB_EXT;
extern const char * NDX_EXT;

#endif // _v_file_h
