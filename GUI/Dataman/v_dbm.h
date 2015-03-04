//-------------------------------------------------------------------
// $Id: v_dbm.h 1179 2009-01-20 10:27:35Z gems $
//
// Declaration of TDataBase and DataBaseList classes
//
// Copyright (C) 1996-2013  S.Dmytriyeva, D.Kulik
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
//
#ifndef __v_dbm_old_h_
#define __v_dbm_old_h_

#include <iostream>
#include <map>

#include "v_ejdb.h"
#include "v_dbm_file.h"
#include "filters_data.h"

extern const char* MARKRECDEL;
extern const char* MARKRECHEAD;
extern const char MARKRKEY;

/// This class contened the structure of old Data Base
class TDataBase
{
    // Definition of chain
    string Keywd;  /// Name of modules DB
    int nRT;      /// Module number
    int frstOD;    /// First objects number to DB record
    int lastOD;    /// Last objects numbedr to DB record

    /// Definition of record key
    TEJDBKey key;
vector<string> aFldShot;          /// Short names of key fields

    // Definition of record key list
    /// Linked records list
    map<IndexEntry, RecEntry, less<IndexEntry> > recList;
    /// Current index in recList
    map<IndexEntry, RecEntry, less<IndexEntry> >::iterator itrL;
    // Work data
    RecStatus status;       /// ? Current states of keys record DB
    time_t crt;             /// ? Time of modifying current record ( not realized )
    //vector<string> wrkFld;  /// Work record key

    // Definition of EJDB files
    // must be extern class for EJDB names
    vector<TDBFile*> aFile;  /// ?extern List of all available DB files
    int specialFilesNum;     /// ?extern Number of default DB files

    vector<int> fls;         /// ? List of opened files (indexes from aFile)
    vector<string> fOpenNameBuf;   /// ? List of names of opened files
    int fNum;                /// ? Current index in list of opened files

// old DB data
    bool rclose;
    bool isDelete;

protected:

    void fromCFG(fstream& f);
    bool dbChangeAllowed( int nf );
    void delfile( int nF );
    int findFnum( int nF );
    int getCurrentFile( int &fnum );

 // from old DB
    void putndx( int nF );
    void getndx( int nF );
    int reclen( );
    int putrec( RecEntry& re, GemDataStream& f );
    int putrec( RecEntry& re, GemDataStream& f, RecHead& rhh );
    int getrec( RecEntry& re, GemDataStream& f, RecHead& rh );
    void opfils();
    int scanfile( int nF, int& fPos, int& fLen,
        GemDataStream& inStream, GemDataStream& outStream);
    void check_file( int nF );
    void RepRecord(const char* pkey );
    void PutKeyList( int nF, GemDataStream& f);
    void GetKeyList_i(int nF, int nRec, GemDataStream& f);

public:

    //  Constructor
    TDataBase( int nrt, const char* name, bool Rclose, bool isDel,
                 int firstObj, int lastObj, int spflNum,
                 int nRkflds, const size_t* rkfrm  );
    TDataBase( fstream& f );
    ~TDataBase();

    /// Save definition of chain to configuration file
    void toCFG( fstream& f );

    //--- Selectors
    /// Get name of modules DB
    const char* GetKeywd() const
       {  return Keywd.c_str();   }
    /// Get records count in opened files
    int RecCount() const
       {  return recList.size(); }

    /// Get current record status
    RecStatus GetStatus() const
       {  return status;  }
    /// Set current record status
    void SetStatus( RecStatus stt )
       {   status = stt;   }
    /// Set Number of default DB files
    void SetSpecialFilesNum( int stt )
       {   specialFilesNum = stt;   }
    /// Time of current record
    time_t Rtime() const
        {  return crt;  }
    time_t GetTime( const char *pkey );

    /// Get current index in list of opened files
    int GetFnum() const
       {  return fNum;  }
    /// Access to list of names of opened files
    const vector<string>& GetOpenFiles() const
       { return fOpenNameBuf;  }

