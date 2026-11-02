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

#include <filesystem>
namespace fs = std::filesystem;

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

TDBFile::TDBFile(const std::string& path, const std::string& the_relative):
    TBinaryFile(path),
    FPosFree(-1),
    dh(nullptr),
    sfe(nullptr)
{
    if(!the_relative.empty()) {
        relative_path = the_relative;
    }
}

void TDBFile::check_sfe()
{
    ErrorIf(sfe == nullptr, relativePath(),
            "Critical error: Failed to allocate memory for the deleted records structure.");
}

void TDBFile::check_dh()
{
    ErrorIf(dh == nullptr, relativePath(),
            "Critical error: Failed to allocate memory for the database file header.");
}

// Set date and time in header of file.
void TDBFile::vdbh_setdt()
{
    strncpy(dh->Date, curDate().c_str(), 11);
    strncpy(dh->Time, curTime().c_str(), 5);
}

//Clear the header of PDB file.
void TDBFile::clear_dh_sfe()
{
    FPosFree = -1;
    if(sfe) {
        std::fill_n(sfe, MAXFESTACK, DBentry{});
    }
    *dh = VDBhead{};
}

/**
 * @brief Initializes the database file header and deleted records stack with starting metadata.
 *
 * Allocates necessary memory, resets counters to zero, sets up version/password safety,
 * and calculates internal file structure offsets based on whether the deleted records stack is enabled.
 *
 * @param version Pointer to an 8-character version string.
 * @param passwd  Pointer to an 8-character password string.
 * @param nRT     Record type.
 * @param if_del  Flag to enable or disable the deleted records tracking system (sfe).
 */
void TDBFile::initialize_header(int nRT, bool if_del)
{
    if(dh == nullptr) {
        dh = new VDBhead;
    }
    if(if_del) {
        if (sfe == nullptr) {
            sfe = new DBentry[MAXFESTACK];
        }
    }
    clear_dh_sfe();

    std::strncpy(dh->VerP, TBinaryFile::VV(), 8);
    dh->VerP[7] = '\0';
    std::strncpy(dh->PassWd, TBinaryFile::PA(), 8);
    dh->PassWd[7] = '\0';

    vdbh_setdt();
    dh->nRT = nRT;
    dh->isDel = if_del;
    dh->FPosTRT = VDBhead::data_size();
    dh->stacOver = VDBhead::data_size();
    if (if_del) {
        dh->FPosTRT += DBentry::data_size() * MAXFESTACK;
    }
    dh->nRec = 0;
    dh->curDr = 0;
    dh->MinDrLen = 0;
    dh->MaxDrLen = 0;
}

/**
 * @brief Forcefully resets the file header and deleted blocks stack, saves it to disk, and updates the file size.
 *
 * This method synchronizes metadata and uses std::filesystem to cross-platform truncate or expand
 * the physical file on the disk to match fLen.
 *
 * @param fLen   The target physical length of the database file on disk.
 * @param nRec   The exact number of records currently stored in the database.
 * @param nRT    Record type.
 * @param is_del Flag to enable or disable the deleted records tracking system (sfe).
 */
void TDBFile::force_update_header(int fLen, int nRec, int nRT, char is_del)
{
    initialize_header(nRT, is_del);
    dh->nRec = nRec;
    FPosFree = fLen;
    put_head(0);

    try {
        // resize file
        std::filesystem::path file_path(fullPath());
        if (std::filesystem::exists(file_path)) {
            std::filesystem::resize_file(file_path, static_cast<uint64_t>(fLen));
        }
    }
    catch (const std::filesystem::filesystem_error& e) {
        Error(relativePath(), "Failed to resize database file physically: " + std::string(e.what()));
    }
}


// Add a new deleted record to the sfe list and mark it as deleted in the file
void TDBFile::add_sfe(RecEntry& re)
{
    RecHead rh;

    check_dh();
    int Ndr = dh->curDr;

    if(dh->isDel) {
        check_sfe();

        if(Ndr >= MAXFESTACK) {
            if(dh->stacOver >= 0) {
                dh->stacOver = -1;
                // vfMessage(0, GetPath(), "Stack of deleted records overflow.");
            }
        }
        else {
            sfe[Ndr].len = re.len;
            sfe[Ndr].pos = re.pos;
            dh->curDr++;

            if(Ndr > 0) {
                std::sort(sfe, sfe + Ndr + 1, [](const DBentry& a, const DBentry& b) {
                    return a.len < b.len;
                });
            }
            dh->MinDrLen = sfe[0].len;
            dh->MaxDrLen = sfe[Ndr].len;
        }
    }
    fstream->seekg(re.pos, std::ios::beg);
    rh.read(*fstream);
    if (std::strncmp(rh.bgm, MARKRECHEAD, 2) == 0) {
        std::strncpy(rh.bgm, MARKRECDEL, 2);
        std::strncpy(rh.endm, MARKRECDEL, 2);
    }
    fstream->seekp(re.pos, std::ios::beg);
    rh.write(*fstream);
    ErrorIf(!fstream->good(), relativePath(), "Database file write error (failed to mark record as deleted).");
}

