//-------------------------------------------------------------------
// $$
//
// Declaration of ProcessWizard class
//
// Copyright (C) 1996-2001  A.Rysin, S.Dmytriyeva
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

#ifndef ProcessWizard_included
#define ProcessWizard_included

#include "gstring.h"

#include "ProcessWizardData.h"

class ProcessWizard : public ProcessWizardData
{
    Q_OBJECT

public:

    ProcessWizard( const char* pkey, char flgs[24], int sizes[6],
                   QWidget* parent = NULL);
    virtual ~ProcessWizard();


    void   getSizes( int size[6] );
    void   getFlags( char flgs[24] );
//    double   getR2();

protected slots:

    void help();
//    void accept();

};
#endif // ProcessWizard_included
