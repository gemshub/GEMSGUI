//-------------------------------------------------------------------
// $Id: DCompWizard.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of DCompWizard class
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
                  QWidget* parent = NULL);
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


