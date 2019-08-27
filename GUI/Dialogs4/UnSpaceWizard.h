//-------------------------------------------------------------------
// $Id: GEM2MTWizard.h 487 2005-03-04 15:44:28Z sveta $
//
// Declaration of UnSpaceWizard class
//
// Copyright (C) 2005-2008  S.Dmytriyeva
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

#ifndef UnSpaceWizard_included
#define UnSpaceWizard_included

#include <QDialog>
#include "ui_UnSpaceWizard4.h"

class UnSpaceWizard : public QDialog, public Ui::UnSpaceWizardData
{
    Q_OBJECT

    void 	resetNextButton();
    void 	resetBackButton();

public:

    UnSpaceWizard( const char* pkey, char flgs[38], int sizes[10],
                   QWidget* parent = nullptr);
    virtual ~UnSpaceWizard();


    void   getSizes( int size[10] );
    void   getFlags( char flgs[38] );
//    double   getR2();

protected slots:
    virtual void languageChange();

protected slots:

    void help();
    void CmNext();
    void CmBack();
    
};

#endif // UnSpaceWizard_included
