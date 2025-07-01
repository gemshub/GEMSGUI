//-------------------------------------------------------------------
// $Id: HelpWindow.cpp 1949 2007-12-13 13:23:32Z gems $
//
// Implementation of HelpWindow class
//
// Copyright (C) 2010,2021 S.Dmytriyeva
//
// This file is part of the GEM-Selektor GUI library which uses the
// Qt v.5 cross-platform App & UI framework (http://qt.io/download-open-source)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the GPL v.3 license
//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------


#include <QtCore>
#include <QtWidgets>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
#include <QtHelp/QHelpLink>
#include <QtPrintSupport/QPrintDialog>
#include <QtPrintSupport/QPrinter>
#else
#include <QtGui>
#include <QPrintDialog>
#include <QPrinter>
#endif
#include <QtHelp/QHelpEngine>
#include <QtHelp/QHelpContentWidget>
#include <QtHelp/QHelpIndexWidget>
#include <QFileInfo>
#include <QMessageBox>
#include <QtHelp/QHelpSearchEngine>
#include <QtHelp/QHelpSearchQueryWidget>
#include <QtHelp/QHelpSearchResultWidget>

#include "ui_HelpWindow4.h"
#include "HelpWindow.h"
#include "GemsMainWindow.h"
#include "visor.h"
#include "service.h"
#include "gemsgui_version.h"

static const char *GEMS_HOWHELP_HTML = "gems_miscel.html#HOWHELP";
const char *GEMS_ABOUT_HTML = "gems_about.html#PAGE_ABOUT";

//extern const std::string _GEMIPM_version_stamp;

HelpWindow* HelpWindow::pDia = nullptr;

