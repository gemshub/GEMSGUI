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
#include "v_mod.h"
#include "v_dbm.h"
#include "GEMS3K/gdatastream.h"


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
        set_all(); // Скидає всі поля у стан маски "*"
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
                std::string str(fld_raw_key, read_limit);
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
// TDBKey  - working with DB record key
//-------------------------------------------------------------

// default configuration
TDBKey::TDBKey( unsigned char nRkflds, const unsigned char* rkfrm ):
        rkFlds( nRkflds )
{
    ErrorIf( rkFlds<=0 || rkFlds>MAXRKFRMSTR,
             "TDBKey", "Error: Invalid number of fields in record key (1).");
    rkLen = new unsigned char[rkFlds];
    rkInd = new unsigned char[rkFlds];
    int i;
    for( fullLen=0, i=0; i<rkFlds; i++)
    {
        rkLen[i] = rkfrm[i];
        if( i==0 )
            rkInd[i]=0;
        else
            rkInd[i] = (rkInd[i-1] + rkLen[i-1]);
        fullLen += rkLen[i];
    }
    pKey = new char[fullLen+rkFlds+1+10];
    uKey = new char[fullLen+1+10];
    SetKey( ALLKEY);
    //pKey[0]='*'; pKey[1]='\0';
    //strcpy( uKey, pKey );
}

//configuration from cfg file

TDBKey::TDBKey(std::fstream& in_stream)
{
//    f.read( (char*)&rkFlds, sizeof(unsigned char) );
    int ii;
    // SD 23/12/2009 unsigned char tmp;
    unsigned int tmp;

    in_stream >> tmp;
    rkFlds = tmp;
    ErrorIf( rkFlds<=0 || rkFlds > MAXRKFRMSTR,
             "TDBKey", "Error: Invalid number of fields in record key (2).");
    rkLen = new unsigned char[rkFlds];
    rkInd = new unsigned char[rkFlds];
//    f.read( (char*)rkLen, rkFlds*sizeof(unsigned char) );
   for( ii=0; ii<rkFlds; ii++)
   {
     in_stream >> tmp;
     rkLen[ii] = tmp;
   }

   ErrorIf( !in_stream.good(), "TDBKey",
             "Error reading TDBKey from configurator.");
   for( fullLen=0, ii=0; ii<rkFlds; ii++)
   {
        if( ii == 0 )
            rkInd[ii] = 0;
        else
            rkInd[ii] = (rkInd[ii-1]+rkLen[ii-1]);
        fullLen += rkLen[ii];
    }
    pKey = new char[fullLen+rkFlds+1+10];
    uKey = new char[fullLen+1+10];
    SetKey( ALLKEY);
    //  pKey[0]='*'; pKey[1]='\0';
    //  strcpy( uKey, pKey );
}

// copy struct

TDBKey::TDBKey( const TDBKey& dbKey ):
        rkFlds( dbKey.rkFlds ),fullLen( dbKey.fullLen)
{
    rkLen = new unsigned char[rkFlds];
    rkInd = new unsigned char[rkFlds];
    pKey = new char[fullLen+rkFlds+1+10];
    uKey = new char[fullLen+1+10];
    for(int i=0; i<rkFlds; i++)
    {
        rkLen[i] = dbKey.rkLen[i];
        rkInd[i] = dbKey.rkInd[i];
    }
    strncpy(pKey, dbKey.pKey, fullLen+rkFlds+1);
    strncpy(uKey, dbKey.uKey, fullLen+1);
}

TDBKey::~TDBKey()
{
    delete[] rkLen;
    delete[] rkInd;
    delete[] pKey;
    delete[] uKey;
}

void
TDBKey::check()
{
    ErrorIf( pKey==nullptr || uKey==nullptr || rkLen==nullptr || rkInd==nullptr,
             "TDBkey", "No memory for key struct.");
}


// Putting DB record key key to internal structure
// in pack and unpack forms
void
TDBKey::SetKey( const char *key )
{
    ErrorIf( key==nullptr,"TDBKey", "No key buf.");

    //vvstr sp(fullLen+rkFlds+1);
    std::string sp;
    if( strncmp( key, ALLKEY, fullLen )==0 )
    {
        std::string st;
        for(int i=0; i<rkFlds; i++)
            st += "*:";
        sp = std::string( st, 0, fullLen);
    }
    else
        sp = char_array_to_string( key, fullLen);

    if( strchr( sp.c_str(), ':' ))  // key in packed form
        unpack( sp.c_str() );
    else
        pack( sp.c_str() );
 }

