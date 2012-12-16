//-------------------------------------------------------------------
// $Id: module_w.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of GemsMainWindow class
//
// Copyright (C) 2011  S.Dmytriyeva
// Uses  gstring class (C) A.Rysin 1999
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

#include <QtGui>
#include <QThread>

#include "visor.h"
#include "HelpWindow.h"
#include "GemsMainWindow.h"
#include "ProgressDialog.h"
#include "NewSystemDialog.h"
#include "LoadMessage.h"
#include "GraphDialog.h"

const char* GEMS_LOGO_ICON = "Icons/gems1.png";
const char* GEMS_DEFAULT_FONT_NAME = "Courier New";
const char* GEMS_VERSION_STAMP = "GEM-Selektor 3 (GEMS3)";
#ifdef __unix
#ifdef __APPLE__
const int GEMS_DEFAULT_FONT_SIZE = 14;
#else
const int GEMS_DEFAULT_FONT_SIZE = 11;
#endif
#else
const int GEMS_DEFAULT_FONT_SIZE = 10;
#endif
TVisorImp* pVisorImp;


//--------------------------------------------------------------------------


void TKeyTable::keyPressEvent(QKeyEvent* e)
{
    QTableWidget::keyPressEvent(e);
    switch( e->key() )
    {
      case Qt::Key_Up:
      case Qt::Key_Down:
      case Qt::Key_PageUp:
      case Qt::Key_PageDown:
        pVisorImp->openRecordKey(  currentRow(), currentColumn()  );
    }
}


//--------------------------------------------------------------------------