HelpWindow::HelpWindow( QWidget* parent):
    QMainWindow( parent ),
    ui(new Ui::HelpWindowData)
{
    ui->setupUi(this);
    string titl = pVisorImp->getGEMTitle();
    titl+= " : Help Viewer ";
    setWindowTitle(  titl.c_str()  );

    pDia = this;

    QSplitter *spl = new QSplitter(Qt::Horizontal,ui->centralwidget );
    //gridLayout->addWidget(spl);

    // from ui form
    tab_ = new QTabWidget(ui->centralwidget);
    tab_->setObjectName(QString::fromUtf8("tab_"));

    tabContents = new QWidget();
    tabContents->setObjectName(QString::fromUtf8("tabContents"));
    tab_->addTab(tabContents, QString());

    tabIndex = new QWidget();
    tabIndex->setObjectName(QString::fromUtf8("tabIndex"));
    verticalLayout = new QVBoxLayout(tabIndex);
    verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
    label = new QLabel(tabIndex);
    label->setObjectName(QString::fromUtf8("label"));
    verticalLayout->addWidget(label);
    lineIndex = new QLineEdit(tabIndex);
    lineIndex->setObjectName(QString::fromUtf8("lineIndex"));
    verticalLayout->addWidget(lineIndex);
    tab_->addTab(tabIndex, QString());

    tabSearch = new QWidget();
    tabSearch->setObjectName(QString::fromUtf8("tabSearch"));
    verticalLayout_2 = new QVBoxLayout(tabSearch);
    verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
    tab_->addTab(tabSearch, QString());

    ui->gridLayout->addWidget(spl/*tab_*/, 0, 0, 1, 1);
    spl->addWidget(tab_);
    tab_->setCurrentIndex(0);

#if QT_VERSION >= 0x050000
    tab_->setTabText(tab_->indexOf(tabContents),  "Contents");
    label->setText("TextLabel");
    tab_->setTabText(tab_->indexOf(tabIndex), "Index");
    tab_->setTabText(tab_->indexOf(tabSearch),  "Search");
#else
    tab_->setTabText(tab_->indexOf(tabContents), QApplication::translate("HelpWindowData", "Contents", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("HelpWindowData", "TextLabel", 0, QApplication::UnicodeUTF8));
    tab_->setTabText(tab_->indexOf(tabIndex), QApplication::translate("HelpWindowData", "Index", 0, QApplication::UnicodeUTF8));
    tab_->setTabText(tab_->indexOf(tabSearch), QApplication::translate("HelpWindowData", "Search", 0, QApplication::UnicodeUTF8));
#endif

    //End Ui form

    QString collectionFile = QString( pVisor->docDir().c_str() )+ QLatin1String("gems3help.qhc");
    // "/home/gems/gemworks/gems3/shared/doc/html/gems3help.qhc";

    findLine = nullptr;

#ifndef GEMS_RELEASE  
    QLabel *label_2 = new QLabel(ui->toolAddress);
    label_2->setText( "Address:" );
    ui->toolAddress->addWidget( label_2 );

    adressLine = new QLineEdit( ui->toolAddress );
    adressLine->setEnabled( true );
    adressLine->setFocusPolicy( Qt::ClickFocus );
    adressLine->setReadOnly( true );
    adressLine->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
    ui->toolAddress->addWidget( adressLine );
#endif

    gui_logger->info("HelpWindow collectionFile= {}", collectionFile.toStdString());

    hEngine = new QHelpEngine(collectionFile, this);
    if (!hEngine->setupData()) {

        gui_logger->error("Error QHelpEngine {}", hEngine->error().toStdString());
        delete hEngine;
        hEngine = nullptr;
        srchWidget =nullptr;
        wIndex =nullptr;
        wContents=nullptr;
    }
    else
    {
        gui_logger->info("HelpWindow collectionFile= {}", collectionFile.toStdString());

        // Contents part
        wContents = hEngine->contentWidget();
        QVBoxLayout* mainBox = new QVBoxLayout(tabContents );
        mainBox->addWidget(wContents);
        connect(wContents, SIGNAL(linkActivated(QUrl)),
                this, SLOT(loadResource(QUrl)));

        //Index define part
        wIndex = hEngine->indexWidget();
        verticalLayout->addWidget(wIndex);
        connect(lineIndex, SIGNAL(textChanged(QString)), this,
                SLOT(filterIndices(QString)));
        connect(wIndex, SIGNAL(linkActivated(QUrl,QString)),
                this, SLOT(loadResource(QUrl)));

        // Search part
        srchWidget = new  SearchWidget(hEngine->searchEngine(), tabSearch );
        verticalLayout_2->addWidget(srchWidget);
        hEngine->searchEngine()->reindexDocumentation();

        connect(srchWidget, SIGNAL(requestShowLink(QUrl)), this,
                SLOT(loadResource(QUrl)));

    }

    wBrowser =new HelpBrowser(hEngine,ui->centralwidget);
    spl->addWidget(wBrowser);
    //  gridLayout->addWidget(wBrowser, 0, 1, 1, 1);

    QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Expanding);
    wBrowser->setSizePolicy(sizePolicy2);
    QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
    tab_->setSizePolicy(sizePolicy1);
    spl->setStretchFactor(1,2);
    QList<int> lst;
    lst.append(300);
    lst.append(800);
    spl->setSizes(lst);
    //tab_->setMaximumWidth(350);
    //tab_->setMinimumWidth(200);
    //wBrowser->setOpenExternalLinks(true);

    setActions();
}

void HelpWindow::setActions()
{
    QObject::connect(ui->action_Quit, SIGNAL(triggered()), this, SLOT(close()));

    connect(wBrowser, SIGNAL(sourceChanged(QUrl)),
            this, SLOT(showAddres(QUrl)));

    connect( ui->actionHome, SIGNAL( triggered()), wBrowser, SLOT(home()));
    connect( ui->actionBack, SIGNAL( triggered()), wBrowser, SLOT(backward()));
    connect( ui->actionForward, SIGNAL( triggered()), wBrowser, SLOT(forward()));
    connect( ui->actionSync_with_Contents, SIGNAL( triggered()), this, SLOT(syncContents()));

    connect( ui->action_About, SIGNAL( triggered()), this, SLOT(helpAbout()));
    connect( ui->actionVersion, SIGNAL( triggered()), this, SLOT(helpVersion()));
    connect( ui->actionHelp_on_Help, SIGNAL( triggered()), this, SLOT(helpOnHelp()));
    connect( ui->action_Print, SIGNAL( triggered()), this, SLOT(helpPrint()));

    connect( ui->action_Find, SIGNAL( triggered()), this, SLOT(actionFind()));
    connect( ui->actionFind_Next, SIGNAL( triggered()), this, SLOT(actionFindNext()));
    connect( ui->actionFind_Previous, SIGNAL( triggered()), this, SLOT(actionFindPrevious()));

    connect( ui->actionZoom_In, SIGNAL( triggered()), this, SLOT(actionZoomIn()));
    connect( ui->actionZoom_Out, SIGNAL( triggered()), this, SLOT(actionZoomOut()));

    connect( wBrowser, SIGNAL( forwardAvailable(bool)), ui->actionForward, SLOT(setEnabled(bool)));
    connect( wBrowser, SIGNAL( backwardAvailable(bool)), ui->actionBack, SLOT(setEnabled(bool)));

    if( hEngine )
        connect(hEngine->searchEngine(), SIGNAL(searchingStarted()),
                this, SLOT(showFind()));

    QLabel *label_2 = new QLabel(ui->toolFind);
    label_2->setText( "Find:" );
    ui->toolFind->addWidget( label_2 );

    findLine = new QLineEdit( ui->toolFind );
    findLine->setEnabled( true );
    findLine->setFocusPolicy( Qt::ClickFocus );
    ui->toolFind->addWidget( findLine );
    ui->toolFind->addAction(ui->actionFind_Previous);
    ui->toolFind->addAction(ui->actionFind_Next);

    //    ui->actionFind_Next->setEnabled(true);
    //    ui->actionFind_Previous->setEnabled(true);
}


