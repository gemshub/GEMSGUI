//-------------------------------------------------------------------
// $Id: v_dbm1.cpp 1032 2008-02-13 10:15:07Z gems $
//
// Implementation of TDBFile, TDBKey and TDBKeyList classes
//
// Copyright (C) 1996-2001 S.Dmytriyeva, D.Kulik
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


#include <cstdlib>
#include <cstdarg>

#include "GEMS3K/gdatastream.h"
#include "v_mod.h"
#include "v_dbm.h"
#include "v_dbfile.h"
#include "v_object.h"

int rlencomp( const void *ie_1, const void *ie_2 );

// Reading header of DB file
void
VDBhead::read(GemDataStream& is)
{
    is.readArray (VerP, 8 );
    is.readArray (PassWd, 8);
    is.readArray (Date, 11);
    is.readArray (Time, 5);
    is >> nRT;                 // type of PDB chain
    is >> nRec;                // number records in file
    is >> stacOver;
    is >> FPosTRT;
    is >> isDel;
    is >> MinDrLen;
    is >> MaxDrLen; // min and max size of deleted block
    is >> curDr;
    ;               // number of deleted blocks
}

// Writing header of DB file
void
VDBhead::write(GemDataStream& is)
{
    is.writeArray (VerP, 8 );
    is.writeArray (PassWd, 8);
    is.writeArray (Date, 11);
    is.writeArray (Time, 5);
    is << nRT;                 // type of PDB chain
    is << nRec;                // number records in file
    is << stacOver;
    is << FPosTRT;
    is << isDel;
    is << MinDrLen;
    is << MaxDrLen; // min and max size of deleted block
    is << curDr;
    ;               // number of deleted blocks
}


// read deleted block description
void
DBentry::read(GemDataStream& is)
{
    is >> pos;
    is >> len;
}

// write deleted block description
void
DBentry::write(GemDataStream& is)
{
    is << pos;
    is << len;
}

// read DBrecord description
void
RecEntry::read(GemDataStream& is)
{
    is >> pos;
    is >> len;
    is >> nFile;
}

// write DBrecord description
void
RecEntry::write(GemDataStream& is)
{
    is << pos;
    is << len;
    is << nFile;
}

// read DB record head
void
RecHead::read(GemDataStream& is)
{
    is.readArray (bgm, 2);
    is >> nRT;
    is >> Nobj;
    is >> rlen;
    is >> crt;
    is.readArray (endm, 2);
}

// write DB record head
void
RecHead::write(GemDataStream& is)
{
    is.writeArray (bgm, 2);
    is << nRT;
    is << Nobj;
    is << rlen;
    is << crt;
    is.writeArray (endm, 2);
}

// class TDBFile, TDBKey, TDBKeyList


//-------------------------------------------------------------
// TDBFile, files that contain DB records
//-------------------------------------------------------------

// default configurations
TDBFile::TDBFile(const std::string& fName,
                 const std::string& fExt, const std::string& fDir):
        TFile(fName, fExt, fDir),
        FPosFree(-1),
        dh(nullptr),
        sfe(nullptr)
{}


TDBFile::TDBFile(const std::string& path):
        TFile(path),
        FPosFree(-1),
        dh(nullptr),
        sfe(nullptr)
{
}

//load configurations from cfg file

TDBFile::TDBFile(fstream& file):
        TFile(file),
        FPosFree(-1),
        dh(nullptr),
        sfe(nullptr)
{}



void
TDBFile::check_sfe()
{
    ErrorIf( sfe == nullptr , GetKeywd(),
             "No memory for deleted records.");
}

void
TDBFile::check_dh()
{
    ErrorIf( dh == nullptr , GetKeywd(),
             "No memory for header of file.");
}

