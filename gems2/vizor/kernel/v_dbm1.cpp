//-------------------------------------------------------------------
// Id: gems/vizor/kernel/v_module.cpp  version 2.0.0    2001
//
// Implementation of TDBFile, TDBKey and TDBKeyList classes
//
// Created : 970207    Modified: 010908
//
// Copyright (C) 1996-2001 S.Dmytriyeva, D.Kulik
// Uses  gstring class (C) A.Rysin 1999
//
// This file is part of the GEM-Vizor library which uses the
// Qt v.2.x GUI Toolkit (Troll Tech AS, http://www.trolltech.com)
// according to the Qt Duo Commercial license #1435515
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL)
//
// See http://les.web.psi.ch/Software/GEMS-PSI/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------
//
#include <stdio.h>
#ifndef __unix
# include <io.h>
#else
# include <unistd.h>
#endif
#include <stdlib.h>
#include <stdarg.h>

#include "v_dbm.h"
#include "v_object.h"

void
VDBhead::read(istream& is)
{
    is.read (VerP, 8 );
    is.read (PassWd, 8);
    is.read (Date, 11);
    is.read (Time, 5);
    is.read ((char*)&nRT, sizeof (int));                 // type of PDB chain
    is.read ((char*)&nRec, sizeof (int));                // number records in file
    is.read ((char*)&FPosRE, sizeof (int));
    is.read ((char*)&FPosTRT, sizeof (int));
    is.read (&isDel, sizeof (char));
    is.read ((char*)&MinDrLen, sizeof(long));
    is.read ((char*)&MaxDrLen, sizeof(long)); // min and max size of deleted block
    is.read ((char*)&curDr, sizeof(int));
    ;               // number of deleted blocks
}


void
VDBhead::write(ostream& is)
{
    is.write (VerP, 8 );
    is.write (PassWd, 8);
    is.write (Date, 11);
    is.write (Time, 5);
    is.write ((char*)&nRT, sizeof (int));                 // type of PDB chain
    is.write ((char*)&nRec, sizeof (int));                // number records in file
    is.write ((char*)&FPosRE, sizeof (int));
    is.write ((char*)&FPosTRT, sizeof (int));
    is.write (&isDel, sizeof (char));
    is.write ((char*)&MinDrLen, sizeof(long));
    is.write ((char*)&MaxDrLen, sizeof(long)); // min and max size of deleted block
    is.write ((char*)&curDr, sizeof(int));
    ;               // number of deleted blocks
}


void
DBentry::read(istream& is)
{
    is.read ((char*)&pos, sizeof (long));
    is.read ((char*)&len, sizeof (long));
}

void
DBentry::write(ostream& is)
{
    is.write ((char*)&pos, sizeof (long));
    is.write ((char*)&len, sizeof (long));
}

void
RecEntry::read(istream& is)
{
    is.read ((char*)&pos, sizeof (long));
    is.read ((char*)&len, sizeof (long));
    is.read ((char*)&nFile, sizeof (unsigned char));
}

void
RecEntry::write(ostream& is)
{
    is.write ((char*)&pos, sizeof (long));
    is.write ((char*)&len, sizeof (long));
    is.write ((char*)&nFile, sizeof (unsigned char));
}

void
RecHead::read(istream& is)
{
    is.read (bgm, 2);
    is.read ((char*)&nRT, sizeof (unsigned char));
    is.read ((char*)&Nobj, sizeof (unsigned char));
    is.read ((char*)&rlen, sizeof (long));    // full len the record in file
    is.read ((char*)&crt, sizeof (time_t));
    is.read (endm, 2);
}

void
RecHead::write(ostream& is)
{
    is.write (bgm, 2);
    is.write ((char*)&nRT, sizeof (unsigned char));
    is.write ((char*)&Nobj, sizeof (unsigned char));
    is.write ((char*)&rlen, sizeof (long));    // full len the record in file
    is.write ((char*)&crt, sizeof (time_t));
    is.write (endm, 2);
}

// class TDBFile, TDBKey, TDBKeyList


