//-------------------------------------------------------------------
// $Id: v_dbfile.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of TDBFile, TDBKey, TDBKeyList, TDataBase and
//	DataBaseList classes
//
// Copyright (C) 1996-2001  S.Dmytriyeva, D.Kulik
// Uses  gstring class (C) A.Rysin 1999
//
// This file is part of the GEM-Selektor GUI library which uses the
// Qt v.4 cross-platform App & UI framework (http://qt.nokia.com)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the GPL v.3 license

//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------
//
#ifndef V_DBFILE_H_
#define V_DBFILE_H_

#include <ctime>
#include "gstring.h"
#include "v_file.h"


class GemDataStream;

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
// Sveta 64   long MinDrLen, MaxDrLen; // min and max size of deleted block
    int MinDrLen, MaxDrLen; // min and max size of deleted block
    int curDr;               // number of deleted blocks

    void read(GemDataStream& is);
    void write(GemDataStream& is);

    static int data_size()
    {
        return sizeof(char[8])*2
               + sizeof(char[11]) + sizeof(char[5])
               + sizeof(int)*4
// Sveta 64    + sizeof(char) + sizeof(long)*2
               + sizeof(char) + sizeof(int)*2
               + sizeof(int);
    }
};

struct DBentry
{   // position and size of deleted block
// Sveta 64    long pos, len;
    int pos, len;

    void read(GemDataStream& is);
    void write(GemDataStream& is);

    static size_t data_size()
    {
// Sveta 64  return sizeof(long)*2;
     return sizeof(int)*2;
    }
};

struct RecEntry
{   //position, size and file number of record
// Sveta 64    long pos, len;
    int     pos, len;
    unsigned char nFile;

    void read(GemDataStream& is);
    void write(GemDataStream& is);
};


// This is file of Data Base
class TDBFile:
            public TFile
{
// Sveta 64    long FPosFree;  // len of file
    int     FPosFree;  // len of file
    VDBhead *dh;    // header of file
    DBentry *sfe;   // stack of deleted record

    TDBFile(const TDBFile&);
    const TDBFile& operator=(const TDBFile&);

protected:
    void check_sfe();
    void v_PDBtry(GemDataStream& fdb);
    void getHead(GemDataStream& fdb);
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
        check_dh();
        dh->nRec = n;
    }
    void GetDh( int& fPos, int& fLen, int& nRT, char& isDel );
    bool GetDhOver();
    void SetDh( int fLen, int nRec, int nRT, char isDel );

    //---  Manipulation files ---
    void Create( unsigned char nRT, bool isDel );
    virtual void Open( FileStatus mode );
    virtual void Close();
    void PutHead( GemDataStream& fdb, int deltRec=0 );
    void vdbh_setdt();

    //---  Manipulation deleted blocks ---
    void AddSfe( RecEntry& re );
    void FindSfe( RecEntry& re );
};


#endif  // V_DBFILE_H_
