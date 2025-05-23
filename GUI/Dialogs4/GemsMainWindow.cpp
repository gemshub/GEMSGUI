//-------------------------------------------------------------------
// $Id: module_w.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of GemsMainWindow class
//
// Copyright (C) 2011  S.Dmytriyeva
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

#include <QMdiArea>

#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif


#include "visor.h"
#include "HelpWindow.h"
#include "GemsMainWindow.h"
#include "NewSystemDialog.h"
#include "ui_GemsMainWindow4.h"

#include "LoadMessage.h"
#include "GraphDialogN.h"

//static const char* GEMS_LOGO_ICON = "Icons/gems1.png";
static const char* GEMS_VERSION_STAMP = "GEM-Selektor 3 (GEMS3)";
#ifndef _WIN32
#ifdef __APPLE__
static const char* GEMS_DEFAULT_FONT_NAME = "Monaco";
const int GEMS_DEFAULT_FONT_SIZE = 13;
#else
static const char* GEMS_DEFAULT_FONT_NAME = "Courier New";
const int GEMS_DEFAULT_FONT_SIZE = 11;
#endif
#else
static const char* GEMS_DEFAULT_FONT_NAME = "Courier New";
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
    QMainWindow(nullptr),
    ui(new Ui::GemsMainWindowData),
    argc(c),
    argv(v),
    last_update( 0 ),
    configAutosave(false),
    proc(nullptr),
    currentNrt(-2),
    settedCureentKeyIntotbKeys(false)
{
    ui->setupUi(this);
    GEMS3KGenerator::default_type_f = GEMS3KGenerator::f_thermofun;
    ui->action_Use_ThermoFUN->setChecked(GEMS3KGenerator::default_type_f>=GEMS3KGenerator::f_thermofun);

    (void)statusBar();
    //setMinimumSize( 300, 200 );

    // from visor_w.cpp (old TVisorImp)

    charWidth = 12;
    charHeight = 18;
    pVisorImp = this;

    updateTime = 10; // centiseconds

    defaultFont = QFont(GEMS_DEFAULT_FONT_NAME, GEMS_DEFAULT_FONT_SIZE);
    setCellFont( defaultFont );
    axisLabelFont.setPointSize(14); //11

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
    toolDataBase->setIconSize(QSize(48,48));
    toolDataBase->addAction(ui->actionIComp);
    toolDataBase->addAction(ui->actionDComp);
    toolDataBase->addAction(ui->actionReacDC);
    toolDataBase->addAction(ui->actionRTparm);
    toolDataBase->addAction(ui->actionPhase);
    toolDataBase->addAction(ui->actionCompos);
    toolDataBase->setWindowTitle("toolDataBase");

    toolProject = new QToolBar(this);
    toolProject->setObjectName(QString::fromUtf8("toolProject"));
    toolProject->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    toolProject->setMovable(false);
    this->addToolBar(Qt::LeftToolBarArea, toolProject);
    toolProject->setIconSize(QSize(48,48));
    toolProject->addAction(ui->actionSysEq);
    toolProject->addAction(ui->actionProcess);
    toolProject->addAction(ui->actionGtDemo);
    toolProject->addAction(ui->actionGEM2MT);
    //toolProject->addAction(actionDualTh);
    toolProject->addAction(ui->actionUnSpace);
    toolProject->addAction(ui->actionProject);
    toolProject->setWindowTitle("toolProject");


    // Define internal area
    splH = new QSplitter(Qt::Horizontal );
    splH->setChildrenCollapsible(false);
    //spl->setMidLineWidth(-3);
    //spl->setLineWidth(-3);
    ui->horizontalLayout->setContentsMargins( 0, 0, 0, 0 );
    ui->horizontalLayout->addWidget(splH);

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
    tbKeys->setFont( getCellFont() );

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
    mdiArea->setActivationOrder(QMdiArea::ActivationHistoryOrder);
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
    splH->setStretchFactor(0, 3);
    splH->setStretchFactor(1, 1);

    // define signal/slots
    connect( ui->action_Use_ThermoFUN, &QAction::toggled, [&](bool checked){
        if(checked) {
            GEMS3KGenerator::default_type_f = GEMS3KGenerator::f_thermofun;
        }
        else {
            GEMS3KGenerator::default_type_f = GEMS3KGenerator::f_json;
        }
    });
    connect( ui->action_calcMode, SIGNAL( triggered()), this, SLOT(CmCalcMode()));
    connect( ui->actionDataBaseMode, SIGNAL( triggered()), this, SLOT(CmDataBaseMode()));

    connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)),
            this, SLOT(updateMenus()));

    connect(splH, SIGNAL(splitterMoved(int,int)),
            this, SLOT(moveToolBar(int,int)));

    //connect(tbKeys, SIGNAL(cellDoubleClicked ( int , int )  ),
    //        this, SLOT(openRecordKey( int, int )));
    connect(tbKeys, SIGNAL(cellClicked(int,int)),
            this, SLOT(openRecordKey(int,int)));

    connect( pFilterKey, SIGNAL(editingFinished()), this, SLOT(changeKeyList()) );

    // Set up Menus and actions

    setActions();
    setCalcClient();

    // Use database mode as start mode
    //actionDataBaseMode->setChecked(true);

    if( MDD_DATABASE == pVisor->ProfileMode )
    {
        SetGeneralMode();
        ui->actionDataBaseMode->setChecked(true);
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
            ui->actionDataBaseMode->setChecked(true);
        }
        else
        {
            ui->action_calcMode->setChecked(true);
            // load last system
            if( rt[RT_SYSEQ]->Find( pVisor->lastSystemKey.c_str()) >= 0 )
                CmShow( pVisor->lastSystemKey.c_str() );
            //NewSystemDialog::pDia->CmSelect( pVisor->lastSystemKey.c_str());
        }
    }

