//-------------------------------------------------------------------
// $Id$
//
// Declaration of TDBFile and TDBFileList classes
//
// Copyright (C) 1996-2026  S.Dmytriyeva, D.Kulik
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
//
#ifndef V_DBFILE_H_
#define V_DBFILE_H_

#include <cstddef>
#include <set>
#include "v_file.h"
#include "GEMS3K/verror.h"

const int MAXFESTACK = 200; // size array deleted record

extern const char* MARKRECDEL;
extern const char* MARKRECHEAD;
extern const char MARKRKEY;
extern const char* ALLKEY;

class TDBKeyList;
class TDataBase;

#pragma pack(push, 1) // Enforces tight byte-packing to eliminate compiler-inserted structure padding

/**
 * @struct VDBhead
 * @brief Represents the master control header of a binary database file (.pdb).
 *
 * Contains critical file metadata, including validation credentials, record counters,
 * file geometric layouts/offsets, and the current operational state of the space recycling system.
 */
struct VDBhead
{
    char VerP[8];           ///< Database engine compatibility version string (exactly 8 bytes)
    char PassWd[8];         ///< Database security access password encrypted string (exactly 8 bytes)
    char Date[11];          ///< File creation or last modification date string (formatted as DD/MM/YYYY)
    char Time[5];           ///< File creation or last modification time string (formatted as HH:MM)
    int32_t nRT;            ///< Core operational mode code or identifier of the database chain
    int32_t nRec;           ///< Total number of active physical records currently stored in the file
    int32_t stacOver;       ///< Overflow indicator or allocation ceiling offset for the deleted blocks stack
    int32_t FPosTRT;        ///< Physical stream offset marking the start of the records or relation tables payload
    char isDel;             ///< Configuration flag: 1 if deleted records tracking is active, 0 otherwise
    int32_t MinDrLen;       ///< Minimum payload size among all currently tracked recycled blocks
    int32_t MaxDrLen;       ///< Maximum payload size among all currently tracked recycled blocks
    int32_t curDr;          ///< Current count of active recycled slots occupied in the deleted blocks array

    void read(GemDataStream& is);
    void write(GemDataStream& os);

    /**
     * @brief Computes the precise binary signature footprint size of the header layout on the disk.
     * @return The exact size of the packed structure (strictly 61 bytes).
     */
    static constexpr size_t data_size()
    {
        return sizeof(VerP) + sizeof(PassWd) + sizeof(Date) + sizeof(Time) +
               sizeof(nRT) + sizeof(nRec) + sizeof(stacOver) + sizeof(FPosTRT) +
               sizeof(isDel) + sizeof(MinDrLen) + sizeof(MaxDrLen) + sizeof(curDr);
    }
};

/**
 * @struct DBentry
 * @brief Represents a single recycled space allocation slot (sfe element) on the disk.
 *
 * Tracks the exact physical location and raw bounds of a previously deleted record block
 * so that the space can be safely matched and reused for subsequent write transactions.
 */
struct DBentry
{
    int32_t pos;            ///< Absolute file stream pointer location of the recycled block
    int32_t len;            ///< Full raw physical span of the recycled block (including its binary header)

    void read(GemDataStream& is);
    void write(GemDataStream& os);

    /**
     * @brief Computes the binary size of a single recycling descriptor slot.
     * @return The exact size on disk (strictly 8 bytes).
     */
    static constexpr size_t data_size()
    {
        return sizeof(pos) + sizeof(len);
    }
};

/**
 * @struct RecEntry
 * @brief Represents memory-resident index coordinates and sizing metadata for an active record.
 *
 * Serves as a high-level tracking descriptor used within caching components, memory indices,
 * and cross-reference table collections to locate data without performing a disk scan.
 */
struct RecEntry
{
    int32_t pos;            ///< Absolute physical stream address allocation within the database file
    int32_t len;            ///< Pure payload chunk size (excluding the overhead of the record binary header)
    unsigned char nFile;    ///< File identification tracking code within the global database manager list

    void read(GemDataStream& is);
    void write(GemDataStream& os);

    /**
     * @brief Computes the serial footprint size of the in-memory coordinate entry block.
     * @return The structural index footprint size (strictly 9 bytes).
     */
    static constexpr size_t data_size()
    {
        return sizeof(pos) + sizeof(len) + sizeof(nFile);
    }
};

/**
 * @struct RecHead
 * @brief Represents the binary header preceding every data record inside the database file (.pdb).
 *
 * Stores validation markers, record formatting metadata, total physical length,
 * and structural creation/modification timestamps.
 */
