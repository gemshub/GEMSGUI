//-------------------------------------------------------------------
// $Id: SettingsDialog.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of LookupDialog class
//
// Copyright (C) 2009  S.Dmytriyeva
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
#ifndef LookupDialog_included
#define LookupDialog_included

#include "LookupDialogData.h"

class LookupDialog : public LookupDialogData
{
    Q_OBJECT

public:

	LookupDialog( QWidget* parent = NULL);
    virtual ~LookupDialog();

    void   getTdata( int& nT, float Tai[4] );
    void   getPdata( int& nP, float Pai[4] );
    void   getFlags( char flgs[4] );

protected slots:
    void CmHelp();


};

#endif // LookupDialog_included