    //--- Selectors for key
    /// Return record key length
    int KeyLen() const
        { return key.KeyLen(); }
    /// Return current record key in packed form
    const char *PackKey()
        { return key.PackKey(); }
    /// Return current record key in unpacked form
    const char *UnpackKey()
        { return key.UnpackKey();  }
    /// Return record key field i
    const char *FldKey( int i ) const
        { return key.FldKey(i); }
    /// Return record key field i len
    size_t FldLen(int i) const
        { return key.FldLen(i);  }
    /// Return number or record key fields
    int KeyNumFlds() const
        { return key.KeyNumFlds();  }
    /// Access to TEJDBKey
    const TEJDBKey& GetDBKey() const
        { return key; }
    /// Putting DB record key pkey to internal structure
    void SetKey( const char *ckey )
        {  key.SetKey(ckey);
           //status = UNDF_;
        }
    /// Change i-th field of TEJDBKey to key
    void SetFldKey( int i, const char *fld )
        {  key.SetFldKey( i, fld ); }
    // Make packed key to seach.
    void MakeKey( unsigned char nRtwrk, string& pkey, ...);

    //--- Manipulation records
    /// Save current record to bson structure
    //void RecToBson( bson *obj );
    /// Load data from bson structure
    //void RecFromBson( bson *obj );

    /// Find record with key into internal record keys list
    bool Find( const char *key );
    /// Test state of record with key key_ as template.
    /// in field field setted any(*) data
    bool FindPart( const char *key_, int field );
    /// Retrive one record from the collection
    void Get( const char *key );
    /// Removes record from the collection
    void Del( const char *key );
    /// Save/update record in the collection
    // fnum - index into internal file list
    void SaveRecord( const char* key, int& fnum  );
    /// Save/update record in the collection
    /// Question for replase
    void SaveRecordQuestion(const char* pkey, int& fnum, bool& yesToAll );
    /// Save new record in the collection
    /// Test unique keys name before add the record(s) ???? must be
    // fnum - index into internal file list
    void AddRecord(const char* key, int& fnum );
    /// Test state of record with key pkey.
    /// If mode == 1 and one record, read this record.
    RecStatus Rtest( const char *key, int mode );

    //--- Manipulation list of records
    /// Get key list for a wildcard search
    int GetKeyList( const char *keypat,
        vector<string>& aKeyList, bool retUnpackform = true );
    /// Rename records ( change first field: for SYSEQ, and > )
    void RenameList( const char* newName, const char *oldName );

    //--- From module part

    // Work with files
    //--- Information about of open file list
    int GetOpenFileNum( const char *secondName  );
    bool SetNewOpenFileList( const vector<string>& aFlNames );
    void GetFileList( int mode, vector<string>& names,
                      vector<int>& indx,  vector<int>& sel );
    void  MakeInNewProfile( const string& dir,
      const string& prfName, const char* f_name=0 );
    void OpenOnlyFromList( const vector<string>& names );
    void GetProfileFileKeywds( const char *_name, vector<string>& aFlkey );
    bool ifDefaultOpen() const;

    //--- Manipulation Data Base
    void Create( int nF );
    void Open( vector<int>& indx);
    void Open( );
    void OpenAllFiles( bool only_kernel = false );
    void Close();
    void AddFile(const string& path);
    void DelFile(const string& path);

    //--- Manipulation files of Data Base
    void RebildFile(const vector<int>& indx);
    int AddFileToList(TDBFile* file);
    void AddOpenFile(const vector<int>& indx);

};

class DataBaseList:
            public vector<TDataBase>
{
public:
    DataBaseList()
    { }

    ~DataBaseList()
    {}

    void Init();
    void fromCFG( fstream& f );
    void toCFG(fstream& f);

    //--- Selectors
    TDataBase& operator[](int);
    int Find(const char* keywd);

    // files selection
    int isOpenFile( const vector<string>& selNames, string& name );
    void buildFilesList( elmFilesConfData& files_data,
                         const vector<string>& selNames );
    void resetFilesSelection(elmFilesConfData& files_data,
                         const vector<string>& selNames);
    void setTreeWidget(elmFilesConfData& files_data);



};

extern DataBaseList rtOld;

//#define oldDBusing     // Old-database mode (comment out to switch to new database mode)
#ifdef oldDBusing        // Pointer to class contened the structure of main DB
extern DataBaseList& rt;
#else
extern EJDataBaseList& rt;
#endif

#endif	// __v_dbm_old_h_
