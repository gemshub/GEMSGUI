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
    auto still_len = database->putrec(*rh, f, time(nullptr));
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


//-----------------------------------------------------------------

DBFileList::~DBFileList()
{
    close_all();
}

void DBFileList::close_all()
{
    for(auto ndx: opened_files) {
        at(ndx)->Close();
    }
    opened_files.clear();
    opened_files_list.clear();
    fNum = 0;
}


//put state to cfg file
void DBFileList::to_cfg(std::fstream& out_stream)
{
    out_stream << special_files_num <<  "  ";
    out_stream << opened_files.size() << "  ";
    for(const auto& fl: opened_files) {
        out_stream << fl << " ";
    }
    out_stream << "  " << size() << std::endl;
    for(const auto& dbfile: *this) {
        dbfile->toCFG(out_stream);
    }
}

//get state from cfg file
void DBFileList::from_cfg(std::fstream& in_stream)
{
    size_t ii, tmp_size = 0;
    in_stream >> special_files_num;

    in_stream >> tmp_size;
    int file_ndx;
    for(ii=0; ii<tmp_size; ++ii) {
        in_stream >> file_ndx;
        opened_files.insert(file_ndx);
    }

    in_stream >> tmp_size;
    for(ii=0; ii<tmp_size; ++ii) {
        push_back(std::make_shared<TDBFile>(in_stream));
    }
}

// add new file to database chain
void DBFileList::add_file(const std::string& path)
{
    push_back(std::make_shared<TDBFile>(path));
}

void DBFileList::reset_open_list()
{
    fNum = 0;
    opened_files_list.clear();
    for(auto ndx: opened_files) {
        opened_files_list.push_back(at(ndx)->Name());
    }
    last_used_file = !opened_files.empty() ? *opened_files.rbegin() : 0;
}

// reload records list from settings
void DBFileList::reset_index_list(TDBKeyList& ind)
{
    ind.initnew(); // clear loaded
    for(auto ndx: opened_files) {
        if(at(ndx)->open_file(open_mode, rclose, ndx, ind)) {
            // warning logging need to rebuld;
        }
    }
    reset_open_list();
}

// open files in PDB
std::vector<int> DBFileList::open_files(FileStatus mode, const std::vector<int>& nff, TDBKeyList& ind)
{
    std::vector<int> list_to_rebuld;
    size_t current_ndx;

    open_mode = mode;
    if(size() == 0) {
        return list_to_rebuld;
    }

    if(opened_files.empty() || !nff.empty()) { //no config
        for(auto fndx: nff) {
            if(fndx<size()) {  // check_index
                opened_files.insert(fndx);
            }
        }
        if(opened_files.size()<1) {
            opened_files.insert(0);
        }
    }

    try {
        ind.initnew(); // clear old loaded
        for(auto ndx: opened_files) {
            current_ndx = ndx;
            at(ndx)->create_with_ndx(nRT, isDelete, ndx, ind);
            if(at(ndx)->open_file(open_mode, rclose, ndx, ind)) {
                list_to_rebuld.push_back(ndx);
            }
        }
        reset_open_list();
        return list_to_rebuld;
    }
    catch(TError& xcpt)  {
        if( xcpt.mess == "Two records with the same key (OpenFiles).") {
            xcpt.mess += "\n 2nd record in file: "+at(current_ndx)->Name();
            // left all before error
            opened_files.erase(opened_files.find(current_ndx), opened_files.end());
            reset_index_list(ind);
        }
        Error(xcpt.title, xcpt.mess);
    }
}

size_t DBFileList::open_new_file(const std::string &path, TDBKeyList& ind)
{
    auto ndx = size();

    try {
        add_file(path);
        at(ndx)->create_with_ndx(nRT, isDelete, ndx, ind);
        if(at(ndx)->open_file(open_mode, rclose, ndx, ind)) {
            // logging to rebuld;
        }
        opened_files.insert(ndx);
        reset_open_list();
    }
    catch(TError& xcpt)  {
        if(xcpt.mess == "Two records with the same key (AddFileToList).") {
            xcpt.mess += "\n 2nd record in file: "+at(ndx)->Name();
            reset_index_list(ind);
        }
        Error( xcpt.title, xcpt.mess);
    }
    return ndx;
}

// add open PDB files
std::vector<int> DBFileList::add_open_files(const std::vector<int>& nff, TDBKeyList& ind)
{
    auto files_to_open = nff;
    files_to_open.insert(files_to_open.end(), opened_files.begin(), opened_files.end());
    return open_files(open_mode, files_to_open, ind);
}

// delete a file from DBfile list
void DBFileList::remove_file(const std::string& path, TDBKeyList& ind)
{
    TDBFile fl(path);
    std::set<size_t> new_opened_files;

    for(size_t ii=0; ii<size(); ++ii)  {
        if(fl.Name() == at(ii)->Name()) {
            auto  nF = ii;

            if(check_readonly(nF)) {
                // logging read only file
                break;
            }
            for(auto& ndx: opened_files) {
                if(ndx>nF) {
                    new_opened_files.insert(ndx-1);
                }
                else if(ndx<nF) {
                    new_opened_files.insert(ndx);
                }
            }
            opened_files = new_opened_files;
            erase(begin()+nF);
            reset_index_list(ind);
            break;
        }
    }
}

