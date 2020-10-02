//-------------------------------------------------------------------
// $Id: DCompWizard.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of DCompWizard class
//
// Copyright (C) 2005-2008  S.Dmytriyeva, D.Kulik
//
// This file is part of the GEM-Selektor GUI library which uses the
// Qt v.4 cross-platform App & UI framework (https://qt.io/download-open-source)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the GPL v.3 license

//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#ifndef DCompWizard_included
#define DCompWizard_included


#include <QDialog>
#include "ui_DCompWizard4.h"

class DCompWizard : public QDialog, public Ui::DCompWizardData
{
    Q_OBJECT

    void 	resetNextButton();
    void 	resetBackButton();

public:

    DCompWizard( const char* pkey, char flgs[15], int size[4],
                  QWidget* parent = nullptr);
    virtual ~DCompWizard();


    void   getSizes( int size[4] );
    void   getFlags( char flgs[15] );
//    double   getR2();

protected slots:
    virtual void languageChange();

protected slots:

    void help();
    void CmNext();
    void CmBack();
    
};


#endif // DCompWizard_included


