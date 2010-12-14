//-------------------------------------------------------------------
// $Id: HelpWindow.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of HelpWindow class
//
// Copyright (C) 2010 S.Dmytriyeva
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

#include <QtHelp/QHelpEngine>
#include <QtHelp/QHelpContentWidget>
#include <QtHelp/QHelpIndexWidget>
#include <QFileInfo>
#include <QMessageBox>
#include <QPrintDialog>
#include <QPrinter>
#include <QtHelp/QHelpSearchEngine>
#include <QtHelp/QHelpSearchQueryWidget>
#include <QtHelp/QHelpSearchResultWidget>
//#include <QtCore/QLibraryInfo>
//#include <QtGui/QApplication>
#include <QtCore>
#include <QtGui>


#include "HelpWindow.h"
#include "visor_w.h"
#include "visor.h"


HelpWindow* HelpWindow::pDia = 0;

HelpWindow::HelpWindow( QWidget* parent):
        QMainWindow( parent )
{
   setupUi(this);
   gstring titl = pVisorImp->getGEMTitle();
           titl+= " : Help Viewer ";
           setWindowTitle( trUtf8(titl.c_str()) );

    pDia = this;

    QSplitter *spl = new QSplitter(Qt::Horizontal,centralwidget );
    //gridLayout->addWidget(spl);

    // from ui form
    tab_ = new QTabWidget(centralwidget);
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

    gridLayout->addWidget(spl/*tab_*/, 0, 0, 1, 1);
    spl->addWidget(tab_);
    tab_->setCurrentIndex(0);

    tab_->setTabText(tab_->indexOf(tabContents), QApplication::translate("HelpWindowData", "Contents", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("HelpWindowData", "TextLabel", 0, QApplication::UnicodeUTF8));
    tab_->setTabText(tab_->indexOf(tabIndex), QApplication::translate("HelpWindowData", "Index", 0, QApplication::UnicodeUTF8));
    tab_->setTabText(tab_->indexOf(tabSearch), QApplication::translate("HelpWindowData", "Search", 0, QApplication::UnicodeUTF8));

    //End Ui form


    QString collectionFile = QString( pVisor->docDir().c_str() )+ QLatin1String("gems3help.qhc");
 // "/home/gems/gemworks/gems3/shared/doc/html/gems3help.qhc";

    findLine = 0;

#ifndef GEMS_RELEASE  
    QLabel *label_2 = new QLabel(toolAddress);
    label_2->setText(trUtf8("Address:"));
    toolAddress->addWidget( label_2 );

    adressLine = new QLineEdit( toolAddress );
    adressLine->setEnabled( TRUE );
    adressLine->setFocusPolicy( Qt::ClickFocus );
    adressLine->setReadOnly( TRUE );
    adressLine->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
    toolAddress->addWidget( adressLine );
#endif  

    hEngine = new QHelpEngine(collectionFile, this);
    if (!hEngine->setupData()) {
        delete hEngine;
        hEngine = 0;
        srchWidget =0;
        wIndex =0;
        wContents=0;
    }
    else
     {
      //cout << collectionFile.toLatin1().data() << endl;

      // Contents part
      wContents = hEngine->contentWidget();
      QVBoxLayout* mainBox = new QVBoxLayout(tabContents );
      mainBox->addWidget(wContents);
      connect(wContents, SIGNAL(linkActivated ( const QUrl& ) ),
          this, SLOT(loadResource( const QUrl& )));

      //Index define part
      wIndex = hEngine->indexWidget();
      verticalLayout->addWidget(wIndex);
      connect(lineIndex, SIGNAL(textChanged(QString)), this,
              SLOT(filterIndices(QString)));
      connect(wIndex, SIGNAL(linkActivated( const QUrl &, const QString & ) ),
          this, SLOT(loadResource( const QUrl& )));

      // Search part
      srchWidget = new  SearchWidget(hEngine->searchEngine(), tabSearch );
      verticalLayout_2->addWidget(srchWidget);
      hEngine->searchEngine()->reindexDocumentation();

      connect(srchWidget, SIGNAL(requestShowLink(QUrl)), this,
          SLOT(loadResource(QUrl)));

  }

    wBrowser =new HelpBrowser(hEngine,centralwidget);
    spl->addWidget(wBrowser);
  //  gridLayout->addWidget(wBrowser, 0, 1, 1, 1);

    QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Expanding);
    wBrowser->setSizePolicy(sizePolicy2);
    QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
    tab_->setSizePolicy(sizePolicy1);
    spl->setStretchFactor(1,2);
    QList<int> lst;
    lst.append(200);
    lst.append(800);
    spl->setSizes(lst);
    //tab_->setMaximumWidth(350);
    //tab_->setMinimumWidth(200);

  setActions();
}

