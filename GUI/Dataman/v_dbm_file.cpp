//-------------------------------------------------------------------
// $Id: v_dbm_file.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of TDBFile class
//
// Copyright (C) 2013 S.Dmytriyeva
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

#include "v_dbm_file.h"
#include "v_user.h"
#include "visor.h"

#include "gdatastream.h"

const int MAXFESTACK = 200;   // size array deleted record
extern const char* MARKRECDEL;
extern const char* MARKRECHEAD;

/// Reading header of DB file
void VDBhead::read(GemDataStream& is)
{
    is.readArray (VerP, 8 );
    is.readArray (PassWd, 8);
    is.readArray (Date, 11);
    is.readArray (Time, 5);
    is >> nRT;                 // type of PDB chain
    is >> nRec;                // number records in file
    is >> stacOver;
    is >> FPosTRT;
    is >> isDel;
    is >> MinDrLen;
    is >> MaxDrLen; // min and max size of deleted block
    is >> curDr;
    ;               // number of deleted blocks
}

/// Writing header of DB file
void VDBhead::write(GemDataStream& is)
{
    is.writeArray (VerP, 8 );
    is.writeArray (PassWd, 8);
    is.writeArray (Date, 11);
    is.writeArray (Time, 5);
    is << nRT;                 // type of PDB chain
    is << nRec;                // number records in file
    is << stacOver;
    is << FPosTRT;
    is << isDel;
    is << MinDrLen;
    is << MaxDrLen; // min and max size of deleted block
    is << curDr;
    ;               // number of deleted blocks
}


/// read deleted block description
void DBentry::read(GemDataStream& is)
{
    is >> pos;
    is >> len;
}

/// write deleted block description
void DBentry::write(GemDataStream& is)
{
    is << pos;
    is << len;
}


/// read DBrecord description
void RecEntry::read(GemDataStream& is)
{
    is >> pos;
    is >> len;
    is >> nFile;
}

/// write DBrecord description
void RecEntry::write(GemDataStream& is)
{
    is << pos;
    is << len;
    is << nFile;
}

/// read DB record head
void RecHead::read(GemDataStream& is)
{
    is.readArray (bgm, 2);
    is >> nRT;
    is >> Nobj;
    is >> rlen;
    is >> crt;
    is.readArray (endm, 2);
}

/// write DB record head
void RecHead::write(GemDataStream& is)
{
    is.writeArray (bgm, 2);
    is << nRT;
    is << Nobj;
    is << rlen;
    is << crt;
    is.writeArray (endm, 2);
}


//-------------------------------------------------------------
// TDBFile, files that contain DB records
//-------------------------------------------------------------

/*// Default configurations
TDBFile::TDBFile(const string& fName,
                 const string& fExt, const string& fDir):
        TFile(fName, fExt, fDir),
        FPosFree(-1),
        f(*new GemDataStream())
{
     makeKeyword();
}
*/

char TDBFile::vv[9]="GEMBASE1";
char TDBFile::pa[9]="01041999";

/// Configurations from file path
TDBFile::TDBFile(const string& path):
        TFile(path),
        FPosFree(-1),  defaultDB(false),
        f(*new GemDataStream())
{
     makeKeyword();
     if( path.find( pVisor->sysDBDir()) != string::npos )
        defaultDB = true;
}

/// Load configurations from cfg file
TDBFile::TDBFile(fstream& file):
        TFile(file),
        FPosFree(-1),  defaultDB(false),
        f(*new GemDataStream())
{
    if( Path.find( pVisor->sysDBDir()) != string::npos )
         defaultDB = true;
}


TDBFile::~TDBFile()
{
    if( Test() != NOT_OPEN )
        Close();
    delete &f;
}