struct RecHead
{
    char bgm[2];          ///< Record beginning marker flags (exactly 2 bytes, e.g., MARKRECHEAD)
    unsigned char nRT;    ///< Core record type or relation table identifier code
    unsigned char Nobj;   ///< Total number of objects or data components packed inside this record
    int32_t rlen;         ///< Full physical length of the record on disk, including this header (strictly 4 bytes)
    uint32_t time_crt;    ///< Unsigned 32-bit creation/update timestamp (safe until the year 2106, strictly 4 bytes)
    char endm[2];         ///< Record ending marker flags (exactly 2 bytes, e.g., MARKRECHEAD)

    void read(GemDataStream& is);
    void write(GemDataStream& os);

    /**
     * @brief Computes the precise binary footprint size of the record header layout on the disk.
     * @return The exact size of the packed structure (strictly 14 bytes).
     */
    static constexpr size_t data_size()
    {
        return sizeof(bgm) + sizeof(nRT) + sizeof(Nobj) + sizeof(rlen) + sizeof(time_crt) + sizeof(endm);
    }
};

#pragma pack(pop) // Restores the default compiler structure alignment rules


/**
 * @class TDBFile
 * @brief Manages low-level binary database file structure operations (.pdb) coupled with index tables (.ndx).
 *
 * Implements core database engine operations including creation, transactional I/O, file synchronization,
 * defragmentation, and deleted blocks recycling (sfe tracking mechanism).
 */
class TDBFile : public TBinaryFile
{

public:
    /**
     * @brief Constructs a TDBFile manager instance.
     * @param path Optional physical or relative path to the database file.
     */
    explicit TDBFile(const std::string& path = "", const std::string& the_relative="");

    TDBFile(const TDBFile&) = delete;
    const TDBFile& operator=(const TDBFile&) = delete;

    /**
     * @brief Creates a new database file on the disk.
     *
     * If a file with the same name already exists at the target path, it will be
     * overwritten (deleted and re-created). Initializes a fresh control structure
     * in memory and flushes the initial file header and empty deleted records stack to disk.
     *
     * @param nRT    Recort type.
     * @param isDel  Flag to enable or disable the deleted records tracking system (sfe).
     */
    void createFile(int nRT, bool isDel);

    /**
     * @brief Opens the database file in a special operational mode.
     *
     * Manages the transition of file states. If the file is already open in a different
     * mode, it closes the file first. After a successful low-level open operation,
     * it automatically reads and restores the database control structures (file header).
     *
     * @param mode The target file status/mode to switch to (e.g., UPDATE_DBV, RDONLY_DBV).
     */
    void openFile(FileStatus mode) override;

    /**
     * @brief Closes the database file and releases all allocated memory resources.
     *
     * Safely deallocates the dynamic memory holding the database file header and
     * the deleted records stack to prevent memory leaks. Resets the free position
     * tracker and delegates the final file closure to the base binary file class.
     */
    void closeFile() override;

    /**
     * @brief Creates a new database file along with its corresponding index file if it does not exist.
     *
     * Checks for the physical existence of the file. If absent, it triggers the creation of
     * the main database file (.pdb) and immediately initializes and writes the associated
     * index key list file (.ndx).
     *
     * @param nRT    Records type.
     * @param isDel  Flag to enable or disable the deleted records tracking system (sfe).
     * @param nF     Database file index within the global file list.
     * @param ind    Reference to the constant TDBKeyList containing the index structure to be saved.
     */
    void create_with_ndx(int nRT, bool isDel, size_t nF, const TDBKeyList& ind);

    /**
     * @brief High-level method to open a database file, load its indexes, and handle stack overflow.
     *
     * Configures connection parameters and automatically restores the key index list.
     * If a stack overflow is detected in the deleted records system, it prompts the user for compression.
     *
     * @param mode     The operational file status/mode to connect with (e.g., UPDATE_DBV, RDONLY_DBV).
     * @param rclose   If true, automatically closes the physical file immediately after loading indexes.
     * @param nF       Database file index within the global file list.
     * @param ind      Output reference to populate with the loaded TDBKeyList.
     *
     * @return true If the database index/file structure requires a rebuild/compression.
     * @return false If the database file is healthy and ready for operations.
     */
    bool open_file(FileStatus mode, bool rclose, size_t nF, TDBKeyList &ind);

