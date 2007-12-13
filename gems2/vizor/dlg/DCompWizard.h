//-------------------------------------------------------------------
// $Id$
//
// Declaration of DCompWizard class
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

#ifndef DCompWizard_included
#define DCompWizard_included


#include "DCompWizardData.h"

//#include <qwizard.h>

class DCompWizard : public DCompWizardData
{
    Q_OBJECT

public:

    DCompWizard( const char* pkey, char flgs[15], int size[4],
                  QWidget* parent = NULL);
    virtual ~DCompWizard();


    void   getSizes( int size[4] );
    void   getFlags( char flgs[15] );

protected slots:

    void help();
//    void accept();

};

#endif // DCompWizard_included