// Change i-th field of DBkey to key
void
TDBKey::SetFldKey( uint i, const char *key )
{
    ErrorIf( i>= rkFlds, key, "Invalid key field number");

    memset(uKey+rkInd[i], ' ' , rkLen[i]);
    strncpy(uKey+rkInd[i], key, rkLen[i]);
}

// unpack key
void
TDBKey::unpack( const char *key )
{
    int i;
    size_t ln;
    char *sp, *sp1;

    check();
    if( strchr( key, ':' ) )  // key in packed form
    {
        strncpy( pKey, key, fullLen);
        pKey[fullLen] = '\0';
        memset( uKey, ' ', fullLen );
        uKey[fullLen]= '\0';
        for( i=0, sp=pKey; i<rkFlds; i++)
        {
            sp1 = strchr( sp, ':' );
            if( !sp1 )
            {
                if( strncmp( sp, S_ANY, 1 ))
                {
                    ln = strlen( sp );
                    ErrorIf( ln==0, key, "Invalid packed record key.");
                }
                else
                { // end '*'
                    ln = 1;
                    strncpy( uKey+rkInd[i], sp, ln );
                    break;
                }
            }
            else  ln = ( sp1- sp);
            ln = std::min<size_t>( ln, rkLen[i] );
            strncpy( uKey+rkInd[i], sp, ln );
            if( *(sp+ln) == ':' )
                  ln++;
            else
              if( *(sp+ln+1) == ':' )
                  ln+=2;
            sp += ln;
        }
        uKey[fullLen]= '\0';
        //// strncpy( key, uKey, fullLen );
    }
}

// pack key
void
TDBKey::pack( const char *key )
{
    uint i;
    int len, ln;
    char *sp;

    check();
    if( !strchr( key, ':' ))  // key in unpacked form
    {
        strncpy( uKey, key, fullLen);
        uKey[fullLen] ='\0';
        memset( pKey, '\0', fullLen+1 );
        for( i=0, len=0; i<rkFlds; i++)
        {
            sp = FldKey(i);
            for( ln = rkLen[i]-1; ln>=0; ln--)
                if(  *(sp+ln) != ' ' ) break;
            ln++;
            if( ln < rkLen[i] )
            {
                strncpy( pKey+len, sp, ln );
                len += ln+1;
                pKey[len-1]=':';
            }
            else
            {
                strncpy(pKey+len, sp, rkLen[i] );
                len += rkLen[i]+1;
                pKey[len-1]=':';
            }
        }
       //// strncpy( key, pKey, fullLen );
    }
}

// make packed key to seach.
void
TDataBase::MakeKey( unsigned char nRTwrk, char *pkey, ... )
{
    va_list Marker;
    unsigned char rkflen;
    uint i, nkf;
    int rts;
    char /**sp,*/ *imf;

    ErrorIf( pkey==nullptr,"MakeKey", "No key buf to make key.");
    *pkey = 0;
    va_start( Marker, pkey );
    for( i=0; i<ind.KeyNumFlds(); i++ )
    {
        //sp= pkey+strlen(pkey);
        rkflen = ind.FldLen(i);
        rts = va_arg( Marker, int );
        switch( rts )
        {
        case K_END:
            strcat( pkey, S_ANY );
            va_end( Marker );
            return;
        case K_EMP:  // field "`"
            strcat( pkey, S_EMPTY );
            break;
        case K_ANY:  // field  "*"
            strcat( pkey, S_ANY );
            break;
        case K_IMM:  // field in string
            imf = va_arg( Marker, char * );
            strncat(pkey, imf, rkflen);
            break;
        case K_ACT:  // get field from  PRIE request
            rts = nRTwrk;
            [[fallthrough]];
        default:     // get field from enathe chain key
            nkf = va_arg( Marker, uint );
            if( !*rt[rts]->FldKey( nkf ))
                strcat( pkey, S_ANY );
            else
            {
                std::string str=  char_array_to_string( rt[rts]->FldKey( nkf ), std::min( rt[rts]->FldLen(nkf), rkflen ));
                StripLine( str );
                strncat( pkey, str.c_str(), rkflen );
            }
            break;
        }
//04/09/01 ????        if( strlen(sp) < rkflen )
            strcat( pkey, ":" );
    }
    va_end( Marker );
}

//-------------------------------------------------------------
// TDBKeyList - working with DB keys list
//-------------------------------------------------------------