//-------------------------------------------------------------
// TDBFile
//-------------------------------------------------------------

// default configurations
TDBFile::TDBFile(const gstring& fName,
                 const gstring& fExt, const gstring& fDir):
        TFile(fName, fExt, fDir),
        FPosFree(-1),
        dh(0),
        sfe(0)
{}


TDBFile::TDBFile(const gstring& path):
        TFile(path),
        FPosFree(1),
        dh(0),
        sfe(0)
{}

//load configurations from cfg file

TDBFile::TDBFile(fstream& file):
        TFile(file),
        FPosFree(-1),
        dh(0),
        sfe(0)
{}



void 
TDBFile::check_sfe()
{
    ErrorIf( sfe == 0 , GetKeywd(),
             "No memory for deleted records.");
}

void
TDBFile::check_dh()
{
    ErrorIf( dh == 0 , GetKeywd(),
             "No memory for header of file.");
}

// Set date and time in header of file.
void 
TDBFile::vdbh_setdt()
{
    check_dh();

    strcpy(dh->Date, curDate().c_str());
    strcpy(dh->Time, curTime().c_str());
}

//Clear the header of PDB file.
void 
TDBFile::clrh()
{
    check_dh();
    FPosFree = -1;
    if( sfe )
        memset( sfe, 0, sizeof(DBentry)*MAXFESTACK );
    memset( dh, 0, sizeof( VDBhead ));
}

//Set header of PDB file
void 
TDBFile::vdbh_new( const char *VerP,
                        const char *passwd,	int nRT, bool ifDel )
{
    if( dh==0 )
        dh = new VDBhead;
    if( ifDel && sfe == 0 )
        sfe = new DBentry[MAXFESTACK];
    clrh();
    if( VerP )
        strncpy( dh->VerP, VerP, 8 );
    if( passwd )
        strncpy( dh->PassWd, passwd, 8 );
    vdbh_setdt();
    dh->nRT = nRT;
    dh->FPosTRT = dh->FPosRE = VDBhead::data_size();
    dh->isDel = ifDel;
    if( ifDel )
        dh->FPosTRT += DBentry::data_size() * MAXFESTACK;
    dh->nRec = 0;
    dh->curDr = 0;
    dh->MinDrLen = 0;
    dh->MaxDrLen = 0;
}

//Create new PDB file. If file exist, it will be deletet.
void 
TDBFile::Create( unsigned char nRT, bool isDel )
{
    fstream ff;

    // if( exist() ) return; in DB

    vdbh_new( TFile::VV(), TFile::PA(), nRT, isDel );
    FPosFree = dh->FPosTRT;
    ff.open( GetPath().c_str(), WRITE_B );
    //   ff.write( (char*)dh, sizeof( VDBhead ) );
    dh->write (ff);
    if( isDel )
        //     ff.write( (char *)sfe, sizeof( DBentry )*MAXFESTACK);
        for (int ii=0; ii<MAXFESTACK; ii++)
            sfe[ii].write (ff);
    ErrorIf( !ff.good(), GetPath(),
             "PDB file creation error.");
    ff.close();
}

// Read the control structure of PDB file.
void  
TDBFile::getHead(fstream& fdb)
{
    check_dh();
    fdb.seekg( 0L, ios::beg );
    //  fdb.read( (char *)dh, sizeof(VDBhead) );
    dh->read (fdb);

    if( dh->isDel )
    {
        if( sfe == 0 )
            sfe = new DBentry[MAXFESTACK];
        // fdb.read( (char *)sfe, MAXFESTACK*sizeof( DBentry ));
        for( int ii=0; ii<MAXFESTACK; ii++)
            sfe[ii].read (fdb);
    }
    ErrorIf( !fdb.good(), GetPath(), "PDB head read error.");
}

