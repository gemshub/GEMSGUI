//-------------------------------------------------------------------
// $Id: help.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of Helper class
//
// Copyright (C) 1996-2001  A.Rysin, S.Dmytriyeva
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
#include <fstream>
#include <iostream>
using namespace std;
#include "help.h"
#include "visor.h"

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
#if !defined(Q_OS_MAC)
       char cur_dir[512];
        getcwd( cur_dir, 512 );
        app += QLatin1String(cur_dir);
        app += QDir::separator();
        app += QLatin1String("assistant.sh");
#else
        app += QLatin1String("/Applications/Gems3.app/Contents/MacOS/Assistant");    // expected to work
#endif

#if defined(Q_OS_WIN)
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
    	cout << app.toLatin1().data() << endl;
    	cout << args[2].toLatin1().data() << endl;

        if (!proc->waitForStarted()) 
        {
            QMessageBox::critical(0, QObject::tr("Gems3"),
                QObject::tr("Unable to launch Qt Assistant (%1)").arg(app));
            return false;
        }    
    }
    return true;   
}

//void Helper::showError(const QString & message )
//{
//	cout <<"Help error" << messate.toLatin1().data() << endl;
//}

void Helper::showDocumentation(const char* file, const char* item1)
{
    if (!startAssistant())
        return;

    gstring path = "";//pVisor->docDir();
    if( item1 )
    {  
        gstring item = item1;
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
           if( gstring(path, path.length()-5, 5) != ".html" )
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
Helper::checkForRef( const gstring& file, gstring& ref)
{
    gstring fname = docPath;
    fname += file;
//    gstring fname = file;

    ifstream is(fname.c_str());
    if( !is.good() )
        throw FileError();

    gstring line;
    gstring ref1 = ref;

    size_t pos_name;
    size_t  pos_1 = ref1.find('[');
    size_t  pos_2 = ref1.rfind("[");
    size_t  pos_12 = ref1.find(']');
    size_t  pos_22 = ref1.rfind("]");
    if( pos_2 == pos_1 )
           pos_2 = pos_22 = gstring::npos;

    gstring o_name = ">";
            o_name += ref1.substr(0, pos_1 );
    gstring o_name2 = "";
    if( pos_2 != gstring::npos  )
    {
        o_name2 = o_name;
        o_name2 += "[]";
        o_name2 += ref1.substr( pos_2, pos_22-pos_2 );
    }
    gstring name = o_name;
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
        if( posf == gstring::npos )
               continue;
        posf += 8;

        if( !o_name2.empty() )
          if( line.find( o_name2 ) != gstring::npos )
          {  // include line such: ">keywd[][m]"
            ref = line.substr( posf );
            ref = ref.substr( 0, ref.find('\"') );
            return true;
          }
ID:
        pos_name = line.find( o_name );
        if( pos_name != gstring::npos )
        {

           size_t poss = o_name.length();
                  poss+= pos_name;
           if( line[poss] !=  '<'&&
               line[poss] !=  '[' )
             continue; // finding full name, not subtring
           ref = line.substr( posf );
           ref = ref.substr( 0, ref.find('\"') );
           if( pos_1 == gstring::npos )
            { // all reference finding
               return true;
            }
            else
            { // add index
              o_name += ref1.substr( pos_1, pos_12-pos_1+1);
              pos_1 = pos_2;
              pos_12 = pos_22;
              pos_2 = pos_22 = gstring::npos;
              goto ID;
            }
         }
        else
           if( !ref.empty() )
           {
              pos_name = line.find( name );
              if( pos_name == gstring::npos ) // next object, return nearest
                   return true;
           }

      }
      return false;
}


// ----------------- End of help.cpp -----------------------

