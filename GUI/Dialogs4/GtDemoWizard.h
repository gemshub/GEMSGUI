//-------------------------------------------------------------------
// $Id: GtDemoWizard.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of GtDemoWizard class
//
// Copyright (C) 2005-2008  S.Dmytriyeva
//
// This file is part of the GEM-Selektor GUI library which uses the
// Qt v.4 cross-platform App & UI framework (http://qt.nokia.com)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the GPL v.3 license

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

    GtDemoWizard( const char* pkey, int sizes[8], const char *ascript,
                  const char *proc_key, const char* aXname, const char* aYname,
                  QWidget* parent = nullptr);
    virtual ~GtDemoWizard();


    void   getSizes( int size[8] );
    gstring getPrKey();
    gstring getScript() const
    { return pageScript->getScript(); }
    TCStringArray getNames( gstring& xName, gstring& yName ) const
    { return pageScript->getNames(xName, yName); }

protected slots:
    virtual void languageChange();

public slots:
    void resetPageList( int, const char* aXname=nullptr, const char* aYname=nullptr );
    void help();
    void CmNext();
    void CmBack();
    void CmChangePage2(int);
    
};

#endif // GtDemoWizard_included