#ifdef USE_GEMS3K_SERVER
    ui->sactionStepwise->setVisible(false);
    ui->sactionStepwise->setEnabled(false);
#endif

    updateMenus();
    //moveToolBar();
}

//   The desctructor
TVisorImp::~TVisorImp()
{
    calc_thread.quit();
    calc_thread.wait();

    // killGEMServer();
    delete pVisor;
    delete ui;
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
void TVisorImp::defineModuleKeysList( size_t nRT )
{
    size_t  kk, ln;
    int jj, ii, colsz;
    string keyfld;
    QTableWidgetItem *item, *curItem=nullptr;
    settedCureentKeyIntotbKeys = false;

    if( currentNrt != static_cast<int>(nRT) )
        return;

    string oldKey = rt[nRT]->UnpackKey();
    pFilterKey->setText( dynamic_cast<TCModule*>(aMod[nRT].get())->getFilter());

    // define tbKeys
    tbKeys->clear();
    tbKeys->setSortingEnabled ( false );
    tbKeys->setColumnCount( rt[nRT]->KeyNumFlds());


    // get list or record keys
    string keyFilter = pFilterKey->text().toStdString();
    TCIntArray temp, colSizes;
    TCStringArray keyList;
    int nKeys = rt[nRT]->GetKeyList( keyFilter.c_str(), keyList, temp);

    for(jj=0; jj<rt[nRT]->KeyNumFlds(); jj++)
        colSizes.push_back( 0/*rt[nRT]->FldLen(jj)*/ );

    // define key list
    tbKeys->setRowCount(nKeys);

    for( ii=0; ii<nKeys; ii++ )
    {
        tbKeys->setRowHeight(ii, htF(ftString, 0)+2);
        for(jj=0, kk=0; jj<rt[nRT]->KeyNumFlds(); jj++)
        {

            ln = rt[nRT]->FldLen(jj);
            keyfld = string(keyList[ii], kk, ln);
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
    for( jj=0; jj<rt[nRT]->KeyNumFlds(); jj++)
    {
        tbKeys->setColumnWidth(jj, wdF( ftString, colSizes[jj]+1, eNo ) );
        item = new QTableWidgetItem(tr("%1").arg( jj+1));
        item->setToolTip( dynamic_cast<TCModule*>(aMod[nRT].get())->GetFldHelp(jj));
        tbKeys->setHorizontalHeaderItem( jj, item );
    }

    tbKeys->setSortingEnabled ( true );
    if(curItem )
    {
        tbKeys->setCurrentItem( curItem );
        tbKeys->scrollToItem( curItem, QAbstractItemView::PositionAtCenter );
    }

    if( pVisor->ProfileMode && (nRT == RT_SYSEQ || nRT == RT_PROCES
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

    rt[nRT]->SetKey(oldKey.c_str());
}

// Change list of Modules for General or Project mode
void TVisorImp::changeModulesKeys( int nRT )
{

    if( nRT >=0 )
        pLine->setText(tr(rt[nRT]->PackKey()));
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
    {
        //pFilterKey->setText(dynamic_cast<TCModule *>(aMod[nRT].get())->getFilter());
        defineModuleKeysList( nRT );
    }
    // currentNrt = nRT;
}


void TVisorImp::openRecordKey( int row, int    )
{
    string currentKey ="";

    if( row >= tbKeys->rowCount())
        return;

    for(int jj=0; jj<tbKeys->columnCount(); jj++)
    {
        currentKey += tbKeys->item( row, jj)->text().toStdString();
        StripLine(currentKey);
        currentKey +=":";
    }

    CmShow( currentKey.c_str() );
}


void TVisorImp::changeKeyList()
{
    if( currentNrt >=0 )
    {
        string filter = pFilterKey->text().toStdString();
        dynamic_cast<TCModule*>(aMod[currentNrt].get())->setFilter(filter.c_str());
        defineModuleKeysList( currentNrt );
    }
}


// working with TCModuleImp
void TVisorImp::OpenModule(QWidget* /*par*/, uint irt, int page, int viewmode, bool /*select*/)
{
    try
    {
        if( irt==RT_SYSEQ && pVisor->ProfileMode == MDD_SYSTEM )
        {
            if( !NewSystemDialog::pDia )
            {
                (new NewSystemDialog(nullptr/*pVisorImp*/));
                //----if( select )
                //----   NewSystemDialog::pDia->CmSelect();
                openMdiChild( NewSystemDialog::pDia, true );

                // Create, if no syseq is present in the project
                if( rt[RT_SYSEQ]->RecCount() <= 0)
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
            TCModuleImp* p = aMod[irt]->pImp;
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
        {
            return child;
        }
        else
        { // find module for graphic window
            GraphDialog *dlg = qobject_cast<GraphDialog *>(activeSubWindow->widget());
            if( dlg )
            {
                QMdiSubWindow *mdi = findMdiChild(dlg->mainModuleName().c_str());
                if( mdi )
                {
                    return(qobject_cast<TCModuleImp *>(mdi->widget()));
                }
            }
        }
    }
    return nullptr;
}

NewSystemDialog *TVisorImp::activeNewSystem()
{
    if (QMdiSubWindow *activeSubWindow = mdiArea->activeSubWindow())
    {
        return qobject_cast<NewSystemDialog *>(activeSubWindow->widget());
    }
    return nullptr;
}

NewSystemDialog *TVisorImp::activeNewSystemCommand()
{
    if (QMdiSubWindow *activeSubWindow = mdiArea->activeSubWindow())
    {
        NewSystemDialog * systwindow = qobject_cast<NewSystemDialog *>(activeSubWindow->widget());
        if( systwindow )
        {
            return systwindow;
        }
        else
        {
            TCModuleImp *child = qobject_cast<TCModuleImp *>(activeSubWindow->widget());
            if( child && child->rtNum() >= MD_RMULTS)
            {
                QMdiSubWindow *win = findNewSystem();
                if( win )
                    return qobject_cast<NewSystemDialog *>(win->widget());
                else  // open   main NewSystemDialog
                {
                    OpenModule(this, RT_SYSEQ, 0, true, true);
                    activeSubWindow = mdiArea->activeSubWindow();
                    return qobject_cast<NewSystemDialog *>(activeSubWindow->widget());
                }
            }
        }
    }
    return nullptr;
}

QMdiSubWindow *TVisorImp::findMdiChild(const QString &moduleName)
{
    foreach (QMdiSubWindow *window, mdiArea->subWindowList())
    {
        TCModuleImp *mdiChild = qobject_cast<TCModuleImp *>(window->widget());
        if ( mdiChild && QString(mdiChild->moduleName().c_str()) == moduleName)
            return window;
    }
    return nullptr;
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
    return nullptr;
}


QMdiSubWindow *TVisorImp::findNewSystem()
{
    foreach (QMdiSubWindow *window, mdiArea->subWindowList())
    {
        NewSystemDialog *mdiChild = qobject_cast<NewSystemDialog *>(window->widget());
        if ( mdiChild )
            return window;
    }
    return nullptr;
}

void TVisorImp::setActiveSubWindow(QWidget *window)
{
    if (!window)
        return;
    mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
}

void TVisorImp::setActiveSubWindowName(const QString &name)
{
    QMdiSubWindow *mdi= nullptr;
    foreach (QMdiSubWindow *window, mdiArea->subWindowList())
    {
        TCModuleImp *mdiChild = qobject_cast<TCModuleImp *>(window->widget());
        if ( mdiChild && QString(mdiChild->moduleName().c_str()) == name) {
            mdi = window;
            break;
        }
        GraphDialog *mdiChildgr = qobject_cast<GraphDialog *>(window->widget());
        if ( mdiChildgr && QString(mdiChildgr->moduleName().c_str()) == name) {
            mdi = window;
            break;
        }
    }
    if(!mdi)
        mdi = findNewSystem();
    setActiveSubWindow(mdi);
}

void TVisorImp::openMdiChild( QWidget *p, bool showMaximized  )
{
    mdiArea->addSubWindow(p);
    disconnect( pModuleName, &QComboBox::currentTextChanged, this, &TVisorImp::setActiveSubWindowName);
    pModuleName->addItem(nameMdiChild( p ).c_str());
    connect( pModuleName, &QComboBox::currentTextChanged, this, &TVisorImp::setActiveSubWindowName);
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

void TVisorImp::closeMdiChild( QWidget *p )
{
    disconnect( pModuleName, &QComboBox::currentTextChanged, this, &TVisorImp::setActiveSubWindowName);
    pModuleName->removeItem( pModuleName->findText(nameMdiChild( p ).c_str()));
    connect( pModuleName, &QComboBox::currentTextChanged, this, &TVisorImp::setActiveSubWindowName);
}

// mode of calculation (System)
void TVisorImp::CmCalcMode()
{
    int newProfileMode;

    newProfileMode = MDD_SYSTEM;

    if( newProfileMode == pVisor->ProfileMode )
    {
        ui->action_calcMode->setChecked(true);
        return;
    }
    // close all windows
    mdiArea->closeAllSubWindows();
    if( mdiArea->subWindowList().count()> 0 )
    {
        // cancel closing window
        ui->action_calcMode->setChecked( false );
        return;
    }
    if( !SetProfileMode())
        ui->action_calcMode->setChecked( false );

    moveToolBar();
}


// mode of work with DataBase
void TVisorImp::CmDataBaseMode()
{
    int newProfileMode;

    newProfileMode = MDD_DATABASE;

    if( newProfileMode == pVisor->ProfileMode )
    {
        ui->actionDataBaseMode->setChecked(true);
        return;
    }

    // close all windows
    mdiArea->closeAllSubWindows();
    if( mdiArea->subWindowList().count()> 0 )
    {
        // cancel closing window
        ui->actionDataBaseMode->setChecked( false );
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
    ui->actionDataBaseMode->setChecked(false);
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
    ui->action_calcMode->setChecked(false);
    return true;
}


//=======================================================================
// service functions from TVisorImp (visor_w.cpp)

void TVisorImp::setCellFont(const QFont& newCellFont)
{
    CellFont = newCellFont;
    QFontMetrics fm(CellFont);
    charWidth = fm.horizontalAdvance("5");
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
    (new HelpWindow(  nullptr  ));
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
            string txt = res.toStdString();
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
        aMod[actwin->rtNum()]->CmHelp2();
    else
    {
        NewSystemDialog *wn = activeNewSystem();
        if( wn )
            wn->CmHelp2();
        else
            OpenHelp( GEMS_HOWTOSTART_HTML );
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

// -------------- End of file GemsMainWindow.cpp ----------------------


