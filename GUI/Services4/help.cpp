//-------------------------------------------------------------------
// $Id: help.cpp 1720 2007-12-13 13:23:32Z gems $
//
// Implementation of HelpConfigurator class
//
// Copyright (C) 2010 S.Dmytriyeva
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
#include <fstream>
#include <iostream>
using namespace std;
#include "help.h"
#include "visor.h"
#include "v_object.h"


// read string untill "end"
void HelpConfigurator::u_getline(istream& is, QString& str, QString end )
{
    char ch;
    is.get(ch);
    while( !is.eof() && is.good()  )
    {
        str += ch;
        if( str.indexOf(end) > 0)
        {
          break;
        }
        is.get(ch);
    }
}

// read all reference from file
void HelpConfigurator::getHrefs( QString file, QString file_name)
{
   char ch;
   fstream f_in( file.toStdString(), ios::in );
   QString ref;

   if( !f_in.good() )
   {
        file += " Fileopen error";
        Error( "HelpConfigurator", file.toStdString());
        return;
   }

   while( !f_in.eof() )
   {
       f_in.get(ch);
       if(  ch == '<' )
       {
           f_in.get(ch);
           if( ch == 'a' )  // <a name="keywd"></a>
           {
               ref = "<a";
               u_getline(f_in, ref, "</a>" );
               addNameToList(ref, file_name);
               continue;
           }
           if( ch == 'i')  // <img src="file" ... >
           {
               ref = "<i";
               u_getline(f_in, ref, ">" );
               addImgToList(ref );
               continue;
           }
       }
   }
}

// read all reference from file
void HelpConfigurator::addNameToList( QString ref, QString file_name )
{
    //QRegExp reg("(\\w+(?::\\w+)?)=(\"[^\"]+\"|'[^']+')");
    QString key;
    QString value;

    int indx = ref.indexOf("name=");
// cout << indx << "    " << ref.toStdString() << endl;
    if(indx > -1 )
    {
      names.append(ref);
      key =ref.section("\"",1,1);
      value = file_name +"#"+key;
      links.insertMulti(key,QUrl(value));
// cout << value.toStdString()<<endl;
    }
    else
        hrefs.append(ref);

}

// read all images from file
void HelpConfigurator::addImgToList( QString ref )
{
    QString value, rref, file_name;
    int indx = ref.indexOf("src=");
// cout << indx << "    " << ref.toStdString(); // << endl;
    if(indx > -1 )   // Bugfixes by DK on 15.02.2012
    {
      rref = ref.mid(indx);
      value = rref.section("\"",1,1);
// cout << "+ " << value.toStdString() << endl;
      file_name = value.section("/", -1);
// cout << "- " << file_name.toStdString() << endl;
      images.append(file_name);
    }
    else
        others.append(ref);
}

// read and parse all *.html files from dir
int HelpConfigurator::readDir(const char *dir)
{
    path = dir;
    QDir thisDir(dir);
    if (!thisDir.isReadable())
    {
        Error( "HelpConfigurator", "GEMS DB directory is not readable");
        return 0;
    }

    thisDir .setFilter(QDir::Files);
    QStringList filters;
    filters << "*.html";
    thisDir.setNameFilters(filters);

    QFileInfoList files = thisDir.entryInfoList();
    if (files.empty())
        return 0;

    QListIterator<QFileInfo> it(files);
    QFileInfo f;
    while ( it.hasNext() )
    {
        f = it.next();
        if (f.isFile() )
        {
            getHrefs( f.filePath(), f.fileName() );
            addFileToList( f.fileName());
        }
    }
    return 1;
}

// Build *.qhp file for help system (gems3helpconfig.qhp by default)
int HelpConfigurator::writeFile(const char *file)
{
    fstream f_out( file, ios::out );
    if( !f_out.good() )
    {
        QString str = QString(file) + " Fileopen error";
        Error( "HelpConfigurator", str.toStdString());
        return 0;
    }

    // Write head
    f_out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
    f_out << "<QtHelpProject version=\"1.0\">" << endl;
    f_out << "    <namespace>gems3</namespace>" << endl;
    f_out << "    <virtualFolder>help</virtualFolder>" << endl;
    f_out << "    <filterSection>" << endl;

    writeContent( f_out);
    writeFiles( f_out );
    writeKeywords( f_out );

    // Write end
    f_out << "    </filterSection>" << endl;
    f_out << "   </QtHelpProject>" << endl;

    return 1;
}


