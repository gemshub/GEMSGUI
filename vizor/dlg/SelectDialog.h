//-------------------------------------------------------------------
// $Id$
//
// Declaration of SelectDialog class
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

#ifndef SelectDialog_included
#define SelectDialog_included

#include <qlistbox.h>

#include "SelectDialogData.h"
#include "gstring.h"
#include "v_user.h"

class SelectDialog : public SelectDialogData
{
    Q_OBJECT

    bool multi;
    bool sel_to_all;
    
protected slots:
    void CmClearAll();
    void CmSelectAll();

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
    int selected()
    {
        return ( result() ) ? pList->currentItem() : -1;
    }
    int selected( bool& all_ )
    {
        all_ = sel_to_all;
        return ( result() ) ? pList->currentItem() : -1;
    }

    TCIntArray allSelected();

};

#endif // SelectDialog_included
