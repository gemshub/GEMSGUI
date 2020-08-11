//-------------------------------------------------------------------
// $Id: PhaseInfoDialog.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of PhaseInfoDialog class
//
// Copyright (C) 2013  S.Dmytriyeva
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

#include <vector>
#include <QClipboard>
#include <QMenu>
#include <QAction>

#include <iostream>
#include "PhaseInfoDialog.h"
#include "v_mod.h"
#include "model_w.h"
#include "service.h"

PhaseInfoDialog::PhaseInfoDialog(QWidget* parent, bool system,
                                 int xph, string phname, vector<int>& xdclist, vector<string>& dcnames, int xdc):
        QDialog( parent )
{
    int row = 0;
    string str;

    setupUi(this);
    setWindowTitle("Phase Information Widget");

    if( system )
    {
        labelSyst->setText("In Modelling Project System:");
        labelPh->setText("              In Phase with this record key:");
        labelDC->setText( QString("             %1  Depended Component(s) with record key(s):").arg(xdclist.size()));
    }
    else
    {
        labelSyst->setText("In Chemical System Definition:");
        labelPh->setText("              In Phase:");
        labelDC->setText( QString("             %1  Depended Component(s):").arg(xdclist.size()));
    }

    systKey->setText(tr (rt[RT_SYSEQ]->PackKey()));

    // define tablePh
    tablePh->setEditTriggers( QAbstractItemView::NoEditTriggers );
    tablePh->setSelectionMode(QAbstractItemView::SingleSelection);
    //tablePh->setSelectionBehavior ( QAbstractItemView::SelectRows );
    tablePh->clear();
    tablePh->setSortingEnabled ( false );
    if( system )
    {
      tablePh->setColumnCount(6);
      tablePh->setColumnWidth(2, 80 );
      tablePh->setColumnWidth(3, 140 );
    }
    else
    {
      tablePh->setColumnCount(3);
      tablePh->setColumnWidth(2, 100 );
    }
    tablePh->setRowCount(1);
    tablePh->setRowHeight(0, htF(ftString, 0)+6);

    tablePh->setItem(0, 0, new QTableWidgetItem(tr("%1").arg( xph)) );
    str = phname;
    tablePh->setItem(0, 1, new QTableWidgetItem(tr("%1").arg( string(str, 0, MAXSYMB).c_str() )) );
    str = str.substr(MAXSYMB);
    if( system)
     { tablePh->setItem(0, 2, new QTableWidgetItem(tr("%1").arg( string(str, 0, 8).c_str() )) );
        str = str.substr(8);
       tablePh->setItem(0, 3, new QTableWidgetItem(tr("%1").arg( string(str, 0, MAXPHNAME).c_str() )) );
       str = str.substr(MAXPHNAME);
       tablePh->setItem(0, 4, new QTableWidgetItem(tr("%1").arg( string(str, 0, MAXSYMB).c_str() )) );
       str = str.substr(MAXSYMB);
       tablePh->setItem(0, 5, new QTableWidgetItem(tr("%1").arg( str.c_str() )) );
    } else
        tablePh->setItem(0, 2, new QTableWidgetItem(tr("%1").arg( str.c_str() )) );
    tablePh->setMaximumHeight(htF(ftString, 0)+10);


    // define tableC
    tableDC->setEditTriggers( QAbstractItemView::NoEditTriggers );
    tableDC->setSelectionMode(QAbstractItemView::SingleSelection);
    //tableDC->setSelectionBehavior ( QAbstractItemView::SelectRows );
    tableDC->clear();
    tableDC->setSortingEnabled ( false );
    if( system )
    {
      tableDC->setColumnCount(5);
       tableDC->setColumnWidth(2, 80 );
       tableDC->setColumnWidth(3, 140 );
    }
    else
    {
        tableDC->setColumnCount(2);
        tableDC->setColumnWidth(1, 140 );
    }
    tableDC->setRowCount(xdclist.size());

    for( size_t ii=0; ii<xdclist.size(); ii++ )
    {
        tableDC->setRowHeight(ii, htF(ftString, 0)+6);
        tableDC->setItem(ii, 0, new QTableWidgetItem(tr("%1").arg( xdclist[ii])) );

        if( xdclist[ii] == xdc )
         row = ii;
        if( system)
        {  str = dcnames[ii];
           // std::cout << str.c_str() << xdc << endl;
           tableDC->setItem(ii, 1, new QTableWidgetItem(tr("%1").arg( string(str, 0, MAXSYMB).c_str() )) );
           str = str.substr(MAXSYMB);
           tableDC->setItem(ii, 2, new QTableWidgetItem(tr("%1").arg( string(str, 0, MAXDRGROUP).c_str() )) );
           str = str.substr(MAXDRGROUP);
           tableDC->setItem(ii, 3, new QTableWidgetItem(tr("%1").arg( string(str, 0, MAXDCNAME).c_str() )) );
           str = str.substr(MAXDCNAME);
           tableDC->setItem(ii, 4, new QTableWidgetItem(tr("%1").arg( str.c_str() )) );
        } else
             tableDC->setItem(ii, 1, new QTableWidgetItem(tr("%1").arg( dcnames[ii].c_str() )) );
    }

    tableDC->setCurrentItem( tableDC->item(row,0) );
    tableDC->scrollToItem( tableDC->item(row,0), QAbstractItemView::PositionAtCenter );

    QObject::connect(buttonBox, SIGNAL(helpRequested()), this, SLOT(help()));
    tableDC->setContextMenuPolicy(Qt::CustomContextMenu);
    connect( tableDC, SIGNAL(customContextMenuRequested(QPoint)),
         this, SLOT(slotPopupContextMenuDC(QPoint)));
    tablePh->setContextMenuPolicy(Qt::CustomContextMenu);
    connect( tablePh, SIGNAL(customContextMenuRequested(QPoint)),
         this, SLOT(slotPopupContextMenuPh(QPoint)));

}