//   The constructor
TVisorImp::TVisorImp(int c, char** v):
        QMainWindow(0),
        argc(c),
        argv(v),
        last_update( 0 ),
        configAutosave(false),
        proc(0),
    currentNrt(-2),
    settedCureentKeyIntotbKeys(false)
{
    setupUi(this);
    (void)statusBar();
    //setMinimumSize( 300, 200 );

// from visor_w.cpp (old TVisorImp)

     charWidth = 12;
     charHeight = 18;
     pVisorImp = this;

   #ifdef Use_mt_mode
       updateTime = 10; // centiseconds
   #else
       updateTime = 1; // second
   #endif

       defaultFont = QFont(GEMS_DEFAULT_FONT_NAME, GEMS_DEFAULT_FONT_SIZE);
       setCellFont( defaultFont );
       axisLabelFont.setPointSize( 14); //11

       pVisor = new TVisor(argc, argv);
       pVisor->Setup();
       pThread = QThread::currentThreadId();
       //setWindowTitle( GEMS_VERSION_STAMP );
       SetCaption("GEM-Selektor 3 (GEMS3) - Geochemical Equilibrium Modelling "
                  "by Gibbs Energy Minimization");

// define menu part
       //iconSystem.addFile(
       //  QString::fromUtf8(":/menu/Icons/EquilibriumModeIcon.png"),
       //                     QSize(), QIcon::Normal, QIcon::Off);
       //iconDatabase.addFile(
       //  QString::fromUtf8(":/menu/Icons/DatabaseModeIcon.png"),
       //                     QSize(), QIcon::Normal, QIcon::Off);

       toolDataBase = new QToolBar(this);
       toolDataBase->setObjectName(QString::fromUtf8("toolDataBase"));
       toolDataBase->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
       toolDataBase->setMovable(false);
       this->addToolBar(Qt::LeftToolBarArea, toolDataBase);
       toolDataBase->setIconSize(QSize(40,40));
       toolDataBase->addAction(actionIComp);
       toolDataBase->addAction(actionDComp);
       toolDataBase->addAction(actionReacDC);
       toolDataBase->addAction(actionRTparm);
       toolDataBase->addAction(actionPhase);
       toolDataBase->addAction(actionCompos);
       toolDataBase->setWindowTitle("toolDataBase");

       toolProject = new QToolBar(this);
       toolProject->setObjectName(QString::fromUtf8("toolProject"));
       toolProject->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
       toolProject->setMovable(false);
       this->addToolBar(Qt::LeftToolBarArea, toolProject);
       toolProject->setIconSize(QSize(40,40));
       toolProject->addAction(actionSysEq);
       toolProject->addAction(actionProcess);
       toolProject->addAction(actionGtDemo);
       toolProject->addAction(actionGEM2MT);
       //toolProject->addAction(actionDualTh);
       //toolProject->addAction(actionUnSpace);
       toolProject->addAction(actionProject);
       toolProject->setWindowTitle("toolProject");


    // Define internal area

    splH = new QSplitter(Qt::Horizontal );
    splH->setChildrenCollapsible(false);
    //spl->setMidLineWidth(-3);
    //spl->setLineWidth(-3);
    horizontalLayout->setContentsMargins( 0, 0, 0, 0 );
    horizontalLayout->addWidget(splH);

// column 1
    QGridLayout *layoutTab = new QGridLayout;
    layoutTab->setContentsMargins( 0, 0, 0, 0 );
    layoutTab->setSpacing(0);
    itemWidget1 = new QWidget;
    itemWidget1->setLayout(layoutTab);

    QPushButton *btnFilter = new QPushButton();
    QIcon icon11;
    icon11.addFile(QString::fromUtf8(":/menu/Icons/SetKeyTemplateIcon.png"),
                   QSize(), QIcon::Normal, QIcon::Off);
    btnFilter->setIcon(icon11);
    connect( btnFilter, SIGNAL(clicked()), this, SLOT(CmFilter()) );
    layoutTab->addWidget( btnFilter, 0,0 );

    pFilterKey = new QLineEdit();
    pFilterKey->setToolTip( "Set filter for record keys");
    //pFilterKey->setReadOnly( true );
    pFilterKey->setText("*");
    layoutTab->addWidget(pFilterKey, 0, 1);

    tbKeys = new TKeyTable( this );
    layoutTab->addWidget( tbKeys, 1, 0, 1, 2 );
    tbKeys->setEditTriggers( QAbstractItemView::NoEditTriggers );
    tbKeys->setSelectionMode(QAbstractItemView::SingleSelection);
    tbKeys->setSelectionBehavior ( QAbstractItemView::SelectRows );

     splH->addWidget(itemWidget1);

// column 2

    QSplitter *splV = new QSplitter(Qt::Vertical );
    splV->setChildrenCollapsible(false);

    mdiArea = new QMdiArea();
    mdiArea->setMidLineWidth(0);
    mdiArea->setLineWidth(0);
    mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mdiArea->setSizePolicy(sizePolicy2);
    splV->addWidget(mdiArea);

    itemWidget2 = new QWidget;
    layout2 = new QHBoxLayout;
    layout2->setContentsMargins( 0, 0, 0, 0 );
    layout2->setSpacing(0);
    itemWidget2->setLayout(layout2);
    messageText = new QLabel("\n");
    layout2->addWidget(messageText);
    splV->addWidget(itemWidget2);

    splV->setStretchFactor(0, 2);
    splV->setStretchFactor(1, 1);

    splH->addWidget(splV);
    //splH->setStretchFactor(0, 1);
    //splH->setStretchFactor(1, 2);

// define signal/slots

    connect( action_calcMode, SIGNAL( triggered()), this, SLOT(CmCalcMode()));
    connect( actionDataBaseMode, SIGNAL( triggered()), this, SLOT(CmDataBaseMode()));
    //connect(bModeGroup, SIGNAL(buttonClicked(int)),
    //        this, SLOT(buttonGroupClicked(int)));

    connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow *)),
            this, SLOT(updateMenus()));

    windowMapper = new QSignalMapper(this);
    connect(windowMapper, SIGNAL(mapped(QWidget *)),
            this, SLOT(setActiveSubWindow(QWidget *)));

    connect(splH, SIGNAL(splitterMoved( int , int  )),
            this, SLOT(moveToolBar( int , int  )));

    //connect(tbKeys, SIGNAL(cellDoubleClicked ( int , int )  ),
    //        this, SLOT(openRecordKey( int, int )));
    connect(tbKeys, SIGNAL(cellClicked ( int , int  ) ),
            this, SLOT(openRecordKey( int, int )));

    connect( pFilterKey, SIGNAL(editingFinished ()), this, SLOT(changeKeyList()) );

 // Set up Menus and actions

    setActions();

    // Use database mode as start mode
    //actionDataBaseMode->setChecked(true);

    if( MDD_DATABASE == pVisor->ProfileMode )
    {
        SetGeneralMode();
        actionDataBaseMode->setChecked(true);
        //CmDataBaseMode();
        //toolProject->hide();
    }
    else
    {

        // Open calc mode and load last project
        if( !SetProfileMode(pVisor->lastProjectKey.c_str()))
        {
            //action_calcMode->setChecked( false );
            SetGeneralMode();
            actionDataBaseMode->setChecked(true);
         }
        else
        {
           action_calcMode->setChecked(true);
           // load last system
           if( rt[RT_SYSEQ].Find( pVisor->lastSystemKey.c_str()) >= 0 )
           CmShow( pVisor->lastSystemKey.c_str() );
           //NewSystemDialog::pDia->CmSelect( pVisor->lastSystemKey.c_str());
        }
     }

    moveToolBar();
    updateMenus();
 }

//   The desctructor
TVisorImp::~TVisorImp()
{
    delete pVisor;
}

void TVisorImp::closeEvent(QCloseEvent* e)
{
    if( pVisor->CanClose() )
    {

        for(int ii=0; ii<10000; ii++);
        if( LoadMessage::pDia )
             LoadMessage::pDia->cancel();

       if( HelpWindow::pDia )
          delete HelpWindow::pDia;
       mdiArea->closeAllSubWindows();
       if( mdiArea->subWindowList().count() > 0 )
           e->ignore();
       else
           QWidget::closeEvent(e);
    }
}


void TVisorImp::resizeEvent ( QResizeEvent * event )
{
    QMainWindow::resizeEvent(event);
    moveToolBar();
}