HelpWindow::~HelpWindow()
{
    delete ui;
}

void HelpWindow::helpVersion()
{
    QMessageBox::information( this,   QString("GEMSGUI v.") + GEMSGUI_VERSION,
                                 QString("\nThis is GEM-Selektor code package\n\n") +
                                 QString::fromStdString(_GEMS_version_stamp) + "\n\n  using \n\n"  +
                                 QString::fromStdString(_GEMIPM_version_stamp) + "\n\nand  Qt6 framework v." + GEMSGUI_QT_VERSION +
                                 "\n\n   OS " + GEMSGUI_OSX  + " compiler  " + GEMSGUI_COMPILER_ID + " " + GEMSGUI_COMPILER_VERSION +
                                 "\n\nFor GEMS R&D community, GPL v.3\n\n"
                                 "(c) 2025,  GEMS Development Team\n\n"
                                 "          Paul Scherrer Institute" );
}

void HelpWindow::helpAbout()
{
    showDocumentation( GEMS_ABOUT_HTML, nullptr );
}

void HelpWindow::helpOnHelp()
{
    showDocumentation( GEMS_HOWHELP_HTML, nullptr );
}

void HelpWindow::helpPrint()
{
    QPrinter printer;

    QPrintDialog dlg(  &printer, this );
    dlg.setWindowTitle(tr("Print GEMS3 Help Page"));
    if (wBrowser->textCursor().hasSelection() )
        dlg.setOption(QAbstractPrintDialog::PrintSelection);
    if( dlg.exec() )
        wBrowser->print( &printer ) ;
}

void HelpWindow::loadResource( QUrl name )
{
    wBrowser->setSource( name );
}

void HelpWindow::showAddres(QUrl name)
{
#ifndef GEMS_RELEASE
    adressLine->setText(name.toString());
#endif
}

void HelpWindow::showFind()
{
    if( hEngine )
    {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
        auto query = hEngine->searchEngine()->queryWidget()->searchInput();
        findLine->setText(query);
#else
        QStringList query = hEngine->searchEngine()->queryWidget()->query().value(0).wordList;
        findLine->setText(query.value(0));
#endif
    }
}

void HelpWindow::actionZoomIn()
{
    wBrowser->zoomIn(2);
}

void HelpWindow::actionZoomOut()
{
    wBrowser->zoomOut(2);
}

void HelpWindow::syncContents()
{
    QUrl url(wBrowser->source()); // adressLine->text()
    QModelIndex idx = wContents->indexOf( url );

    if(idx.isValid())
        wContents->setCurrentIndex(idx);
    tab_->setCurrentIndex(0);

}

void HelpWindow::actionFind()
{
    if( !findLine )
    {
        QLabel *label_2 = new QLabel(ui->toolFind);
        label_2->setText("Find for:");
        ui->toolFind->addWidget( label_2 );

        findLine = new QLineEdit( ui->toolFind );
        findLine->setEnabled( true );
        findLine->setFocusPolicy( Qt::ClickFocus );
        ui->toolFind->addWidget( findLine );
        ui->actionFind_Next->setEnabled(true);
        ui->actionFind_Previous->setEnabled(true);
    }
    else
    {
        actionFindNext();
    }

}

