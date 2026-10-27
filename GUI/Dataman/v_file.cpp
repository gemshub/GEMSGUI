//-------------------------------------------------------------------
// $Id: v_file.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of TFile class
//
// Copyright (C) 1996-2001 S.Dmytriyeva
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

#ifndef _WIN32
#include <unistd.h>
#else
#include <io.h>
#endif

#include "v_file.h"
#include "v_dbm.h"
#include "GEMS3K/v_service.h"
#include "GEMS3K/gdatastream.h"
#include <filesystem>
namespace fs = std::filesystem;

extern std::shared_ptr<spdlog::logger> gui_logger;

std::vector<std::string> folderFiles(const std::string& folder, const std::string& ext) {
    std::vector<std::string> files;
    fs::path dir_path(folder);
    std::error_code ec;

    auto it = fs::recursive_directory_iterator(dir_path, fs::directory_options::skip_permission_denied, ec);
    if(ec) {
        gui_logger->error("The {} folder cannot be opened/accessed.", dir_path.string());
        return files;
    }

    // Traverse through all files and subdirectories recursively
    for(auto const& dir_entry : it) {
        if (dir_entry.is_regular_file(ec) && !ec) {
            if (ext.empty() || dir_entry.path().extension() == ext) {
                std::string file = dir_entry.path().string();
                gui_logger->debug("Adding file: {}", file);
                files.emplace_back(std::move(file));
            }
        }
    }
    return files;
}

bool fileExist(const std::string &file_path)
{
    std::error_code ec;
    return fs::exists(file_path, ec);
}

bool removeFolder(const std::string &folder)
{
    std::error_code ec;
    std::uintmax_t removed_count = fs::remove_all(folder, ec);
    if (ec) {
        gui_logger->error("The {} folder cannot be removed.", folder);
        return false;
    }
    gui_logger->debug("Deletes a directory {} and all of its contents {} recursively", folder, removed_count);
    return true;
}

void makeDBDirectory(const std::string& folder)
{
    if(fileExist(folder)) {
        auto files_into = folderFiles(folder, ".pdb");
        if(!files_into.empty()) {
            Error(folder, "Error creating Modelling Project directory! (Contains existing .pdb files)");
            return;
        }
        gui_logger->warn("Directory {} already exists but contains no .pdb files.", folder);
        return;
    }
    if(!u_create_directory(folder)) {
        Error(folder, "Error creating Modelling Project directory! (OS creation failed)");
    }
}

void deleteDBDirectory(const std::string& folder)
{
    if(!fileExist(folder)) {
        return;
    }
    auto files_to_remove = folderFiles(folder, ".pdb");
    for(auto& module : rt) {
        if(module) {
            module->removeFiles(files_to_remove);
        }
    }
    if (!removeFolder(folder)) {
        gui_logger->warn("Failed to fully delete DB directory: {}", folder);
    }
}

bool copyBinaryFile(const std::string& source, const std::string& destination) {
    std::error_code ec;

    fs::path src_path(source);
    fs::path dst_path(destination);

    if(fs::is_directory(dst_path, ec) && !ec) {
        dst_path /= src_path.filename();
    }
    fs::copy_options options = fs::copy_options::overwrite_existing;
    fs::copy_file(src_path, dst_path, options, ec);
    if(ec) {
        gui_logger->warn("Error copying from {} to {}. Reason: {}", source, dst_path.string(), ec.message());
        return false;
    }
    return true;
}

//----------------------------------------------------------
// TFile
//----------------------------------------------------------

char TFile::vv[9]="GEMBASE1";
char TFile::pa[9]="01041999";

void
TFile::write( std::fstream& out_stream )
{
    out_stream << Keywd << "  "; // endl;
    int ln = Path.length() + 1;
    out_stream << ln << ' '; // endl;
    out_stream << Path.c_str() << std::endl;

    ErrorIf( out_stream.bad(), GetKeywd(),
             "Error writing TFile to configurator.");
}

void *
TFile::read( std::fstream& in_stream )
{
    //    is.read( Keywd, MAX_FILENAME_LEN );
    // we should put length here !!
    in_stream >> Keywd;
    int ln;
    in_stream >> ln;
    std::string sss;
    in_stream >> sss;

    Path = sss;
    u_splitpath(Path, dir, name, ext);

    ErrorIf( in_stream.bad(), GetKeywd(),
             "Error reading TFile from configurator.");
    return this;
}

