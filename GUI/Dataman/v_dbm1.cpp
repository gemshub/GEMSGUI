//-------------------------------------------------------------------
// $Id: v_dbm1.cpp 1032 2008-02-13 10:15:07Z gems $
//
// Implementation of TDBFile, TDBKey and TDBKeyList classes
//
// Copyright (C) 1996-2001 S.Dmytriyeva, D.Kulik
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


#include <cstdlib>
#include <cstdarg>
#include <ctime>
#include <string_view>
#include "v_dbm.h"
#include "v_mod.h"
#include "m_param.h"
#include "GEMS3K/gdatastream.h"

static const char MARKEXTOBJ[] = "PAX1";   // precedes the extended-settings objects of a project record
const char* ALLKEY  = "*";
const char* MARKRECDEL = "##";
const char* MARKRECHEAD = "$$";
const char MARKRKEY  = '#';
const int	MAXRKFRMSTR = 20;  // max fields in key


TDBKeyNew::TDBKeyNew(unsigned char nflds, const unsigned char* rkfrm)
    : fixed_keys_len(0)
{
    ErrorIf(rkfrm == nullptr, "TDBKey", "Error: Configuration geometry source pointer is null.");
    ErrorIf(nflds == 0 || nflds > MAXRKFRMSTR,
            "TDBKey", "Error: Invalid number of fields in record key specification (1).");

    flds_len.reserve(nflds);
    flds_value.reserve(nflds);

    for(size_t ii = 0; ii < nflds; ++ii) {
        flds_len.push_back(rkfrm[ii]);
        flds_value.push_back("*");
        fixed_keys_len += rkfrm[ii];
    }
}

TDBKeyNew::TDBKeyNew(std::fstream& in_stream)
    : fixed_keys_len(0)
{
    unsigned int nflds = 0;
    in_stream >> nflds;
    ErrorIf(nflds == 0 || nflds > MAXRKFRMSTR,
            "TDBKey", "Error: Invalid number of fields in record key specification (2).");

    flds_len.reserve(nflds);
    flds_value.reserve(nflds);

    for(size_t ii = 0; ii < nflds; ++ii) {
        unsigned int tmp = 0;
        in_stream >> tmp;
        ErrorIf(!in_stream.good(), "TDBKey", "Error: Stream corrupted while parsing key sub-field limits.");
        flds_len.push_back(tmp);
        flds_value.push_back("*");
        fixed_keys_len += tmp;
    }
}

void TDBKeyNew::setKey(const std::string& new_key)
{
    if(new_key.empty() || new_key == ALLKEY) {
        set_all();
    }
    else if(new_key.find(':') != std::string::npos) {
        set_packed(new_key);
    }
    else {
        set_unpacked(new_key);
    }
}

void TDBKeyNew::setFieldKey(size_t ndx, const std::string& new_fld)
{
    if(ndx < flds_value.size()) {
        std::string new_val = new_fld.substr(0, flds_len[ndx]);
        strip(new_val);
        flds_value[ndx] = std::move(new_val);
    }
}

void TDBKeyNew::set_packed(const std::string& key)
{
    const std::string& pack_key = key;

    size_t pbegin = 0;
    size_t total_fields = fieldsInKey();
    for(size_t ii = 0; ii < total_fields; ++ii) {
        if (pbegin >= pack_key.size()) {
            setFieldKey(ii, "");
            continue;
        }
        size_t pend = pack_key.find(':', pbegin);
        if(pend == std::string::npos) {
            size_t token_len = std::min(pack_key.size() - pbegin, flds_len[ii]);
            setFieldKey(ii, pack_key.substr(pbegin, token_len));
            pbegin += token_len;
        }
        else {
            size_t pure_len = pend - pbegin;
            size_t token_len = std::min(pure_len, flds_len[ii]);
            setFieldKey(ii, pack_key.substr(pbegin, token_len));
            pbegin = pend + 1;
        }
    }
}

void TDBKeyNew::set_unpacked(const std::string& key)
{
    std::string unpack_key = key;
    if(unpack_key.size() < fixed_keys_len) {
        unpack_key.resize(fixed_keys_len, ' ');
    }

    size_t pbegin = 0;
    size_t total_fields = fieldsInKey();
    for(size_t ii = 0; ii < total_fields; ++ii) {
        setFieldKey(ii, unpack_key.substr(pbegin, flds_len[ii]));
        pbegin += flds_len[ii];
    }
}