// Write the control structure of PDB file.
void
TDBFile::PutHead( fstream& fdb, int deltRec )
{
    check_dh();
    dh->nRec +=deltRec;
    vdbh_setdt();
    fdb.seekg(0L, ios::beg );
    // fdb.write( (char *)dh, sizeof(VDBhead) );
    dh->write (fdb);
    if( dh->isDel )
        //    fdb.write( (char *)sfe, MAXFESTACK*sizeof( DBentry ));
        for( int ii=0; ii<MAXFESTACK; ii++)
            sfe[ii].write (fdb);
    ErrorIf( !fdb.good(), GetPath(), "Database file write (protection?) error");
    fdb.flush();
}

//Read and test VDBhead
void 
TDBFile::v_PDBtry( fstream& fdb )
{
    if( dh==0 )
        dh = new VDBhead;
    getHead( fdb );
    // check version
    //  if( strncmp( dh->VerP, TFile::VV(), 8 ))
    //      Error( GetPath(), "Illegal version in PDB file.");
    fdb.seekg(0l, ios::end);
    FPosFree = fdb.tellg();
}

// Open PDB file in special mode
void
TDBFile::Open( FileStatus mode )
{
    if( Test() == mode )
        return;
    if( Test() != NOT_OPEN )
        Close();
    TFile::Open( mode );
    v_PDBtry( f );
}

//Close PDB file
void 
TDBFile::Close()
{
    delete dh;
    delete[] sfe;
    dh = 0;
    sfe = 0;
    FPosFree = 0;
    TFile::Close();
}

int 
rlencomp( const void *ie_1, const void *ie_2 )
{
    DBentry* ie1 = (DBentry *)ie_1;
    DBentry* ie2 = (DBentry *)ie_2;
    if( ie1->len > ie2->len ) return 1;
    if( ie1->len < ie2->len ) return -1;
    return 0;
}

//Add new deleted record in sfe list and mark deleted record in file
void 
TDBFile::AddSfe( RecEntry& re )
{
    RecHead rh;

    check_dh();
    int Ndr = dh->curDr;
    if( dh->isDel )
    {
        check_sfe();
        ErrorIf( Ndr>=MAXFESTACK , GetPath(),
                 "Stack of deleted records overflow.");
        sfe[Ndr].len = re.len;
        sfe[Ndr].pos = re.pos;
        dh->curDr++;
        if( Ndr )   // sort sfe list
            qsort( sfe, Ndr+1, sizeof(DBentry), rlencomp );
        dh->MinDrLen = sfe[0].len;
        dh->MaxDrLen = sfe[Ndr].len;
    }
    // mark deleted record in file
    f.seekg(re.pos, ios::beg );
    //   f.read( (char *)&rh, sizeof(RecHead) );
    rh.read (f);
    if( !strncmp( rh.bgm, MARKRECHEAD, 2 ))
    {
        strncpy( rh.bgm, MARKRECDEL, 2 );
        strncpy( rh.endm, MARKRECDEL, 2 );
    }
    f.seekg(re.pos, ios::beg );
    //   f.write( (char *)&rh, sizeof(RecHead) );
    rh.write (f);
    ErrorIf( !f.good(), GetPath(), "PDB file write error.");
}

//Add new deleted record in sfe list and mark deleted record in file
void 
TDBFile::FindSfe( RecEntry& re )
{
    int i,j;
    RecEntry rn;

    check_dh();
    int Ndr = dh->curDr;
    if( !dh->isDel || Ndr==0 || dh->MaxDrLen<re.len)
        goto NoBlock;  // no sfe list or no deleted blocks or big len
    j = -1;    // seach block
    for( i=0; i<Ndr; i++ )
    {
        if( sfe[i].len < re.len )
            continue;
        else
        {
            j = i;
            break;
        }
    }
    if( j < 0 )
        goto NoBlock; // no block
    re.pos = sfe[j].pos;
    rn.pos = re.pos + re.len;
    rn.len = sfe[j].len - re.len;
    rn.nFile = re.nFile;
    // deleted block from sfe list
    memmove( sfe+j, sfe+j+1, ( dh->curDr-- - j)*sizeof(DBentry) );
    j = dh->curDr;
    memset( sfe+j, 0, sizeof(DBentry));
    if( rn.len >= dh->MinDrLen )
        AddSfe( rn );
    return;
NoBlock:
    re.pos = FPosFree;
    FPosFree += re.len;
}

