//-------------------------------------------------------------------
// $Id: v_dbm.cpp 1179 2009-01-20 10:27:35Z gems $
//
// Implementation of TDBFile and TDataBase classes
//
// Copyright (C) 1996-2013 S.Dmytriyeva, A.Rysin
//
// This file is part of the GEM-Selektor GUI library which uses the
// Qt v.4 cross-platform App & UI framework (http://qt.nokia.com)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the terms of GEMS4 Development
// Quality Assurance Licence (GEMS4.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#include "v_dbm.h"
#include "v_user.h"
#include "v_mod.h"
#include "visor.h"
#include "service.h"
#include "gdatastream.h"
#include "v_object.h"


//#define Change_DB_Mode   1 // Set readonly data Base mode
const char* ALLKEY  = "*";
const char* MARKRECDEL = "##";
const char* MARKRECHEAD = "$$";
const char MARKRKEY  = '#';

//-------------------------------------------------------------
// TDataBase
//-------------------------------------------------------------

//-------------------------------------------------------------
// TEJDataBase - This class contened the structure of EJDB Data Base
//-------------------------------------------------------------

// Put state to cfg file
void TDataBase::toCFG(fstream& out_stream)
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
   out_stream << specialFilesNum <<  "  ";
out_stream << rclose << ' '; // endl;
out_stream << isDelete << "  "; // endl;

     // opened files definition
   out_stream << fls.size() << "  ";
   for( ii=0; ii<fls.size(); ii++)
        out_stream << fls[ii] << ' ';
// DB files
   int numFiles = aFile.size();
   out_stream << "  " << numFiles << endl;
   ErrorIf( !out_stream.good(), GetKeywd(),
             "Error writing TDataBase to configurator");
    for( ii=0; ii<numFiles; ii++)
        aFile[ii]->toCFG(out_stream);
}

// Get state from cfg file
void TDataBase::fromCFG(fstream& in_stream)
{
    int ii;
    // DB definition
    in_stream >> Keywd;
    in_stream >> nRT;
    in_stream >> frstOD;
    in_stream >> lastOD;
    in_stream >> specialFilesNum;
in_stream >> rclose;
in_stream >> isDelete;
    // opened files
    int numFiles, fl;
    in_stream >> numFiles;
    for( ii=0; ii<numFiles; ii++)
    {
        in_stream >> fl;
        fls.push_back(fl);
    }
// DB files
    in_stream >> numFiles;
    ErrorIf( !in_stream.good(), GetKeywd(),
             "Error reading TDataBase from configurator");
    for( ii=0; ii<numFiles; ii++)
        aFile.push_back( new TDBFile(in_stream) );
}

/// Default configuration of the Data Base
TDataBase::TDataBase( int nrt, const char* name,
                          bool Rclose, bool isDel,
                          int firstObj, int lastObj, int spflNum,
                          int nRkflds, const size_t* rkfrm  ):
    Keywd( name ), nRT( nrt ), frstOD(firstObj), lastOD(lastObj),
    key( nRkflds, rkfrm ), status( UNDF_),
    specialFilesNum(spflNum), fNum(-1),
    rclose(Rclose), isDelete(isDel)
{
    crt = time(NULL);
}

/// Configuration from cfg file
TDataBase::TDataBase( fstream& fcfg ):
        key( fcfg ), status(UNDF_), fNum(-1)
{
    fromCFG(fcfg);
    crt = time(NULL);
}

TDataBase::~TDataBase()
{
    /*
      for(int j=0; j<fls.size(); j++ )
        aFile[fls[j]]->Close();
      fls.clear();
      fOpenNameBuf.clear();
    */

}

int TDataBase::findFnum( int nF )
{
    for(int ii = 0; ii< fls.size(); ii++ )
    {
        if( fls[ii] == nF )
          return ii;
    }
    return -1;
}


#ifdef Change_DB_Mode
bool TDataBase::dbChangeAllowed( int /*nf*/ )
{
    return true;
}
#else
bool TDataBase::dbChangeAllowed( int nf )
{
    return ( pVisor->isDBChangeMode() )
        || ( nf >= specialFilesNum );
}
#endif

//--- Manipulation records

/* Save current record to bson structure
void TDataBase::RecToBson( bson *obj )
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
    for( ii=0, no=frstOD; no<=lastOD;  no++, ii++)
    {
           sprintf(buf, "%d", ii);
           bson_append_start_object( obj, buf);
           bson_append_int( obj, "id", no );
           aObj[no].toBsonObject( obj );
           bson_append_finish_object( obj );
    }
    bson_append_finish_array(obj);

    bson_finish( obj );
}

// Load data from bson structure
void TDataBase::RecFromBson( bson *obj )
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
}
*/

