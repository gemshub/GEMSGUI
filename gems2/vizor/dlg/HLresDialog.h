//-------------------------------------------------------------------
// $Id$
//
// Declaration of HLresDialog class
//
// Copyright (C) 1996-2001   S.Dmytriyeva
// Uses  gstring class (C) A.Rysin 1999
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
#ifndef HLRESDIALOG_H
#define HLRESDIALOG_H


#include <fstream>
#include <qvariant.h>
#include <qdialog.h>
#include "gstring.h"
#include "HLresDialogData.h"
class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QListView;
class QListViewItem;
class QPushButton;

class HLresDialog :
          public HLresDialogData
{
    Q_OBJECT

    QListViewItem* item;


public:
   static HLresDialog* pDia;

   HLresDialog( QWidget* parent = 0,
         const char* name = 0 );
    ~HLresDialog();


protected slots:
    void CmClose();
    void CmHelp();
    void CmPrint();
    void CmRefresh();

protected:

    void closeEvent(QCloseEvent* ev);
    void loadList();
    void printList(fstream& f);
};

#endif // HLRESDIALOG_H
