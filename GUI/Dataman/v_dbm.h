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

    //--- Manipulation records
    /// Checks whether a document exists.
    bool existsRecord(const std::string& key)
    {
        auto Rfind = Find(key.c_str());
        if(Rfind <0 )
            return false;
        return true;
    }

    /// Creates a new document in the collection from the current data.
    /// \param key      - key of document
    std::string addRecord(const std::string& key, size_t file_ndx)
    {
        std::string rid = key;

        return rid;
    }

    /// Retrive one document from the collection
    ///  \param key      - key of document
    void readRecord(const std::string& key)
    {
        int Rnum = Find(key.c_str());
        if(Rnum<0)  {
            std::string msg = "Record ";
            msg += std::string(key, 0, KeyLen());
            msg += " not found!" ;
            Error( getKeywd(), msg );
        }
        Get( Rnum );
    }

    /// Updates an existing document described by the key.
    ///  \param key      - key of document
    std::string updateRecord(const std::string& key)
    {
        std::string rid = key;
        auto Rnum = Find(key.c_str());
        Rep(Rnum);
        return rid;
    }

    /// Removes document from the collection
    /// \param key      - key of document
    void deleteRecord(const std::string& key, bool err_if_no=true)
    {
        int Rnum = Find(key.c_str());
        if(Rnum>=0)  {
            Del(Rnum);
        }
        else if(err_if_no) {
            std::string msg = "Record ";
            msg += std::string(key, 0, KeyLen());
            msg += " not found!" ;
            Error( getKeywd(), msg );
        }
    }


    uint AddRecordToFile1(const char *key, int file);
    void Rep( int i);
    int Find( const char *key);
    int FindCurrent( const char *key);
    RecStatus Rtest( const char *key, int mode = 1);
    bool FindPart( const char *key_, uint field );
    time_t GetTime(size_t record_ndx);               // time record number i
    time_t Rtime() const
    {
        return crt;
    }  // time current record

    size_t GetKeyList(const char *keypat, TCStringArray& aKey);
    size_t GetKeyListOld( const char *keypat,TCStringArray& aKey,TCIntArray& anR);
    void addRecord(QWidget *window, const char *pkey);
    void addRecord(QWidget *window, const char *pkey, int &fnum);

    //--- Manipulation files of Data Base
    const std::vector<std::string>& getOpenFiles(std::string &active) const
    {
        active = activeFileName();
        return dbfiles_list.opened_files_list;
    }
    uint GetOpenFileNum( const char *secondName  )
    {
        std::string name = std::string(getKeywd())+"."+secondName;
        return dbfiles_list.file_name_index(name);
    }
    void GetFileList(int mode, std::vector<std::string>& names,
                     std::vector<int>& indeces,
                     std::vector<int>& sel);
    void OpenAllFiles( bool only_kernel = false );
    void OpenOnlyFromList( TCStringArray& names );
    bool SetNewOpenFileList( const TCStringArray& aFlNames );
    void  MakeInNewProfile(const std::string& dir,
                          const std::string& prfName, const std::string& f_name={});
    void GetProfileFileKeywds( const char *name, TCStringArray& aFlkey )
    {
        aFlkey = dbfiles_list.file_keywds_according_name(name);
    }
    bool ifDefaultOpen() const
    {
        return dbfiles_list.check_special_open();
    }
    void RebildFile(const TCIntArray& indx);
    void AddFileToList(const std::string &path)
    {
        dbfiles_list.open_new_file(nRT2, path, ind);
    }
    void AddOpenFile(const TCIntArray& indx);
    size_t activeFileIndex() const
    {
        return dbfiles_list.active_file_index();
    }

    std::string activeFileName() const
    {
        return dbfiles_list.active_file_relpath();
    }



    //---Other
    void addFiles(const std::string &folder, const std::vector<std::string> &path_list);
    void removeFiles(const std::vector<std::string>& path_list);
    void toJsonObject( QJsonObject& obj ) const;
    std::string fromJsonObject( const QJsonObject& obj );
    void toJsonObjectNew(QJsonObject &obj, const std::string& project_name) const;
    std::string fromJsonObjectNew( const QJsonObject& obj );
    void RenameList( const char* newName,
                    const char *oldName );



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
    void check_record_key(const char *pkey);
    bool check_rt_name(const std::string &file_name) const;



    // void RecKey( uint i, std::string& kbuf )
    // {
    //     ind.RecKey(i,kbuf);
    // }



    bool KeyTest( const char* key );

    // void SetFldKey( uint i, const char *fld )
    // {
    //     ind.SetFldKey( i, fld );
    // }



    //--- Information about of open file list

    //--- Manipulation Data Base
    //void Create1( uint nF );

    //  RecStatus TryRec( RecStatus rs, char *key, int mode = 1);

    //--- Manipulation list of records

    // const TCStringArray& KeyList() const
    // {
    //     return ind.KeyList();
    // }
    // const TCIntArray& RnList() const
    // {
    //     return ind.RnList();
    // }

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

