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

#include "v_dbm.h"
#include "v_mod.h"
#include "visor.h"
#include "service.h"
#include "m_param.h"
#include "v_service.h"

static const char MARKEXTOBJ[] = "PAX1";   // precedes the extended-settings objects of a project record
#include "GEMS3K/gdatastream.h"


//-------------------------------------------------------------
// TDataBase
//-------------------------------------------------------------

const char* ALLKEY  = "*";
const char* MARKRECDEL = "##";
const char* MARKRECHEAD = "$$";
const char MARKRKEY  = '#';

std::string TDataBase::make_key_template(const std::vector<std::string> &first_fields) const
{
    size_t ii=0;
    std::string key_template;
    for(ii=0; ii<std::min<size_t>(first_fields.size(), KeyNumFlds()); ++ii) {
        key_template += first_fields[ii].substr(0, FldLen(ii))+":";
    }
    for(; ii<KeyNumFlds(); ++ii) {
        key_template += "*:";
    }
    return key_template;
}

//must be fix types as int16_t
void TDataBase::toCFG(std::fstream& out_stream)
{
    unsigned short numFields = ind.KeyNumFlds();
    out_stream << numFields << "  ";
    unsigned char* aLen = ind.RkFrm();
    for(auto ii=0; ii<numFields; ++ii) {
        out_stream << ( static_cast<unsigned short>(aLen[ii])) << ' ';
    }
    out_stream << "  ";
    out_stream << Keywd << ' ';
    out_stream << uint(nRT) << "  ";
    out_stream << rclose << ' ';
    out_stream << isDelete << "  ";
    out_stream << frstOD << ' ';
    out_stream << uint(nOD) << "  ";
    out_stream << frstODjson << "  ";
    out_stream << lastODjson << "  ";
    dbfiles_list.to_cfg(out_stream);
    out_stream << std::endl;
}

//get state from cfg file
void TDataBase::fromCFG(std::fstream& in_stream)
{
    unsigned int tmp = 0;

    in_stream >> Keywd;
    in_stream >> tmp;
    nRT = tmp;
    in_stream >> rclose;
    in_stream >> isDelete;
    in_stream >> frstOD;
    in_stream >> tmp;
    nOD = tmp;
    in_stream >> frstODjson;
    in_stream >> lastODjson;
    dbfiles_list.from_cfg(in_stream);
    // to do `find place`
    dbfiles_list.rclose = rclose;
    dbfiles_list.isDelete = isDelete;
    dbfiles_list.nRT = nRT;
}

//default configuration of the database
TDataBase::TDataBase( size_t nrt, const char* name,
                     bool Rclose, bool isDel, int nOf, unsigned char Nobj,
                     int filesNum, unsigned char nRkflds, const unsigned char* rkfrm ):
    nRT(nrt), status(UNDF_), rclose(Rclose), isDelete(isDel),
    frstOD(nOf), nOD(Nobj), frstODjson(frstOD), lastODjson(frstOD+nOD-1),
    ind(nRkflds, rkfrm),
    dbfiles_list(nRT, rclose, isDelete)
{
    strncpy(Keywd, name, MAXKEYWD-1);
    Keywd[MAXKEYWD-1] = 0;
    crt = time(nullptr);
}

//configuration from cfg file
TDataBase::TDataBase(std::fstream& f):
        status(UNDF_), ind(f),
        dbfiles_list(0, true, true)
{
    fromCFG(f);
    crt = time(nullptr);
}

TDataBase::~TDataBase()
{
    dbfiles_list.close_all();
}

// add new file to database chain
void TDataBase::AddFile(const std::string& path)
{
    dbfiles_list.add_file(path);
}

// get record length in the PDB file
int TDataBase::reclen()
{
    int objs_len=0;
    for(uint j=0; j<nOD; ++j) {
        objs_len += aObj[j+frstOD]->lenDB();
    }
    if(headerObjCount() != nOD) {
        objs_len += 4;          // marker before the extended-settings objects
    }
    return objs_len;
}