    /**
     * @brief Saves or updates a database record, manages deleted blocks allocation, and synchronizes indexes.
     *
     * If updating an existing record, the old physical space is moved to the deleted blocks stack (sfe).
     * Calculates the new required length (payload + key data + record header), allocates space
     * via find_sfe, writes the record, updates the file header counter, and flushes index tables (.ndx).
     *
     * @param new_record True if this is a completely new insertion; false if it overwrites an existing record.
     * @param rh         Pointer to the RecEntry structure holding the record geometry and file placement.
     * @param database   Pointer to the TDataBase context handling record packing and writing logic.
     */
    void save_record_to_file(bool new_record, RecEntry *rh, TDataBase* database);

    /**
     * @brief Deletes a record from the database file physically and updates index tables.
     *
     * Creates a safe local copy of the record entry, factors in the record header size,
     * and pushes the freed block into the deleted records stack (sfe) for future recycling.
     * Decrements the total record counter in the file header, updates the index file (.ndx),
     * and handles the auto-close (rclose) transaction lifecycle.
     *
     * @param rh       Pointer to the RecEntry structure representing the record to be deleted.
     * @param database Pointer to the active TDataBase context containing the index tables.
     */
    void delete_record_from_file(RecEntry *rh, TDataBase *database);

    /**
     * @brief Reads a record payload from the physical database file and unpacks it into memory.
     *
     * Automatically ensures the database file stream is active, positions the stream pointer,
     * reads the record header along with the data via the TDataBase context, and handles
     * the transaction lifecycle according to the auto-close (rclose) flag.
     *
     * @param rh       Pointer to the RecEntry structure holding the record's physical file position and geometry.
     * @param database Pointer to the active TDataBase context that executes the record unpacking logic.
     */
    void read_record_from_file(RecEntry *rh, TDataBase *database);

    /**
     * @brief Reads the last update/creation timestamp of a specific record from the database file.
     *
     * Positions the stream pointer at the record's physical address, verifies the formatting
     * markers of the record header, and extracts the time_t value. Respects the auto-close (rclose) flag.
     *
     * @param ree Pointer to the RecEntry containing the physical file position of the target record.
     * @return time_t The timestamp (crt) stored inside the record header.
     */
    time_t get_time(RecEntry *ree);

    /**
     * @brief Rebuilds (compresses/defragments) the database file by removing all deleted records.
     *
     * Scans the active database file, copies healthy records into a temporary file (.tmp),
     * physically replaces the original file with the defragmented one using cross-platform filesystem utilities,
     * updates the compressed file structures, and refreshes the indexes.
     *
     * @param nF       Database file index within the global file list.
     * @param database Pointer to the active TDataBase context executing the physical file scanning.
     */
    void rebuild_file(size_t nF, TDataBase *database);

protected:
    /// Current length of the physical file on the disk (free position append offset)
    int FPosFree;

    /// Pointer to the allocated database file header structure
    VDBhead *dh;

    /// Pointer to the allocated deleted records stack array (reusable blocks list)
    DBentry *sfe;

    /**
     * @brief Current operational mode/status for the active database connection.
     *
     * Stores the file status (such as UPDATE_DBV, RDONLY_DBV, or NOT_OPEN) reflecting
     * how the physical database file was opened.
     */
    FileStatus connect_db_mode = UPDATE_DBV;

    /**
     * @brief Flag to automatically close the database file after a read or write operation.
     *
     * If set to true, the file stream will be immediately closed upon completing the current
     * transaction to ensure data persistence and release system resource locks.
     */
    bool rclose = true;

    // --- Internal Sanity Checks ---
    /// Validates if the sfe array memory is correctly allocated.
    void check_sfe();
    /// Validates if the file header structure memory is correctly allocated.
    void check_dh();

    // --- Header & Memory Synchronization ---
    /// Stamps the current system date and time into the file header structure.
    void vdbh_setdt();
    /// Resets header properties and fills the sfe stack array with clean zero blocks.
    void clear_dh_sfe();
    /// Prepares a fresh header layout in the memory before file initialization.
    void initialize_header(int nRT, bool if_del);
    /// Forces truncation/expansion and synchronizes memory structures directly to disk.
    void force_update_header(int fLen, int nRec, int nRT, char is_del);

    // --- Manipulation of Deleted Blocks (Space Recycling System) ---
    /// Adds a freed record's spatial metrics to the sfe stack and marks it as deleted in the stream.
    void add_sfe(RecEntry& re);
    /// Searches for a reusable block matching the required payload size using an optimized strategy.
    void find_sfe(RecEntry& re);
    /// Fallback strategy to allocate new storage space at the physical end of the file.
    void allocate_new_block(RecEntry &re);

