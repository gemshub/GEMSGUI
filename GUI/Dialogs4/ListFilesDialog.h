//-------------------------------------------------------------------
// $Id: ListFilesDialog.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of ListFilesDialog class
//
// Copyright (C) 1996-2001  A.Rysin, S.Dmytriyeva
// Uses  gstring class (C) A.Rysin 1999
//
// This file is part of the GEM-Selektor GUI library which uses the
// Qt v.4 cross-platform App & UI framework (http://qt.nokia.com)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the GPL v.3 license

//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#ifndef ListFilesDialog_included
#define ListFilesDialog_included

#include <QDialog>
#include <qradiobutton.h>

#include "ui_ListFilesDialog4.h"
#include "gstring.h"

class ListFilesDialog : public QDialog, public Ui::ListFilesDialogData
{
    Q_OBJECT

    QTreeWidgetItem* pprf;
    QTreeWidgetItem* pkern;


public slots:
    virtual void CmHelp();

protected slots:
    virtual void languageChange();

public:

    ListFilesDialog(QWidget* win, const char * prfName,
         const char* caption = nullptr );
    virtual ~ListFilesDialog();

    void allSelected( TCStringArray& aFls, TCIntArray& aCnt);

};

#endif // ListFilesDialog_included


