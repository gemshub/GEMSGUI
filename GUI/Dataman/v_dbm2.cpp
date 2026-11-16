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
//#include "v_mod.h"
#include "visor.h"
#include "service.h"
#include "m_param.h"
#include "v_service.h"

//#include "GEMS3K/gdatastream.h"


//-------------------------------------------------------------
// TDataBase
//-------------------------------------------------------------


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


//default configuration of the database
TDataBase::TDataBase(size_t rt_type, std::string_view name, bool Rclose, bool isDel,
                     int32_t nOf, unsigned char Nobj, int32_t filesNum,
                     unsigned char nRkflds, const unsigned char* rkfrm):
    record_type(rt_type),
    frstOD(nOf), nOD(Nobj),
    frstODjson(frstOD), lastODjson(frstOD+nOD-1),
    keys_map(nRkflds, rkfrm),
    dbfiles_list(Rclose, isDel),
    status(UNDF_)
{
    std::strncpy(Keywd, name.data(), MAXKEYWD-1);  //????
    Keywd[MAXKEYWD-1] = 0;
    crt_time = time(nullptr);
}

//configuration from cfg file
TDataBase::TDataBase(std::fstream& f):
    keys_map(f),
    dbfiles_list(true, true),
    status(UNDF_)
{
    fromCFG(f);
    crt_time = time(nullptr);
}

TDataBase::~TDataBase()
{
    dbfiles_list.close_all();
}

void TDataBase::toCFG(std::fstream& out_stream) const
{
    unsigned short numFields = KeyNumFlds();
    out_stream << numFields << "  ";
    for(auto ii=0; ii<numFields; ++ii) {
        out_stream << FldLen(ii) << ' ';
    }
    out_stream << "  ";
    out_stream << Keywd << ' ';
    out_stream << uint(record_type) << "  ";
    out_stream << dbfiles_list.auto_close << ' ';
    out_stream << dbfiles_list.reuse_deleted << "  ";
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
    record_type = tmp;
    in_stream >> dbfiles_list.auto_close;
    in_stream >> dbfiles_list.reuse_deleted;
    in_stream >> frstOD;
    in_stream >> tmp;
    nOD = tmp;
    in_stream >> frstODjson;
    in_stream >> lastODjson;
    dbfiles_list.from_cfg(in_stream);
}


bool TDataBase::check_rt_name(const std::string &file_name) const
{
    auto head_name = getKeywd()+".";
    if(file_name.find(head_name) != std::string::npos ||
        (record_type == RT_UNSPACE && file_name.find("probe.") != std::string::npos) ||   //check old names
        (record_type == RT_DUALTH && file_name.find("duterm.") != std::string::npos)) {
        return true;
    }
    return false;
}

// =========================================================================
// --- Database File Manipulation Methods ---
// =========================================================================


void  TDataBase::openDatabase(FileStatus mode, const std::vector<int>& files_ndx)
{
    auto files_to_rebuild = dbfiles_list.open_files(record_type, mode, files_ndx, keys_map);
    if(!files_to_rebuild.empty()) {
        rebuildsFiles(files_to_rebuild);
    }
}

void TDataBase::closeDatabase()
{
    dbfiles_list.close_all();
    keys_map.initnew();
}

void TDataBase::addFiles(const std::string &folder, const std::vector<std::string> &path_list)
{
    std::vector<std::string> rt_file_list;
    for(const auto& path : path_list) {
        if(check_rt_name(path)) {
            rt_file_list.push_back(path);
        }
    }
    dbfiles_list.add_files(folder, rt_file_list);
}

