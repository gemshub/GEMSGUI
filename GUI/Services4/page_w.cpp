//-------------------------------------------------------------------
// $Id: page_w.cpp 969 2007-12-13 15:41:25Z gems $
//
// Implementation of TCPage, TQueryWindow and TCWindo
//  TCellCheck, TCellText and TQueryWindow classes
//
// Copyright (C) 1996-2009  A.Rysin, S.Dmitrieva
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

#include <cstdio>

#include <QPushButton>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "v_mod.h"
#include "page_w.h"
#include "GemsMainWindow.h"

const int wdSPACE = 8;
const int htSPACE = 2;
const int wdFSPACE = wdSPACE;
const int htFSPACE = htSPACE;

static const int X0 = 10;	// position of the 1st Field
static const int Y0 = 10;	// on the Page

//----------------------------------------------------------------
// TCPage - define one page on Module window
//----------------------------------------------------------------

TCPage::TCPage(PageInfo& r, bool info, QWidget *parent ):
        QWidget(parent),
        rInfo(r),
        fRedraw(true),
        firstRedraw(true)
{
  //  r.pPage = this;
    mwidth = x = X0;
    mheight = y = Y0;
    
    setFont( pVisorImp->getCellFont() );
    AddFields( info );
}

void TCPage::objectChanged()
{
	rInfo.rWinInfo.rM.CellChanged();
}

TCPage::~TCPage()
{}

void TCPage::show()
{
//    if( fRedraw )
        RedrawFields();
    QWidget::show();
}

//    Updates the page
void TCPage::Update()
{
// for( int ii=0; ii<aModels.GetCount(); ii++ )
//  if( aModels[ii]->SizeChanged() )
//  {
            fRedraw = true;
//            break;
//  }
 if( fRedraw )
    RedrawFields(); 	
}

void TCPage::AddFields( bool info )
{
    TObjectModel* model;
    TObjectTable* fieldTable;
//    TCellText*  fieldText;
	QList<FieldInfo>	aFlds;

    size_t ii = 0, cnt;

    while( ii<getFieldCnt() )
    {
       FieldInfo fi = *rInfo.aFieldInfo[ii];
    
       if(info)
       { if( fi.edit != eParam )  // for TQueryWindow
         { ii++;   
    	   continue;
         }
         else fi.edit = eYes;
       }
        
       aFlds.clear();
       aFlds.append(fi);
        
       cnt = 1;
       while( (ii+cnt) < getFieldCnt() && 
               ( rInfo.aFieldInfo[ii+cnt]->place == Tied ||
                     rInfo.aFieldInfo[ii+cnt]->place == Sticked ||
                     rInfo.aFieldInfo[ii+cnt]->place == UndeTabl ) )
       {
           aFlds.append(*rInfo.aFieldInfo[ii+cnt]);
    	   cnt++;
       }

       model = new TObjectModel( aFlds, this );
       aModels.push_back( std::shared_ptr<TObjectModel>(model) );
       
   	  switch( fi.fType )
       {
        case ftText:
//        	fieldText =  new TCellText( fi, 0, 0, this );
//         	aFields.Add( fieldText );
//         	aTypes.Add(0);
//             break;     
       default:
       	    fieldTable =  new TObjectTable( aFlds, this );
                TObjectDelegate *deleg = new TObjectDelegate( fieldTable, this);
       	    fieldTable->setItemDelegate(deleg);
       	    fieldTable->setModel(model);
            aFields.push_back( std::shared_ptr<TObjectTable>(fieldTable) );
            aTypes.push_back(1);
            break;     
       }
      
   	  ii += cnt;
    }  
   RedrawFields(); // setup sizes for field tables
}

