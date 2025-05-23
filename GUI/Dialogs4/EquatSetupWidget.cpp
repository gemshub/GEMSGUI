//-------------------------------------------------------------------
// $Id: EquatSetupWidget.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of EquatSetup class
//
// Copyright (C) 2010  S.Dmytriyeva
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

#include <QMenu>

#include "ui_EquatSetupWidget4.h"
#include "EquatSetupWidget.h"
#include "CalcDialog.h"
#include "m_param.h"
#include "service.h"


EquatSetup::EquatSetup( QWidget* parent, equatSetupData aEqData,
                        int nRT, std::vector<pagesSetupData>& wnData, std::vector<pagesSetupData>& scalarsList,
                        const char* script, const char* aXname, const char* aYname ):
    QWidget( parent ),
    ui(new Ui::EquatWidgetForm),
    cPage(-1), cnRT(-1), eqData(aEqData), useCalc(false)
{

    ui->setupUi(this);

    if( aXname )
        xNam = aXname;
    else
        xNam = eqData.xName;
    if( aYname )
        yNam = aYname;
    else
        yNam = eqData.yName;

    ui->listStatic->setWrapping( true );
    ui->listStatic->setSelectionMode(QAbstractItemView::MultiSelection);

    // define lists pages
    resetPageList( nRT,wnData, scalarsList );

    // define Script
    ui->textScript->setText(script);

    // define commands
    QObject::connect( ui->keywdList, SIGNAL(currentRowChanged(int)),
                      this, SLOT(changePage(int)));
}


EquatSetup::~EquatSetup()
{
    delete ui;
}

// work with lists
void EquatSetup::resetPageList( int newRT,
                                std::vector<pagesSetupData>& wnData, std::vector<pagesSetupData>& scalarsList )
{
    size_t ii, jj;
    int nO;
    QString str;
    QListWidgetItem* item1;
    QWidget* page1;
    QHBoxLayout* horizontalLayout1;
    QListWidget* lstIndexes1;


    if( cnRT == newRT )
        return;

    try
    {
        // delete old data
        cPage = 0;
        // ui->keywdList->setCurrentItem(0);
        if(  ui->winStac->count() > 1)
        { for(ii= ui->winStac->count()-1; ii>0; ii--)
            {
                ui->winStac->removeWidget(  ui->winStac->widget(ii) );
            }
        }
        pgData.clear();
        pLists.clear();
        // define lists pages
        pgData.push_back( pagesSetupData("Scalars", -1));
        pLists.append(  ui->listStatic  );
        ui->keywdList->clear();
        new QListWidgetItem( "Scalars",   ui->keywdList);
        emptyScriptTable();

        cnRT = newRT;

        //define scalars
        QObject::disconnect(  ui->listStatic->selectionModel(), SIGNAL( selectionChanged( const QItemSelection&, const QItemSelection& ) ),
                              this, SLOT( changeTable( const QItemSelection&, const QItemSelection& )) );
        QObject::disconnect(  ui->listStatic, SIGNAL(customContextMenuRequested(QPoint)),
                              this, SLOT(slotPopupContextMenu(QPoint)));

        ui->listStatic->clear();
        for(  ii=0; ii<scalarsList.size(); ii++ )
        {
            stData.push_back( pagesSetupData(scalarsList[ii]));
            item1 = new QListWidgetItem( scalarsList[ii].pageName.c_str(),   ui->listStatic);
            int nJ = scalarsList[ii].ndx;
            if(nJ<0) nJ = 0;
            nO = scalarsList[ii].nObj;
            if( aObj[nO]->GetM() > 1 )
                item1->setToolTip( aObj[nO]->GetDescription(0,nJ).c_str() );
            else
                item1->setToolTip( aObj[nO]->GetDescription(nJ,0).c_str() );
        }

        // init new pages
        for( ii=0; ii<wnData.size(); ii++ )
        {
            nO = wnData[ii].nObj;

            TCStringArray lst;
            TProfil::pm->getNamesList( nO, lst);
            if( lst.size() < 1 )  // undefined indexation
                continue;

            pgData.push_back( pagesSetupData(wnData[ii]));
            gui_logger->trace("EquatSetup::resetPageList {}  {}", pgData[pgData.size()-1].pageName, pgData[pgData.size()-1].ndxName);
            // insert to list
            //str = QString("%1 (%2)").arg(
            //    wnData[ii].pageName.c_str(), aObj[nO]->GetKeywd());
            str = QString("%1").arg( wnData[ii].pageName.c_str());
            item1 = new QListWidgetItem( str,   ui->keywdList);

            // add page
            page1 = new QWidget();
            //page1->setObjectName(wnData[nWin].pageName.c_str());
            horizontalLayout1 = new QHBoxLayout(page1);
            lstIndexes1 = new QListWidget(page1);
            lstIndexes1->setWrapping( true );
            lstIndexes1->setResizeMode(QListView::Adjust);
            lstIndexes1->setSelectionMode(QAbstractItemView::MultiSelection);
            horizontalLayout1->addWidget(lstIndexes1);
            ui->winStac->addWidget(page1);

            // insert items to list of indexes
            for(  jj=0; jj<lst.size(); jj++ )
            {
                item1 = new QListWidgetItem( lst[jj].c_str(), lstIndexes1);
                //str = QString("%1").arg(jj);
                // item1->setToolTip( str );
            }
            pLists.append(lstIndexes1);
        }

        // define current page
        cPage = 0;
        ui->keywdList->setCurrentItem(nullptr);
        ui->keywdList->item(0)->setSelected(true);
        changePage( cPage );

        for(int  ii1=0; ii1<pLists.count(); ii1++ ) //  ui->listStatic added as first
        {
            QObject::connect( pLists[ii1]->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
                               this, SLOT(changeTable(const QItemSelection&, const QItemSelection&)));

            pLists[ii1]->setContextMenuPolicy(Qt::CustomContextMenu);
            connect( pLists[ii1], SIGNAL(customContextMenuRequested(QPoint)),
                     this, SLOT(slotPopupContextMenu(QPoint)));
        }
    }
    catch( TError& xcpt )
    {
        vfMessage( this, xcpt.title, xcpt.mess);
        throw;
    }

}

