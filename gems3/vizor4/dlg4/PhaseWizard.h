//-------------------------------------------------------------------
// $Id: PhaseWizard.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of PhaseWizard class
//
// Copyright (C) 2005-2008  S.Dmytriyeva, D.Kulik
// Uses  gstring class (C) A.Rysin 1999
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

#ifndef PhaseWizard_included
#define PhaseWizard_included


#include <QDialog>
#include "ui_PhaseWizard4.h"

class PhaseWizard : public QDialog, public Ui::PhaseWizardData
{
    Q_OBJECT

    void 	resetNextButton();
    void 	resetBackButton();

public:

    PhaseWizard( const char* pkey, char flgs[12], int size[6],
                  double r2, QWidget* parent = NULL);
    virtual ~PhaseWizard();


    void   getSizes( int size[6] );
    void   getFlags( char flgs[12] );
    double   getR2();

protected slots:
    virtual void languageChange();

protected slots:

    void help();
    void CmNext();
    void CmBack();
    
};

#endif // PhaseWizard_included


