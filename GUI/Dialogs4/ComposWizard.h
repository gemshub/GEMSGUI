//-------------------------------------------------------------------
// $ Id:$
//
// Declaration of ComposWizard class
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

#ifndef ComposWizard_included
#define ComposWizard_included


#include <QDialog>

namespace Ui {
class ComposWizardData;
}

class ComposWizard : public QDialog
{
    Q_OBJECT

    Ui::ComposWizardData *ui;
    void 	resetNextButton();
    void 	resetBackButton();

public:

    ComposWizard( const char* pkey, char flgs[6], int sizes[2],
    double r2, QWidget* parent = nullptr);
    virtual ~ComposWizard();


    void   getSizes( int sizs[2] );
    void   getFlags( char flgs[6] );
    double   getR2();

protected slots:

    void help();
    void CmNext();
    void CmBack();
    
};


#endif // ComposWizard_included