//get information from dh
void 
TDBFile::GetDh( long& fPos, long& fLen )
{
    check_dh();
    fPos = dh->FPosTRT;
    fLen = FPosFree;
}

//set information to dh
void 
TDBFile::SetDh( long& fLen, int nRec )
{
    check_dh();
    dh->curDr = 0;
    dh->MinDrLen = 0;
    dh->MaxDrLen = 0;
    FPosFree = fLen;
    dh->nRec = nRec;
    if( dh->isDel )
    {
        check_sfe();
        memset( (char *)sfe, 0, sizeof(DBentry)*MAXFESTACK );
    }
    PutHead( f );
#ifndef __unix
    int handle = f.rdbuf()->fd();
    chsize( handle, fLen );
#else
#if __GNUC__ != 3
    int handle = f.rdbuf()->fd();
#else
cerr << "trunc dummy" << endl;
///  WE HAVE TO GET HANDLE SOMEHOW!!! 
    int handle = 0;
#endif // __GCC__ != 3
    ftruncate( handle, fLen );
#endif
    f.flush();
}

//-------------------------------------------------------------
// TDBKey
//-------------------------------------------------------------

// default configuration
TDBKey::TDBKey( unsigned char nRkflds, const unsigned char* rkfrm ):
        rkFlds( nRkflds )
{
    ErrorIf( rkFlds<=0 || rkFlds>MAXRKFRMSTR,
             "TDBKey", "Illegan number fields in key.");
    rkLen = new unsigned char[rkFlds];
    rkInd = new unsigned char[rkFlds];
    int i;
    for( fullLen=0, i=0; i<rkFlds; i++)
    {
        rkLen[i] = rkfrm[i];
        if( i==0 )
            rkInd[i]=0;
        else
            rkInd[i] = (unsigned char)(rkInd[i-1] + rkLen[i-1]);
        fullLen += rkLen[i];
    }
    pKey = new char[fullLen+rkFlds+1+10];
    uKey = new char[fullLen+1+10];
    SetKey( ALLKEY);
    //pKey[0]='*'; pKey[1]='\0';
    //strcpy( uKey, pKey );
}

//configuration from cfg file

TDBKey::TDBKey(fstream& in_stream)
{
//    f.read( (char*)&rkFlds, sizeof(unsigned char) );
    unsigned short tmp;
in_stream >> tmp;
rkFlds = tmp;
    ErrorIf( rkFlds<=0 || rkFlds>MAXRKFRMSTR,
             "TDBKey", "Illegan number fields in key.");
    rkLen = new unsigned char[rkFlds];
    rkInd = new unsigned char[rkFlds];
//    f.read( (char*)rkLen, rkFlds*sizeof(unsigned char) );
for(int ii=0; ii<rkFlds; ii++) {
    in_stream >> tmp;
    rkLen[ii] = tmp;
}

    ErrorIf( !in_stream.good(), "TDBKey",
             "Error reading TDBKey from configurator.");
    int ii;
    for( fullLen=0, ii=0; ii<rkFlds; ii++)
    {
        if( ii == 0 )
            rkInd[ii] = 0;
        else
            rkInd[ii] = (unsigned char)(rkInd[ii-1]+rkLen[ii-1]);
        fullLen += rkLen[ii];
    }
    pKey = new char[fullLen+rkFlds+1+10];
    uKey = new char[fullLen+1+10];
    SetKey( ALLKEY);
    //  pKey[0]='*'; pKey[1]='\0';
    //  strcpy( uKey, pKey );
}

// copy struct

TDBKey::TDBKey( const TDBKey& dbKey ):
        rkFlds( dbKey.rkFlds ),fullLen( dbKey.fullLen)
{
    rkLen = new unsigned char[rkFlds];
    rkInd = new unsigned char[rkFlds];
    pKey = new char[fullLen+rkFlds+1+10];
    uKey = new char[fullLen+1+10];
    for(int i=0; i<rkFlds; i++)
    {
        rkLen[i] = dbKey.rkLen[i];
        rkInd[i] = dbKey.rkInd[i];
    }
    strncpy(pKey, dbKey.pKey, fullLen+rkFlds+1);
    strncpy(uKey, dbKey.uKey, fullLen+1);
}

