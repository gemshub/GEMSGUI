/****************************************************************************
** $Id: helpwindow.cpp 570 2005-10-06 08:41:44Z gems $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "helpwindow.h"
#include <qstatusbar.h>
#include <q3textbrowser.h>
#include <qpixmap.h>
#include <q3popupmenu.h>
#include <qmenubar.h>
#include <q3toolbar.h>
#include <qtoolbutton.h>
#include <qicon.h>
#include <qfile.h>
#include <q3textstream.h>
#include <q3stylesheet.h>
#include <qmessagebox.h>
#include <q3filedialog.h>
#include <qapplication.h>
#include <qcombobox.h>
#include <qevent.h>
#include <qlineedit.h>
#include <qobject.h>
#include <qfileinfo.h>
#include <qfile.h>
#include <qdatastream.h>
#include <qprinter.h>
#include <q3simplerichtext.h>
#include <q3paintdevicemetrics.h>
#include <qpainter.h>
#include <qlayout.h>

#include <q3network.h>
#include <q3urloperator.h>
#include <q3dragobject.h>
//Added by qt3to4:
#include <Q3CString>
#include <Q3Frame>
#include <Q3VBoxLayout>

#include <ctype.h>

#include "visor.h"

bool opReady = false;
static QByteArray allData;
QString contextBase;

const QMimeSource * 
HttpMimeSourceFactory::data ( const QString & abs_name_ ) const
{
	QString abs_name(abs_name_);
	
    // not good - works only in some cases
    if( abs_name.endsWith("/") )
        abs_name.append("index.html");

    if( !abs_name.startsWith("http:") )
	return Q3MimeSourceFactory::data(abs_name);
    else {

cerr << "request for http data: " << abs_name.ascii() << endl;


	Q3UrlOperator oper(abs_name);
	connect(&oper, SIGNAL(finished(Q3NetworkOperation*)), 
	    this, SLOT(finishedSlot(Q3NetworkOperation*)));
	connect(&oper, SIGNAL(data(const QByteArray&, Q3NetworkOperation*)), 
	    this, SLOT(dataSlot(const QByteArray&, Q3NetworkOperation*)));

	oper.get();

	opReady = false;
	while( !opReady ) {
	    qApp->processEvents();
	}
cerr << "bail out" << endl;

	QFileInfo fi(abs_name);
	// get the right mimetype
	QString e = fi.extension(FALSE);
	Q3CString mimetype = "application/octet-stream";
	if ( e.startsWith("htm") )
	    mimetype = Q3CString("text/")+Q3CString(e.lower().latin1());
	else
	    mimetype = Q3CString("image/")+Q3CString(e.lower().latin1());

	Q3StoredDrag* sr = new Q3StoredDrag( mimetype );
	sr->setEncodedData( allData );

	allData.resize(0);

	return sr;
    }
}


void 
HttpMimeSourceFactory::dataSlot(const QByteArray & data, Q3NetworkOperation * op)
{
cerr << "dataSlot" <<  "  " << allData.size() << endl;
    if( data.size() == 0 )
	return;
	
    size_t newSize = allData.size() + data.size();
    char* newArray = new char[newSize];

cerr << "dataSlot" << newSize << "  " << allData.size() << endl;

    if( allData.size() > 0 )
	memcpy(newArray, allData.data(), allData.size());
    memcpy(newArray + allData.size(), data.data(), data.size());
    allData.assign(newArray, newSize);
}

void
HttpMimeSourceFactory::finishedSlot(Q3NetworkOperation *op)
{

cerr << "finished" << endl;
    ready = true;
    opReady = true;
    
    if ( op && op->state() == Q3NetworkProtocol::StFailed ) {
	// an error happend, let the user know that
	QMessageBox::critical( 0, "ERROR", op->protocolDetail() );
    }
}

QString 
HttpMimeSourceFactory::makeAbsolute ( const QString & abs_or_rel_name, const QString & context ) const
{
	if( abs_or_rel_name.startsWith("http://") /*&& !context.startsWith("http://")*/ )
        {
	 cerr<< "makeAbsolute " << abs_or_rel_name << endl;
       if( !context.isEmpty() )
	     cerr << " context " << context << endl;
         return Q3MimeSourceFactory::makeAbsolute(abs_or_rel_name, "");
//	    return abs_or_rel_name;
	}
	else
	  if( !context.isEmpty() && context.startsWith("http://") )
          {
		cerr<< "makeAbsolute " << abs_or_rel_name << endl;
		if( !context.isEmpty() )
		    cerr << " context " << context << endl;

		QString contextBase;
		if( abs_or_rel_name.startsWith("/") )
		    contextBase = context.left( context.find("/", 8) );
		else
		    contextBase = context.left( context.findRev("/")+1 );

               contextBase += abs_or_rel_name;
   	       return Q3MimeSourceFactory::makeAbsolute( contextBase, context );
//		return QMimeSourceFactory::makeAbsolute( abs_or_rel_name, contextBase );
//		return contextBase + abs_or_rel_name;  SD oct 2005
	    }
	    else
		return Q3MimeSourceFactory::makeAbsolute(abs_or_rel_name, context);
}