void TVisorImp::showEvent ( QShowEvent * event )
{
    QMainWindow::showEvent(event);
    moveToolBar();
}

//------------------------------------------------------------------------------------
// Define list of Module keys using filter
void TVisorImp::defineModuleKeysList( int nRT )
{
  int ii, jj, kk, ln, colsz;
  gstring keyfld;
  QTableWidgetItem *item, *curItem=0;
  gstring oldKey = rt[nRT].UnpackKey();
  settedCureentKeyIntotbKeys = false;

  if(currentNrt != nRT)
    return;

  pFilterKey->setText(((TCModule*)&aMod[nRT])->getFilter());

  // define tbKeys
  tbKeys->clear();
  tbKeys->setSortingEnabled ( false );
  tbKeys->setColumnCount( rt[nRT].KeyNumFlds());


  // get list or record keys
  gstring keyFilter = pFilterKey->text().toLatin1().data();
  TCIntArray temp, colSizes;
  TCStringArray keyList;
  int nKeys = rt[nRT].GetKeyList( keyFilter.c_str(), keyList, temp);

  for(jj=0; jj<rt[nRT].KeyNumFlds(); jj++)
   colSizes.Add( 0/*rt[nRT].FldLen(jj)*/ );

  // define key list
  tbKeys->setRowCount(nKeys);

  for( ii=0; ii<nKeys; ii++ )
  {
      tbKeys->setRowHeight(ii, htF(ftString, 0)+2);
      for(jj=0, kk=0; jj<rt[nRT].KeyNumFlds(); jj++)
      {

          ln = rt[nRT].FldLen(jj);
          keyfld = gstring(keyList[ii], kk, ln);
          StripLine(keyfld);
          colsz = keyfld.length()+1;
          if( colsz > colSizes[jj])
              colSizes[jj] = colsz;
          kk += ln;
          item = new QTableWidgetItem(tr("%1").arg( keyfld.c_str()));
          tbKeys->setItem(ii, jj, item );
       }
      if( oldKey == keyList[ii] )
      {    curItem = tbKeys->item(ii,0);
           settedCureentKeyIntotbKeys = true;
      }

  }
  for(jj=0; jj<rt[nRT].KeyNumFlds(); jj++)
  {
      tbKeys->setColumnWidth(jj, wdF( ftString, colSizes[jj], eNo ) );
      item = new QTableWidgetItem(tr("%1").arg( jj+1));
      item->setToolTip( ((TCModule*)&aMod[nRT])->GetFldHelp(jj));
      tbKeys->setHorizontalHeaderItem( jj, item );
  }

  tbKeys->setSortingEnabled ( true );
  if(curItem )
  {
    tbKeys->setCurrentItem( curItem );
    tbKeys->scrollToItem( curItem, QAbstractItemView::PositionAtCenter );
  }

  if( pVisor->ProfileMode == true && (nRT == RT_SYSEQ || nRT == RT_PROCES
    || nRT == RT_UNSPACE  || nRT > RT_GTDEMO ) )
  {
     tbKeys->hideColumn(0);
     tbKeys->hideColumn(1);
  }
  else
  {
     tbKeys->showColumn(0);
     tbKeys->showColumn(1);
  }

  rt[nRT].SetKey(oldKey.c_str());
}

// Change list of Modules for General or Project mode
void TVisorImp::changeModulesKeys( int nRT )
{

    if( nRT >=0 )
      pLine->setText(tr (rt[nRT].PackKey()));
    else
        pLine->setText(tr(""));

    if( currentNrt == nRT )
        return;

    currentNrt = nRT;
    if( nRT < 0)
    {
        tbKeys->clear();
        tbKeys->setColumnCount( 0);
        tbKeys->setRowCount(0);
        pFilterKey->setText("*");

    }
    else
    {    //pFilterKey->setText(((TCModule*)&aMod[nRT])->getFilter());
         defineModuleKeysList( nRT );
    }
   // currentNrt = nRT;
}


void TVisorImp::openRecordKey( int row, int    )
{
    gstring currentKey ="";

    if( row >= tbKeys->rowCount())
        return;

    for(int jj=0; jj<tbKeys->columnCount(); jj++)
    {
        currentKey += tbKeys->item( row, jj)->text().toLatin1().data();
        StripLine(currentKey);
        currentKey +=":";
     }

    CmShow( currentKey.c_str() );
}


void TVisorImp::changeKeyList()
{
    if( currentNrt >=0 )
    { gstring filter = pFilterKey->text().toLatin1().data();
     ((TCModule*)&aMod[currentNrt])->setFilter(filter.c_str());
     defineModuleKeysList( currentNrt );
    }
}

//------------------------------------------------------------------------------------------