/* Save current record to txt file
void TDataBase::RecToTxt( fstream& ftxt )
{
    aObj[o_reckey].SetPtr( const_cast<char *>(PackKey()) );
    aObj[o_reckey].toTXT(ftxt);
    for( int no=frstOD; no<=lastOD;  no++ )
         aObj[no].toTXT( ftxt );
}

// Loads Data Record from txt-file
void TDataBase::RecFromTxt( fstream& ftxt, int &fnum )
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
bool TDataBase::Find( const char *pkey )
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
bool TDataBase::FindPart( const char *key_, int field )
{
    TEJDBKey wkey(key);
    wkey.SetKey( key_ );
    wkey.SetFldKey(field,"*");
    RecStatus iRet = Rtest( wkey.PackKey(), 0 );
    return ( iRet==MANY_ || iRet==ONEF_ );
}

// Retrive one record from the collection
void TDataBase::Get( const char *pkey )
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

    RecEntry* rh= &itrL->second;
    RecHead rhh;

    // test and open file
    int nF = rh->nFile;
    check_file( nF );

    aFile[nF]->Open();
    getrec( *rh, aFile[nF]->f, rhh );

    // Set up internal data
    status = ONEF_;
    fNum = findFnum( nF );
    if( rclose )  aFile[nF]->Close();
}

/// Removes record from the collection
void TDataBase::Del( const char *pkey )
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

    RecEntry* rh=&itrL->second;
    // test and open file
    int nF = rh->nFile;
    check_file( nF );
    if( !dbChangeAllowed( nF ) )
    {
        string erstr = "Cannot delete record:\n";
               erstr += pkey;
               erstr += ".\nChanges to system database file\n";
               erstr += aFile[nF]->Name().c_str();
               erstr += " are not allowed!";
        Error("TEJDB0003", erstr );
    }
    aFile[nF]->Open();
    // delete record
    rh->len += RecHead::data_size();
    aFile[nF]->AddSfe( *rh );
    recList.erase(itrL);
    aFile[nF]->PutHead( aFile[nF]->f, -1);
    putndx( nF );

    // Set up internal data
    itrL = recList.end();
    status = UNDF_;
    fNum = findFnum( nF );
    if( rclose )  aFile[nF]->Close();
}

/// Save new record in the collection
/// fnum - index into internal file list
void TDataBase::AddRecord(const char* pkey, int& _fnum  )
{
    int fnum = _fnum;

    // Select current file name if fnum == -1 - undefined
    if( fnum < 0 )
        fnum = getCurrentFile( _fnum );
    if( _fnum == -2 ) // cancel
       return;
    int nF = fls[fnum];

    // Try to insert new record to list
    key.SetKey( pkey );
    if( key.isPattern() )
      Error("TEJDB0010", "Cannot save under record key template" );

    pair<map<IndexEntry, RecEntry>::iterator,bool> ret;

    RecEntry re_(-1, -1, nF );
    ret = recList.insert( pair<IndexEntry, RecEntry>(key.retIndex(),re_) );

    //ret = recList.insert( key.retIndex() );
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
    if( !dbChangeAllowed( nF ) )
    {
               recList.erase(itrL);
               string erstr = "Cannot add record:\n";
               erstr += pkey;
               erstr += ".\nChanges to system database file\n";
               erstr += aFile[nF]->Name().c_str();
               erstr += " are not allowed!";
        Error("TEJDB0003", erstr );
    }

    aFile[nF]->Open();
    int len = reclen();
    len += RecHead::data_size() + strlen(PackKey()) + 1;
    RecEntry* rh = &itrL->second;
    rh->len = len;
    aFile[nF]->FindSfe( *rh );
    rh->len = len - RecHead::data_size();
    int oldlen = aFile[nF]->FLen();
    int iRet =putrec( *rh, aFile[nF]->f );
    if( iRet!=0 )
    {
        if( aFile[nF]->FLen() != oldlen )
            aFile[nF]->FNewLen( iRet) ;
        rh->len -=iRet;
    }

    aFile[nF]->PutHead( aFile[nF]->f, 1);
    putndx(nF);

    // Set up internal data
    status = ONEF_;
    fNum = fnum;
    if( rclose )
        aFile[nF]->Close();
}

/// Save/update record in the collection
void TDataBase::SaveRecord(const char* pkey, int& fnum )
{
    if( Find( pkey ) )
    {
        RepRecord( pkey );
    }
    else
        AddRecord( pkey, fnum );
}

/// Save==update record in the collection
void TDataBase::RepRecord(const char* pkey )
{
   int nF;
   // Try to insert new record to list
   key.SetKey( pkey );
   if( key.isPattern() )
     Error("TEJDB0010", "Cannot save under record key template" );

   itrL = recList.find( key.retIndex() );
   ErrorIf(itrL == recList.end(), "RepRecord", "Cannot save" );
   RecEntry* rh = &itrL->second;

   // test and open file
   nF = rh->nFile;
   check_file( nF );
   if( !dbChangeAllowed( nF ) )
       {
           string erstr = "Cannot save record:\n";
                  erstr += pkey;
                  erstr += ".\nChanges to system database file\n";
                  erstr += aFile[nF]->Name().c_str();
                  erstr += " are not allowed!";
           Error("TEJDB0003", erstr );
       }

   aFile[nF]->Open();
   // delete record
   rh->len += RecHead::data_size();
   aFile[nF]->AddSfe( *rh );
   rh->len = reclen() + RecHead::data_size() + strlen( PackKey() ) + 1;
   aFile[nF]->FindSfe( *rh );
   rh->len -= RecHead::data_size();
   int oldlen = aFile[nF]->FLen();
   int iRet =putrec( *rh, aFile[nF]->f );
   if( iRet!=0 )
   {
       if( aFile[nF]->FLen() != oldlen )
           aFile[nF]->FNewLen( iRet );
       rh->len -=iRet;
   }
   aFile[nF]->PutHead( aFile[nF]->f, 0);
   putndx( nF );

     // Set up internal data
     status = ONEF_;
     fNum = findFnum( nF );
     if( rclose )  aFile[nF]->Close();
}

/// Save/update record in the collection
/// Question for replase
/// return true if for all
void TDataBase::SaveRecordQuestion(const char* pkey, int& fnum, bool& yesToAll )
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
RecStatus TDataBase::Rtest( const char *key, int mode )
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

int TDataBase::GetKeyList( const char *keypat,
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

    map<IndexEntry, RecEntry, less<IndexEntry> >::iterator it;
    it = recList.begin();
    while( it != recList.end() )
    {
        if( !AllRecs )
            if( !wkey.compareTemplate( it->first ) )
              { it++;
                continue;
              }
        aKeyList.push_back( wkey.indexKey( retUnpackform, it->first ) );
        if( OneRec )
         break;

        it++;
    }
    return aKeyList.size();
}

// Rename records (Change first field: for SYSEQ, and >)
void TDataBase::RenameList( const char* newName,
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
void TDataBase::MakeKey( unsigned char nRTwrk, string& pkey, ... )
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

// Working with files

/// Delete the keys of records file nF
void TDataBase::delfile( int nF )
{
    map<IndexEntry, RecEntry, less<IndexEntry> >::iterator itdel;
    map<IndexEntry, RecEntry, less<IndexEntry> >::iterator it;
    it = recList.begin();
    while( it != recList.end() )
    {
        if( it->second.nFile == nF )
        {
             itdel = it;
             it++;
             recList.erase(itdel);
        }
        else it++;
    }
}

int TDataBase::getCurrentFile( int &fnum )
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

// From old v_dbm2.h


void TDataBase::check_file( int nF )
{
    ErrorIf( nF<0 || nF>aFile.size(),
             GetKeywd(),"Invalid file number.");
}

// add new file to database chain
void TDataBase::AddFile(const string& path)
{
    aFile.push_back(new TDBFile(path));
}

// write the keys of records to ndx file
void TDataBase::PutKeyList( int nF, GemDataStream& f)
{
    map<IndexEntry, RecEntry, less<IndexEntry> >::iterator it;
    it = recList.begin();
    while( it != recList.end() )
    {
        if( it->second.nFile == nF )
        {
            f.writeArray(  key.indexKey(true, it->first), KeyLen() );
            //for( j=0; j<KeyNumFlds(); j++)
            //   f.writeArray(  RecKeyFld(i,j), FldLen(j) );
            it->second.write(f);    // re[i].write (f);
        }
        it++;
    }
    ErrorIf( !f.good(), "TDBKeyList",  "Index file write error.");
}

// read the keys of records from ndx file
void TDataBase::GetKeyList_i(int nF, int nRec, GemDataStream& f)
{
    RecEntry re_;
    char keyb[300];
    memset( keyb, 0, 300);
    TEJDBKey wkey(key);
    pair<map<IndexEntry, RecEntry>::iterator,bool> ret;

    for( int i=0; i< nRec; i++ )
    {
        f.readArray( keyb, KeyLen());
        re_.read(f);
        re_.nFile = nF;
        wkey.SetKey( keyb );
        ret = recList.insert( pair<IndexEntry, RecEntry>(wkey.retIndex(),re_) );
        if( ret.second == false)
        {
            string erstr = "Cannot add new record:\n";
                   erstr += keyb;
                   erstr += ".\nTwo records with the same key!";
            Error("TEJDB0014", erstr );
        }
    }
    SetKey(ALLKEY); // bugfix: current key for not readed record
    ErrorIf( !f.good(), "TDBKeyList", "Index file read error.");
}
// put information to index file
void TDataBase::putndx( int nF )
{
    string Path;
    GemDataStream f;

    check_file(nF);
    Path = u_makepath( aFile[nF]->Dir(),
                aFile[nF]->Name(), NDX_EXT );
    f.open( Path.c_str(), WRITE_B);
    ErrorIf( !f.good(), Path,"Index file open error");
    aFile[nF]->vdbh_setdt( );
    aFile[nF]->DhWrite(f);
    PutKeyList( nF, f);
}

// get information from index file
void TDataBase::getndx( int nF )
{
    string Path;
    VDBhead dh;
    GemDataStream f;

    check_file( nF );
    Path = u_makepath( aFile[nF]->Dir(),
                aFile[nF]->Name(), NDX_EXT );
    f.open( Path.c_str(), RDONLY_B);
    if( !f.good() )
      Error(  Path,"Index file open error");
    dh.read (f);
    GetKeyList_i( nF, dh.nRec, f);
}

// get record length in the PDB file
int TDataBase::reclen( )
{
    int Olen=0;
    for( int no=frstOD; no<=lastOD;  no++ )
        Olen += aObj[no].lenDB();
    return Olen;
}

// put record in DB file
int TDataBase::putrec( RecEntry& rep, GemDataStream& f )
{
    int len;
    int StillLen;
    RecHead rh;
    string pack_key = PackKey();

    // header of the record
    strncpy( rh.bgm, MARKRECHEAD, 2 );
    strncpy( rh.endm, MARKRECHEAD, 2 );
    rh.nRT = nRT;               //warning: compatibility
    rh.Nobj = lastOD-frstOD+1;
    rh.rlen =  rep.len;
    StillLen = rep.len;
    rh.crt = time( NULL );
    f.seekg(rep.pos, ios::beg );
    rh.write (f);
    // put packed key
    len = pack_key.length();
    pack_key  += MARKRKEY;
    f.writeArray( pack_key.c_str(), (len+1) );
    StillLen -= len+1;
    ErrorIf( !f.good(), GetKeywd(),
             "PDB file write error");
    for( int no=frstOD; no<=lastOD;  no++ )
        StillLen -= aObj[no].toDB( f );
    crt = rh.crt;
    return StillLen;
}

// put record in DB file
int TDataBase::putrec( RecEntry& rep, GemDataStream& f, RecHead& rhh  )
{
    int len;
    int StillLen;
    RecHead rh;
    string pack_key = PackKey();

    // header of the record
    strncpy( rh.bgm, MARKRECHEAD, 2 );
    strncpy( rh.endm, MARKRECHEAD, 2 );
    rh.nRT = nRT;               //warning: compatibility
    rh.Nobj = lastOD-frstOD+1;
    rh.rlen =  rep.len;
    StillLen = rep.len;
    rh.crt = rhh.crt;
    f.seekp(rep.pos, ios::beg );
    rh.write (f);
    // put packed key
    len = pack_key.length();
    pack_key += MARKRKEY;
    f.writeArray( pack_key.c_str(), (len+1) );
    StillLen -= len+1;
    ErrorIf( !f.good(), GetKeywd(),
             "PDB file write error");
    for( int no=frstOD; no<=lastOD;  no++ )
        StillLen -= aObj[no].toDB( f );
    crt = rh.crt;
    return StillLen;
}


// Gets a record from PDB file
int TDataBase::getrec( RecEntry& rep, GemDataStream& f, RecHead& rh )
{
    int StillLen;
    // RecHead rh;
    char keybuf[300];
    //string key = PackKey();

    StillLen = rep.len;
    f.seekg(rep.pos, ios::beg );
    rh.read (f);
    if( strncmp( rh.bgm, MARKRECHEAD, 2 ) ||
            strncmp( rh.endm, MARKRECHEAD, 2 ) ||
            (rh.Nobj != lastOD-frstOD+1 &&
             ( (rh.Nobj) != o_tpstr-frstOD+1  &&
               (rh.Nobj) != o_phsdval-frstOD+1  &&
               (rh.Nobj) != o_sptext-frstOD+1 ) ) )
        Error( GetKeywd(),"Record header format error");

    f.getline( keybuf, KeyLen()+KeyNumFlds(), MARKRKEY);
    ErrorIf( f.gcount()>=(KeyLen()+KeyNumFlds()), GetKeywd(),
             "Error reading database record key" );
    SetKey(keybuf);
    StillLen -= f.gcount();  //???+1
    // test
    bool flag_spppar = false;
    for( int no=frstOD; no<=lastOD;  no++ )
    {   // get objects from file
        if ( no == o_phstr2  )
            if( StillLen < sizeof(short)*16 )     // old record of phase
               break;

        if ( no == o_tpstr )
          if( StillLen < 28 )
             continue;
       if ( no == o_sptext )   //12.12.12
          if( StillLen < 10 )
             continue;

        StillLen -= aObj[no].ofDB(f);
        aObj[no].checkUtf8();
        if (no == o_spppar )
            flag_spppar = true;
    }
    if( StillLen!=0 && rep.len!=0 && !flag_spppar)
        Error( GetKeywd(),
          "Actual record length differs from that specified in the header");
    crt = rh.crt;
    return StillLen;
}


//Read record update time from PDB file to memory.
time_t TDataBase::GetTime( const char *pkey )
{
    TEJDBKey wkey(key);
    wkey.SetKey( pkey );
    itrL = recList.find( wkey.retIndex() );

    if(itrL == recList.end() )
        return time( NULL );

    int nF;
    RecEntry* ree=&itrL->second;// ind.RecPosit(i);
    RecHead rh;

    // test and open file
    nF = ree->nFile;
    check_file( nF );

    aFile[nF]->Open();
    aFile[nF]->f.seekg(ree->pos, ios::beg );
    //   aFile[nF]->f.read( (char *)&rh, sizeof(RecHead) );
    rh.read (aFile[nF]->f);
    if( strncmp( rh.bgm, MARKRECHEAD, 2 ) ||
            strncmp( rh.endm, MARKRECHEAD, 2 ) || rh.Nobj != lastOD-frstOD+1 )
        Error( GetKeywd(), "PDB record header format error");
    status =  UNDF_;
    ;
    fNum = findFnum( nF );
    if( rclose )  aFile[nF]->Close();
    return rh.crt;
}


//Create new PDB file.
void TDataBase::Create( int nF )
{
    check_file( nF );
    if( !aFile[nF]->Exist() )
    {
        aFile[nF]->Create( nRT, isDelete );
        putndx( nF );    // indexes
    }

}

// Makes a list of open file names
void TDataBase::opfils()
{
    fNum = 0;
    fOpenNameBuf.clear();
    for(int i=0; i<fls.size(); i++)
        fOpenNameBuf.push_back( aFile[fls[i]]->Name() );
}

/// Selection in the list of full file names
void TDataBase::GetFileList(int mode, vector<string>& names,
                            vector<int>& indx,  vector<int>& sel)
{
    names.clear();
    indx.clear();
    sel.clear();
    for( int i=0; i<aFile.size(); i++ )
    {
        int nF = findFnum( i );
        if( (nF==-1&&(mode&closef))||(nF!=-1&&(mode&openf)) )
        {
            aFile[i]->Makepath();
            names.push_back( string(aFile[i]->GetKeywd())+string(" ")+
                       string(aFile[i]->GetPath()));
            indx.push_back(i);
            if( (mode&oldself) && nF != -1) //select already open files
                sel.push_back(indx.size()-1);
        }
    }
}

// open files in PDB
void TDataBase::Open()
{
    int j;
    if( aFile.size() == 0 )
        return;

    if( fls.size()<=0 ) //no config
       fls.push_back(0);

    recList.clear();
    itrL= recList.end();
    for( j=0; j<fls.size(); j++ )
        Create( fls[j] );
try
  {
    vector<int> comp;
    for( j=0; j<fls.size(); j++ )
    {
        aFile[fls[j]]->Open();
        if( aFile[fls[j]]->GetDhOver())
             comp.push_back(fls[j]);
        getndx( fls[j] );
        if( rclose )  aFile[fls[j]]->Close();
    }
    opfils();
    if( comp.size() > 0 )
         RebildFile( comp);
  }
  catch( TError& xcpt )
  {
        if( xcpt.title == "TEJDB0014" )
        {
           xcpt.mess += "\n 2nd record in file: ";
           xcpt.mess +=  aFile[fls[j]]->Name();

           // delete all records loaded from last file
           delfile( j );
           // delete unloaded files
           fls.erase(fls.begin()+j, fls.end() );

           opfils();
         }
         Error( xcpt.title, xcpt.mess);
   }
}

// open files in PDB
void TDataBase::Open( vector<int>& nff )
{
    if( aFile.size() == 0 )
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

// open all files in PDB chain
void TDataBase::OpenAllFiles( bool only_kernel )
{
    int j;
    if( aFile.size() == 0 )
        return;

    Close();
    for( j=0; j<aFile.size(); j++)
     if( only_kernel && (j >=  specialFilesNum) )
       continue;
     else
        fls.push_back( j );

    Open();
}


//close files in PDB
void TDataBase::Close()
{
    for(int j=0; j<fls.size(); j++ )
        aFile[fls[j]]->Close();
    fls.clear();
    fNum = 0;
    fOpenNameBuf.clear();
    recList.clear();
    itrL= recList.end();
}


//close files in PDB
void TDataBase::OpenOnlyFromList( const vector<string>& names )
{
    if( aFile.size() == 0 )
        return;

    Close();
    int ii, jj;

    for( ii=0; ii< aFile.size(); ii++)
    {
      for( jj=0; jj< names.size(); jj++)
        if(  aFile[ii]->Name().find( names[jj] ) != string::npos )
         break;
      if( jj < names.size() )
         fls.push_back( ii );
    }

    Open();
}

// add new file to DBfile list
int TDataBase::AddFileToList(TDBFile* file)
{
    aFile.push_back( file );
try{
    Create(aFile.size()-1);
    file->Open();
    getndx(aFile.size()-1);
    fls.push_back(aFile.size()-1);
    if( rclose )
        file->Close();
    opfils();
 }
 catch( TError& xcpt )
    {
          if( xcpt.title == "TEJDB0014" )
          {
             xcpt.mess += "\n 2nd record in file: ";
             xcpt.mess +=  aFile.back()->Name();

             // delete all records loaded from last file
             delfile( fls.size() );
           }
           Error( xcpt.title, xcpt.mess);
     }
  return findFnum( aFile.size()-1 );
}


// delete a file from DBfile list
void TDataBase::DelFile(const string& path)
{
    TDBFile fl(path);

    for(int ii=0; ii<aFile.size(); ii++)
        if( fl.Name() == aFile[ii]->Name() )
        {
            int  nF = ii;
            int  nFls = findFnum( nF );
            if( nFls != -1 ) //close file and delete indexes
            {
                aFile[nF]->Close();
                delfile( nF );
                fls.erase( fls.begin()+nFls);
            }
            fNum = 0; //?????
            aFile.erase(aFile.begin()+nF);
            opfils();
            break;
        }
}


// add open PDB files
void TDataBase::AddOpenFile(const vector<int>& nff)
{
  int j=0;
  if( nff.size()<1 )
        return;
try{
    vector<int> comp;

    for( j=0; j<nff.size(); j++)
    {
        Create( nff[j] );
        aFile[nff[j]]->Open();
        if( aFile[nff[j]]->GetDhOver())
         if( vfQuestion( 0, aFile[nff[j]]->GetPath(),
         "Stack of deleted records overflow.\nCompress?" ))
             comp.push_back(nff[j]);
        // end added
        getndx( nff[j] );
        fls.push_back( nff[j] );
        if( rclose )
            aFile[nff[j]]->Close();
    }
    opfils();
        if( comp.size() >0 )
         RebildFile( comp);
    // end added

  }
  catch( TError& xcpt )
  {
        if( xcpt.title == "TEJDB0014" )
        {
           xcpt.mess += "\n 2nd record in file: ";
           xcpt.mess +=  aFile[nff[j]]->Name();

           // delete all records loaded from last file
           delfile( fls.size() );
           opfils();
         }
         Error( xcpt.title, xcpt.mess);
   }
}


// Scan database file
int TDataBase::scanfile( int nF, int& fPos, int& fLen,
		GemDataStream& inStream, GemDataStream& outStream)
{
    RecEntry recordEntry;
    RecHead recordHead;
    string str;
    //RecEntry& rep = ind.RecPosit(0);
    int len, fEnd = fPos;
    int ni, nRec=0;
    pair<map<IndexEntry, RecEntry>::iterator,bool> ret;

    recordEntry.nFile = (unsigned char)nF;             //warning
    recordEntry.len = 0;
    while( fPos < fLen )
    {
       char ch;
       inStream.seekg( fPos, ios::beg );
       inStream.get(ch);
       if( ch != MARKRECHEAD[0] )
        {
            fPos++;
            continue;
        }
        inStream.get(ch);
        if( ch != MARKRECHEAD[0] )
        {
            fPos+=2;
            continue;
        }
        recordEntry.pos = fPos;

        try
        {
            len = getrec(recordEntry, inStream, recordHead);
        }
        catch( TError& /*xcpt*/ )
        {
            fPos += 2;
            continue;
        }
        fPos += (-len) + RecHead::data_size();
        len = reclen() + strlen(PackKey()) + 1;
        // test for existing records
        if( Find( PackKey() ) ) // name of exist record
        {
           str=PackKey();
           str = aMod[nRT]->GetKeyofRecord( str.c_str(),
                      "This key record already exists! Replace please?", KEY_NEW );
           if(  str.empty() )
                continue;
           key.SetKey( str.c_str());
         }
        RecEntry re_(fEnd, len, nF);
        ret = recList.insert( pair<IndexEntry, RecEntry>(key.retIndex(),re_) );
        itrL = ret.first;
        if( ret.second == false )
         continue;
        //ni = ind.addndx( nF, len, ind.PackKey() );
        //ind.RecPosit(ni)->pos = fEnd;
        fEnd += len + RecHead::data_size();

        putrec( itrL->second, outStream, recordHead );
        nRec++;
        pVisor->Message( 0, 0, "Compressing database file. "
        		"Please, wait...", (int)fPos, (int)fLen);
    }
    fLen = fEnd;
    return nRec;
}

