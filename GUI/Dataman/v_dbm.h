//-------------------------------------------------------------------
// $Id: v_dbm.h 1179 2009-01-20 10:27:35Z gems $
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
#ifndef V_DBM_H_
#define V_DBM_H_

#include <initializer_list>
#include <ctime>
#include "v_object.h"
#include "v_dbfile.h"

class GemDataStream;

const int	MAXRKFRMSTR = 20;  // max fields in key
const size_t IND_PLUS = 112;    // augment of index key buf
extern const char* ALLKEY;

enum keyctrl {   // codes key bild
    K_END = -5, K_EMP, K_ANY, K_IMM, K_ACT
};

struct KeyArg {
    int32_t command;
    std::string str_value;
    uint32_t field_index = 0;

    KeyArg(int32_t cmd):
        command(cmd), str_value(""), field_index(0) {}

    KeyArg(int32_t cmd, int32_t fld_idx):
        command(cmd), str_value(""), field_index(static_cast<uint32_t>(fld_idx)) {}

    KeyArg(int32_t cmd, const char* str):
        command(cmd), str_value(str ? str : ""), field_index(0) {}

    KeyArg(int32_t cmd, std::string str):
        command(cmd), str_value(str), field_index(0) {}
};

/**
 * @class TDBKeyNew
 * @brief Manages structured, multi-field compound database index keys.
 *
 * This class handles the parsing, packing, unpacking, formatting, and dynamic compilation
 * of multi-segment search and sort database keys separated by a colon utility character.
 */
class TDBKeyNew
{
public:
    /**
     * @brief Constructs a key layout descriptor from raw data geometry arrays.
     *
     * @param nRkflds Number of individual sub-fields constituting the compound key.
     * @param rkfrm   Pointer to an array defining the maximum byte lengths for each sub-field.
     */
    explicit TDBKeyNew(unsigned char nRkflds, const unsigned char* rkfrm);

    /**
     * @brief Restores the key metadata configuration state directly from an active file stream.
     * @param f Reference to the active binary or text file input stream.
     */
    TDBKeyNew(std::fstream& f);

    /**
     * @brief Explicitly defaults the standard copy constructor.
     */
    TDBKeyNew(const TDBKeyNew& other) = default;

    /**
     * @brief Explicitly defaults the standard move constructor.
     */
    TDBKeyNew(TDBKeyNew&& other) noexcept = default;

    /**
     * @brief Explicitly defaults the copy assignment operator.
     */
    TDBKeyNew& operator=(const TDBKeyNew& other) = default;

    /**
     * @brief Explicitly defaults the move assignment operator.
     */
    TDBKeyNew& operator=(TDBKeyNew&& other) noexcept = default;

    /**
     * @brief Virtual destructor to ensure clean cleanup in polymorphically inherited classes.
     */
    virtual ~TDBKeyNew() = default;

    // --- Selectors (Getters) ---

    /**
     * @brief Serializes the composite key into its tight, packed string representation.
     * @return std::string The packed key structure.
     */
    std::string packKey() const;

    /**
     * @brief Deserializes the key into an expanded, space-padded unpacked layout.
     * @return std::string The unpacked formatted key.
     */
    std::string unpackKey() const;

    /**
     * @brief Returns the total static maximum buffer size reserved for the fixed key layout.
     * @return size_t The exact byte length constraint.
     */
    size_t length() const
    {
        return fixed_keys_len;
    }

    /**
     * @brief Returns the total number of sub-fields registered in this compound key structure.
     * @return size_t The length count of the fields vector.
     */
    size_t fieldsInKey() const
    {
        return flds_len.size();
    }

    /**
     * @brief Safely retrieves the maximum byte limit of a specific sub-field by its index.
     * @param ndx The zero-based index of the target sub-field.
     * @return size_t The maximum allowed length, or 0 if the index is out of bounds.
     */
    size_t fieldLength(size_t ndx) const
    {
        return ndx < fieldsInKey() ? flds_len[ndx] : 0;
    }