// default configuration
TDBKeyList::TDBKeyList( unsigned char nRkflds, const unsigned char* rkfrm ):
        TDBKey( nRkflds, rkfrm )
{
    init();
}

//configuration from cfg file
TDBKeyList::TDBKeyList(std::fstream& f):
        TDBKey(f)
{
    init();
}

void
TDBKeyList::check_ndx()
{
    ErrorIf( ndx[0]==nullptr || re==nullptr ,"TDBMKeyList",
             "No memory for list of indexes.");
}

void
TDBKeyList::check_i(uint i)
{
    if( i>recInDB )
        Error( "TDBMKeyList","Invalid record index in the chain.");
}

//initialization of DB keys list state
void
TDBKeyList::init()
{
    nI = -1;
    recInDB = 0;
    nBuf = IND_PLUS;
    size_t k;

    for(uint i=0; i<KeyNumFlds(); i++)
    {
        k = nBuf * FldLen(i);
        ndx[i] = new char[k];
    }
    re = new RecEntry[nBuf];
}

// reset index key list
void
TDBKeyList::initnew()
{
    nI = -1;
    recInDB = 0;
    for(uint j=0; j<KeyNumFlds(); j++)
        memset( ndx[j], 0, FldLen(j)*nBuf*sizeof(char) );
    memset( re, 0, nBuf*sizeof(RecEntry) );
    aKey.clear();
    anR.clear();
}

// delete the keys of records file nF
void
TDBKeyList::delfile( uint nF )
{
    uint i=0;
    while( i < recInDB )
    {
        if( re[i].nFile == nF )
            delndx( i );
        else i++;
    }
}

TDBKeyList::~TDBKeyList()
{
    for(int i=0; i<KeyNumFlds(); i++)
        delete[] ndx[i];
    delete[] re;
}

RecEntry*
TDBKeyList::RecPosit(uint i)
{
    return &re[i];
}

// compare key i record and work key
int
TDBKeyList::keycom( uint i )
{
    int l = 0;
    for( uint j=0; j<KeyNumFlds(); j++)
    {
        l = memcmp( FldKey(j), RecKeyFld( i,j ), FldLen(j) );
        if( l )
           return l;
    }
    return l;
}

//Add new key of record in list. Realloc memory if nessasery.
uint
TDBKeyList::addndx( uint nF, int len, const char *key )
{
    int i, l;
    uint j;

    SetKey( key );
    if( recInDB >= nBuf ) // realloc buf
    {
        nBuf += IND_PLUS;
        for( j=0; j<KeyNumFlds(); j++)
        {
            char* new_sp = new char[nBuf*FldLen(j)];
            memcpy(new_sp, ndx[j], (nBuf-IND_PLUS)*FldLen(j));
            delete[] ndx[j];
            ndx[j] = new_sp;
        }
        RecEntry* new_sp = new RecEntry[nBuf];
        memcpy(new_sp, re, (nBuf-IND_PLUS)*sizeof(RecEntry));
        delete[] re;
        re = new_sp;
    }
    i = recInDB;
    if( i )  l = keycom( i-1 );
    else l = 1;
    while( l < 0 )
    {
        i--;
        if(!i) break;
        l = keycom( i-1 );
    }
    if(l==0) {
       Error( key, "Two records with the same key (AddNewKey).");
    }
    uint ii=i;
    for( j=0; j<KeyNumFlds(); j++)
        memmove( RecKeyFld(ii+1,j), RecKeyFld(ii,j),
                 FldLen(j)*(recInDB-ii)*sizeof(char) );
    memmove( re+ii+1, re+ii, (recInDB-ii)*sizeof(RecEntry) );
    for( j=0; j<KeyNumFlds(); j++)
        memcpy( RecKeyFld(ii,j), FldKey(j), FldLen(j) );
    re[i].len = len;
    re[i].nFile = nF;   //warning
    nI = i;
    recInDB++;
    return ii;
}

// Delete i-th record from list
void
TDBKeyList::delndx( uint i )
{
    uint j;

    check_i(i);
    for( j=0; j<KeyNumFlds(); j++)
        memmove( RecKeyFld(i,j), RecKeyFld(i+1,j),
                 FldLen(j)*(recInDB-i)*sizeof(char) );
    memmove( re+i, re+i+1, (recInDB-i)*sizeof(RecEntry) );
    recInDB--;
    if( i == recInDB )  i--;
    nI = i;
}