// put record in DB file
int TDataBase::putrec( RecEntry& rep, GemDataStream& f, int crt)
{
    size_t len;
    int StillLen;
    RecHead rh;
    char *pack_key = const_cast<char*>(ind.PackKey());

    // header of the record
    strncpy( rh.bgm, MARKRECHEAD, 2 );
    strncpy( rh.endm, MARKRECHEAD, 2 );
    rh.nRT = nRT;               //warning: compatibility
    rh.Nobj = headerObjCount();
    rh.rlen =  rep.len;
    StillLen = rep.len;
    rh.crt = crt;
    f.seekg(rep.pos, std::ios::beg);
    rh.write(f);
    // put packed key
    len = strlen(pack_key);
    f.writeArray(pack_key, len);
    char marker = MARKRKEY;
    f.writeArray(&marker, 1);
    StillLen -= len + 1;
    ErrorIf(!f.good(), GetKeywd(), "PDB file write error");
    for(uint j=0; j<nOD; ++j)  {   // put objects to file
        if(j==headerObjCount() && j!=nOD)  {   // start of the objects an older GEMSGUI does not know (see getExtendedSettings())
            f.writeArray( const_cast<char*>(MARKEXTOBJ), 4 );
            StillLen -= 4;
        }
        StillLen -= aObj[j+frstOD]->toDB(f);
    }
    crt = rh.crt;
    return StillLen;
}

unsigned char TDataBase::headerObjCount() const
{
    return frstOD == o_spppar ? nOD-NUM_PAXOBJ : nOD;
}

// Reads the objects stored after the ones counted in the header of a project record: the extended
// numerical settings. A record saved by an older GEMSGUI has none, and then the defaults are set.
// An older GEMSGUI stops after its own objects and ignores this tail, so such a record stays
// readable by it. Returns the number of bytes read.
int TDataBase::getExtendedSettings( GemDataStream& f, int nRead, int StillLen )
{
    int len = 0;
    char mark[4] = { 0, 0, 0, 0 };
    if( nRead == headerObjCount() )
    {
        auto pos = f.tellg();
        f.readArray( mark, 4 );
        if( !f.good() )
        {
            f.clear();
            f.seekg( pos, std::ios::beg );
            memset( mark, 0, 4 );
        }
    }
    if( TProfil::pm )
        TProfil::pm->resetExtendedSettings();   // objects the record does not hold keep the defaults
    if( memcmp( mark, MARKEXTOBJ, 4 ) == 0 )
    {
        len = 4;
        // a record written when the tail had fewer objects ends early: stop there
        for( uint j = nRead; j < nOD && StillLen-len >= aObj[j+frstOD]->lenDB(); j++ )
            len += aObj[j+frstOD]->ofDB(f);
    }
    return len;
}

// Gets a record from PDB file
int TDataBase::getrec( RecEntry& rep, GemDataStream& f, RecHead& rh )
{
    int StillLen;
    char *key = const_cast<char*>(ind.PackKey());

    StillLen = rep.len;
    f.seekg(rep.pos, std::ios::beg );
    rh.read(f);
    if( strncmp(rh.bgm, MARKRECHEAD, 2) ||
        strncmp(rh.endm, MARKRECHEAD, 2) ||
        (rh.Nobj != nOD && (nOD+frstOD-1) != o_tpstr  &&
         (rh.Nobj+frstOD-1) != o_phsdval  && !(frstOD == o_spppar && rh.Nobj <= nOD) ) ) {
        gui_logger->info("Record header format error {} {} {} {}", char_array_to_string(Keywd, MAXKEYWD), rh.Nobj, nOD, frstOD);
        Error(GetKeywd(), "Record header format error");
    }
    f.getline(key, KeyLen()+KeyNumFlds(), MARKRKEY);
    ErrorIf(f.gcount()>=(KeyLen()+KeyNumFlds()), GetKeywd(), "Error reading database record key" );
    StillLen -= f.gcount();

    bool flag_spppar = false;
    for(int j=0; j<rh.Nobj/*nOD*/; j++ )  {  // get objects from file
        //     now loop according actual record size
        //                if ( j+frstOD == o_phstr2  )
        //                    if( ( StillLen < static_cast<int>(16*sizeof(short)) && rep.len!=0 ) )
        //                    {
        //                        gui_logger->info("old record of phase: {}", key);
        //                        break;
        //                    }

        if(j+frstOD == o_tpstr) {
            if(StillLen < 28) {
                continue;
            }
        }
        if (j+frstOD == o_sptext) {   //12.12.12
            if(StillLen < 10) {
                continue;
            }
        }
        StillLen -= aObj[j+frstOD]->ofDB(f);
        if(j+frstOD == o_spppar) {
            flag_spppar = true;
        }
    }
    if(frstOD == o_spppar) {
        StillLen -= getExtendedSettings(f, rh.Nobj, StillLen);
    }
    if(StillLen!=0 && rep.len!=0 && !flag_spppar) {
        Error(GetKeywd(), "Actual record length differs from that specified in the header");
    }
    crt = rh.crt;
    return StillLen;
}


