//-------------------------------------------------------------------
// $Id: SelectDialog.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of SelectDialog class
//
// Copyright (C) 1996-2007  A.Rysin, S.Dmytriyeva
//
// This file is part of the GEM-Selektor GUI library which uses the
// Qt v.4 cross-platform App & UI framework (https://qt.io/download-open-source)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the GPL v.3 license

//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#ifndef SelectDialog_included
#define SelectDialog_included

#include <QDialog>
#include <QListWidget>
#include "v_user.h"

namespace Ui {
class SelectDialogData;
}

class SelectDialog : public QDialog
{
    Q_OBJECT

    Ui::SelectDialogData *ui;
    bool multi;
    bool sel_to_all;

protected slots:

    void CmHelp();
    virtual void CmSelectAll();
    virtual void CmClearAll();

public:

    /*! single select constructor
    */

    SelectDialog(QWidget* parent, const char* title,
                 TCStringArray& list, int sel);

    SelectDialog(QWidget* parent, const char* title,
                 TCStringArray& list, int sel, bool all_ );

    /*! multiple select constructor
    */
    SelectDialog(QWidget* parent, const char* title,
                 TCStringArray& list, TCIntArray& sel);

    virtual ~SelectDialog();

    /*! returns single selection
        returns '-1' if nothing selected
    */
    int selected();

    int selected( bool& all_ );

    TCIntArray allSelected();

};

#endif // SelectDialog_included
