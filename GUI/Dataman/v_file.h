//-------------------------------------------------------------------
// $Id: v_file.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of TFile class and some filename related constants
//
// Copyright (C) 1996-2000 S.Dmytriyeva, D.Kulik
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

#ifndef _v_file_h_
#define _v_file_h_

#include <fstream>

class GemDataStream;

typedef std::ios::openmode FileStatus;

const int 	MAX_FILENAME_LEN = 20;

static const FileStatus  NOT_OPEN = FileStatus(0),    // no file  opend
    RDONLY_T = std::ios::in/*|std::ios::nocreate*/,               //0x01, !
    RDONLY_B = std::ios::in/*|std::ios::nocreate*/|std::ios::binary,   //0x81,
    WRITE_T =  std::ios::out,                            //0x02,
    WRITE_B =  std::ios::out|std::ios::binary,                //0x82,
    UPDATE_T = std::ios::in|std::ios::out,                    //0x03,
    UPDATE_B = std::ios::in|std::ios::out|std::ios::binary,        //0x83,
    APPEND_T = /*std::ios::in|*/std::ios::out|std::ios::app,           //0x09, !
    APPEND_B = /*std::ios::in|*/std::ios::out|std::ios::app|std::ios::binary, //0x84,
    RDONLY_DBV = std::ios::in/*|std::ios::nocreate*/|std::ios::binary, //0x05, !
    UPDATE_DBV = std::ios::in|std::ios::out|std::ios::binary;       // 0x83 !


class TFile
{
    static  char vv[9];
    static  char pa[9];

    char Keywd[MAX_FILENAME_LEN+1];
    FileStatus  status;
    std::string dir;
    std::string name;
    std::string ext;
    std::string Path;

    void makeKeyword();

protected:
    virtual void write( std::fstream& );
    virtual void *read( std::fstream& );

public:
    GemDataStream& f;

    //  TFile();
    TFile(const std::string& fName,
          const std::string& fExt, const std::string& fDir );
    TFile(const std::string& path);
    TFile(std::fstream& fcfg);
    virtual ~TFile();
    void toCFG(std::fstream& fcfg);

    //--- Selectors
    const char* GetKeywd() const
    {
        return Keywd;
    }
    FileStatus Test() const
    {
        return status;
    }
    const std::string& Name() const
    {
        return name;
    }
    const std::string& Ext() const
    {
        return ext;
    }
    const std::string& Dir() const
    {
        return dir;
    }
    const std::string& GetPath() const
    {
        return Path;
    }

    //--- Manipulation
    void newFile( const std::string& newName, const std::string& newExt,
                  const std::string& newDir );
    void Makepath();
    bool Exist();
//    bool CheckOverwrite(FileStatus mode);
    virtual void Open( FileStatus mode );
    virtual void Close();
    void OpenFromFileName( const std::string& filename, FileStatus mode );
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
