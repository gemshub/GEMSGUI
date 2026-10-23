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
#include <ctime>
#include "v_dbfile.h"
#include "v_user.h"
#include "v_dbm.h"
#include "service.h"
#include "GEMS3K/gdatastream.h"

static int rlencomp( const void *ie_1, const void *ie_2 );

// Reading header of DB file
void VDBhead::read(GemDataStream& is)
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
void VDBhead::write(GemDataStream& is)
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
void DBentry::read(GemDataStream& is)
{
    is >> pos;
    is >> len;
}

// write deleted block description
void DBentry::write(GemDataStream& is)
{
    is << pos;
    is << len;
}

// read DBrecord description
void RecEntry::read(GemDataStream& is)
{
    is >> pos;
    is >> len;
    is >> nFile;
}

// write DBrecord description
void RecEntry::write(GemDataStream& is)
{
    is << pos;
    is << len;
    is << nFile;
}

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

TDBFile::TDBFile(std::fstream& file):
    TFile(file),
    FPosFree(-1),
    dh(nullptr),
    sfe(nullptr)
{}

void TDBFile::check_sfe()
{
    ErrorIf( sfe == nullptr , GetKeywd(),
            "No memory for deleted records.");
}

void TDBFile::check_dh()
{
    ErrorIf( dh == nullptr , GetKeywd(),
            "No memory for header of file.");
}

// Set date and time in header of file.
void TDBFile::vdbh_setdt()
{
    check_dh();
    strncpy(dh->Date, curDate().c_str(), 11);
    strncpy(dh->Time, curTime().c_str(), 5);
}

//Clear the header of PDB file.
void TDBFile::clrh()
{
    check_dh();
    FPosFree = -1;
    if( sfe )
        memset( sfe, 0, sizeof(DBentry)*MAXFESTACK );
    memset( dh, 0, sizeof( VDBhead ));
}