void HelpWindow::setActions()
{
    connect(wBrowser, SIGNAL(sourceChanged ( const QUrl & )   ),
        this, SLOT(showAddres( const QUrl& )));

    connect( actionHome, SIGNAL( triggered()), wBrowser, SLOT(home()));
    connect( actionBack, SIGNAL( triggered()), wBrowser, SLOT(backward()));
    connect( actionForward, SIGNAL( triggered()), wBrowser, SLOT(forward()));
    connect( actionSync_with_Contents, SIGNAL( triggered()), this, SLOT(syncContents()));

    connect( action_About, SIGNAL( triggered()), this, SLOT(helpAbout()));
    connect( actionVersion, SIGNAL( triggered()), this, SLOT(helpVersion()));
    connect( action_Print, SIGNAL( triggered()), this, SLOT(helpPrint()));

    connect( action_Find, SIGNAL( triggered()), this, SLOT(actionFind()));
    connect( actionFind_Next, SIGNAL( triggered()), this, SLOT(actionFindNext()));
    connect( actionFind_Previous, SIGNAL( triggered()), this, SLOT(actionFindPrevious()));

    connect( actionZoom_In, SIGNAL( triggered()), this, SLOT(actionZoomIn()));
    connect( actionZoom_Out, SIGNAL( triggered()), this, SLOT(actionZoomOut()));

    connect( wBrowser, SIGNAL( forwardAvailable(bool)), actionForward, SLOT(setEnabled(bool)));
    connect( wBrowser, SIGNAL( backwardAvailable(bool)), actionBack, SLOT(setEnabled(bool)));

    if( hEngine )
     connect(hEngine->searchEngine(), SIGNAL(searchingStarted()),
                this, SLOT(showFind()));

    QLabel *label_2 = new QLabel(toolFind);
    label_2->setText(trUtf8("Find:"));
    toolFind->addWidget( label_2 );

    findLine = new QLineEdit( toolFind );
    findLine->setEnabled( TRUE );
    findLine->setFocusPolicy( Qt::ClickFocus );
    toolFind->addWidget( findLine );
    toolFind->addAction(actionFind_Previous);
    toolFind->addAction(actionFind_Next);

//    actionFind_Next->setEnabled(true);
//    actionFind_Previous->setEnabled(true);
}


HelpWindow::~HelpWindow()
{}

void HelpWindow::languageChange()
{
    retranslateUi(this);
}

void HelpWindow::helpVersion()
{
    QMessageBox::information(this, trUtf8("About Gems3:"),
           trUtf8("The Simple Text Viewer enables the user to select and view existing\n"
                  "files.\n\n"
                  "HTML files is displayed using rich text, while other files are\n"
                  "presented as plain text. The application provides a file dialog\n"
                  "allowing the user to search for files using wildcard matching. The\n"
                  "search is performed within in the specified directory, and the user is\n"
                  "given an option to browse the existing file system to find the\n"
                  "relevant directory.") );

}

void HelpWindow::helpAbout()
{
    VersionDialog dlg;
    dlg.exec();
}

void HelpWindow::helpPrint()
{
 QPrinter printer;

  QPrintDialog dlg(  &printer, this );
  dlg.setWindowTitle(tr("Print Document"));
  if (wBrowser->textCursor().hasSelection() )
      dlg.addEnabledOption(QAbstractPrintDialog::PrintSelection);
  if( dlg.exec() )
    wBrowser->print( &printer ) ;
}

void HelpWindow::loadResource( const QUrl &name )
{
    wBrowser->setSource( name );
}

void HelpWindow::showAddres( const QUrl &name )
{
#ifndef GEMS_RELEASE
    adressLine->setText(name.toString());
#endif
}

void HelpWindow::showFind()
{
    if( hEngine )
    {
       QStringList query = hEngine->searchEngine()->queryWidget()->query().value(0).wordList;
       findLine->setText(query.value(0));
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
      QLabel *label_2 = new QLabel(toolFind);
      label_2->setText(trUtf8("Find for:"));
      toolFind->addWidget( label_2 );

      findLine = new QLineEdit( toolFind );
      findLine->setEnabled( TRUE );
      findLine->setFocusPolicy( Qt::ClickFocus );
      toolFind->addWidget( findLine );
      actionFind_Next->setEnabled(true);
      actionFind_Previous->setEnabled(true);
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

  QTextDocument::FindFlags flg = 0;
  if(action_Case_sensetiv->isChecked() )
       flg |=QTextDocument::FindCaseSensitively;

  if(action_Whole_words->isChecked() )
       flg |=QTextDocument::FindWholeWords;

  wBrowser->find( findLine->text(), flg );
}

void HelpWindow::actionFindPrevious()
{
  if( !findLine )
   return;

  QTextDocument::FindFlags flg = QTextDocument::FindBackward;
  if(action_Case_sensetiv->isChecked() )
       flg |=QTextDocument::FindCaseSensitively;

  if(action_Whole_words->isChecked() )
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

    gstring path = "qthelp://gems3/doc/";
    QUrl path_str;

    if( item1 )
    {
       path_str = showHelpForKeyword( item1 );
    }
    else
    {
        path += file;
        // adding ".html" if needed
        if( path.rfind( "#" ) == path.npos )
           if( gstring(path, path.length()-5, 5) != ".html" )
              path += ".html";
        path_str = QUrl(path.c_str());
    }

   cout << "showDocumentation " << path_str.toString().toLatin1().data() << endl;
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
    QList<QHelpSearchQuery> query = srchEngine->queryWidget()->query();
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

//---------------------------------------------------------------

VersionDialog::VersionDialog( QWidget* parent):
        QDialog( parent )
{
   setupUi(this);
   gstring titl = pVisorImp->getGEMTitle();
           titl+= " : Version Viewer ";
   setWindowTitle( trUtf8(titl.c_str()) );
}

VersionDialog::~VersionDialog()
{}

void VersionDialog::languageChange()
{
    retranslateUi(this);
}

//-------- End of file HelpWindow.cpp ----------------------------


