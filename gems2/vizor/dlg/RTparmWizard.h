//-------------------------------------------------------------------
// $$
//
// Declaration of RTparmWizard class
//
// Copyright (C) 2005  S.Dmytriyeva, D.Kulik
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

#ifndef RTparmWizard_included
#define RTparmWizard_included


#include "RTparmWizardData.h"


class RTparmWizard : public RTparmWizardData
{
    Q_OBJECT

public:

    RTparmWizard( const char* pkey, char flgs[10], int sizes[7],
                  QWidget* parent = NULL);
    virtual ~RTparmWizard();


    void   getSizes( int size[7] );
    void   getFlags( char flgs[10] );

protected slots:

    void help();

};

#endif // RTparmWizard_included