string EquatSetup::getScript() const
{
    string res = ui->textScript->toPlainText().toStdString();
    return res;
}

TCStringArray EquatSetup::getNames(string& xName, string& yName) const
{
    xName = xNam;
    yName = yNam;

    return  namLines;
}

void EquatSetup::setNames(TCStringArray lst )
{
    namLines.clear();
    for(size_t ii=0; ii<lst.size(); ii++ )
        namLines.push_back(lst[ii]);
}

QString EquatSetup::getTextScript() const
{
    return ui->textScript->toPlainText();
}

void EquatSetup::setTextScript(const QString &text)
{
    ui->textScript->setText( text );
}

void EquatSetup::changePage( int nPage )
{
    if(nPage < 0 )
        return;
    cPage = nPage;
    ui->winStac->setCurrentIndex ( nPage );
    int nJ = pgData[nPage].ndx;
    if(nJ<0) nJ = 0;

    if( cPage <= 0 )
        ui->lDesc->setText( "List of static data objects (see tooltip on each object name)");
    else
        ui->lDesc->setText( aObj[pgData[nPage].nObj]->GetDescription(0,nJ).c_str());
}

void EquatSetup::changeTable(const QItemSelection & selected, const QItemSelection & deselected)
{
    QModelIndex  ndx;
    int row;

    // added selected
    foreach( ndx,  selected.indexes()  )
    {
        string stt = ndx.data(Qt::DisplayRole).toString().toStdString();
        tableInsertRow( pgData[cPage].nObj, ndx.row(), stt.c_str() );
    }
    // delete deselected
    foreach( ndx,  deselected.indexes()  )
    {
        row = tableFindRow( pgData[cPage].nObj, ndx.row());
        if( row >= 0)
            tableDeleteRow( row );
    }
}

// internal functions
int EquatSetup::tableFindRow( int nO, int ndx)
{
    int nRow = -1;
    for(size_t ii=0; ii<scriptData.size(); ii++ )
    {
        if(scriptData[ii].nObj == nO && scriptData[ii].nIdx == ndx )
        { nRow = ii; break; }
    }
    return nRow;
}

string EquatSetup::getStringValue( int nO, int ndx, const char * andName )
{
    QString textS;

    string str = andName;
    strip( str );
    if(cPage == 0)
    {
        if( stData[ndx].ndx == -1 )
        {
            textS = QString("%1[%2]").arg(str.c_str(), eqData.indexName.c_str() );
        }
        else
        {
            textS = QString("%1").arg(str.c_str());
        }
    }
    else
    {

        if( aObj[nO]->GetM() > 1)
        {
            QString ndxS;

            if( !pgData[cPage].ndxName.empty() )
            {
                ndxS =  pgData[cPage].ndxName.c_str();//
                textS = QString("%1[{%2}][{%3}]").arg( aObj[nO]->GetKeywd(), ndxS, str.c_str() );
            }
            else
            {  ndxS = QString("%1").arg(pgData[cPage].ndx);
                textS = QString("%1[{%2}][%3]").arg( aObj[nO]->GetKeywd(), str.c_str(), ndxS );
            }
        }
        else
        {
            textS = QString("%1[{%2}]").arg( aObj[nO]->GetKeywd(), str.c_str() );
        }
    }

    str = textS.toStdString();
    return str;
}

