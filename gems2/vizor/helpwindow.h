/****************************************************************************
** $Id$
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef HELPWINDOW_H
#define HELPWINDOW_H

#include <qdialog.h>
#include <qstringlist.h>
#include <qmap.h>
#include <qmime.h>
#include <qnetwork.h>

class QComboBox;
class QTextBrowser;
class QPopupMenu;
class QMenuBar;
class QToolBar;
class QStatusBar;
class QNetworkOperation;



class HttpMimeSourceFactory:
    public QObject,
    public QMimeSourceFactory {
    
    Q_OBJECT

    QString pathName;
    bool ready;

 public:
    HttpMimeSourceFactory():
	ready(false) {
	qInitNetworkProtocols();
    }
    virtual const QMimeSource * data ( const QString & abs_name ) const;
    QString makeAbsolute ( const QString & abs_or_rel_name, const QString & context ) const;
    
 protected slots:
    void finished(QNetworkOperation *op);
	    
};



class HelpWindow :
    public QDialog
{
    Q_OBJECT

public:
    HelpWindow( const QString& path_, QWidget* parent = 0, bool modal=false);
    ~HelpWindow();

    void loadFile( const QString& path, QWidget* parent = 0 );

private slots:
    void setBackwardAvailable( bool );
    void setForwardAvailable( bool );

    void textChanged();
    void about();
    void aboutQt();
    void openFile();
    void newWindow();
    void print();

    void pathSelected( const QString & );
    void histChosen( int );
    void bookmChosen( int );
    void addBookmark();
    
private:
    void readHistory();
    void readBookmarks();
    
    QTextBrowser* browser;
    QComboBox *pathCombo;
    int backwardId, forwardId;
    QString selectedURL;
    QStringList history, bookmarks;
    QMap<int, QString> mHistory, mBookmarks;
    QPopupMenu *hist, *bookm;

// simulating QMainWindow    
    QToolBar* toolbar;
    QMenuBar* menubar;
    QStatusBar* statusbar;

    QMenuBar* menuBar() { return menubar; }    
    QStatusBar* statusBar() { return statusbar; }    
    QToolBar* toolBar() { return toolbar; }    
};

#endif
