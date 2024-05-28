//-------------------------------------------------------------------
// $Id: v_dbm2.cpp 1179 2009-01-20 10:27:35Z gems $
//
// Implementation of TDataBase, DataBaseList classes
//
// Copyright (C) 1996-2001 S.Dmytriyeva, A.Rysin
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

#include <QJsonObject>
#include <QJsonArray>

#include "v_user.h"
#include "v_dbm.h"
#include "v_dbfile.h"
#include "v_object.h"
#include "v_mod.h"
#include "visor.h"
#include "service.h"
#include "GEMS3K/gdatastream.h"
#include "v_module.h"


//#define Change_DB_Mode   1 // Set readonly data Base mode

//-------------------------------------------------------------
// TDataBase
//-------------------------------------------------------------

const char* ALLKEY  = "*";
const char* MARKRECDEL = "##";
const char* MARKRECHEAD = "$$";
const char MARKRKEY  = '#';

// Test permission to insert changes to DB file

#ifdef Change_DB_Mode

bool TDataBase::dbChangeAllowed( int /*nf*/, bool /*ifRep*/ )
{
    return true;
}

#else

bool TDataBase::dbChangeAllowed( uint nf, bool /*ifRep*/ )
{
    return ( pVisor->isDBChangeMode() /*&&  ifRep==true*/ )
        || ( nf >= static_cast<uint>(specialFilesNum) );
}

#endif

void TDataBase::check_file( uint nF )
{
    ErrorIf(  nF>aFile.size(), GetKeywd(),"Invalid file number.");
}

//put state to cfg file
void TDataBase::toCFG(fstream& out_stream)
{
    unsigned short numFields = ind.KeyNumFlds();
//    f.write( (char*)&cc, sizeof(unsigned char) );
out_stream << numFields << "  "; // << endl;
    unsigned char* aLen = ind.RkFrm();
//    f.write( (char*)cd, ind.KeyNumFlds()*sizeof(unsigned char) );
for( int ii=0; ii<numFields; ii++)
    out_stream << ( static_cast<unsigned short>(aLen[ii])) << ' ';
out_stream << "  "; //  endl;
//    f.write( Keywd, MAXKEYWD );
out_stream << Keywd << ' '; // endl;
//    f.write( (char*)&nRT, sizeof(unsigned char) );
out_stream << uint(nRT) << "  "; // endl;
//    f.write( (char*)&rclose, sizeof(bool) );
out_stream << rclose << ' '; // endl;
//    f.write( (char*)&isDelete, sizeof(bool) );
out_stream << isDelete << "  "; // endl;
//    f.write( (char*)&frstOD, sizeof(int) );
out_stream << frstOD << ' '; // endl;
//    f.write( (char*)&nOD, sizeof(unsigned char) );
out_stream << uint(nOD) << "  "; // endl;
out_stream << frstODjson << "  "; // endl;
out_stream << lastODjson << "  "; // endl;
//    f.write( (char*)&specialFilesNum, sizeof(int) );
out_stream << specialFilesNum <<  "  "; //  endl;

    int numFls = fls.size();
//    f.write( (char*)&nF, sizeof(int) );
out_stream << numFls << "  "; // << endl;
    for(size_t ii=0; ii<fls.size(); ii++) {
//        f.write( (char*)&fls[i], sizeof(int) );
out_stream << fls[ii] << ' '; // << endl;
    }
    int numFiles = aFile.size();
//    f.write( (char*)&nF, sizeof(int) );
out_stream << "  " << numFiles << endl;

    ErrorIf( !out_stream.good(), GetKeywd(),
             "Error writing TDataBase to configurator");

    for(size_t ii=0; ii<aFile.size(); ii++) {
        aFile[ii]->toCFG(out_stream);
    }
}

//get state from cfg file
void TDataBase::fromCFG(fstream& in_stream)
{
    unsigned int tmp = 0;

//    f.read( Keywd, MAXKEYWD );
in_stream >> Keywd;
//    f.read( (char*)&nRT, sizeof(unsigned char) );
in_stream >> tmp;
nRT = tmp;
//    f.read( (char*)&rclose, sizeof(bool) );
in_stream >> rclose;
//    f.read( (char*)&isDelete, sizeof(bool) );
in_stream >> isDelete;
//    f.read( (char*)&frstOD, sizeof(int) );
in_stream >> frstOD;
//    f.read( (char*)&nOD, sizeof(unsigned char) );
in_stream >> tmp;
nOD = tmp;
in_stream >> frstODjson;
in_stream >> lastODjson;
//    f.read( (char*)&specialFilesNum, sizeof(int) );
in_stream >> specialFilesNum;

    int numFields;
//    f.read( (char*)&nF, sizeof(int) );
in_stream >> numFields;

    int fl;
    for(int i=0; i<numFields; i++)
    {
//        f.read( (char*)&fl, sizeof(int) );
in_stream >> fl;
        fls.push_back(fl);
    }
//    f.read( (char*)&nF, sizeof(int) );
int numFiles;
in_stream >> numFiles;

    ErrorIf( !in_stream.good(), GetKeywd(),
             "Error reading TDataBase from configurator");

    for(int ii=0; ii<numFiles; ii++)
        aFile.push_back( std::make_shared<TDBFile>(in_stream));
}