std::string TDBKeyNew::makeKey(uint8_t rt_current, std::initializer_list<KeyArg> args) const
{
    std::string compiled_key;

    size_t total_fields = fieldsInKey();
    compiled_key.reserve(fixed_keys_len+total_fields);
    auto it = args.begin();

    for(size_t i = 0; i < total_fields; ++i) {
        if (it == args.end() || it->command == K_END) {
            compiled_key += S_ANY;
            compiled_key += ":";
            continue;
        }

        size_t rkflen = flds_len[i];

        switch(it->command) {
        case K_EMP: compiled_key += S_EMPTY; break;
        case K_ANY: compiled_key += S_ANY;   break;
        case K_IMM:
            compiled_key += it->str_value.substr(0, rkflen);
            break;
        case K_ACT:
        default:  {
            int32_t rts = (it->command == K_ACT) ? rt_current : it->command;
            uint32_t nkf = it->field_index;

            std::string fld_raw_key = rt[rts]->FldKey(nkf);
            if (fld_raw_key.empty()) {
                compiled_key += S_ANY;
            }
            else {
                size_t read_limit = std::min(static_cast<size_t>(rt[rts]->FldLen(nkf)), rkflen);
                std::string str = fld_raw_key.substr(0, read_limit);
                strip(str);
                compiled_key += str;
            }
        }
        break;
        }
        compiled_key += ":";
        ++it;
    }
    return compiled_key;
}

std::string TDBKeyNew::packKey() const
{
    std::string pack_key;
    size_t total_fields = fieldsInKey();
    pack_key.reserve(fixed_keys_len+total_fields);
    for(size_t ii=0; ii<total_fields; ++ii) {
        pack_key.append(fieldKey(ii)+":");
    }
    return pack_key;
}

std::string TDBKeyNew::unpackKey() const
{
    std::string unpack_key;
    unpack_key.reserve(fixed_keys_len);
    for(size_t ii=0; ii<fieldsInKey(); ++ii) {
        unpack_key.append(unpackFieldKey(ii));
    }
    return unpack_key;
}

//-------------------------------------------------------------
// TDBKeyMap - working with DB keys list
//-------------------------------------------------------------

void  TDBKeyMap::initnew()
{
    key_record_map.clear();
    current_key.setKey(ALLKEY);
}

void TDBKeyMap::delfile(size_t nF)
{
    std::erase_if(key_record_map, [nF](const auto& item) {
        return static_cast<size_t>(item.second.nFile) == nF;
    });
}

bool TDBKeyMap::delkey(const std::string& unpack_key)
{
    return key_record_map.erase(unpack_key)>0;
}

void TDBKeyMap::putkey(const std::string& unpack_key, const RecEntry& entry)
{
    key_record_map.insert_or_assign(unpack_key, entry);
}

bool TDBKeyMap::haskey(const std::string& unpack_key) const
{
    return key_record_map.contains(unpack_key);
}

const RecEntry* TDBKeyMap::findkey(const std::string& unpack_key) const
{
    auto it = key_record_map.find(unpack_key);
    if (it != key_record_map.end()) {
        return &(it->second);
    }
    return nullptr;
}

RecEntry* TDBKeyMap::findkey(const std::string& unpack_key)
{
    auto it = key_record_map.find(unpack_key);
    if (it != key_record_map.end()) {
        return &(it->second);
    }
    return nullptr;
}

void TDBKeyMap::save_keys_ndxs(size_t nF, GemDataStream& f, const std::string& file_name) const
{
    const size_t key_len = current_key.length();
    for(const auto& [key, entry] : key_record_map) {
        if(static_cast<size_t>(entry.nFile) == nF) {
            std::string fixed_buf = key;
            fixed_buf.resize(key_len, ' ');
            f.writeArray(fixed_buf.data(), key_len);
            entry.write(f);
        }
    }
    ErrorIf(!f.good(), file_name, "Index file write error: Failed to flush key entries to disk.");
}

