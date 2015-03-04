//#pragma warning (disable:4786)
#include <iostream>
#include <cstdarg>

#include "v_object.h"
#include "v_ejdb.h"
#include "v_ejdb_file.h"
#include "visor.h"
#include "service.h"
#include "v_mod.h"

//extern const char* ALLKEY;

void strip(string& str)
{
  string::size_type pos1 = str.find_first_not_of(' ');
  string::size_type pos2 = str.find_last_not_of(' ');
  str = str.substr( (pos1 == string::npos ? 0 : pos1),
     ( pos2 == string::npos ? str.length() - 1 : pos2 - pos1 + 1 ));
}

string replace( string str, const char* old_part, const char* new_part)
{
    size_t pos = str.find( old_part );
    if( pos == string::npos )
      return str;

    string res = str.substr(0, pos);
            res += new_part;
            res += str.substr( pos+strlen(old_part));
    return res;
}

string replace_all( string str, const char* old_part, const char* new_part)
{
    size_t pos = str.find( old_part );
    string res = "";

    while( pos != string::npos )
    {
        res += str.substr(0, pos);
        res += new_part;
        str = str.substr( pos+strlen(old_part));
        pos = str.find( old_part );
    }
    return res+str;
}
//-------------------------------------------------------------
// IndexEntry  - Element in sequence of record keys
//-------------------------------------------------------------

bool operator <( const IndexEntry& iEl,  const IndexEntry& iEr)
{
    for( int ii=0; ii<iEl.keyFlds.size(); ii++ )
    {
        if(iEl.keyFlds[ii] == iEr.keyFlds[ii])
          continue;
        return ( iEl.keyFlds[ii] < iEr.keyFlds[ii] ? true: false);
    }
    return false;
}

bool operator >( const IndexEntry& iEl,  const IndexEntry& iEr)
{
    for( int ii=0; ii<iEl.keyFlds.size(); ii++ )
    {
        if(iEl.keyFlds[ii] == iEr.keyFlds[ii])
          continue;
        return ( iEl.keyFlds[ii] > iEr.keyFlds[ii] ? true: false);
    }
    return false;
}

bool operator ==( const IndexEntry& iEl,  const IndexEntry& iEr)
{
    return equal( iEl.keyFlds.begin(), iEl.keyFlds.end(), iEr.keyFlds.begin());
}

bool operator !=( const IndexEntry& iEl,  const IndexEntry& iEr)
{
    return !equal( iEl.keyFlds.begin(), iEl.keyFlds.end(), iEr.keyFlds.begin());
}

//-------------------------------------------------------------
// TEJDBKey  - working with DB record key
//-------------------------------------------------------------

/// Default configuration
TEJDBKey::TEJDBKey( int rkFlds, const size_t* rkfrm )
{
    int ii;
    for( fullLen=0, ii=0; ii<rkFlds; ii++)
    {
        rkLen.push_back( rkfrm[ii]);
        rkFld.push_back( "*"  );
        fullLen += rkLen[ii];
    }
}

/// Configuration from cfg file
TEJDBKey::TEJDBKey(fstream& in_stream)
{
    int ii, rkFlds, tmp;
    in_stream >> rkFlds;
    for( fullLen=0, ii=0; ii<rkFlds; ii++)
    {
        in_stream >> tmp;
        rkLen.push_back( tmp);
        rkFld.push_back( "*"  );
        fullLen += rkLen[ii];
    }
}

/// Copy struct
TEJDBKey::TEJDBKey( const TEJDBKey& dbKey ):
        fullLen( dbKey.fullLen )
{
    for(int ii=0; ii<dbKey.KeyNumFlds(); ii++)
    {
        rkLen.push_back( dbKey.rkLen[ii] );
        rkFld.push_back( dbKey.rkFld[ii]  );
    }
    pKey = dbKey.pKey;
    uKey = dbKey.uKey;
}

// Return record key in packed form
const char *TEJDBKey::pack( const vector<string>& akeyFlds )
{
    string sp;
    pKey = "";

    for(int ii=0; ii<KeyNumFlds(); ii++)
    {
        sp = akeyFlds[ii];
        //strip( sp );
        pKey += sp;
        if(  sp.length() < rkLen[ii] )
            pKey+= ":";
    }
    return pKey.c_str();
}

// Return current record key in unpacked form
const char *TEJDBKey::unpack( const vector<string>& akeyFlds )
{
    string sp;
    uKey = "";

    for(int ii=0; ii<KeyNumFlds(); ii++)
    {
        sp = akeyFlds[ii];
        //strip( sp );
        uKey += sp;
        if(  sp.length() < rkLen[ii] )
            uKey.append( rkLen[ii]-sp.length(), ' ');
    }
    return uKey.c_str();
}

// Putting DB record key key to internal structure
void TEJDBKey::SetKey( const char *key )
{
    ErrorIf( key==0, "TEJDBKey", "Undefined key of record.");
    rkFld.clear();
    string sp = key;

    if( sp == ALLKEY )
    {
        for(int ii=0; ii<KeyNumFlds(); ii++)
           rkFld.push_back( "*" );
    }
    else
    {
        for(int ii=0; ii<KeyNumFlds(); ii++)
        {
          size_t pos = sp.find_first_of(':');
          if( pos == string::npos )
          {
              pos = sp.length();
              if( ii < KeyNumFlds()-1)
                  if( pos < rkLen[ii] )
                  ErrorIf( pos < rkLen[ii], key, "Invalid packed record key.");
          }
          pos =  min( pos, rkLen[ii] );
          rkFld.push_back( sp.substr(0, pos) );
          strip( rkFld[ii] );
          if( sp[pos] == ':' )
              pos++;
          sp = sp.substr(pos);
        }
    }
 }

// Change i-th field of TEJDBKey to key
void TEJDBKey::SetFldKey( int i, const char *key )
{
    ErrorIf( i>= KeyNumFlds() || i<0, key, "Invalid key field number");
    string sp = string( key,  0, rkLen[i]);
    strip( sp );
    rkFld[i] = sp;
}

