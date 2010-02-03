//-------------------------------------------------------------------
// $Id: KeyProfile.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of KeyProfile class
//
// Copyright (C) 1996-2008  A.Rysin, S.Dmytriyeva
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

#ifndef KeyProfile_included
#define KeyProfile_included

#include <QDialog>

#include "ui_KeyProfile4.h"
#include "gstring.h"

class KeyProfile : public QDialog, public Ui::KeyProfileData
{
    Q_OBJECT

    int iRt;
    bool newKey;

protected slots:
    virtual void CmNew();
    virtual void CmHelp();
    void CmReturnIA();
    void CmReturnSmart();
    virtual void languageChange();

public:

    KeyProfile(QWidget* win, int irt, const char* caption = 0);
    virtual ~KeyProfile();

    gstring getKey();
    bool   getAqGasState();
    bool   getFilesState();
    bool   getRemakeState();
    int   getMakeDump();
    gstring getTemplateKey();

};


#endif // KeyProfile_included