/*! HelpWindow class represents window for displaying HTML documentation
    it can work with local filesystem or with HTTP protocol
*/

static QString HISTORY_FILE;
static QString BOOKMARKS_FILE;

HelpWindow::HelpWindow( const QString& path_,
			QWidget* parent, bool modal )
    : QDialog( parent, 0, modal, Qt::WDestructiveClose ),
      pathCombo( 0 ), 
      selectedURL()
{
    HISTORY_FILE = QString(pVisor->userGEMDir().c_str()) +  "/.helpviewer_history";
    BOOKMARKS_FILE = QString(pVisor->userGEMDir().c_str()) +  "/.helpviewer_bookmarks";

    menubar = new QMenuBar(this);
    toolbar = new Q3ToolBar("Navigation Toolbar", 0, this);
    statusbar = new QStatusBar(this);

    readHistory();
    readBookmarks();

    browser = new Q3TextBrowser( this );
    browser->setMimeSourceFactory(new HttpMimeSourceFactory());
    browser->setFrameStyle( Q3Frame::Panel | Q3Frame::Sunken );
    connect( browser, SIGNAL( textChanged() ),
	     this, SLOT( textChanged() ) );

//    setCentralWidget( browser );
    Q3VBoxLayout *vlayout = new Q3VBoxLayout(this);
    vlayout->addWidget(menubar);
    vlayout->addWidget(toolbar);
    vlayout->addWidget(browser);
    vlayout->addWidget(statusbar);
    
    connect( browser, SIGNAL( highlighted( const QString&) ),
	     statusBar(), SLOT( message( const QString&)) );

    resize( 620, 660 );

    Q3PopupMenu* file = new Q3PopupMenu( this );
//    file->insertItem( tr("&New Window"), this, SLOT( newWindow() ), CTRL+Key_N );
    file->insertItem( tr("&Open File"), this, SLOT( openFile() ), CTRL+Key_O );
    file->insertItem( tr("&Print"), this, SLOT( print() ), CTRL+Key_P );
    file->insertSeparator();
    file->insertItem( tr("&Close"), this, SLOT( close() ), CTRL+Key_Q );
//    file->insertItem( tr("E&xit"), qApp, SLOT( closeAllWindows() ), CTRL+Key_X );

    // The same three icons are used twice each.
    gstring img = pVisor->imgDir() + "back.xpm";
    QIcon icon_back( QPixmap(img.c_str()) );
    img = pVisor->imgDir() + "forward.xpm";
    QIcon icon_forward( QPixmap(img.c_str()) );
    img = pVisor->imgDir() + "home.xpm";
    QIcon icon_home( QPixmap(img.c_str()) );

    Q3PopupMenu* go = new Q3PopupMenu( this );
    backwardId = go->insertItem( icon_back,
				 "&Backward", browser, SLOT( backward() ),
				 CTRL+Key_Left );
    forwardId = go->insertItem( icon_forward,
				"&Forward", browser, SLOT( forward() ),
				CTRL+Key_Right );
    go->insertItem( icon_home, "&Home", browser, SLOT( home() ) );

    Q3PopupMenu* help = new Q3PopupMenu( this );
    help->insertItem( "&About ...", this, SLOT( about() ) );

    hist = new Q3PopupMenu( this );
    QStringList::Iterator it = history.begin();
    for ( ; it != history.end(); ++it )
	mHistory[ hist->insertItem( *it ) ] = *it;
    connect( hist, SIGNAL( activated( int ) ),
	     this, SLOT( histChosen( int ) ) );

    bookm = new Q3PopupMenu( this );
    bookm->insertItem( "Add Bookmark", this, SLOT( addBookmark() ) );
    bookm->insertSeparator();

    QStringList::Iterator it2 = bookmarks.begin();
    for ( ; it2 != bookmarks.end(); ++it2 )
	mBookmarks[ bookm->insertItem( *it2 ) ] = *it2;
    connect( bookm, SIGNAL( activated( int ) ),
	     this, SLOT( bookmChosen( int ) ) );

    menuBar()->insertItem( tr("&File"), file );
    menuBar()->insertItem( tr("&Go"), go );
    menuBar()->insertItem( tr( "History" ), hist );
    menuBar()->insertItem( tr( "Bookmarks" ), bookm );
    menuBar()->insertSeparator();
    menuBar()->insertItem( tr("&Help"), help );

    menuBar()->setItemEnabled( forwardId, FALSE);
    menuBar()->setItemEnabled( backwardId, FALSE);
    connect( browser, SIGNAL( backwardAvailable( bool ) ),
	     this, SLOT( setBackwardAvailable( bool ) ) );
    connect( browser, SIGNAL( forwardAvailable( bool ) ),
	     this, SLOT( setForwardAvailable( bool ) ) );


//    QToolBar* toolbar = new QToolBar( this );
//    addToolBar( toolbar, "Toolbar");
    QToolButton* button;

    button = new QToolButton( icon_back, tr("Backward"), "", browser, SLOT(backward()), toolbar );
    connect( browser, SIGNAL( backwardAvailable(bool) ), button, SLOT( setEnabled(bool) ) );
    button->setEnabled( FALSE );
    button = new QToolButton( icon_forward, tr("Forward"), "", browser, SLOT(forward()), toolbar );
    connect( browser, SIGNAL( forwardAvailable(bool) ), button, SLOT( setEnabled(bool) ) );
    button->setEnabled( FALSE );
    button = new QToolButton( icon_home, tr("Home"), "", browser, SLOT(home()), toolbar );

    toolbar->addSeparator();

    pathCombo = new QComboBox( TRUE, toolbar );
    connect( pathCombo, SIGNAL( activated( const QString & ) ),
	     this, SLOT( pathSelected( const QString & ) ) );
    toolbar->setStretchableWidget( pathCombo );
//    setRightJustification( TRUE );
//    setDockEnabled( Left, FALSE );
//    setDockEnabled( Right, FALSE );

    pathCombo->insertItem( path_ );

    show();
    loadFile( path_, parent );

    browser->setFocus();
}


