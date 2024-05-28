//-------------------------------------------------------------------
// $Id: v_dbm.h 1179 2009-01-20 10:27:35Z gems $
//
// Declaration of TDBFile, TDBKey, TDBKeyList, TDataBase and
//	DataBaseList classes
//
// Copyright (C) 1996-2001  S.Dmytriyeva, D.Kulik
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
//
#ifndef V_DBM_H_
#define V_DBM_H_

#include <ctime>
#include "v_object.h"
#include "v_dbfile.h"


class GemDataStream;

const int	MAXRKFRMSTR = 20;  // max fields in key
const size_t IND_PLUS = 112;    // augment of index key buf

enum keyctrl {   // codes key bild
    K_END = -5, K_EMP, K_ANY, K_IMM, K_ACT
};

extern const char* ALLKEY;


// This is struct contened the key of record
class TDBKey
{
    char *pKey;                  // packed key of record
    char *uKey;                  // unpacked key
    unsigned char rkFlds;        // number fields in key
    unsigned char *rkLen;        // len of fields
    unsigned char *rkInd;        // index of fields
    unsigned char fullLen;       // full length of records key

    const TDBKey& operator=(const TDBKey&);

protected:
    void unpack( const char *key );
    void pack( const char *key );

public:
    TDBKey(unsigned char nRkflds,const unsigned char* rkfrm );
    TDBKey(fstream& f);
    TDBKey( const TDBKey& dbKey );
    virtual ~TDBKey();
    void check();

    //--- Selectors
    const char *PackKey()
    {
        pKey[fullLen+rkFlds] = '\0';
        return pKey;
    }
    const char *UnpackKey() const
    {
        uKey[fullLen] = '\0';
        return uKey;
    }
    unsigned char KeyLen() const
    {
        return fullLen;
    }
    unsigned char KeyNumFlds() const
    {
        return rkFlds;
    }
    char *FldKey( uint i) const
    {
        return uKey+rkInd[i];
    }
    unsigned char FldLen(uint i) const
    {
        return rkLen[i];
    }
    unsigned char* RkFrm() const
    {
        return rkLen;
    }

    //--- Manipulation keys
    void SetKey( const char *key );
    void SetFldKey( uint i, const char *fld );
    //  void MakeKey( int nRtwrk, char *pkey, ...);	// !!!!!!
}
;

//This class contened the list of indexes
class TDBKeyList:
            public TDBKey
{
    int nI;
    size_t recInDB;  // records in DB
    size_t nBuf;
    RecEntry *re;
    char *ndx[MAXRKFRMSTR];

    //key list for a wildcard search
    TCStringArray aKey;
    TCIntArray anR;

    TDBKeyList(const TDBKeyList&);
    const TDBKeyList operator=(const TDBKeyList&);

protected:
    void init();
    int keycom( uint );
    uint keycmp( uint );
    void arec_add( uint );
    void check_ndx();

public:
    TDBKeyList( unsigned char nRkflds, const unsigned char* rkfrm );
    TDBKeyList( fstream& f );
    ~TDBKeyList();
    void check_i(uint i);

    //--- Selectors
    RecEntry* RecPosit(uint i);

    int iRec() const
    {
        return nI;
    }
    size_t RecCount() const
    {
        return recInDB;
    }
    char *RecKeyFld(uint i, uint j) const
    {
        return ndx[j]+i*FldLen(j);
    }
    const TCStringArray& KeyList() const
    {
        return aKey;
    }
    const TCIntArray& RnList() const
    {
        return anR;
    }
    size_t KeyInList()
    {
        return aKey.size();
    }

    //--- Manipulation records
    uint  addndx( uint nF, int len, const char *key );
    void delndx(uint i);
    int  findx( const char *key );
    size_t  xlist( const char *pattern );
    void PutKeyList( uint nF, GemDataStream& f);
    void GetKeyList_i( uint nF,int nRec, GemDataStream& f );
    void delfile( uint nF );

    //--- Manipulation key
    void PutKey(uint i);
    void RecKey(uint i, string& kbuf );

    //--- reset class
    void initnew();
};

//-------------------------------------------------------------

enum RecStatus {   // states of keys record Data Base
    UNDF_=-7 /* undefined state */, FAIL_=-1 /* access error */,
    NONE_=0 /* no records */, ONEF_ ,
    MANY_ , EMPC_, /* empty chain */
    /* NONELIST_ * no records to part */
};

struct RecHead
{  // head the record in DB file
    char bgm[2];
    unsigned char nRT, Nobj;
// Sveta 64   long rlen;    // full len the record in file
    int rlen;    // full len the record in file
// Sveta 64    time_t crt;
    int crt;
    char endm[2];

    void read(GemDataStream& is);
    void write(GemDataStream& os);

    static int data_size()
    {
        return sizeof(char[2]) + sizeof(char)*2
// Sveta 64               + sizeof(long) + sizeof(time_t) + sizeof(char[2]);
               + sizeof(int) + sizeof(int) + sizeof(char[2]);
    }
};

enum modeList { openf = 0x01, closef = 0x02, oldself = 0x04 };

// Structure of Data Base

class TDataBase
{
    char Keywd[MAXKEYWD];
    unsigned char nRT;
    RecStatus status;
    bool rclose;
    bool isDelete;
    int frstOD;
    unsigned char nOD;
    int frstODjson;
    int lastODjson;
    TDBKeyList ind;
    std::vector< std::shared_ptr<TDBFile>> aFile;  // list of add files
    TCIntArray fls;

