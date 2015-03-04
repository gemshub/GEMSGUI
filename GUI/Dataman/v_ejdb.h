#ifndef __v_ejdb_h_
#define __v_ejdb_h_

#include <vector>
#include <set>

#include "v_vals.h"
#include "v_file.h"
#include "v_json.h"
#include "filters_data.h"


class EJCOLL;

extern const char* ALLKEY;

enum keyctrl {   // codes key bild
    K_END = -5, K_EMP, K_ANY, K_IMM, K_ACT
};

enum RecStatus {   // states of keys record Data Base
    UNDF_=-7 /* undefined state */, FAIL_=-1 /* access error */,
    NONE_=0 /* no records */, ONEF_ ,
    MANY_,  EMPC_ /* empty chain */
};

enum modeList { openf = 0x01, closef = 0x02, oldself = 0x04 };


/// Element in sequence of record keys
class IndexEntry
{
   vector<string> keyFlds;      /// Record key fields
   mutable string bson_oid;             /// oid of record (bson indexes)
   mutable int nFile;                   /// Index in db files list

 public:

   IndexEntry( const char* abson_oid, int nF, vector<string> akeyFlds):
       bson_oid(abson_oid),  nFile(nF), keyFlds(akeyFlds)
   { }

   IndexEntry( vector<string>& akeyFlds ):
       bson_oid("-1"),  nFile(-1), keyFlds(akeyFlds)
   { }

   IndexEntry( const IndexEntry& ndxE ):
       bson_oid(ndxE.bson_oid),  nFile(ndxE.nFile), keyFlds(ndxE.keyFlds)
   { }

   string getKey( vector<size_t>& rkLen );
   const string& getKeyField( int ii ) const
   { return keyFlds[ii];}

   const string& getBsonOid() const
   { return bson_oid; }
   void setBsonOid( const char* oid ) const
   { bson_oid = oid; }

   const int& getFile() const
   { return nFile; }
   int setFile( int nF ) const
   { nFile = nF; }

   friend bool operator <( const IndexEntry&,  const IndexEntry& );
   friend bool operator >( const IndexEntry&,  const IndexEntry& );
   friend bool operator==( const IndexEntry&,  const IndexEntry& );
   friend bool operator!=( const IndexEntry&,  const IndexEntry& );
   friend class TEJDBKey;
};


/// This is struct contened the key of record
class TEJDBKey
{
protected:
    vector<size_t> rkLen;    /// Length of fields
    vector<string> rkFld;    /// Current key fields
    size_t fullLen;          /// Full length of records key

    // const TEJDBKey& operator=(const TDBKey&);
    string pKey;                  /// Current key in packed form ( external )
    string uKey;                  /// Current key in upacked form ( external )

    /// Return record key in packed form
    const char *pack( const vector<string>& akeyFlds );
    /// Return record key in unpacked form
    const char *unpack( const vector<string>& akeyFlds );

public:
    TEJDBKey( int nRkflds, const size_t* rkfrm );
    TEJDBKey( fstream& f);
    TEJDBKey( const TEJDBKey& dbKey );
    virtual ~TEJDBKey(){}

    /// Return current record key in packed form
    const char *PackKey()
    { return pack( rkFld ); }

    /// Return current record key in unpacked form
    const char *UnpackKey()
    { return unpack( rkFld ); }

    /// Return record key length
    size_t KeyLen() const
    {  return fullLen;  }

    /// Return number of record key fields
    int KeyNumFlds() const
    {  return rkLen.size();  }

    /// Return record key field i
    const char* FldKey(int i) const
    {   return rkFld[i].c_str(); }

    /// Return record key field i len
    size_t FldLen(int i) const
    {   return rkLen[i];   }

    //--- Manipulation keys
    /// Putting DB record key pkey to internal structure
    void SetKey( const char *pkey );
    /// Change i-th field of TEJDBKey to fld
    void SetFldKey( int i, const char *fld );

    IndexEntry retIndex()
    { return IndexEntry(rkFld); }
    /// Return index key in unpacked/packed form
    const char *indexKey( const bool unpackKey, const IndexEntry& ndx )
    {
        if( unpackKey )
          return unpack( ndx.keyFlds );
        else
          return pack( ndx.keyFlds );
    }
    bool compareTemplate( const IndexEntry& elm );