void TDBFile::allocate_new_block(RecEntry& re)
{
    re.pos = FPosFree;
    FPosFree += re.len;
}

// Find a deleted block in the sfe list to put the record there
void TDBFile::find_sfe(RecEntry& re)
{
    check_dh();
    int Ndr = dh->curDr;

    if(!dh->isDel || Ndr == 0 || dh->MaxDrLen < re.len) {
        allocate_new_block(re);
        return;
    }

    auto* start = sfe;
    auto* end = sfe + Ndr;
    auto* it = std::lower_bound(start, end, re.len, [](const DBentry& block, int target_len) {
        return block.len < target_len;
    });
    if(it == end) {
        allocate_new_block(re);
        return;
    }

    int j = static_cast<int>(it - start);
    re.pos = sfe[j].pos;
    RecEntry rn;
    rn.pos = re.pos + re.len;
    rn.len = sfe[j].len - re.len;
    rn.nFile = re.nFile;
    int items_to_move = Ndr - 1 - j;
    if (items_to_move > 0) {
        std::memmove(sfe + j, sfe + j + 1, items_to_move * sizeof(DBentry));
    }
    dh->curDr--;
    sfe[dh->curDr] = DBentry{};
    if(rn.len >= dh->MinDrLen) {
        add_sfe(rn);
    }
}

// Read the control structure of the database file (file header and deleted records stack)
void TDBFile::get_head()
{
    if (dh == nullptr) {
        dh = new VDBhead;
    } else {
        *dh = VDBhead{};
    }
    fstream->seekg(0L, std::ios::beg);
    dh->read(*fstream);

    if(dh->isDel) {
        if(sfe == nullptr) {
            sfe = new DBentry[MAXFESTACK];
        }
        for(int ii=0; ii<MAXFESTACK; ++ii) {
            sfe[ii].read(*fstream);
        }
    }

    ErrorIf(!fstream->good(), relativePath(),
            "Database file read error: Failed to restore control structures or file header.");

    // --- Check db version  ---
    // if (std::strncmp(dh->VerP, TFile::VV(), 8) != 0) {
    //     Error(GetPath(), "Invalid database file version. Expected version: " + std::string(TFile::VV(), 8));
    // }

    fstream->seekg(0L, std::ios::end);
    FPosFree = fstream->tellg();
}

// Write the control structure of PDB file.
void TDBFile::put_head(int delta_rec)
{
    check_dh();

    dh->nRec += delta_rec;
    vdbh_setdt();
    //fstream->flush();
    fstream->seekp(0L, std::ios::beg);
    dh->write(*fstream);
    if(dh->isDel) {
        check_sfe();
        for(int ii = 0; ii < MAXFESTACK; ii++) {
            sfe[ii].write(*fstream);
        }
    }
    ErrorIf(!fstream->good(), relativePath(),
            "Database file write error. Please check file permissions or disk space.");
    fstream->flush();
}

//--------------------------------------------------------------

void TDBFile::createFile(int nRT, bool is_del)
{
    GemDataStream ff;
    initialize_header(nRT, is_del);
    FPosFree = dh->FPosTRT;
    ff.open(fullPath().c_str(), WRITE_B);
    dh->write(ff);
    if (dh->isDel) {
        for (int ii = 0; ii < MAXFESTACK; ii++) {
            sfe[ii].write(ff);
        }
    }
    ErrorIf(!ff.good(), relativePath(), "Database file creation error. Failed to initialize physical file on disk.");
    ff.close();
}

// Open PDB file in special mode
void TDBFile::openFile(FileStatus ios_mode)
{
    if(ios_mode == status) {
        return;
    }
    if(status != NOT_OPEN) {
        closeFile();
    }
    if(ios_mode == NOT_OPEN) {
        return;
    }
    TBinaryFile::openFile( ios_mode );
    get_head();
}

//Close PDB file
void TDBFile::closeFile()
{
    delete dh;
    delete[] sfe;
    dh = nullptr;
    sfe = nullptr;
    FPosFree = 0;
    TBinaryFile::closeFile();
}

std::string TDBFile::index_file_name() const
{
    std::filesystem::path p(full_path);
    p.replace_extension(NDX_EXT);
    return p.string();
}