//Get new index file and physically deleted records
void TDataBase::RebildFile(const vector<int>& nff)
{
    vector<int> fls_old;
    for(int j=0; j< fls.size(); j++)
        fls_old.push_back( fls[j] );
    Close();

    for(int j=0; j<nff.size(); j++)
    {
       int  nRec, nRT, isDel;
       int fPos, fLen;
       unsigned char nF = (unsigned char)nff[j];
        // test and open file
        check_file( nF );
        if( !dbChangeAllowed( nF ) )
        {
            ///vfMessage( aFile[nF]->Name(), "Cannot rebuild PDB file!");
            continue;
        }
        aFile[nF]->Open();

        //ind.delfile( nF );
        aFile[nF]->GetDh( fPos, fLen, nRT, isDel );

        string tmpFileName = aFile[nF]->GetPath() + ".tmp";
        GemDataStream outStream( tmpFileName, ios::out | ios::binary );
        for(int ii=0; ii<fPos; ii++ )
          outStream.put(0);

         outStream.seekp( fPos/*DBhead::data_size()*/, ios::beg);

         nRec = scanfile( nF, fPos, fLen, aFile[nF]->f, outStream );

         outStream.close();

         aFile[nF]->OpenFromFileName( tmpFileName );

         aFile[nF]->SetDh( fLen, nRec, nRT, isDel );

          putndx( nF );
          status = UNDF_;

          aFile[nF]->Close();
          delfile( nF );
      }

     pVisor->CloseMessage();
     Open( fls_old );
}

