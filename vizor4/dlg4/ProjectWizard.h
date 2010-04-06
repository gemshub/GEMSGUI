//-------------------------------------------------------------------
// $Id: RTparmWizard.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of ProjectWizard class
//
// Copyright (C) 2009  S.Dmytriyeva, D.Kulik
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

#ifndef ProjectWizard_included
#define ProjectWizard_included

#include <QDialog>
#include <QComboBox>
#include "ui_ProjectWizard4.h"

class ProjectWizard : public QDialog, public Ui::ProjectWizardData
{
    Q_OBJECT

    void 	resetNextButton();
    void 	resetBackButton();

public:
	// flgs 0-9 MSpmv, 10-29 TPptv, 30-33 TPun, 34-37  TPsv
	ProjectWizard( const char* pkey, char flgs[38],  QWidget* parent = NULL);
    virtual ~ProjectWizard();

    int get_Settings()
        { return pTaskType->currentIndex(); }
    void   getFlags( char flgs[38] );

protected slots:
    virtual void languageChange();

protected slots:

    void help();
    void CmNext();
    void CmBack();
    
};

#endif // ProjectWizard_included
