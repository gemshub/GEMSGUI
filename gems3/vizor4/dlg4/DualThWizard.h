//-------------------------------------------------------------------
// $Id: RTparmWizard.h 487 2005-03-04 15:44:28Z sveta $
//
// Declaration of DualThWizard class
//
// Copyright (C) 2005-2008  S.Dmytriyeva, D.Kulik
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

#ifndef DualThWizard_included
#define DualThWizard_included


#include <QDialog>
#include "ui_DualThWizard4.h"

class DualThWizard : public QDialog, public Ui::DualThWizardData
{
    Q_OBJECT

    void 	resetNextButton();
    void 	resetBackButton();

public:

    DualThWizard( const char* pkey, char flgs[20], int sizes[8],
                  QWidget* parent = NULL);
    virtual ~DualThWizard();


    void   getSizes( int size[8] );
    void   getFlags( char flgs[20] );
//    double   getR2();

protected slots:
    virtual void languageChange();

protected slots:

    void help();
    void CmNext();
    void CmBack();
    
};


#endif // DualThWizard_included