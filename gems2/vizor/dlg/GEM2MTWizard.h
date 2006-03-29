//-------------------------------------------------------------------
// $Id: GEM2MTWizard.h 487 2005-03-04 15:44:28Z sveta $
//
// Declaration of GEM2MTWizard class
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

#ifndef GEM2MTWizard_included
#define GEM2MTWizard_included

#include "GEM2MTWizardData.h"

class GEM2MTWizard : public GEM2MTWizardData
{
    Q_OBJECT

public:

    GEM2MTWizard( const char* pkey, char flgs[22], int sizes[17],
                   QWidget* parent = NULL);
    virtual ~GEM2MTWizard();


    void   getSizes( int size[17] );
    void   getFlags( char flgs[22] );
//    double   getR2();

protected slots:

    void help();
//    void accept();

};
#endif // GEM2MTWizard_included