/// Setted pattern of record key
bool TEJDBKey::isPattern()
{
  bool OneRec =  true;

  for(int ii=0; ii<KeyNumFlds(); ii++)
      if( rkFld[ii].find_first_of("*?") != std::string::npos )
      {    OneRec = false;
           break;
      }
  return !OneRec;
}

/// Setted ALLKEY
bool TEJDBKey::isAll()
{
  bool AllRecs = true;

  for(int ii=0; ii<KeyNumFlds(); ii++)
      if( rkFld[ii] != "*" )
      {    AllRecs = false;
           break;
      }
  return AllRecs;
}

bool TEJDBKey::compareTemplate( const IndexEntry& elm )
{
  int i, j;
  string kpart, kelm;

  //if( !isAll() )
    for( j=0; j<KeyNumFlds(); j++)
    {
        kpart = FldKey(j);
        kpart.append( rkLen[j]-kpart.length(), ' ');
        kelm = elm.getKeyField(j);
        kelm.append( rkLen[j]-kelm.length(), ' ');

        if( kpart == "*" )
            continue; //to next field
        for( i=0; i<rkLen[j]; i++ )
        {
            if( kpart[i] == '*' ) // next field
              break;
            switch( kpart[i] )
            {
             case '?': // no ' '
                if( kelm[i] == ' ' )
                    return false;
                break;
             default:
                if( kpart[i] != kelm[i] )
                    return false;
            }
        }

    }

  return true;
}

//-------------------------------------------------------------
// TEJDataBase - This class contened the structure of EJDB Data Base
//-------------------------------------------------------------

// Put state to cfg file
void TEJDataBase::toCFG(fstream& out_stream)
{
    int ii;
     // key definition
   out_stream << key.KeyNumFlds() << "  ";
   for( ii=0; ii< key.KeyNumFlds(); ii++ )
      out_stream << key.FldLen(ii) << ' ';
     // DB  definition
   out_stream << "  ";
   out_stream << Keywd << ' ';
   out_stream << nRT << "  ";
   out_stream << frstOD << ' ';
   out_stream << lastOD << "  ";
   //out_stream << specialFilesNum <<  "  ";
     // opened files definition
   out_stream << fls.size() << "  ";
   for( ii=0; ii<fls.size(); ii++)
        out_stream << fls[ii] << ' ';
   out_stream << endl;
   /* DB files
   int numFiles = aFile.size();
   out_stream << "  " << numFiles << endl;
   ErrorIf( !out_stream.good(), GetKeywd(),
             "Error writing TDataBase to configurator");
    for(int ii=0; ii<numFiles; ii++)
        aFile[ii]->toCFG(out_stream);
   */
}

// Get state from cfg file
void TEJDataBase::fromCFG(fstream& in_stream)
{
    int ii;
    // DB definition
    in_stream >> Keywd;
    in_stream >> nRT;
    in_stream >> frstOD;
    in_stream >> lastOD;
    //in_stream >> specialFilesNum;
    // opened files
    int numFiles, fl;
    in_stream >> numFiles;
    for( ii=0; ii<numFiles; ii++)
    {
        in_stream >> fl;
        fls.push_back(fl);
    }
}

/// Default configuration of the Data Base
TEJDataBase::TEJDataBase( int nrt, const char* name,
                          int firstObj, int lastObj, int spflNum,
                          int nRkflds, const size_t* rkfrm  ):
    Keywd( name ), nRT( nrt ), frstOD(firstObj), lastOD(lastObj),
    key( nRkflds, rkfrm ), status( UNDF_),
    /*specialFilesNum(spflNum),*/ fNum(-1)
{
    crt = time(NULL);
}

/// Configuration from cfg file
TEJDataBase::TEJDataBase( fstream& fcfg ):
        key( fcfg ), status(UNDF_), fNum(-1)
{
    fromCFG(fcfg);
    crt = time(NULL);
}

TEJDataBase::~TEJDataBase()
{
    /*
      for(int j=0; j<fls.size(); j++ )
        aFile[fls[j]]->Close();
      fls.clear();
      fOpenNameBuf.clear();
    */

}

int TEJDataBase::findFnum( int nF )
{
    int ii;
    for( ii = 0; ii< fls.size(); ii++ )
    {
        if( fls[ii] == nF )
          return ii;
    }
    return -1;
}

void TEJDataBase::delFnum( int nF )
{
    vector<int>::iterator it = fls.begin();
    while( it < fls.end() )
    {
        if( *it == nF )
        {
            fls.erase(it);
            break;
        }
        it++;
    }
 }

#ifdef Change_DB_Mode
bool TEJDataBase::dbChangeAllowed( int /*nf*/ )
{
    return true;
}
#else
bool TEJDataBase::dbChangeAllowed( int nFile )
{
    return ( pVisor->isDBChangeMode() )
        || ( !flsEJ[nFile].isDefaultDB() );
}
#endif

/// Get or create collection 'module name' before saving/delete
EJCOLL *TEJDataBase::openCollection( int nFile, bool createifempty )
{
  // Test and open file  (name of ejdb must be take from nFile)
  flsEJ[nFile].Open();
  if( !flsEJ[nFile].ejDB )
  {
      string err= "Cannot open EJDB "+ flsEJ[nFile].Name();
      Error("TEJDB0011", err ); //return NULL;
  }

  EJCOLL *coll;
  if( createifempty )
  {  // Get or create collection 'module name'
     coll = ejdbcreatecoll(flsEJ[nFile].ejDB, GetKeywd(), NULL );
     if( !coll )
     {
       closeCollection( nFile );
       string err= "Cannot open EJDB collection "+ flsEJ[nFile].Name()+GetKeywd();
        Error("TEJDB0012", err );
     }
   }
   else
      coll = ejdbgetcoll(flsEJ[nFile].ejDB, GetKeywd());

   return coll;
}

/// Close
void TEJDataBase::closeCollection( int nFile )
{
    flsEJ[nFile].Close();
}

// end of extern

//--- Manipulation records

