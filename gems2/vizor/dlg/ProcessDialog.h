//-------------------------------------------------------------------
// Id: gems/vizor/dlg/ProcessDialog.h  version 2.0.0    2001
//
// Declaration of ProcessDialog class
//
// Created : 020312
//
// Copyright (C) 1996-2001  A.Rysin, S.Dmytriyeva
// Uses  gstring class (C) A.Rysin 1999
//
// This file is part of the GEM-Vizor library which uses the
// Qt v.2.x GUI Toolkit (Troll Tech AS, http://www.trolltech.com)
// according to the Qt Duo Commercial license #1435515
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL)
//
// See http://les.web.psi.ch/Software/GEMS-PSI/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#ifndef ProcessDialog_included
#define ProcessDialog_included

#include "gstring.h"

#include "ProcessDialogData.h"

class ProcessDialog : public ProcessDialogData
{
    Q_OBJECT

public:

    ProcessDialog(
       const char* pkey, char type, QWidget* parent = NULL);
    virtual ~ProcessDialog();


    void   getSizes( int size[6] );
    char   getType();


protected slots:
    void CmHelp();

};

#endif // ProcessDialog_included