//default configuration of the database
TDataBase::TDataBase( size_t nrt, const char* name,
                      bool Rclose, bool isDel, int nOf, unsigned char Nobj,
                      int filesNum, unsigned char nRkflds, const unsigned char* rkfrm ):
        nRT(nrt), status(UNDF_), rclose(Rclose), isDelete(isDel),
        frstOD(nOf), nOD(Nobj), frstODjson(frstOD), lastODjson(frstOD+nOD-1),
        ind( nRkflds, rkfrm ),
        aFile(), fls(), specialFilesNum(filesNum)
{
    strncpy( Keywd, name, MAXKEYWD-1 );
    Keywd[ MAXKEYWD-1 ] = 0;
    fNum = 0;
    crt = time(nullptr);
}

//configuration from cfg file
TDataBase::TDataBase( fstream& f ):
        status(UNDF_), ind(f ), aFile(), fls()
{
    fromCFG(f);
    crt = time(nullptr);
    fNum = 0;
}

TDataBase::~TDataBase()
{
    for(size_t j=0; j<fls.size(); j++ )
        aFile[fls[j]]->Close();

    //  aFile.clear();
    fls.clear();
    fNum = 0;
    fOpenNameBuf.clear();
}

// add new file to database chain
void TDataBase::AddFile(const std::string& path)
{
    aFile.push_back( std::make_shared<TDBFile>(path));
}

// put information to index file
void TDataBase::putndx( uint nF )
{
    std::string Path;
    GemDataStream f;

    check_file(nF);
    Path = u_makepath( aFile[nF]->Dir(),
                aFile[nF]->Name(), NDX_EXT );
    f.open( Path.c_str(), WRITE_B);
    ErrorIf( !f.good(), Path.c_str(),"Index file open error");
    aFile[nF]->vdbh_setdt( );
    //  f.write( (char *)aFile[nF]->Dh(), sizeof(VDBhead) );
    aFile[nF]->Dh()->write (f);
    ind.PutKeyList( nF, f);
}

// get information from index file
void TDataBase::getndx( uint nF )
{
    std::string Path;
    VDBhead dh;
    GemDataStream f;

    check_file( nF );
    Path = u_makepath( aFile[nF]->Dir(),
                aFile[nF]->Name(), NDX_EXT );
    f.open( Path.c_str(), RDONLY_B);
    if( !f.good() )
      Error(  Path, "Index file open error");
    //   f.read( (char *)&dh, sizeof(VDBhead) );
    dh.read (f);
    ind.GetKeyList_i( nF, dh.nRec, f);
}

// get record length in the PDB file
int TDataBase::reclen( )
{
    int Olen=0;
    for(uint j=0; j<nOD; j++ )
        Olen += aObj[j+frstOD]->lenDB();
    return Olen;
}

// put record in DB file
int TDataBase::putrec( RecEntry& rep, GemDataStream& f )
{
    uint j;
    size_t len;
    int StillLen;
    RecHead rh;
    char *pack_key = const_cast<char*>(ind.PackKey());

    // header of the record
    strncpy( rh.bgm, MARKRECHEAD, 2 );
    strncpy( rh.endm, MARKRECHEAD, 2 );
    rh.nRT = nRT;               //warning: compatibility
    rh.Nobj = nOD;
    rh.rlen =  rep.len;
    StillLen = rep.len;
    rh.crt = time( nullptr );
    f.seekg(rep.pos, ios::beg );
    //   f.write( (char *)&rh, sizeof(RecHead) );
    rh.write (f);
    // put packed key
    len = strlen( pack_key );
    pack_key[len] = MARKRKEY;
    f.writeArray( pack_key, len+1 );
    pack_key[len] = '\0';
    StillLen -= len+1;
    ErrorIf( !f.good(), GetKeywd(),
             "PDB file write error");
    for( j=0; j<nOD; j++ )    // put objects to file
        StillLen -= aObj[j+frstOD]->toDB( f );
    crt = rh.crt;
    return StillLen;
}

// put record in DB file
int TDataBase::putrec( RecEntry& rep, GemDataStream& f, RecHead& rhh  )
{
    uint j;
    size_t len;
    int StillLen;
    RecHead rh;
    char *pack_key = const_cast<char*>(ind.PackKey());

    // header of the record
    strncpy( rh.bgm, MARKRECHEAD, 2*sizeof(char));
    strncpy( rh.endm, MARKRECHEAD, 2*sizeof(char) );
    rh.nRT = nRT;               //warning: compatibility
    rh.Nobj = nOD;
    rh.rlen =  rep.len;
    StillLen = rep.len;
    rh.crt = rhh.crt;
    f.seekp(rep.pos, ios::beg );
    //   f.write( (char *)&rh, sizeof(RecHead) );
    rh.write (f);
    // put packed key
    len = strlen( pack_key );
    pack_key[len] = MARKRKEY;
    f.writeArray( pack_key, (len+1) );
    pack_key[len] = '\0';
    StillLen -= len+1;
    ErrorIf( !f.good(), GetKeywd(),
             "PDB file write error");
    for( j=0; j<nOD; j++ )    // put objects to file
        StillLen -= aObj[j+frstOD]->toDB( f );
    crt = rh.crt;
    return StillLen;
}