// Save current record to bson structure
void TEJDataBase::RecToBson( bson *obj, time_t crtt, bson_oid_t *oid )
{
   char buf[100];
   int ii, no;

   bson_init( obj );

   //write key fileds
   bson_append_start_array(obj, "key");
   for( ii=0; ii < KeyNumFlds(); ii++)
   {
          sprintf(buf, "%d", ii);
          bson_append_start_object( obj, buf );
 ///         bson_append_string( obj,"kf", aFldShot[ii].c_str() );
          bson_append_int( obj, "fx", ii );
          bson_append_int( obj, "fl", FldLen(ii) );
          bson_append_string( obj,"fv", FldKey(ii) );
          bson_append_finish_object( obj );
    }
    bson_append_finish_array(obj);

    // write objects
    bson_append_start_array(obj, "dod");
    for( ii=0, no=frstOD; no<=lastOD;  no++)
    {
        if( aObj[no].GetDescription(0,0) == "ejdbignore" )
          continue;
           sprintf(buf, "%d", ii);
           bson_append_start_object( obj, buf);
           bson_append_int( obj, "id", no );
           aObj[no].toBsonObject( obj );
           bson_append_finish_object( obj );
           ii++;
    }
    bson_append_finish_array(obj);

    // added Modify time
    bson_append_time_t( obj , "mtime", crtt );

    if( oid )
    {
        bson_append_oid(obj, JDBIDKEYNAME, oid);
    }
    bson_finish( obj );
}

// Load data from bson structure
string TEJDataBase::RecFromBson( bson *obj )
{
    char buf[100];
    int ii, no;

    // Get key of record  (only for test ???? )
    const char *arrkey = bson_find_array( obj->data, "key");
    string keyStr = "", kbuf;
    for( ii=0; ii<KeyNumFlds(); ii++ )
    {
        sprintf(buf, "%d", ii);
        const char *objkey = bson_find_object( arrkey, buf);
        if( !bson_find_string( objkey, "fv", kbuf ) )
            kbuf = "*";
        strip( kbuf );
        keyStr += kbuf.substr(0, FldLen( ii ));
        keyStr += ":";
    }

    // read objects
    const char *arrdod = bson_find_array( obj->data, "dod");
    bson_iterator itobj;
    bson_iterator_from_buffer(&itobj, arrdod);
    ii = 0;
    while( bson_iterator_next(&itobj) )
    {
        const char *objdod = bson_iterator_value(&itobj);
        if( !bson_find_value( objdod, "id", no ))
          no= -1; // undefined id
        if( !bson_find_string( objdod, "label", kbuf ))
          continue; // undefined label
        strip(kbuf);

        // test changing index of object in list
        if( no<0 || strcmp( aObj[no].GetKeywd(), kbuf.c_str() ))
            no = aObj.Find(kbuf.c_str());
        if( no < 0)
          continue;
        aObj[no].fromBsonObject( objdod );
   }

    // get time
    crt =  bson_find_time_t( obj->data, "mtime" );
    return keyStr;
}

// Load data from bson structure
void TEJDataBase::KeyFromBson( const char* bsdata, int nFile )
{
    char buf[100];
    int ii;

    // Get key of record
    const char *arrkey = bson_find_array( bsdata, "key");
    string keyStr = "", kbuf;
    for( ii=0; ii<KeyNumFlds(); ii++ )
    {
        sprintf(buf, "%d", ii);
        const char *objkey = bson_find_object( arrkey, buf);
        if( !bson_find_string( objkey, "fv", kbuf ) )
            kbuf = "*";
        strip( kbuf );
        keyStr += kbuf.substr(0, FldLen( ii ));
        keyStr += ":";
    }

    char oidhex[25];
    bson_iterator it;
    bson_find_from_buffer(&it, bsdata, "_id" );
    bson_oid_to_string(bson_iterator_oid(&it), oidhex);

    // Try to insert new record to list
    key.SetKey( keyStr.c_str() );
    if( key.isPattern() )
      Error("TEJDB0010", "Cannot save under record key template" );

    pair<set<IndexEntry>::iterator,bool> ret;
    ret = recList.insert( key.retIndex() );
    itrL = ret.first;
    // Test unique keys name before add the record(s)
    if( ret.second == false)
    {
        string erstr = "Cannot add new record:\n";
               erstr += keyStr;
               erstr += ".\nTwo records with the same key!";
        Error("TEJDB0014", erstr );
    }
    itrL->setBsonOid(oidhex);
    itrL->setFile(nFile);
}

/* Save current record to txt file
void TEJDataBase::RecToTxt( fstream& ftxt )
{
    aObj[o_reckey].SetPtr( const_cast<char *>(PackKey()) );
    aObj[o_reckey].toTXT(ftxt);
    for( int no=frstOD; no<=lastOD;  no++ )
         aObj[no].toTXT( ftxt );
}

// Loads Data Record from txt-file
void TEJDataBase::RecFromTxt( fstream& ftxt, int &fnum )
{
    char buf[300];
    aObj[o_reckey].SetPtr( buf );
    aObj[o_reckey].ofTXT( ftxt );
    buf[ KeyLen() ] = '\0';

    for( int no=frstOD; no<=lastOD;  no++ )
        aObj[no].ofTXT( ftxt );

    SaveRecordQuestion( buf, fnum );
}
*/

//Seach record index with key pkey.
bool TEJDataBase::Find( const char *pkey )
{
    if(recList.empty() )
      return false;
    TEJDBKey wkey(key);
    wkey.SetKey( pkey );
    itrL = recList.find( wkey.retIndex() );

    if( itrL == recList.end() )
        return  false;
    else
        return true;
}

//Test state of record with key key_ as template.
// in field field setted any(*) data
bool TEJDataBase::FindPart( const char *pkey, int field )
{
    TEJDBKey wkey(key);
    wkey.SetKey( pkey );
    wkey.SetFldKey(field,"*");
    RecStatus iRet = Rtest( wkey.PackKey(), 0 );
    return ( iRet==MANY_ || iRet==ONEF_ );
}