void HelpWindow::loadFile( const QString& home, QWidget* parent )
{
//    disconnect( SIGNAL( destroyed() ), this, SLOT( close() ) );
    if ( !home.isEmpty() )
	browser->setSource( home );
//    connect( parent, SIGNAL( destroyed() ), SLOT( close() ) );
}


void HelpWindow::setBackwardAvailable( bool b)
{
    menuBar()->setItemEnabled( backwardId, b);
}

void HelpWindow::setForwardAvailable( bool b)
{
    menuBar()->setItemEnabled( forwardId, b);
}


void HelpWindow::textChanged()
{
    if ( browser->documentTitle().isNull() )
	setCaption( browser->context() );
    else
	setCaption( browser->documentTitle() ) ;

    selectedURL = browser->context();
    if ( !selectedURL.isEmpty() && pathCombo ) {
	bool exists = FALSE;
	int i;
	for ( i = 0; i < pathCombo->count(); ++i ) {
	    if ( pathCombo->text( i ) == selectedURL ) {
		exists = TRUE;
		break;
	    }
	}
	if ( !exists ) {
	    pathCombo->insertItem( selectedURL, 0 );
	    pathCombo->setCurrentItem( 0 );
	    mHistory[ hist->insertItem( selectedURL ) ] = selectedURL;
	} else
	    pathCombo->setCurrentItem( i );
	selectedURL = QString::null;
    }
}

