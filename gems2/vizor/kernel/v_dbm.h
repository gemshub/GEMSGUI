//-------------------------------------------------------------------
// $Id$
//
// Declaration of TDBFile, TDBKey, TDBKeyList, TDataBase and
//	DataBaseList classes
//
// Copyright (C) 1996-2001  S.Dmytriyeva, D.Kulik
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
//
#ifndef __v_dbm_h_
#define __v_dbm_h_

#include <time.h>
#include "gstring.h"

#include "array.h"
#include "v_file.h"


const int MAXFESTACK = 200; // size array deleted record

extern const char* MARKRECDEL;
extern const char* MARKRECHEAD;
extern const char MARKRKEY;
extern const char* ALLKEY;

struct VDBhead
{       // header of PDB file
    char VerP[8];
    char PassWd[8];
    char Date[11];
    char Time[5];
    int nRT;                 // type of PDB chain
    int nRec;                // number records in file
    int  stacOver,FPosTRT;
    char isDel;
    long MinDrLen, MaxDrLen; // min and max size of deleted block
    int curDr;               // number of deleted blocks

    void read(istream& is);
    void write(ostream& is);

    static size_t data_size()
    {
        return sizeof(char[8])*2
               + sizeof(char[11]) + sizeof(char[5])
               + sizeof(int)*4
               + sizeof(char) + sizeof(long)*2
               + sizeof(int);
    }
};

struct DBentry
{   // position and size of deleted block
    long pos, len;

    void read(istream& is);
    void write(ostream& is);

    static size_t data_size()
    {
        return sizeof(long)*2;
    }
};

struct RecEntry
{   //position, size and file number of record
    long pos, len;
    unsigned char nFile;

    void read(istream& is);
    void write(ostream& is);
};


// This is file of Data Base
class TDBFile:
            public TFile
{
    long FPosFree;  // len of file
    VDBhead *dh;    // header of file
    DBentry *sfe;   // stack of deleted record

    TDBFile(const TDBFile&);
    const TDBFile& operator=(const TDBFile&);

protected:
    void check_sfe();
    void v_PDBtry(fstream& fdb);
    void getHead(fstream& fdb);
    void vdbh_new( const char *VerP, const char *passwd, int nRT, bool ifDel );
    void clrh();

public:
    //  TDBFile();
    TDBFile(const gstring& fName,
            const gstring& fExt, const gstring& fDir );
    TDBFile(const gstring& path);
    TDBFile(fstream& f);

    void check_dh();

    //--- Selectors
    VDBhead* Dh() const
    {
        return dh;
    }    // header of file
    long FLen() const
    {
        return  FPosFree;
    }

    void FNewLen( long dlt )
    {
        FPosFree-=dlt;
    }
    void SetnRec( int n )
    {
        check_dh(), dh->nRec = n;
    }
    void GetDh( long& fPos, long& fLen );
    bool GetDhOver();
    void SetDh( long& fLen, int nRec );

    //---  Manipulation files ---
    void Create( unsigned char nRT, bool isDel );
    virtual void Open( FileStatus mode );
    virtual void Close();
    void PutHead( fstream& fdb, int deltRec=0 );
    void vdbh_setdt();

    //---  Manipulation deleted blocks ---
    void AddSfe( RecEntry& re );
    void FindSfe( RecEntry& re );
};

//-------------------------------------------------------------

const int MAXRKFRMSTR = 20,  // max fields in key
                        IND_PLUS = 112;    // augment of index key buf

enum keyctrl {   // codes key bild
    K_END=-5, K_EMP, K_ANY, K_IMM, K_ACT };


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
    void unpack( char *key );
    void pack( char *key );

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
    char *FldKey(int i) const
    {
        return uKey+rkInd[i];
    }
    unsigned char FldLen(int i) const
    {
        return rkLen[i];
    }
    unsigned char* RkFrm() const
    {
        return rkLen;
    }

    //--- Manipulation keys
    void SetKey( const char *key );
    void SetFldKey( int i, const char *fld );
    //  void MakeKey( int nRtwrk, char *pkey, ...);	// !!!!!!
}
;

//-------------------------------------------------------------

