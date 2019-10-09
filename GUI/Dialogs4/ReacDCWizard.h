//-------------------------------------------------------------------
// $Id: ReacDCWizard.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of ReacDCWizard class
//
// Copyright (C) 2005-2008  S.Dmytriyeva, D.Kulik
// Uses  gstring class (C) A.Rysin 1999
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

#ifndef ReacDCWizard_included
#define ReacDCWizard_included


#include <QDialog>
#include "ui_ReacDCWizard4.h"

class ReacDCWizard : public QDialog, public Ui::ReacDCWizardData
{
    Q_OBJECT

    void 	resetNextButton();
    void 	resetBackButton();

public:

    ReacDCWizard( const char* pkey, char flgs[12], int size[4],
                  QWidget* parent = nullptr);
    virtual ~ReacDCWizard();


    void   getSizes( int size[4] );
    void   getFlags( char flgs[12] );
//    double   getR2();

protected slots:
    virtual void languageChange();

protected slots:

    void help();
    void CmNext();
    void CmBack();
    
};


#endif // ReacDCWizard_included


