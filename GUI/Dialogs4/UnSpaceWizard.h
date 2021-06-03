//-------------------------------------------------------------------
// $Id: GEM2MTWizard.h 487 2005-03-04 15:44:28Z sveta $
//
// Declaration of UnSpaceWizard class
//
// Copyright (C) 2005-2008  S.Dmytriyeva
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

#ifndef UnSpaceWizard_included
#define UnSpaceWizard_included

#include <QDialog>

namespace Ui {
class UnSpaceWizardData;
}

class UnSpaceWizard : public QDialog
{
    Q_OBJECT

    Ui::UnSpaceWizardData *ui;
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

    void help();
    void CmNext();
    void CmBack();
    
};

#endif // UnSpaceWizard_included