/*!
    Add new record with key pkey to file in DB files list
*/
uint TDataBase::AddRecordToFile(const char *pkey, int file_ndx)
{
    dbfiles_list.check_file(file_ndx);
    if(!dbfiles_list.check_writable(file_ndx, "add record")) {
        return -1;
    }

    auto record_ndx = ind.addndx(file_ndx, 0, pkey);
    RecEntry* rh = ind.RecPosit(record_ndx);
    dbfiles_list[file_ndx]->save_record_to_file(true, rclose, rh, this);
    status = ONEF_;
    return record_ndx;
}

// Replace i-th record in Database
void TDataBase::Rep(int record_ndx)
{
    ind.check_i(record_ndx);
    RecEntry* rh = ind.RecPosit(record_ndx);
    ind.PutKey(record_ndx);

    auto file_ndx = rh->nFile;
    dbfiles_list.check_file(file_ndx);
    if(!dbfiles_list.check_writable(file_ndx, "modify record")) {
        return;
    }

    dbfiles_list[file_ndx]->save_record_to_file(false, rclose, rh, this);
    status = ONEF_;
}

//delete i-th record from database
void TDataBase::Del(int record_ndx)
{
    ind.check_i(record_ndx);
    RecEntry* rh=ind.RecPosit(record_ndx);

    auto file_ndx = rh->nFile;
    dbfiles_list.check_file(file_ndx);
    if(!dbfiles_list.check_writable(file_ndx, "delete record")) {
        return;
    }
    RecEntry copy_rh = *rh;
    ind.delndx(record_ndx);
    dbfiles_list[file_ndx]->delete_record_from_file(rclose, &copy_rh, this);
    status = UNDF_;
}

//Read i-th record from PDB file to memory.
void TDataBase::Get(int record_ndx)
{
    ind.check_i(record_ndx);
    RecEntry* rh=ind.RecPosit(record_ndx);

    auto file_ndx = rh->nFile;
    dbfiles_list.check_file(file_ndx);
    dbfiles_list[file_ndx]->read_record_from_file(rclose, rh, this);
    ind.PutKey(record_ndx);
    status = ONEF_;
}

//Read record update time from PDB file to memory.
time_t TDataBase::GetTime(size_t record_ndx)
{
    ind.check_i(record_ndx);
    RecEntry* ree=ind.RecPosit(record_ndx);

    auto file_ndx = ree->nFile;
    dbfiles_list.check_file(file_ndx);
    auto time = dbfiles_list[file_ndx]->get_time(rclose, ree);
    status =  UNDF_;
    return time;
}

//Seach record index with key pkey.
// Returns -1 no record.
int TDataBase::Find(const char *pkey)
{
    if(!KeyTest(pkey)) {
        status = UNDF_;
    }
    return ind.findx(pkey);
}