// working with TCModuleImp
void TVisorImp::OpenModule(QWidget* /*par*/, int irt, int page, int viewmode, bool select)
{
    try
    {
       if( irt==RT_SYSEQ && pVisor->ProfileMode == MDD_SYSTEM )
       {
         if( !NewSystemDialog::pDia )
         {
           (new NewSystemDialog(0/*pVisorImp*/));
           //----if( select )
           //----   NewSystemDialog::pDia->CmSelect();
           openMdiChild( NewSystemDialog::pDia, true );

           // Create, if no syseq is present in the project
           if( rt[RT_SYSEQ].RecCount() <= 0)
               NewSystemDialog::pDia->CmCreate();

           //mdiArea->addSubWindow(NewSystemDialog::pDia);
           //NewSystemDialog::pDia->showMaximized();//show();
           if( !settedCureentKeyIntotbKeys )
               openRecordKey(0,0);
        }
        else
         {
           QMdiSubWindow *wn = findNewSystem();
           NewSystemDialog::pDia->Update();
           setActiveSubWindow(wn);
         }
       }
       else
       {
         TCModuleImp* p = aMod[irt].pImp;
         if( !p )
         {   p = new TCModuleImp(irt, page, viewmode);
            //--p->Raise(page);
            // Select record if list not empty
            //----if( select )
            //----    p->SelectStart();
            openMdiChild( p );
            //mdiArea->addSubWindow(p);
            //p->show();
            if( !settedCureentKeyIntotbKeys )
                openRecordKey(0,0);
          }
          else
          {
            QMdiSubWindow *wn = findMdiChild(p->moduleName().c_str());
            p->setViewMode(viewmode);
            p->Update(true);
            setActiveSubWindow(wn);
             //--p->Raise(page);
           }
      }
    }
    catch(TError& e)
    {
        vfMessage(this, "Error opening module", e.mess.c_str() );
    }
}

TCModuleImp *TVisorImp::activeMdiChild()
{
    if (QMdiSubWindow *activeSubWindow = mdiArea->activeSubWindow())
    {
       TCModuleImp *child = qobject_cast<TCModuleImp *>(activeSubWindow->widget());
       if(child)
          return child;
       else
       { // find module for graphic window
         GraphDialog *dlg = qobject_cast<GraphDialog *>(activeSubWindow->widget());
         if( dlg )
         {
             QMdiSubWindow *mdi = findMdiChild(dlg->mainModuleName().c_str());
             if( mdi )
             return(qobject_cast<TCModuleImp *>(mdi->widget()));
         }
       }
    }
    return 0;
}

NewSystemDialog *TVisorImp::activeNewSystem()
{
    if (QMdiSubWindow *activeSubWindow = mdiArea->activeSubWindow())
    {
     return qobject_cast<NewSystemDialog *>(activeSubWindow->widget());
    }
    return 0;
}

NewSystemDialog *TVisorImp::activeNewSystemCommand()
{
    if (QMdiSubWindow *activeSubWindow = mdiArea->activeSubWindow())
    {
       NewSystemDialog * systwindow = qobject_cast<NewSystemDialog *>(activeSubWindow->widget());
       if( systwindow )
         return systwindow;
       else
       {
         TCModuleImp *child = qobject_cast<TCModuleImp *>(activeSubWindow->widget());
         if( child && child->rtNum() >= MD_RMULTS)
         {
           QMdiSubWindow *win = findNewSystem();
           if( win )
             return qobject_cast<NewSystemDialog *>(win->widget());
           else  // open   main NewSystemDialog
           {  OpenModule(this, RT_SYSEQ, 0, true, true);
              activeSubWindow = mdiArea->activeSubWindow();
              return qobject_cast<NewSystemDialog *>(activeSubWindow->widget());
           }
         }
       }
    }
    return 0;
}

QMdiSubWindow *TVisorImp::findMdiChild(const QString &moduleName)
{
    foreach (QMdiSubWindow *window, mdiArea->subWindowList())
    {
        TCModuleImp *mdiChild = qobject_cast<TCModuleImp *>(window->widget());
        if ( mdiChild && QString(mdiChild->moduleName().c_str()) == moduleName)
            return window;
    }
    return 0;
}

QMdiSubWindow *TVisorImp::findMdiGraph(const QString &moduleName)
{
    QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
    for (int i = windows.size()-1; i >= 0; i--)
    {
        GraphDialog *mdiChild = qobject_cast<GraphDialog *>(windows.at(i)->widget());
        if ( mdiChild && QString(mdiChild->mainModuleName().c_str()) == moduleName)
            return windows.at(i);
    }
    return 0;
}


QMdiSubWindow *TVisorImp::findNewSystem()
{
    foreach (QMdiSubWindow *window, mdiArea->subWindowList())
    {
        NewSystemDialog *mdiChild = qobject_cast<NewSystemDialog *>(window->widget());
        if ( mdiChild )
            return window;
    }
    return 0;
}

void TVisorImp::setActiveSubWindow(QWidget *window)
{
    if (!window)
        return;
    mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
}

void TVisorImp::setActiveSubWindowIdex( int ndx )
{
   if ( ndx >=0 && ndx < mdiArea->subWindowList().size() )
       mdiArea->setActiveSubWindow(mdiArea->subWindowList().at(ndx));
}

