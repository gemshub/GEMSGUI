//-------------------------------------------------------------------
// $Id: GEM2MTWizard.h 487 2005-03-04 15:44:28Z sveta $
//
// Declaration of GEM2MTWizard class
//
// Copyright (C) 2005-2007  S.Dmytriyeva
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

#ifndef GEM2MTWizard_included
#define GEM2MTWizard_included

#include "ui_GEM2MTWizard4.h"

class GEM2MTWizard : public QDialog, public Ui::GEM2MTWizardData
{
    Q_OBJECT

    void 	resetNextButton();
    void 	resetBackButton();

public:

	GEM2MTWizard( const char* pkey, char flgs[22], int sizes[17],
	                   QWidget* parent = NULL);
	virtual ~GEM2MTWizard();

    void   getSizes( int size[17] );
    void   getFlags( char flgs[22] );
//    double   getR2();

protected slots:
    virtual void languageChange();

protected slots:

    void help();
    void CmNext();
    void CmBack();
    
};

#endif // GEM2MTWizard_included