// Gets a record from PDB file
int TDataBase::getrec( RecEntry& rep, GemDataStream& f, RecHead& rh )
{
    int j;
    int StillLen;
    // RecHead rh;
    char *key = const_cast<char*>(ind.PackKey());

    StillLen = rep.len;
    f.seekg(rep.pos, ios::beg );
    //   f.read( (char *)&rh, sizeof(RecHead) );
    rh.read (f);
    if( strncmp( rh.bgm, MARKRECHEAD, 2 ) ||
            strncmp( rh.endm, MARKRECHEAD, 2 ) ||
            (rh.Nobj != nOD && (nOD+frstOD-1) != o_tpstr  && 
             (rh.Nobj+frstOD-1) != o_phsdval  && (nOD+frstOD-1) != o_sptext ) )
        Error( GetKeywd(),"Record header format error");
    f.getline( key, KeyLen()+KeyNumFlds(), MARKRKEY);
    ErrorIf( f.gcount()>=(KeyLen()+KeyNumFlds()), GetKeywd(),
             "Error reading database record key" );
    StillLen -= f.gcount();  //???+1
    // test
    bool flag_spppar = false;
    for( j=0; j<rh.Nobj/*nOD*/; j++ )   // get objects from file
    {
//     now loop according actual record size
//                if ( j+frstOD == o_phstr2  )
//                    if( ( StillLen < static_cast<int>(16*sizeof(short)) && rep.len!=0 ) )
//                    {
//                        gui_logger->info("old record of phase: {}", key);
//                        break;
//                    }

       if ( j+frstOD == o_tpstr  )
          if( StillLen < 28 )
             continue;
       if ( j+frstOD == o_sptext )   //12.12.12
          if( StillLen < 10 )
             continue;
        StillLen -= aObj[j+frstOD]->ofDB(f);
        if (j+frstOD == o_spppar )
            flag_spppar = true;
    }
    if( StillLen!=0 && rep.len!=0 && !flag_spppar)
        Error( GetKeywd(),
          "Actual record length differs from that specified in the header");
    crt = rh.crt;
    return StillLen;
}


/*!
    Add new record with key pkey to file in DB files list
*/
uint TDataBase::AddRecordToFile( const char *pkey, int file )
{
    int len, oldlen;
    unsigned char nF;
    // test and open file
    fNum = file;
    nF = fls[file];
    check_file( nF );

    ErrorIf( !dbChangeAllowed( nF ), aFile[nF]->Name(), 
              "Cannot add record: changes to system database are not allowed!" );
    aFile[nF]->Open( UPDATE_DBV );

    auto i = ind.addndx( nF, 0, pkey );     // add record
    len = reclen();
    len += RecHead::data_size() + strlen(ind.PackKey()) + 1;
    RecEntry* rh = ind.RecPosit(i);
    rh->len = len;


    aFile[nF]->FindSfe( *rh );
    rh->len = len - RecHead::data_size();
    oldlen = aFile[nF]->FLen();
    int iRet =putrec( *rh, aFile[nF]->f );
    if( iRet!=0 )
    {
        if( aFile[nF]->FLen() != oldlen )
            aFile[nF]->FNewLen( iRet) ;
        rh->len -=iRet;
    }

    aFile[nF]->PutHead( aFile[nF]->f, 1);

    putndx(nF);

    status = ONEF_;
    if( rclose )
        aFile[nF]->Close();
    return i;
}

// Replace i-th record in Database
void TDataBase::Rep(int i)
{
    unsigned char nF;
    int oldlen, iRet;
    RecEntry* rh = ind.RecPosit(i);

    ind.check_i(i);
    // test and open file
    nF = rh->nFile;
    check_file( nF );
    ErrorIf( !dbChangeAllowed( nF, true ), aFile[nF]->Name(),
                "Cannot modify record: changes to system database are not allowed!" );

    aFile[nF]->Open( UPDATE_DBV );
    // delete record
    rh->len += RecHead::data_size();
    aFile[nF]->AddSfe( *rh );

    ind.PutKey(i);
    rh->len = reclen() + RecHead::data_size() + strlen(ind.PackKey()) + 1;
    aFile[nF]->FindSfe( *rh );
    rh->len -= RecHead::data_size();
    oldlen = aFile[nF]->FLen();
    iRet =putrec( *rh, aFile[nF]->f );
    if( iRet!=0 )
    {
        if( aFile[nF]->FLen() != oldlen )
            aFile[nF]->FNewLen( iRet );
        rh->len -=iRet;
    }
    aFile[nF]->PutHead( aFile[nF]->f, 0);
    putndx( nF );
    status = ONEF_;
    fNum = findIndex<int>( fls, nF );
    if( rclose )  aFile[nF]->Close();
}

