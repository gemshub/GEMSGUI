//-------------------------------------------------------------------
// $Id: v_dbm2.cpp 1179 2009-01-20 10:27:35Z gems $
//
// Implementation of TDataBase, DataBaseList classes
//
// Copyright (C) 1996-2001 S.Dmytriyeva, A.Rysin
// Uses  gstring class (C) A.Rysin 1999
//
// This file is part of the GEM-Vizor library which uses the
// Qt v.2.x GUI Toolkit (Troll Tech AS, http://www.trolltech.com)
// according to the Qt Duo Commercial license
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#include "v_user.h"
#include "v_dbm.h"
#include "v_dbfile.h"
#include "v_object.h"
#include "v_mod.h"
#include "visor.h"
#include "service.h"
#include "gdatastream.h"


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

bool TDataBase::dbChangeAllowed( int nf, bool /*ifRep*/ )
{
    return ( pVisor->isDBChangeMode() /*&&  ifRep==true*/ )
	    || ( nf >= specialFilesNum );
}

#endif

void TDataBase::check_file( int nF )
{
    ErrorIf( nF<0 || nF>(int)aFile.GetCount(),
             GetKeywd(),"Invalid file number.");
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
    out_stream << ((unsigned short)aLen[ii]) << ' ';
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
//    f.write( (char*)&specialFilesNum, sizeof(int) );
out_stream << specialFilesNum <<  "  "; //  endl;

    int numFls = fls.GetCount();
//    f.write( (char*)&nF, sizeof(int) );
out_stream << numFls << "  "; // << endl;
    for(int ii=0; ii<numFls; ii++) {
//        f.write( (char*)&fls[i], sizeof(int) );
out_stream << fls[ii] << ' '; // << endl;
    }
    int numFiles = aFile.GetCount();
//    f.write( (char*)&nF, sizeof(int) );
out_stream << "  " << numFiles << endl;

    ErrorIf( !out_stream.good(), GetKeywd(),
             "Error writing TDataBase to configurator");

    for(int ii=0; ii<numFiles; ii++) {
        aFile[ii].toCFG(out_stream);
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
        fls.Add(fl);
    }
//    f.read( (char*)&nF, sizeof(int) );
int numFiles;
in_stream >> numFiles;

    ErrorIf( !in_stream.good(), GetKeywd(),
             "Error reading TDataBase from configurator");

    for(int ii=0; ii<numFiles; ii++)
        aFile.Add( new TDBFile(in_stream));
}

//default configuration of the database
TDataBase::TDataBase( int nrt, const char* name,
                      bool Rclose, bool isDel, int nOf, unsigned char Nobj,
                      int filesNum, unsigned char nRkflds, const unsigned char* rkfrm ):
        nRT((unsigned char)nrt), status(UNDF_), rclose(Rclose), isDelete(isDel),
        frstOD(nOf), nOD(Nobj),
        ind( nRkflds, rkfrm ),
        aFile(4, 1), fls(2, 1), specialFilesNum(filesNum)
{
    strncpy( Keywd, name, MAXKEYWD-1 );
    Keywd[ MAXKEYWD-1 ] = 0;
    fNum = 0;
    crt = time(NULL);
}

//configuration from cfg file
TDataBase::TDataBase( fstream& f ):
        status(UNDF_), ind(f ), aFile(4, 1), fls(2, 1)
{
    fromCFG(f);
    crt = time(NULL);
    fNum = 0;
}

TDataBase::~TDataBase()
{
    for(uint j=0; j<fls.GetCount(); j++ )
        aFile[fls[j]].Close();

    //  aFile.Clear();
    fls.Clear();
    fNum = 0;
    fOpenNameBuf.Clear();
}

// add new file to database chain
void TDataBase::AddFile(const gstring& path)
{
    aFile.Add(new TDBFile(path));
}