/// Make keyword of DB internal file
void TDBFile::makeKeyword()
{
    string key;
    if( name.empty() )
        return;

    // ignoring version and after
    // get version
    string fname = name;
    size_t pos1 = fname.find(".ver");
    if( pos1 != string::npos )
       fname = fname.substr(0, pos1 );

    key = string(fname, 0, 2);
    size_t npos = 0;
    size_t npos2 = fname.find(".", npos);
    while( npos2 != string::npos )
    {   npos = npos2+1;
        key += string(fname, npos, 2);
        npos2 = fname.find(".", npos);
    }
    key += string(fname, npos+2);

    strncpy( Keywd, key.c_str(), MAX_FILENAME_LEN);
    Keywd[MAX_FILENAME_LEN]='\0';
}

/// Create new PDB file. If file exist, it will be deletet.
void TDBFile::Create( unsigned char nRT, bool isDel )
{
    GemDataStream ff;

    vdbh_new( vv, pa, nRT, isDel );
    FPosFree = dh.FPosTRT;
    ff.open( GetPath().c_str(), WRITE_B );
    dh.write (ff);
    if( dh.isDel )
    {
        DBentry csfe(0,0);
        for( int ii=0; ii<MAXFESTACK; ii++)
            csfe.write(ff);
    }
    ErrorIf( !ff.good(), GetPath(), "PDB file creation error.");
    ff.close();
}

/// Open file
void TDBFile::Open()
{
    if( Test() )
        return;

// it seems like latest libstdc++ specs does not enforce
// state and pointer reset :(
// we have to do it on our own
    f.clear();
    f.open( Path.c_str(), UPDATE_DBV );

    ErrorIf( !f.good(), GetPath(), "File open error.");
    f.seekp(0, ios::beg);
    f.seekg(0, ios::beg);

    isopened = true;
    getHead( f );
    f.seekg(0l, ios::end);
    FPosFree = f.tellg();
}


/// Close PDB file
void TDBFile::Close()
{
    f.close();
    clrh();
    isopened = false;
}


/// Set date and time in header of file.
void TDBFile::vdbh_setdt()
{
    memcpy(dh.Date, curDate().c_str(), curDate().length());
    dh.Date[curDate().length()]='\0';
    memcpy(dh.Time, curTime().c_str(), curTime().length());
    dh.Time[curTime().length()]='\0';
}

/// Clear the header of PDB file.
void TDBFile::clrh()
{
    FPosFree = -1;
    sfe.clear();
    memset( &dh, 0, sizeof( VDBhead ));
}

/// Set header of PDB file
void TDBFile::vdbh_new( const char *VerP,
     const char *passwd, int nRT, bool ifDel )
{
    clrh();
    if( VerP )
        strncpy( dh.VerP, VerP, 8 );
    if( passwd )
        strncpy( dh.PassWd, passwd, 8 );
    vdbh_setdt();
    dh.nRT = nRT;
    dh.FPosTRT = dh.stacOver = VDBhead::data_size();
    dh.isDel = ifDel;
    if( ifDel )
        dh.FPosTRT += DBentry::data_size() * MAXFESTACK;
    dh.nRec = 0;
    dh.curDr = 0;
    dh.MinDrLen = 0;
    dh.MaxDrLen = 0;
}


/// Read the control structure of PDB file.
void TDBFile::getHead(GemDataStream& fdb)
{
    fdb.seekg( 0L, ios::beg );
    dh.read (fdb);
    sfe.clear();

    if( dh.isDel )
    {
        DBentry csfe(0,0);
        for( int ii=0; ii<dh.curDr; ii++)
        {
            csfe.read(fdb);
            sfe.push_back( csfe );
        }
    }
    ErrorIf( !fdb.good(), GetPath(), "PDB head read error.");
}

/// Write the control structure of PDB file.
void TDBFile::PutHead( GemDataStream& fdb, int deltRec )
{
    dh.nRec += deltRec;
    vdbh_setdt();

    fdb.seekg(0L, ios::beg );
    dh.curDr = sfe.size();
    dh.write (fdb);
    if( dh.isDel )
    {
      list<DBentry>::iterator it;
      it = sfe.begin();
      while( it != sfe.end() )
      {   it->write (fdb); it++; }
    }
    ErrorIf( !fdb.good(), GetPath(), "Database file write (protection?) error");
    fdb.flush();
}