//Set new open Files list  (Reopen by list)
bool TDataBase::SetNewOpenFileList(const vector<string>& aFlKeywd)
{
    bool allOpend = true;
    
    if(aFlKeywd.size() < 1 )
        return  allOpend;

    Close();
    for(int i=0; i<aFlKeywd.size(); i++)
    {
        int nF = -1;
        for(int j=0; j<aFile.size(); j++)
        {
            //cout << aFlKeywd[i].compare( aFile[j]->GetKeywd())
            //     << " " << aFlKeywd[i].c_str()
            //     << " " << aFile[j]->GetKeywd() << endl;
            if( aFlKeywd[i] == aFile[j]->GetKeywd() )
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

    Open(fls);
    return allOpend;
}

//Make new project-file and close another project files (make new Project)
void TDataBase::MakeInNewProfile(const string& dir,
   const string& prfName, const char * f_name )
{
    /* open only default files (no user files)*/
    //  Close();
    vector<int> fl;
    for(int  i=0; i<fls.size(); i++ )
    {
        if( fls[i] < specialFilesNum)
        {
            fl.push_back(fls[i]);
        }
    }
    Close();
    for(int  ii=0; ii<fl.size(); ii++ )
        fls.push_back(fl[ii]);
    if( fls.size() >= 1)
        Open();

    /* add new project files*/
    string name(dir);
    name += "/";

    if( f_name == 0 || *f_name == '\0' )
    {  name += GetKeywd();
       name += ".";
       name += prfName;
       name += ".";
       name += PDB_EXT;
    }
    else
       name += f_name;
   TDBFile *aFl = new TDBFile( name );
   AddFileToList( aFl );
}

// get open file number by name (0 if not found )
int TDataBase::GetOpenFileNum( const char* secondName )
{
    string name = GetKeywd();
    name += ".";
    name +=secondName;
    for(int i=0; i<fls.size(); i++)
        if( name == aFile[fls[i]]->Name() )
            return i;
    return 0;
}

// get list of files keywds, that contained name
void TDataBase::GetProfileFileKeywds( const char *_name, vector<string>& aFlkey )
{
    string name = ".";
    name +=_name;
    name += ".";

    for(int ii=0; ii< aFile.size(); ii++)
    {
      if(  aFile[ii]->GetPath().find( name ) != string::npos )
          aFlkey.push_back( aFile[ii]->GetKeywd() );
    }
}

// true if opened some files from default data base
bool TDataBase::ifDefaultOpen() const
{
    for(int i=0; i<fls.size(); i++)
        if( aFile[fls[i]]->isDefaultDB() )
            return true;
    return false;
}

//-------------------------------------------------------------
// DataBaseList
//-------------------------------------------------------------

int DataBaseList::Find(const char* s)
{
    for( int ii=0; ii<size(); ii++ )
        if( at(ii).GetKeywd() == s  )
            return ii;
    return -1;
}

// default configuration
void DataBaseList::Init()
{
    // RT_SDATA default
    size_t sdref_rkfrm[3] = { 20, 5, 7 };
    push_back( TDataBase( size(), "sdref", false, true,
                        o_sdauthr, o_sdauthr+8/*9*/, 3, 3, sdref_rkfrm ) );
    // RT_CONST default
    size_t const_rkfrm[2] = { 8, 24 };
    push_back( TDataBase( size(), "const", false, true,
                        o_constlab, o_constlab+2/*3*/, 1, 2, const_rkfrm ) );

}

// configuration to file
void DataBaseList::toCFG(fstream& out_stream)
{
    int nR = size();
    out_stream << nR << endl;

    for(int ii=0; ii<size(); ii++)
        at(ii).toCFG( out_stream );
}

// configuration from file
void DataBaseList::fromCFG(fstream& in_stream)
{
    int nDB;

    in_stream >> nDB;
    for( int ii=0; ii<nDB; ii++)
        push_back( TDataBase( in_stream ) );  // Add( new TDataBase( in_stream ) );
}

TDataBase&
DataBaseList::operator[](int ii)
{
    ErrorIf( ii > size(),
             "DataBaseList","Invalid chain index.");
    return at(ii);
}

/// Set up files_data  (lists of all kernel files names and  opened kernel files )
void DataBaseList::buildFilesList( elmFilesConfData& files_data,
                                   const vector<string>& selNames )
{
   size_t pos1, pos2;
   int cnt, cnt_sel, ind;

   for(int i=RT_SDATA; i<=RT_PHASE; i++ )
    {
        vector<string> names;
        vector<int> indx;
        vector<int> sel;
        at(i).GetFileList(closef|openf|oldself, names, indx, sel);
        cnt = 0;
        cnt_sel = 0;
        for(int ii=names.size()-1; ii>=0; ii-- )
        {
          // select only DB.default files
          if( names[ii].find( pVisor->sysDBDir())== string::npos )
              continue;
          // get 2 colums
          pos1 = names[ii].find_first_of(" ");
          pos2 = names[ii].rfind("/");
          files_data.flKeywds.push_back( names[ii].substr( 0, pos1 ) );
          files_data.flNames.push_back( names[ii].substr( pos2+1 ).c_str() );
          cnt++;
          ind = files_data.flKeywds.size()-1;
          if( i == RT_SDATA || i == RT_CONST ||
              isOpenFile( selNames, files_data.flNames[ind] ) == 1 )
          {
            files_data.selKeywds.push_back( files_data.flKeywds[ind] );
            cnt_sel++;
          }
        }
        files_data.flCnt.push_back( cnt );
        files_data.selCnt.push_back( cnt_sel );
    }

   setTreeWidget( files_data);

}

/// Returns; boolean true if a keyword was found in the file name, false otherwise
///    for each of open file keywords;
int DataBaseList::isOpenFile( const vector<string>& selNames, string& name  )
{
    string fname = name;

    //scip extension
    size_t pos1 = fname.rfind(".");
    fname = fname.substr( 0, pos1 );
    // scip version
    pos1 = fname.find(".ver");
    if( pos1 != string::npos )
      fname = fname.substr(0, pos1 );

    // first tag name of chain
    pos1 = fname.find(".");
    fname = fname.substr( pos1+1 );

    for(int ii=0; ii < selNames.size(); ii++ )
    {
        if(  name.find( selNames[ii] ) != string::npos )
            return 1;

        if(  selNames[ii].find(fname) != string::npos )
            return 1;
    }
    return 0;
}

/// Reset list of opened kernel files
void DataBaseList::resetFilesSelection(elmFilesConfData& files_data,
                                       const vector<string>& selNames)
{
  vector<string> newSelKeywds;   // list of selected files
  vector<int>    newSelCnt;      // count of newly selected files for type
  int cnt=0;
  //int cnt2=0;

   for(int i=0; i<files_data.flCnt.size(); i++ )
    {
        int cnt_sel = 0;
        for(int ii=0; ii<files_data.flCnt[i]; ii++ )
        {
          if( isOpenFile( selNames, files_data.flNames[cnt+ii] ) )
          {
             newSelKeywds.push_back( files_data.flKeywds[cnt+ii] );
             cnt_sel++;
          }
        }
        cnt += files_data.flCnt[i];
        //cnt2 += files_data.selCnt[i];
        newSelCnt.push_back( cnt_sel );
    }

  files_data.selKeywds.clear();
  files_data.selCnt.clear();
  for(int ii=0; ii<newSelCnt.size(); ii++ )
       files_data.selCnt.push_back( newSelCnt[ii] );
  for(int ii=0; ii<newSelKeywds.size(); ii++ )
       files_data.selKeywds.push_back( newSelKeywds[ii] );
}

void DataBaseList::setTreeWidget(elmFilesConfData& files_data)
{
    int ii;
    string fname, tag="defaultDB", vers="";
    size_t pos1, pos2;

    FileTree* mainRow = new FileTree(tag, vers);

    for( ii=0; ii<files_data.flNames.size(); ii++ )
    {
        fname = files_data.flNames[ii];

        //scip extension
        pos1 = fname.rfind(".");
        fname = fname.substr( 0, pos1+1 );

        // get version
        pos1 = fname.find(".ver");
        if( pos1 != string::npos )
        {
            vers = fname.substr(pos1+1+3);
            fname = fname.substr(0, pos1+1 );
            pos2 = vers.rfind(".");
            vers = vers.substr(0, pos2);
        }
        else
            vers = "not versioned";

        // first tag name of chain
        pos1 = fname.find(".");
        fname = fname.substr(pos1);

        mainRow->insertTag( fname, vers, '.' );
    }

    // delete last "."
    tag = ".";
    mainRow->deleteTag( tag);
    files_data.ftree = mainRow;
 }


DataBaseList rtOld;
#ifdef oldDBusing        // Pointer to class contened the structure of main DB
DataBaseList& rt=rtOld;
#else
EJDataBaseList& rt=rtEJ;
#endif


//--------------------- End of v_dbm.cpp ---------------------------