void TVisorImp::openMdiChild( QWidget *p, bool showMaximized  )
{
  mdiArea->addSubWindow(p);
  pModuleName->addItem( nameMdiChild( p ).c_str());
  if( showMaximized )
   p->showMaximized();
  else
   p->show();

  if( mdiArea->activeSubWindow() )
  {
    QIcon icon = iconMdiChild( p );
    mdiArea->activeSubWindow()->setWindowIcon(icon);
  }
}

int TVisorImp::indexMdiChild( QWidget *p )
{
    QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
    for (int i = 0; i < windows.size(); i++)
    {
        if( windows.at(i)->widget() == p )
        {
          return i;
        }
    }
    return -1;
}

void TVisorImp::closeMdiChild( QWidget *p )
{
   //pModuleName->removeItem( nameMdiChild( p ).c_str() );
    int ndx = indexMdiChild( p );
    if( ndx >= 0 )
    {
      pModuleName->removeItem(ndx);
      mdiArea->removeSubWindow(p);
    }
}

// mode of calculation (System)
void TVisorImp::CmCalcMode()
{
  int newProfileMode;

  newProfileMode = MDD_SYSTEM;

  if( newProfileMode == pVisor->ProfileMode )
  {
    action_calcMode->setChecked(true);
    return;
  }
  // close all windows
  mdiArea->closeAllSubWindows();
  if( mdiArea->subWindowList().count()> 0 )
  {
      // cancel closing window
      action_calcMode->setChecked( false );
      return;
  }
  if( !SetProfileMode())
     action_calcMode->setChecked( false );

  moveToolBar();
}


// mode of work with DataBase
void TVisorImp::CmDataBaseMode()
{
  int newProfileMode;

  newProfileMode = MDD_DATABASE;

  if( newProfileMode == pVisor->ProfileMode )
  {
    actionDataBaseMode->setChecked(true);
    return;
  }

  // close all windows
  mdiArea->closeAllSubWindows();
  if( mdiArea->subWindowList().count()> 0 )
  {
      // cancel closing window
      actionDataBaseMode->setChecked( false );
      return;
  }
  SetGeneralMode();
  moveToolBar();
}


// Calc part functions
bool TVisorImp::SetProfileMode(const char * profileKey )
{
    try
    {
        pVisor->ProfileMode = MDD_SYSTEM;
        if( !TProfil::pm->initCalcMode(profileKey) )
        {
            pVisor->ProfileMode = MDD_DATABASE;
            return false;
        }
    }
    catch( TError& xcpt )
    {
        pVisor->ProfileMode = MDD_DATABASE;
        TProfil::pm->dyn_kill();
        TProfil::pm->set_def();
        vfMessage(window(), xcpt.title, xcpt.mess);
        return false;
    }
    catch(...)
    {
        pVisor->ProfileMode = MDD_DATABASE;
        TProfil::pm->dyn_kill();
        TProfil::pm->set_def();
        throw;  // go on
    }
    toolDataBase->hide();
    toolProject->show();
    //action_calcMode->setIcon(iconDatabase);
    //action_calcMode->setToolTip("Go to Database Management");
    //pModeName->setText(" E");
    actionDataBaseMode->setChecked(false);
    setActionPrecise();
    CmSysEq();
    return true;
}

bool TVisorImp::SetGeneralMode()
{
    TProfil::pm->deleteAutoGenerated();
    TProfil::pm->dyn_kill();
    TProfil::pm->set_def();
    pVisor->ProfileMode = MDD_DATABASE;
    toolDataBase->show();
    toolProject->hide();
    //action_calcMode->setIcon(iconSystem);
    //action_calcMode->setToolTip("Go to Calculation of Equilibria");
    //pModeName->setText(" D");
    action_calcMode->setChecked(false);
    return true;
}


//=======================================================================
// service functions from TVisorImp (visor_w.cpp)

void TVisorImp::setCellFont(const QFont& newCellFont)
{
    CellFont = newCellFont;
    QFontMetrics fm(CellFont);
    charWidth = fm.width("5");
    charHeight = fm.height();
}

/*!
   Opens html <file> positioned on anchor <item> (as in "file:/myfile.html#item")

   NOTE: we should not specify parent parameter for modeless dialog
   because otherwise even that it allows work with parent window it will
   be always overlapping it. For modal windows (and thus modal help) we need parent
*/

void TVisorImp::GetHelp( )
{
        (new HelpWindow(  0  ));
        //HelpWindow::pDia->show();
}

void TVisorImp::OpenHelp(const char* file, const char* item1, int page )
{
    if( HelpWindow::pDia )
    {
       if( item1 && page>=0 )
       {
          QString res = item1;
          res += QString("_%1").arg(page);
          gstring txt = res.toLatin1().data();
          HelpWindow::pDia->showDocumentation( file, txt.c_str() );
        }
        else
          HelpWindow::pDia->showDocumentation( file, item1 );

       HelpWindow::pDia->show();
       HelpWindow::pDia->raise();
    }
   // old help assistantClient->showDocumentation( file, item);
}