void EquatSetup::tableInsertRow( int nO, int ndx, const char * andName )
{
    string str = getStringValue(  nO, ndx, andName );

    if( useCalc )
    {
        CalcDialog calc(topLevelWidget(), -1 );
        if( calc.exec() )
        {
            str = calc.funText( str.c_str() );
        }
    }

    // added for clear names that got from Process calcScripts
    if(namLines.size() > scriptData.size() )
        namLines.clear();

    if(cPage == 0)
    {
        scriptData.push_back(  scriptSetupData( cPage, nO, andName,
                                                ndx, andName, str.c_str() ));
        namLines.push_back(andName);
        if( yNam.empty() )
            yNam = andName;
    }
    else
    {
        scriptData.push_back(  scriptSetupData( cPage, nO, aObj[nO]->GetKeywd(),
                                                ndx, andName, str.c_str() ));
        namLines.push_back(andName);
        if( yNam.empty() )
            yNam = aObj[nO]->GetKeywd();
    }

    scriptUpdate();
    useCalc = false;
}


void EquatSetup::CmCalc()
{
    int row = pLists[cPage]->currentRow();
    int nO =  pgData[cPage].nObj;
    int nLine = tableFindRow( nO, row );
    if( nLine >=0 )
        tableDeleteRow( nLine );
    //pLists[cPage]->item(row)->setSelected(false);

    useCalc = true;

    if( nLine < 0 )
    {
        pLists[cPage]->item(row)->setSelected(true);
    }
    else
    {
        QListWidgetItem* ndx = pLists[cPage]->item(row);
        string stt = ndx->data(Qt::DisplayRole).toString().toStdString();
        tableInsertRow( nO, row, stt.c_str() );
    }
}


void EquatSetup::tableDeleteRow( int nRow )
{
    scriptData.erase(scriptData.begin()+nRow);
    namLines.erase(namLines.begin() +nRow);
    scriptUpdate();
}

void EquatSetup::emptyScriptTable()
{
    scriptData.clear();;
    namLines.clear();
    //scriptUpdate();
}


void EquatSetup::scriptUpdate()
{
    size_t ii;
    QString buf, tScript;

    if( !eqData.xName.empty() )
    {
        tScript = QString("%1[%2] =: %3;\n").arg( eqData.xName.c_str(),
                                                  eqData.indexName.c_str(), eqData.abscissaEquat.c_str() );
        for( ii=0; ii<eqData.abscissaLines.size(); ii++ )
        {
            buf = QString("%1[%2][%3] =: %4;\n").arg(
                        eqData.xName.c_str(), eqData.indexName.c_str(),
                        QString("%1").arg(ii+1), eqData.abscissaLines[ii].c_str() );
            tScript += buf;
        }

    }

    for( ii=0; ii<scriptData.size(); ii++ )
    {
        buf = QString("%1[%2][%3] =: %4;\n").arg(
                    eqData.yName.c_str(), eqData.indexName.c_str(),
                    QString("%1").arg(ii), scriptData[ii].lineText.c_str() );
        tScript += buf;
    }

    ui->textScript->setText(tScript);
}

// copy data
void EquatSetup::slotPopupContextMenu(const QPoint &pos)
{
    QMenu *menu = new QMenu( pLists[cPage]/*this*/);

    QAction *act =  new QAction(tr("Calculator"), this);
    act->setStatusTip(tr("Use Calculator for specified equation"));
    connect(act, SIGNAL(triggered()), this, SLOT(CmCalc()));
    menu->addAction(act);

    menu->addSeparator();

    act =  new QAction(tr("&Abscissa"), this);
    act->setStatusTip(tr("Define value as Abscissa"));
    connect(act, SIGNAL(triggered()), this, SLOT(CmAbscissa()));
    menu->addAction(act);

    if( eqData.useSeveral )
    {
        act =  new QAction(tr("&AbscissaAdd"), this);
        act->setStatusTip(tr("Define value as new Abscissa"));
        connect(act, SIGNAL(triggered()), this, SLOT(CmAbscissaAdd()));
        menu->addAction(act);

    }


    menu->exec( pLists[cPage]->mapToGlobal(pos) );
    delete menu;
}

void EquatSetup::CmAbscissa()
{
    int nO =  pgData[cPage].nObj;
    QListWidgetItem* ndx = pLists[cPage]->currentItem();
    string str = ndx->data(Qt::DisplayRole).toString().toStdString();

    eqData.abscissaEquat = getStringValue( nO, pLists[cPage]->currentRow(), str.c_str() );
    if(nO<0)
        xNam = str;
    else
        xNam = aObj[nO]->GetKeywd();

    eqData.abscissaLines.clear();
    scriptUpdate();
}

void EquatSetup::CmAbscissaAdd()
{
    int nO =  pgData[cPage].nObj;
    QListWidgetItem* ndx = pLists[cPage]->currentItem();
    string str = ndx->data(Qt::DisplayRole).toString().toStdString();

    eqData.abscissaLines.push_back( getStringValue( nO, pLists[cPage]->currentRow(), str.c_str() ));
    /*if(nO<0)
      xNam = str;
    else
      xNam = aObj[nO]->GetKeywd();
   */
    scriptUpdate();
}

//--------------------- End of EquatSetupWidget.cpp ---------------------------