// Retrive one record from the collection
void TEJDataBase::Get( const char *pkey )
{
    key.SetKey( pkey );
    if( key.isPattern() )
      Error("TEJDB0010", "Cannot get under record key template" );

    itrL = recList.find( key.retIndex() );

    if( itrL == recList.end() )
    {
       string erstr = pkey;
              erstr += "\nrecord to retrive does not exist!";
       Error("TEJDB0001", erstr );
    }

    // Get oid of record
    bson_oid_t oid;
    string stroid = itrL->getBsonOid();
    bson_oid_from_string( &oid, stroid.c_str() );

    // Get current collection file ( must be done )
    int nF = itrL->getFile();
    EJCOLL *coll = openCollection( nF );

    // Read bson structure from collection by index oid
    bson *bsrec = ejdbloadbson(coll, &oid);
    // Close database (must be done for exeption )
    closeCollection( nF );

    if( !bsrec )
    {  string errejdb = "Error Loading record ";
              errejdb+= pkey;
              errejdb+= " from EJDB";
       Error( "TEJDB0025",  errejdb );
    }

    // Save bson structure to internal arrays
    RecFromBson( bsrec );
    bson_destroy(bsrec);


    // Set up internal data
    status = ONEF_;
    fNum = findFnum( nF );
}

/// Removes record from the collection
void TEJDataBase::Del( const char *pkey )
{
    key.SetKey( pkey );
    if( key.isPattern() )
      Error("TEJDB0010", "Cannot delete under record key template" );

    itrL = recList.find( key.retIndex() );

    if( itrL == recList.end() )
    {
       string erstr = pkey;
              erstr+= "\nrecord to delete does not exist!";
       Error("TEJDB0002", erstr );
    }

    // Get oid of record
    bson_oid_t oid;
    bson_oid_from_string( &oid, itrL->getBsonOid().c_str() );

    // Get current collection file
    int nF = itrL->getFile();
    if( !dbChangeAllowed( nF ) )
    {
        string erstr = "Cannot delete record:\n";
               erstr += pkey;
               erstr += ".\nChanges to system database file\n";
               erstr += flsEJ[nF].Name();
               erstr += " are not allowed!";
        Error("TEJDB0003", erstr );
    }
    EJCOLL *coll = openCollection( nF );

    // Remove BSON object from collection.
    bool iRet = ejdbrmbson( coll,  &oid );
    /// putndx( nF ); !!! working with index file

    // Close database (must be done for exeption )
    closeCollection( nF );

    if( !iRet )
    {  string errejdb = "Error deleting of record ";
              errejdb+= pkey;
              errejdb+= " from EJDB";
       Error( "TEJDB0024",  errejdb );
    }


    // Set up internal data
    recList.erase(itrL);
    itrL = recList.end();
    status = UNDF_;
    fNum = findFnum( nF );
}

/// Save new record in the collection
/// fnum - index into internal file list
void TEJDataBase::AddRecord(const char* pkey, int& _fnum  )
{
    int fnum = _fnum;

    // Select current file name if fnum == -1 - undefined
    if( fnum < 0 )
        fnum = getCurrentFile( _fnum );
    if( _fnum == -2 ) // cancel
       return;

    // Try to insert new record to list
    key.SetKey( pkey );
    if( key.isPattern() )
      Error("TEJDB0010", "Cannot save under record key template" );

    pair<set<IndexEntry>::iterator,bool> ret;

    ret = recList.insert( key.retIndex() );
    itrL = ret.first;
    // Test unique keys name before add the record(s)
    if( ret.second == false)
    {
        string erstr = "Cannot add new record:\n";
               erstr += pkey;
               erstr += ".\nTwo records with the same key!";
        Error("TEJDB0004", erstr );
    }

    // Get current collection file ( must be done )
    int nF = fls[fnum];
    if( !dbChangeAllowed( nF ) )
    {
               recList.erase(itrL);
               string erstr = "Cannot add record:\n";
               erstr += pkey;
               erstr += ".\nChanges to system database file\n";
               erstr += flsEJ[nF].Name();
               erstr += " are not allowed!";
        Error("TEJDB0003", erstr );
    }
    EJCOLL *coll = openCollection( nF );

    // Get bson structure from internal arrays
    bson bsrec;
    RecToBson( &bsrec, time(NULL) );

    // Persist BSON object in the collection
    char bytes[25];
    bson_oid_t oid;
    bool retSave = ejdbsavebson(coll, &bsrec, &oid);
    // Close database (must be done for exeption )
    closeCollection( nF );
    if( !retSave )
    {  string errejdb = bson_first_errormsg(&bsrec);
       bson_destroy(&bsrec);
       recList.erase(itrL);
       Error( "TEJDB0021",  errejdb );
    }
    else
    {
       bson_oid_to_string( &oid, bytes );
      itrL->setBsonOid(bytes);
      itrL->setFile(nF);
      /// putndx(nF); work with indexes
      bson_destroy(&bsrec);
    }
    cout << "Add record " << retSave << " oid " << bytes[25] << endl;

    // Set up internal data
    status = ONEF_;
    fNum = fnum;
}

/// Save/update record in the collection
void TEJDataBase::SaveRecord(const char* pkey, int& _fnum )
{
   int nF, fnum = _fnum;
   bson_oid_t oid;
   bson bsrec;

   // Try to insert new record to list
   key.SetKey( pkey );
   if( key.isPattern() )
     Error("TEJDB0010", "Cannot save under record key template" );

   pair<set<IndexEntry>::iterator,bool> ret;

   ret = recList.insert( key.retIndex() );
   itrL = ret.first;

   if( ret.second == true ) // new record
    {
        // Select current file name if fnum == -1 - undefined
        if( fnum <0 )
           fnum = getCurrentFile( _fnum );

        if( _fnum == -2 )  // cancel command
        {
          recList.erase(itrL);
          return;
        }
        // Get current collection file ( must be done )
        nF = fls[fnum];
        fNum = fnum;
        // Get bson structure from internal arrays
        RecToBson( &bsrec, time(NULL) );
    }
    else  // update record in the collection
    {
        nF = itrL->getFile();
        bson_oid_from_string( &oid, itrL->getBsonOid().c_str() );
        fNum = findFnum( nF );
        // Get bson structure from internal arrays
        RecToBson( &bsrec, time(NULL), &oid );
    }

    if( !dbChangeAllowed( nF ) )
        {
            string erstr = "Cannot save record:\n";
                   erstr += pkey;
                   erstr += ".\nChanges to system database file\n";
                   erstr += flsEJ[nF].Name();
                   erstr += " are not allowed!";
            Error("TEJDB0003", erstr );
        }
     EJCOLL *coll = openCollection( nF );

     // Persist BSON object in the collection
     bool retSave = ejdbsavebson(coll, &bsrec, &oid);
     // Close database (must be done for exeption )
     closeCollection( nF );

     if( !retSave )
     {  string errejdb = bson_first_errormsg(&bsrec);
        bson_destroy(&bsrec);
        if( ret.second == true )
            recList.erase(itrL);
        Error( "TEJDB0022",  errejdb );
     }

     if( ret.second == true ) // new record
     {   char bytes[25];
         bson_oid_to_string( &oid, bytes );
         itrL->setBsonOid(bytes);
         itrL->setFile(nF);
         /// putndx(nF); work with indexes
     }
     bson_destroy(&bsrec);
     cout << "Saving record " << retSave <<
              " oid " << itrL->getBsonOid().c_str() << endl;

     // Set up internal data
     status = ONEF_;
}