//delete i-th record from database
void TDataBase::Del(int i)
{
    unsigned char nF;
    RecEntry* rh=ind.RecPosit(i);

    ind.check_i(i);
    // test and open file
    nF = rh->nFile;
    check_file( nF );
    ErrorIf( !dbChangeAllowed( nF ),
    aFile[nF]->Name(), "Cannot delete record: changes to system database are not allowed!");


    aFile[nF]->Open( UPDATE_DBV );
    // delete record
    rh->len += RecHead::data_size();
    aFile[nF]->AddSfe( *rh );
    ind.delndx(i);
    aFile[nF]->PutHead( aFile[nF]->f, -1);
    putndx( nF );
    status = UNDF_;
    if( rclose )  aFile[nF]->Close();
}

//Read i-th record from PDB file to memory.
void TDataBase::Get(int i)
{
    unsigned char nF;
    RecEntry* rh=ind.RecPosit(i);
    RecHead rhh;

    ind.check_i(i);
    // test and open file
    nF = rh->nFile;
    check_file( nF );

    aFile[nF]->Open( UPDATE_DBV );
    getrec( *rh, aFile[nF]->f, rhh );
    ind.PutKey(i);
    status = ONEF_;
    fNum = findIndex<int>( fls, nF );
    if( rclose )  aFile[nF]->Close();
}

//Rename record (Change first field: for SYSEQ, and >)
//Sveta 24/07/2001
void TDataBase::RenameList( const char* newName,
        const char *oldName )
{
    if( strlen(newName) > FldLen(0) )
      return;

    string str_old = char_array_to_string( oldName, FldLen(0) );
//04/09/01 ????    if( strlen(oldName)<FldLen(0) )
        str_old += ":";
    for( int i=1; i<KeyNumFlds(); i++)
            str_old += "*:";

    TCStringArray arKey;
    TCIntArray arR;

    auto Nrec = GetKeyList( str_old.c_str(), arKey, arR );
    if( Nrec < 1)
      return;

    int nrec;
    string str;

    for(size_t i=0; i<Nrec; i++ )
    {
        nrec =  Find( arKey[i].c_str() );
        Get( nrec );
        // changing record key
        str = ind.PackKey();
        Del( nrec );
        replace( str, oldName, newName);
        AddRecordToFile( str.c_str(), fNum );
    }
}

void TDataBase::toJsonObject( QJsonObject &obj ) const
{
    //write key fileds
    QJsonArray keyArray;
    for( int ii=0; ii < KeyNumFlds(); ii++)
    {
        QJsonObject keyObject;
        //      keyObject[ "kf" ] = aFldShot[ii].c_str();
        keyObject[ "fx" ] = ii;
        keyObject[ "fl" ] = FldLen(ii);
        auto fld_key = char_array_to_string( FldKey(ii), FldLen(ii) );
        strip(fld_key);
        keyObject[ "fv" ] = fld_key.c_str();
        keyArray.append(keyObject);
    }
    obj[ "key" ] = keyArray;

    // write objects
    QJsonArray dodArray;
    for( int no=frstODjson; no<=lastODjson; no++)
    {
        if( frstODjson!=frstOD && aObj[no]->GetDescription(0,0) == "internaldb" )
            continue;
        QJsonObject dodObject;
        dodObject[ "id" ] = no;
        aObj[no]->toJsonObject( dodObject );
        dodArray.append(dodObject);
    }
    obj[ "dod" ] = dodArray;
}

std::string TDataBase::fromJsonObject(const QJsonObject &obj)
{
    int ii, no;

    QJsonArray keyArray = obj[ "key" ].toArray();
    string keyStr = "", kbuf;
    for( ii=0; ii< std::min<int>( KeyNumFlds(), keyArray.size()); ii++ )
    {
        QJsonObject keyObject = keyArray[ii].toObject();
        kbuf = keyObject[ "fv" ].toString("*").toStdString();
        strip( kbuf );
        keyStr += kbuf.substr(0, FldLen( ii ));
        keyStr += ":";
    }

    // read objects
    QJsonArray dodArray = obj[ "dod" ].toArray();
    for( const auto& dod_val: dodArray)
    {
        QJsonObject dodObject = dod_val.toObject();
        no = dodObject["id"].toInt(-1);
        kbuf = dodObject[ "label" ].toString("").toStdString();
        if( kbuf.empty() )
          continue; // undefined label
        strip(kbuf);

        // test changing index of object in list
        if( no<0 || strcmp( aObj[no]->GetKeywd(), kbuf.c_str() ))
            no = aObj.Find( kbuf.c_str() );
        if( no < 0)
          continue;
        aObj[no]->fromJsonObject( dodObject );
   }

    return keyStr;
}


void TDataBase::toJsonObjectNew(QJsonObject &obj, const std::string& project_name) const
{
    QJsonArray keyArray;
    for( int ii=0; ii < KeyNumFlds(); ii++) {
        auto fld_key = char_array_to_string( FldKey(ii), FldLen(ii) );
        strip(fld_key);
        keyArray.append(fld_key.c_str());
    }
    obj["key"] = keyArray;
    obj["project"] = project_name.c_str();

    QJsonObject dodAll;
    for( int no=frstODjson; no<=lastODjson;  no++) {
        if( frstODjson!=frstOD && aObj[no]->GetDescription(0,0) == "internaldb" )
            continue;
        dodAll[aObj[no]->GetKeywd()] = aObj[no]->toJsonValue();
    }
    obj["dod"] = dodAll;
}