/// Add new deleted record in sfe list and mark deleted record in file
void TDBFile::AddSfe( RecEntry& re )
{
    RecHead rh;

    if( dh.isDel )
    {

        if( dh.curDr >= MAXFESTACK )
        {
          if( dh.stacOver >= 0 )
          {
            dh.stacOver = -1;
           // vfMessage(0, GetPath(),"Stack of deleted records overflow." );
          }
        }
        else
        {
            list<DBentry>::iterator it;
            it = sfe.begin();
            while( it != sfe.end() )
            {
              if( re.len <= it->len )
                break;
              it++;
            }
            if( it == sfe.begin() )
              dh.MinDrLen = re.len;
            if( it == sfe.end() )
              dh.MaxDrLen = re.len;
            sfe.insert( it,  DBentry(re.pos, re.len) );
            dh.curDr++;
        }
    }

    // mark deleted record in file
    f.seekg(re.pos, ios::beg );
    rh.read (f);
    if( !strncmp( rh.bgm, MARKRECHEAD, 2 ))
    {
        strncpy( rh.bgm, MARKRECDEL, 2 );
        strncpy( rh.endm, MARKRECDEL, 2 );
    }
    f.seekg(re.pos, ios::beg );
    rh.write (f);
    ErrorIf( !f.good(), GetPath(), "PDB file write error.");
}

/// Find deleted block in sfe list for puttig record here
void TDBFile::FindSfe( RecEntry& re )
{
    int i,j;
    RecEntry rn;

    int Ndr = dh.curDr;
    if( !dh.isDel || Ndr==0 || dh.MaxDrLen<re.len)
    {
        // no sfe list or no deleted blocks or big len
        re.pos = FPosFree;
        FPosFree += re.len;
        return;
     }
    // seach block
    list<DBentry>::iterator it;
    it = sfe.begin();
    while( it != sfe.end() )
    {
        if( re.len <= it->len )
          break;
        it++;
    }
    if( it == sfe.end() )
    {
        // big len
        re.pos = FPosFree;
        FPosFree += re.len;
        return;
     }
    re.pos = it->pos;
    rn.pos = re.pos + re.len;
    rn.len = it->len - re.len;
    rn.nFile = re.nFile;
    // deleted block from sfe list
    sfe.erase(it);
    if( rn.len >= dh.MinDrLen )
        AddSfe( rn );
}

/// Get information from dh
void TDBFile::GetDh( int& fPos, int& fLen, int& nRT, int& isDel )
{
    fPos = dh.FPosTRT;
    fLen = FPosFree;
    nRT = dh.nRT;
    isDel = dh.isDel;
}

/// Test a lot of deleted blocks
bool TDBFile::GetDhOver()
{
    return  (dh.stacOver < 0);
}

/// Sets information into compressed PDB file header
void TDBFile::SetDh( int fLen, int nRec, int nRT, int isDel )
{
    clrh();
    strncpy( dh.VerP, vv, 8 );
    strncpy( dh.PassWd, pa, 8 );
    vdbh_setdt();
    dh.nRT = nRT;
    dh.FPosTRT = dh.stacOver = VDBhead::data_size();
    dh.isDel = isDel;
    if( isDel )
        dh.FPosTRT += DBentry::data_size() * MAXFESTACK;
    dh.nRec = nRec;
    dh.curDr = 0;
    dh.MinDrLen = 0;
    dh.MaxDrLen = 0;
    FPosFree = fLen;
    PutHead( f );

    f.flush();
}

// file names constants
const char * PDB_EXT = "pdb";
const char * NDX_EXT = "ndx";

//--------------------- End of v_dbm_file.cpp ---------------------------