    /**
     * @brief Safely fetches the current value of a specific sub-field without modification.
     * @param ndx The zero-based index of the target sub-field.
     * @return std::string The raw field string literal value, or an empty string if out of bounds.
     */
    std::string fieldKey(size_t ndx) const
    {
        return ndx < flds_value.size() ? flds_value[ndx] : "";
    }

    /**
     * @brief Unpacks, expands, and aligns a specific sub-field with blank spaces matching its specification.
     * @param ndx The zero-based index of the target sub-field.
     * @return std::string The space-padded unpacked field key value string.
     */
    std::string unpackFieldKey(size_t ndx) const
    {
        if (ndx >= flds_value.size()) {
            return "";
        }
        auto unpack_fld = flds_value[ndx];
        unpack_fld.resize(flds_len[ndx], ' ');
        return unpack_fld;
    }

    // --- Manipulation Keys (Setters) ---

    /**
     * @brief Parses and overrides the entire composite key from a raw composite input string.
     * @param new_key The incoming raw packed or unpacked key string.
     */
    void setKey(const std::string& new_key);

    /**
     * @brief Safely overrides the literal value of a single specific sub-field slot.
     * @param ndx     The zero-based index of the target sub-field slot.
     * @param new_fld The incoming new string value for the specified field.
     */
    void setFieldKey(size_t ndx, const std::string& new_fld);

    /**
     * @brief Compiles a type-safe multi-segment compound index key from structured command parameters.
     *
     * Iterates through a flat list of formatting arguments, processes wildcard tokens (*, `),
     * pulls dynamic values from active relation tables, and merges them using colon delimiters.
     *
     * @param nrt   The active operational relation table context code.
     * @param args  Initializer list containing structured KeyArg configuration options.
     * @return std::string The safely compiled, formatted index key.
     */
    std::string makeKey(uint8_t nrt, std::initializer_list<KeyArg> args) const;

protected:
    /// Maximum allowed physical capacity bounds for each independent key segment field.
    std::vector<size_t> flds_len;

    /// Current alphanumeric buffer values for each sub-field component segment.
    std::vector<std::string> flds_value;

    /// The aggregated ceiling byte capacity constraint for the combined packed/unpacked key.
    size_t fixed_keys_len;

    void set_packed(const std::string& key);
    void set_unpacked(const std::string& key);
    void set_all()
    {
        for (auto& val : flds_value) {
            val = "*";
        }
    }
};

/// Table key -> the selector, which must be an object containing the key attribute.
using keysmap_t = std::map<std::string, RecEntry>;

// This is struct contened the key of record
class TDBKey
{
    char *pKey;                  // packed key of record
    char *uKey;                  // unpacked key
    unsigned char rkFlds;        // number fields in key
    unsigned char *rkLen;        // len of fields
    unsigned char *rkInd;        // index of fields
    unsigned char fullLen;       // full length of records key

    const TDBKey& operator=(const TDBKey&);

protected:
    void unpack( const char *key );
    void pack( const char *key );

public:
    TDBKey(unsigned char nRkflds,const unsigned char* rkfrm );
    TDBKey(std::fstream& f);
    TDBKey( const TDBKey& dbKey );
    virtual ~TDBKey();
    void check();

    //--- Selectors
    const char *PackKey()
    {
        pKey[fullLen+rkFlds] = '\0';
        return pKey;
    }
    const char *UnpackKey() const
    {
        uKey[fullLen] = '\0';
        return uKey;
    }
    unsigned char KeyLen() const
    {
        return fullLen;
    }
    unsigned char KeyNumFlds() const
    {
        return rkFlds;
    }
    char *FldKey( uint i) const
    {
        return uKey+rkInd[i];
    }
    unsigned char FldLen(uint i) const
    {
        return rkLen[i];
    }
    unsigned char* RkFrm() const
    {
        return rkLen;
    }

