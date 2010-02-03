//-------------------------------------------------------------------
// $Id: NewSystemDialog.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of NewSystemDialog class
//
// Copyright (C) 1996-2002  A.Rysin, S.Dmytriyeva
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

#ifndef SystemDialog_included
#define SystemDialog_included

#include <fstream>

#include <qmenubar.h>
#include <qlineedit.h>
#include <q3toolbar.h>
#include <q3listview.h>
//Added by qt3to4:
#include <QEvent>
#include <QMouseEvent>
#include <QCloseEvent>
#include <QFocusEvent>

#include "gstring.h"
#include "NewSystemDialogData.h"


class MLineEdit :
          public QLineEdit
{
   Q_OBJECT

    Q3ToolBar* toolBar;
    Q3ListViewItem* it;
    int col;
    int type;
    gstring Vals;

protected:
    void focusOutEvent(QFocusEvent* ev);
    void mousePressEvent(QMouseEvent* e);
//    void mouseDoubleClickEvent(QMouseEvent *ev);

protected slots:
    void SetIndex(int ii);
    void CmHide( int x, int y );
    void CmAccept();

public:

   MLineEdit( QWidget * parent, const char * name=0 );
    ~MLineEdit()
    {}

  void setData( Q3ListViewItem *ait, int acol);
  void getData();
};


class NewSystemDialog : public NewSystemDialogData
{
    void CmEdit( Q3ListViewItem *it, const QPoint & pos, int col);
};

#endif // NewSystemDialog_included