void TVisorImp::CmHelp2()
{
  TCModuleImp *actwin = activeMdiChild();
  if( actwin )
    aMod[actwin->rtNum()].CmHelp2();
  else
  {
     NewSystemDialog *wn = activeNewSystem();
     if( wn )
          wn->CmHelp2();
     else
        OpenHelp( GEMS_HOWTOSTART_HTML );
        /*if(pVisor->ProfileMode == MDD_DATABASE)
        {
          HowToStartDialog dlg(this);
          dlg.exec();
        }*/
  }
}

const char* TVisorImp::version()
{
    return GEMS_VERSION_STAMP;
}

const char* TVisorImp::getGEMTitle()
{
    return GEMS_VERSION_STAMP;
}


void TVisorImp::Update(bool force)
{
    if( pThread != QThread::currentThreadId() )
    {
       // (1)NewThread *thread = dynamic_cast<NewThread*>(QThread::currentThread());
       //if( thread )
       //		thread->emitUpdate(force);
       // (2)
        QMetaObject::invokeMethod(this, "Update",  Qt::QueuedConnection,
                                   Q_ARG( bool, force ));
        // (3)
        // QMetaObject::invokeMethod(ProcessProgressDialog::pDia, "slUpdate",
        //	   Qt::QueuedConnection, Q_ARG( bool, force ));
       return;
    }

    if( ProgressDialog::pDia )
        ProgressDialog::pDia->Update(force);

    if( ProcessProgressDialog::pDia )
        ProcessProgressDialog::pDia->Update();

    if( NewSystemDialog::pDia )
        NewSystemDialog::pDia->Update();

    for( uint ii=0; ii<aMod.GetCount(); ii++ )
        aMod[ii].Update(force);

    int nrt = nRTofActiveSubWindow();
    if( nrt>=0 )
       pLine->setText(tr (rt[nrt].PackKey()));
}


/*void TVisorImp::SaveSystem()
{
    TSysEq::pm->CmSave();
}*/

void TVisorImp::CalcMulti()
{
#ifdef Use_mt_mode
    TProfil::pm->userCancel = false;

    try
    {
        ProgressDialog* dlg = new ProgressDialog(/*window()*/this, false, true);
        //dlg->exec();
        dlg->show();
    }
    catch( TError& xcpt )
    {
        vfMessage(this, xcpt.title, xcpt.mess);
    }
#else
    TProfil::pm->fStopCalc = false;
    try
    {
        showMss = 1L;
        TProfil::pm->CalcEqstat();
    }
    catch( TError& xcpt )
    {
        vfMessage(this, xcpt.title, xcpt.mess);
    }
#endif
}

// Service window functions
void TVisorImp::OpenProgress( bool step )
{
    if( ProgressDialog::pDia )
        ProgressDialog::pDia->raise();
    else
        (new ProgressDialog(NewSystemDialog::pDia, step))->show();
}

void TVisorImp::CloseProgress()
{
    if( ProgressDialog::pDia )
       ProgressDialog::pDia->CmClose();
}

bool TVisorImp::Message( QWidget* parent, const char* name,
             const char* msg, int prog, int total, bool move )
{
     if( LoadMessage::pDia )
     {

        if( LoadMessage::pDia->wasCanceled() )
        {
          LoadMessage::pDia->setValue(LoadMessage::pDia->maximum());
          return true;
        }
        LoadMessage::pDia->Update(msg, prog, total);
     }
    else
    {
        //--QPushButton *btn = new QPushButton();
        LoadMessage* mssg = new LoadMessage(0/* parent*/, name, msg, prog, total);
        //--mssg->setCancelButton(btn);
        layout2->addWidget(mssg);
        qApp->processEvents();
        bool enabled = !(pVisor->ProfileMode == MDD_SYSTEM && LoadMessage::pDia );
        setMenuEnabled( enabled );
         connect( mssg, SIGNAL(canceled () ), this, SLOT(setMenuEnabled()) );

       // (new LoadMessage( parent, name, msg, prog, total))->show();
      //  if( move && parent  )
      //    LoadMessage::pDia->move(parent->x()+parent->width(), parent->y());
    }

   return false;
}

void TVisorImp::CloseMessage()
{
   if( LoadMessage::pDia )
    {
       LoadMessage::pDia->setValue(LoadMessage::pDia->maximum());
       LoadMessage::pDia->close();
   }
}

void TVisorImp::ProcessProgress( QWidget* parent, int nRT )
{
    TProcess::pm->userCancel = false;
    TGEM2MT::pm->userCancel = false;

    ProcessProgressDialog* dlg =
             new ProcessProgressDialog( 0/*parent*/, nRT );
    //   dlg->show();
    layout2->addWidget(dlg);
    bool enabled = !(pVisor->ProfileMode == MDD_SYSTEM );
    setMenuEnabled( enabled );
}

//=================================================================
// thread staff