    //--- Manipulation keys
    void SetKey( const char *key );
    void SetFldKey( uint i, const char *fld );
    //  void MakeKey( int nRtwrk, char *pkey, ...);	// !!!!!!
}
;

//This class contened the list of indexes
class TDBKeyList:
            public TDBKey
{
    int nI;
    size_t recInDB;  // records in DB
    size_t nBuf;
    RecEntry *re;
    char *ndx[MAXRKFRMSTR];

    //key list for a wildcard search
    TCStringArray aKey;
    TCIntArray anR;

    TDBKeyList(const TDBKeyList&);
    const TDBKeyList operator=(const TDBKeyList&);

protected:
    void init();
    int keycom( uint );
    uint keycmp( uint );
    void arec_add( uint );
    void check_ndx();

public:
    TDBKeyList( unsigned char nRkflds, const unsigned char* rkfrm );
    TDBKeyList( std::fstream& f );
    ~TDBKeyList();
    void check_i(uint i);

    //--- Selectors
    RecEntry* RecPosit(uint i);

    // int iRec() const
    // {
    //     return nI;
    // }
    size_t RecCount() const
    {
        return recInDB;
    }
    char *RecKeyFld(uint i, uint j) const
    {
        return ndx[j]+i*FldLen(j);
    }
    const TCStringArray& KeyList() const
    {
        return aKey;
    }
    const TCIntArray& RnList() const
    {
        return anR;
    }
    size_t KeyInList()
    {
        return aKey.size();
    }

    //--- Manipulation records
    uint  addndx( uint nF, int len, const char *key );
    void delndx(uint i);
    int  findx( const char *key );
    size_t  xlist( const char *pattern );
    void PutKeyList( uint nF, GemDataStream& f) const;
    void GetKeyList_i( uint nF,int nRec, GemDataStream& f, const std::string& file_name );
    void delfile( uint nF );

    //--- Manipulation key
    void PutKey(uint i);
    void RecKey(uint i, std::string& kbuf );

    //--- reset class
    void initnew();
};

//-------------------------------------------------------------

enum RecStatus {   // states of keys record Data Base
    UNDF_=-7 /* undefined state */, FAIL_=-1 /* access error */,
    NONE_=0 /* no records */, ONEF_ ,
    MANY_ , EMPC_, /* empty chain */
    /* NONELIST_ * no records to part */
};

enum modeList { openf = 0x01, closef = 0x02, oldself = 0x04 };

// Structure of Data Base

class TDataBase
{
    friend TDBFile;

    char Keywd[MAXKEYWD];
    unsigned char nRT2;
    RecStatus status;
    int frstOD;
    unsigned char nOD;
    int frstODjson;
    int lastODjson;
    TDBKeyList ind;
    TDBFileList dbfiles_list;

    //work param
    time_t crt;

public:

    TDataBase( size_t nrt, const char* name, bool Rclose, bool isDel,
              int nOf, unsigned char Nobj, int filesNum,
              unsigned char nRkflds, const unsigned char* rkfrm );
    TDataBase( std::fstream& f );
    ~TDataBase();
    //put state to cfg file
    void toCFG( std::fstream& f );

    //--- Selectors
    RecStatus GetStatus() const
    {
        return status;
    }
    void SetStatus( RecStatus stt )
    {
        status = stt;
    }

    /// Sets the number of objects of the record (cached configurations may hold an older count).
    void setObjectCount(unsigned char n)
    {
        if( lastODjson == frstODjson+nOD-1 )
            lastODjson = frstODjson+n-1;
        nOD = n;
    }
    void updateJsonOD(int frstOD, int lastOD)
    {
        frstODjson =frstOD;
        lastODjson =lastOD;
    }

    std::string getKeywd() const
    {
        return char_array_to_string(Keywd, MAXKEYWD);
    }

