//-------------------------------------------------------------------
// $Id: GtDemoWizard.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of GtDemoWizard class
//
// Copyright (C) 2005-2008  S.Dmytriyeva
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

#ifndef GtDemoWizard_included
#define GtDemoWizard_included

#include <QDialog>
#include "ui_GtDemoWizard4.h"
#include "EquatSetupWidget.h"
#include "gstring.h"


class GtDemoWizard : public QDialog, public Ui::GtDemoWizardData
{
    Q_OBJECT

    int nRT;
    gstring script;
    EquatSetup *pageScript;
    QButtonGroup *allButtons;

    void  resetNextButton();
    void  resetBackButton();
    int   getnRT();

public:

    GtDemoWizard( const char* pkey, int sizes[7], const char *ascript,
                  const char *proc_key, QWidget* parent = NULL);
    virtual ~GtDemoWizard();


    void   getSizes( int size[7] );
    gstring getPrKey();
    gstring getScript() const
    { return pageScript->getScript(); }
    TCStringArray getNames() const
    { return pageScript->getNames(); }

protected slots:
    virtual void languageChange();

public slots:
    void resetPageList( int );
    void help();
    void CmNext();
    void CmBack();
    void CmChangePage2(int);
    
};

#endif // GtDemoWizard_included