// put information to index file
void TDataBase::putndx( int nF )
{
    gstring Path;
    GemDataStream f;

    check_file(nF);
    Path = u_makepath( aFile[nF].Dir(),
                aFile[nF].Name(), NDX_EXT );
    f.open( Path.c_str(), WRITE_B);
    ErrorIf( !f.good(), Path.c_str(),"Index file open error");
    aFile[nF].vdbh_setdt( );
    //  f.write( (char *)aFile[nF]->Dh(), sizeof(VDBhead) );
    aFile[nF].Dh()->write (f);
    ind.PutKeyList( nF, f);
}

// get information from index file
void TDataBase::getndx( int nF )
{
    gstring Path;
    VDBhead dh;
    GemDataStream f;

    check_file( nF );
    Path = u_makepath( aFile[nF].Dir(),
                aFile[nF].Name(), NDX_EXT );
    f.open( Path.c_str(), RDONLY_B);
    if( !f.good() )
      Error(  Path.c_str(),"Index file open error");
    //   f.read( (char *)&dh, sizeof(VDBhead) );
    dh.read (f);
    ind.GetKeyList_i( nF, dh.nRec, f);
}

// get record length in the PDB file
int TDataBase::reclen( )
{
    int Olen=0;
    for(int j=0; j<nOD; j++ )
        Olen += aObj[j+frstOD].lenDB();
    return Olen;
}

// put record in DB file
int TDataBase::putrec( RecEntry& rep, GemDataStream& f )
{
    int j, len;
    int StillLen;
    RecHead rh;
    char *pack_key = (char*)ind.PackKey();

    // header of the record
    strncpy( rh.bgm, MARKRECHEAD, 2 );
    strncpy( rh.endm, MARKRECHEAD, 2 );
    rh.nRT = nRT;               //warning: compatibility
    rh.Nobj = nOD;
    rh.rlen =  rep.len;
    StillLen = rep.len;
    rh.crt = time( NULL );
    f.seekg(rep.pos, ios::beg );
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
        StillLen -= aObj[j+frstOD].toDB( f );
    crt = rh.crt;
    return StillLen;
}

// put record in DB file
int TDataBase::putrec( RecEntry& rep, GemDataStream& f, RecHead& rhh  )
{
    int j, len;
    int StillLen;
    RecHead rh;
    char *pack_key = (char*)ind.PackKey();

    // header of the record
    strncpy( rh.bgm, MARKRECHEAD, 2 );
    strncpy( rh.endm, MARKRECHEAD, 2 );
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
        StillLen -= aObj[j+frstOD].toDB( f );
    crt = rh.crt;
    return StillLen;
}


