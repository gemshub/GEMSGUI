//-------------------------------------------------------------------
// $Id: v_file.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of TFile class and some filename related constants
//
// Copyright (C) 1996-2000 S.Dmytriyeva, D.Kulik
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

#ifndef _v_file_h_
#define _v_file_h_

#include <vector>
#include <string>
#include <fstream>
class GemDataStream;

typedef std::ios::openmode FileStatus;

const int 	MAX_FILENAME_LEN = 20;

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

class TFile
{
    static  char vv[9];
    static  char pa[9];

    char Keywd[MAX_FILENAME_LEN+1];
    FileStatus  status;
    std::string dir;
    std::string name;
    std::string ext;
    std::string Path;

    void makeKeyword();

protected:
    virtual void write( std::fstream& );
    virtual void *read( std::fstream& );

public:
    GemDataStream& f;

    //  TFile();
    TFile(const std::string& fName,
          const std::string& fExt, const std::string& fDir );
    TFile(const std::string& path);
    TFile(std::fstream& fcfg);
    virtual ~TFile();
    void toCFG(std::fstream& fcfg);

    //--- Selectors
    const char* GetKeywd() const
    {
        return Keywd;
    }
    FileStatus Test() const
    {
        return status;
    }
    const std::string& Name() const
    {
        return name;
    }
    const std::string& Ext() const
    {
        return ext;
    }
    const std::string& Dir() const
    {
        return dir;
    }
    const std::string& GetPath() const
    {
        return Path;
    }

    //--- Manipulation
    void newFile( const std::string& newName, const std::string& newExt,
                  const std::string& newDir );
    void Makepath();
    bool Exist();
//    bool CheckOverwrite(FileStatus mode);
    virtual void Open( FileStatus mode );
    virtual void Close();
    void OpenFromFileName( const std::string& filename, FileStatus mode );
    //  void Reopen(const char* file, FileStatus mode);

    static const char* VV()
    {
        return vv;
    }
    static const char* PA()
    {
        return pa;
    }
};

// file names constants

extern const char *PDB_EXT;
extern const char *NDX_EXT;

#endif // _v_file_h