// Set date and time in header of file.
void
TDBFile::vdbh_setdt()
{
    check_dh();
// workaround DK 28.01.2012 on ubuntu 11.04: strcpy() caused crash
    memcpy(dh->Date, curDate().c_str(), curDate().length());
    dh->Date[curDate().length()]='\0';
    memcpy(dh->Time, curTime().c_str(), curTime().length());
    dh->Time[curTime().length()]='\0';
//    strcpy(dh->Date, curDate().c_str());
//    strcpy(dh->Time, curTime().c_str());
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
     const char *passwd, int nRT, bool ifDel )
{
    if( dh==nullptr )
        dh = new VDBhead;
    if( ifDel && sfe == nullptr )
        sfe = new DBentry[MAXFESTACK];
    clrh();
    if( VerP )
        strncpy( dh->VerP, VerP, 8 );
    if( passwd )
        strncpy( dh->PassWd, passwd, 8 );
    vdbh_setdt();
    dh->nRT = nRT;
    dh->FPosTRT = dh->stacOver = VDBhead::data_size();
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
    GemDataStream ff;

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
TDBFile::getHead(GemDataStream& fdb)
{
    check_dh();
    fdb.seekg( 0L, ios::beg );
    //  fdb.read( (char *)dh, sizeof(VDBhead) );
    dh->read (fdb);

    if( dh->isDel )
    {
        if( sfe == nullptr )
            sfe = new DBentry[MAXFESTACK];
        // fdb.read( (char *)sfe, MAXFESTACK*sizeof( DBentry ));
        for( int ii=0; ii<MAXFESTACK; ii++)
            sfe[ii].read (fdb);
    }
    ErrorIf( !fdb.good(), GetPath(), "PDB head read error.");
}

// Write the control structure of PDB file.
void
TDBFile::PutHead( GemDataStream& fdb, int deltRec )
{
    check_dh();

    dh->nRec += deltRec;
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
TDBFile::v_PDBtry( GemDataStream& fdb )
{
    if( dh==nullptr )
        dh = new VDBhead;
    getHead( fdb );
    // check version
    //  if( strncmp( dh->VerP, TFile::VV(), 8 ))
    //      Error( GetPath(), "Invalid version in PDB file.");
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
    dh = nullptr;
    sfe = nullptr;
    FPosFree = 0;
    TFile::Close();
}

// compare deleted blocs
int rlencomp( const void *ie_1, const void *ie_2 )
{
    const DBentry* ie1 = static_cast<const DBentry *>(ie_1);
    const DBentry* ie2 = static_cast<const DBentry *>(ie_2);
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
        //changed 30/10/2002
        //ErrorIf( Ndr>=MAXFESTACK , GetPath(),
        //         "Stack of deleted records overflow.");
        if( Ndr>=MAXFESTACK )
        {
          if( dh->stacOver >= 0 )
          {
            dh->stacOver = -1;
           // vfMessage(0, GetPath(),"Stack of deleted records overflow." );
          }
        }
        else
        { sfe[Ndr].len = re.len;
          sfe[Ndr].pos = re.pos;
          dh->curDr++;
          if( Ndr )   // sort sfe list
              qsort( sfe, Ndr+1, sizeof(DBentry), rlencomp );
          dh->MinDrLen = sfe[0].len;
          dh->MaxDrLen = sfe[Ndr].len;
        }
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

//Find deleted block in sfe list for puttig record here
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
TDBFile::GetDh( int& fPos, int& fLen, int& nRT, char& isDel )
{
    check_dh();
    fPos = dh->FPosTRT;
    fLen = FPosFree;
    nRT = dh->nRT;
    isDel = dh->isDel;
}

//get information from dh
bool
TDBFile::GetDhOver()
{
    return (dh->stacOver < 0);
}

// sets information into compressed PDB file header
// Fixed 07.09.04
void
TDBFile::SetDh( int fLen, int nRec, int nRT, char isDel )
{
    if( dh==nullptr )
        dh = new VDBhead;
    if( isDel && sfe == nullptr )
        sfe = new DBentry[MAXFESTACK];
    clrh();
    strncpy( dh->VerP, TFile::VV(), 8 );
    strncpy( dh->PassWd, TFile::PA(), 8 );
    vdbh_setdt();
    dh->nRT = nRT;
    dh->FPosTRT = dh->stacOver = VDBhead::data_size();
    dh->isDel = isDel;
    if( isDel )
    {
        dh->FPosTRT += DBentry::data_size() * MAXFESTACK;
        check_sfe();
        memset( reinterpret_cast<char*>(sfe), 0, DBentry::data_size() * MAXFESTACK );
    }
    dh->nRec = nRec;
    dh->curDr = 0;
    dh->MinDrLen = 0;
    dh->MaxDrLen = 0;
    FPosFree = fLen;
    PutHead( f );
/*
#ifndef __unix
    int handle = f.rdbuf()->fd();
    chsize( handle, fLen );
#else
#if __GNUC__ != 3
    int handle = f.rdbuf()->fd();
#else
cerr << "trunc dummy" << endl;
//  WE HAVE TO GET HANDLE SOMEHOW!!!
    int handle = 0;
#endif // __GCC__ != 3
    ftruncate( handle, fLen );
#endif
*/
    f.flush();
}

//-------------------------------------------------------------
// TDBKey  - working with DB record key
//-------------------------------------------------------------

// default configuration
TDBKey::TDBKey( unsigned char nRkflds, const unsigned char* rkfrm ):
        rkFlds( nRkflds )
{
    ErrorIf( rkFlds<=0 || rkFlds>MAXRKFRMSTR,
             "TDBKey", "Error: Invalid number of fields in record key (1).");
    rkLen = new unsigned char[rkFlds];
    rkInd = new unsigned char[rkFlds];
    int i;
    for( fullLen=0, i=0; i<rkFlds; i++)
    {
        rkLen[i] = rkfrm[i];
        if( i==0 )
            rkInd[i]=0;
        else
            rkInd[i] = (rkInd[i-1] + rkLen[i-1]);
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
    int ii;
    // SD 23/12/2009 unsigned char tmp;
    unsigned int tmp;

    in_stream >> tmp;
    rkFlds = tmp;
    ErrorIf( rkFlds<=0 || rkFlds > MAXRKFRMSTR,
             "TDBKey", "Error: Invalid number of fields in record key (2).");
    rkLen = new unsigned char[rkFlds];
    rkInd = new unsigned char[rkFlds];
//    f.read( (char*)rkLen, rkFlds*sizeof(unsigned char) );
   for( ii=0; ii<rkFlds; ii++)
   {
     in_stream >> tmp;
     rkLen[ii] = tmp;
   }

   ErrorIf( !in_stream.good(), "TDBKey",
             "Error reading TDBKey from configurator.");
   for( fullLen=0, ii=0; ii<rkFlds; ii++)
   {
        if( ii == 0 )
            rkInd[ii] = 0;
        else
            rkInd[ii] = (rkInd[ii-1]+rkLen[ii-1]);
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
    ErrorIf( pKey==nullptr || uKey==nullptr || rkLen==nullptr || rkInd==nullptr,
             "TDBkey", "No memory for key struct.");
}


// Putting DB record key key to internal structure
// in pack and unpack forms
void
TDBKey::SetKey( const char *key )
{
    ErrorIf( key==nullptr,"TDBKey", "No key buf.");

    //vvstr sp(fullLen+rkFlds+1);
    std::string sp;
    if( strncmp( key, ALLKEY, fullLen )==0 )
    {
        string st;
        for(int i=0; i<rkFlds; i++)
            st += "*:";
        sp = std::string( st, 0, fullLen);
    }
    else
        sp = std::string( key, 0, fullLen);

    if( strchr( sp.c_str(), ':' ))  // key in packed form
        unpack( sp.c_str() );
    else
        pack( sp.c_str() );
 }

// Change i-th field of DBkey to key
void
TDBKey::SetFldKey( uint i, const char *key )
{
    ErrorIf( i>= rkFlds, key, "Invalid key field number");

    memset( uKey+rkInd[i], ' ' , rkLen[i] );
    strncpy( uKey+rkInd[i], key, min<int>(rkLen[i], strlen(key) ) );
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
                    ErrorIf( ln==0, key, "Invalid packed record key.");
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
TDBKey::unpack( const char *key )
{
    int i;
    size_t ln;
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
                    ErrorIf( ln==0, key, "Invalid packed record key.");
                }
                else
                { // end '*'
                    ln = 1;
                    strncpy( uKey+rkInd[i], sp, ln );
                    break;
                }
            }
            else  ln = ( sp1- sp);
            ln = min<size_t>( ln, rkLen[i] );
            strncpy( uKey+rkInd[i], sp, ln );
            if( *(sp+ln) == ':' )
                  ln++;
            else
              if( *(sp+ln+1) == ':' )
                  ln+=2;
            sp += ln;
        }
        uKey[fullLen]= '\0';
        //// strncpy( key, uKey, fullLen );
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
TDBKey::pack( const char *key )
{
    uint i;
    int len, ln;
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
       //// strncpy( key, pKey, fullLen );
    }
}

// make packed key to seach.
void
TDataBase::MakeKey( unsigned char nRTwrk, char *pkey, ... )
{
    va_list Marker;
    unsigned char rkflen;
    uint i, nkf;
    int rts;
    char /**sp,*/ *imf;

    ErrorIf( pkey==nullptr,"MakeKey", "No key buf to make key.");
    *pkey = 0;
    va_start( Marker, pkey );
    for( i=0; i<ind.KeyNumFlds(); i++ )
    {
        //sp= pkey+strlen(pkey);
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
        case K_IMM:  // field in string
            imf = va_arg( Marker, char * );
            strncat( pkey, imf, min( strlen(imf), static_cast<size_t>(rkflen) ));
            break;
        case K_ACT:  // get field from  PRIE request
            rts = nRTwrk;
            [[fallthrough]];
        default:     // get field from enathe chain key
            nkf = va_arg( Marker, uint );
            if( !*rt[rts]->FldKey( nkf ))
                strcat( pkey, S_ANY );
            else
            {
                string str=  string( rt[rts]->FldKey( nkf ), 0,
                                       min( rt[rts]->FldLen(nkf), rkflen ));
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
// TDBKeyList - working with DB keys list
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
    ErrorIf( ndx[0]==nullptr || re==nullptr ,"TDBMKeyList",
             "No memory for list of indexes.");
}

void
TDBKeyList::check_i(uint i)
{
    if( i>recInDB )
        Error( "TDBMKeyList","Invalid record index in the chain.");
}

//initialization of DB keys list state
void
TDBKeyList::init()
{
    nI = -1;
    recInDB = 0;
    nBuf = IND_PLUS;
    size_t k;

    for(uint i=0; i<KeyNumFlds(); i++)
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
    for(uint j=0; j<KeyNumFlds(); j++)
        memset( ndx[j], 0, FldLen(j)*nBuf*sizeof(char) );
    memset( re, 0, nBuf*sizeof(RecEntry) );
    aKey.clear();
    anR.clear();
}

// delete the keys of records file nF
void
TDBKeyList::delfile( uint nF )
{
    uint i=0;
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

RecEntry*
TDBKeyList::RecPosit(uint i)
{
    return &re[i];
}

// compare key i record and work key
int
TDBKeyList::keycom( uint i )
{
    int l = 0;
    for( uint j=0; j<KeyNumFlds(); j++)
    {
        l = memcmp( FldKey(j), RecKeyFld( i,j ), FldLen(j) );
        if( l )
           return l;
    }
    return l;
}

//Add new key of record in list. Realloc memory if nessasery.
uint
TDBKeyList::addndx( uint nF, int len, const char *key )
{
    int i, l;
    uint j;

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
    ErrorIf( l==0, key, "Two records with the same key (AddNewKey).");
    uint ii=i;
    for( j=0; j<KeyNumFlds(); j++)
        memmove( RecKeyFld(ii+1,j), RecKeyFld(ii,j),
                 FldLen(j)*(recInDB-ii)*sizeof(char) );
    memmove( re+ii+1, re+ii, (recInDB-ii)*sizeof(RecEntry) );
    for( j=0; j<KeyNumFlds(); j++)
        memcpy( RecKeyFld(ii,j), FldKey(j), FldLen(j) );
    re[i].len = len;
    re[i].nFile = nF;   //warning
    nI = i;
    recInDB++;
    return ii;
}

// Delete i-th record from list
void
TDBKeyList::delndx( uint i )
{
    uint j;

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
    int l;

    SetKey( key );
    if( recInDB <= 0 ) return -1;
    for(uint i=0; i< recInDB; i++ )
    {
        l = keycom(i);
        if( l < 0 ) // No record
            break;
        if( l == 0 )
        {
            nI = i;
            return nI;
        }
    }
    return -1;
}

//Set key i - record to work key structure
void
TDBKeyList::PutKey( uint i)
{
    uint j;
    check_i(i);
    TDBKey::check();
    for( j=0; j<KeyNumFlds(); j++)
        memcpy( FldKey(j), RecKeyFld(i,j), FldLen(j) );
    SetKey( FldKey(0) );
}

//Put key i record to kbuf in unpack form.
void
TDBKeyList::RecKey(uint i, string& kbuf )
{
    uint j;
    check_i(i);
    kbuf = "";
    for( j=0; j<KeyNumFlds(); j++)
        kbuf += string( RecKeyFld(i,j), 0, FldLen(j) );
}

// write the keys of records to ndx file
void
TDBKeyList::PutKeyList( uint nF, GemDataStream& f)
{
    unsigned char j;
    for(uint i=0; i< recInDB; i++ )
        if( re[i].nFile == nF )
        {
            for( j=0; j<KeyNumFlds(); j++)
                f.writeArray(  RecKeyFld(i,j), FldLen(j) );
            //        f.write( (char *)(re+i), sizeof(RecEntry) );
            re[i].write (f);
        }
    ErrorIf( !f.good(), "TDBKeyList",  "Index file write error.");
}

// read the keys of records from ndx file
void
TDBKeyList::GetKeyList_i(uint nF, int nRec, GemDataStream& f)
{
    RecEntry re_;
    //vvstr key(KeyLen()+1);
    char key[MAXRKEYLEN+10];
    memset( key, 0, KeyLen()+1);
    for( int i=0; i< nRec; i++ )
    {
        f.readArray( key, KeyLen());
        //     f.read( (char *)&re_, sizeof(RecEntry));
        re_.read (f);
        auto indRec = addndx( nF, re_.len, key);
        re[indRec].pos = re_.pos;
    }
    SetKey(ALLKEY); // bugfix: current key for not readed record
    ErrorIf( !f.good(), "TDBKeyList", "Index file read error.");
}

// compare key ni record and pattern in work key
uint
TDBKeyList::keycmp( uint ni )
{
    if( !(strchr(UnpackKey(),'*') || strchr(UnpackKey(),'?')))
        return  keycom(ni);      // no pattern

    for( uint j=0; j<KeyNumFlds(); j++)
    {
        char *sp = FldKey(j);
        if( *(sp) == '*' )
	    continue; //to next field

        for( uint i=0; i<FldLen(j); i++ )
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
TDBKeyList::arec_add( uint ni )
{
    string s;
    RecKey( ni, s);
    aKey.push_back( s );
    anR.push_back( ni );
}

//get key list for a wildcard search
size_t
TDBKeyList::xlist( const char *pattern )
{
    bool OneRec, AllRecs;
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
    aKey.clear();
    anR.clear();
    if( OneRec )
    {
       auto i = findx( pattern );
        if( i >= 0 )
            arec_add( i );
        return aKey.size();
    }
    for(uint ii=0; ii<RecCount(); ii++ )
    {
        if( !AllRecs )
            if( keycmp(ii) != 0 )
                continue;
        arec_add(ii);
    }
    return aKey.size();
}

//--------------------- End of v_dbm1.cpp ---------------------------