void TDBKeyMap::get_keys_ndxs(size_t nF, size_t nRec, GemDataStream& f, const std::string& file_name)
{
    const size_t key_len = current_key.length();
    std::string key_buf(key_len, '\0');
    RecEntry re;

    for(size_t i = 0; i < nRec; ++i) {
        f.readArray(key_buf.data(), key_len);
        ErrorIf(!f.good(), "TDBKeyMap", file_name + ": Index file read error while parsing key.");
        re.read(f);
        ErrorIf(!f.good(), "TDBKeyMap", file_name + ": Index file read error while parsing record entry.");
        re.nFile = static_cast<unsigned char>(nF);
        putkey(key_buf, re);
    }
    current_key.setKey(ALLKEY);
}


/**
 * @brief Matches a target string against a wildcard pattern supporting '*' and '?'.
 *
 * Uses an iterative two-pointer algorithm with O(1) space complexity, avoiding
 * the overhead and special-character escaping issues of std::regex.
 *
 * @param str     The target string to evaluate.
 * @param pattern The wildcard pattern ('*' matches any sequence, '?' matches any single character).
 * @return true   If the string matches the pattern, false otherwise.
 */
constexpr bool match_wildcard(std::string_view str, std::string_view pattern) noexcept
{
    size_t s_idx = 0;
    size_t p_idx = 0;
    size_t star_idx = std::string_view::npos;
    size_t match_idx = 0;

    while(s_idx < str.size()) {
        if(p_idx < pattern.size() && (pattern[p_idx] == '?' || pattern[p_idx] == str[s_idx])) {
            ++s_idx;
            ++p_idx;
        }
        else if(p_idx < pattern.size() && pattern[p_idx] == '*') {
            star_idx = p_idx;
            match_idx = s_idx;
            ++p_idx;
        }
        else if(star_idx != std::string_view::npos) {
            p_idx = star_idx + 1;
            ++match_idx;
            s_idx = match_idx;
        }
        else {
            return false;
        }
    }

    while(p_idx < pattern.size() && pattern[p_idx] == '*') {
        ++p_idx;
    }
    return p_idx == pattern.size();
}

constexpr std::string_view rtrim_spaces(std::string_view sv) noexcept
{
    size_t last = sv.find_last_not_of(' ');
    return (last == std::string_view::npos) ? std::string_view{} : sv.substr(0, last + 1);
}

/*
 * @brief Checks if a fixed-width unpacked record key matches a parsed wildcard key template field-by-field.
 *
 * Slices the unpacked key according to the field lengths defined in current_key,
 * trims trailing padding spaces from each segment, and evaluates it against the field pattern.
 *
 * @param unpacked_key The fixed-width unpacked key stored in key_record_map.
 * @param pattern_key  The parsed TDBKeyNew object representing the search template.
 * @return true        If every sub-field matches its corresponding wildcard pattern.
 */
bool TDBKeyMap::match_unpacked_key(std::string_view unpacked_key, const TDBKeyNew& pattern_key) const noexcept
{
    size_t offset = 0;
    const size_t total_fields = pattern_key.fieldsInKey();

    for(size_t i = 0; i < total_fields; ++i) {
        const size_t fld_len = pattern_key.fieldLength(i);
        if(offset + fld_len > unpacked_key.size()) {
            return false;
        }
        std::string_view key_field = rtrim_spaces(unpacked_key.substr(offset, fld_len));
        std::string pat_field = pattern_key.fieldKey(i);
        if(pat_field != "*" && !match_wildcard(key_field, pat_field)) {
            return false;
        }

        offset += fld_len;
    }
    return true;
}

std::vector<std::string> TDBKeyMap::xlist(const std::string& keypat) const
{
    std::vector<std::string> found_keys;
    TDBKeyNew pattern_key = current_key;
    pattern_key.setKey(keypat);

    for (const auto& [unpacked_key, entry] : key_record_map) {
        if (match_unpacked_key(unpacked_key, pattern_key)) {
            found_keys.push_back(unpacked_key);
        }
    }

    return found_keys;
}


//-------------------------------------------------------------
// Database - low level
//-------------------------------------------------------------


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

/**
 * @brief Serializes the record header, packed key, and all data objects into a binary database stream.
 *
 * Positions the output stream at the allocated file offset, writes the 14-byte RecHead,
 * appends the packed key terminated by MARKRKEY, writes all associated data objects
 * (inserting MARKEXTOBJ before extended objects if present), and returns remaining unused bytes.
 *
 * @param rep  Pointer to the RecEntry specifying the target file offset and allocated length.
 * @param f    Target binary database stream (GemDataStream).
 * @param tcrt Unsigned 32-bit creation/modification timestamp to embed in the record header.
 * @return int32_t Remaining unallocated bytes (StillLen); 0 indicates an exact size match.
 */
