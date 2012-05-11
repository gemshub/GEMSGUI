//-------------------------------------------------------------------
// $Id$
//
// Declaration of ListFilesDialog class
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
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#ifndef ListFilesDialog_included
#define ListFilesDialog_included

#include <qradiobutton.h>
#include "ListFilesDialogData.h"
#include "gstring.h"

class ListFilesDialog : public ListFilesDialogData
{
    Q_OBJECT

    QListViewItem* pprf;
    QListViewItem* pkern;


protected slots:
    void CmHelp();  // 05.01.01

public:

    ListFilesDialog(QWidget* win, const char * prfName,
         const char* caption = 0 );
    virtual ~ListFilesDialog();

    void allSelected( TCStringArray& aFls, TCIntArray& aCnt);

};

#endif // ListFilesDialog_included


