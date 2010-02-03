//-------------------------------------------------------------------
// $Id: GEM2MTWizard.h 487 2005-03-04 15:44:28Z sveta $
//
// Declaration of UnSpaceWizard class
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
                   QWidget* parent = NULL);
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