// put information to index file
void TDBFile::put_ndx(size_t nF, const TDBKeyList& ind)
{
    GemDataStream ndx_f;
    auto path = index_file_name();
    ndx_f.open(path.c_str(), WRITE_B);
    ErrorIf(!ndx_f.good(), path,
            "Index file write error: Failed to open or create the index file for writing.");
    check_dh();
    vdbh_setdt();
    dh->write(ndx_f);
    ind.PutKeyList(nF, ndx_f);
}

// get information from index file
void TDBFile::get_ndx(size_t nF, TDBKeyList& ind)
{
    VDBhead dh;
    GemDataStream ndx_f;
    auto path = index_file_name();
    ndx_f.open(path.c_str(), RDONLY_B);
    ErrorIf(!ndx_f.good(), path,
            "Index file read error: Failed to open the index file. File may be missing or corrupted.");
    dh.read(ndx_f);
    ind.GetKeyList_i(nF, dh.nRec, ndx_f, path);
}

void TDBFile::create_with_ndx(int nRT, bool isDel, size_t nF, const TDBKeyList& ind)
{
    if(!exist()) {
        createFile(nRT, isDel);
        put_ndx(nF, ind);
    }

}

bool TDBFile::open_file(FileStatus mode, bool is_close, size_t nF, TDBKeyList& ind)
{
    connect_db_mode = mode;
    rclose = is_close;
    bool need_rebuilding = false;

    openFile(mode);

    try {
        check_dh();
        if (dh->stacOver < 0) {
            std::string prompt = "Stack of deleted records has overflowed.\nWould you like to compress the database?";
            if (vfQuestion(nullptr, relativePath(), prompt.c_str())) {
                need_rebuilding = true;
            }
        }
        get_ndx(nF, ind);
    }
    catch (...) {
        if (rclose) {
            closeFile();
        }
        throw;
    }

    if (rclose) {
        closeFile();
    }
    return need_rebuilding;
}

time_t TDBFile::get_time(RecEntry* ree)
{
    if (status == NOT_OPEN) {
        openFile(connect_db_mode);
    }

    RecHead rh;
    try {
        fstream->seekg(ree->pos, std::ios::beg);
        rh.read(*fstream);

        if (std::strncmp(rh.bgm, MARKRECHEAD, 2) != 0 ||
            std::strncmp(rh.endm, MARKRECHEAD, 2) != 0) {
            Error(relativePath(), "Database record header format error. Integrity check failed.");
        }
    }
    catch (...) {
        if (rclose) {
            closeFile();
        }
        throw;
    }

    if (rclose) {
        closeFile();
    }

    // creation/modification time
    return rh.crt;
}

void TDBFile::save_record_to_file(bool new_record, RecEntry* rh, TDataBase* database)
{
    if (rh == nullptr || database == nullptr) {
        Error(relativePath(), "Failed to save record: Invalid RecEntry or TDataBase pointer.");
        return;
    }

    if (status == NOT_OPEN) {
        openFile(connect_db_mode);
    }

    try {
        if (!new_record) {
            RecEntry old_block = *rh;
            old_block.len += RecHead::data_size();
            add_sfe(old_block);
        }

        size_t key_len = std::strlen(database->ind.PackKey()) + 1;
        size_t payload_len = database->reclen();
        size_t total_required_len = payload_len + key_len + RecHead::data_size();

        rh->len = total_required_len;
        find_sfe(*rh);
        rh->len -= RecHead::data_size();
        long int old_flen = FPosFree;
        time_t current_time = std::time(nullptr);

        long int still_len = database->putrec(*rh, *fstream, current_time);
        if (still_len != 0) {
            if (FPosFree != old_flen) {
                FPosFree -= still_len;
            }
            rh->len -= still_len;
        }

        put_head(new_record ? 1 : 0);
        put_ndx(rh->nFile, database->ind);
    }
    catch (...) {
        if (rclose) {
            closeFile();
        }
        throw;
    }

    if (rclose) {
        closeFile();
    }
}

void TDBFile::delete_record_from_file(RecEntry* rh, TDataBase* database)
{
    if (status == NOT_OPEN) {
        openFile(connect_db_mode);
    }

    try {
        rh->len += RecHead::data_size();
        add_sfe(*rh);
        put_head(-1);
        put_ndx(rh->nFile, database->ind);
    }
    catch (...) {
        if (rclose) {
            closeFile();
        }
        throw;
    }
    if (rclose) {
        closeFile();
    }
}

void TDBFile::read_record_from_file(RecEntry* rh, TDataBase* database)
{
    if (status == NOT_OPEN) {
        openFile(connect_db_mode);
    }
    RecHead rhh;
    try {
        database->getrec(*rh, *fstream, rhh);
    }
    catch (...) {
        if (rclose) {
            closeFile();
        }
        throw;
    }
    if (rclose) {
        closeFile();
    }
}