/// Save/update record in the collection
/// Question for replase
void TEJDataBase::SaveRecordQuestion(const char* pkey, int& fnum, bool& yesToAll )
{
    if( !yesToAll && Find( pkey ) )
    {
      switch( vfQuestion3(aMod[nRT]->window(), pkey,
                         "Data record with this key already exists! Replace?",
                          "&Yes", "&No", "&Yes to All" ))
      {
         case VF3_3: yesToAll=true;
         case VF3_1: break;
         case VF3_2: return;
      }
    }
    SaveRecord( pkey, fnum );
}

// Test state of record with key pkey.
// If mode == 1 and one record, read this record.
// returns state of record
RecStatus TEJDataBase::Rtest( const char *key, int mode )
{
    int iRec;
    const char *pkey;
    bool OneRec = true;

    if( key != 0 && *key )
        pkey = key;
    else
        pkey = PackKey();

    if( strpbrk( pkey, "*?" ))
        OneRec = false; // pattern

    status = UNDF_;

    if( RecCount() <= 0 )
        return EMPC_;

    if( OneRec )
    {
       if( !Find( pkey )  )
            return NONE_;
       if( mode == 1 )
          Get( pkey );
       return ONEF_;
    }
    else // template
    {   vector<string> aKeyList;
        iRec = GetKeyList( pkey, aKeyList, false );
        if( iRec == 0 )
            return NONE_;
        if( iRec == 1 )
        {
            if( mode == 1 )
                Get( aKeyList[0].c_str() );
            return ONEF_;
        }
        return MANY_;
    }
}

int TEJDataBase::GetKeyList( const char *keypat,
        vector<string>& aKeyList, bool retUnpackform )
{
    // Set key template
    TEJDBKey wkey(key);
    wkey.SetKey( keypat );
    aKeyList.clear();

    /* Define functional
    compareTemplate tmpl;
    tmpl.setKey( &wkey );
    tmpl.setForm( retUnpackform );
    // Applies function to each of the elements in the range
    for_each( recList.begin(), recList.end(), tmpl );
    // Copy result
    copy( tmpl.getKeyList().begin(), tmpl.getKeyList().end(), aKeyList.begin() );
    */

    bool OneRec = !wkey.isPattern(),
         AllRecs = wkey.isAll();

    set<IndexEntry, less<IndexEntry> >::iterator it;
    it = recList.begin();
    while( it != recList.end() )
    {
        if( !AllRecs )
            if( !wkey.compareTemplate( *it ) )
              { it++;
                continue;
              }
        aKeyList.push_back( wkey.indexKey( retUnpackform, *it ) );
        if( OneRec )
         break;

        it++;
    }
    return aKeyList.size();
}

// Rename records (Change first field: for SYSEQ, and >)
void TEJDataBase::RenameList( const char* newName,
        const char *oldName )
{
    if( strlen(newName) > FldLen(0) )
      return;

    TEJDBKey wkey(key);
    wkey.SetKey( ALLKEY );
    wkey.SetFldKey( 0, oldName );

    vector<string> aKeyList;
    GetKeyList( wkey.PackKey(), aKeyList, false );

    for(int ii=0; ii<aKeyList.size(); ii++ )
    {
        Get( aKeyList[ii].c_str() );
        Del( aKeyList[ii].c_str() );
        // changing record key
        SetFldKey( 0, newName );
        AddRecord( PackKey(), fNum );
    }
}

// make packed key to seach.
void TEJDataBase::MakeKey( unsigned char nRTwrk, string& pkey, ... )
{
    TEJDBKey wkey(key);
    va_list Marker;
    int ii, rts, nkf;
    char *imf;

    va_start( Marker, pkey );
    for( ii=0; ii< wkey.KeyNumFlds(); ii++ )
    {
        rts = va_arg( Marker, int );
        switch( rts )
        {
        case K_END:
            for( ; ii < wkey.KeyNumFlds(); ii++ )
              wkey.SetFldKey( ii, S_ANY );
            break;
        case K_EMP:  // field "`"
            wkey.SetFldKey( ii, S_EMPTY );
            break;
        case K_ANY:  // field  "*"
            wkey.SetFldKey( ii, S_ANY );
            break;
        case K_IMM:  // field in string
            imf = va_arg( Marker, char * );
            wkey.SetFldKey( ii, imf );
            break;
        case K_ACT:  // get field from  PRIE request
            rts = nRTwrk;
        default:     // get field from enathe chain key
            nkf = va_arg( Marker, int );
            if( !*rt[rts].FldKey( nkf ))
                wkey.SetFldKey( ii, S_EMPTY );
            else
                wkey.SetFldKey( ii, rt[rts].FldKey( nkf ) );

            break;
        }
    }
    pkey = wkey.PackKey();
    va_end( Marker );
}

//--------------------------------------------------------------
// Working with files

/// Delete the keys of records file nF
void TEJDataBase::delfile( int nF )
{
   set<IndexEntry, less<IndexEntry> >::iterator itdel;
   set<IndexEntry, less<IndexEntry> >::iterator it;
    it = recList.begin();
    while( it != recList.end() )
    {
        if( it->getFile() == nF )
        {
             itdel = it;
             it++;
             recList.erase(itdel);
        }
        else it++;
    }
}