// Gets a record from PDB file
int TDataBase::getrec( RecEntry& rep, GemDataStream& f, RecHead& rh )
{
    int j;
    int StillLen;
    // RecHead rh;
    char *key = (char*)ind.PackKey();

    StillLen = rep.len;
    f.seekg(rep.pos, ios::beg );
    //   f.read( (char *)&rh, sizeof(RecHead) );
    rh.read (f);
    if( strncmp( rh.bgm, MARKRECHEAD, 2 ) ||
            strncmp( rh.endm, MARKRECHEAD, 2 ) ||
            (rh.Nobj != nOD && (nOD+frstOD-1) != o_tpstr  ) )
        Error( GetKeywd(),"Record header format error");
    f.getline( key, KeyLen()+KeyNumFlds(), MARKRKEY);
    ErrorIf( f.gcount()>=(KeyLen()+KeyNumFlds()), GetKeywd(),
             "Error reading database record key" );
    StillLen -= f.gcount();  //???+1
    // test
    bool flag_spppar = false;
    for( j=0; j<nOD; j++ )   // get objects from file
    {
        if ( j+frstOD == o_tpstr )
          if( StillLen < 28 )
             continue;
        StillLen -= aObj[j+frstOD].ofDB(f);
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
int TDataBase::AddRecordToFile( const char *pkey, int file )
{
    int len, oldlen;
    unsigned char nF;
    // test and open file
    fNum = file;
    nF = (unsigned char)fls[file];
    check_file( nF );

    ErrorIf( !dbChangeAllowed( nF ),
	aFile[nF].Name(), "Cannot add record: changes to system database are not allowed!" );


    aFile[nF].Open( UPDATE_DBV );


    int i = ind.addndx( nF, 0, pkey );     // add record
    len = reclen();
    len += RecHead::data_size() + strlen(ind.PackKey()) + 1;
    RecEntry* rh = ind.RecPosit(i);
    rh->len = len;


    aFile[nF].FindSfe( *rh );
    rh->len = len - RecHead::data_size();

    oldlen = aFile[nF].FLen();
    int iRet =putrec( *rh, aFile[nF].f );
    if( iRet!=0 )
    {
        if( aFile[nF].FLen() != oldlen )
            aFile[nF].FNewLen( iRet) ;
        rh->len -=iRet;
    }
    aFile[nF].PutHead( aFile[nF].f, 1);
    putndx(nF);
    status = ONEF_;
    if( rclose )
        aFile[nF].Close();
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
    ErrorIf( !dbChangeAllowed( nF, true ), aFile[nF].Name(),
                "Cannot modify record: changes to system database are not allowed!" );

    aFile[nF].Open( UPDATE_DBV );
    // delete record
    rh->len += RecHead::data_size();
    aFile[nF].AddSfe( *rh );

    ind.PutKey(i);
    rh->len = reclen() + RecHead::data_size() + strlen(ind.PackKey()) + 1;
    aFile[nF].FindSfe( *rh );
    rh->len -= RecHead::data_size();
    oldlen = aFile[nF].FLen();
    iRet =putrec( *rh, aFile[nF].f );
    if( iRet!=0 )
    {
        if( aFile[nF].FLen() != oldlen )
            aFile[nF].FNewLen( iRet );
        rh->len -=iRet;
    }
    aFile[nF].PutHead( aFile[nF].f, 0);
    putndx( nF );
    status = ONEF_;
    fNum = fls.Find( nF );
    if( rclose )  aFile[nF].Close();
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
	aFile[nF].Name(), "Cannot delete record: changes to system database are not allowed!");


    aFile[nF].Open( UPDATE_DBV );
    // delete record
    rh->len += RecHead::data_size();
    aFile[nF].AddSfe( *rh );
    ind.delndx(i);
    aFile[nF].PutHead( aFile[nF].f, -1);
    putndx( nF );
    status = UNDF_;
    if( rclose )  aFile[nF].Close();
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

    aFile[nF].Open( UPDATE_DBV );
    getrec( *rh, aFile[nF].f, rhh );
    ind.PutKey(i);
    status = ONEF_;
    fNum = fls.Find( nF );
    if( rclose )  aFile[nF].Close();
}

//Rename record (Change first field: for SYSEQ, and >)
//Sveta 24/07/2001
void TDataBase::RenameList( const char* newName,
        const char *oldName )
{
    if( strlen(newName) > FldLen(0) )
      return;

    gstring str_old = gstring( oldName, 0, FldLen(0) );
//04/09/01 ????    if( strlen(oldName)<FldLen(0) )
        str_old += ":";
    for( int i=1; i<KeyNumFlds(); i++)
            str_old += "*:";

    TCStringArray arKey;
    TCIntArray arR;

    uint Nrec = GetKeyList( str_old.c_str(), arKey, arR );
    if( Nrec < 1)
      return;

    int nrec;
    gstring str;

    for(uint i=0; i<Nrec; i++ )
    {
        nrec =  Find( arKey[i].c_str() );
        Get( nrec );
        // changing record key
        str = ind.PackKey();
        Del( nrec );
        str = str.replace( oldName, newName);
        AddRecordToFile( str.c_str(), fNum );
    }
}


//Read record update time from PDB file to memory.
time_t TDataBase::GetTime( int i )
{
    unsigned char nF;
    RecEntry* ree=ind.RecPosit(i);
    RecHead rh;

    ind.check_i(i);
    // test and open file
    nF = ree->nFile;
    check_file( nF );

    aFile[nF].Open( UPDATE_DBV );
    aFile[nF].f.seekg(ree->pos, ios::beg );
    //   aFile[nF]->f.read( (char *)&rh, sizeof(RecHead) );
    rh.read (aFile[nF].f);
    if( strncmp( rh.bgm, MARKRECHEAD, 2 ) ||
            strncmp( rh.endm, MARKRECHEAD, 2 ) || rh.Nobj != nOD )
        Error( GetKeywd(), "PDB record header format error");
    status =  UNDF_;
    ;
    fNum = fls.Find( nF );
    if( rclose )  aFile[nF].Close();
    return rh.crt;
}

//Seach record index with key pkey.
// Returns -1 no record.
int TDataBase::Find( const char *pkey)
{
    status = UNDF_;
    return ind.findx( pkey );
}

//Test state of record with key pkey.
//If mode == 1 and one record, read this record.
// returns state of record
RecStatus TDataBase::Rtest( const char *key, int mode )
{
    int iRec;
    const char *pkey;
    bool OneRec;

    if( key != 0 )
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
        iRec = Find( pkey );
        if( iRec < 0 )
            return NONE_;
        if( mode == 1)
            Get( iRec );
        return ONEF_;
    }
    else // template
    {
        iRec = ind.xlist( pkey );
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
bool TDataBase::FindPart( const char *key_, int field )
{

    vstr key(KeyLen(), key_);
    TDBKey dbKey(GetDBKey());
    dbKey.SetKey(key);
    dbKey.SetFldKey(field,"*");
    gstring str_key( dbKey.UnpackKey(), 0, KeyLen() );
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
void TDataBase::Create( int nF )
{
    check_file( nF );
    if( !aFile[nF].Exist() )
    {
        aFile[nF].Create( nRT, isDelete );
        putndx( nF );    // indexes
    }

}

// Makes a list of open file names
void TDataBase::opfils()
{
    fNum = 0;
    fOpenNameBuf.Clear();
    for(uint i=0; i<fls.GetCount(); i++)
        fOpenNameBuf.Add( aFile[fls[i]].Name() );
}

// Selection in the list of full file names
void TDataBase::GetFileList(int mode, TCStringArray& names,
                            TCIntArray& indeces, TCIntArray& sel)
{
    names.Clear();
    indeces.Clear();
    sel.Clear();
    for( uint i=0; i<aFile.GetCount(); i++ )
    {
        int nF = fls.Find(i);
        if( (nF==-1&&(mode&closef))||(nF!=-1&&(mode&openf)) )
        {
            aFile[i].Makepath();
            names.Add( gstring(aFile[i].GetKeywd())+gstring(" ")+
                       gstring(aFile[i].GetPath()));
            indeces.Add(i);
            if( (mode&oldself) && nF != -1) //select already open files
                sel.Add(indeces.GetCount()-1);
        }
    }
}

// open files in PDB
void
TDataBase::Open( bool type, FileStatus mode, const TCIntArray& nff )
{
    uint j;
    ///
    if( aFile.GetCount() == 0 )
        return;

    if( fls.GetCount()<=0 ) //no config
    {
        if( type )
        {
            if( nff.GetCount()<1 )
                fls.Add(0);
            for( j=0; j< nff.GetCount(); j++)
                fls.Add( nff[j] );
        }
        else  fls.Add(0);
    }
    //  opfils();
    ind.initnew();
    for( j=0; j<fls.GetCount(); j++ )
        Create( fls[j] );
try
  {
    TCIntArray comp;
    for( j=0; j<fls.GetCount(); j++ )
    {
        aFile[fls[j]].Open( mode );
        //added Sveta 04/11/2002 to index files
        if( aFile[fls[j]].GetDhOver())
// Sveta 06/2005
//         if( vfQuestion( 0, aFile[fls[j]].GetPath(),
//         "Stack of deleted records overflow.\nCompress?" ))
             comp.Add(fls[j]);
        // end added
        getndx( fls[j] );
        if( rclose )  aFile[fls[j]].Close();
    }
    opfils();
    //added Sveta 04/11/2002 to index files
        if( comp.GetCount() >0 )
         RebildFile( comp);
    // end added

  }
  catch( TError& xcpt )
  {
        if( xcpt.mess == "Two records with the same key.")
        {
          xcpt.mess += "\n 2nd record in file: ";
          xcpt.mess +=  aFile[fls[j]].Name();

          while( j < fls.GetCount() )
            fls.Remove(j);
         ind.initnew();

          for( j=0; j<fls.GetCount(); j++ )
          {
             aFile[fls[j]].Open( mode );
             getndx( fls[j] );
             if( rclose )  aFile[fls[j]].Close();
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
    uint j;

    if( aFile.GetCount() == 0 )
        return;

    Close();
    for( j=0; j< aFile.GetCount(); j++)
     if( only_kernel && (int)j >=  specialFilesNum )
       continue;
     else
        fls.Add( j );

    Open( true, UPDATE_DBV, 0 );
}


//close files in PDB
void TDataBase::Close()
{
    for(uint j=0; j<fls.GetCount(); j++ )
        aFile[fls[j]].Close();
    fls.Clear();
    fNum = 0;
    fOpenNameBuf.Clear();
    ind.initnew();
}


//close files in PDB
void TDataBase::OpenOnlyFromList( TCStringArray& names )
{
    if( aFile.GetCount() == 0 )
        return;

    Close();
    uint ii, jj;

    for( ii=0; ii< aFile.GetCount(); ii++)
    {
      for( jj=0; jj< names.GetCount(); jj++)
        if(  aFile[ii].Name().find( names[jj] ) != gstring::npos )
         break;
      if( jj < names.GetCount() )
         fls.Add( ii );
    }

    Open( true, UPDATE_DBV, 0 );
}

// add new file to DBfile list
int TDataBase::AddFileToList(TDBFile* file)
{
    aFile.Add( file );
try{
    Create(aFile.GetCount()-1);
    file->Open( UPDATE_DBV );
    getndx(aFile.GetCount()-1);
    fls.Add(aFile.GetCount()-1);
    if( rclose )
        file->Close();
    opfils();
 }
 catch( TError& xcpt )
  {
        if( xcpt.mess == "Two records with the same key.")
        {
          xcpt.mess += "\n 2nd record in file: ";
          xcpt.mess +=  aFile[aFile.GetCount()-1].Name();

          ind.initnew();

          for(uint j=0; j<fls.GetCount(); j++ )
          {
             aFile[fls[j]].Open( UPDATE_DBV );
             getndx( fls[j] );
             if( rclose )
                aFile[fls[j]].Close();
           }
           opfils();
         }
         Error( xcpt.title, xcpt.mess);
   }
   return fls.Find( aFile.GetCount()-1 );
}


/*// get number of file, that name conterned substring (only open files)
int TDataBase::GetFileNum(const char* substr_name)
{
  for(int ii=fls.GetCount()-1; ii>=0; ii-- )
  {
   if( aFile[fls[ii]].Name().find( substr_name ) != gstring::npos )
     return ii;
  }
  return -1;
}
*/

// delete a file from DBfile list
void TDataBase::DelFile(const gstring& path)
{
    TDBFile fl(path);

    for(uint ii=0; ii<aFile.GetCount(); ii++)
        if( fl.Name() == aFile[ii].Name() )
        {
            int  nF = ii;
            int  nFls = fls.Find( nF );
            if( nFls != -1 ) //close file and delete indexes
            {
                aFile[nF].Close();
                ind.delfile( nF );
                fls.Remove(nFls);
            }
            fNum = 0; //?????
            aFile.Remove(nF);
            opfils();
            break;
        }
}


// add open PDB files
void TDataBase::AddOpenFile(const TCIntArray& nff)
{
  uint j=0;
  if( nff.GetCount()<1 )
        return;
try{
    TCIntArray comp;

    for( j=0; j<nff.GetCount(); j++)
    {  //fls.Add( nff[j] );
        Create( nff[j] );
        aFile[nff[j]].Open( UPDATE_DBV );
        //added Sveta 04/11/2002 to index files
        if( aFile[nff[j]].GetDhOver())
         if( vfQuestion( 0, aFile[nff[j]].GetPath(),
         "Stack of deleted records overflow.\nCompress?" ))
             comp.Add(nff[j]);
        // end added
        getndx( nff[j] );
        fls.Add( nff[j] );
        if( rclose )
            aFile[nff[j]].Close();
    }
    opfils();
    //added Sveta 04/11/2002 to index files
        if( comp.GetCount() >0 )
         RebildFile( comp);
    // end added

  }
  catch( TError& xcpt )
  {
        if( xcpt.mess == "Two records with the same key.")
        {
          xcpt.mess += "\n 2nd record in file: ";
          xcpt.mess +=  aFile[nff[j]].Name();

          ind.initnew();

          for( j=0; j<fls.GetCount(); j++ )
          {
             aFile[fls[j]].Open( UPDATE_DBV );
             getndx( fls[j] );
             if( rclose )
                aFile[fls[j]].Close();
           }
           opfils();
         }
         Error( xcpt.title, xcpt.mess);
   }

}


//Get new lists of keys and indexes
int TDataBase::GetKeyList( const char *keypat,TCStringArray& aKey, TCIntArray& anR)
{
    int l = ind.xlist( keypat );
//    status = UNDF_;         // 09/11/2004 Sveta    Is it necessary ? KD 13.01.05
    aKey = ind.KeyList();
    anR =ind.RnList();
    return l;
}


// Scan database file
int TDataBase::scanfile( int nF, int& fPos, int& fLen,
		GemDataStream& inStream, GemDataStream& outStream)
{
    RecEntry recordEntry;
    RecHead recordHead;
    //RecEntry& rep = ind.RecPosit(0);
    int len, fEnd = fPos;
    int ni, nRec=0;

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
        {	/*
                   if( !vfQuestion( Keywd,
                        " Continue with possible loss of data in \n"
                        "the record or cancel PDB file compression?"))
                     break;
            	*/
            fPos += 2;
            continue;
        }

        fPos += (-len) + RecHead::data_size();
        len = reclen() + strlen(ind.PackKey()) + 1;
        ni = ind.addndx( nF, len, ind.PackKey() );
        //RecEntry& rep = ind.RecPosit(ni);
        ind.RecPosit(ni)->pos = fEnd;
        fEnd += len + RecHead::data_size();

        putrec( *ind.RecPosit(ni), outStream, recordHead );

        nRec++;

        pVisor->Message( 0, 0, "Compressing database file\n"
        		"Please, wait...", (int)fPos, (int)fLen);
    }
    fLen = fEnd;

    return nRec;
}

//Get new index file and physically deleted records
void TDataBase::RebildFile(const TCIntArray& nff)
{
    // close&open db files added Sveta 06/03/02
    TCIntArray fls_old;
    for(uint j=0; j< fls.GetCount(); j++)
        fls_old.Add( fls[j] );
    Close();

    for(uint j=0; j<nff.GetCount(); j++)
    {
	int  nRec, nRT, isDel;
	int fPos, fLen;
        unsigned char nF = (unsigned char)nff[j];
        // test and open file
        check_file( nF );
        if( ! dbChangeAllowed( nF ) )
        {
            ///vfMessage( aFile[nF]->Name(), "Cannot rebuild PDB file!");
            continue;
        }
        aFile[nF].Open( UPDATE_DBV );

        //ind.delfile( nF );
        aFile[nF].GetDh( fPos, fLen, nRT, isDel );

	gstring tmpFileName = aFile[nF].GetPath() + ".tmp";
	GemDataStream outStream( tmpFileName, ios::out | ios::binary );
        for(int ii=0; ii<fPos; ii++ )
          outStream.put(0);

	outStream.seekp(fPos/*VDBhead::data_size()*/, ios::beg);

        nRec = scanfile( nF, fPos, fLen, aFile[nF].f, outStream );

	outStream.close();

	aFile[nF].OpenFromFileName( tmpFileName, UPDATE_DBV );

        aFile[nF].SetDh( fLen, nRec, nRT, isDel );

        putndx( nF );
        status = UNDF_;

        aFile[nF].Close();
        ind.delfile( nF );
    }

    pVisor->CloseMessage();
    Open( true, UPDATE_DBV, fls_old );
}

//Set new open Files list  (Reopen by list)
bool TDataBase::SetNewOpenFileList(const TCStringArray& aFlKeywd)
{
    bool allOpend = true;
    
	if(aFlKeywd.GetCount() < 1 )
        return  allOpend;

    Close();
    for(uint i=0; i<aFlKeywd.GetCount(); i++)
    {
        int nF = -1;
        for(uint j=0; j<aFile.GetCount(); j++)
        {
            if( aFlKeywd[i] == aFile[j].GetKeywd() )
            {
                nF = j;
                break;
            }
	    }

        if( nF >= 0 )
            fls.Add(nF);
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

    Open(false, UPDATE_DBV, fls);
    return allOpend;
}

//Make new project-file and close another project files (make new Project)
void TDataBase::MakeInNewProfile(const gstring& dir,
   const char *prfName, const char * f_name )
{
    /* open only default files (no user files)*/
    //  Close();
    TCIntArray fl;
    for(uint  i=0; i<fls.GetCount(); i++ )
    {
        if( fls[i] < specialFilesNum)
        {
            fl.Add(fls[i]);
        }
    }
    Close();
    for(uint  ii=0; ii<fl.GetCount(); ii++ )
        fls.Add(fl[ii]);
    if( fls.GetCount() >= 1)
        Open(false, UPDATE_DBV, fl);

    /* add new project files*/
    gstring name(dir);
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
    gstring name = GetKeywd();
    name += ".";
    name +=secondName;
    for(uint i=0; i<fls.GetCount(); i++)
        if( name == aFile[fls[i]].Name() )
            return i;
    return 0;
}

// get list of files keywds, that contained name
void TDataBase::GetProfileFileKeywds( const char *_name, TCStringArray& aFlkey )
{
    gstring name = ".";
    name +=_name;
    name += ".";

    for(uint ii=0; ii< aFile.GetCount(); ii++)
    {
      if(  aFile[ii].GetPath().find( name ) != gstring::npos )
          aFlkey.Add( aFile[ii].GetKeywd() );
    }
}

// true if opened some files from default data base
bool TDataBase::ifDefaultOpen() const
{
    for(uint i=0; i<fls.GetCount(); i++)
        if( aFile[fls[i]].GetPath().find( pVisor->sysDBDir())
              != gstring::npos )
            return true;
    return false;
}

//-------------------------------------------------------------
// DataBaseList
//-------------------------------------------------------------

int DataBaseList::Find(const char* s)
{
    for( uint ii=0; ii<GetCount(); ii++ )
        if( strcmp(s, elem(ii).GetKeywd() ) )
            return ii;
    return -1;
}

// default configuration
void DataBaseList::Init()
{
    // RT_SDATA default
    unsigned char sdref_rkfrm[3] = { 20, 5, 7 };
    Add( new TDataBase( GetCount(), "sdref", false, true,
                        o_sdauthr, 9, 3, 3, sdref_rkfrm ) );
    // RT_CONST default
    unsigned char const_rkfrm[2] = { 8, 24 };
    Add( new TDataBase( GetCount(), "const", false, true,
                        o_constlab, 3, 1, 2, const_rkfrm ) );
}

// configuration to file
void DataBaseList::toCFG(fstream& out_stream)
{
    int nR = GetCount();
out_stream << nR << endl;

//    f.write( (char*)&nR, sizeof(int) );
    for(uint ii=0; ii<GetCount(); ii++)
        elem(ii).toCFG( out_stream );
}

// configuration from file
void DataBaseList::fromCFG(fstream& in_stream)
{
    int nDB;

//    f.read( (char*)&nR, sizeof(int) );
in_stream >> nDB;
    for( int ii=0; ii<nDB; ii++)
        Add( new TDataBase( in_stream ) );
}

TDataBase&
DataBaseList::operator[](uint ii) const
{
    ErrorIf( ii > GetCount(),
             "DataBaseList","Illegal chain index.");
    return TIArray<TDataBase>::elem(ii);
}

DataBaseList::~DataBaseList()
{
 //   for( uint ii=0; ii<GetCount(); ii++ )
 //       delete elem(ii);
}

DataBaseList rt;

//--------------------- End of v_dbm2.cpp ---------------------------