//Set header of PDB file
void TDBFile::vdbh_new( const char *VerP,
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
void TDBFile::Create(int nRT, bool isDel )
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
void TDBFile::getHead(GemDataStream& fdb)
{
    check_dh();
    fdb.seekg( 0L, std::ios::beg );
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
void TDBFile::PutHead( GemDataStream& fdb, int deltRec )
{
    check_dh();

    dh->nRec += deltRec;
    vdbh_setdt();

    //fdb.flush();
    fdb.seekg(0L, std::ios::beg );
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
void TDBFile::v_PDBtry( GemDataStream& fdb )
{
    if( dh==nullptr )
        dh = new VDBhead;
    getHead( fdb );
    // check version
    //  if( strncmp( dh->VerP, TFile::VV(), 8 ))
    //      Error( GetPath(), "Invalid version in PDB file.");
    fdb.seekg(0l, std::ios::end);
    FPosFree = fdb.tellg();
}

// Open PDB file in special mode
void TDBFile::Open( FileStatus mode )
{
    if( Test() == mode )
        return;
    if( Test() != NOT_OPEN )
        Close();
    TFile::Open( mode );
    v_PDBtry( f );
}

//Close PDB file
void TDBFile::Close()
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
void TDBFile::AddSfe( RecEntry& re )
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
    f.seekg(re.pos, std::ios::beg );
    //   f.read( (char *)&rh, sizeof(RecHead) );
    rh.read (f);
    if( !strncmp( rh.bgm, MARKRECHEAD, 2 ))
    {
        strncpy( rh.bgm, MARKRECDEL, 2 );
        strncpy( rh.endm, MARKRECDEL, 2 );
    }
    f.seekg(re.pos, std::ios::beg );
    //   f.write( (char *)&rh, sizeof(RecHead) );
    rh.write (f);
    ErrorIf( !f.good(), GetPath(), "PDB file write error.");
}

//Find deleted block in sfe list for puttig record here
void TDBFile::FindSfe( RecEntry& re )
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
void TDBFile::GetDh( int& fPos, int& fLen, int& nRT, char& isDel )
{
    check_dh();
    fPos = dh->FPosTRT;
    fLen = FPosFree;
    nRT = dh->nRT;
    isDel = dh->isDel;
}

//get information from dh
bool TDBFile::GetDhOver()
{
    return (dh->stacOver < 0);
}

// sets information into compressed PDB file header
// Fixed 07.09.04
void TDBFile::SetDh( int fLen, int nRec, int nRT, char isDel )
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
c err << "trunc dummy" << endl;
//  WE HAVE TO GET HANDLE SOMEHOW!!!
    int handle = 0;
#endif // __GCC__ != 3
    ftruncate( handle, fLen );
#endif
*/
    f.flush();
}

//----------------------------------------------------------------

std::string TDBFile::index_file_name() const
{
    return u_makepath(Dir(), Name(), NDX_EXT );
}

// put information to index file
void TDBFile::put_ndx(size_t nF, const TDBKeyList& ind)
{
    GemDataStream ndx_f;
    auto path = index_file_name();
    ndx_f.open(path.c_str(), WRITE_B);
    ErrorIf(!ndx_f.good(), path, "Index file open error.");
    vdbh_setdt();
    Dh()->write(ndx_f);
    ind.PutKeyList(nF, ndx_f);
}

// get information from index file
void TDBFile::get_ndx(size_t nF, TDBKeyList& ind)
{
    VDBhead dh;
    GemDataStream ndx_f;
    auto path = index_file_name();
    ndx_f.open(path.c_str(), RDONLY_B);
    ErrorIf(!ndx_f.good(), path,  "Index file open error");
    dh.read(ndx_f);
    ind.GetKeyList_i(nF, dh.nRec, ndx_f, path);
}


void TDBFile::create_with_ndx(int nRT, bool isDel, size_t nF, const TDBKeyList& ind)
{
    if(!Exist()) {
        Create(nRT, isDel);
        put_ndx(nF, ind);
    }

}

bool TDBFile::open_file(FileStatus mode, bool rclose, size_t nF, TDBKeyList& ind)
{
    bool need_rebuilding = false;
    Open(mode);
    if(GetDhOver()) {
        if(vfQuestion(nullptr, GetPath(), "Stack of deleted records overflow.\nCompress?" )) {
            need_rebuilding = true;
        }
    }
    get_ndx(nF, ind);
    if(rclose) {
        Close();
    }
    return need_rebuilding;
}

//Read record update time from PDB file to memory.
time_t TDBFile::get_time(bool rclose, RecEntry* ree)
{
    RecHead rh;
    Open(UPDATE_DBV);
    f.seekg(ree->pos, std::ios::beg);
    rh.read(f);
    if(strncmp(rh.bgm, MARKRECHEAD, 2) ||
        strncmp(rh.endm, MARKRECHEAD, 2)) {
        Error(GetKeywd(), "PDB record header format error");
    }
    if(rclose) {
        Close();
    }
    return rh.crt;
}

void TDBFile::save_record_to_file(bool new_record, bool rclose, RecEntry* rh, TDataBase* database)
{
    Open(UPDATE_DBV);

    if(!new_record) {  // add deleted block
        rh->len += RecHead::data_size();
        AddSfe(*rh);
    }

    auto new_record_len = database->reclen();
    new_record_len += strlen(database->ind.PackKey())+1;
    new_record_len += RecHead::data_size();
    rh->len = new_record_len;

    // find new block
    FindSfe(*rh);
    rh->len -= RecHead::data_size();
    auto old_flen = FLen();
    auto still_len = database->putrec(*rh , f);
    if(still_len != 0) {
        if(FLen() != old_flen) {
            FNewLen(still_len);
        }
        rh->len -=still_len;
    }

    PutHead(f, (new_record ? 1 : 0));
    put_ndx(rh->nFile, database->ind);
    if(rclose) {
        Close();
    }
}

//delete i-th record from database
void TDBFile::delete_record_from_file(bool rclose, RecEntry* rh, TDataBase* database)
{
    Open( UPDATE_DBV );
    rh->len += RecHead::data_size();
    AddSfe(*rh);
    PutHead(f, -1);
    put_ndx(rh->nFile, database->ind);
    if(rclose) {
        Close();
    }
}

//Read i-th record from PDB file to memory.
void TDBFile::read_record_from_file(bool rclose, RecEntry* rh, TDataBase* database)
{
    RecHead rhh;
    Open(UPDATE_DBV);
    database->getrec(*rh, f, rhh);
    if(rclose) {
        Close();
    }
}

void TDBFile::rebuild_file(size_t nF, TDataBase* database)
{
    int  nRec, nRT1, fPos, fLen;
    char isDel;

    Open(UPDATE_DBV);
    GetDh(fPos, fLen, nRT1, isDel);

    std::string tmp_file_name = GetPath()+".tmp";
    GemDataStream out_stream(tmp_file_name, std::ios::out | std::ios::binary);
    for(int ii=0; ii<fPos; ++ii) {
            out_stream.put(0);
    }
    out_stream.seekp(fPos/*VDBhead::data_size()*/, std::ios::beg);
    nRec = database->scanfile(nF, fPos, fLen, f, out_stream);
    out_stream.close();

    OpenFromFileName(tmp_file_name, UPDATE_DBV);
    SetDh(fLen, nRec, nRT1, isDel);
    put_ndx(nF, database->ind);
    Close();
}