//Make new project-file and close another project files (make new Project)
std::string TDataBase::gen_new_project_file_name(const std::string& dir,
                                                 const std::string& project_name,
                                                 const std::string& f_name) const
{
    std::string name(dir);
    name += "/";

    if(f_name.empty()) {
        name += getKeywd()+".";
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
void TDataBase::addProjectFile(const std::string& dir, const std::string& prfName,
                               const std::string& f_name)
{
    auto files_list = dbfiles_list.opened_special_files();
    closeDatabase();
    openDatabase(UPDATE_DBV, files_list);

    // add new project files
    std::string name = gen_new_project_file_name(dir, prfName, f_name);
    addNewFile(name);
}

void TDataBase::removeFiles(const std::vector<std::string> &path_list)
{
    std::set<std::string> target_names;
    for (const auto& path : path_list) {
        if(check_rt_name(path)) {
            target_names.insert(TDBFile(path).relativePath());
        }
    }
    dbfiles_list.remove_files(target_names, keys_map);
}

void TDataBase::openAllFiles(bool only_kernel)
{
    if(dbfiles_list.empty_files_list()) {
        return;
    }
    closeDatabase();
    auto files_list = dbfiles_list.all_files(only_kernel);
    openDatabase(UPDATE_DBV, files_list);
}

void TDataBase::openOnlyWith(const std::vector<std::string>& names)
{
    if(dbfiles_list.empty_files_list()) {
        return;
    }
    closeDatabase();
    auto files_list = dbfiles_list.files_with_name(names);
    openDatabase(UPDATE_DBV, files_list);
}

// add open PDB files
void TDataBase::openAdditionalFiles(const std::vector<int>& indx)
{
    auto files_to_rebuild = dbfiles_list.add_open_files(record_type, indx, keys_map);
    if(!files_to_rebuild.empty()) {
        rebuildsFiles(files_to_rebuild);
    }
}

//Set new open Files list  (Reopen by list)
bool TDataBase::openOnlyWithKeywords(const std::vector<std::string>& keywords)
{
    bool allOpend = true;
    if(dbfiles_list.empty_files_list()) {
        return allOpend;
    }
    closeDatabase();
    auto files_list = dbfiles_list.files_with_keywds(keywords, allOpend);
    openDatabase(UPDATE_DBV, files_list);
    return allOpend;
}


size_t TDataBase::indexOfName(const std::string &second_name)
{
    std::string name = std::string(getKeywd()) + "." + second_name;
    size_t file_ndx = dbfiles_list.file_name_index(name);
    if (file_ndx == std::string::npos) {
        Error(name, "Database access error: Attempted to reference a non-existent file.");
        return std::string::npos;
    }
    return file_ndx;
}

void TDataBase::rebuildsFiles(const std::vector<int>& indx)
{
    std::vector<int> old_sel(dbfiles_list.opened_files.begin(), dbfiles_list.opened_files.end());
    closeDatabase();
    for(auto file_ndx: indx) {
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
        keys_map.delfile(file_ndx);
    }
    pVisor->CloseMessage();
    openDatabase(UPDATE_DBV, old_sel);
}


// =========================================================================
// --- Database Records Manipulation Methods ---
// =========================================================================

/**
 * @brief Validates the record key for emptiness and restricted wildcard characters.
 *
 * Ensures the key is not empty and does not contain illegal search mask
 * characters (*, ?, /) that could corrupt database index lookups.
 *
 * @param key The record key string to be validated.
 */
void TDataBase::check_record_key(const std::string& key)
{
    ErrorIf(key.empty(), getKeywd(), "The record key cannot be empty.");
    if(isPattern(key)) {
        Error(getKeywd(),
              "Attempted to insert a record with a wildcard key. Special characters (*, ?, /) are not allowed.");
    }
}

std::string TDataBase::addRecord(const std::string& key, size_t file_ndx)
{
    status = RecStatus{};
    check_record_key(key);

    SetKey(key);
    const auto unpack_key = UnpackKey();
    const RecEntry* rh = keys_map.findkey(unpack_key);

    if (rh != nullptr) {
        SetKey(ALLKEY);
        const size_t safe_len = std::min(key.size(), KeyLen());
        std::string msg = "Failed to add record: '" + key.substr(0, safe_len) + "' already exists.";
        Error(getKeywd(), msg);
        return "";
    }

    dbfiles_list.check_file(file_ndx);
    if (!dbfiles_list.check_writable(file_ndx, "add record")) {
        return "";
    }

    RecEntry rec{};
    rec.nFile = static_cast<unsigned char>(file_ndx);
    keys_map.putkey(unpack_key, rec);

    try {
        dbfiles_list[file_ndx]->save_record_to_file(true, keys_map.findkey(unpack_key), this);
    }
    catch (...) {
        keys_map.delkey(unpack_key);
        SetKey(ALLKEY);
        throw;
    }

    status = ONEF_;
    return unpack_key;
}

void TDataBase::readRecord(const std::string& key)
{
    status = RecStatus{};
    check_record_key(key);

    SetKey(key);
    const RecEntry* rh = keys_map.findkey(UnpackKey());

    if(rh == nullptr) {
        SetKey(ALLKEY);
        const size_t safe_len = std::min(key.size(), KeyLen());
        std::string msg = "Database access error: Record '" + key.substr(0, safe_len) + "' was not found!";
        Error(getKeywd(), msg);
        return;
    }

    const auto file_ndx = rh->nFile;
    dbfiles_list.check_file(file_ndx);
    dbfiles_list[file_ndx]->read_record_from_file(rh, this);
    status = ONEF_;
}

std::string TDataBase::updateRecord(const std::string& key)
{
    status = RecStatus{};
    check_record_key(key);

    SetKey(key);
    RecEntry* rh = keys_map.findkey(UnpackKey());

    if(rh == nullptr) {
        SetKey(ALLKEY);
        const size_t safe_len = std::min(key.size(), KeyLen());
        std::string msg = "Failed to update record: '" + key.substr(0, safe_len) + "' does not exist.";
        Error(getKeywd(), msg);
        return "";
    }

    const auto file_ndx = rh->nFile;
    dbfiles_list.check_file(file_ndx);
    if(!dbfiles_list.check_writable(file_ndx, "modify record")) {
        return "";
    }

    dbfiles_list[file_ndx]->save_record_to_file(false, rh, this);
    status = ONEF_;

    return UnpackKey();
}

void TDataBase::deleteRecord(const std::string& key, bool err_if_no)
{
    status = RecStatus{};
    check_record_key(key);

    SetKey(key);
    const auto unpack_key = UnpackKey();
    const RecEntry* rh = keys_map.findkey(unpack_key);

    if(rh == nullptr) {
        SetKey(ALLKEY);
        if (err_if_no) {
            const size_t safe_len = std::min(key.size(), KeyLen());
            std::string msg = "Database access error: Record '" + key.substr(0, safe_len) + "' not found for deletion!";
            Error(getKeywd(), msg);
        }
        return;
    }

    const auto file_ndx = rh->nFile;
    dbfiles_list.check_file(file_ndx);
    if (!dbfiles_list.check_writable(file_ndx, "delete record")) {
        return;
    }

    RecEntry copy_rh = *rh;
    keys_map.delkey(unpack_key);
    dbfiles_list[file_ndx]->delete_record_from_file(&copy_rh, this);

    SetKey(ALLKEY);
    status = UNDF_;
}

std::time_t TDataBase::getTime(const std::string& key)
{
    TDBKeyNew temp_key = keys_map.current_key;
    temp_key.setKey(key);

    const std::string unpack_key = temp_key.unpackKey();
    const RecEntry* rh = keys_map.findkey(unpack_key);

    if(rh == nullptr) {
        return static_cast<std::time_t>(0);
    }

    const auto file_ndx = rh->nFile;
    dbfiles_list.check_file(file_ndx);

    return dbfiles_list[file_ndx]->get_time(rh);
}

void TDataBase::toJsonObject( QJsonObject &obj ) const
{
    //write key fileds
    QJsonArray keyArray;
    for(size_t ii=0; ii < KeyNumFlds(); ++ii) {
        QJsonObject keyObject;
        //      keyObject[ "kf" ] = aFldShot[ii].c_str();
        keyObject[ "fx" ] = static_cast<int>(ii);
        keyObject[ "fl" ] = static_cast<int>(FldLen(ii));
        keyObject[ "fv" ] = QString::fromStdString(FldKey(ii));
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
    for(ii=0; ii< std::min<int>(KeyNumFlds(), keyArray.size()); ++ii) {
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
    for(size_t ii=0; ii < KeyNumFlds(); ++ii) {
        auto fld_key = FldKey(ii);
        strip(fld_key);
        keyArray.append(QString::fromStdString(fld_key));
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

void TDataBase::recordToTxt(const std::string& key, std::ostream& f)
{
    std::string key_buffer = key;
    aObj[o_reckey]->SetPtr(static_cast<void*>(key_buffer.data()));
    aObj[o_reckey]->toTXT(f);
    for(int no = frstOD; no < frstOD + nOD; ++no) {
        aObj[no]->toTXT(f);
    }
}

std::string TDataBase::recordFromTxt(std::istream& f)
{
    size_t required_key_len = KeyLen();
    size_t buffer_size = std::max(static_cast<size_t>(256), required_key_len + 1);
    std::vector<char> safe_buf(buffer_size, '\0');

    aObj[o_reckey]->SetPtr(static_cast<void*>(safe_buf.data()));
    aObj[o_reckey]->ofTXT(f);
    for(int no = frstOD; no < frstOD + nOD; ++no) {
        aObj[no]->ofTXT(f);
    }
    safe_buf[required_key_len] = '\0';
    return char_array_to_string(safe_buf.data(), required_key_len);
}

void TDataBase::renameList(const std::string& newName, const std::string& oldName)
{
    if(newName.length() > FldLen(0)) {
        return;
    }

    const std::string str_old_template = make_key_template({oldName});
    const auto keys_list = keys_map.xlist(str_old_template);

    for(const auto& rkey : keys_list) {
        const RecEntry* rh = keys_map.findkey(rkey);
        if(rh == nullptr) {
            continue;
        }
        const size_t target_file_ndx = rh->nFile;

        readRecord(rkey);
        deleteRecord(rkey);

        keys_map.current_key.setKey(rkey);
        keys_map.current_key.setFieldKey(0, newName);
        const std::string new_record_key = keys_map.current_key.packKey();

        addRecord(new_record_key, target_file_ndx);
    }
}

RecStatus TDataBase::Rtest(const std::string& pkey, int mode)
{
    status = UNDF_;

    if(RecCount() <= 0) {
        status = EMPC_;
        return status;
    }

    TDBKeyNew temp_key = keys_map.current_key;
    temp_key.setKey(pkey);
    const std::string unpack_key = temp_key.unpackKey();
    const bool is_pattern = isPattern(unpack_key);

    if(!is_pattern) {
        const RecEntry* rh = keys_map.findkey(unpack_key);
        if(rh == nullptr) {
            status = NONE_;
            return status;
        }
        if(mode == 1) {
            readRecord(unpack_key);
        }
        status = ONEF_;
        return status;
    }
    else {
        const auto keys_list = keys_map.xlist(unpack_key);
        if(keys_list.empty()) {
            status = NONE_;
            return status;
        }
        if(keys_list.size() == 1) {
            if(mode == 1) {
                readRecord(keys_list[0]);
            }
            status = ONEF_;
            return status;
        }
        status = MANY_;
        return status;
    }
}

std::string TDataBase::patternToField(const std::string& pkey, size_t field) const
{
    TDBKeyNew temp_key = keys_map.current_key;
    temp_key.setKey(pkey);
    temp_key.setFieldKey(field, "*");
    return temp_key.unpackKey();
}


bool TDataBase::findPart(const std::string& pkey, size_t field)
{
    const auto pattern_key = patternToField(pkey, field);
    const RecStatus iret = Rtest(pattern_key, 0);
    return (iret == MANY_ || iret == ONEF_);
}

std::string TDataBase::fields2key(const std::vector<std::string>& fields, bool packed) const
{
    TDBKeyNew temp_key = keys_map.current_key;
    temp_key.setKey(ALLKEY);

    const size_t count = std::min(fields.size(), temp_key.fieldsInKey());
    for(size_t i = 0; i < count; ++i) {
        temp_key.setFieldKey(i, fields[i]);
    }
    return packed ? temp_key.packKey() : temp_key.unpackKey();
}

//---------------------------------------------------------------------------------

void TDataBase::addRecord(QWidget* window, const std::string& key)
{
    bool ok_to_all = false;
    check_record_key(key);
    ErrorIf(dbfiles_list.empty_files_list(), char_array_to_string(Keywd, MAXKEYWD),
            "No database file chosen to save the record.");

    int file_ndx = dbfiles_list.file_to_add_record(window, key, false, ok_to_all);
    if(file_ndx>=0) {
        addRecord(key, file_ndx);
    }
}

// Adds the record, or all records to file Sveta 15/06/01
void TDataBase::addRecord(QWidget* window, const std::string& key, int& fnum)
{
    bool ok_to_all = false;
    check_record_key(key);
    ErrorIf(dbfiles_list.empty_files_list(), char_array_to_string(Keywd, MAXKEYWD),
            "No database file chosen to save the record.");
    int file_ndx;
    if(fnum >= 0) {
        file_ndx = fnum;
    }
    else {
        file_ndx = dbfiles_list.file_to_add_record(window, key, true, ok_to_all);
        if(ok_to_all && file_ndx >= 0) {
            fnum = file_ndx;
        }
    }
    if(file_ndx>=0) {
        addRecord(key, file_ndx);
    }
    else {
        fnum = -2;   // User pressed 'Cancel' or closed the dialog window
    }
}

bool TDataBase::saveRecordQuestion(QWidget *window, const std::string &key, int &quest_reply, int &fnum)
{

    if(existsRecord(key))   {
        ReplaceRecordwithQuestion(window, key, quest_reply);
        if( quest_reply == VF_CANCEL )
            return false;
    }
    else
    {
        addRecord(window, key, fnum);
        if( fnum == -2 )
            return false;
    }
    return true;
}

void TDataBase::ReplaceRecordwithQuestion(QWidget *window, const std::string &key, int &quest_reply)
{
    if(quest_reply == VF_YES_ALL)
    {
        updateRecord(key);
    }
    else if(quest_reply != VF_NO_ALL)
    {
        std::string replace_question = "Data record with this key already exists! Replace?";
        quest_reply = vfQuestionYesNoAll(window, key, replace_question);
        switch(quest_reply)
        {
        case VF_YES_ALL:
        case VF_YES:
            updateRecord(key);
            break;
        }
    }
}



//-------------------------------------------------------------
// DataBaseList
//-------------------------------------------------------------

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