    //work param
    time_t crt;

protected:

    //--- check
    void check_file( uint nF );
    void putndx( uint nF );
    void getndx( uint nF );
    int reclen( );
    int putrec( RecEntry& re, GemDataStream& f );
    int putrec( RecEntry& re, GemDataStream& f, RecHead& rhh );
    int getrec( RecEntry& re, GemDataStream& f, RecHead& rh );
    void opfils();
    int scanfile( uint nF, int& fPos, int& fLen,
	    GemDataStream& inStream, GemDataStream& outStream);
    void fromCFG(fstream& f);
    bool dbChangeAllowed( uint nf, bool ifRep=false );

public:
    int specialFilesNum;
    int fNum;
    TCStringArray fOpenNameBuf;

    //  TDateBase();
    TDataBase( size_t nrt, const char* name, bool Rclose, bool isDel,
               int nOf, unsigned char Nobj, int filesNum,
               unsigned char nRkflds, const unsigned char* rkfrm );
    TDataBase( fstream& f );
    ~TDataBase();
    void toCFG( fstream& f );
    void AddFile(const std::string& path);
    void DelFile(const std::string& path);
    void updateJsonOD(int frstOD, int lastOD)
    {
        frstODjson =frstOD;
        lastODjson =lastOD;
    }


    //--- Selectors
    RecStatus GetStatus() const
    {
     return status;
    }
    void SetStatus( RecStatus stt )
    {
      status = stt;
    }

    const char* GetKeywd() const
    {
        return Keywd;
    }
    uint KeyLen() const
    {
        return ind.KeyLen();
    }
    void RecKey( uint i, string& kbuf )
    {
        ind.RecKey(i,kbuf);
    }

    const char *PackKey()
    {
        return ind.PackKey();
    }

    const char *UnpackKey()
    {
        return ind.UnpackKey();
    }

    time_t Rtime() const
    {
        return crt;
    }  // time current record
    time_t GetTime( uint i );               // time record number i
    char *FldKey( uint i ) const
    {
        return ind.FldKey(i);
    }
    unsigned char FldLen(uint i) const
    {
        return ind.FldLen(i);
    }
    unsigned char KeyNumFlds() const
    {
        return ind.KeyNumFlds();
    }
    bool KeyTest( const char* key );
    const TDBKey& GetDBKey() const
    {
        return ind;
    }
    const TCStringArray& GetOpenFiles() const
    {
        return fOpenNameBuf;
    }
    int GetWorkFileName() const
    {
        return fNum;
    }
    void SetKey( const char *key )
    {
        ind.SetKey(key);
    }
    void SetFldKey( uint i, const char *fld )
    {
        ind.SetFldKey( i, fld );
    }

    void MakeKey( unsigned char nRtwrk, char *pkey, ...);
    size_t RecCount() const
    {
        return ind.RecCount();
    }
    int GetObjFirst() const
    {
        return  frstOD;
    }
    unsigned char GetObjCount() const
    {
        return nOD;
    }


    //--- Information about of open file list
    uint GetOpenFileNum( const char *secondName  );
    bool SetNewOpenFileList( const TCStringArray& aFlNames );
    void GetFileList( int mode, TCStringArray& names,
                      TCIntArray& indx,  TCIntArray& sel );
    //  int GetNumFiles() { return aFile.size(); }
    void  MakeInNewProfile( const std::string& dir,
      const char *prfName, const char * f_name=nullptr );
    void OpenOnlyFromList( TCStringArray& names );
    void GetProfileFileKeywds( const char *_name, TCStringArray& aFlkey );
    bool ifDefaultOpen() const;

    //--- Manipulation Data Base
    void Create( uint nF );
    void Open( bool type, FileStatus mode, const TCIntArray& indx);
    void OpenAllFiles( bool only_kernel = false );
    void Close();

    //--- Manipulation files of Data Base
    void RebildFile(const TCIntArray& indx);
    int AddFileToList(TDBFile* file);
    // int GetFileNum(const char* substr_name);
    void AddOpenFile(const TCIntArray& indx);

    //--- Manipulation records
    uint AddRecordToFile(const char *key, int file);
    void Rep( int i);
    void Del( int i);
    void Get( int i);
    int Find( const char *key);
    int FindCurrent( const char *key);
    RecStatus Rtest( const char *key, int mode = 1);
    bool FindPart( const char *key_, uint field );
    //  RecStatus TryRec( RecStatus rs, char *key, int mode = 1);

    //--- Manipulation list of records
    size_t GetKeyList( const char *keypat,TCStringArray& aKey,TCIntArray& anR);
    const TCStringArray& KeyList() const
    {
        return ind.KeyList();
    }
    const TCIntArray& RnList() const
    {
        return ind.RnList();
    }
    void RenameList( const char* newName,
        const char *oldName );

    void toJsonObject( QJsonObject& obj ) const;
    std::string fromJsonObject( const QJsonObject& obj );
    void toJsonObjectNew(QJsonObject &obj, const std::string& project_name) const;
    std::string fromJsonObjectNew( const QJsonObject& obj );
};

// Data Base container : rt

class DataBaseList:
            public std::vector<std::shared_ptr<TDataBase>>
{
public:
    DataBaseList():
           std::vector<std::shared_ptr<TDataBase>>()
    { }

    ~DataBaseList();

    void Init();
    void fromCFG( fstream& f );
    void toCFG(fstream& f);

    //--- Selectors
    int Find(const char* keywd);
};

extern DataBaseList rt;

#endif	// V_DBM_H_
