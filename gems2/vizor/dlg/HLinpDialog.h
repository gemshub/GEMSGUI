//-------------------------------------------------------------------
// Id: gems/vizor/dlg/HLinpDialog.h  version 2.0.0    2001
//
// Declaration of HLinpDialog class
//
// Created : 970207    Modified: 010908
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
#ifndef HLinpDialog_H
#define HLinpDialog_H


#include <fstream>
#include <qvariant.h>
#include <qdialog.h>
#include <qlineedit.h>

#include "gstring.h"
#include "HLinpDialogData.h"


class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QLineEdit;
class QListView;
class QListViewItem;
class QPushButton;

class MLineEdit :
          public QLineEdit
{
   Q_OBJECT

    QListViewItem* it;
    int col;
    int type;
    gstring Vals;

protected:
    void focusOutEvent(QFocusEvent*);
    void mousePressEvent(QMouseEvent* e);

protected slots:
    void SetIndex(int ii);

public:

   MLineEdit( QWidget * parent, const char * name=0 );
    ~MLineEdit()
    {}

  void setData( QListViewItem *ait, int acol);
  void getData();
};


class HLinpDialog :
          public HLinpDialogData
{
    Q_OBJECT

    QListViewItem* item;
    MLineEdit* colEdit;

public:
   static HLinpDialog* pDia;

   HLinpDialog( QWidget* parent = 0,
         const char* name = 0 );
    ~HLinpDialog();
   void Update();


protected slots:
    void CmHelp();
    void CmPrint();
    void CmBCC();
    void CmCancel();
    void CmOK();
    void CmUndo();
    void CmEdit( QListViewItem *it, const QPoint & pos, int col);

protected:

    void closeEvent(QCloseEvent* ev);
    void loadList();
    void saveList();
    void printList(fstream& f);
};

#endif // HLinpDialog_H
