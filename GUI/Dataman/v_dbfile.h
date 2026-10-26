//-------------------------------------------------------------------
// $Id: v_dbfile.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of TDBFile, TDBKey, TDBKeyList, TDataBase and
//	DataBaseList classes
//
// Copyright (C) 1996-2001  S.Dmytriyeva, D.Kulik
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
//
#ifndef V_DBFILE_H_
#define V_DBFILE_H_

#include <vector>
#include <set>
#include <memory>
#include "v_file.h"
#include "GEMS3K/verror.h"

const int MAXFESTACK = 200; // size array deleted record

extern const char* MARKRECDEL;
extern const char* MARKRECHEAD;
extern const char MARKRKEY;
extern const char* ALLKEY;

class TDBKeyList;
class TDataBase;
class QWidget;

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
    // Sveta 64   long MinDrLen, MaxDrLen; // min and max size of deleted block
    int MinDrLen, MaxDrLen; // min and max size of deleted block
    int curDr;               // number of deleted blocks

    void read(GemDataStream& is);
    void write(GemDataStream& is);

    static int data_size()
    {
        return sizeof(char[8])*2
               + sizeof(char[11]) + sizeof(char[5])
               + sizeof(int)*4
               // Sveta 64    + sizeof(char) + sizeof(long)*2
               + sizeof(char) + sizeof(int)*2
               + sizeof(int);
    }
};

struct DBentry
{   // position and size of deleted block
    // Sveta 64    long pos, len;
    int pos, len;

    void read(GemDataStream& is);
    void write(GemDataStream& is);

    static size_t data_size()
    {
        // Sveta 64  return sizeof(long)*2;
        return sizeof(int)*2;
    }
};

struct RecEntry
{   //position, size and file number of record
    // Sveta 64    long pos, len;
    int     pos, len;
    unsigned char nFile;

    void read(GemDataStream& is);
    void write(GemDataStream& is);
};


// This is file of Data Base
class TDBFile:
                public TFile
{
    // Sveta 64    long FPosFree;  // len of file
    int     FPosFree;  // len of file
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
    TDBFile(const std::string& fName,
            const std::string& fExt, const std::string& fDir );
    TDBFile(const std::string& path);
    TDBFile(std::fstream& f);

    void check_dh();

    //--- Selectors
    VDBhead* Dh() const
    {
        return dh;
    }    // header of file

    int FLen() const
    {
        return FPosFree;
    }

    void FNewLen( int dlt )
    {
        FPosFree -= dlt;
    }
    void SetnRec( int n )
    {
        check_dh();
        dh->nRec = n;
    }
    void GetDh( int& fPos, int& fLen, int& nRT, char& isDel );
    bool GetDhOver();
    void SetDh( int fLen, int nRec, int nRT, char isDel );

    //---  Manipulation files ---
    void Create(int nRT, bool isDel );
    virtual void Open( FileStatus mode );
    virtual void Close();
    void PutHead( GemDataStream& fdb, int deltRec=0 );
    void vdbh_setdt();

    //---  Manipulation deleted blocks ---
    void AddSfe( RecEntry& re );
    void FindSfe( RecEntry& re );


    std::string index_file_name() const;
    void put_ndx(size_t nF, const TDBKeyList &ind);
    void get_ndx(size_t nF, TDBKeyList &ind);
    time_t get_time(bool rclose, RecEntry *ree);
    void create_with_ndx(int nRT, bool isDel, size_t nF, const TDBKeyList& ind);

    /// Open file and read indexes
    /// return true if Stack of deleted records overflow and need compress
    bool open_file(FileStatus mode, bool rclose, size_t nF, TDBKeyList &ind);

    void save_record_to_file(bool new_record, bool rclose, RecEntry *rh, TDataBase* database);
    void delete_record_from_file(bool rclose, RecEntry *rh, TDataBase *database);
    void read_record_from_file(bool rclose, RecEntry *rh, TDataBase *database);
    void rebuild_file(size_t nF, TDataBase *database);
};


class DBFileList: public std::vector<std::shared_ptr<TDBFile>>
{
    friend class TDataBase;

public:
    static bool dbChangeMode;

    DBFileList(size_t nrt, bool need_close, bool isdel):
        std::vector<std::shared_ptr<TDBFile>>(),
        nRT(nrt),
        isDelete(isdel),
        rclose(need_close)
    { }

    virtual ~DBFileList();

    void from_cfg(std::fstream& f);
    void to_cfg(std::fstream& f);

    /// Check legal file index
    bool check_index(size_t file_ndx) const
    {
        return file_ndx<size();
    }

    /// Check file is readonly (from DB.defaults)
    bool check_readonly(size_t file_ndx) const
    {
        return false;//!dbChangeMode && file_ndx<special_files_num;
    }

    /// Check file is open
    bool check_open(size_t file_ndx) const
    {
        return opened_files.find(file_ndx)!=opened_files.end();
    }

    void check_file(size_t file_ndx)
    {
        if(!check_index(file_ndx)) {
            std::string max_valid = (size() > 0) ? std::to_string(size() - 1) : "0";
            Error("check_file", "Illegal DB file index: " + std::to_string(file_ndx) +
                                    ". Valid range is 0 to " + max_valid + ".");
        }
        else if(!check_open(file_ndx)) {
            Error("check_file", "Attempted to access a database file that is not open. File path: " +
                                    at(file_ndx)->GetPath());
        }
        last_used_file = file_ndx;
    }

     bool check_writable(size_t nF, const std::string& operation) const
    {
        if(check_readonly(nF)) {
            std::string error_msg = "Cannot " + operation +
                                    ": modifying a system database file is not allowed (read-only mode enabled).";
            Error(at(nF)->Name(), error_msg);
            return false;
        }
        return true;
    }

    void finish_dbdefaults()
    {
        special_files_num = size();
    }

    void add_file(const std::string &path);

    std::vector<int> open_files(FileStatus mode, const std::vector<int> &nff, TDBKeyList &ind);
    std::vector<int> add_open_files(const std::vector<int> &nff, TDBKeyList &ind);
    size_t open_new_file(const std::string &path, TDBKeyList &ind);
    void remove_file(const std::string &path, TDBKeyList &ind);


    /// Generate file list for using in GUI
    void files_list(int mode, std::vector<std::string> &names,
                    std::vector<int> &indeces, std::vector<int> &sel) const;

    std::vector<int> all_files(bool only_kernel) const;
    /// indexes all files contained names
    std::vector<int> files_with_name(const std::vector<std::string> &names) const;
    std::vector<int> files_with_keywds(const std::vector<std::string> &keywds, bool &all_found) const;
    std::vector<int> opened_special_files() const;
    /// Index file with name (search only from open files)
    size_t file_name_index(const std::string &file_name) const;
    /// get list of files keywds, that contained name
    std::vector<std::string> file_keywds_according_name(const std::string &file_name_part) const;
    /// true if opened some files from default data base
    bool check_special_open() const;

    // >= 0 selected file
    // -1 cancell
    int file_to_add_record(QWidget *window, const std::string &record_key, bool multy, bool& ok_to_all);
protected:

    unsigned char nRT;
    bool isDelete;

    /// Close file after any read/write
    bool rclose;


    FileStatus open_mode;

    /// Indexes of opened files
    std::set<size_t> opened_files;

    /// Number files from DB.defaults
    size_t special_files_num;

    /// Current record file index
    size_t fNum = 0;
    size_t last_used_file=0;

    /// List of opened files
    std::vector<std::string> opened_files_list;

    void close_all();
    void reset_open_list();
    void reset_index_list(TDBKeyList &ind);
};

#endif  // V_DBFILE_H_