void HelpWindow::actionFindNext()
{
    if( !findLine )
        return;

    QTextDocument::FindFlags flg;
    if(ui->action_Case_sensetiv->isChecked() )
        flg |=QTextDocument::FindCaseSensitively;

    if(ui->action_Whole_words->isChecked() )
        flg |=QTextDocument::FindWholeWords;

    wBrowser->find( findLine->text(), flg );
}

void HelpWindow::actionFindPrevious()
{
    if( !findLine )
        return;

    QTextDocument::FindFlags flg = QTextDocument::FindBackward;
    if(ui->action_Case_sensetiv->isChecked() )
        flg |=QTextDocument::FindCaseSensitively;

    if(ui->action_Whole_words->isChecked() )
        flg |=QTextDocument::FindWholeWords;

    wBrowser->find( findLine->text(), flg );
}

void HelpWindow::filterIndices(const QString &filter)
{
    if (filter.contains(QLatin1Char('*')))
        wIndex->filterIndices(filter, filter);
    else
        wIndex->filterIndices(filter, QString());
}

void HelpWindow::showDocumentation(const char* file, const char* item1)
{
    if (!hEngine)
        return;

    string path = "qthelp://gems3/help/";
    QUrl path_str;

    if( !file/*item1*/ )
    {
        path_str = showHelpForKeyword( item1 );
    }
    else
    {
        path += file;
        // adding ".html" if needed
        if( path.rfind( "#" ) == path.npos )
        {   if( string(path, path.length()-5, 5) != ".html" )
                path += ".html";
            if( item1  )
            {  path += "#";
                path +=item1;
            }
        }
        path_str = QUrl(path.c_str());
    }

    gui_logger->debug("HelpWindow showDocumentation= {}", path_str.toString().toStdString());
    loadResource(  QUrl(path_str) );
}

// searches for reference <keyword> in the indexModel
//    returns  QUrl for sourse if  match is found
//    or empty Url
//    <keyword> should be in format:
//    objectlabel or objectlabel_<index>
QUrl HelpWindow::showHelpForKeyword(const QString &keyword)
{
    if (!hEngine)
        return QUrl();

#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
    // finding full name, not subString
    //QMap<QString, QUrl> links = hEngine->indexModel()->linksForKeyword( keyword );
    auto links = hEngine->documentsForKeyword(keyword);
    if (links.count())
        return links.constBegin()->url;

    QString kwInternal = keyword;
    int ndx = kwInternal.lastIndexOf('_');
    if(ndx > -1)
    {
      kwInternal= kwInternal.remove(QRegularExpression("_[0-9]{1,3}")/*ndx*/);
        // links = hEngine->indexModel()->linksForKeyword( kwInternal );
        links = hEngine->documentsForKeyword( kwInternal );
        if (links.count())
            return links.constBegin()->url;
    }
#else
    // finding full name, not subString
    QMap<QString, QUrl> links = hEngine->indexModel()->linksForKeyword( keyword );
    if (links.count())
        return links.constBegin().value();

    QString kwInternal = keyword;
    int ndx = kwInternal.lastIndexOf('_');
    if(ndx > -1)
    {    kwInternal= kwInternal.remove(QRegExp("_[0-9]{1,3}")/*ndx*/);
        links = hEngine->indexModel()->linksForKeyword( kwInternal );
        if (links.count())
            return links.constBegin().value();
    }

#endif


    //for old keywd list
    //    "objectlabel[indexN][indexM]", if sizeN >1 and sizeM > 1
    //    "objectlabel[][indexM]", if sizeN > 1 and sizeM > 1
    //    "objectlabel[index]", if sizeN >1 or sizeM > 1
    //    "objectlabel", if sizeN == 1 and sizeM == 1
    /*QStringList list1 = keyword.split("[");
    QString kwInternal;
    if(list1.count() > 2 )
    {
        // include line such: "keywd[][m]"
        kwInternal = list1[0] + "[]["+list1[2];
        links = hEngine->indexModel()->linksForKeyword( kwInternal );
            if (links.count())
              return links.constBegin().value();
        // include line such: "keywd[n]"
        kwInternal = list1[0]+"["+list1[1];
        links = hEngine->indexModel()->linksForKeyword( kwInternal );
            if (links.count())
              return links.constBegin().value();
    }
    // include line such: "keywd"
    kwInternal = list1[0];
    links = hEngine->indexModel()->linksForKeyword( kwInternal );
        if (links.count())
          return links.constBegin().value();*/
    return QUrl();
}


