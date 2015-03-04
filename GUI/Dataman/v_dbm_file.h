//-------------------------------------------------------------------
// $Id: v_dbm_file.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of TDBFile class and some filename related constants
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

#ifndef _v_dbm_file_h_
#define _v_dbm_file_h_

#include <list>
#include "v_file.h"

class GemDataStream;

/// Header of PDB file
struct VDBhead
{
    char VerP[8];
    char PassWd[8];
    char Date[11];
    char Time[5];
    int nRT;                 // type of PDB chain
    int nRec;                // number records in file
    int  stacOver,FPosTRT;
    char isDel;
// Sveta 64   long MinDrLen, MaxDrLen; // min and max size of deleted block
    int MinDrLen, MaxDrLen; // min and max size of deleted block
    int curDr;               // number of deleted blocks

    void read(GemDataStream& is);
    void write(GemDataStream& is);

    static size_t data_size()
    {
        return sizeof(char[8])*2
               + sizeof(char[11]) + sizeof(char[5])
               + sizeof(int)*4
// Sveta 64    + sizeof(char) + sizeof(long)*2
               + sizeof(char) + sizeof(int)*2
               + sizeof(int);
    }
};

/// Position and size of deleted block
struct DBentry
{
// Sveta 64    long pos, len;
    int pos, len;

    DBentry( int apos, int alen ):
        pos(apos), len(alen)
    {}
    void read(GemDataStream& is);
    void write(GemDataStream& is);

    static size_t data_size()
    {
// Sveta 64  return sizeof(long)*2;
     return sizeof(int)*2;
    }
};


/// Position, size and file number of record
struct RecEntry
{   int     pos, len;
    unsigned char nFile;

    RecEntry(int apos, int alen, unsigned char anFile ):
        pos(apos), len(alen), nFile(anFile)
    {}
    RecEntry():
        pos(-1), len(-1), nFile(-1)
    {}
    void read(GemDataStream& is);
    void write(GemDataStream& is);
};

/// Head the record in DB file
struct RecHead
{
    char bgm[2];
    unsigned char nRT, Nobj;
// Sveta 64   long rlen;    // full len the record in file
    int rlen;    // full len the record in file
// Sveta 64    time_t crt;
    int crt;
    char endm[2];

    void read(GemDataStream& is);
    void write(GemDataStream& os);

    static size_t data_size()
    {
        return sizeof(char[2]) + sizeof(char)*2
// Sveta 64               + sizeof(long) + sizeof(time_t) + sizeof(char[2]);
               + sizeof(int) + sizeof(int) + sizeof(char[2]);
    }
};

/// This is file of Data Base
class TDBFile:
            public TFile
{
    static  char vv[9];
    static  char pa[9];

    // Sveta 64    long FPosFree;  // len of file
    int     FPosFree;  // len of file
    VDBhead dh;    // header of file
    list<DBentry> sfe;   // stack of deleted record

    bool defaultDB; // file from DB.default

    TDBFile(const TDBFile&);
    const TDBFile& operator=(const TDBFile&);

protected:

    void makeKeyword();

    void getHead(GemDataStream& fdb);
    void vdbh_new( const char *VerP, const char *passwd, int nRT, bool ifDel );
    void clrh();

public:

    GemDataStream& f;

    //  TDBFile();
//  TDBFile(const string& fName,
//          const string& fExt, const string& fDir );
    TDBFile(const string& path);
    TDBFile(fstream& f);
    virtual ~TDBFile();

    bool isDefaultDB()
    {
        return defaultDB;
    }

    //---  Manipulation files ---
    void Create( unsigned char nRT, bool isDel );
    virtual void Open( );
    virtual void Close();

    //--- Selectors
    void DhWrite(GemDataStream& is )
    {
        dh.write(is);
    }    // header of file

    int FLen() const
    {
        return FPosFree;
    }

    void FNewLen( int dlt )
    {
        FPosFree -= dlt;
    }

    void SetnRec( int n )
    {
        dh.nRec = n;
    }

    void GetDh( int& fPos, int& fLen, int& nRT, int& isDel );
    bool GetDhOver();
    void SetDh( int fLen, int nRec, int nRT, int isDel );

    void PutHead( GemDataStream& fdb, int deltRec=0 );
    void vdbh_setdt();

    //---  Manipulation deleted blocks ---
    void AddSfe( RecEntry& re );
    void FindSfe( RecEntry& re );
};


// file names constants

extern const char * PDB_EXT;
extern const char * NDX_EXT;

#endif // _v_dbm_file_h
