//-------------------------------------------------------------------
// $Id: RTparmWizard.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of RTparmWizard class
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

#ifndef RTparmWizard_included
#define RTparmWizard_included

#include <QDialog>
#include "ui_RTparmWizard4.h"

class RTparmWizard : public QDialog, public Ui::RTparmWizardData
{
    Q_OBJECT

    void 	resetNextButton();
    void 	resetBackButton();

public:

    RTparmWizard( const char* pkey, char flgs[10], int sizes[7],
                  QWidget* parent = NULL);
    virtual ~RTparmWizard();


    void   getSizes( int size[7] );
    void   getFlags( char flgs[10] );
//    double   getR2();

protected slots:
    virtual void languageChange();

protected slots:

    void help();
    void CmNext();
    void CmBack();
    
};


#endif // RTparmWizard_included