    size_t RecCount() const
    {
        return ind.RecCount();
    }
    const char *PackKey()
    {
        return ind.PackKey();
    }
    const char *UnpackKey()
    {
        return ind.UnpackKey();
    }
    void SetKey( const char *key )
    {
        ind.SetKey(key);
    }
    const TDBKey& GetDBKey() const
    {
        return ind;
    }
    uint KeyLen() const
    {
        return ind.KeyLen();
    }
    char *FldKey( uint i ) const
    {
        return ind.FldKey(i);
    }
    unsigned char FldLen(uint i) const
    {
        return ind.FldLen(i);
    }
    unsigned char KeyNumFlds() const
    {
        return ind.KeyNumFlds();
    }
    void MakeKey( unsigned char nRtwrk, char *pkey, ...);

    void Open( bool type, FileStatus mode, const TCIntArray& indx);
    void Close();

    // =========================================================================
    // --- Database Records Manipulation Methods ---
    // =========================================================================

    /**
     * @brief Checks whether a specific record exists in the database.
     * @param key The unique key index identifier of the record.
     * @return true If the record is located, false otherwise.
     */
    bool existsRecord(const std::string& key)
    {
        return Find(key.c_str()) >= 0;
    }

    /**
     * @brief Creates and appends a new record to the specified collection storage file.
     *
     * @param key      The unique key string identifier for the new record.
     * @param file_ndx Target physical database file tracking slot index.
     * @return std::string A detailed verification string or record ID generated upon success.
     */
    std::string addRecord(const std::string& key, size_t file_ndx);

    /**
     * @brief Retrieves a single record payload from the collection and loads it into active memory.
     *
     * @param key The unique key string identifier of the target record.
     * @throws Critical exception/Error if the record is non-existent.
     */
    void readRecord(const std::string& key)
    {
        int Rnum = Find(key.c_str());
        if(Rnum < 0) {
            std::string msg = "Database access error: Record '" + key + "' was not found!";
            Error(getKeywd(), msg);
            return;
        }
        Get(Rnum);
    }

    /**
     * @brief Updates the payload content of an existing record matched by its key.
     *
     * @param key The unique key identifier the record to be modified.
     * @return std::string The validated record identifier string.
     */
    std::string updateRecord(const std::string& key)
    {
        int Rnum = Find(key.c_str());
        if(Rnum < 0) {
            Error(getKeywd(), "Failed to update record: '" + key + "' does not exist.");
            return key;
        }

        Rep(Rnum);
        return key;
    }

    /**
     * @brief Removes a specific record from the database collection physically or marks it as recycled.
     *
     * @param key       The unique key identifier string of the record earmarked for deletion.
     * @param err_if_no If true, triggers a system Error diagnostic if the target record was missing.
     */
    void deleteRecord(const std::string& key, bool err_if_no = true)
    {
        int Rnum = Find(key.c_str());
        if(Rnum >= 0) {
            Del(Rnum);
        }
        else if(err_if_no) {
            std::string msg = "Database access error: Record '" + key + "' not found for deletion!";
            Error(getKeywd(), msg);
        }
    }

    /**
     * @brief Prompts the user via a graphical interface dialog to confirm saving or overriding a record.
     *
     * Generates context-aware confirmation messaging and coordinates bulk transaction responses.
     *
     * @param[in]  window      Pointer to the parent QWidget UI component for modal rendering (Qt 6).
     * @param[in]  key         The unique record index lookup key being reviewed.
     * @param[out] quest_reply Reference populated with the chosen interactive option code.
     * @param[out] fnum        Reference populated with the selected target database file identifier.
     * @return true If the transaction is approved for execution, false if it was aborted or cancelled.
     */
    bool saveRecordQuestion(QWidget *window, const std::string& key, int &quest_reply, int &fnum);

    /**
     * @brief Interactively appends a new record using the provided key.
     * @param window Pointer to the parent QWidget interface context window.
     * @param key    The unique key string identifier for the new record.
     */
    void addRecord(QWidget *window, const std::string& key);