/*void HelpWindow::closeEvent(QCloseEvent* ev)
{
      ev->ignore();
      hide();
      //pDia = 0;
}*/

//---------------------------------------------------------------

HelpBrowser::HelpBrowser(QHelpEngine *hEngine, QWidget *parent)
    : QTextBrowser(parent),m_Engine( hEngine )
{

}

QVariant HelpBrowser::loadResource(int type, const QUrl &name)
{
    if( name.toString().indexOf("qthelp") < 0 )
        return QTextBrowser::loadResource( type, name);

    QByteArray ba;
    if (type < 4 && m_Engine) {
        QUrl url(name);
        if (name.isRelative())
            url = source().resolved(url);
        ba = m_Engine->fileData(url);
    }
    return ba;
}

//---------------------------------------------------------------

SearchWidget::SearchWidget(QHelpSearchEngine *engine, QWidget *parent)
    : QWidget(parent), srchEngine(engine)
{
    QVBoxLayout *vLayout = new QVBoxLayout(this);

    rsltWidget = srchEngine->resultWidget();
    QHelpSearchQueryWidget *qWidget = srchEngine->queryWidget();

    vLayout->addWidget(qWidget);
    vLayout->addWidget(rsltWidget);

    connect(srchEngine, SIGNAL(searchingStarted()), this,
            SLOT(searchingStarted()));
    connect(srchEngine, SIGNAL(searchingFinished(int)), this,
            SLOT(searchingFinished(int)));
    connect(qWidget, SIGNAL(search()), this, SLOT(search()));
    connect(rsltWidget, SIGNAL(requestShowLink(QUrl)), this,
            SIGNAL(requestShowLink(QUrl)));

}

SearchWidget::~SearchWidget()
{

}

void SearchWidget::search() const
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
    auto query = srchEngine->queryWidget()->searchInput();
#else
    QList<QHelpSearchQuery> query = srchEngine->queryWidget()->query();
#endif
    srchEngine->search(query);
}

void SearchWidget::searchingStarted()
{
    qApp->setOverrideCursor(QCursor(Qt::WaitCursor));
}

void SearchWidget::searchingFinished(int hits)
{
    Q_UNUSED(hits)
    qApp->restoreOverrideCursor();
}

//----------------------------------------------------------------
// Help file entry point constants

const char *WZSTEP = "WZSTEP";
const char *MWPAGE = "MWPAGE";
const char *DBM = "DBM";
const char *NUMSET = "NUMSET";

// modules
const char *GM_COMPOS_WZ_HTML = "gm_compos";
const char *GM_COMPOS_HTML = "gm_compos";
const char *GM_ICOMP_HTML = "gm_icomp";          //m_icomp.cpp
const char *GM_DCOMP_HTML = "gm_dcomp";          //m_dcomp.cpp
const char *GM_DCOMP_WZ_HTML = "gm_dcomp";       //DCompWizard
const char *GM_REACDC_HTML = "gm_reacdc";        //m_reacdc.cpp
const char *GM_REACDC_WZ_HTML = "gm_reacdc";     //ReacDCWizard
const char *GM_RTPARM_HTML = "gm_rtparm";        //m_rtparm.cpp
const char *GM_RTPARM_WZ_HTML = "gm_rtparm";     //RTparmWizard
const char *GM_PHASE_HTML = "gm_phase";          //m_phase.cpp
const char *GM_PHASE_WZ_HTML = "gm_phase";       //PhaseWizard
const char *GM_SDREF_HTML = "gm_sdref";          //m_sdata.cpp
const char *GM_CONST_HTML = "gm_const";          //m_const.cpp
const char *GM_PROJECT_HTML = "gm_project";      //m_param.cpp
const char *GM_PROJECT_WZ_HTML = "gm_project";   //ProjectWizard
const char *GEMS_NUMSET_HTML = "gm_project";     //m_param.cpp
const char *GM_SYSEQ_HTML = "gm_syseq";          //m_syseq.cpp
const char *GEMS_SYSTEM_WZ_HTML = "gems_bcc";    //SystemWizard
const char *GM_PROCES_HTML = "gm_proces";        //m_proces.cpp
const char *GM_PROCES_WZ_HTML = "gm_proces";     //ProcesWizard
const char *GM_GTDEMO_HTML = "gm_gtdemo";        //m_gtdemo.cpp
const char *GM_GTDEMO_WZ_HTML = "gm_gtdemo";     //GtDemoWizard
const char *GM_GEM2MT_HTML = "gm_gem2mt";        //m_gem2mt.cpp
const char *GM_GEM2MT_WZ_HTML = "gm_gem2mt";     //GEM2MTWizard
const char *GM_UNSPACE_HTML = "gm_unspace";      //m_unspace.cpp
const char *GM_UNSPACE_WZ_HTML = "gm_unspace";   //UnSpaceWizard
const char *GM_DUALTH_HTML = "gm_dualth";        //m_dualth.cpp
const char *GM_DUALTH_WZ_HTML = "gm_dualth";     //DualThWizard