std::string TDataBase::fromJsonObjectNew(const QJsonObject &obj)
{
    QJsonArray keyArray = obj["key"].toArray();
    string keyStr = "", kbuf;
    for(int ii=0; ii< std::min<int>(KeyNumFlds(), keyArray.size()); ii++ ) {
        kbuf =  keyArray[ii].toString("*").toStdString();
        strip( kbuf );
        keyStr += kbuf.substr(0, FldLen( ii ));
        keyStr += ":";
    }
    auto dodAll = obj["dod"].toObject();
    for( int no=frstODjson; no<=lastODjson;  no++) {
        if( frstODjson!=frstOD && aObj[no]->GetDescription(0,0) == "internaldb" )
            continue;
        aObj[no]->fromJsonValue(dodAll[aObj[no]->GetKeywd()]);
    }
    return keyStr;
}


//Read record update time from PDB file to memory.
time_t TDataBase::GetTime( uint i )
{
    unsigned char nF;
    RecEntry* ree=ind.RecPosit(i);
    RecHead rh;

    ind.check_i(i);
    // test and open file
    nF = ree->nFile;
    check_file( nF );

    aFile[nF]->Open( UPDATE_DBV );
    aFile[nF]->f.seekg(ree->pos, ios::beg );
    //   aFile[nF]->f.read( (char *)&rh, sizeof(RecHead) );
    rh.read (aFile[nF]->f);
    if( strncmp( rh.bgm, MARKRECHEAD, 2 ) ||
            strncmp( rh.endm, MARKRECHEAD, 2 ) || rh.Nobj != nOD )
        Error( GetKeywd(), "PDB record header format error");
    status =  UNDF_;
    fNum = findIndex<int>( fls, nF );
    if( rclose )  aFile[nF]->Close();
    return rh.crt;
}

//Seach record index with key pkey.
// Returns -1 no record.
int TDataBase::Find( const char *pkey)
{
    if( !KeyTest( pkey ) )
         status = UNDF_;
    return ind.findx( pkey );
}

//Seach record index with key pkey.
// Returns -1 no record.
// No change status of record
int TDataBase::FindCurrent( const char *pkey)
{
    return ind.findx( pkey );
}

//Test state of record with key pkey.
//If mode == 1 and one record, read this record.
// returns state of record
RecStatus TDataBase::Rtest( const char *key, int mode )
{
    const char *pkey;
    bool OneRec;

    if( key != nullptr )
        pkey=key;
    else
        pkey = PackKey();

    if( strpbrk( pkey, "*?" ))
        OneRec = false; // pattern
    else
    {
        if( *pkey )
            OneRec = true;  // valid key: one record
        else
        {
            pkey = S_EMPTY;
            OneRec = false;
        }
    }
    status = UNDF_;
    if( ind.RecCount() <= 0 )
        return EMPC_;
AGAIN:
    if( OneRec )
    {
        auto iRec = Find( pkey );
        if( iRec < 0 )
            return NONE_;
        if( mode == 1)
            Get( iRec );
        return ONEF_;
    }
    else // template
    {
        auto iRec = ind.xlist( pkey );
        if( iRec==0 )
            return NONE_;
        if( iRec==1 )
        {
            OneRec = true;
            ind.PutKey( ind.RnList()[0] );
            pkey = ind.PackKey();
            goto AGAIN;
        }
        return MANY_;
    }
}

//Test state of record with key key_ as template.
// in field field setted any(*) data
bool TDataBase::FindPart( const char *key_, uint field )
{
    TDBKey dbKey(GetDBKey());
    dbKey.SetKey(key_);
    dbKey.SetFldKey(field,"*");
    string str_key( dbKey.UnpackKey(), 0, KeyLen() );
    RecStatus iRet = Rtest( str_key.c_str(), 0 );
    return ( iRet==MANY_ || iRet==ONEF_ );
}

// test work record key (equat)
bool TDataBase::KeyTest( const char* key )
{
    if( strchr( key, ':' ))  // key in packed form
        return  (!strcmp( key, PackKey()));
    else
        return  (!strcmp( key, ind.UnpackKey()));
}

//Test state of record with key pkey.  (in Task Manager)
//If mode == 1 and one record read this record.
// return state of record
/*RecStatus TDataBase::TryRec( RecStatus rs,
		char *key, int mode )
{
  if( !KeyTest( key ) )
        status = UNDF_;
  if( rs != UNDF_ && rs == status )
	return status;
  status = Rtest( key, mode );
  return status;
} */

//Create new PDB file.
void TDataBase::Create( uint nF )
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
    for(size_t i=0; i<fls.size(); i++)
        fOpenNameBuf.push_back( aFile[fls[i]]->Name().c_str() );
}

