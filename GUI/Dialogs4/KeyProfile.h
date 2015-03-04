//-------------------------------------------------------------------
// $Id: KeyProfile.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of KeyProfile class
//
// Copyright (C) 1996-2008  A.Rysin, S.Dmytriyeva
// Uses  string class (C) A.Rysin 1999
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

#ifndef KeyProfile_included
#define KeyProfile_included

#include <QDialog>

#include "ui_KeyProfile4.h"
#include "verror.h"

class KeyProfile : public QDialog, public Ui::KeyProfileData
{
    Q_OBJECT

    int iRt;
    bool newKey;
    bool loadOld;

protected slots:
    virtual void CmNew();
    virtual void CmHelp();
    void CmLoadAccept();
    void CmReturnIA();
    void CmReturnSmart();
    virtual void languageChange();

public:

    KeyProfile(QWidget* win, int irt, const char* caption = 0);
    virtual ~KeyProfile();

    string getKey();
    bool   getAqGasState();
    bool   getFilesState();
    bool   getRemakeState();
    bool   getLoadOldState()
           { return loadOld; }
    int   getMakeDump();
    string getTemplateKey();

};


#endif // KeyProfile_included