TDBKey::~TDBKey()
{
    delete[] rkLen;
    delete[] rkInd;
    delete[] pKey;
    delete[] uKey;
}

void 
TDBKey::check()
{
    ErrorIf( pKey==0 || uKey==0 || rkLen==0 || rkInd==0,
             "TDBkey", "No memory for key struct.");
}


void 
TDBKey::SetKey( const char *key )
{
    ErrorIf( key==0,"TDBKey", "No key buf.");

    vstr sp(fullLen+rkFlds+1);
    if( strncmp( key, ALLKEY, fullLen )==0 )
    {
        gstring st;
        for(int i=0; i<rkFlds; i++)
            st += "*:";
        strncpy( sp, st.c_str(), fullLen);
    }
    else
        strncpy( sp, key, fullLen);

    sp[fullLen] = '\0';
    if( strchr( sp, ':' ))  // key in packed form
        unpack( sp );
    else
        pack( sp );
}


void 
TDBKey::SetFldKey( int i, const char *key )
{
    ErrorIf( i>= rkFlds, key, "Illegal key field number");

    memset( uKey+rkInd[i], ' ' , rkLen[i] );
    strncpy( uKey+rkInd[i], key, rkLen[i] );
}

/*
// unpack key
void
TDBKey::unpack( char *key )
{
    int i;
    unsigned ln;
    char *sp, *sp1;

    check();
    if( strchr( key, ':' ) )  // key in packed form
    {
        strncpy( pKey, key, fullLen);
        pKey[fullLen] = '\0';
        memset( uKey, ' ', fullLen );
        uKey[fullLen]= '\0';
        for( i=0, sp=pKey; i<rkFlds; i++)
        {
            sp1 = strchr( sp, ':' );
            if( !sp1 )
            {
                if( strncmp( sp, S_ANY, 1 ))
                {
                    ln = strlen( sp );
                    ErrorIf( ln==0, key, "Illegal packed record key.");
                }
                else
                { // end '*'
                    ln = 1;
                    strncpy( uKey+rkInd[i], sp, ln );
                    break;
                }
            }
            else  ln = ( sp1- sp);
            ln = min( ln, (unsigned int)rkLen[i] );
            strncpy( uKey+rkInd[i], sp, ln );
            if( *(sp+ln) == ':' ) ln++;
            sp += ln;
        }
        uKey[fullLen]= '\0';
        strncpy( key, uKey, fullLen );
    }
}
*/


// unpack key
void
TDBKey::unpack( char *key )
{
    int i;
    unsigned ln;
    char *sp, *sp1;

    check();
    if( strchr( key, ':' ) )  // key in packed form
    {
        strncpy( pKey, key, fullLen);
        pKey[fullLen] = '\0';
        memset( uKey, ' ', fullLen );
        uKey[fullLen]= '\0';
        for( i=0, sp=pKey; i<rkFlds; i++)
        {
            sp1 = strchr( sp, ':' );
            if( !sp1 )
            {
                if( strncmp( sp, S_ANY, 1 ))
                {
                    ln = strlen( sp );
                    ErrorIf( ln==0, key, "Illegal packed record key.");
                }
                else
                { // end '*'
                    ln = 1;
                    strncpy( uKey+rkInd[i], sp, ln );
                    break;
                }
            }
            else  ln = ( sp1- sp);
            ln = min( ln, (unsigned int)rkLen[i] );
            strncpy( uKey+rkInd[i], sp, ln );
            if( *(sp+ln) == ':' )
                  ln++;
            else
              if( *(sp+ln+1) == ':' )
                  ln+=2;
            sp += ln;
        }
        uKey[fullLen]= '\0';
        strncpy( key, uKey, fullLen );
    }
}