    /**
     * @brief Interactively appends a new record to a specific file tracking slot location.
     * @param window Pointer to the parent QWidget interface context window.
     * @param key    The unique key string identifier for the new record.
     * @param fnum   Reference to the integer variable containing or receiving the targeted file tracking number.
     */
    void addRecord(QWidget *window, const std::string& key, int &fnum);

    int Find( const char *key);
    RecStatus Rtest( const char *key, int mode = 1);
    bool FindPart( const char *key_, uint field );
    time_t GetTime(size_t record_ndx);               // time record number i
    time_t Rtime() const
    {
        return crt;
    }  // time current record

    size_t GetKeyList(const char *keypat, TCStringArray& aKey);
    void RenameList( const char* newName,
                    const char *oldName );


    // =========================================================================
    // --- Database File Manipulation Methods ---
    // =========================================================================

    /**
     * @brief Registers multiple database files in the system framework.
     * @param folder    The root directory path where the files are located.
     * @param path_list Collection of database file names or relative paths.
     */
    void addFiles(const std::string &folder, const std::vector<std::string> &path_list);

    /**
     * @brief Adds a completely new database file to the system and opens it.
     * @param path The absolute or relative file system path for the new database file.
     */
    void addNewFile(const std::string &path)
    {
        try {
            dbfiles_list.open_new_file(nRT2, path, ind);
        }
        catch (...) {
            Error(path, "Failed to initialize and open new database file structure.");
            throw;
        }
    }

    /**
     * @brief Creates a new project database file and closes all other active files.
     * @param dir          The target project destination directory.
     * @param project_name The unique name identifier of the new project.
     * @param fname        Optional specific layout filename.
     */
    void addProjectFile(const std::string& dir, const std::string& project_name, const std::string& fname = "");

    /**
     * @brief Removes specified database files from the system and updates operational indices.
     * @param path_list Collection of database file paths earmarked for removal.
     */
    void removeFiles(const std::vector<std::string>& path_list);

    /**
     * @brief Returns the zero-based index of the currently active database file.
     * @return size_t The active file index slot.
     */
    size_t activeFileIndex() const
    {
        return dbfiles_list.active_file_index();
    }

    /**
     * @brief Returns the relative file path of the currently active database file.
     * @return std::string The relative file path string.
     */
    std::string activeFileName() const
    {
        return dbfiles_list.active_file_relpath();
    }

    /**
     * @brief Checks whether any system-default or core database files are currently open.
     * @return true If core/default files are locked in the stream, false otherwise.
     */
    bool isDefaultOpened() const
    {
        return dbfiles_list.check_special_open();
    }

    /**
     * @brief Finds the tracking index of a file by its short name variant among open files.
     *
     * Automatically compounds the complete operational lookup name using the current system keyword mask.
     *
     * @param second_name The secondary suffix name of the database file.
     * @return size_t The valid file index, or std::string::npos if the file does not exist.
     */
    size_t indexOfName(const std::string &second_name)
    {
        std::string name = std::string(getKeywd()) + "." + second_name;
        size_t file_ndx = dbfiles_list.file_name_index(name);
        if (file_ndx == std::string::npos) {
            Error(name, "Database access error: Attempted to reference a non-existent file.");
            return std::string::npos;
        }
        return file_ndx;
    }

    /**
     * @brief Rebuilds, compresses, and defragments specified database files by recycling deleted blocks.
     * @param indices Vector collection of target file numeric indices to process.
     */
    void rebuildsFiles(const std::vector<int>& indices);

    /**
     * @brief Appends a collection of database files by their indices into the active open files set.
     * @param indices Vector tracking slot codes to be opened.
     */
    void openAdditionalFiles(const std::vector<int>& indices);

    /**
     * @brief Opens all registered database files matching the system workspace profile.
     * @param only_kernel If true, filters and restricts the load operation strictly to core/kernel files.
     */
    void openAllFiles(bool only_kernel = false);

