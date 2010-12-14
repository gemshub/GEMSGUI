//-------------------------------------------------------------------
// $Id: HelpWindow.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of HelpWindow class
//
// Copyright (C) 2010  A.Rysin
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

#ifndef HelpWindow_included
#define HelpWindow_included


#include <QMainWindow>
#include <QTextBrowser>

#include "ui_HelpWindow4.h"
#include "ui_VersionDialog4.h"

QT_BEGIN_NAMESPACE
class QHelpEngineCore;
class QHelpEngine;
class QHelpContentWidget;
class QHelpIndexWidget;
class QHelpSearchEngine;
class QHelpSearchResultWidget;
class QLabel;
QT_END_NAMESPACE;

class HelpBrowser : public QTextBrowser
{
    Q_OBJECT

public:
    HelpBrowser(QHelpEngine *hEngine, QWidget *parent);
//    void showHelpForKeyword(const QString &id);

private:
    QVariant loadResource(int type, const QUrl &name);

    QHelpEngine *m_Engine;
};

class SearchWidget : public QWidget
{
    Q_OBJECT

    QHelpSearchEngine *srchEngine;
    QHelpSearchResultWidget *rsltWidget;

   // bool eventFilter(QObject* o, QEvent *e);

public:
    SearchWidget(QHelpSearchEngine *engine, QWidget *parent = 0);
    ~SearchWidget();

signals:
    void requestShowLink(const QUrl &url);

private slots:
    void search() const;
    void searchingStarted();
    void searchingFinished(int hits);

};


class HelpWindow : public QMainWindow, public Ui::HelpWindowData
{
    Q_OBJECT

    QHelpEngine *hEngine;
    QHelpContentWidget *wContents;
    QHelpIndexWidget *wIndex;
    HelpBrowser *wBrowser;
    QLineEdit *adressLine;
    QLineEdit *findLine;
    SearchWidget* srchWidget;

    QTabWidget *tab_;
    QWidget *tabContents;
    QWidget *tabIndex;
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QLineEdit *lineIndex;
    QWidget *tabSearch;
    QVBoxLayout *verticalLayout_2;


public:
    static HelpWindow* pDia;

    HelpWindow( QWidget* parent = NULL );
    virtual ~HelpWindow();

    void showDocumentation(const char* file, const char* item1);

protected:

    QUrl showHelpForKeyword(const QString &id);

 //   void closeEvent(QCloseEvent* ev);
    void setActions();

protected slots:
    virtual void languageChange();
    void loadResource( const QUrl &name );
    void filterIndices(const QString &filter);
    void showAddres(const QUrl &name);
    void syncContents();
    //void searchLine();
    void helpAbout();
    void helpVersion();
    void helpPrint();
    void actionFind();
    void actionFindNext();
    void actionFindPrevious();
    void actionZoomIn();
    void actionZoomOut();
    void showFind();
};

class VersionDialog : public QDialog, public Ui::VersionDialogData
{
    Q_OBJECT

public:

    VersionDialog( QWidget* parent = NULL );
    virtual ~VersionDialog();


protected slots:
    virtual void languageChange();

};

#endif // HelpWindow_included