/*
// pack key
void
TDBKey::pack( char *key )
{
    int i, len, ln;
    char *sp;

    check();
    if( !strchr( key, ':' ))  // key in unpacked form
    {
        strncpy( uKey, key, fullLen);
        uKey[fullLen] ='\0';
        memset( pKey, '\0', fullLen+1 );
        for( i=0, len=0; i<rkFlds; i++)
        {
            sp = FldKey(i);
            for( ln = rkLen[i]-1; ln>=0; ln--)
                if(  *(sp+ln) != ' ' ) break;
            ln++;
            if( ln < rkLen[i] )
            {
                strncpy( pKey+len, sp, ln );
                len += ln+1;
                pKey[len-1]=':';
            }
            else
            {
                strncpy(pKey+len, sp, rkLen[i] );
                len += rkLen[i];
            }
        }
        strncpy( key, pKey, fullLen );
    }
}
*/

// pack key
void
TDBKey::pack( char *key )
{
    int i, len, ln;
    char *sp;

    check();
    if( !strchr( key, ':' ))  // key in unpacked form
    {
        strncpy( uKey, key, fullLen);
        uKey[fullLen] ='\0';
        memset( pKey, '\0', fullLen+1 );
        for( i=0, len=0; i<rkFlds; i++)
        {
            sp = FldKey(i);
            for( ln = rkLen[i]-1; ln>=0; ln--)
                if(  *(sp+ln) != ' ' ) break;
            ln++;
            if( ln < rkLen[i] )
            {
                strncpy( pKey+len, sp, ln );
                len += ln+1;
                pKey[len-1]=':';
            }
            else
            {
                strncpy(pKey+len, sp, rkLen[i] );
                len += rkLen[i]+1;
                pKey[len-1]=':';
            }
        }
        strncpy( key, pKey, fullLen );
    }
}

// make packed key to seach.
void
TDataBase::MakeKey( unsigned char nRTwrk, char *pkey, ... )
{
    va_list Marker;
    uint rkflen;
    int i, rts, nkf;
    char *sp, *imf;

    ErrorIf( pkey==0,"MakeKey", "No key buf to make key.");
    *pkey = 0;
    va_start( Marker, pkey );
    for( i=0; i<ind.KeyNumFlds(); i++ )
    {
        sp= pkey+strlen(pkey);
        rkflen = ind.FldLen(i);
        rts = va_arg( Marker, int );
        switch( rts )
        {
        case K_END:
            strcat( pkey, S_ANY );
            va_end( Marker );
            return;
        case K_EMP:  // field "`"
            strcat( pkey, S_EMPTY );
            break;
        case K_ANY:  // field  "*"
            strcat( pkey, S_ANY );
            break;
        case K_IMM:  // field in gstring
            imf = va_arg( Marker, char * );
            strncat( pkey, imf, min( strlen(imf), rkflen ));
            break;
        case K_ACT:  // get field from  PRIE request
            rts = nRTwrk;
        default:     // get field from enathe chain key
            nkf = va_arg( Marker, int );
            if( !*rt[rts].FldKey( nkf ))
                strcat( pkey, S_ANY );
            else
            {
                gstring str=  gstring( rt[rts].FldKey( nkf ), 0,
                                       min( (unsigned int)rt[rts].FldLen(nkf), rkflen ));
                StripLine( str );
                strncat( pkey, str.c_str(), rkflen );
            }
            break;
        }
//04/09/01 ????        if( strlen(sp) < rkflen )
            strcat( pkey, ":" );
    }
    va_end( Marker );
}

//-------------------------------------------------------------
// TDBKeyList
//-------------------------------------------------------------

// default configuration
TDBKeyList::TDBKeyList( unsigned char nRkflds, const unsigned char* rkfrm ):
        TDBKey( nRkflds, rkfrm )
{
    init();
}

//configuration from cfg file
TDBKeyList::TDBKeyList(fstream& f):
        TDBKey(f)
{
    init();
}

void 
TDBKeyList::check_ndx()
{
    ErrorIf( ndx==0 || re==0 ,"TDBMKeyList",
             "No memory for list of indexes.");
}