void HelpConfigurator::writeFiles( fstream& f_out)
{
    f_out << "      <files>" << endl;
    for( int ii =0; ii<files.count(); ii++)
    {
      f_out << "        <file>" << files[ii].toStdString() << "</file>" << endl;
    }
    images.sort();
    images.removeDuplicates();
    for( int ii =0; ii<images.count(); ii++)
    {
      f_out << "        <file>" << images[ii].toStdString() << "</file>" << endl;
    }
    f_out << "      </files>" << endl;
}


void HelpConfigurator::writeKeywords( fstream& f_out)
{
    QStringList kwds;
    QList<QUrl> urls;

    //kwds = links.keys();
    //for(int ii=0; ii<links.count(); ii++ )
    // cout << kwds[ii].toStdString() << endl;

    kwds = links.uniqueKeys();
    f_out << "    <keywords>" << endl;
    for( int ii =0; ii<kwds.count(); ii++)
    {

        // add only keywords for objects
        if( showObjectForKeyword(kwds[ii])<0)
          continue;
        urls = links.values( kwds[ii] );

      for( int jj=0; jj<urls.count(); jj++ )
      f_out << "        <keyword name=\"" << kwds[ii].toStdString()
            << "\" ref=\"" << urls[jj].toString().toStdString()  << "\"/>" << endl;
    }
    f_out << "    </keywords>" << endl;
}

void HelpConfigurator::writeContent( fstream& f_out)
{
    QString ref;
    QString contentfile = path;
            contentfile += "/gems3helpconfig.toc";
    fstream f_in( contentfile.toStdString(), ios::in );
    if( !f_in.good() )
    {
        contentfile += " Fileopen error";
        Error( "HelpConfigurator", contentfile.toStdString());
        return;
    }

    while( !f_in.eof() )
    {
      ref = "";
      u_getline(f_in, ref, ">" );
      f_out << ref.toStdString();
    }
}

// Searches for reference <keyword> in the list of objects
//    <keyword> should be in format: objectlabel or objectlabel_<index>
int HelpConfigurator::showObjectForKeyword(const QString &keyword)
{
    int nO = -1;

    // finding full name, not subString
    nO = aObj.Find(keyword.toLatin1().data());
    if( nO < 0 )
    {
       QString kwInternal = keyword;
       int ndx = kwInternal.lastIndexOf('_');  // finding only keyword
       if(ndx > -1)
       {    kwInternal= kwInternal.remove(QRegExp("_[0-9]{1,3}")/*ndx*/);
            nO = aObj.Find(kwInternal.toLatin1().data());
       }
    }

    return nO;
}


/*The simplest case: data exchange using disk files only
int main( int argc, char* argv[] )
 {
   // Analyzing command line arguments
   // Default arguments
   char input_dir[256] = "./html";
   char result_file[256] = "./html/gems3helpconfig.qhp";
   if (argc >= 2 )
       strncpy( input_dir, argv[1], 256);
   if (argc >= 3 )
       strncpy( result_file, argv[2], 256);

   HelpConfiguration rr;
    if( rr.readDir(input_dir))
      rr.writeFile(result_file);

    return 0;
 }
 */