void TCPage::RedrawFields()
{
    fRedraw = false;

    ePlaceMode place;
    int rowSize =0, colSize=0;
    int oldRowSize, oldColSize;

    mwidth = x = X0;
    mheight = y = Y0;
   
    for( size_t ii=0; ii<aModels.size(); ii++ )
    {
    	oldRowSize = rowSize;
    	oldColSize = colSize;
    	
   	  // get size of element
        place = aModels[ii]->getObjectPlace();
        aFields[ii]->getObjectSize(rowSize, colSize);

       // calculating position of the element
       switch( place )
       {
         case First:   x=10;
                       y=10;
                       break;
        case Tied:
        case Right:
                     x += oldColSize;
                        break;
        case Fixed:
        case NextP:
                      x += oldColSize;
                      x += wdFSPACE;
                      break;

        case UndeTabl:
        case Sticked:
        case Down:
                      y += oldRowSize;
                      y += /*LineH +*/ htFSPACE;
                      break;
        case Under:
                      y += oldRowSize;
                      y += htFSPACE;
                      break;

        case MutableB:
        case SkipLine:
                       y += oldRowSize;
        	           y += /*LineH +*/pVisorImp->getCharHeight();
                       x = 10;
                       break;
            }

      aFields[ii]->setGeometry(x, y, colSize, rowSize);
      if( colSize== 0 || rowSize==0 )
    	  aFields[ii]->hide();
      else
    	  aFields[ii]->show();
      
      // size of the window
        mwidth = max( mwidth,  x+colSize );
        mheight = max( mheight, y+rowSize );

        // estimating the height of the line
        switch( place )
        {
        case First:
            break;
        case Tied:
        case Right:
        case Fixed:
        case NextP:
            rowSize = max(rowSize, oldRowSize );
            break;
        case Under:
        case UndeTabl:
        case Sticked:
        case Down:
        case SkipLine:
        case MutableB:
            break;
        }
    }

    resize(mwidth+5, mheight+5);
}

//==========================================
// TQueryWindow - default Remake dialog 
// in other case used wizards
//==========================================


TQueryWindow::TQueryWindow(CWinInfo& w):
        QDialog( w.pWin ),
        rInfo( w )
{
    string s = w.rM.GetName();
    s += ": Remake dialog (press Ok to use default parameters)";
    setWindowTitle(s.c_str());

    // show info fields
    // adds all fields from given module window
    // only from  last page
    // that have 'param' flag set to the Query dialog
    auto LastPage = rInfo.aPageInfo.size()-1;
    aPage = new  TCPage( *rInfo.aPageInfo[LastPage], true );
    scroll = new QScrollArea;
    scroll->setWidget(aPage);
   
    hint = new QLabel;
    hint->setText("Setup fields, look at tooltips!");

    QPushButton* btn = new QPushButton;
    btn->setText("&Ok");
    btn->setDefault(true);

    QVBoxLayout* mainBox = new QVBoxLayout(this);
    mainBox->addWidget( scroll, 1 );
    mainBox->addWidget( hint, 0 );
    mainBox->addWidget( btn, 0 );
   
    connect( btn, SIGNAL(clicked()), this, SLOT(accept()) );
}

// !!! TQueryWindow does not have 'Cancel' button
// since it modifies cells right in their memory
// workaround would be pretty time-consuming
void
TQueryWindow::done(int result)
{
    // focus out for last focused widget
    if( qApp->focusWidget() )                // ??? not need
        qApp->focusWidget()->clearFocus();
    // save state for user window
    rInfo.pWin->Update();                   // ??? not need
    QDialog::done(result);
}

//==========================================
// TCWindow- 
//==========================================