//Seach record index with key pkey.
// Returns -1 no record.
// No change status of record
int TDataBase::FindCurrent(const char *pkey)
{
    return ind.findx(pkey);
}

//Rename record (Change first field: for SYSEQ, and >)
void TDataBase::RenameList(const char* newName, const char *oldName)
{
    if(strlen(newName) > FldLen(0)) {
        return;
    }
    std::string str_old_template = make_key_template({oldName});
    auto Nrec = ind.xlist(str_old_template.c_str());
    auto keys_list = ind.KeyList();
    int rec_ndx;
    std::string record_key;

    for(size_t i=0; i<Nrec; ++i) {
        rec_ndx = Find(keys_list[i].c_str());
        Get(rec_ndx);
        record_key = ind.PackKey();
        Del(rec_ndx);
        replace(record_key, oldName, newName);
        AddRecordToFile(record_key.c_str(), dbfiles_list.last_used_file);
    }
}

void TDataBase::toJsonObject( QJsonObject &obj ) const
{
    //write key fileds
    QJsonArray keyArray;
    for(int ii=0; ii < KeyNumFlds(); ++ii) {
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
    for(int no=frstODjson; no<=lastODjson; ++no)  {
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
    std::string keyStr = "", kbuf;
    for(ii=0; ii< std::min<int>( KeyNumFlds(), keyArray.size()); ++ii) {
        QJsonObject keyObject = keyArray[ii].toObject();
        kbuf = keyObject[ "fv" ].toString("*").toStdString();
        strip( kbuf );
        keyStr += kbuf.substr(0, FldLen( ii ));
        keyStr += ":";
    }

    // read objects
    QJsonArray dodArray = obj[ "dod" ].toArray();
    for(const auto& dod_val: dodArray) {
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
    for(int ii=0; ii < KeyNumFlds(); ++ii) {
        auto fld_key = char_array_to_string( FldKey(ii), FldLen(ii) );
        strip(fld_key);
        keyArray.append(fld_key.c_str());
    }
    obj["key"] = keyArray;
    obj["project"] = project_name.c_str();

    QJsonObject dodAll;
    for(int no=frstODjson; no<=lastODjson;  ++no) {
        if( frstODjson!=frstOD && aObj[no]->GetDescription(0,0) == "internaldb" )
            continue;
        dodAll[aObj[no]->GetKeywd()] = aObj[no]->toJsonValue();
    }
    obj["dod"] = dodAll;
}

std::string TDataBase::fromJsonObjectNew(const QJsonObject &obj)
{
    QJsonArray keyArray = obj["key"].toArray();
    std::string keyStr = "", kbuf;
    for(int ii=0; ii< std::min<int>(KeyNumFlds(), keyArray.size()); ++ii) {
        kbuf =  keyArray[ii].toString("*").toStdString();
        strip( kbuf );
        keyStr += kbuf.substr(0, FldLen( ii ));
        keyStr += ":";
    }
    auto dodAll = obj["dod"].toObject();
    if( frstOD == o_spppar && TProfil::pm )
        TProfil::pm->resetExtendedSettings();
    for(int no=frstODjson; no<=lastODjson;  ++no) {
        if( frstODjson!=frstOD && aObj[no]->GetDescription(0,0) == "internaldb" )
            continue;
        QString obj_key(aObj[no]->GetKeywd());
        aObj[no]->fromJsonValue(dodAll[obj_key]);
        if( obj_key == "dSDval") {
            auto nSd = aObj[o_dcdim]->Get(0, 2);
            aObj[o_dcsdval]->Alloc(nSd, 1, V_SD_VALEN );
        }
        if( obj_key == "rSDval") {
            auto nSd = aObj[o_redim]->Get(0, 7);
            aObj[o_resdval]->Alloc(nSd, 1, V_SD_VALEN);
        }
        if( obj_key == "dSDref") {
            auto nSd = (aObj[no]->GetM()==0 ? 0 : aObj[no]->GetN()); // in case of empty
            aObj[o_dcdim]->Put(nSd, 0, 2);
        }
        if( obj_key == "rSDref") {
            auto nSd = (aObj[no]->GetM()==0 ? 0 : aObj[no]->GetN());
            aObj[o_redim]->Put(nSd, 0, 7);
        }
    }
    return keyStr;
}



//Test state of record with key pkey.
//If mode == 1 and one record, read this record.
// returns state of record
RecStatus TDataBase::Rtest(const char *key, int mode)
{
    const char *pkey;
    bool OneRec;

    if(key != nullptr) {
        pkey=key;
    }
    else {
        pkey = PackKey();
    }

    if(strpbrk( pkey, "*?" )) {
        OneRec = false; // pattern
    }
    else {
        if( *pkey ) {
            OneRec = true;  // valid key: one record
        }
        else {
            pkey = S_EMPTY;
            OneRec = false;
        }
    }
    status = UNDF_;
    if(ind.RecCount() <= 0) {
        return EMPC_;
    }
AGAIN:
    if( OneRec ) {
        auto iRec = Find( pkey );
        if( iRec < 0 ) {
            return NONE_;
        }
        if(mode == 1) {
            Get( iRec );
        }
        return ONEF_;
    }
    else { // template
        auto iRec = ind.xlist(pkey);
        if( iRec==0 ) {
            return NONE_;
        }
        if( iRec==1 )   {
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
    std::string str_key(dbKey.UnpackKey(), 0, KeyLen() );
    RecStatus iRet = Rtest( str_key.c_str(), 0 );
    return ( iRet==MANY_ || iRet==ONEF_ );
}

// test work record key (equat)
bool TDataBase::KeyTest(const char* key)
{
    if( strchr( key, ':' ))  // key in packed form
        return  (!strcmp( key, PackKey()));
    else
        return  (!strcmp( key, ind.UnpackKey()));
}


// //Create new PDB file.
// void TDataBase::Create( uint nF )
// {
//     check_file( nF );
//     aFile[nF]->create_with_ndx(nRT, isDelete, nF, ind);
// }

// // Makes a list of open file names
// void TDataBase::opfils()
// {
//     fNum = 0;
//     fOpenNameBuf.clear();
//     for(size_t i=0; i<fls.size(); i++)
//         fOpenNameBuf.push_back( aFile[fls[i]]->Name().c_str() );
// }

// Selection in the list of full file names
void TDataBase::GetFileList(int mode, std::vector<std::string>& names,
                            std::vector<int>& indeces,
                            std::vector<int>& sel)
{
    return dbfiles_list.files_list(mode, names, indeces, sel);
}

// open files in PDB
void  TDataBase::Open(bool type, FileStatus mode, const TCIntArray& nff)
{
    auto files_to_rebuild = dbfiles_list.open_files(mode, nff, ind);
    if(!files_to_rebuild.empty()) {
        RebildFile(files_to_rebuild);
    }
}

// open all files in PDB chain
void TDataBase::OpenAllFiles(bool only_kernel)
{
    if(dbfiles_list.size() == 0) {
        return;
    }
    Close();
    auto files_list = dbfiles_list.all_files(only_kernel);
    Open(true, UPDATE_DBV, files_list);
}

//close files in PDB
void TDataBase::Close()
{
    dbfiles_list.close_all();
    ind.initnew();
}


//close files in PDB
void TDataBase::OpenOnlyFromList(TCStringArray& names)
{
    if(dbfiles_list.size() == 0) {
        return;
    }
    Close();
    auto files_list = dbfiles_list.files_with_name(names);
    Open(true, UPDATE_DBV, files_list);
}

// add new file to DBfile list
void TDataBase::AddFileToList(const std::string &path)
{
    dbfiles_list.open_new_file(path, ind);
}

// delete a file from DBfile list
void TDataBase::DelFile(const std::string& path)
{
    dbfiles_list.remove_file(path, ind);
}


// add open PDB files
void TDataBase::AddOpenFile(const TCIntArray& nff)
{
    auto files_to_rebuild = dbfiles_list.add_open_files(nff, ind);
    if(!files_to_rebuild.empty()) {
        RebildFile(files_to_rebuild);
    }
}

//Get new lists of keys and indexes
size_t TDataBase::GetKeyList( const char *keypat,TCStringArray& aKey, TCIntArray& anR)
{
    auto l = ind.xlist( keypat );
    aKey = ind.KeyList();
    anR =ind.RnList();
    return l;
}


// Scan restore database file
int TDataBase::scanfile(uint nF, int& fPos, int& fLen,
                        GemDataStream& inStream, GemDataStream& outStream)
{
    RecEntry recordEntry;
    RecHead recordHead;
    std::string str;
    //RecEntry& rep = ind.RecPosit(0);
    int len, fEnd = fPos;
    int nRec=0;

    recordEntry.nFile = nF;             //warning
    recordEntry.len = 0;
    while(fPos < fLen) {
        char ch;
        inStream.seekg( fPos, std::ios::beg );
        inStream.get(ch);
        if(ch != MARKRECHEAD[0]) {
            fPos++;
            continue;
        }
        inStream.get(ch);
        if(ch != MARKRECHEAD[0]) {
            fPos+=2;
            continue;
        }
        recordEntry.pos = fPos;
        try  {
            len = getrec(recordEntry, inStream, recordHead);
        }
        catch(TError& xcpt) {
            gui_logger->warn("scan error : {}", xcpt.mess);
            fPos += 2;
            continue;
        }

        fPos += (-len) + RecHead::data_size();
        len = reclen() + strlen(ind.PackKey()) + 1;
        // test for existing records
        int  Rnum = Find(ind.PackKey());
        if( Rnum>=0 ) { // name of exist record
            str=PackKey();
            str = aMod[nRT]->GetKeyofRecord(str.c_str(),
                                            "This key record already exists! Replace please?", KEY_NEW);
            if(str.empty()) {
                continue;
            }
            ind.SetKey( str.c_str());
        }

        auto ni = ind.addndx( nF, len, ind.PackKey() );
        ind.RecPosit(ni)->pos = fEnd;
        fEnd += len + RecHead::data_size();
        putrec(*ind.RecPosit(ni), outStream, recordHead.crt);

        nRec++;
        gui_logger->debug("restore record: {}", ind.PackKey());
        pVisor->Message(nullptr, "Compressing database file.\n Please, wait...", fPos, fLen);
    }
    fLen = fEnd;

    return nRec;
}

//Get new index file and physically deleted records
void TDataBase::RebildFile(const TCIntArray& nff)
{
    std::vector<int> old_sel(dbfiles_list.opened_files.begin(), dbfiles_list.opened_files.end());
    Close();
    for(auto file_ndx: nff) {
        try {
            dbfiles_list.check_file(file_ndx);
            if(!dbfiles_list.check_writable(file_ndx, "rebuild PDB file")) {
                continue;  // here exception
            }
        }
        catch(TError& xcpt) {
            continue;
        }
        dbfiles_list[file_ndx]->rebuild_file(file_ndx, this);
        status = UNDF_;
        ind.delfile(file_ndx);
    }
    pVisor->CloseMessage();
    Open(true, UPDATE_DBV, old_sel);
}

//Set new open Files list  (Reopen by list)
bool TDataBase::SetNewOpenFileList(const TCStringArray& aFlKeywd)
{
    bool allOpend = true;
    if(dbfiles_list.size() == 0) {
        return allOpend;
    }
    Close();
    auto files_list = dbfiles_list.files_with_keywds(aFlKeywd, allOpend);
    Open(true, UPDATE_DBV, files_list);
    return allOpend;
}

//Make new project-file and close another project files (make new Project)
std::string TDataBase::gen_new_project_file_name(const std::string& dir,
                                                 const std::string& project_name,
                                                 const std::string& f_name) const
{
    std::string name(dir);
    name += "/";

    if(f_name.empty()) {
        name += GetKeywd();
        name += ".";
        name += project_name;
        name += ".";
        name += PDB_EXT;
    }
    else {
        name += f_name;
    }
    return name;
}


//Make new project-file and close another project files (make new Project)
void TDataBase::MakeInNewProfile(const std::string& dir, const std::string& prfName,
                                 const std::string& f_name)
{
    auto files_list = dbfiles_list.opened_special_files();
    Close();
    Open(false, UPDATE_DBV, files_list);

    // add new project files
    std::string name = gen_new_project_file_name(dir, prfName, f_name);
    AddFileToList(name);
}

// get open file number by name (0 if not found )
uint TDataBase::GetOpenFileNum(const char* secondName)
{
    std::string name = std::string(GetKeywd())+"."+secondName;
    return dbfiles_list.file_name_index(name);
}

// get list of files keywds, that contained name
void TDataBase::GetProfileFileKeywds(const char *name, TCStringArray& aFlkey)
{
    aFlkey = dbfiles_list.file_keywds_according_name(name);
}

// true if opened some files from default data base
bool TDataBase::ifDefaultOpen() const
{
    return dbfiles_list.check_special_open();
}

void TDataBase::check_record_key(const char *pkey)
{
    ErrorIf(!pkey || pkey[0] == '\0',
            char_array_to_string(Keywd, MAXKEYWD),
            "The record key cannot be empty.");

    if (std::strpbrk(pkey, "*?/") != nullptr) {
        Error(char_array_to_string(Keywd, MAXKEYWD),
              "Attempted to insert a record with a wildcard key.");
    }
}

void TDataBase::addRecord(QWidget* window, const char *pkey)
{
    bool ok_to_all = false;
    check_record_key(pkey);
    ErrorIf(dbfiles_list.opened_files.empty(),
            char_array_to_string(Keywd, MAXKEYWD),
            "No database file chosen to save the record.");

    int file_ndx = dbfiles_list.file_to_add_record(window, pkey, false, ok_to_all);
    if(file_ndx>=0) {
        AddRecordToFile(pkey, file_ndx);
    }
}

// Adds the record, or all records to file Sveta 15/06/01
void TDataBase::addRecord(QWidget* window, const char* pkey, int& fnum)
{
    bool ok_to_all = false;
    check_record_key(pkey);
    ErrorIf(dbfiles_list.opened_files.empty(),
            char_array_to_string(Keywd, MAXKEYWD),
            "No database file chosen to save the record.");
    int file_ndx;
    if(fnum >= 0) {
        file_ndx = fnum;
    }
    else {
        file_ndx = dbfiles_list.file_to_add_record(window, pkey, true, ok_to_all);
        if(ok_to_all && file_ndx >= 0) {
            fnum = file_ndx;
        }
    }
    if(file_ndx>=0) {
        AddRecordToFile(pkey, file_ndx);
    }
    else {
        fnum = -2;   // User pressed 'Cancel' or closed the dialog window
    }
}


//-------------------------------------------------------------
// DataBaseList
//-------------------------------------------------------------

int DataBaseList::Find(const char* s)
{
    for(size_t ii=0; ii<size(); ii++) {
        if(strcmp(s, at(ii)->GetKeywd())) {
            return static_cast<int>(ii);
        }
    }
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
void DataBaseList::toCFG(std::fstream& out_stream)
{
    int nR = size();
    out_stream << nR << std::endl;
    for(size_t ii=0; ii<size(); ++ii) {
        at(ii)->toCFG(out_stream);
    }
}

// configuration from file
void DataBaseList::fromCFG(std::fstream& in_stream)
{
    int nDB;
    in_stream >> nDB;
    for(int ii=0; ii<nDB; ++ii) {
        push_back(std::make_shared<TDataBase>(in_stream));
    }
}

DataBaseList::~DataBaseList()
{}

DataBaseList rt;

//--------------------- End of v_dbm2.cpp ---------------------------

