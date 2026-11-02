//-------------------------------------------------------------------
// $Id$
//
// Implementation of TBinaryFile class and some file-related constants and functions
//
// Copyright (C) 1996-2026 S.Dmytriyeva, D.Kulik
//
// This file is part of the GEM-Selektor GUI library which uses the
// Qt v.6 cross-platform App & UI framework (https://qt.io/download-open-source)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the GPL v.3 license
//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#include <filesystem>
namespace fs = std::filesystem;

#include "v_dbm.h"
#include "service.h"
#include "GEMS3K/v_service.h"
#include "GEMS3K/gdatastream.h"

//char TBinaryFile::vv[9]="GEMBASE1";
//char TBinaryFile::pa[9]="01041999";
char TBinaryFile::vv[9]="GEMBASE2";
char TBinaryFile::pa[9]="01112026";
const char * PDB_EXT = "pdb";
const char * NDX_EXT = "ndx";

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

void makeDirectory(QWidget* window, const std::string& folder, int ask_overwrite)
{
    if(fileExist(folder)) {
        if(ask_overwrite > 0) {
            std::string mess = folder + "\nThis directory exists! Overwrite?";

            // If the user clicks "No", cancel the operation safely
            if(!vfQuestion(window, "Create new directory", mess)) {
                gui_logger->info("Directory creation cancelled by user: {}", folder);
                return; // Safely exit without triggering a fatal Error state
            }

            if(ask_overwrite == 2) {
                gui_logger->info("Wiping existing directory as requested: {}", folder);
                removeFolder(folder);
            }
            else {
                gui_logger->warn("Directory {} already exists. Proceeding with existing contents.", folder);
                return;
            }
        }
        else {
            gui_logger->warn("Directory {} already exists. Silent skip.", folder);
            return;
        }
    }
    if(!u_create_directory(folder)) {
        Error(folder, "Error creating directory! (OS creation failed)");
    }
}

/// Deterministic FNV-1a 64-bit hashing algorithm
static uint64_t fnv1a_64(const std::string& str) {
    uint64_t hash = 14695981039346656037ULL;
    const uint64_t fnv_prime = 1099511628211ULL;
    for(char c : str) {
        hash ^= static_cast<uint8_t>(c);
        hash *= fnv_prime;
    }
    return hash;
}

/// Cross-platform path normalization (handles slashes and case-insensitivity)
static std::string normalizePath(const fs::path& path) {
    std::string target = path.generic_string();
    std::transform(target.begin(), target.end(), target.begin(), [](unsigned char c) {
        return std::tolower(c);
    });
    return target;
}

//----------------------------------------------------------
// TBinaryFile
//----------------------------------------------------------

TBinaryFile::TBinaryFile(const std::string& path):
    full_path(path),
    status(NOT_OPEN),
    fstream(std::make_unique<GemDataStream>())
{
    split_path();
    make_keyword();
    std::string normalized = normalizePath(relative_path);
    fnv1a_64_hash = fnv1a_64(normalized);
}

TBinaryFile::~TBinaryFile()
{
    if(status != NOT_OPEN) {
        closeFile();
    }
}

GemDataStream &TBinaryFile::stream()
{
    return *fstream;
}

bool TBinaryFile::exist()
{
    return fileExist(full_path);
}

void TBinaryFile::openFile(FileStatus ios_mode)
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
    bool is_readonly = (ios_mode & std::ios::in) &&
                       !(ios_mode & (std::ios::out | std::ios::app | std::ios::trunc));

    if (!exist() && is_readonly) {
        Error(full_path, "File not found.");
        return;
    }

    fstream->clear();
    fstream->open(full_path.c_str(), ios_mode);

    ErrorIf(!fstream->good(), full_path, "File open error.");

    if ((ios_mode & std::ios::out) && !(ios_mode & std::ios::app))  {
        fstream->seekp(0, std::ios::beg);
    }
    if (ios_mode & std::ios::in) {
        fstream->seekg(0, std::ios::beg);
    }
    status = ios_mode;
}

void TBinaryFile::closeFile()
{
    if(status != NOT_OPEN) {
        fstream->close();
    }
    status = NOT_OPEN;
}

void TBinaryFile::from_cfg(std::fstream &config_stream)
{
    size_t ln;
    config_stream >> Keywd;
    //config_stream >> fnv1a_64_hash;
    config_stream >> ln;
    config_stream >> full_path;

    split_path();
    //check make_keyword();
    std::string normalized = normalizePath(relative_path);
    fnv1a_64_hash = fnv1a_64(normalized);
}

void TBinaryFile::to_cfg(std::fstream &config_stream)
{
    config_stream << Keywd << "  ";
    config_stream << fnv1a_64_hash << "  ";
    //config_stream << full_path.length() << ' ';
    config_stream << full_path << std::endl;
}

void TBinaryFile::split_path()
{
    fs::path path(full_path);
    relative_path = file_name = path.filename().stem().string();
    ext = path.extension().string();
    if(!ext.empty() && ext[0] == '.') {
        ext.erase(0, 1);
    }
}

void TBinaryFile::make_keyword()
{
    std::string key;
    if(file_name.empty()) {
        return;
    }

    // ignoring version and after
    std::string fname = file_name;
    size_t pos1 = fname.find(".ver");
    if(pos1 != std::string::npos) {
        fname = fname.substr(0, pos1);
    }
    key = std::string(fname, 0, 2);
    size_t npos = 0;
    size_t npos2 = fname.find(".", npos);
    while(npos2 != std::string::npos) {
        npos = npos2+1;
        key += std::string(fname, npos, 2);
        npos2 = fname.find(".", npos);
    }
    key += std::string(fname, npos+2);
    strncpy(Keywd, key.c_str(), MAX_FILENAME_LEN);
    Keywd[MAX_FILENAME_LEN]='\0';
}

//--------------------- End of v_file.cpp ---------------------------

