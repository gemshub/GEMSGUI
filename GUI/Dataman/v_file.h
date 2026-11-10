//-------------------------------------------------------------------
// $Id$
//
// Declaration of TBinaryFile class and some file-related constants and functions
//
// Copyright (C) 1996-2026 S.Dmytriyeva, D.Kulik
//
// This file is part of the GEM-Selektor GUI library which uses the
// Qt v.6 cross-platform application & UI framework (https://qt.io/download-open-source)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the GPL v.3 license
//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#ifndef _v_file_h_
#define _v_file_h_

#include <cstdint>
#include <vector>
#include <string>
#include <fstream>
#include <memory>
#include "GEMS3K/v_service.h"

class GemDataStream;
class QWidget;

typedef std::ios::openmode FileStatus;

const int 	MAX_FILENAME_LEN = 20;

// file names constants
extern const char *PDB_EXT;
extern const char *NDX_EXT;

static const FileStatus  NOT_OPEN = FileStatus(0),    // no file  opend
    RDONLY_B = std::ios::in|std::ios::binary,
    WRITE_B =  std::ios::out|std::ios::binary,
    APPEND_B =   std::ios::out|std::ios::app|std::ios::binary,
    RDONLY_DBV = std::ios::in|std::ios::binary,
    UPDATE_DBV = std::ios::in|std::ios::out|std::ios::binary;

/**
 * @brief Recursively finds all files in a folder, optionally filtered by extension.
 *
 * @param folder The root directory path to scan.
 * @param ext The target file extension (e.g., ".txt"). If empty, returns all files.
 * @return A vector of strings containing full paths to the discovered files.
*/
std::vector<std::string> folderFiles(const std::string& folder, const std::string& ext);

/**
 * @brief Checks if a file or directory exists at the given path safely.
 * @param file_path The path to the file or directory.
 * @return true if the path exists, false if it doesn't or if an OS error occurs.
 */
bool fileExist(const std::string &file_path);

/**
 * @brief Deletes a directory and all of its contents recursively.
 * @param folder The path to the folder to be removed.
 * @return true if the folder was successfully deleted or didn't exist; false if an error occurred.
 */
bool removeFolder(const std::string &folder);

/**
 * @brief Prepares or creates a database directory, ensuring no conflicting files exist.
 * @param folder The target directory path.
 */
void makeDBDirectory(const std::string& folder);

/**
 * @brief Safely clears external references and deletes the entire database directory.
 * @param folder The path to the database directory.
 */
void deleteDBDirectory(const std::string& folder);

/**
 * @brief Copies a binary file from source to destination using std::filesystem.
 * @param source The path to the source file.
 * @param destination The path to the destination file or target directory.
 * @return true if successfully copied, false if an OS error occurs.
 */
bool copyBinaryFile(const std::string& source, const std::string& destination);

/**
 * @brief Prepares a directory, handling overwrites via UI prompts.
 * @param window Parent Qt window for modal dialogs.
 * @param folder Target directory path.
 * @param ask_overwrite 0 = skip check, 1 = keep existing files, 2 = wipe directory clean if it exists.
 */
void makeDirectory(QWidget* window, const std::string& folder, int ask_overwrite);


/// @class TBinaryFile
/// @brief Represents a cross-platform binary file handler managed within a database or project structure.
/// @details This class handles file paths (both absolute and relative to the project root),
///          manages low-level data streams, and supports unique file identification using both
///          legacy keywords and modern deterministic 64-bit FNV-1a hashes.
class TBinaryFile
{
    static char vv[9];
    static char pa[9];

public:
    /// @brief Constructor that initializes the file object with a given path.
    /// @param path The initial filesystem path to the target binary file.
    explicit TBinaryFile(const std::string& path="");

    /// @brief Virtual destructor to ensure proper cleanup of resources in derived classes.
    virtual ~TBinaryFile();

    /// @brief Gets the legacy keyword identifier for backward compatibility.
    /// @return The character array Keywd converted to a safe std::string.
    std::string getFKeywd() const
    {
        return char_array_to_string(Keywd, MAX_FILENAME_LEN);
    }

    /// @brief Gets the modern deterministic 64-bit FNV-1a hash of the file.
    /// @return The precomputed uint64_t hash of the normalized relative path.
    uint64_t getHash() const
    {
        return fnv1a_64_hash;
    }

    /// @brief Gets the clean filename without path and extensions.
    /// @return A const reference to the internal file_name string.
    const std::string& name() const
    {
        return file_name;
    }

    /// @brief Gets the standardized path relative to the project/database root.
    /// @return A const reference to the cross-platform relative_path string.
    const std::string& relativePath() const
    {
        return relative_path;
    }

    /// @brief Gets the absolute (full) path to the file on disk.
    /// @return A const reference to the full_path string.
    const std::string& fullPath() const
    {
        return full_path;
    }

    /// @brief Checks if the binary file actually exists on the physical storage.
    /// @return True if the file exists on disk, false otherwise.
    bool exist();

    /// @brief Opens the binary file using the specified mode.
    /// @param mode The access/open mode (e.g., Read, Write) to apply to the file stream.
    virtual void openFile(FileStatus mode);

    /// @brief Safely closes the active file stream and flushes any pending data.
    virtual void closeFile();

    virtual void from_cfg(std::fstream& config_stream);
    virtual void to_cfg(std::fstream& config_stream);

    static const char* VV()
    {
        return vv;
    }
    static const char* PA()
    {
        return pa;
    }

protected:
    /// Legacy unique key generated from file_name (kept for backward compatibility)
    char Keywd[MAX_FILENAME_LEN + 1];

    /// Modern deterministic FNV-1a 64-bit hash computed from the normalized relative_path
    uint64_t fnv1a_64_hash;

    /// The absolute (full) path to the binary file on disk
    std::string full_path;

    /// The path relative to the database (default or projects) root-path
    std::string relative_path;

    /// The clean filename without the directory path and without the final extension
    std::string file_name;

    /// The final file extension (e.g., ".dat" or "bin")
    std::string ext;

    /// Internal file open/error status
    FileStatus status;

    /// Reference to the underlying low-level data stream
    std::unique_ptr<GemDataStream> fstream;

    /// Generates the legacy keyword (or updates the new FNV-1a hash)
    void make_keyword();
    void split_path();
};


#endif // v_file_h