void TVisorImp::theadService( int nFunction, QWidget* par )
{
       switch( nFunction  )
       {
        case thMessage:
          vfMessage( par, thdata.title, thdata.mess );
                       break;
        case thQuestion:
          thdata.res = vfQuestion( par, thdata.title, thdata.mess );
                       break;
        case thQuestion3:
          thdata.res = vfQuestion3( par, thdata.title, thdata.mess,
                                          thdata.s1, thdata.s2, thdata.s3 );
                       break;
        case thChoice:
          thdata.res = vfChoice( par, thdata.list,
                             thdata.title.c_str(), thdata.seli );
                       break;
        case thChoice2:
          thdata.res = vfChoice2( par, thdata.list, thdata.title.c_str(),
                              thdata.seli, thdata.all  );
                       break;
        case thExcludeFillEdit:
          thdata.res = vfExcludeFillEdit( par, thdata.title.c_str(),
             thdata.list, thdata.sel, thdata.fill_data );
                       break;
        default:
                       break;
        }
      thdata.wait = false;
      ThreadControl::wakeOne();
}


//QMutex updateMutex;
QMutex calcMutex;
QWaitCondition calcWait;
QWaitCondition progressWait;

QMutex& TVisorImp::getMutexCalc()
{
    return calcMutex;
}

QWaitCondition& TVisorImp::getWaitCalc()
{
//    return calcWait;
   return progressWait;
}

QWaitCondition& TVisorImp::getWaitProgress()
{
   return progressWait;
}

Qt::HANDLE pThread;

void ThreadControl::wakeOne()
{
    progressWait.wakeOne();
//    pVisorImp->getWaitProgress().wakeOne();
}

void ThreadControl::wakeAll()
{
   progressWait.wakeAll();
//   pVisorImp->getWaitProgress().wakeAll();
}

bool ThreadControl::wait()
{
    return progressWait.wait(&calcMutex);
//  return pVisorImp->getWaitProgress().wait(&pVisorImp->getMutexCalc());
}

bool ThreadControl::wait(unsigned long time )
{
    return progressWait.wait(&calcMutex);
    //return pVisorImp->getWaitProgress().wait(&pVisorImp->getMutexCalc(),time);
}

char* ThreadControl::GetPoint()
{
    return pVisorImp->getTCpoint();
}

void ThreadControl::SetPoint(const char* str )
{
    pVisorImp->setTCpoint( str );
}

//===================================================================

/*ModuleListItem mdList[15] =  {
  { RT_ICOMP, "Independent Components","Independent Components (IComp)" },
  { RT_DCOMP, "Dependent Components",  "Dependent Components (DComp)" },
  { RT_REACDC,"React Components" ,  "Dependent Components (ReacDC)" },
  { RT_RTPARM,"T/P Tabulations" ,   "T/P Tabulations and Plots (RTparm)" },
  { RT_PHASE, "Thermodynamic Phases",  " Thermodynamic Phases (Phase)" },
  { RT_COMPOS, "Predefined Compositions", "Predefined Compositions (Compos)" },

  { RT_SDATA, "Bibliography&Scripts", "Bibliography and Scripts (SDref)" },
  { RT_CONST, "Numerical Constants",  "Numerical Constants (Const)" },

  { RT_SYSEQ, "Chemical Systems", "Single Chemical Systems (SysEq)" },
  { RT_PROCES,"Process Simulators", "Process Simulators (Process)" },
  { RT_GEM2MT,"Mass Transport", "Coupled Fluid-Mass Transport (GEM2MT)" },
  { RT_GTDEMO,"Data Samplers",  "Data Samplers and Plotters (GtDemo)" },
  { RT_DUALTH,"Dual Thermodynamics", "Dual Thermodynamics (DualTh)" },
  { RT_UNSPACE, "Sensitivity Studies", "Sensitivity Studies (UnSpace)" },
  { RT_PARAM, "Project&Settings",      "Modelling Project (Project)" },
 };
*/
// working with MdeChild
/*
MdiChild *TVisorImp::activeMdiChild()
{
    if (QMdiSubWindow *activeSubWindow = mdiArea->activeSubWindow())
        return qobject_cast<MdiChild *>(activeSubWindow->widget());
    return 0;
}

QMdiSubWindow *TVisorImp::findMdiChild(const QString &moduleName)
{
    foreach (QMdiSubWindow *window, mdiArea->subWindowList())
    {
        MdiChild *mdiChild = qobject_cast<MdiChild *>(window->widget());
        if (mdiChild->currentName() == moduleName)
            return window;
    }
    return 0;
}

void TVisorImp::setActiveSubWindow(QWidget *window)
{
    if (!window)
        return;
    mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
}

void TVisorImp::openMdiChild( QTreeWidgetItem * item, int column )
{
  int nline = item->type();
  QMdiSubWindow *wn = findMdiChild( mdList[nline].modName );
  if( wn )
      setActiveSubWindow(wn);
  else
  { MdiChild *child = createMdiChild( nline );
    child->show();
  }
}

MdiChild *TVisorImp::createMdiChild( int nline )
{
    MdiChild *child = new MdiChild( mdList[nline] );
    mdiArea->addSubWindow(child);
    return child;
}
*/