// Selection in the list of full file names
void TDataBase::GetFileList(int mode, TCStringArray& names,
                            TCIntArray& indeces, TCIntArray& sel)
{
    names.clear();
    indeces.clear();
    sel.clear();
    for( size_t i=0; i<aFile.size(); i++ )
    {

        int nF = fNum = findIndex<int>( fls, i );
        if( (nF==-1&&(mode&closef))||(nF!=-1&&(mode&openf)) )
        {
            aFile[i]->Makepath();
            names.push_back( string(aFile[i]->GetKeywd())+string(" ")+aFile[i]->GetPath());
            indeces.push_back(i);
            if( (mode&oldself) && nF != -1) //select already open files
                sel.push_back(indeces.size()-1);
        }
    }
}

// open files in PDB
void
TDataBase::Open( bool type, FileStatus mode, const TCIntArray& nff )
{
    uint j;
    ///
    if( aFile.size() == 0 )
        return;

    if( fls.size()<=0 ) //no config
    {
        if( type )
        {
            if( nff.size()<1 )
                fls.push_back(0);
            for( j=0; j< nff.size(); j++)
                fls.push_back( nff[j] );
        }
        else  fls.push_back(0);
    }
    //  opfils();
    ind.initnew();
    for( j=0; j<fls.size(); j++ )
        Create( fls[j] );
try
  {
    TCIntArray comp;
    for( j=0; j<fls.size(); j++ )
    {
        aFile[fls[j]]->Open( mode );
        //added Sveta 04/11/2002 to index files
        if( aFile[fls[j]]->GetDhOver())
// Sveta 06/2005
//         if( vfQuestion( 0, aFile[fls[j]].GetPath(),
//         "Stack of deleted records overflow.\nCompress?" ))
             comp.push_back(fls[j]);
        // end added
        getndx( fls[j] );
        if( rclose )  aFile[fls[j]]->Close();
    }
    opfils();
    //added Sveta 04/11/2002 to index files
        if( comp.size() >0 )
         RebildFile( comp);
    // end added

  }
  catch( TError& xcpt )
  {
        if( xcpt.mess == "Two records with the same key (OpenFiles).")
        {
          xcpt.mess += "\n 2nd record in file: ";
          xcpt.mess +=  aFile[fls[j]]->Name().c_str();

          while( j < fls.size() )
            fls.erase( fls.begin()+j);
         ind.initnew();

          for( j=0; j<fls.size(); j++ )
          {
             aFile[fls[j]]->Open( mode );
             getndx( fls[j] );
             if( rclose )  aFile[fls[j]]->Close();
           }
           opfils();
         }
         Error( xcpt.title, xcpt.mess);
   }
    opfils();
}

// open all files in PDB chain
void
TDataBase::OpenAllFiles( bool only_kernel )
{
    int j;

    if( aFile.size() == 0 )
        return;

    Close();
    for( j=0; j< static_cast<int>(aFile.size()); j++)
     if( only_kernel && j >=  specialFilesNum )
       continue;
     else
        fls.push_back( j );

    Open( true, UPDATE_DBV, {} );
}


//close files in PDB
void TDataBase::Close()
{
    for(size_t j=0; j<fls.size(); j++ )
        aFile[fls[j]]->Close();
    fls.clear();
    fNum = 0;
    fOpenNameBuf.clear();
    ind.initnew();
}


//close files in PDB
void TDataBase::OpenOnlyFromList( TCStringArray& names )
{
    if( aFile.size() == 0 )
        return;

    Close();
    uint ii, jj;

    for( ii=0; ii< aFile.size(); ii++)
    {
      for( jj=0; jj< names.size(); jj++)
      {
          auto all_name =  std::string("."+names[jj]+".");
          if(  aFile[ii]->GetPath().find( all_name.c_str() ) != string::npos )
            break;
      }
      if( jj < names.size() )
         fls.push_back( ii );
    }

    Open( true, UPDATE_DBV, {} );
}

// add new file to DBfile list
int TDataBase::AddFileToList(TDBFile* file)
{
    aFile.push_back( std::shared_ptr<TDBFile>(file) );
try{
    Create(aFile.size()-1);
    file->Open( UPDATE_DBV );
    getndx(aFile.size()-1);
    fls.push_back(aFile.size()-1);
    if( rclose )
        file->Close();
    opfils();
 }
 catch( TError& xcpt )
  {
        if( xcpt.mess == "Two records with the same key (AddFileToList).")
        {
          xcpt.mess += "\n 2nd record in file: ";
          xcpt.mess +=  aFile[aFile.size()-1]->Name().c_str();

          ind.initnew();

          for(size_t j=0; j<fls.size(); j++ )
          {
             aFile[fls[j]]->Open( UPDATE_DBV );
             getndx( fls[j] );
             if( rclose )
                aFile[fls[j]]->Close();
           }
           opfils();
         }
         Error( xcpt.title, xcpt.mess);
   }
   return  findIndex<int>( fls, aFile.size()-1 );
}


/*// get number of file, that name conterned substring (only open files)
int TDataBase::GetFileNum(const char* substr_name)
{
  for(int ii=fls.GetCount()-1; ii>=0; ii-- )
  {
   if( aFile[fls[ii]]->Name().find( substr_name ) != string::npos )
     return ii;
  }
  return -1;
}
*/

