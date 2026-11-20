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

#include <map>
#include <initializer_list>
#include <ctime>
#include "v_object.h"
#include "v_dbfile.h"

class GemDataStream;

extern const char* ALLKEY;

enum keyctrl {   // codes key bild
    K_END = -5, K_EMP, K_ANY, K_IMM, K_ACT
};

enum RecStatus {   // states of keys record Data Base
    UNDF_=-7 /* undefined state */, FAIL_=-1 /* access error */,
    NONE_=0 /* no records */, ONEF_ ,
    MANY_ , EMPC_, /* empty chain */
    /* NONELIST_ * no records to part */
};

enum modeList { openf = 0x01, closef = 0x02, oldself = 0x04 };

/**
 * @typedef keysmap_t
 * @brief Associative map linking an unpacked record key string to its physical file metadata.
 *
 * Maps `<unpacked record key>` to `<memory-resident index coordinates and sizing metadata (RecEntry)>`.
 */
using keysmap_t = std::map<std::string, RecEntry>;

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
     * @brief Returns a read-only reference to all sub-field values of the compound key.
     * @return const std::vector<std::string>& Vector containing the current string values for all key segments.
     */
    const std::vector<std::string>& allFields() const noexcept
    {
        return flds_value;
    }

    /**
     * @brief Unpacks, expands, and aligns a specific sub-field with blank spaces matching its specification.
     * @param ndx The zero-based index of the target sub-field.
     * @return std::string The space-padded unpacked field key value string.
     */
    std::string unpackFieldKey(size_t ndx) const
    {
        if(ndx >= flds_value.size()) {
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

/**
 * @class TDBKeyMap
 * @brief Manages the in-memory associative map of loaded database record keys and their file offsets.
 *
 * Provides fast logarithmic lookups, wildcard filtering, and binary index (.ndx) serialization
 * for active database records.
 */
class TDBKeyMap
{
    friend class TDataBase;

public:
    /**
     * @brief Constructs a key map manager with an explicit key field layout specification.
     *
     * @param nRkflds Number of sub-fields constituting the compound key.
     * @param rkfrm   Pointer to an array defining the maximum byte lengths for each sub-field.
     */
    TDBKeyMap(unsigned char nRkflds, const unsigned char* rkfrm)
        : current_key(nRkflds, rkfrm), key_record_map() {}

    /**
     * @brief Constructs and configures a key map manager directly from a configurator file stream.
     *
     * @param f Reference to the active configuration input stream.
     */
    explicit TDBKeyMap(std::fstream& f)
        : current_key(f), key_record_map() {}

    /**
     * @brief Virtual destructor to ensure proper cleanup of derived classes.
     */
    virtual ~TDBKeyMap() = default;

    // Disable copy semantics to prevent accidental duplication of large in-memory index maps
    TDBKeyMap(const TDBKeyMap&) = delete;
    TDBKeyMap& operator=(const TDBKeyMap&) = delete;

    /**
     * @brief Returns the total number of indexed records stored in the map.
     * @return size_t The exact count of active record entries.
     */
    size_t count() const noexcept
    {
        return key_record_map.size();
    }

    /**
     * @brief Checks whether the index key map contains no records.
     * @return true If the map is completely empty, false otherwise.
     */
    bool empty() const noexcept
    {
        return key_record_map.empty();
    }

    /**
     * @brief Resets the in-memory index key map and restores the default wildcard search key.
     */
    void initnew();

    /**
     * @brief Checks whether a specific unpacked key exists in the index map.
     * @param unpack_key The unpacked string key to look up.
     * @return true If the key exists in the map, false otherwise.
     */
    bool haskey(const std::string& unpack_key) const;

    /**
     * @brief Retrieves a pointer to the RecEntry associated with the given unpacked key.
     *
     * @param unpack_key The unpacked string key to search for.
     * @return const RecEntry* Pointer to the found entry, or nullptr if the key does not exist.
     */
    const RecEntry* findkey(const std::string& unpack_key) const;

    /**
     * @brief Retrieves a mutable pointer to the RecEntry associated with the given unpacked key.
     *
     * Useful when updating the physical position or length of an existing record in-place.
     *
     * @param unpack_key The unpacked string key to search for.
     * @return RecEntry* Mutable pointer to the found entry, or nullptr if not found.
     */
    RecEntry* findkey(const std::string& unpack_key);

    /**
     * @brief Inserts a new record entry or updates an existing one in the index map.
     * @param unpack_key The unpacked string key identifying the record.
     * @param entry      The RecEntry metadata (file position, length, file index) to store.
     */
    void putkey(const std::string& unpack_key, const RecEntry& entry);

    /**
     * @brief Removes a single record index entry from the map by its unpacked key.
     * @param unpack_key The unpacked string key identifying the record to remove.
     */
    bool delkey(const std::string& unpack_key);

    /**
     * @brief Removes all record index entries belonging to a specific database file.
     * @param nF The database file index whose record keys should be purged from memory.
     */
    void delfile(size_t nF);

    /**
     * @brief Serializes all index keys belonging to a specific database file into a binary index stream (.ndx).
     * @param nF        The database file index to filter records by.
     * @param f         Output binary stream for the .ndx file.
     * @param file_name File path used for error reporting diagnostics.
     */
    void save_keys_ndxs(size_t nF, GemDataStream& f, const std::string& file_name) const;

    /**
     * @brief Deserializes record index entries from a binary index stream (.ndx) into the in-memory map.
     * @param nF        The database file index assigned to all loaded records.
     * @param nRec      Total number of records expected in the index file.
     * @param f         Input binary stream for the .ndx file.
     * @param file_name File path used for error reporting diagnostics.
     */
    void get_keys_ndxs(size_t nF, size_t nRec, GemDataStream& f, const std::string& file_name);

    /**
     * @brief Filters all indexed records matching the provided wildcard key template.
     *
     * Parses the incoming pattern into field segments and evaluates every entry in the
     * index map, returning a standalone vector of matched unpacked keys.
     *
     * @param keypat Wildcard pattern string (e.g., "aq:*:H2O*:*").
     * @return std::vector<std::string> A vector containing all matched unpacked keys.
     */
    std::vector<std::string> xlist(const std::string& keypat) const;

protected:

    ///Current (last accessed) database record key
    TDBKeyNew current_key;
    /// List all loaded keys for database (internal loaded data from opened files)
    keysmap_t key_record_map;

    /**
     * @brief Checks if a fixed-width unpacked record key matches a parsed wildcard key template field-by-field.
     *
     * @param unpacked_key The fixed-width unpacked key stored in key_record_map.
     * @param pattern_key  The parsed TDBKeyNew object representing the search template.
     * @return true        If every sub-field matches its corresponding wildcard pattern.
     */
    bool match_unpacked_key(std::string_view unpacked_key, const TDBKeyNew& pattern_key) const noexcept;
};

//-------------------------------------------------------------

/**
 * @class TDataBase
 * @brief High-level manager for a structured database chain, coordinating record CRUD operations,
 *        in-memory key indexing, and multi-file binary storage (.pdb / .ndx).
 */
class TDataBase
{
    friend TDBFile;

public:

    /**
     * @brief Checks whether the given key string contains wildcard or path-separator pattern characters.
     *
     * Evaluates the input key view in-place without memory allocations for the presence
     * of '*', '?', or '/' characters.
     *
     * @param key The key string view to evaluate.
     * @return true If the key contains at least one wildcard/pattern character, false otherwise.
    */
    static constexpr bool isPattern(std::string_view key) noexcept
    {
        return key.find_first_of("*?/") != std::string_view::npos;
    }

    /**
     * @brief Constructs and initializes a new database chain manager with explicit parameters.
     *
     * @param rt_type  Record type identifier for this database chain.
     * @param name     Keyword/name identifier of the database chain.
     * @param Rclose   If true, automatically closes files after each read/write transaction.
     * @param isDel    If true, enables deleted records tracking and space recycling (sfe).
     * @param nOf      Index of the first data object in the global object list.
     * @param Nobj     Total number of data objects packed in a single record.
     * @param filesNum Initial capacity or number of files in the database chain.
     * @param nRkflds  Number of sub-fields constituting the compound record key.
     * @param rkfrm    Pointer to an array defining maximum byte lengths for each key sub-field.
     */
    TDataBase(size_t rt_type, std::string_view name, bool Rclose, bool isDel,
              int32_t nOf, unsigned char Nobj, int32_t filesNum,
              unsigned char nRkflds, const unsigned char* rkfrm);

    /**
     * @brief Constructs and restores the database chain configuration from an active file stream.
     * @param f Reference to the input configuration file stream.
     */
    explicit TDataBase(std::fstream& f);

    /**
     * @brief Virtual destructor ensuring proper resource cleanup for derived classes.
     */
    virtual ~TDataBase();

    // Disable copy semantics to prevent accidental duplication of active file handles and index maps
    TDataBase(const TDataBase&) = delete;
    TDataBase& operator=(const TDataBase&) = delete;

    /**
     * @brief Serializes the current database configuration state to a configuration file stream.
     * @param f Reference to the output configuration file stream.
     */
    void toCFG(std::fstream& f) const;

    /// Sets the number of objects of the record (cached configurations may hold an older count).
    void setObjectCount(unsigned char n)
    {
        if(lastODjson == frstODjson+nOD-1) {
            lastODjson = frstODjson+n-1;
        }
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

    /**
     * @brief Opens a specific subset of database files and automatically triggers defragmentation if required.
     *
     * @param mode      The target file status/mode to switch to (e.g., READ, WRITE, UPDATE_DBV).
     * @param files_ndx Vector collection containing the numeric tracking indices of the files to be opened.
    */
    void openDatabase(FileStatus mode, const std::vector<int>& files_ndx);

    /**
     * @brief Closes all currently opened database files and resets the active index collections.
    */
    void closeDatabase();


    //--- Selectors
    RecStatus GetStatus() const
    {
        return status;
    }
    void SetStatus( RecStatus stt )
    {
        status = stt;
    }
    size_t RecCount() const
    {
        return keys_map.count();
    }
    std::string PackKey()
    {
        return keys_map.current_key.packKey();
    }
    std::string UnpackKey()
    {
        return keys_map.current_key.unpackKey();
    }
    void SetKey(const std::string& key)
    {
        keys_map.current_key.setKey(key);
    }
    /**
     * @brief Safely overrides the literal value of a single specific sub-field slot.
     * @param ndx     The zero-based index of the target sub-field slot.
     * @param new_fld The incoming new string value for the specified field.
     */
    void setFieldKey(size_t ndx, const std::string& new_fld)
    {
        keys_map.current_key.setFieldKey(ndx, new_fld);
    }


    const TDBKeyNew& GetDBKey() const
    {
        return keys_map.current_key;
    }
    size_t KeyLen() const
    {
        return keys_map.current_key.length();
    }
    std::string FldKey(size_t i) const
    {
        return keys_map.current_key.fieldKey(i);
    }
    size_t FldLen(size_t i) const
    {
        return keys_map.current_key.fieldLength(i);
    }
    size_t KeyNumFlds() const
    {
        return keys_map.current_key.fieldsInKey();
    }

    /**
     * @brief Splits a composite record key string into its individual sub-field segments.
     *
     * @param key The composite packed or unpacked record key string to split.
     * @return std::vector<std::string> A vector of individual key sub-field values.
     */
    std::vector<std::string> key2fields(const std::string& key) const
    {
        TDBKeyNew temp_key = keys_map.current_key;
        temp_key.setKey(key);
        return temp_key.allFields();
    }

    /**
     * @brief Assembles a composite record key string from a vector of individual sub-field values.
     *
     * @param fields A vector containing the string values for the key sub-fields.
     * @param packed If true (default), returns a colon-delimited packed key; otherwise, returns a space-padded unpacked key.
     * @return std::string The assembled composite record key.
     */
    std::string fields2key(const std::vector<std::string>& fields, bool packed = true) const;

    //void MakeKey( unsigned char nRtwrk, char *pkey, ...);

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
    std::string makeKey(uint8_t nrt, std::initializer_list<KeyArg> args) const
    {
        return keys_map.current_key.makeKey(nrt, args);
    }

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
        TDBKeyNew temp_key = keys_map.current_key;
        temp_key.setKey(key);
        const std::string unpack_key = temp_key.unpackKey();
        return keys_map.haskey(unpack_key);
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
    void readRecord(const std::string& key);

    /**
     * @brief Updates the payload content of an existing record matched by its key.
     *
     * @param key The unique key identifier the record to be modified.
     * @return std::string The validated record identifier string.
     */
    std::string updateRecord(const std::string& key);

    /**
     * @brief Removes a specific record from the database collection physically or marks it as recycled.
     *
     * @param key       The unique key identifier string of the record earmarked for deletion.
     * @param err_if_no If true, triggers a system Error diagnostic if the target record was missing.
     */
    void deleteRecord(const std::string& key, bool err_if_no = true);

    /**
     * @brief Retrieves the creation or last modification timestamp of a specific record.
     * @param key The unique packed or unpacked key string identifying the target record.
     * @return std::time_t The record timestamp, or 0 if the record does not exist in the index map.
     */
    std::time_t getTime(const std::string& key);

    /**
     * @brief Returns the creation or last modification timestamp of the currently loaded record.
     * @return std::time_t The cached timestamp value (crt_time).
     */
    std::time_t Rtime() const noexcept
    {
        return crt_time;
    }

    /**
     * @brief Tests the existence and multiplicity state of records matching the given key or wildcard pattern.
     *
     * @param pkey The packed or unpacked record key or wildcard pattern to test.
     * @param mode Operational mode: if 1 and exactly one record matches, automatically calls readRecord().
     * @return RecStatus The resulting record state (EMPC_, NONE_, ONEF_, MANY_, or UNDF_).
     */
    RecStatus Rtest(const std::string& pkey, int mode);

    /**
     * @brief Replaces a specific field segment in the given record key with a wildcard ("*").
     * @param pkey  The source packed or unpacked record key string.
     * @param field The zero-based index of the key field to replace with "*".
     * @return std::string The modified unpacked key pattern.
     */
    std::string patternToField(const std::string& pkey, size_t field) const;

    /**
     * @brief Checks if any records match the given key when a specific field segment is wildcarded.
     *
     * @note Updates the internal database access status as a side effect of calling Rtest().
     *
     * @param pkey  The source packed or unpacked record key string.
     * @param field The zero-based index of the key field to ignore (replace with "*").
     * @return true If at least one matching record exists (status is ONEF_ or MANY_), false otherwise.
     */
    bool findPart(const std::string& pkey, size_t field);

    /**
     * @brief Generates a filtered list of unique record keys matching a specified wildcard layout pattern.
     *
     * Scans the active compound index tree using the provided template pattern (e.g., containing '*' or '?'),
     * populates the output vector with all matched valid string keys, and returns the total collection size.
     *
     * @param[in]  keypat The wildcard lookup pattern or key template used to filter records.
     * @param[out] keys   Output reference vector to be populated with the generated matching key strings.
     * @return size_t The exact total count of unique keys loaded into the output vector container.
    */
    size_t getKeysList(const std::string& keypat, std::vector<std::string>& keys)
    {
        keys = keys_map.xlist(keypat);
        return keys.size();
    }

    /**
     * @brief Batch-renames records by updating strictly the first field of all keys matching oldName.
     *
     * @param newName The new identifier string for the first key field.
     * @param oldName The existing first-field identifier to search for and replace.
     */
    void renameList(const std::string& newName, const std::string& oldName);

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
        dbfiles_list.open_new_file(record_type, path, keys_map);
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
    size_t indexOfName(const std::string &second_name);

    /**
     * @brief Finds the tracking index of a file by its relative path among open files.
     *
     * @param relative_path The relative file system path of the database file to look up.
     * @return size_t The valid zero-based file index, or std::string::npos if the file is not found.
     */
    size_t indexOfPath(const std::string& relative_path) const
    {
        return dbfiles_list.file_path_index(relative_path);
    }

    /**
     * @brief Returns the total number of database files registered in the system list.
     * @return size_t The exact count of files managed by dbfiles_list.
     */
    size_t filesCount() const noexcept
    {
        return dbfiles_list.all_dbfiles.size();
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
                            std::vector<int>& sel)
    {
        return dbfiles_list.files_list(mode, names, indices, sel);
    }

    //--- Working with Json
    void toJsonObject(QJsonObject& obj) const;
    std::string fromJsonObject(const QJsonObject& obj);
    void toJsonObjectNew(QJsonObject &obj, const std::string& project_name) const;
    std::string fromJsonObjectNew( const QJsonObject& obj );

    /**
     * @brief Unloads (exports) a specific database record and its nested objects into a formatted text stream.
     *
     * Retrieves the record payload into the system buffers, synchronizes the record key pointer,
     * and sequentially serializes all associated chemical or thermodynamic data objects into the text output.
     *
     * @param[in] key The unique key string identifier of the record to export.
     * @param[out] f  The destination target text output stream (e.g., std::ofstream or std::stringstream).
    */
    void recordToTxt(const std::string& key, std::ostream& f);
    /**
     * @brief Loads (imports) data records and nested sub-objects configuration from a formatted text stream.
     *
     * Dynamically allocates a type-safe input buffer to avoid stack smashing, parses the record identifiers
     * along with its serialized sub-components, and returns the safely extracted and validated record key.
     *
     * @param[in] f   The source text input stream (e.g., std::ifstream or std::stringstream).
     * @return std::string The successfully parsed, formatted, and null-terminated record key string.
     */
    std::string recordFromTxt(std::istream& f);

protected:
    /// Keyword identifier of the database chain type (zero-initialized)
    char Keywd[MAXKEYWD]{};

    /// Record type code associated with this database chain
    unsigned char record_type = 0;

    /// First object index in the global object list for this chain
    int32_t frstOD = 0;

    /// Number of data objects stored within a single record
    unsigned char nOD = 0;

    /// First object index used during JSON serialization/export
    int32_t frstODjson = 0;

    /// Last object index used during JSON serialization/export
    int32_t lastODjson = 0;

    /// In-memory associative map linking unpacked record keys to their physical file coordinates
    TDBKeyMap keys_map;

    /// Collection manager for low-level binary database files (.pdb) and index tables (.ndx)
    TDBFileList dbfiles_list;

    // --- Runtime Operational Parameters ---

    /// Status flag reflecting the outcome of the last record access operation
    RecStatus status{};

    /// Creation or last modification timestamp of the last accessed record
    std::time_t crt_time = 0;

    void fromCFG(std::fstream& f);

    int reclen();
    int putrec(const RecEntry* re, GemDataStream& f, uint32_t crt);
    int getrec(const RecEntry* re, GemDataStream& f, RecHead& rh);
    int scanfile(size_t nF, int32_t& fPos, int32_t& fLen,
                            GemDataStream& inStream, GemDataStream& outStream);

    /// Object count written to the record header. The extended-settings objects of the project
    /// record are not counted, so that older GEMSGUI versions still read the record.
    unsigned char headerObjCount() const;
    int getExtendedSettings( GemDataStream& f, int nRead, int StillLen );

    std::string make_key_template(const std::vector<std::string>& first_fields) const;
    std::string gen_new_project_file_name(const std::string& dir,
                                          const std::string& project_name,
                                          const std::string& f_name) const;
    void check_record_key(const std::string& key);
    bool check_rt_name(const std::string &file_name) const;

    void ReplaceRecordwithQuestion(QWidget *window, const std::string& key, int &quest_reply);
};

/**
 * @class DataBaseList
 * @brief Container managing the collection of active database chains (commonly accessed as 'rt').
 *
 * Encapsulates a vector of shared pointers to TDataBase instances, providing direct indexed access,
 * standard C++20 range-based iteration, and configuration file serialization.
 */
class DataBaseList
{
public:
    using container_type = std::vector<std::shared_ptr<TDataBase>>;
    using iterator = container_type::iterator;
    using const_iterator = container_type::const_iterator;

    /**
     * @brief Default constructor initializing an empty database collection.
     */
    DataBaseList() = default;

    /**
     * @brief Virtual destructor.
     */
    virtual ~DataBaseList() = default;

    /**
     * @brief Provides read-only indexed access to a database chain (no bounds checking).
     * @param index Zero-based index of the database chain (e.g., RT_SYSEQ, RT_PARAM).
     * @return const std::shared_ptr<TDataBase>& Const reference to the database pointer.
     */
    const std::shared_ptr<TDataBase>& operator[](size_t index) const noexcept
    {
        return all_databases[index];
    }

    /**
     * @brief Provides mutable indexed access to a database chain (no bounds checking).
     * @param index Zero-based index of the database chain.
     * @return std::shared_ptr<TDataBase>& Reference to the database pointer.
     */
    std::shared_ptr<TDataBase>& operator[](size_t index) noexcept
    {
        return all_databases[index];
    }
    /**
     * @brief Appends a shared pointer to a database chain to the end of the container.
     * @param db Const reference to the shared pointer to append.
     */
    void push_back(const std::shared_ptr<TDataBase>& db)
    {
        all_databases.push_back(db);
    }

    /**
     * @brief Moves a shared pointer to a database chain to the end of the container.
     * @param db Rvalue reference to the shared pointer to move (e.g., from std::make_shared).
     */
    void push_back(std::shared_ptr<TDataBase>&& db)
    {
        all_databases.push_back(std::move(db));
    }

    /**
     * @brief Returns a mutable reference to the last database chain in the container.
     * @return std::shared_ptr<TDataBase>& Reference to the last element.
     */
    std::shared_ptr<TDataBase>& back() noexcept
    {
        return all_databases.back();
    }

    /**
     * @brief Returns a read-only reference to the last database chain in the container.
     * @return const std::shared_ptr<TDataBase>& Const reference to the last element.
     */
    const std::shared_ptr<TDataBase>& back() const noexcept
    {
        return all_databases.back();
    }

    // =========================================================================
    // --- Standard Iterators (enables range-based 'for' & std::ranges) ---
    // =========================================================================

    iterator begin() noexcept { return all_databases.begin(); }
    iterator end() noexcept { return all_databases.end(); }

    const_iterator begin() const noexcept { return all_databases.begin(); }
    const_iterator end() const noexcept { return all_databases.end(); }

    const_iterator cbegin() const noexcept { return all_databases.cbegin(); }
    const_iterator cend() const noexcept { return all_databases.cend(); }

    /**
     * @brief Returns the total number of database chains registered in the container.
     * @return size_t The exact count of TDataBase entries.
     */
    size_t size() const noexcept { return all_databases.size(); }

    /**
     * @brief Checks whether the database container is empty.
     * @return true If no database chains are registered, false otherwise.
     */
    bool empty() const noexcept { return all_databases.empty(); }

    /**
     * @brief Removes all database chains from the container, releasing their shared pointers.
     */
    void clear() noexcept
    {
        all_databases.clear();
    }


    // =========================================================================
    // --- Lifecycle & Configuration Serialization ---
    // =========================================================================

    /**
     * @brief Initializes and populates the default set of system database chains.
     */
    void Init();

    /**
     * @brief Restores the database chains collection from an input configuration file stream.
     * @param f Reference to the input configuration file stream.
     */
    void fromCFG(std::fstream& f);

    /**
     * @brief Serializes the current database chains collection into an output configuration file stream.
     * @param f Reference to the output configuration file stream.
     */
    void toCFG(std::fstream& f) const;

protected:
    /// Underlying storage holding shared pointers to all active database chains
    container_type all_databases;
};


extern DataBaseList rt;

#endif	// V_DBM_H_