/* column 1
    defineButtonsList();

    bModeGroup = new QButtonGroup;
    bModeGroup->setExclusive(true);
    QVBoxLayout *layout1 = new QVBoxLayout;
    layout1->setContentsMargins( 0, 0, 0, 0 );
    layout1->setSpacing(0);
    QWidget *itemWidget1 = new QWidget;

    dataBaseButton = new QToolButton(itemWidget1);
    dataBaseButton->setCheckable(true);
    dataBaseButton->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
    dataBaseButton->setIcon(QIcon(QPixmap(":/images/database_access.png")
                        .scaled(30, 30)));
    dataBaseButton->setIconSize(QSize(40, 40));
    dataBaseButton->setText("DataBase");
    dataBaseButton->setToolTip("Thermodynamic Database Management");
    bModeGroup->addButton(dataBaseButton, 0);
    layout1->addWidget(dataBaseButton);

    projectButton = new QToolButton(itemWidget1);
    projectButton->setCheckable(true);
    projectButton->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
    //projectButton->setIcon(QIcon(QPixmap(":/images/chemical-solutions-icon.png")
    //                    .scaled(40, 40)));
    projectButton->setIcon(QIcon(QPixmap(":/images/index1.jpeg")
                        .scaled(30, 30)));
    projectButton->setIconSize(QSize(40, 40));
    projectButton->setText("System");
    projectButton->setToolTip("Computation of Equilibria");
    bModeGroup->addButton(projectButton, 1);
    layout1->addWidget(projectButton);

    QSpacerItem *verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    layout1->addItem(verticalSpacer);

    itemWidget1->setLayout(layout1);
    itemWidget1->setFixedWidth( 50 );
    horizontalLayout->addWidget(itemWidget1);
    //spl->addWidget(itemWidget1);
    toolBar->setFixedWidth(itemWidget1->width());
*/

/* Define list of Modules for General and Project mode
void TVisorImp::defineButtonsList()
{
  int ii;
  QStringList lst;

  bModeGroup = new QButtonGroup;
  bModeGroup->setExclusive(true);
  QVBoxLayout *layout1 = new QVBoxLayout;
  layout1->setContentsMargins( 0, 0, 0, 0 );
  layout1->setSpacing(0);
  QWidget *itemWidget1 = new QWidget;

  // Thermodynamic Database
  for( ii=0; ii<6; ii++ )
  {
    //lst.clear();
    //lst << mdList[ii].name;
    //item = new  QTreeWidgetItem( thermoData,
    //             lst, *ii*mdList[ii].nRT);
    //item->setToolTip( 0, mdList[ii].toolTip);

    dataBaseButton = new QToolButton(*itemWidget1*);
    dataBaseButton->setCheckable(true);

    QFont fnt = dataBaseButton->font();
    fnt.setPointSize( 8 );
    dataBaseButton->setFont(fnt);
    QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Minimum);
    dataBaseButton->setSizePolicy(sizePolicy2);
    //dataBaseButton->setFixedWidth( 50 );
    //dataBaseButton->setContentsMargins( 0, 0, 0, 0 );

    dataBaseButton->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
    dataBaseButton->setIcon(QIcon(QPixmap(":/images/database_access.png")
                        .scaled(30, 30)));
    dataBaseButton->setIconSize(QSize(30, 30));


    dataBaseButton->setText(aMod[mdList[ii].nRT].GetName());
    dataBaseButton->setToolTip(mdList[ii].toolTip);
    bModeGroup->addButton(dataBaseButton, mdList[ii].nRT);
    layout1->addWidget(dataBaseButton);
  }

  // GEM Modeling Project
  for( ii=8; ii<15; ii++ )
  {
    //lst.clear();
    //lst << mdList[ii].name;
    //item = new  QTreeWidgetItem( projectData,
    //            lst, *ii*mdList[ii].nRT);
    //item->setToolTip( 0, mdList[ii].toolTip);
  }

  //  Other Data
  for( ii=6; ii<8; ii++ )
  {
    //lst.clear();
    //lst << mdList[ii].name;
    //item = new  QTreeWidgetItem( otherData,
    //             lst, *ii*mdList[ii].nRT);
    //item->setToolTip(0, mdList[ii].toolTip);
  }

  QSpacerItem *verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
  layout1->addItem(verticalSpacer);
  itemWidget1->setLayout(layout1);
  itemWidget1->setFixedWidth( 60 );
  //QScrollArea *scroll = new QScrollArea;
  //scroll->setWidget(itemWidget1);
  //scroll->setFixedWidth( 70 );
  //horizontalLayout->addWidget(scroll);
  //toolBar->setFixedWidth(scroll->width());

  horizontalLayout->addWidget(itemWidget1);
  toolBar->setFixedWidth(itemWidget1->width());

}*/

// -------------- End of file GemsMainWindow.cpp ----------------------


