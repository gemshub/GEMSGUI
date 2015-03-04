//-------------------------------------------------------------------
// $Id: PhaseWizard.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of PhaseWizard class
//
// Copyright (C) 2005-2008  S.Dmytriyeva, D.Kulik
// Uses  string class (C) A.Rysin 1999
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

#ifndef PhaseWizard_included
#define PhaseWizard_included


#include <QDialog>
#include "ui_PhaseWizard4.h"

class PhaseWizard : public QDialog, public Ui::PhaseWizardData
{
    Q_OBJECT

    int ndxInPhPs_0;
    void 	resetNextButton();
    void 	resetBackButton();

public:

    PhaseWizard( const char* pkey, char flgs[37], int size[30],
                  double r2, QWidget* parent = NULL);
    virtual ~PhaseWizard();


    void   getSizes( int size[30] );
    void   getFlags( char flgs[37] );
    double   getR2();

protected slots:
    virtual void languageChange();

protected slots:

    void help();
    void CmNext();
    void CmBack();
    
};

#endif // PhaseWizard_included