// delete a file from DBfile list
void TDataBase::DelFile(const std::string& path)
{
    TDBFile fl(path);

    for(size_t ii=0; ii<aFile.size(); ii++)
        if( fl.Name() == aFile[ii]->Name() )
        {
            auto  nF = ii;
            int  nFls = findIndex<int>( fls, nF );
            if( nFls != -1 ) //close file and delete indexes
            {
                aFile[nF]->Close();
                ind.delfile( nF );
                fls.erase( fls.begin()+nFls);
            }
            fNum = 0; //?????
            aFile.erase( aFile.begin()+ nF);
            opfils();
            break;
        }
}


// add open PDB files
void TDataBase::AddOpenFile(const TCIntArray& nff)
{
  size_t j=0;
  if( nff.size()<1 )
        return;
try{
    TCIntArray comp;

    for( j=0; j<nff.size(); j++)
    {  //fls.Add( nff[j] );
        Create( nff[j] );
        aFile[nff[j]]->Open( UPDATE_DBV );
        //added Sveta 04/11/2002 to index files
        if( aFile[nff[j]]->GetDhOver())
         if( vfQuestion( nullptr, aFile[nff[j]]->GetPath().c_str(),
         "Stack of deleted records overflow.\nCompress?" ))
             comp.push_back(nff[j]);
        // end added
        getndx( nff[j] );
        fls.push_back( nff[j] );
        if( rclose )
            aFile[nff[j]]->Close();
    }
    opfils();
    //added Sveta 04/11/2002 to index files
        if( comp.size() >0 )
         RebildFile( comp);
    // end added

  }
  catch( TError& xcpt )
  {
        if( xcpt.mess == "Two records with the same key (AddOpenFile).")
        {
          xcpt.mess += "\n 2nd record in file: ";
          xcpt.mess +=  aFile[nff[j]]->Name().c_str();

          ind.initnew();

          for( j=0; j<fls.size(); j++ )
          {
             aFile[fls[j]]->Open( UPDATE_DBV );
             getndx( fls[j] );
             if( rclose )
                aFile[fls[j]]->Close();
           }
           opfils();
         }
         Error( xcpt.title, xcpt.mess);
   }

}


//Get new lists of keys and indexes
size_t TDataBase::GetKeyList( const char *keypat,TCStringArray& aKey, TCIntArray& anR)
{
    auto l = ind.xlist( keypat );
//    status = UNDF_;         // 09/11/2004 Sveta    Is it necessary ? KD 13.01.05
    aKey = ind.KeyList();
    anR =ind.RnList();
    return l;
}


// Scan database file
int TDataBase::scanfile( uint nF, int& fPos, int& fLen,
		GemDataStream& inStream, GemDataStream& outStream)
{
    RecEntry recordEntry;
    RecHead recordHead;
    string str;
    //RecEntry& rep = ind.RecPosit(0);
    int len, fEnd = fPos;
    int nRec=0;

    recordEntry.nFile = nF;             //warning
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
        catch( TError& xcpt )
        {
            gui_logger->warn("scan error : {}", xcpt.mess);
            /*    if( !vfQuestion( Keywd,
                        " Continue with possible loss of data in \n"
                        "the record or cancel PDB file compression?"))
                     break;
                */
            fPos += 2;
            continue;
        }

        fPos += (-len) + RecHead::data_size();
        len = reclen() + strlen(ind.PackKey()) + 1;
        // test for existing records
        int  Rnum = Find(ind.PackKey() );
        if( Rnum>=0 ) // name of exist record
        {
           str=PackKey();
           str = aMod[nRT]->GetKeyofRecord( str.c_str(),
                      "This key record already exists! Replace please?", KEY_NEW );
           if(  str.empty() )
                continue;
           ind.SetKey( str.c_str());

         }

        auto ni = ind.addndx( nF, len, ind.PackKey() );
        //RecEntry& rep = ind.RecPosit(ni);
        ind.RecPosit(ni)->pos = fEnd;
        fEnd += len + RecHead::data_size();

        putrec( *ind.RecPosit(ni), outStream, recordHead );

        nRec++;
        gui_logger->debug("restore record: {}", ind.PackKey());
        pVisor->Message( nullptr, nullptr, "Compressing database file. "
                "Please, wait...", fPos, fLen);
    }
    fLen = fEnd;

    return nRec;
}

//Get new index file and physically deleted records
void TDataBase::RebildFile(const TCIntArray& nff)
{
    // close&open db files added Sveta 06/03/02
    TCIntArray fls_old;
    for(size_t j=0; j< fls.size(); j++)
        fls_old.push_back( fls[j] );
    Close();

    for(size_t j=0; j<nff.size(); j++)
    {
        int  nRec, nRT1;
        char isDel;
        int fPos, fLen;
        uint nF = nff[j];
        // test and open file
        check_file( nF );
        if( !dbChangeAllowed( nF ) )
        {
            vfMessage( nullptr, aFile[nF]->Name(), "Cannot rebuild PDB file!");
            continue;
        }
        aFile[nF]->Open( UPDATE_DBV );

        //ind.delfile( nF );
        aFile[nF]->GetDh( fPos, fLen, nRT1, isDel );

        std::string tmpFileName = aFile[nF]->GetPath().c_str();
        tmpFileName += ".tmp";
        GemDataStream outStream( tmpFileName, ios::out | ios::binary );
        for(int ii=0; ii<fPos; ii++ )
            outStream.put(0);

        outStream.seekp(fPos/*VDBhead::data_size()*/, ios::beg);

        nRec = scanfile( nF, fPos, fLen, aFile[nF]->f, outStream );

        outStream.close();

        aFile[nF]->OpenFromFileName( tmpFileName.c_str(), UPDATE_DBV );

        aFile[nF]->SetDh( fLen, nRec, nRT1, isDel );

        putndx( nF );
        status = UNDF_;

        aFile[nF]->Close();
        ind.delfile( nF );
    }

    pVisor->CloseMessage();
    Open( true, UPDATE_DBV, fls_old );
}

