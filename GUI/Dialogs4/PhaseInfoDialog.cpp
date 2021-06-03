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

#include <QClipboard>
#include <QMenu>
#include <QAction>

#include "ui_PhaseInfoDialog4.h"
#include "PhaseInfoDialog.h"
#include "v_mod.h"
#include "model_w.h"
#include "service.h"

PhaseInfoDialog::PhaseInfoDialog( QWidget* parent, bool system, int xph, string phname,
                                  vector<int>& xdclist, vector<string>& dcnames, int xdc):
    QDialog( parent ),
    ui(new Ui::PhaseInfoDialogData)
{
    int row = 0;
    string str;

    ui->setupUi(this);
    setWindowTitle("Phase Information Widget");

    if( system )
    {
        ui->labelSyst->setText("In Modelling Project System:");
        ui->labelPh->setText("              In Phase with this record key:");
        ui->labelDC->setText( QString("             %1  Depended Component(s) with record key(s):").arg(xdclist.size()));
    }
    else
    {
        ui->labelSyst->setText("In Chemical System Definition:");
        ui->labelPh->setText("              In Phase:");
        ui->labelDC->setText( QString("             %1  Depended Component(s):").arg(xdclist.size()));
    }

    ui->systKey->setText(tr (rt[RT_SYSEQ]->PackKey()));

    // define ui->tablePh
    ui->tablePh->setEditTriggers( QAbstractItemView::NoEditTriggers );
    ui->tablePh->setSelectionMode(QAbstractItemView::SingleSelection);
    //ui->tablePh->setSelectionBehavior ( QAbstractItemView::SelectRows );
    ui->tablePh->clear();
    ui->tablePh->setSortingEnabled ( false );
    if( system )
    {
        ui->tablePh->setColumnCount(6);
        ui->tablePh->setColumnWidth(2, 80 );
        ui->tablePh->setColumnWidth(3, 140 );
    }
    else
    {
        ui->tablePh->setColumnCount(3);
        ui->tablePh->setColumnWidth(2, 100 );
    }
    ui->tablePh->setRowCount(1);
    ui->tablePh->setRowHeight(0, htF(ftString, 0)+6);

    ui->tablePh->setItem(0, 0, new QTableWidgetItem(tr("%1").arg( xph)) );
    str = phname;
    ui->tablePh->setItem(0, 1, new QTableWidgetItem(tr("%1").arg( string(str, 0, MAXSYMB).c_str() )) );
    str = str.substr(MAXSYMB);
    if( system)
    { ui->tablePh->setItem(0, 2, new QTableWidgetItem(tr("%1").arg( string(str, 0, 8).c_str() )) );
        str = str.substr(8);
        ui->tablePh->setItem(0, 3, new QTableWidgetItem(tr("%1").arg( string(str, 0, MAXPHNAME).c_str() )) );
        str = str.substr(MAXPHNAME);
        ui->tablePh->setItem(0, 4, new QTableWidgetItem(tr("%1").arg( string(str, 0, MAXSYMB).c_str() )) );
        str = str.substr(MAXSYMB);
        ui->tablePh->setItem(0, 5, new QTableWidgetItem(tr("%1").arg( str.c_str() )) );
    } else
        ui->tablePh->setItem(0, 2, new QTableWidgetItem(tr("%1").arg( str.c_str() )) );
    ui->tablePh->setMaximumHeight(htF(ftString, 0)+10);


    // define ui->tableC
    ui->tableDC->setEditTriggers( QAbstractItemView::NoEditTriggers );
    ui->tableDC->setSelectionMode(QAbstractItemView::SingleSelection);
    //ui->tableDC->setSelectionBehavior ( QAbstractItemView::SelectRows );
    ui->tableDC->clear();
    ui->tableDC->setSortingEnabled ( false );
    if( system )
    {
        ui->tableDC->setColumnCount(5);
        ui->tableDC->setColumnWidth(2, 80 );
        ui->tableDC->setColumnWidth(3, 140 );
    }
    else
    {
        ui->tableDC->setColumnCount(2);
        ui->tableDC->setColumnWidth(1, 140 );
    }
    ui->tableDC->setRowCount(xdclist.size());

    for( size_t ii=0; ii<xdclist.size(); ii++ )
    {
        ui->tableDC->setRowHeight(ii, htF(ftString, 0)+6);
        ui->tableDC->setItem(ii, 0, new QTableWidgetItem(tr("%1").arg( xdclist[ii])) );

        if( xdclist[ii] == xdc )
            row = ii;
        if( system)
        {
            str = dcnames[ii];
            // std::cout << str.c_str() << xdc << endl;
            ui->tableDC->setItem(ii, 1, new QTableWidgetItem(tr("%1").arg( string(str, 0, MAXSYMB).c_str() )) );
            str = str.substr(MAXSYMB);
            ui->tableDC->setItem(ii, 2, new QTableWidgetItem(tr("%1").arg( string(str, 0, MAXDRGROUP).c_str() )) );
            str = str.substr(MAXDRGROUP);
            ui->tableDC->setItem(ii, 3, new QTableWidgetItem(tr("%1").arg( string(str, 0, MAXDCNAME).c_str() )) );
            str = str.substr(MAXDCNAME);
            ui->tableDC->setItem(ii, 4, new QTableWidgetItem(tr("%1").arg( str.c_str() )) );
        } else
            ui->tableDC->setItem(ii, 1, new QTableWidgetItem(tr("%1").arg( dcnames[ii].c_str() )) );
    }

    ui->tableDC->setCurrentItem( ui->tableDC->item(row,0) );
    ui->tableDC->scrollToItem( ui->tableDC->item(row,0), QAbstractItemView::PositionAtCenter );

    QObject::connect(ui->buttonBox, SIGNAL(helpRequested()), this, SLOT(help()));
    ui->tableDC->setContextMenuPolicy(Qt::CustomContextMenu);
    connect( ui->tableDC, SIGNAL(customContextMenuRequested(QPoint)),
             this, SLOT(slotPopupContextMenuDC(QPoint)));
    ui->tablePh->setContextMenuPolicy(Qt::CustomContextMenu);
    connect( ui->tablePh, SIGNAL(customContextMenuRequested(QPoint)),
             this, SLOT(slotPopupContextMenuPh(QPoint)));
    QObject::connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
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

    menu->exec( ui->tableDC->viewport()->mapToGlobal(pos) );
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

    menu->exec( ui->tablePh->viewport()->mapToGlobal(pos) );
    delete menu;
}

void PhaseInfoDialog::CopyDataDC()
{
    QString clipText = ui->tableDC->currentItem()->text();
    clipText = clipText.trimmed();
    QApplication::clipboard()->setText(clipText/*, QClipboard::Clipboard*/);
}

void PhaseInfoDialog::CopyDataPh()
{
    QString clipText = ui->tablePh->currentItem()->text();
    clipText = clipText.trimmed();
    QApplication::clipboard()->setText(clipText/*, QClipboard::Clipboard*/);
}

PhaseInfoDialog::~PhaseInfoDialog()
{
    delete ui;
}

void PhaseInfoDialog::help()
{
    pVisorImp->OpenHelp( GEMS_PHINFO_HTML );
}

//--------------------- End of PhaseInfoDialog.cpp ---------------------------