// copy data
void PhaseInfoDialog::slotPopupContextMenuDC(const QPoint &pos)
{
   QMenu *menu = new QMenu(this);

   QAction *act =  new QAction(tr("&Copy"), this);
     //act->setShortcut(tr("Ctrl+C"));
     act->setStatusTip(tr("Copy selected data"));
           connect(act, SIGNAL(triggered()), this, SLOT(CopyDataDC()));
     menu->addAction(act);

     menu->exec( tableDC->viewport()->mapToGlobal(pos) );
     delete menu;
}

void PhaseInfoDialog::slotPopupContextMenuPh(const QPoint &pos)
{
   QMenu *menu = new QMenu(this);

   QAction *act =  new QAction(tr("&Copy"), this);
     //act->setShortcut(tr("Ctrl+C"));
     act->setStatusTip(tr("Copy selected data"));
           connect(act, SIGNAL(triggered()), this, SLOT(CopyDataPh()));
     menu->addAction(act);

     menu->exec( tablePh->viewport()->mapToGlobal(pos) );
     delete menu;
}

void PhaseInfoDialog::CopyDataDC()
{
   QString clipText = tableDC->currentItem()->text();
   clipText = clipText.trimmed();
   QApplication::clipboard()->setText(clipText/*, QClipboard::Clipboard*/);
}

void PhaseInfoDialog::CopyDataPh()
{
   QString clipText = tablePh->currentItem()->text();
   clipText = clipText.trimmed();
   QApplication::clipboard()->setText(clipText/*, QClipboard::Clipboard*/);
}

PhaseInfoDialog::~PhaseInfoDialog()
{}

void PhaseInfoDialog::languageChange()
{
    retranslateUi(this);
}


void PhaseInfoDialog::help()
{
   pVisorImp->OpenHelp( GEMS_PHINFO_HTML );
}

//--------------------- End of PhaseInfoDialog.cpp ---------------------------