void 
TDBKeyList::check_i(int i)
{
    if( i<0||i>recInDB )
        Error( "TDBMKeyList","Illegal record index in the chain.");
}

void 
TDBKeyList::init()
{
    nI = -1;
    recInDB = 0;
    nBuf = IND_PLUS;
    int k;

    for(int i=0; i<KeyNumFlds(); i++)
    {
        k = nBuf * FldLen(i);
        ndx[i] = new char[k];
    }
    re = new RecEntry[nBuf];
}

// reset index key list
void
TDBKeyList::initnew()
{
    nI = -1;
    recInDB = 0;
    for(int j=0; j<KeyNumFlds(); j++)
        memset( ndx[j], 0, FldLen(j)*nBuf*sizeof(char) );
    memset( re, 0, nBuf*sizeof(RecEntry) );
    aKey.Clear();
    anR.Clear();
}

// delete the keys of records file nF
void 
TDBKeyList::delfile( int nF )
{
    int i=0;
    while( i < recInDB )
    {
        if( re[i].nFile == nF )
            delndx( i );
        else i++;
    }
}

TDBKeyList::~TDBKeyList()
{
    for(int i=0; i<KeyNumFlds(); i++)
        delete[] ndx[i];
    delete[] re;
}

// compare key i record and work key
int 
TDBKeyList::keycom( int i )
{
    int l = 0;
    for( int j=0; j<KeyNumFlds(); j++)
    {
        l = memcmp( FldKey(j), RecKeyFld( i,j ), FldLen(j) );
        if( l ) return l;
    }
    return l;
}

//Add new key of record in list. Realloc memory if nessasery.
int
TDBKeyList::addndx( int nF, long len, const char *key )
{
    int i, l, j;
    //unsigned j;

    SetKey( key );
    if( recInDB >= nBuf ) // realloc buf
    {
        nBuf += IND_PLUS;
        for( j=0; j<KeyNumFlds(); j++)
        {
            char* new_sp = new char[nBuf*FldLen(j)];
            memcpy(new_sp, ndx[j], (nBuf-IND_PLUS)*FldLen(j));
            delete[] ndx[j];
            ndx[j] = new_sp;
        }
        //            ndx[j] = (char *)realloc( ndx[j], nBuf*FldLen(j)*sizeof(char));

        //     re = (RecEntry *)realloc( re, nBuf*sizeof(RecEntry));

        RecEntry* new_sp = new RecEntry[nBuf];
        memcpy(new_sp, re, (nBuf-IND_PLUS)*sizeof(RecEntry));
        delete[] re;
        re = new_sp;
    }
    i = recInDB;
    if( i )  l = keycom( i-1 );
    else l = 1;
    while( l < 0 )
    {
        i--;
        if(!i) break;
        l = keycom( i-1 );
    }
    ErrorIf( l==0, key, "Two records with the same key.");
    for( j=0; j<KeyNumFlds(); j++)
        memmove( RecKeyFld(i+1,j), RecKeyFld(i,j),
                 FldLen(j)*(recInDB-i)*sizeof(char) );
    memmove( re+i+1, re+i, (recInDB-i)*sizeof(RecEntry) );
    for( j=0; j<KeyNumFlds(); j++)
        memcpy( RecKeyFld(i,j), FldKey(j), FldLen(j) );
    re[i].len = len;
    re[i].nFile = (unsigned char)nF;   //warning
    nI = i;
    recInDB++;
    return i;
}

// Delete record from list
void 
TDBKeyList::delndx( int i )
{
    int j;

    check_i(i);
    for( j=0; j<KeyNumFlds(); j++)
        memmove( RecKeyFld(i,j), RecKeyFld(i+1,j),
                 FldLen(j)*(recInDB-i)*sizeof(char) );
    memmove( re+i, re+i+1, (recInDB-i)*sizeof(RecEntry) );
    recInDB--;
    if( i == recInDB )  i--;
    nI = i;
}

// find record in list
int 
TDBKeyList::findx( const char *key )
{
    int i,l;

    SetKey( key );
    if( recInDB <= 0 ) return -1;
    for( i=0; i< recInDB; i++ )
    {
        l = keycom(i);
        if( l < 0 ) // No record
            break;
        if( l == 0 )
        {
            nI = i;
            return i;
        }
    }
    return -1;
}