// find record in list
int
TDBKeyList::findx( const char *key )
{
    int l;

    SetKey( key );
    if( recInDB <= 0 ) return -1;
    for(uint i=0; i< recInDB; i++ )
    {
        l = keycom(i);
        if( l < 0 ) // No record
            break;
        if( l == 0 )
        {
            nI = i;
            return nI;
        }
    }
    return -1;
}

//Set key i - record to work key structure
void
TDBKeyList::PutKey( uint i)
{
    uint j;
    TDBKey::check();
    for( j=0; j<KeyNumFlds(); j++)
        memcpy( FldKey(j), RecKeyFld(i,j), FldLen(j) );
    SetKey( FldKey(0) );
}

//Put key i record to kbuf in unpack form.
void
TDBKeyList::RecKey(uint i, std::string& kbuf )
{
    uint j;
    check_i(i);
    kbuf = "";
    for( j=0; j<KeyNumFlds(); j++)
        kbuf += char_array_to_string( RecKeyFld(i,j), FldLen(j) );
}

// write the keys of records to ndx file
void
TDBKeyList::PutKeyList( uint nF, GemDataStream& f) const
{
    unsigned char j;
    for(uint i=0; i< recInDB; i++ )
        if( re[i].nFile == nF )
        {
            for( j=0; j<KeyNumFlds(); j++)
                f.writeArray(  RecKeyFld(i,j), FldLen(j) );
            //        f.write( (char *)(re+i), sizeof(RecEntry) );
            re[i].write (f);
        }
    ErrorIf( !f.good(), "TDBKeyList",  "Index file write error.");
}

// read the keys of records from ndx file
void TDBKeyList::GetKeyList_i(uint nF, int nRec, GemDataStream& f, const std::string& file_name)
{
    RecEntry re_;
    //vvstr key(KeyLen()+1);
    char key[MAXRKEYLEN+10];
    memset( key, 0, KeyLen()+1);
    for( int i=0; i< nRec; i++ )
    {
        f.readArray( key, KeyLen());
        //     f.read( (char *)&re_, sizeof(RecEntry));
        ErrorIf( !f.good(), "TDBKeyList", file_name+ " index file read error.");
        re_.read (f);
        auto indRec = addndx( nF, re_.len, key);
        re[indRec].pos = re_.pos;
    }
    SetKey(ALLKEY); // bugfix: current key for not readed record
    ErrorIf( !f.good(), "TDBKeyList", file_name+ " index file read error.");
}

// compare key ni record and pattern in work key
uint
TDBKeyList::keycmp( uint ni )
{
    if( !(strchr(UnpackKey(),'*') || strchr(UnpackKey(),'?')))
        return  keycom(ni);      // no pattern

    for( uint j=0; j<KeyNumFlds(); j++)
    {
        char *sp = FldKey(j);
        if( *(sp) == '*' )
	    continue; //to next field

        for( uint i=0; i<FldLen(j); i++ )
        {
            switch( *(sp+i) )
            {
            case '?': // no ' '
                if( *(RecKeyFld(ni,j)+i) == ' ' )
		    return( j+1 );
                break;

            case '*':
                goto NEXTKF;

            default:
                if( *(sp+i) != *(RecKeyFld(ni,j)+i) )
                    return( j+1 );
            }
        }
NEXTKF:
        ;
    }

    return 0;
}

//add records key in key list for a wildcard search
void
TDBKeyList::arec_add( uint ni )
{
    std::string s;
    RecKey( ni, s);
    aKey.push_back( s );
    anR.push_back( ni );
}

//get key list for a wildcard search
size_t
TDBKeyList::xlist( const char *pattern )
{
    bool OneRec, AllRecs;
    SetKey( pattern );

    // analyze pattern
    if( strchr( UnpackKey(), '*' ) || strchr( UnpackKey(), '?' ))
        OneRec = false;
    else
    {
        OneRec = true;/* AllRecs = false; */
    }
    if( !OneRec && strspn(UnpackKey(),"* :") < strlen(UnpackKey()) )
        AllRecs = false;
    else
        AllRecs = true;
    aKey.clear();
    anR.clear();
    if( OneRec )
    {
       auto i = findx( pattern );
        if( i >= 0 )
            arec_add( i );
        return aKey.size();
    }
    for(uint ii=0; ii<RecCount(); ii++ )
    {
        if( !AllRecs )
            if( keycmp(ii) != 0 )
                continue;
        arec_add(ii);
    }
    return aKey.size();
}

//--------------------- End of v_dbm1.cpp ---------------------------