/// Choose a database file to put a record, if fnum<0 - undefined
int TEJDataBase::getCurrentFile( int &fnum )
{
    int file = fnum;

    if( fnum < 0 )
    {
       ErrorIf( fOpenNameBuf.empty(), GetKeywd(),
               "No database file choosen");

       if( fOpenNameBuf.size() > 1 )
       {
          bool ok_to_all = false;
          string sname = "Choose a database file to put a record: "
                  + string( PackKey() );
          file = fOpenNameBuf.size() - 1;
          file = vfChoice2( aMod[nRT]->window(), fOpenNameBuf,
                            sname.c_str(), file, ok_to_all );
          if( ok_to_all == true && file >= 0 )
              fnum = file;
       }
       else
         file = 0;
   }
   return file;
}

//Read record update time from PDB file to memory.
time_t TEJDataBase::GetTime( const char *pkey )
{
    time_t rtime = time(NULL);
    key.SetKey( pkey );
    if( key.isPattern() )
      Error("TEJDB0010", "Cannot get time under record key template" );

    itrL = recList.find( key.retIndex() );
    if( itrL == recList.end() )
        return rtime;

    // Get oid of record
    bson_oid_t oid;
    string stroid = itrL->getBsonOid();
    bson_oid_from_string( &oid, stroid.c_str() );

    // Get current collection file ( must be done )
    int nF = itrL->getFile();
    EJCOLL *coll = openCollection( nF );

    // Read bson structure from collection by index oid
    bson *bsrec = ejdbloadbson(coll, &oid);
    // Close database (must be done for exeption )
    closeCollection( nF );

    if( !bsrec )
    {  bson_destroy(bsrec);
       string errejdb = "Error Loading record ";
              errejdb+= pkey;
              errejdb+= " from EJDB";
       Error( "TEJDB0025",  errejdb );
    }

    // get time
    rtime  =  bson_find_time_t( bsrec->data, "mtime" );
    // RecFromBson( bsrec );
    bson_destroy(bsrec);

    status =  UNDF_;
    fNum = findFnum( nF );
    return rtime;
}


/// True if opened some files from default data base
bool TEJDataBase::ifDefaultOpen() const
{
    for(int i=0; i<fls.size(); i++)
        if( flsEJ[fls[i]].isDefaultDB() )
            return true;
    return false;
}

/// Get open file number by name (0 if not found )
int TEJDataBase::GetOpenFileNum( const char* DBname )
{
    for( int i=0; i<fls.size(); i++)
        if( flsEJ[fls[i]].Name() == DBname )
            return i;
    return 0;
}

/// Open all EJ DataBases for module
void TEJDataBase::OpenAllFiles( bool only_kernel )
{
    if( flsEJ.size() == 0 )
        return;

    Close();
    for( int ii=0; ii< flsEJ.size(); ii++)
       if( !only_kernel || flsEJ[ii].isDefaultDB() )
           fls.push_back( ii );
    Open();
 }

/// Reopen EJ DataBases for module
/// Parameter: names - EJDB names to open
void TEJDataBase::OpenOnlyFromList( const vector<string>& names )
{
    if( flsEJ.size() == 0 )
        return;

    Close();
    int ii, jj;
    for( ii=0; ii< flsEJ.size(); ii++)
    {
        for( jj=0; jj< names.size(); jj++)
           if(  flsEJ[ii].Name().find( names[jj] ) != string::npos )
           {  fls.push_back( ii );
              break;
           }
    }
    Open();
}

/// Get list of files keywds, that contained _name
void TEJDataBase::GetProfileFileKeywds( const char *_name, vector<string>& aFlkey )
{
    string startName = string(GetKeywd(), 0, 2);
    //aFlkey.clear();

    for(int ii=0; ii< flsEJ.size(); ii++)
    {
      if(  flsEJ[ii].Name().find( _name ) != string::npos )
      {
          aFlkey.push_back( startName+flsEJ[ii].GetKeywd() );
      }
    }
}

/// Set new open Files list  (Reopen by list of Keywd)
bool TEJDataBase::SetNewOpenFileList(const vector<string>& aFlKeywd)
{
    bool allOpend = true;
    //string startName = string(GetKeywd(), 0, 2);

    if(aFlKeywd.size() < 1 )
        return  allOpend;

    Close();
    for(int i=0; i<aFlKeywd.size(); i++)
    {
        int nF = -1;

        for(int j=0; j<flsEJ.size(); j++)
        {

            if( string( aFlKeywd[i], 2) == flsEJ[j].GetKeywd() )
            {
                nF = j;
                break;
            }
        }

        if( nF >= 0 )
            fls.push_back(nF);
        else
        {
            if( !vfQuestion( 0, aFlKeywd[i],
             "This database file was not found in the project or default database.\n"
                    " Continue without this file (Y) or cancel(N)?" ))
               Error( aFlKeywd[i],
                    "Please, provide this database file and try opening the project again!");
            allOpend = false;
        }
    }

    Open();
    return allOpend;
}

/// Selection in the list of full file names
void TEJDataBase::GetFileList(int mode, vector<string>& names,
                            vector<int>& indx,  vector<int>& sel)
{
    string startName = string(GetKeywd(), 0, 2);
    names.clear();
    indx.clear();
    sel.clear();
    for( int i=0; i<flsEJ.size(); i++ )
    {
        int nF = findFnum( i );
        if( (nF==-1&&(mode&closef))||(nF!=-1&&(mode&openf)) )
        {
            //aFile[i]->Makepath();
            names.push_back( startName+string(flsEJ[i].GetKeywd())+string(" ")+
                             flsEJ[i].GetPath()+"_"+GetKeywd());
            indx.push_back(i);
            if( (mode&oldself) && nF != -1) //select already open files
                sel.push_back(indx.size()-1);
        }
    }
}

// Working with collections