//Set new open Files list  (Reopen by list)
bool TDataBase::SetNewOpenFileList(const TCStringArray& aFlKeywd)
{
    bool allOpend = true;
    
    if(aFlKeywd.size() < 1 )
        return  allOpend;

    Close();
    for(uint i=0; i<aFlKeywd.size(); i++)
    {
        int nF = -1;
        for(size_t j=0; j<aFile.size(); j++)
        {
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
            if( !vfQuestion( nullptr, aFlKeywd[i].c_str(),
             "This database file was not found in the project or default database.\n"
            		" Continue without this file (Y) or cancel(N)?" ))
               Error( aFlKeywd[i], "Please, provide this database file and try opening the project again!");
            allOpend = false;
        }    
    }

    Open(false, UPDATE_DBV, fls);
    return allOpend;
}

//Make new project-file and close another project files (make new Project)
void TDataBase::MakeInNewProfile(const std::string& dir,
   const char *prfName, const char * f_name )
{
    /* open only default files (no user files)*/
    //  Close();
    TCIntArray fl;
    for(size_t  i=0; i<fls.size(); i++ )
    {
        if( fls[i] < specialFilesNum)
        {
            fl.push_back(fls[i]);
        }
    }
    Close();
    for(size_t  ii=0; ii<fl.size(); ii++ )
        fls.push_back(fl[ii]);
    if( fls.size() >= 1)
        Open(false, UPDATE_DBV, fl);

    /* add new project files*/
    std::string name(dir);
    name += "/";

    if( f_name == nullptr || *f_name == '\0' )
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
uint TDataBase::GetOpenFileNum( const char* secondName )
{
    std::string name = GetKeywd();
    name += ".";
    name +=secondName;
    for(size_t i=0; i<fls.size(); i++)
        if( name == aFile[fls[i]]->Name() )
            return i;
    return 0;
}

// get list of files keywds, that contained name
void TDataBase::GetProfileFileKeywds( const char *_name, TCStringArray& aFlkey )
{
    std::string name = ".";
    name +=_name;
    name += ".";

    for(size_t ii=0; ii< aFile.size(); ii++)
    {
      if(  aFile[ii]->GetPath().find( name ) != std::string::npos )
          aFlkey.push_back( aFile[ii]->GetKeywd() );
    }
}

// true if opened some files from default data base
bool TDataBase::ifDefaultOpen() const
{
    for(size_t i=0; i<fls.size(); i++)
        if( aFile[fls[i]]->GetPath().find( pVisor->sysDBDir().c_str())
              != std::string::npos )
            return true;
    return false;
}

//-------------------------------------------------------------
// DataBaseList
//-------------------------------------------------------------

int DataBaseList::Find(const char* s)
{
    for( size_t ii=0; ii<size(); ii++ )
        if( strcmp(s, at(ii)->GetKeywd() ) )
            return static_cast<int>(ii);
    return -1;
}

// default configuration
void DataBaseList::Init()
{
    // RT_SDATA default
    unsigned char sdref_rkfrm[3] = { 20, 5, 7 };
    push_back( std::make_shared<TDataBase>( RT_SDATA, "sdref", false, true,
                        o_sdauthr, 9, 3, 3, sdref_rkfrm ) );
    // RT_CONST default
    unsigned char const_rkfrm[2] = { 8, 24 };
    push_back( std::make_shared<TDataBase>( RT_CONST, "const", false, true,
                        o_constlab, 3, 1, 2, const_rkfrm ) );
}

// configuration to file
void DataBaseList::toCFG(fstream& out_stream)
{
    int nR = size();
out_stream << nR << endl;

//    f.write( (char*)&nR, sizeof(int) );
    for(size_t ii=0; ii<size(); ii++)
        at(ii)->toCFG( out_stream );
}

// configuration from file
void DataBaseList::fromCFG(fstream& in_stream)
{
    int nDB;

//    f.read( (char*)&nR, sizeof(int) );
in_stream >> nDB;
    for( int ii=0; ii<nDB; ii++)
        push_back( std::make_shared<TDataBase>(in_stream ) );
}



DataBaseList::~DataBaseList()
{
 //   for( uint ii=0; ii<GetCount(); ii++ )
 //       delete elem(ii);
}

DataBaseList rt;

//--------------------- End of v_dbm2.cpp ---------------------------