TCWindow::TCWindow(TCModuleImp* pImp, CWinInfo& i, int page):
        QWidget(pImp),
        rInfo(i)
{
    i.pWin = this;

    if( page >= getPageCnt() )
        page = 0;
 
    iCurPage = page;
    QWidget*  pWin = this;
    
    // setup buttons
    pTab = new QButtonGroup;//(pWin);
    connect( pTab, SIGNAL(buttonClicked(QAbstractButton*)), SLOT(EvTabSelChange(QAbstractButton*)) );

    QHBoxLayout* buttonBox = new QHBoxLayout();

    // don't show button if single page
    if( getPageCnt() > 1 )
    {
        for( int ii=0; ii<getPageCnt(); ii++ )
        {
            QPushButton* p = new QPushButton;//( pWin);
            p->setText( rInfo.aPageInfo[ii]->name.c_str() );
            p->setAutoExclusive(true);
            p->setCheckable( true );
            p->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            //p->setMaximumWidth( 50 );
            pTab->addButton(p, ii);
            buttonBox->addWidget( p);
            if(ii == iCurPage)
            	p->setChecked(true);
       }
    }
    
    /*if( !getCModule().IsSubModule() )
    {
        pFilesBut = new QPushButton;
        pFilesBut->setToolTip( "Show open database files" );
        pFilesBut->setText("Files");
        pFilesBut->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        connect( pFilesBut, SIGNAL(clicked()), SLOT(EvFileDown()) );
        buttonBox->addWidget( pFilesBut );
    }*/

   //pPackKey = new QLineEdit;
   //pPackKey->setEnabled( TRUE );
   //pPackKey->setFocusPolicy( Qt::ClickFocus );
   //pPackKey->setReadOnly( TRUE );
   //pPackKey->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
   //QFont pk_font = pPackKey->font();
   //pk_font.setBold(true);
   //pPackKey->setFont(pk_font);
   //buttonBox->addWidget( pPackKey );

    pRTime = new QLabel;//(pWin);
    pRTime->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    pRTime->setFrameStyle( QFrame::Box | QFrame::Raised );
    buttonBox->addWidget( pRTime );

    ShowInfo();

    // setup pages
    pages =  new QStackedWidget;//(pWin);
    for( int ii=0; ii<getPageCnt(); ii++ )
    {
        PageInfo& pi = *rInfo.aPageInfo[ii];
        TCPage* pPage = new TCPage(pi);
        scroll = new QScrollArea;
        scroll->setFrameStyle( QFrame::NoFrame ); //Remove outer frame about data objects
        scroll->setWidget(pPage);
        pages->addWidget( scroll );
        aPages.push_back( std::shared_ptr<TCPage>( pPage) );
    }
    pages->setCurrentIndex(iCurPage);

    QVBoxLayout* mainBox = new QVBoxLayout(pWin);
    //mainBox->addLayout(keyBox);
    mainBox->addLayout(buttonBox);
    mainBox->addWidget( pages, 1 );

   parentWidget()->resize(rInfo.init_width, rInfo.init_height);
}

TCWindow::~TCWindow()
{
    delete pTab;
}

void TCWindow::closeEvent(QCloseEvent* evt)
{
    //rInfo.init_width = parentWidget()->width();
    //rInfo.init_height = parentWidget()->height();
    QWidget::closeEvent(evt);
}

void TCWindow::showEvent( QShowEvent * event )
{
    QWidget::showEvent(event);
    gui_logger->debug("rInfo.init_width {} rInfo.init_height {}", rInfo.init_width, rInfo.init_height);
}

//    Changes the page tab for module and calls TCModule::EvPageChanged()
void TCWindow::EvTabSelChange(QAbstractButton* button)
{
    auto  newSel = pTab->id(button);
    pages->setCurrentIndex( newSel );
    if( newSel != iCurPage && getCModule().IsSubModule() )
        getCModule().EvPageChanged( iCurPage );
    iCurPage = newSel;
    pVisorImp->SetStatus( "" );
}

// Shows information on current record
void TCWindow::ShowInfo()
{
    int nR;
    if( getCModule().pImp->IsSubModule() )
        nR = RT_SYSEQ;
    else nR = getCModule().rtNum();

    time_t time = rt[nR]->Rtime();
    struct tm *time_s;
    tzset();
    time_s = localtime(&time);
    char str[80];
    strftime(str, 20, "%d/%m/%Y, %H:%M", time_s);

    //pPackKey->setText( rt[nR].PackKey() );
    pRTime->setText( str );
}

//    Updates window
void TCWindow::Update()
{
    ShowInfo();
    for( int ii=0; ii<getPageCnt(); ii++ )
    	aPages[ii]->Update();  // ??? not need
}



//---------------------- End of file  page_w.cpp ---------------------------