HelpWindow::~HelpWindow()
{
    history.clear();
    QMap<int, QString>::Iterator it = mHistory.begin();
    for ( ; it != mHistory.end(); ++it )
	history.append( *it );

//    QFile f( QDir::currentDirPath() + "/.history" );
    QFile f( HISTORY_FILE );
    f.open( QIODevice::WriteOnly );
    QDataStream s( &f );
    s << history;
    f.close();

    bookmarks.clear();
    QMap<int, QString>::Iterator it2 = mBookmarks.begin();
    for ( ; it2 != mBookmarks.end(); ++it2 )
	bookmarks.append( *it2 );

//    QFile f2( QDir::currentDirPath() + "/.bookmarks" );
    QFile f2( BOOKMARKS_FILE );
    f2.open( QIODevice::WriteOnly );
    QDataStream s2( &f2 );
    s2 << bookmarks;
    f2.close();
}

void HelpWindow::about()
{
    QMessageBox::about( this, "Help Window",
			"Allows to browse GEM-Selektor HTML documentation");
}


void HelpWindow::aboutQt()
{
    QMessageBox::aboutQt( this, "QBrowser" );
}

void HelpWindow::openFile()
{
#ifndef QT_NO_FILEDIALOG 
    QString fn = Q3FileDialog::getOpenFileName( QString::null, QString::null, this );
    if ( !fn.isEmpty() )
	browser->setSource( fn );
#endif
}

void HelpWindow::newWindow()
{
    ( new HelpWindow(browser->source()) )->show();
}

void HelpWindow::print()
{
#ifndef QT_NO_PRINTER
    QPrinter printer;
    printer.setFullPage(TRUE);
    if ( printer.setup() ) {
	QPainter p( &printer );
	Q3PaintDeviceMetrics metrics(p.device());
	int dpix = metrics.logicalDpiX();
	int dpiy = metrics.logicalDpiY();
	const int margin = 72; // pt
	QRect body(margin*dpix/72, margin*dpiy/72,
		   metrics.width()-margin*dpix/72*2,
		   metrics.height()-margin*dpiy/72*2 );
	QFont font("times", 10);
	Q3SimpleRichText richText( browser->text(), font, browser->context(), browser->styleSheet(),
				  browser->mimeSourceFactory(), body.height() );
	richText.setWidth( &p, body.width() );
	QRect view( body );
	int page = 1;
	do {
	    richText.draw( &p, body.left(), body.top(), view, colorGroup() );
	    view.moveBy( 0, body.height() );
	    p.translate( 0 , -body.height() );
	    p.setFont( font );
	    p.drawText( view.right() - p.fontMetrics().width( QString::number(page) ),
			view.bottom() + p.fontMetrics().ascent() + 5, QString::number(page) );
 	    if ( view.top() >= body.top() + richText.height() )
		break;
	    printer.newPage();
	    page++;
	} while (TRUE);
    }
#endif
}

void HelpWindow::pathSelected( const QString &_path )
{
    browser->setSource( _path );
    QMap<int, QString>::Iterator it = mHistory.begin();
    bool exists = FALSE;
    for ( ; it != mHistory.end(); ++it ) {
	if ( *it == _path ) {
	    exists = TRUE;
	    break;
	}
    }
    if ( !exists )
	mHistory[ hist->insertItem( _path ) ] = _path;
}

void HelpWindow::readHistory()
{
    if ( QFile::exists( HISTORY_FILE ) ) {
	QFile f( HISTORY_FILE );
	f.open( QIODevice::ReadOnly );
	QDataStream s( &f );
	s >> history;
	f.close();
	while ( history.count() > 20 )
	    history.remove( history.begin() );
    }
}

void HelpWindow::readBookmarks()
{
    if ( QFile::exists( BOOKMARKS_FILE ) ) {
	QFile f( BOOKMARKS_FILE );
	f.open( QIODevice::ReadOnly );
	QDataStream s( &f );
	s >> bookmarks;
	f.close();
    }
}

void HelpWindow::histChosen( int i )
{
    if ( mHistory.contains( i ) )
	browser->setSource( mHistory[ i ] );
}

void HelpWindow::bookmChosen( int i )
{
    if ( mBookmarks.contains( i ) )
	browser->setSource( mBookmarks[ i ] );
}

void HelpWindow::addBookmark()
{
    mBookmarks[ bookm->insertItem( caption() ) ] = browser->context();
}