    // Test work DB record key
    bool isPattern();
    bool isAll();

 //   friend class compareTemplate;
 };


/// This class contened the structure of EJDB Data Base
class TEJDataBase
{
    // Definition of chain
    string Keywd;  /// Name of modules DB
    int nRT;       /// Module number
    int frstOD;    /// First objects number to DB record
    int lastOD;    /// Last objects numbedr to DB record

    /// Definition of record key
    TEJDBKey key;
vector<string> aFldShot;          /// Short names of key fields

    // Definition of record key list
    /// Linked records list
    set<IndexEntry, less<IndexEntry> > recList;
    /// Current index in recList
    set<IndexEntry, less<IndexEntry> >::iterator itrL;
    // Work data
    RecStatus status;       /// ? Current states of keys record DB
    time_t crt;             /// ? Time of modifying current record ( not realized )
    //vector<string> wrkFld;  /// Work record key

    // Definition of EJDB files
    // int specialFilesNum;     /// ?extern Number of default DB files
    vector<int> fls;         /// ? List of opened files (indexes from aFile)
    vector<string> fOpenNameBuf;   /// ? List of names of opened files
    int fNum;                /// ? Current index in list of opened files

protected:

    void fromCFG(fstream& f);
    int findFnum( int nF );

    // must be moved
    bool dbChangeAllowed( int nf );
    //
    EJCOLL *openCollection( int nFile, bool createifempty = true );
    void closeCollection( int nFile );
    void loadCollection( int nFile );
    /// Load records key from bson structure
    void KeyFromBson( const char* bsdata, int nFile );

    void opfils();
    void delfile( int nF );
    int  getCurrentFile( int &fnum );

public:

    //  Constructor
    TEJDataBase( int nrt, const char* name,
                 int firstObj, int lastObj, int spflNum,
                 int nRkflds, const size_t* rkfrm  );
    TEJDataBase( fstream& f );
    ~TEJDataBase();

    /// Save definition of chain to configuration file
    void toCFG( fstream& f );

    //--- Selectors
    /// Get name of modules DB
    const char* GetKeywd() const
       {  return Keywd.c_str();   }
    /// Get records count in opened files
    int RecCount() const
       {  return recList.size(); }

    /// Get current record status
    RecStatus GetStatus() const
       {  return status;  }
    /// Set current record status
    void SetStatus( RecStatus stt )
       {   status = stt;   }
    // Set Number of default DB files
    //void SetSpecialFilesNum( int stt )
    //   {   specialFilesNum = stt;   }
    /// Time of current record
    time_t Rtime() const
        {  return crt;  }
    time_t GetTime( const char *pkey );

    /// Get current index in list of opened files
    int GetFnum() const
       {  return fNum;  }
    /// Access to list of names of opened files
    const vector<string>& GetOpenFiles() const
       { return fOpenNameBuf;  }

    //--- Selectors for key
    /// Return record key length
    int KeyLen() const
        { return key.KeyLen(); }
    /// Return current record key in packed form
    const char *PackKey()
        { return key.PackKey(); }
    /// Return current record key in unpacked form
    const char *UnpackKey()
        { return key.UnpackKey();  }
    /// Return record key field i
    const char *FldKey( int i ) const
        { return key.FldKey(i); }
    /// Return record key field i len
    size_t FldLen(int i) const
        { return key.FldLen(i);  }
    /// Return number or record key fields
    int KeyNumFlds() const
        { return key.KeyNumFlds();  }
    /// Access to TEJDBKey
    const TEJDBKey& GetDBKey() const
        { return key; }
    /// Putting DB record key pkey to internal structure
    void SetKey( const char *ckey )
        {  key.SetKey(ckey);
           // status = UNDF_;
        }
    /// Change i-th field of TEJDBKey to key
    void SetFldKey( int i, const char *fld )
        {  key.SetFldKey( i, fld ); }
    // Make packed key to seach.
    void MakeKey( unsigned char nRtwrk, string& pkey, ...);