void TDBFile::rebuild_file(size_t nF, TDataBase* database)
{
    openFile(UPDATE_DBV);

    int fPos = dh->FPosTRT;
    int fLen = FPosFree;
    int nRT1 = dh->nRT;
    char isDel = dh->isDel;

    std::string tmp_file_name = full_path + ".tmp";
    GemDataStream out_stream(tmp_file_name, std::ios::out | std::ios::binary);
    for(int ii = 0; ii < fPos; ++ii) {
        out_stream.put(0);
    }
    out_stream.seekp(fPos, std::ios::beg);
    int nRec = database->scanfile(nF, fPos, fLen, *fstream, out_stream);
    out_stream.close();
    closeFile();

    try {
        std::filesystem::path orig_path(full_path);
        std::filesystem::path temp_path(tmp_file_name);

        if (std::filesystem::exists(temp_path)) {
            std::filesystem::rename(temp_path, orig_path);
        } else {
            Error(relativePath(), "Database rebuild failed: Temporary file was not created successfully.");
            return;
        }
    }
    catch (const std::filesystem::filesystem_error& e) {
        Error(relativePath(), "Database file replacement error during rebuild: " + std::string(e.what()));
        return;
    }

    try {
        openFile(UPDATE_DBV);
        fstream->seekg(0L, std::ios::end);
        int new_physical_len = static_cast<int>(fstream->tellg());
        force_update_header(new_physical_len, nRec, nRT1, isDel);
        put_ndx(nF, database->ind);
    }
    catch (...) {
            closeFile();
        throw;
    }
    closeFile();
}

//-----------------------------------------------------------------

DBFileList::~DBFileList()
{
    close_all();
}

void DBFileList::close_all()
{
    for(auto ndx: opened_files) {
        at(ndx)->closeFile();
    }
    opened_files.clear();
    opened_files_list.clear();
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
        dbfile->to_cfg(out_stream);
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
        auto dbfile = std::make_shared<TDBFile>();
        dbfile->from_cfg(in_stream);
        push_back(dbfile);
    }
}

// add new file to database chain
void DBFileList::add_file(const std::string &folder, const std::string &path)
{
    fs::path combined_path = fs::path(folder)/path;
    push_back(std::make_shared<TDBFile>(combined_path.string(), path));
}

void DBFileList::reset_open_list()
{
    opened_files_list.clear();
    for(auto ndx: opened_files) {
        opened_files_list.push_back(at(ndx)->relativePath());
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
            xcpt.mess += "\n 2nd record in file: "+at(current_ndx)->relativePath();
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
        add_file(path, "");
        at(ndx)->create_with_ndx(nRT, isDelete, ndx, ind);
        if(at(ndx)->open_file(open_mode, rclose, ndx, ind)) {
            // logging to rebuld;
        }
        opened_files.insert(ndx);
        reset_open_list();
    }
    catch(TError& xcpt)  {
        if(xcpt.mess == "Two records with the same key (AddFileToList).") {
            xcpt.mess += "\n 2nd record in file: "+at(ndx)->relativePath();
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

void DBFileList::remove_files(const std::set<std::string>& target_names, TDBKeyList& ind)
{
    try {
        for(size_t ii = size(); ii > 0; --ii)  {
            size_t nF = ii - 1;
            if(target_names.count(at(nF)->relativePath())) {

                if(check_writable(nF, "remove file")) {
                    continue; // Skip deletion in db default
                }

                std::set<size_t> new_opened_files;
                for(auto const& ndx : opened_files) {
                    if(ndx > nF) {
                        new_opened_files.insert(ndx - 1);
                    }
                    else if(ndx < nF) {
                        new_opened_files.insert(ndx);
                    }
                }
                opened_files = std::move(new_opened_files);
                erase(begin() + nF);
            }
        }
        // Re-sync indexing layout after successful removal sequence
        reset_index_list(ind);
    }
    catch (const TError& e) {
        // Ensure index integrity even if an operation fails midway
        reset_index_list(ind);
        throw;
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
            names.push_back(std::string(at(ii)->getFKeywd())+std::string(" ")+at(ii)->relativePath());
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
            if(at(ii)->relativePath().find(all_name) != std::string::npos) {
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
            if(keywd == at(ii)->getFKeywd()) {
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
        if(at(ndx)->relativePath() == file_name) {
            return ndx;
        }
    }
    return 0; // in old implementation (posible throw error)
}

std::vector<std::string> DBFileList::file_keywds_according_name(const std::string& file_name_part) const
{
    std::vector<std::string> keywds;
    std::string all_name = "." +file_name_part +".";
    for(size_t ii=0; ii<size(); ++ii) {
        if(at(ii)->relativePath().find(all_name) != std::string::npos) {
            keywds.push_back(at(ii)->getFKeywd());
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
