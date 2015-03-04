//-------------------------------------------------------------------
// $Id: help.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of HelpConfigurator class
//
// Copyright (C) 2010  S.Dmitrieva
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

#ifndef __help_h
#define __help_h

#include <QMap>
#include <QString>
#include <QStringList>
#include <QUrl>
#include <QDir>

class HelpConfigurator
{
  QString path;
  QMap<QString, QUrl> links;
  QStringList images;
  QStringList files;

  void getHrefs( QString file, QString file_name);
  void u_getline(istream& is, QString& str, QString end );
  void addNameToList( QString ref, QString file_name );
  void addImgToList(  QString ref );
  void addFileToList(  QString file )
  {
      files.append(file);
  }

  void writeFiles( fstream& f_out);
  void writeKeywords( fstream& f_out);
  void writeContent( fstream& f_out);

#ifndef IPMGEMPLUGIN
  int showObjectForKeyword(const QString &keyword);
#endif

public:
  HelpConfigurator(){}

  int readDir(const char *dir);
  int writeFile(const char *file);

protected:
// only for test
 QStringList hrefs;
 QStringList names;
 QStringList others;

};

/* old help system
class QProcess;

// This class searches for some reference in html help file
class Helper
{

    string docPath;
       
public:
   Helper();
    ~Helper();
    void showDocumentation(const char* file, const char* item1);

private:
    bool startAssistant();
     QProcess *proc;

    bool checkForRef(const string& file, string& ref);
    //! sets the path where html-files are
    void setPath(const string& p)
    {
    	docPath = p;
    }
    struct FileError
        {}
    ;
};*/

#endif  //__help_h