void TEJDataBase::loadCollection( int nFile )
{
    EJCOLL *coll = openCollection( nFile, false );

    if( !coll )
    {
        closeCollection( nFile );
        return;
    }

    // select all records
    bson bsq1;
    bson_init_as_query(&bsq1);
    bson_finish(&bsq1);

    bson bshits1;
    bson_init_as_query(&bshits1);
    bson_append_start_object(&bshits1, "$fields");
    bson_append_int(&bshits1, "_id", 1);
    bson_append_int(&bshits1, "key", 1);
    bson_append_finish_object(&bshits1);
    bson_finish(&bshits1);

    EJQ *q1 = ejdbcreatequery(flsEJ[nFile].ejDB, &bsq1, NULL, 0, &bshits1 );
    ErrorIf( !q1, "TEJDB0013", "Error in query (test)" );

    uint32_t count = 0;
    TCXSTR *log = tcxstrnew();
    TCLIST *q1res = ejdbqryexecute(coll, q1, &count, 0, log);
    //fprintf(stderr, "%s", TCXSTRPTR(log));

cout << count << " records in collection " << GetKeywd() << endl;
    for (int i = 0; i < TCLISTNUM(q1res); ++i)
    {
        void *bsdata = TCLISTVALPTR(q1res, i);
        //bson_iterator_from_buffer(&it, (char *)bsdata);
        // added record key to list
        KeyFromBson( (const char *)bsdata, nFile );
    }

    bson_destroy(&bsq1);
    tclistdel(q1res);
    tcxstrdel(log);
    ejdbquerydel(q1);

    closeCollection( nFile );
}

/// Makes a list of open file names
void TEJDataBase::opfils()
{
    fNum = 0;
    fOpenNameBuf.clear();
    for(int i=0; i<fls.size(); i++)
        fOpenNameBuf.push_back( flsEJ[fls[i]].Name() );
}

/// Open EJDB files and build linked record list
void TEJDataBase::Open()
{
    int j;
    if( flsEJ.size() == 0 )
        return;

    if( fls.size()<=0 ) // no config
       fls.push_back(0);
    recList.clear();
    itrL= recList.end();

try
  {
    for( j=0; j<fls.size(); j++ )
    {
        loadCollection( fls[j] );
    }
    opfils();
  }
  catch( TError& xcpt )
  {
        if( xcpt.title == "TEJDB0014" )
        {
           xcpt.mess += "\n 2nd record in file: ";
           xcpt.mess +=  flsEJ[fls[j]].Name();

           // delete all records loaded from last file
           delfile( j );
           // delete unloaded files
           fls.erase(fls.begin()+j, fls.end() );
           opfils();
         }
         Error( xcpt.title, xcpt.mess);
   }
}

/// Open EJDB files use list nff
void TEJDataBase::Open( vector<int>& nff )
{
    if( flsEJ.size() == 0 )
        return;

    if( fls.size()<=0 ) //no config
    {
        if( nff.size()<1 )
                fls.push_back(0);
        for(int j=0; j< nff.size(); j++)
                fls.push_back( nff[j] );
    }
    Open();
}

/// Close files all EJDB files
void TEJDataBase::Close()
{
    for(int j=0; j<fls.size(); j++ )
        flsEJ[fls[j]].Close();
    fls.clear();
    fNum = 0;
    fOpenNameBuf.clear();
    recList.clear();
    itrL= recList.end();
}

// add open PDB files
void TEJDataBase::AddOpenFile(const vector<int>& nff)
{
  int j=0;
  if( nff.size()<1 )
        return;
try{
    for( j=0; j<nff.size(); j++)
    {
        loadCollection( nff[j] );
        fls.push_back( nff[j] );
    }
    opfils();
  }
  catch( TError& xcpt )
  {
        if( xcpt.title == "TEJDB0014" )
        {
           xcpt.mess += "\n 2nd record in file: ";
           xcpt.mess +=  flsEJ[nff[j]].Name();

           // delete all records loaded from last file
           delfile( fls.size() );
           opfils();
         }
         Error( xcpt.title, xcpt.mess);
   }
}


//-------------------------------------------------------------
// DataBaseList
//-------------------------------------------------------------

int EJDataBaseList::Find(const char* s)
{
    for( int ii=0; ii<size(); ii++ )
        if( at(ii).GetKeywd() == s  )
            return ii;
    return -1;
}

// default configuration
void EJDataBaseList::Init()
{
    // RT_SDATA default
    size_t sdref_rkfrm[3] = { 20, 5, 7 };
    push_back( TEJDataBase( size(), "sdref",
                        o_sdauthr, o_sdauthr+8/*9*/, 3, 3, sdref_rkfrm ) );
    // RT_CONST default
    size_t const_rkfrm[2] = { 8, 24 };
    push_back( TEJDataBase( size(), "const",
                        o_constlab, o_constlab+2/*3*/, 1, 2, const_rkfrm ) );
}

// configuration to file
void EJDataBaseList::toCFG(fstream& out_stream)
{
    int nR = size();
    out_stream << nR << endl;

    for(int ii=0; ii<size(); ii++)
        at(ii).toCFG( out_stream );
}

// configuration from file
void EJDataBaseList::fromCFG(fstream& in_stream)
{
    int nDB;

    in_stream >> nDB;
    for( int ii=0; ii<nDB; ii++)
        push_back( TEJDataBase( in_stream ) );  // Add( new TDataBase( in_stream ) );
}

TEJDataBase&
EJDataBaseList::operator[](int ii)
{
    ErrorIf( ii > size(),
             "DataBaseList","Invalid chain index.");
    return at(ii);
}

/// Set up files_data  (lists of all kernel files names and  opened kernel files )
void EJDataBaseList::buildFilesList( elmFilesConfData& files_data,
                                   const vector<string>& selNames )
{
   size_t pos1, pos2;
   int cnt, cnt_sel;
   vector<string> selkeywds;

   // make selected indexes for each collections
   getOpenFiles( selNames, selkeywds );

   for(int i=RT_SDATA; i<=RT_PHASE; i++ )
    {
       cnt = 0;
       cnt_sel = 0;
       if( i != RT_PARAM )
       { vector<string> names;
         vector<int> indx;
         vector<int> sel;
         at(i).GetFileList(closef|openf|oldself, names, indx, sel);
         for(int ii=names.size()-1; ii>=0; ii-- )
         {
          // select only DB.default files
            if( !flsEJ[indx[ii]].isDefaultDB() )
              continue;
          // get 2 colums
          pos1 = names[ii].find_first_of(" ");
          pos2 = names[ii].rfind("/");
          string fkeywd =  names[ii].substr( 0, pos1 );
          files_data.flKeywds.push_back( fkeywd );
          files_data.flNames.push_back(  names[ii].substr( pos2+1 ) );
          cnt++;

          if( find( selkeywds.begin(), selkeywds.end(), fkeywd.substr(2) ) != selkeywds.end() )
          {
            files_data.selKeywds.push_back( fkeywd );
            cnt_sel++;
          }
         }
        }
        files_data.flCnt.push_back( cnt );
        files_data.selCnt.push_back( cnt_sel );
    }

   files_data.ftree = flsEJ.ftree.get();
}

