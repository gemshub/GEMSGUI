//-------------------------------------------------------------------
// $Id: RTparmWizard.h 487 2005-03-04 15:44:28Z sveta $
//
// Declaration of DualThWizard class
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

#ifndef DualThWizard_included
#define DualThWizard_included

#include <QDialog>

namespace Ui {
class DualThWizardData;
}

class DualThWizard : public QDialog
{
    Q_OBJECT

    Ui::DualThWizardData *ui;
    void 	resetNextButton();
    void 	resetBackButton();

public:

    DualThWizard( const char* pkey, char flgs[20], int sizes[8],
    QWidget* parent = nullptr);
    virtual ~DualThWizard();


    void   getSizes( int size[8] );
    void   getFlags( char flgs[20] );
    //    double   getR2();

protected slots:

    void help();
    void CmNext();
    void CmBack();
    
};


#endif // DualThWizard_included
