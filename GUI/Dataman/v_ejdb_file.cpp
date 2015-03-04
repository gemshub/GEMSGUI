//-------------------------------------------------------------------
// $Id: v_ejdb_file.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of TEJDBFile class
//
// Copyright (C) 2013 S.Dmytriyeva
//
// This file is part of the GEM-Selektor GUI library which uses the
// Qt v.4 cross-platform App & UI framework (http://qt.nokia.com)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the terms of GEMS3 Development
// Quality Assurance Licence (GEMS3.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#include "v_ejdb_file.h"
#include "v_user.h"
#include "visor.h"

#include <unistd.h>
#include <QDir>
#include <QString>
#include <QFile>
#include <QFileInfo>


// To do for EJDB
// 1. FileStatus structure must be changed for EJDB
// JBOWRITER | JBOCREAT | JBOTRUNC
// deleted all FileStatus as parameter
// 2. Open/Close TEjdbFile as smart ptr -
//    Close only if last opened close
//    ( This can be discuss)
// 3.Make lists to Elements dilalog (tree in stl )
// SetTreeWidget(), getSelectionTreeWidget() and other
// into EJDBFileList
// Did, but not tested
// 4. deleteDBDir, makeDBDir and other move into  TDataBase class
// and write analogs into TEJDataBase
// 5?? To do read/write cfg files to json format??
// Use Andy TConfig


//-------------------------------------------------------------
// TEJDBFile, files that contain EJDB records
//-------------------------------------------------------------


/*// Default configurations
TEJDBFile::TEJDBFile(const string& fName,
                 const string& fExt, const string& fDir):
        TFile(fName, fExt, fDir),
        ejDB(0)
{
     makeKeyword();
}
*/

/// Configurations from file path
TEJDBFile::TEJDBFile(const string& path):
        TFile(path),
        defaultDB(false), numEJDB(0), ejDB(0)
{
     makeKeyword();
     if( path.find( pVisor->sysDBDir()) != string::npos )
        defaultDB = true;
}

/// Load configurations from cfg file
TEJDBFile::TEJDBFile(fstream& file):
        TFile(file),
         defaultDB(false), numEJDB(0), ejDB(0)
{
   if( Path.find( pVisor->sysDBDir()) != string::npos )
       defaultDB = true;
}


TEJDBFile::~TEJDBFile()
{
    if( Test() )
        Close();
}

/// Make keyword of DB internal file
void TEJDBFile::makeKeyword()
{
    string key;
    if( name.empty() )
        return;

    string fname = name;

    key = string(fname, 0, 2);
    size_t npos = 0;
    size_t npos2 = fname.find("_", npos);
    while( npos2 != string::npos )
    {   npos = npos2+1;
        key += string(fname, npos, 2);
        npos2 = fname.find("_", npos);
    }
    key += string(fname, npos+2);

    strncpy( Keywd, key.c_str(), MAX_FILENAME_LEN-2);
    Keywd[MAX_FILENAME_LEN-2]='\0';
}

/// Open file in special mode
void TEJDBFile::Open()
{

    if( Test() )
    {
        numEJDB++;
    }
    else
    {

      // Test and open file  (name of ejdb must be take from nFile)
      ejDB = ejdbnew();
      if (!ejdbopen(ejDB, Path.c_str(), JBOWRITER | JBOCREAT ))
      {
        ejdbdel(ejDB);
        cout << " ejdbopen error" << endl;
        ejDB = 0;
        isopened = false;
        return;
       }

      numEJDB++;
      isopened = true;
    }
}

/// Close EJ DataBase
void TEJDBFile::Close()
{
    numEJDB--;

    if( numEJDB <= 0 )
    { if(ejDB )
      { ejdbclose(ejDB);
        ejdbdel(ejDB);
        ejDB = 0;
      }
    isopened = false;
    }
}

/// Create PDB file
void TEJDBFile::Create( bool isDel )
{
    Open();
    Close();
    // make changelog.txt file
    string clfile = dir + "/Changelog.txt";
    fstream ff( clfile.c_str(), ios::out);
    ff << "File " << name.c_str() << " created on " << curDate().c_str() << " " << curTime().c_str() << endl;
    ff << "<Version> = v0.1" << endl;
}


/// Read version from Changelog.txt file
void TEJDBFile::readVersion( )
{
    // open changelog.txt file
    string clfile = dir + "/Changelog.txt";
    fstream ff( clfile.c_str(), ios::in);
    string fbuf;
    size_t pos;
    version = "not versioned";

    while( ff.good() )
    {
      getline ( ff ,fbuf);
      pos = fbuf.find("<Version>");
      if( pos != string::npos )
      {
         pos = fbuf.find("=", pos+1);
         if( pos != string::npos )
         {
           version = fbuf.substr(pos+1);
           strip( version);
           break;
         }
      }
    }
}