// Selection in the list of full file names
void DBFileList::files_list(int mode, std::vector<std::string>& names,
                            std::vector<int>& indeces,
                            std::vector<int>& sel) const
{
    names.clear();
    indeces.clear();
    sel.clear();

    for(size_t ii=0; ii<size(); ++ii)  {
        bool isopened = check_open(ii);

        if( (!isopened&&(mode&closef)) || (isopened&&(mode&openf)) )  {
            //at(ii)->Makepath();
            names.push_back(std::string(at(ii)->GetKeywd())+std::string(" ")+at(ii)->GetPath());
            indeces.push_back(ii);
            if((mode&oldself) && isopened) { //select already open files
                sel.push_back(indeces.size()-1);
            }
        }
    }
}

// indexes all files in PDB chain
std::vector<int> DBFileList::all_files(bool only_kernel) const
{
    std::vector<int> file_ndxs;
    for(size_t ii=0; ii<size(); ++ii) {
        if(only_kernel && ii>=special_files_num) {
            break;
        }
        file_ndxs.push_back(ii);
    }
    return file_ndxs;
}

std::vector<int> DBFileList::files_with_name(const std::vector<std::string>& names) const
{
    std::vector<int> file_ndxs;
    for(size_t ii=0; ii<size(); ++ii) {
        for(const auto& name: names) {
            auto all_name =  std::string("."+name+".");
            if(at(ii)->GetPath().find(all_name) != std::string::npos) {
                file_ndxs.push_back(ii);
                break;
            }
        }
    }
    return file_ndxs;
}

std::vector<int> DBFileList::files_with_keywds(const std::vector<std::string>& keywds, bool& all_found) const
{
    size_t ii;
    all_found = true;
    std::vector<int> file_ndxs;

    for(const auto& keywd: keywds) {
        for(ii=0; ii<size(); ++ii) {
            if(keywd == at(ii)->GetKeywd()) {
                file_ndxs.push_back(ii);
                break;
            }
        }
        if(ii>=size()) {
            if(!vfQuestion(nullptr, keywd,
                            "This database file was not found in the project or default database.\n"
                            " Continue without this file (Y) or cancel(N)?" )) {
                Error(keywd, "Please, provide this database file and try opening the project again!");
            }
            all_found = false;
        }
    }

    return file_ndxs;
}

std::vector<int> DBFileList::opened_special_files() const
{
    std::vector<int> file_ndxs;
    for(auto& ndx: opened_files) {
        if(ndx<special_files_num) {
            file_ndxs.push_back(ndx);
        }
    }
    return file_ndxs;
}

size_t DBFileList::file_name_index(const std::string& file_name) const
{
    for(auto it = opened_files.rbegin(); it != opened_files.rend(); ++it) {
        size_t ndx = *it;
        if(at(ndx)->Name() == file_name) {
            return static_cast<int>(ndx);
        }
    }
    return 0; // in old implementation (posible throw error)
}

std::vector<std::string> DBFileList::file_keywds_according_name(const std::string& file_name_part) const
{
    std::vector<std::string> keywds;
    std::string all_name = "." +file_name_part +".";
    for(size_t ii=0; ii<size(); ++ii) {
        if(at(ii)->GetPath().find(all_name) != std::string::npos) {
            keywds.push_back(at(ii)->GetKeywd());
        }
    }
    return keywds;
}

bool DBFileList::check_special_open() const
{
    for(auto ndx: opened_files) {
        if(ndx<special_files_num) {
            return true;
        }
    }
    return false;
}

/**
 * @brief Determines the target database file index for adding a new record.
 *
 * If only one file is open, it is selected automatically. If multiple files are open,
 * it displays a graphical dialog (vfChoice/vfChoice2) prompting the user to choose
 * the destination file. Supports bulk processing via 'multy' and 'ok_to_all' flags.
 *
 * @param window     Pointer to the parent QWidget for displaying the choice dialog.
 * @param record_key The unique key of the record being added, used in the prompt message.
 * @param multy      Flag indicating bulk operations (enables the "Apply to all" option).
 * @param ok_to_all  Output reference flag that becomes true if the user chooses "Apply to all".
 *
 * @return int The valid file index (>= 0) if selected, or -1 if the operation was cancelled.
 */
int DBFileList::file_to_add_record(QWidget *window, const std::string &record_key, bool multy, bool& ok_to_all)
{
    // Guard clause: if no database files are open, immediately cancel the operation
    if (opened_files.empty()) {
        return -1;
    }

    int ndx = -1;
    int file = last_used_file;

    if (opened_files.size() == 1) {
        file = *opened_files.begin();
    }
    else {
        std::string mess = "Choose a database file for the record: " + record_key;
        int default_ndx = opened_files_list.size()-1;
        if(multy) {
            ndx = vfChoice2(window, opened_files_list, mess.c_str(), default_ndx, ok_to_all);
        }
        else {
            ndx = vfChoice(window, opened_files_list, mess.c_str(), default_ndx);
        }

        if(ndx >= 0 && static_cast<size_t>(ndx) < opened_files_list.size()) {
            file = file_name_index(opened_files_list[ndx]);
        }
        else {
            file = -1; // User pressed 'Cancel' or closed the dialog window
        }
    }
    return file;
}
