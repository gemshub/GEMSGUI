//-------------------------------------------------------------------
// $Id: SystemWizard.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of SystemWizard class
//
// Copyright (C) 2010  S.Dmytriyeva, D.Kulik
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

#ifndef SystemWizard_included
#define SystemWizard_included

#include <QDialog>
#include <QComboBox>
#include "ui_SystemWizard4.h"
#include "gstring.h"


class SystemWizard : public QDialog, public Ui::SystemWizardData
{
    Q_OBJECT

    void 	resetNextButton();
    void 	resetBackButton();

public:

    SystemWizard( const char* pkey, char flgs[40],
         gstring name, gstring comment, gstring EQkey,  QWidget* parent = NULL);
    virtual ~SystemWizard();

    gstring getName();
    gstring getComment();
    gstring getEQkey();

    void   getFlags( char flgs[40] );

protected slots:
    virtual void languageChange();

protected slots:

    void help();
    void CmNext();
    void CmBack();
    
};

#endif // SystemWizard_included