int32_t TDataBase::putrec(const RecEntry* rep, GemDataStream& f, uint32_t tcrt)
{
    ErrorIf(rep == nullptr, getKeywd(), "Internal error: Null RecEntry pointer passed to putrec.");

    const std::string pack_key = PackKey();
    RecHead rh{};

    std::memcpy(rh.bgm, MARKRECHEAD, 2);
    std::memcpy(rh.endm, MARKRECHEAD, 2);
    rh.nRT = record_type;
    rh.Nobj = static_cast<unsigned char>(headerObjCount());
    rh.rlen = rep->len;
    rh.time_crt = tcrt;

    int32_t StillLen = rep->len;

    f.seekp(rep->pos, std::ios::beg);
    rh.write(f);

    const size_t key_len = pack_key.length();
    f.writeArray(pack_key.data(), key_len);
    char marker = MARKRKEY;
    f.writeArray(&marker, 1);
    StillLen -= static_cast<int32_t>(key_len + 1);
    ErrorIf(!f.good(), getKeywd(), "PDB file write error while writing record header and key.");

    const size_t hdr_obj_cnt = static_cast<size_t>(headerObjCount());
    for(size_t j = 0; j < nOD; ++j) {
        if(j == hdr_obj_cnt && j != nOD) {
            f.writeArray(const_cast<char*>(MARKEXTOBJ), 4);
            StillLen -= 4;
        }
        StillLen -= static_cast<int32_t>(aObj[j + frstOD]->toDB(f));
    }
    ErrorIf(!f.good(), getKeywd(), "PDB file write error while writing record data objects.");

    crt_time = static_cast<std::time_t>(rh.time_crt);
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

/**
 * @brief Reads and deserializes a record header, packed key, and data objects from a binary stream.
 *
 * Seeks to the physical file offset specified in rep, validates the 14-byte RecHead markers
 * and object count, extracts the MARKRKEY-terminated key into the active key state,
 * deserializes all associated data objects (including extended settings if applicable),
 * and verifies that the actual read byte count matches the header specification.
 *
 * @param[in]  rep Pointer to the RecEntry specifying the record's file offset and expected length.
 * @param[in]  f   Source binary database stream (GemDataStream).
 * @param[out] rh  Reference to a RecHead structure populated with the read header metadata.
 * @return int32_t Remaining unread bytes (0 indicates exact match; negative during raw scanfile).
 */
int32_t TDataBase::getrec(const RecEntry* rep, GemDataStream& f, RecHead& rh)
{
    ErrorIf(rep == nullptr, getKeywd(), "Internal error: Null RecEntry pointer passed to getrec.");

    int32_t StillLen = rep->len;

    f.seekg(rep->pos, std::ios::beg);
    rh.read(f);
    ErrorIf(!f.good(), getKeywd(), "PDB file read error while reading record header.");

    if(std::strncmp(rh.bgm, MARKRECHEAD, 2) != 0 ||
        std::strncmp(rh.endm, MARKRECHEAD, 2) != 0 ||
        (rh.Nobj != nOD && (nOD + frstOD - 1) != o_tpstr &&
         (rh.Nobj + frstOD - 1) != o_phsdval && !(frstOD == o_spppar && rh.Nobj <= nOD))) {
        gui_logger->info("Record header format error {} {} {} {}", getKeywd(), rh.Nobj, nOD, frstOD);
        Error(getKeywd(), "Record header format error");
    }

    const size_t max_len = static_cast<size_t>(KeyLen() + KeyNumFlds()) + 1;
    std::string key(max_len, '\0');
    f.getline(key.data(), static_cast<std::streamsize>(max_len), MARKRKEY);
    ErrorIf(!f.good(), getKeywd(), "Database record key read error.");

    const size_t actual_len = key.find('\0');
    if (actual_len != std::string::npos) {
        key.resize(actual_len);
    }
    SetKey(key);
    StillLen -= static_cast<int32_t>(f.gcount());

    // 4. Десеріалізуємо об'єкти даних запису
    bool flag_spppar = false;
    for(int32_t j = 0; j < rh.Nobj; ++j) {
        if(j + frstOD == o_tpstr && StillLen < 28) {
            continue;
        }
        if(j + frstOD == o_sptext && StillLen < 10) {
            continue;
        }
        StillLen -= static_cast<int32_t>(aObj[j + frstOD]->ofDB(f));
        if(j + frstOD == o_spppar) {
            flag_spppar = true;
        }
    }

    if (frstOD == o_spppar) {
        StillLen -= static_cast<int32_t>(getExtendedSettings(f, rh.Nobj, StillLen));
    }

    ErrorIf(!f.good(), getKeywd(), "PDB file read error while reading record data objects.");

    if(StillLen != 0 && rep->len != 0 && !flag_spppar) {
        Error(getKeywd(), "Actual record length differs from that specified in the header");
    }

    crt_time = static_cast<std::time_t>(rh.time_crt);
    return StillLen;
}

/**
 * @brief Scans a database file stream for valid records, copies them to a clean stream, and rebuilds indices.
 *
 * Iterates through raw bytes starting at fPos up to fLen, locates record header markers (MARKRECHEAD),
 * extracts healthy records, resolves duplicate key conflicts interactively, writes compacted records
 * into outStream, and updates the in-memory index map with new file offsets.
 *
 * @param[in]     nF        Target database file index in dbfiles_list.
 * @param[in,out] fPos      Starting byte offset for scanning; updated as records are processed.
 * @param[in,out] fLen      Input upper byte bound of the source file; updated to the new compacted file size.
 * @param[in,out] inStream  Source binary stream containing fragmented/raw database records.
 * @param[in,out] outStream Destination binary stream receiving compacted healthy records.
 * @return int32_t Total number of valid records restored and written to outStream.
 */
int32_t TDataBase::scanfile(size_t nF, int32_t& fPos, int32_t& fLen,
                            GemDataStream& inStream, GemDataStream& outStream)
{
    RecEntry recordEntry{};
    RecHead recordHead{};
    int32_t fEnd = fPos;
    int32_t nRec = 0;

    recordEntry.nFile = static_cast<unsigned char>(nF);
    recordEntry.len = 0;

    while(fPos < fLen) {
        char ch = 0;
        inStream.seekg(fPos, std::ios::beg);
        inStream.get(ch);
        if (!inStream.good() || ch != MARKRECHEAD[0]) {
            fPos++;
            continue;
        }

        inStream.get(ch);
        if (!inStream.good() || ch != MARKRECHEAD[1]) {
            fPos++;
            continue;
        }

        recordEntry.pos = fPos;
        int32_t raw_len = 0;
        try {
            raw_len = getrec(&recordEntry, inStream, recordHead);
        }
        catch (TError& xcpt) {
            gui_logger->warn("scan error : {}", xcpt.mess);
            fPos += 2;
            continue;
        }

        fPos += (-raw_len) + static_cast<int32_t>(RecHead::data_size());
        const int32_t len = static_cast<int32_t>(reclen() + PackKey().length() + 1);

        std::string unpack_key = UnpackKey();
        const RecEntry* rh = keys_map.findkey(unpack_key);
        if(rh != nullptr) {
            std::string resolved_key = aMod[record_type]->GetKeyofRecord(
                unpack_key,
                "This key record already exists! Replace please?",
                KEY_NEW
                );
            if(resolved_key.empty()) {
                continue;
            }
            SetKey(resolved_key);
            unpack_key = UnpackKey();
        }

        RecEntry new_rh{};
        new_rh.nFile = static_cast<unsigned char>(nF);
        new_rh.len = len;
        new_rh.pos = fEnd;
        keys_map.putkey(unpack_key, new_rh);

        try {
            putrec(keys_map.findkey(unpack_key), outStream, static_cast<std::time_t>(recordHead.time_crt));
            nRec++;
            fEnd += len + static_cast<int32_t>(RecHead::data_size());
            gui_logger->debug("restore record: {}", PackKey());
        }
        catch (...) {
            keys_map.delkey(unpack_key);
            SetKey(ALLKEY);
            throw;
        }
    }

    fLen = fEnd;
    return nRec;
}

//--------------------- End of v_dbm1.cpp ---------------------------
