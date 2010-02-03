//-------------------------------------------------------------------
// $Id: ProcessWizard.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of ProcessWizard class
//
// Copyright (C) 2005-2007  S.Dmytriyeva
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

#ifndef ProcessWizard_included
#define ProcessWizard_included

#include <QDialog>
#include "ui_ProcessWizard4.h"
#include "EquatSetupWidget.h"

class ProcessWizard : public QDialog, public Ui::ProcessWizardData
{
    Q_OBJECT

    gstring calcScript;
    gstring outScript;
    EquatSetup *pageScript;

    void 	resetNextButton();
    void 	resetBackButton();

public:

    ProcessWizard( const char* pkey, char flgs[24], int sizes[6],
       const char *acalcScript, const char *aoutScript,  QWidget* parent = NULL);
    virtual ~ProcessWizard();


    void   getSizes( int size[6] );
    void   getFlags( char flgs[24] );
    gstring getOutScript() const
    { return pageScript->getScript(); }
    TCStringArray getNames() const
    { return pageScript->getNames(); }

protected slots:
    virtual void languageChange();

protected slots:

    void help();
    void CmNext();
    void CmBack();
    void resetPageList();

};

#endif // ProcessWizard_included