    // --- Low-Level Stream Manipulation ---
    /// Flushes the active header to disk and modifies the total active record count by a delta.
    void put_head(int delta_rec);
    /// Parses the file stream from position zero to reconstruct header and sfe metadata arrays.
    void get_head();

    // --- Indexing and File Extensions System (.ndx) ---
    /// Cross-platform utility that returns the active file path with its extension changed to .ndx
    std::string index_file_name() const;
    /// Serializes and writes the complete active key indexing list table to the .ndx file.
    void put_ndx(size_t nF, const TDBKeyList &ind);
    /// De-serializes and reads the complete key indexing list table from the .ndx file.
    void get_ndx(size_t nF, TDBKeyList &ind);
};

/**
 * @class TDBFileList
 * @brief Manages the list of low-level binary database file structure operations (.pdb) coupled with index tables (.ndx).
 *
 * Supports managing an open file set.
 */
class TDBFileList
{
    friend class TDataBase;

public:

    /**
     * @brief If set to true, allows write access to system database files in DB.default (for developers only).
     */
    static bool dbChangeMode;

    /**
     * @brief Constructor initializing the database file list configuration.
     * @param need_close Sets the auto_close flag to manage aggressive resource unlocking.
     * @param isdel Sets the reuse_deleted flag to enable/disable the sfe tracking mechanism.
     */
    explicit TDBFileList(bool need_close, bool isdel):
        all_dbfiles(),
        reuse_deleted(isdel),
        auto_close(need_close)
    { }

    virtual ~TDBFileList();

    /// Fast elements access (No bounds checking, strict performance)
    const std::shared_ptr<TDBFile>& operator[](size_t index) const { return all_dbfiles[index]; }
    std::shared_ptr<TDBFile>& operator[](size_t index) { return all_dbfiles[index]; }

    /**
     * @brief Reads and loads configuration from a file stream.
     * @param f Input file stream.
     */
    void from_cfg(std::fstream& f);
    /**
     * @brief Saves and writes current configuration to a file stream.
     * @param f Output file stream.
     */
    void to_cfg(std::fstream& f);

    /// Check if the file index is within the valid vector range
    bool check_index(size_t file_ndx) const
    {
        return file_ndx<all_dbfiles.size();
    }

    /// Check if the file is system-protected/readonly
    bool check_readonly(size_t file_ndx) const
    {
        return !dbChangeMode && file_ndx<special_files_num;
    }

    /// Check if the file is currently marked as open
    bool check_open(size_t file_ndx) const
    {
        return opened_files.find(file_ndx)!=opened_files.end();
    }

    /// Validates file index and open status. Throws/logs errors on failure.
    void check_file(size_t file_ndx)
    {
        if(!check_index(file_ndx)) {
            std::string max_valid = (all_dbfiles.size() > 0) ? std::to_string(all_dbfiles.size() - 1) : "0";
            Error("check_file", "Illegal DB file index: " + std::to_string(file_ndx) +
                                    ". Valid range is 0 to " + max_valid + ".");
        }
        else if(!check_open(file_ndx)) {
            Error("check_file", "Attempted to access a database file that is not open. File path: " +
                                    all_dbfiles[file_ndx]->relativePath());
        }
        last_used_file = file_ndx;
    }

    /// Check if the file can be modified for a specific operation
    bool check_writable(size_t nF, const std::string& operation) const
    {
        if(check_readonly(nF)) {
            std::string error_msg = "Cannot " + operation +
                                    ": modifying a system database file is not allowed (read-only mode enabled).";
            Error(all_dbfiles[nF]->relativePath(), error_msg);
            return false;
        }
        return true;
    }

    // --- (File Registration) ---

    /// Register a database file in the system.
    void add_file(const std::string &folder, const std::string &path);

    /// Register multiple database files in the system.
    void add_files(const std::string &folder, const std::vector<std::string> &path_list);

    // --- (Open File Management) ---

    /**
     * @brief Open database files with specified indices for a specific record type.
     *
     * @param record_type   The identifier representing the specific type of database records (Record Type ID).
     * @param mode          The file access status mode (UPDATE_DBV, RDONLY_DBV).
     * @param nff           A list of file indices target files to be opened.
     * @param ind           Reference to the active key/index table structure to load records into.
     * @return std::vector<size_t> A list of indices of the files that requires a rebuild/compression.
     */
    std::vector<int> open_files(int record_type, FileStatus mode, const std::vector<int> &nff, TDBKeyList &ind);

