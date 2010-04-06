//-------------------------------------------------------------------
// $Id: help.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of Helper class
//
// Copyright (C) 1996-2001  A.Rysin
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
#ifndef __help_h
#define __help_h

#include <QtCore/QString>
#include "gstring.h"

class QProcess;


// This class searches for some reference in html help file
class Helper
{
 //   Q_OBJECT

    gstring docPath;
    
//public slots:
//	void showError(const QString & message );
    
public:
	Helper();
    ~Helper();
    void showDocumentation(const char* file, const char* item1);

private:
    bool startAssistant();
     QProcess *proc;

    bool checkForRef(const gstring& file, gstring& ref);
    /*! sets the path where html-files are */
    void setPath(const gstring& p)
    {
    	docPath = p;
    }
    struct FileError
        {}
    ;
};

#endif  //__help_h