    /**
     * @brief Restricts and opens only files whose names match a specific wildcard sub-pattern '.<name>.'.
     * @param names Collection of match pattern sub-strings.
     */
    void openOnlyWith(const std::vector<std::string>& names);

    /**
     * @brief Opens database files matching given functional system keywords.
     * @param keywords Collection of filtering system keyword strings.
     * @return true If all targeted files were successfully located and opened, false otherwise.
     */
    bool openOnlyWithKeywords(const std::vector<std::string>& keywords);

    /**
     * @brief Populates a vector with project file keywords containing a specific name fragment match.
     * @param name_part The sub-string pattern to scan for.
     * @param keywords  Output reference vector to store matching keyword results.
     */
    void getProjectFilesKeywds(const std::string& name_part, std::vector<std::string>& keywords)
    {
        keywords = dbfiles_list.file_keywds_according_name(name_part);
    }

    /**
     * @brief Retrieves the complete immutable collection of currently opened file names.
     * @param[out] active Output string populated with the relative path of the active file.
     * @return const std::vector<std::string>& Read-only reference to the internal files vector.
     */
    const std::vector<std::string>& getOpenFiles(std::string &active) const
    {
        active = activeFileName();
        return dbfiles_list.opened_files_list;
    }

    /**
     * @brief Evaluates and generates paths along with selection status matrices for GUI representations.
     *
     * @param mode    Operational selection filtering mode.
     * @param names   Output container for generated file layout name strings.
     * @param indices Output container for generated physical file tracking indices.
     * @param sel     Output selection state matrix flags (e.g., used to check rows in Qt views).
     */
    void selectionFilesList(int mode,
                            std::vector<std::string>& names,
                            std::vector<int>& indices,
                            std::vector<int>& sel);



    //--- Working with Json
    void toJsonObject( QJsonObject& obj ) const;
    std::string fromJsonObject( const QJsonObject& obj );
    void toJsonObjectNew(QJsonObject &obj, const std::string& project_name) const;
    std::string fromJsonObjectNew( const QJsonObject& obj );

    //--- Could be moved to db internal
    int GetObjFirst() const
    {
        return  frstOD;
    }
    unsigned char GetObjCount() const
    {
        return nOD;
    }


protected:
    void Get( int i);
    void Del( int i);
    void Rep( int i);
    void ReplaceRecordwithQuestion(QWidget *window, const std::string& key, int &quest_reply);


    /// Object count written to the record header. The extended-settings objects of the project
    /// record are not counted, so that older GEMSGUI versions still read the record.
    unsigned char headerObjCount() const;


    //--- check
    int reclen( );
    int putrec( RecEntry& re, GemDataStream& f, uint32_t crt );
    //int putrec( RecEntry& re, GemDataStream& f, RecHead& rhh );
    int getrec( RecEntry& re, GemDataStream& f, RecHead& rh );
    int getExtendedSettings( GemDataStream& f, int nRead, int StillLen );
    void opfils();
    int scanfile( uint nF, int& fPos, int& fLen,
	    GemDataStream& inStream, GemDataStream& outStream);
    void fromCFG(std::fstream& f);

    std::string gen_new_project_file_name(const std::string& dir,
                                          const std::string& project_name, const std::string& f_name) const;
    void check_record_key(const std::string& key);
    bool check_rt_name(const std::string &file_name) const;

    bool KeyTest( const char* key );
    std::string make_key_template(const std::vector<std::string>& first_fields) const;

};

// Data Base container : rt

class DataBaseList:
            public std::vector<std::shared_ptr<TDataBase>>
{
public:
    DataBaseList():
           std::vector<std::shared_ptr<TDataBase>>()
    { }

    ~DataBaseList();

    void Init();
    void fromCFG( std::fstream& f );
    void toCFG(std::fstream& f);

};

extern DataBaseList rt;

#endif	// V_DBM_H_