    //--- Manipulation records
    /// Save current record to bson structure
    void RecToBson( bson *obj, time_t crtt, bson_oid_t *oid  = 0 );
    /// Load data from bson structure (return readed record key)
    string RecFromBson( bson *obj );

    /// Find record with key into internal record keys list
    bool Find( const char *key );
    /// Test state of record with key key_ as template.
    /// in field field setted any(*) data
    bool FindPart( const char *key_, int field );
    /// Retrive one record from the collection
    void Get( const char *key );
    /// Removes record from the collection
    void Del( const char *key );
    /// Save/update record in the collection
    // fnum - index into internal file list
    void SaveRecord( const char* key, int& fnum  );
    /// Save/update record in the collection
    /// Question for replase
    void SaveRecordQuestion(const char* pkey, int& fnum, bool& yesToAll );
    /// Save new record in the collection
    /// Test unique keys name before add the record(s) ???? must be
    // fnum - index into internal file list
    void AddRecord(const char* key, int& fnum );
    /// Test state of record with key pkey.
    /// If mode == 1 and one record, read this record.
    RecStatus Rtest( const char *key, int mode );

    //--- Manipulation list of records
    /// Get key list for a wildcard search
    int GetKeyList( const char *keypat,
        vector<string>& aKeyList, bool retUnpackform = true );
    /// Rename records ( change first field: for SYSEQ, and > )
    void RenameList( const char* newName, const char *oldName );

    //--- From module part

    // Work with files

    //--- Information about of open file list
    bool ifDefaultOpen() const;
    int GetOpenFileNum( const char *secondName  );
    void OpenAllFiles( bool only_kernel = false );
    void OpenOnlyFromList( const vector<string>& names );

    //--- Working with file keywords
    void GetProfileFileKeywds( const char *_name, vector<string>& aFlkey );
    bool SetNewOpenFileList( const vector<string>& aFlNames );
    void GetFileList( int mode, vector<string>& names,
                      vector<int>& indx,  vector<int>& sel );

    //--- Manipulation Data Base

    //void  MakeInNewProfile( const string& dir,
    //  const char *prfName, const char * f_name=0 )
    //{} // not used for EJDB collection created, when first save
    //void Create( int nF ) // EJDB collection created, when first save

    void Open( vector<int>& indx);
    void Open( );
    void Close();
    void delFnum( int nF );
//void DelFile(const string& path) - must be use when open project

    //--- Manipulation files of Data Base
    void AddOpenFile(const vector<int>& indx);

};



class EJDataBaseList:
            public vector<TEJDataBase>
{

    void getOpenFiles( const vector<string>& selNames, vector<string>& selfiles  );

  public:
    EJDataBaseList()
    { }

    ~EJDataBaseList()
    {}

    void Init();
    void fromCFG( fstream& f );
    void toCFG(fstream& f);

    //--- Selectors
    TEJDataBase& operator[](int) ;
    int Find(const char* keywd);

    // files selection
    void buildFilesList( elmFilesConfData& files_data,
                         const vector<string>& selNames );
    void resetFilesSelection(elmFilesConfData& files_data,
                         const vector<string>& selNames);
};

extern EJDataBaseList rtEJ;

/* Function object type for template
class compareTemplate
{
   TEJDBKey* key;
   bool unpackKey;
   vector<string> keyList;

  public:

    void setKey(TEJDBKey* pkey)
    { key = pkey; }
    void setForm( bool form )
    { unpackKey = form; }
    vector<string> getKeyList()
    {  return keyList; }

    void operator() (const IndexEntry& elm ) const
    {
      string sp;
      if( !key->isAll() )
        for( int j=0; j<key->KeyNumFlds(); j++)
        {
            sp = key->FldKey(j);
            if( sp == "*" )
                continue; //to next field
            for( int i=0; i<key->FldLen(j); i++ )
            {
                switch( sp[i] )
                {
                case '?': // no ' '
                    if( elm.getKeyField(j)[i] == ' ' )
                         return;
                    break;
                case '*':
                    goto NEXTKF;
                default:
                    if( sp[i] != elm.getKeyField(j)[i] )
                        return;
                }
            }
    NEXTKF: ;
        }

      keyList.push_back( key->indexKey( unpackKey, elm ));
    }
};
*/

#endif	// __v_ejdb_h_