/// Returns list of indexes selected files
void EJDataBaseList::getOpenFiles( const vector<string>& selNames, vector<string>& selkeywds )
{
    int jj, ii;
    string fname;

    selkeywds.clear();

    for( jj=0; jj<flsEJ.size(); jj++ )
    {
        if(!flsEJ[jj].isDefaultDB() )
          continue;
        fname = flsEJ[jj].Name();
        fname = replace_all( fname, "_", ".");
        fname = "."+fname;

        for( ii=0; ii < selNames.size(); ii++ )
        {
             if(  fname.find( selNames[ii] ) != string::npos )
             {  selkeywds.push_back(flsEJ[jj].GetKeywd()); break; }

             if(  selNames[ii].find(fname) != string::npos )
             {  selkeywds.push_back(flsEJ[jj].GetKeywd()); break; }
         }
    }
}

/// Reset list of opened kernel files
void EJDataBaseList::resetFilesSelection(elmFilesConfData& files_data,
                                       const vector<string>& selNames)
{
  vector<string> selkeywds;
  int cnt=0;

  // make selected indexes for each collections
  getOpenFiles( selNames, selkeywds );
  files_data.selKeywds.clear();
  files_data.selCnt.clear();

  for(int i=0; i<files_data.flCnt.size(); i++ )
  {
        int cnt_sel = 0;
        for(int ii=0; ii<files_data.flCnt[i]; ii++ )
        {
            string fkeywd = files_data.flKeywds[cnt+ii];
            if( find( selkeywds.begin(), selkeywds.end(), fkeywd.substr(2) ) != selkeywds.end() )
            {
              files_data.selKeywds.push_back( fkeywd );
              cnt_sel++;
            }
       }
      cnt += files_data.flCnt[i];
      files_data.selCnt.push_back( cnt_sel );
  }
}

EJDataBaseList rtEJ;

// --------------------------------------------------------------
/*
void TEJDataBase::check_file( int nF )
{
    ErrorIf( nF<0 || nF>aFile.size(),
             GetKeywd(),"Invalid file number.");
}


//-------------------------------------------------------------
// TEJDBKeyList - working with DB keys list
//-------------------------------------------------------------


* Set key current record to work key structure
void TEJDBKeyList::PutKey( const list<IndexEntry>::iterator it)
{
    for( int ii=0; ii<KeyNumFlds(); ii++)
        SetFldKey( ii, it->keyFlds[ii].c_str() );
}
*

/// Set key current record to work key structure
void TEJDBKeyList::PutKey()
{
    for( int ii=0; ii<KeyNumFlds(); ii++)
        SetFldKey( ii, itnI->keyFlds[ii].c_str() );
}

/// Put key ndx record to kbuf in unpack form.
void TEJDBKeyList::recKey(const list<IndexEntry>::iterator it, string& kbuf )
{
    string sp;
    size_t len;
    kbuf.assign( fullLen, ' ' );

    for(int ii=0; ii<KeyNumFlds(); ii++)
    {
        sp = it->keyFlds[ii];
        len =  min( sp.length(), rkLen[ii] );
        kbuf.replace( rkInd[ii], len, sp, 0, len );
    }
}


void TEJDBKeyList::check_ndx(int ndx)
{
    if( ndx < 0 || ndx > RecCount() )
        Error( "TEJDBMKeyList","Invalid record index in the chain.");
}
*/

/// Documentation

/**
 * Persist BSON object in the collection.
 * If saved bson does't have _id primary key then `oid` will be set to generated bson _id,
 * otherwise `oid` will be set to the current bson's _id field.
 *
 * NOTE: Field names of passed `bs` object may not contain `$` and `.` characters,
 *       error condition will be fired in this case.
 *
 * @param coll JSON collection handle.
 * @param bson BSON object id pointer.
 * @param oid OID pointer will be set to object's _id
 * @return If successful return true, otherwise return false.
 */
///EJDB_EXPORT bool ejdbsavebson(EJCOLL *coll, bson *bs, bson_oid_t *oid);

/**
 * Persist BSON object in the collection.
 * If saved bson does't have _id primary key then `oid` will be set to generated bson _id,
 * otherwise `oid` will be set to the current bson's _id field.
 *
 * NOTE: Field names of passed `bs` object may not contain `$` and `.` characters,
 *       error condition will be fired in this case.
 *
 * @param coll JSON collection handle.
 * @param bs BSON object id pointer.
 * @param oid OID pointer will be set to object's _id
 * @param merge If true the merge will be performend with old and new objects. Otherwise old object will be replaced.
 * @return If successful return true, otherwise return false.
 */
///EJDB_EXPORT bool ejdbsavebson2(EJCOLL *jcoll, bson *bs, bson_oid_t *oid, bool merge);

/**
 * Remove BSON object from collection.
 * The `oid` argument should points the primary key (_id)
 * of the bson record to be removed.
 * @param coll JSON collection ref.
 * @param oid BSON object id pointer.
 * @return
 */
///EJDB_EXPORT bool ejdbrmbson(EJCOLL *coll, bson_oid_t *oid);

/**
 * Load BSON object with specified 'oid'.
 * If loaded bson is not NULL it must be freed by bson_del().
 * @param coll
 * @param oid
 * @return BSON object if exists otherwise return NULL.
 */
///EJDB_EXPORT bson* ejdbloadbson(EJCOLL *coll, const bson_oid_t *oid);