// configuration to file
void TFile::toCFG(std::fstream& fcfg)
{
    write(fcfg);
}

TFile::TFile(std::fstream& fcfg):
        status( NOT_OPEN ),
	f(*new GemDataStream())
{
    read(fcfg);
    Makepath();
}

TFile::TFile(const std::string& fName,
             const std::string& fExt, const std::string& fDir):
        status( NOT_OPEN ),
	f(*new GemDataStream())
{
    newFile( fName, fExt, fDir );
    makeKeyword();
}

TFile::TFile(const std::string& path):
        status( NOT_OPEN ),
        Path(path),
	f(*new GemDataStream())
{
    u_splitpath(Path, dir, name, ext);
    makeKeyword();
}

TFile::~TFile()
{
    if( status != NOT_OPEN )
        Close();
    delete &f;
}

void
TFile::makeKeyword()
{
    std::string key;
    if( name.empty() )
        return;

    // ignoring version and after
    // get version
    std::string fname = name;
    size_t pos1 = fname.find(".ver");
    if( pos1 != std::string::npos )
       fname = fname.substr(0, pos1 );

    key = std::string(fname, 0, 2);
    size_t npos = 0;
    size_t npos2 = fname.find(".", npos);
    while( npos2 != std::string::npos )
    {   npos = npos2+1;
        key += std::string(fname, npos, 2);
        npos2 = fname.find(".", npos);
    }
    key += std::string(fname, npos+2);

    strncpy( Keywd, key.c_str(), MAX_FILENAME_LEN);
    Keywd[MAX_FILENAME_LEN]='\0';
}


// Set new full file name. Old file closed.
void TFile::newFile( const std::string& newName, const std::string& newExt,
                     const std::string& newDir )
{
    if( status != NOT_OPEN )
        Close();

    dir = newDir;
    ext = newExt;
    name = newName;

    Makepath();
}

//Get full name of file
void TFile::Makepath()
{
    Path = u_makepath( dir, name, ext );
}

//Check for existence of file
bool TFile::Exist()
{
    if( Test() )
        return true;  // file alredy open
    if( Path.empty() )
        Makepath();

    if( access( Path.c_str(), 0 ) )
        return false;
    else  return true;
}

// Open file in special mode
void TFile::Open( FileStatus mode )
{
    if( mode == status )
        return;
    if( status != NOT_OPEN )
        Close();
    if( mode == NOT_OPEN )
        return;
    if( !Exist() && !(mode == WRITE_B || mode == APPEND_B) )
        Error( GetPath(),"File not found.");
#ifndef _WIN32
    if( mode== UPDATE_DBV && access( Path.c_str(), W_OK ) != 0)
        mode = RDONLY_DBV;
#else
if( mode== UPDATE_DBV && access( Path.c_str(), 02 ) != 0)
    mode = RDONLY_DBV;
#endif

    /*   if( mode == WRITE_T  || mode == WRITE_B )
          if( !vfQuestion( "Question", "Write over previous file (Y) or skip (N)?" ) )
                 Error( GetKeywd(),"Cansel of open file.");
    */
// it seems like latest libstdc++ specs does not enforce
// state and pointer reset :(
// we have to do it on our own
    f.clear();
    f.open( Path.c_str(), mode );

    ErrorIf( !f.good(), GetPath(), "File open error.");

    if( (mode & std::ios::out) || (mode & std::ios::app) )
    f.seekp(0, std::ios::beg);
    if( mode & std::ios::in )
    f.seekg(0, std::ios::beg);

    status = mode;
}

void TFile::OpenFromFileName( const std::string& filename, FileStatus mode )
{
    Close();
    remove(GetPath().c_str());
    rename(filename.c_str(), GetPath().c_str());
    Open(mode);
}

/*
// check file in special mode
bool TFile::CheckOverwrite( FileStatus mode )
{
    if( Exist() && (mode == WRITE_T || mode == WRITE_B) )
        return true;
    return false;
}
*/

//Close file
void TFile::Close()
{
    f.close();
    status = NOT_OPEN;
}

// file names constants

const char * PDB_EXT = "pdb";
const char * NDX_EXT = "ndx";

//--------------------- End of v_file.cpp ---------------------------

