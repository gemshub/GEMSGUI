//-------------------------------------------------------------------
// $ Id:$
//
// Declaration of ComposWizard class
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
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#ifndef ComposWizard_included
#define ComposWizard_included


#include "ComposWizardData.h"

//#include <qwizard.h>

class ComposWizard : public ComposWizardData
{
    Q_OBJECT

public:

    ComposWizard( const char* pkey, char flgs[6], int sizes[2],
                  double r2, QWidget* parent = NULL);
    virtual ~ComposWizard();


    void   getSizes( int sizs[2] );
    void   getFlags( char flgs[6] );
    double   getR2();

protected slots:

    void help();
//    void accept();

};

#endif // ComposWizard_included
