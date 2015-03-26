//-------------------------------------------------------------------
// $Id: v_ejdb_file.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of TEJDBFile class and some filename related constants
//
// Copyright (C) 2013 S.Dmytriyeva
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

#ifndef _v_ejdb_file_h_
#define _v_ejdb_file_h_

#include <vector>

#ifndef __APPLE__
#include <memory>   // deprecated
#endif

#include "v_file.h"
#include "ejdb.h"
#include "filters_data.h"

/// Class for EJDB file manipulation
class TEJDBFile: public TFile
{

    bool defaultDB; ///< File from DB.default
    string version; ///< Version of EJDB

    int numEJDB;    ///< Number of usage EJDB database

protected:

    virtual void makeKeyword();

public:
    EJDB *ejDB ;

    //  TFile();
//    TEJDBFile(const string& fName,
//          const string& fExt, const string& fDir );
    TEJDBFile(const string& path);
    TEJDBFile(fstream& fcfg);
    virtual ~TEJDBFile();

    bool isDefaultDB()
     { return defaultDB; }
    const string& Version() const
    {
        return version;
    }
    void readVersion();

    //--- Manipulation
    void Create( bool isDel );
    virtual void Open();
    virtual void Close();

};

class EJDBFileList:
            public vector<TEJDBFile>
{

    /// Make name of EJ DataBase and configurate path
    string  makeEJDBname(string& path, bool &isDefault );
    /// Scan directory and make EJDB file list
    void readEJDBDir( const char *dir );

    void addTree();

public:

    auto_ptr<FileTree> ftree;  ///< Default DB Files tree structure

    EJDBFileList()
    {
        string tag="defaultDB", vers="";
        ftree = auto_ptr<FileTree> (new FileTree(tag, vers));
    }

    ~EJDBFileList()
    {}

    void Init();
    void fromCFG( fstream& f );
    void toCFG(fstream& f);

    //--- Selectors
    TEJDBFile& operator[](int) ;
    int Find(const char* keywd);
    int FindPath( const string& path );

    /// Added existing EJ DataBase
    bool addExist( string path );

    /// Added new or link existing EJ DataBase
    void addNew( string& path );

};


extern EJDBFileList flsEJ;


#endif // _v_ejdb_file_h