/* old help system
#include <QtCore/QByteArray>
#include <QtCore/QDir>
#include <QtCore/QLibraryInfo>
#include <QtCore/QProcess>
#include <QtGui/QMessageBox>

Helper::Helper():
	docPath(""),proc(0)
{}

Helper::~Helper()
{
  if (proc && proc->state() == QProcess::Running) {
        proc->terminate();
        proc->waitForFinished(3000);
    }
    delete proc;
}

bool Helper::startAssistant()
{
    if (!proc)
        proc = new QProcess();

    if (proc->state() != QProcess::Running) {
    	
        docPath = pVisor->docDir();
        // let's try to find resources by path of the executable
 //   QString app = "/home/gems/qt-4.5.2/bin/";//QLibraryInfo::location(QLibraryInfo::BinariesPath) + QDir::separator();
       QString app;
 //    QString app = QLibraryInfo::location(QLibraryInfo::BinariesPath) + QDir::separator();
#ifdef __unix
#ifdef __APPLE__
        app += QLatin1String("/Applications/Gems3.app/Contents/MacOS/Assistant");    // expected to work
#else
#ifdef GEMS_RELEASE
//  #if !defined(Q_OS_MAC)
        char cur_dir[512];
        getcwd( cur_dir, 512 );
        app += QLatin1String(cur_dir);
        app += QDir::separator();
        app += QLatin1String("assistant.sh");
#else
        app += QLatin1String("assistant");
#endif
#endif  // unix
#else    // windows
// #if defined(Q_OS_WIN)
        app += QLatin1String("assistant.exe");
//        app += QLatin1String(".exe");
#endif        
        QStringList args;
        args << QLatin1String("-enableRemoteControl")
            << QLatin1String("-collectionFile")
            << docPath.c_str() //QLibraryInfo::location(QLibraryInfo::ExamplesPath)
            + QLatin1String("gemshelp.qhc");
            ;

        proc->start(app, args);
        cout << app.toStdString() << endl;
        cout << args[2].toStdString() << endl;

        if (!proc->waitForStarted()) 
        {
            QMessageBox::critical(0, QObject::tr("Gems3"),
                QObject::tr("Unable to launch Qt Assistant (%1)").arg(app));
            return false;
        }    
    }
    return true;   
}


void Helper::showDocumentation(const char* file, const char* item1)
{
    if (!startAssistant())
        return;

    string path = "";//pVisor->docDir();
    if( item1 )
    {  
        string item = item1;
        checkForRef( file, item );
        if( item.empty() )
            Error("Help", item + ": No such item in HTML files!");
        path += item;
    }
    else
    {
        path += file;
        // adding ".html" if needed
        if( path.rfind( "#" ) == path.npos )
           if( string(path, path.length()-5, 5) != ".html" )
              path += ".html"; 
    }
    QString path_str = path.c_str();


    QByteArray ba("SetSource qthelp://gems3/doc/");
    ba.append(path.c_str());
//    ba.append("; syncContents");
    ba.append('\0');

    cout << path.c_str() << " " << proc->state() << endl;

   proc->write(ba);
}

// searches for reference <ref> in the index <file>
//    returns true if exact match is found
//    returns false if not found and sets <ref> to HTML file
//    name where full description is to be browsed
//    ( null - if no match found )
//    <ref> should be in format:
//    "objectlabel[indexN][indexM]", if sizeN >1 and sizeM > 1
//    "objectlabel[][indexM]", if sizeN > 1 and sizeM > 1
//    "objectlabel[index]", if sizeN >1 or sizeM > 1
//    "objectlabel", if sizeN == 1 and sizeM == 1
//
//    Fixed by DS 17/02/2005
bool
Helper::checkForRef( const string& file, string& ref)
{
    string fname = docPath;
    fname += file;
//    string fname = file;

    ifstream is(fname.c_str());
    if( !is.good() )
        throw FileError();

    string line;
    string ref1 = ref;

    size_t pos_name;
    size_t  pos_1 = ref1.find('[');
    size_t  pos_2 = ref1.rfind("[");
    size_t  pos_12 = ref1.find(']');
    size_t  pos_22 = ref1.rfind("]");
    if( pos_2 == pos_1 )
           pos_2 = pos_22 = string::npos;

    string o_name = ">";
            o_name += ref1.substr(0, pos_1 );
    string o_name2 = "";
    if( pos_2 != string::npos  )
    {
        o_name2 = o_name;
        o_name2 += "[]";
        o_name2 += ref1.substr( pos_2, pos_22-pos_2 );
    }
    string name = o_name;
    ref = "";

    /// should be case insensitive  !!!
    while(1)
    {
        u_getline(is, line, '\n');
        if( is.eof() )
            break;
        if( !is.good() )
            throw FileError();

        size_t posf = line.find("a href=\"");  // 08.01.01
        if( posf == string::npos )
               continue;
        posf += 8;

        if( !o_name2.empty() )
          if( line.find( o_name2 ) != string::npos )
          {  // include line such: ">keywd[][m]"
            ref = line.substr( posf );
            ref = ref.substr( 0, ref.find('\"') );
            return true;
          }
ID:
        pos_name = line.find( o_name );
        if( pos_name != string::npos )
        {

           size_t poss = o_name.length();
                  poss+= pos_name;
           if( line[poss] !=  '<'&&
               line[poss] !=  '[' )
             continue; // finding full name, not subtring
           ref = line.substr( posf );
           ref = ref.substr( 0, ref.find('\"') );
           if( pos_1 == string::npos )
            { // all reference finding
               return true;
            }
            else
            { // add index
              o_name += ref1.substr( pos_1, pos_12-pos_1+1);
              pos_1 = pos_2;
              pos_12 = pos_22;
              pos_2 = pos_22 = string::npos;
              goto ID;
            }
         }
        else
           if( !ref.empty() )
           {
              pos_name = line.find( name );
              if( pos_name == string::npos ) // next object, return nearest
                   return true;
           }

      }
      return false;
}

*/
// ----------------- End of help.cpp -----------------------

