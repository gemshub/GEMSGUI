//-------------------------------------------------------------------
// $Id: SelectDialog.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of SelectDialog class
//
// Copyright (C) 1996-2007  A.Rysin, S.Dmytriyeva
// Uses  string class (C) A.Rysin 1999
//
// This file is part of the GEM-Selektor GUI library which uses the
// Qt v.4 cross-platform App & UI framework (http://qt.nokia.com)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the terms of GEMS3 Development
// Quality Assurance Licence (GEMS3.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#ifndef SelectDialog_included
#define SelectDialog_included

#include <QDialog>
#include <QListWidget>

#include "ui_SelectDialog4.h"
#include <string>
using namespace std;

class SelectDialog : public QDialog, public Ui::SelectDialogData
{
    Q_OBJECT

    bool multi;
    bool sel_to_all;

protected slots:
    virtual void languageChange();
    void CmHelp();
    virtual void CmSelectAll();
    virtual void CmClearAll();

public:

    /*! single select constructor
    */

    SelectDialog(QWidget* parent, const char* title,
                 const vector<string>& list, int sel);

    SelectDialog(QWidget* parent, const char* title,
                 const vector<string>& list, int sel, bool all_ );

    /*! multiple select constructor
    */
    SelectDialog(QWidget* parent, const char* title,
                 const vector<string>& list,
                 const vector<int>& sel);

    virtual ~SelectDialog();

    /*! returns single selection
        returns '-1' if nothing selected
    */
    int selected()
    {
        return (( result() ) ? pList->currentRow() : -1);
    }

    int selected( bool& all_ )
    {
        all_ = sel_to_all;
        return ( ( result() ) ? pList->currentRow() : -1);
    }

    vector<int> allSelected();

};

#endif // SelectDialog_included