const char *GSM_SYSTEM_HTML = "gems_bcc";        //ms_system.cpp
const char *GSM_MTPARM_HTML = "gems_systd";      //ms_mtparm.cpp
const char *GSM_EQDEMO_HTML = "gems_sys";        //ms_calc.h
const char *GSM_MULTI_HTML =  "gems_ipm";        //ms_calc.h

// dialogs
//const char *GEMS_ABOUT_HTML = "gems_about";       // MainDialog
const char *GEMS_TUTOR_HTML = "gems_tutorial";    // MainDialog
const char *GEMS_SETUP_HTML = "gems_setup";       // SettingsDialog
const char *GEMS_TDBAS_HTML = "gems_tdbase";      // DBDialog
const char *GEMS_MISCL_HTML = "gems_miscel";      // DBAdvDialog
const char *GEMS_FORMT_HTML = "gems_miscel.html#FORMATS";   // DBAdvDialog
const char *GEMS_REKEY_HTML = "gems_rekey";       // KeyFilter
const char *GEMS_MPROJ_HTML = "gm_project.html#HOWTO";    // KeyProfile
const char *GEMS_MPROJ_LF_HTML = "gm_project.html#FILES"; // ListFilesDialog
const char *GEMS_ONESYS_HTML = "gems_sys";        // NewSystemDialog
const char *GEMS_HOWTO_HTML = "gems_sys.html#HOWTO";
const char *GEMS_BCC_HTML = "gems_bcc";
const char *GEMS_HOWTOB_HTML = "gems_bcc.html#HOWTO";
const char *GEMS_IPM_HTML = "gems_ipm";
const char *GEMS_RECIPE_HTML = "gems_bcc.html#RECIPE"; // InputSystemDialog
const char *GEMS_GRAPH_HTML = "gems_graph"; // GraphDialog
const char *GEMS_GRAPH_CUS_HTML = "gems_graph.html#CUSTOMIZE"; // LegendDialog
const char *GEMS_MISSING_HTML = "gems_bcc.html#MISSING";  // ExcludeFillDialog
const char *GEMS_MODES_HTML = "gm_project.html#MODES";   //EQDialog
const char *GEMS_ELEMENTS_HTML = "gems_elements"; // ElementsDialog
const char *GEMS_SETFILT_HTML = "gems_elements.html#FILTERS"; // SetFiltersDialog
const char *GEMS_APHSETUP_HTML = "gems_aphsetup"; // AutoPhaseDialog
const char *GEMS_TEMPL_HTML = "gems_miscel.html#TEMPLATES";  // ChangeKeyDialog
const char *GEMS_PHINFO_HTML = "gems_miscel.html#PHASEINFO";  // PhaseInfoDialog
const char *GEMS_WRIPMF_HTML = "gems_ipm.html#WRITEIPMFILES"; // LookupDialog
const char *GEMS_MARK_HTML = "gems_miscel.html#MARK"; // SelectDialog
const char *GEMS_SELECT_HTML = "gems_miscel.html#SELECT"; // KeyDialog
const char *GEMS_CALCUL_HTML = "gems_miscel.html#CALCULATOR";  // CalcDialog
// MainDialog
const char *GEMS_HOWTOSTART_HTML = "gems_about.html#PAGE_HOWTOSTART";
//const char *GEMS_ABOUT_HTML = "gems_about.html#PAGE_ABOUT";
const char *GEMS_AUTHORS_HTML = "gems_about.html#PAGE_AUTHORS";
const char *GEMS_THANKS_HTML = "gems_about.html#PAGE_THANKS";
const char *GEMS_LICENSE_HTML = "gems_about.html#PAGE_LICENSE";
// old
const char *GEMS_TOC_HTML = "gems_toc"; // default for modules

//-------- End of file HelpWindow.cpp ----------------------------


