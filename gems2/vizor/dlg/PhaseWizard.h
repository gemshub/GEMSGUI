//-------------------------------------------------------------------
// $$
//
// Declaration of PhaseWizard class
//
// Copyright (C) 2005  S.Dmytriyeva, D.Kulik
// Uses  gstring class (C) A.Rysin 1999
//
// This file is part of the GEM-Vizor library which uses the
// Qt v.2.x GUI Toolkit (Troll Tech AS, http://www.trolltech.com)
// according to the Qt Duo Commercial license
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL)
//
// See http://les.web.psi.ch/Software/GEMS-PSI/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#ifndef PhaseWizard_included
#define PhaseWizard_included


#include "PhaseWizardData.h"

//#include <qwizard.h>

class PhaseWizard : public PhaseWizardData
{
    Q_OBJECT

public:

    PhaseWizard( const char* pkey, char flgs[12], int size[6],
                  double r2, QWidget* parent = NULL);
    virtual ~PhaseWizard();


    void   getSizes( int size[6] );
    void   getFlags( char flgs[12] );
    double   getR2();

protected slots:

    void help();
//    void accept();

};

#endif // PhaseWizard_included