//This class contened the list of indexes
class TDBKeyList:
            public TDBKey
{
    int nI;
    int recInDB;  // records in DB
    int nBuf;
    RecEntry *re;
    char *ndx[MAXRKFRMSTR];

    //key list for a wildcard search
    TCStringArray aKey;
    TCIntArray anR;

    TDBKeyList(const TDBKeyList&);
    const TDBKeyList operator=(const TDBKeyList&);

protected:
    void init();
    int keycom( int );
    int keycmp( int );
    void arec_add( int );
    void check_ndx();

public:
    TDBKeyList( unsigned char nRkflds, const unsigned char* rkfrm );
    TDBKeyList( fstream& f );
    ~TDBKeyList();
    void check_i(int i);

    //--- Selectors
    int iRec() const
    {
        return nI;
    }
    long RecCount() const
    {
        return recInDB;
    }
    RecEntry* RecPosit(int i)
    {
        return &re[i];
    }
    char *RecKeyFld(int i, unsigned j) const
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
    int KeyInList()
    {
        return aKey.GetCount();
    }

    //--- Manipulation records
    int  addndx( int nF, long len, const char *key );
    void delndx(int i);
    int  findx( const char *key );
    int  xlist( const char *pattern );
    void PutKeyList( int nF, fstream& f);
    void GetKeyList_i( int nF,int nRec, fstream& f );
    void delfile( int nF );

    //--- Manipulation key
    void PutKey(int i);
    void RecKey(int i, gstring& kbuf );

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
    long rlen;    // full len the record in file
    time_t crt;
    char endm[2];

    void read(istream& is);
    void write(ostream& os);

    static size_t data_size()
    {
        return sizeof(char[2]) + sizeof(char)*2
               + sizeof(long) + sizeof(time_t) + sizeof(char[2]);
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
    TDBKeyList ind;
    TIArray<TDBFile> aFile;  // list of add files
    TCIntArray fls;

    //work param
    time_t crt;

protected:
    void putndx( int nF );
    void getndx( int nF );
    long reclen( );
    long putrec( RecEntry& re, fstream& f );
    long putrec( RecEntry& re, fstream& f, RecHead& rhh );
    long getrec( RecEntry& re, fstream& f, RecHead& rh );
    void opfils();
    int scanfile( int nF, long& fPos, long& fLen, fstream& f);
    void fromCFG(fstream& f);
    bool changedDB( int nf, bool ifRep=false );

public:
    int specialFilesNum;
    uint fNum;
    TCStringArray fOpenNameBuf;

    //  TDateBase();
    TDataBase( int nrt, char* name, bool Rclose, bool isDel,
               int nOf, unsigned char Nobj, int filesNum,
               unsigned char nRkflds, const unsigned char* rkfrm );
    TDataBase( fstream& f );
    ~TDataBase();
    void toCFG( fstream& f );
    void AddFile(const gstring& path);
    void DelFile(const gstring& path);

    //--- check
    void check_file( int nF );

    //--- Selectors
    const char* GetKeywd() const
    {
        return Keywd;
    }
    int KeyLen() const
    {
        return ind.KeyLen();
    }
    void RecKey( int i, gstring& kbuf )
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
    time_t GetTime( int i );               // time record number i
    char *FldKey( int i ) const
    {
        return ind.FldKey(i);
    }
    unsigned char FldLen(int i) const
    {
        return ind.FldLen(i);
    }
    unsigned char KeyNumFlds() const
    {
        return ind.KeyNumFlds();
    }
    bool KeyTest( const char* key );
    TDBKey& GetDBKey() const
    {
        return (TDBKey&)ind;
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
    void SetFldKey( int i, const char *fld )
    {
        ind.SetFldKey( i, fld );
    }

    void MakeKey( unsigned char nRtwrk, char *pkey, ...);
    long RecCount() const
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
    int GetOpenFileNum( const char *secondName  );
    void SetNewOpenFileList( const TCStringArray& aFlNames );
    void GetFileList( int mode, TCStringArray& names,
                      TCIntArray& indx,  TCIntArray& sel );
    //  int GetNumFiles() { return aFile.GetCount(); }
    void  MakeInNewProfile( const gstring& dir,
      const char *prfName, const char * f_name=0 );
    void OpenOnlyFromList( TCStringArray& names );
    void GetProfileFileKeywds( const char *_name, TCStringArray& aFlkey );
    bool ifDefaultOpen() const;

    //--- Manipulation Data Base
    void Create( int nF );
    void Open( bool type, FileStatus mode, const TCIntArray& indx);
    void OpenAllFiles( bool only_kernel = false );
    void Close();

    //--- Manipulation files of Data Base
    void RebildFile(const TCIntArray& indx);
    int AddFileToList(TDBFile* file);
    // int GetFileNum(const char* substr_name);
    void AddOpenFile(const TCIntArray& indx);

    //--- Manipulation records
    int AddRecordToFile(const char *key, int file);
    void Rep( int i);
    void Del( int i);
    void Get( int i);
    int Find( const char *key);
    RecStatus Rtest( const char *key, int mode = 1);
    bool FindPart( const char *key_, int field );
    //  RecStatus TryRec( RecStatus rs, char *key, int mode = 1);

    //--- Manipulation list of records
    int GetKeyList( const char *keypat,TCStringArray& aKey,TCIntArray& anR);
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

};

// Data Base container : rt

class DataBaseList:
            public TIArray<TDataBase>
{
public:
    DataBaseList():
            TIArray<TDataBase>(40)
    { }

    ~DataBaseList();

    void Init();
    void fromCFG( fstream& f );
    void toCFG(fstream& f);

    //--- Selectors
    TDataBase& operator[](uint) const;
    int Find(const char* keywd);
};

extern DataBaseList rt;

#endif	// __v_dbm_h_