//-------------------------------------------------------------
// EJDBFileList
//-------------------------------------------------------------

int EJDBFileList::Find(const char* s)
{
    for( int ii=0; ii<size(); ii++ )
        if( at(ii).GetKeywd() == s  )
            return ii;
    return -1;
}

int EJDBFileList::FindPath( const string& path )
{
    for( int ii=0; ii<size(); ii++ )
        if( at(ii).Dir() == path  )
            return ii;
    return -1;
}

// default configuration
void EJDBFileList::Init()
{
   // scan EJDBProject and EJDBdefault directories
  readEJDBDir( pVisor->sysDBDir().c_str() );
  readEJDBDir( pVisor->userEJDBProfDir().c_str() );
  string tag = ".";
  ftree->deleteTag( tag);
}

// configuration to file
void EJDBFileList::toCFG(fstream& out_stream)
{
    int nR = size();
    out_stream << nR << endl;

    for(int ii=0; ii<size(); ii++)
        at(ii).toCFG( out_stream );
}

// configuration from file
void EJDBFileList::fromCFG(fstream& in_stream)
{
    int nDB;

    in_stream >> nDB;
    for( int ii=0; ii<nDB; ii++)
    {
        push_back( TEJDBFile( in_stream ) );  // Add( new TDataBase( in_stream ) );
        addTree();
    }
    string tag = ".";
    ftree->deleteTag( tag );
}

TEJDBFile&
EJDBFileList::operator[](int ii)
{
    ErrorIf( ii > size(),
             "EJDBFileList","Invalid index.");
    return at(ii);
}


void EJDBFileList::addTree()
{
    int fNdx = size()-1;

    // read version
    //at(fNdx).readVersion();
    back().readVersion();

    // make tree
    if(/*at(fNdx)*/back().isDefaultDB())
    {
      string fname = /*at(fNdx)*/back().Name();
      fname = "_" +fname+"_";
      ftree->insertTag( fname,  /*at(fNdx)*/back().Version(), '_', fNdx );
    }
}

// make name of EJ DataBase and configurate path
string  EJDBFileList::makeEJDBname(string& path, bool &isDefault )
{
    string name = "";
    if( path.find( pVisor->sysDBDir()) != string::npos )\
    {
       isDefault = true;
       name = path.substr( pVisor->sysDBDir().length() );
    }
    else if( path.find( pVisor->userEJDBProfDir()) != string::npos )\
         {
           isDefault = false;
           name = path.substr( pVisor->userEJDBProfDir().length() );
         }
    else // EJDB not in defaultDB and not in Projects
        return ""; //Error( path, "Undefined EJDB directory");

    name = replace_all( name, "/", "_");
    name = replace_all( name, "\\", "_");
    // make default full path
    path += "/";
    path += name;

    return name;
}

// Added new or link existing EJ DataBase
void EJDBFileList::addNew( string& path )
{
  bool defDB = false;

  // create new EJDB directory
  if( access( path.c_str(), 0 ) )
  {
    pVisor->makeDBDir(path.c_str(), false );
  }

  // make name of EJ DataBase and configurate path
  string name = makeEJDBname( path, defDB );

  push_back( TEJDBFile( path.c_str() ) );
  // test existing
  if( access( path.c_str(), 0 ) )
  {
    // create new EJDB if not exist
    /*at(size()-1)*/back().Create( true );
  }
  addTree();
  string tag = ".";
  ftree->deleteTag( tag);
}

/// Added existing EJ DataBase
bool EJDBFileList::addExist( string path )
{
    bool defDB = false;

    // make name of EJ DataBase and configurate path
    string name = makeEJDBname( path, defDB );

    // test existing
    if( access( path.c_str(), 0 ) )
      return false;

    push_back( TEJDBFile( path.c_str() ) );
    addTree();
    return true;
}

void EJDBFileList::readEJDBDir( const char *dir )
{
    QDir thisDir(dir);

    if (!thisDir.isReadable())
    {
#ifdef __unix
       cout << "Error :" << dir << endl;
#endif
       throw TFatalError(/*"GEMS Init"*/dir, "GEMS EJDB directory is not readable");
     }

   thisDir.setFilter(QDir::AllDirs);

   QFileInfoList dirs = thisDir.entryInfoList( QDir::NoFilter, QDir::Name );
   if( dirs.empty() )
       return;

   QListIterator<QFileInfo> it(dirs);
   QFileInfo f;
   while( it.hasNext() )
   {
     f = it.next();;
     if( f.isSymLink() || f.isDir() )
     {
         if( f.fileName() == "." || f.fileName() == "..")
          continue;
         string dirname = f.absoluteFilePath().toUtf8().data();
         if( addExist( dirname ))
           readEJDBDir( dirname.c_str() );
     }
   }
}


EJDBFileList flsEJ;


//--------------------- End of v_ejdb_file.cpp ---------------------------