//Set key i - record to work key structure
void 
TDBKeyList::PutKey( int i)
{
    int j;
    check_i(i);
    TDBKey::check();
    for( j=0; j<KeyNumFlds(); j++)
        memcpy( FldKey(j), RecKeyFld(i,j), FldLen(j) );
    SetKey( FldKey(0) );
}

//Put key i record to kbuf in unpack form.
void 
TDBKeyList::RecKey(int i, gstring& kbuf )
{
    int j;
    check_i(i);
    kbuf = "";
    for( j=0; j<KeyNumFlds(); j++)
        kbuf += gstring( RecKeyFld(i,j), 0, FldLen(j) );
}

// write the keys of records to ndx file
void 
TDBKeyList::PutKeyList( int nF, fstream& f)
{
    unsigned char j;
    for(int i=0; i< recInDB; i++ )
        if( re[i].nFile == nF )
        {
            for( j=0; j<KeyNumFlds(); j++)
                f.write(  RecKeyFld(i,j), FldLen(j) );
            //        f.write( (char *)(re+i), sizeof(RecEntry) );
            re[i].write (f);
        }
    ErrorIf( !f.good(), "TDBKeyList",  "Index file write error.");
}

// read the keys of records from ndx file
void 
TDBKeyList::GetKeyList(int nF, int nRec, fstream& f)
{
    RecEntry re_;

    vstr key(KeyLen()+1);
    memset( key, 0, KeyLen()+1);
    for( int i=0; i< nRec; i++ )
    {
        f.read( key, KeyLen());
        //     f.read( (char *)&re_, sizeof(RecEntry));
        re_.read (f);
        int indRec = addndx( nF, re_.len, key);
        re[indRec].pos = re_.pos;
    }

    ErrorIf( !f.good(), "TDBKeyList", "Index file read error.");
}

// compare key ni record and pattern in work key
int
TDBKeyList::keycmp( int ni )
{
    if( !(strchr(UnpackKey(),'*') || strchr(UnpackKey(),'?')))
        return  keycom(ni);      // no pattern

    for( int j=0; j<KeyNumFlds(); j++)
    {
        char *sp = FldKey(j);
        if( *(sp) == '*' )
	    continue; //to next field

        for( int i=0; i<FldLen(j); i++ )
        {
            switch( *(sp+i) )
            {
            case '?': // no ' '
                if( *(RecKeyFld(ni,j)+i) == ' ' )
		    return( j+1 );
                break;

            case '*':
                goto NEXTKF;

            default:
                if( *(sp+i) != *(RecKeyFld(ni,j)+i) )
                    return( j+1 );
            }
        }
NEXTKF:
        ;
    }

    return 0;
}

//add records key in key list for a wildcard search
void 
TDBKeyList::arec_add( int ni )
{
    gstring s;
    RecKey( ni, s);
    aKey.Add( s );
    anR.Add( ni );
}

//get key list for a wildcard search
int 
TDBKeyList::xlist( const char *pattern )
{
    bool OneRec, AllRecs;
    int i;

    SetKey( pattern );
    // analyze pattern
    if( strchr( UnpackKey(), '*' ) || strchr( UnpackKey(), '?' ))
        OneRec = false;
    else
    {
        OneRec = true;/* AllRecs = false; */
    }
    if( !OneRec && strspn(UnpackKey(),"* :") < strlen(UnpackKey()) )
        AllRecs = false;
    else
        AllRecs = true;
    aKey.Clear();
    anR.Clear();
    if( OneRec )
    {
        i = findx( pattern );
        if( i >= 0 )
            arec_add( i );
        return aKey.GetCount();
    }
    for( i=0; i<RecCount(); i++ )
    {
        if( !AllRecs )
            if( keycmp(i) != 0 )
                continue;
        arec_add(i);
    }
    return aKey.GetCount();
}
//--------------------- End of v_dbm1.cpp ---------------------------


