//-------------------------------------------------------------------
// $$
//
// Declaration of GtDemoWizard class
//
// Copyright (C) 2005  S.Dmytriyeva
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

#ifndef GtDemoWizard_included
#define GtDemoWizard_included

#include "GtDemoWizardData.h"

class GtDemoWizard : public GtDemoWizardData
{
    Q_OBJECT

public:

    GtDemoWizard( const char* pkey, int sizes[7],
                   QWidget* parent = NULL);
    virtual ~GtDemoWizard();


    void   getSizes( int size[7] );
//    void   getFlags( char flgs[24] );
//    double   getR2();

protected slots:

    void help();
//    void accept();

};
#endif // GtDemoWizard_included