    /**
     * @brief Add files with specified indices to the set of open files for a specific record type.
     *
     * @param record_type   The identifier representing the specific type of database records (Record Type ID).
     * @param nff           A list of file indices to safely add to the opened set.
     * @param ind           Reference to the key list structure for synchronization.
     * @return std::vector<size_t> A list of indices of the files that requires a rebuild/compression.
     */
    std::vector<int> add_open_files(int record_type, const std::vector<int> &nff, TDBKeyList &ind);

    /**
     * @brief Add a completely new database file to the system and open it for the specified record type.
     *
     * @param record_type   The identifier representing the specific type of database records (Record Type ID).
     * @param path          The absolute file system path for the new database file.
     * @param ind           Reference to the active key list to initialize new index allocations.
     * @return size_t       The newly allocated file index within the file manager.
     */
    size_t open_new_file(int record_type, const std::string &path, TDBKeyList &ind);

    /// Remove specified files from the system and update indices.
    void remove_files(const std::set<std::string>& target_names, TDBKeyList& ind);

    /// Close all currently open database files.
    void close_all();

    // --- (Index Lookups) ---

    /**
     * @brief Find the index of a file by its name (searches only among open files).
     * @return The file index, or static_cast<size_t>(-1) / npos if not found.
     */
    size_t file_name_index(const std::string &file_name) const;

    /**
     * @brief Find the index of a file by its relative path (searches only among open files).
     * @return The file index, or std::string::npos if not found.
     */
    size_t file_path_index(const std::string &file_path) const;

    /// Returns true if any system/default database files are currently open.
    bool check_special_open() const;

    // --- (GUI Helpers & Search Queries) ---

    /// Generate file paths and status data for GUI representation.
    void files_list(int mode, std::vector<std::string> &names,
                    std::vector<int> &indices, std::vector<int> &sel) const;

    /// Generate a list of all existing file indices (optionally filtering for core/kernel files only).
    std::vector<int> all_files(bool only_kernel) const;

    /// Generate a list of indices for opened system/special files (from db.default).
    std::vector<int> opened_special_files() const;

    /// Find indices of all files whose names match the pattern '.<name>.'.
    std::vector<int> files_with_name(const std::vector<std::string> &names) const;

    /// Get a list of file keywords that contain the specified name pattern '.<name>.'.
    std::vector<std::string> file_keywds_according_name(const std::string &name) const;

    /// Find indices of all files matching the given keywords.
    std::vector<int> files_with_keywds(const std::vector<std::string> &keywds, bool &all_found) const;

    /**
    * @brief Determines the target database file for adding a new record.
    *
    * If only one file is open, it is selected automatically. If multiple files are open,
    * it displays a graphical dialog prompting the user to choose the destination file.
    *
    * @param window     Pointer to the parent QWidget for displaying the choice dialog.
    * @param record_key The unique key of the record being added.
    * @param multi      Flag indicating bulk operations (enables the "Apply to all" option).
    * @param ok_to_all  Output reference flag that becomes true if the user chooses "Apply to all".
    *
    * @return int The valid file index (>= 0) if selected, or -1 if the operation was cancelled.
    * @note Uses 'int' to allow returning -1 as a cancel signal. Ensure to check for -1 before casting to size_t.
    */
    int file_to_add_record(QWidget *window, const std::string &record_key, bool multi, bool& ok_to_all);

    size_t active_file_index() const
    {
        return last_used_file;
    }

    std::string active_file_relpath() const
    {
        return check_index(last_used_file) ?
                   all_dbfiles[last_used_file]->relativePath() : "";
    }

    bool empty_files_list() const
    {
        return all_dbfiles.empty();
    }

protected:

    std::vector<std::shared_ptr<TDBFile>> all_dbfiles;

    /// Number of protected system files loaded from DB.defaults
    size_t special_files_num;

    /// Flag to enable or disable the deleted records tracking system (sfe).
    bool reuse_deleted;

    /// Flag to automatically close the database file after a read or write operation.
    bool auto_close;

    FileStatus open_mode;

    /// Indexes of opened files
    std::set<size_t> opened_files;

    /// List of opened files names
    std::vector<std::string> opened_files_list;

    /// Current (last accessed) database file index
    size_t last_used_file=0;

    void reset_open_list();
    void reset_index_list(TDBKeyList &ind);
};

#endif  // V_DBFILE_H_
