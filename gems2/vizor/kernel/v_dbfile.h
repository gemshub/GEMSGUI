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
#ifndef __v_dbfile_h_
#define __v_dbfile_h_

#include <time.h>
#include "gstring.h"

#include "array.h"
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
    long MinDrLen, MaxDrLen; // min and max size of deleted block
    int curDr;               // number of deleted blocks

    void read(GemDataStream& is);
    void write(GemDataStream& is);

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

    void read(GemDataStream& is);
    void write(GemDataStream& is);

    static size_t data_size()
    {
        return sizeof(long)*2;
    }
};

struct RecEntry
{   //position, size and file number of record
    long pos, len;
    unsigned char nFile;

    void read(GemDataStream& is);
    void write(GemDataStream& is);
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
    
    long FLen() const
    {
        return FPosFree;
    }

    void FNewLen( long dlt )
    {
        FPosFree -= dlt;
    }
    void SetnRec( int n )
    {
        check_dh(), dh->nRec = n;
    }
    void GetDh( long& fPos, long& fLen, int& nRT, bool& isDel );
    bool GetDhOver();
    void SetDh( long fLen, int nRec, int nRT, bool isDel );

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


#endif
