/****************************************************************************
** $Id$
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "helpwindow.h"
#include <qstatusbar.h>
#include <qtextbrowser.h>
#include <qpixmap.h>
#include <qpopupmenu.h>
#include <qmenubar.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qiconset.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qstylesheet.h>
#include <qmessagebox.h>
#include <qfiledialog.h>
#include <qapplication.h>
#include <qcombobox.h>
#include <qevent.h>
#include <qlineedit.h>
#include <qobjectlist.h>
#include <qfileinfo.h>
#include <qfile.h>
#include <qdatastream.h>
#include <qprinter.h>
#include <qsimplerichtext.h>
#include <qpaintdevicemetrics.h>
#include <qpainter.h>
#include <qlayout.h>

#include <qnetwork.h>
#include <qurloperator.h>
#include <qdragobject.h>

#include <ctype.h>

#include "visor.h"

bool opReady = false;
static QByteArray allData;

const QMimeSource * 
HttpMimeSourceFactory::data ( const QString & abs_name_ ) const
{
	QString abs_name(abs_name_);
	
    // not good - works only in some cases
    if( abs_name.endsWith("/") )
        abs_name.append("index.html");

    if( !abs_name.startsWith("http:") )
	return QMimeSourceFactory::data(abs_name);
    else {

cerr << "request for http data: " << abs_name << endl;
    
	QUrlOperator oper(abs_name);
	connect(&oper, SIGNAL(finished(QNetworkOperation*)), 
	    this, SLOT(finishedSlot(QNetworkOperation*)));
	connect(&oper, SIGNAL(data(const QByteArray&, QNetworkOperation*)), 
	    this, SLOT(dataSlot(const QByteArray&, QNetworkOperation*)));

	oper.get();
	
	opReady = false;
	while( !opReady ) {
	    qApp->processEvents();
	}
cerr << "bail out" << endl;

	QFileInfo fi(abs_name);
	// get the right mimetype
	QString e = fi.extension(FALSE);
	QCString mimetype = "application/octet-stream";
	if ( e.startsWith("htm") )
	    mimetype = QCString("text/")+QCString(e.lower().latin1());
	else
	    mimetype = QCString("image/")+QCString(e.lower().latin1());

	QStoredDrag* sr = new QStoredDrag( mimetype );
	sr->setEncodedData( allData );

	allData.resize(0);

	return sr;
    }
}


void 
HttpMimeSourceFactory::dataSlot(const QByteArray & data, QNetworkOperation * op)
{
    if( data.size() == 0 )
	return;
	
    size_t newSize = allData.size() + data.size();
    char* newArray = new char[newSize];

    if( allData.size() > 0 )
	memcpy(newArray, allData.data(), allData.size());
    memcpy(newArray + allData.size(), data.data(), data.size());
    allData.assign(newArray, newSize);
}

void
HttpMimeSourceFactory::finishedSlot(QNetworkOperation *op)
{

cerr << "finished" << endl;
    ready = true;
    opReady = true;
    
    if ( op && op->state() == QNetworkProtocol::StFailed ) {
	// an error happend, let the user know that
	QMessageBox::critical( 0, "ERROR", op->protocolDetail() );
    }
}

QString 
HttpMimeSourceFactory::makeAbsolute ( const QString & abs_or_rel_name, const QString & context ) const
{
	if( abs_or_rel_name.startsWith("http://") /*&& !context.startsWith("http://")*/ ) {
	cerr<< "makeAbsolute " << abs_or_rel_name << endl;
	if( !context.isEmpty() )
	    cerr << " context " << context << endl;
	    return QMimeSourceFactory::makeAbsolute(abs_or_rel_name, "");
//	    return abs_or_rel_name;
	}
	else
	    if( !context.isEmpty() && context.startsWith("http://") ) {
		cerr<< "makeAbsolute " << abs_or_rel_name << endl;
		if( !context.isEmpty() )
		    cerr << " context " << context << endl;

		QString contextBase;
		if( abs_or_rel_name.startsWith("/") )
		    contextBase = context.left( context.find("/", 8) );
		else
		    contextBase = context.left( context.findRev("/")+1 );

		return contextBase + abs_or_rel_name;
	    }
	    else
		return QMimeSourceFactory::makeAbsolute(abs_or_rel_name, context);
}



/*! HelpWindow class represents window for displaying HTML documentation
    it can work with local filesystem or with HTTP protocol
*/

HelpWindow::HelpWindow( const QString& path_,
			QWidget* parent, bool modal )
    : QDialog( parent, 0, modal, WDestructiveClose ),
      pathCombo( 0 ), 
      selectedURL()
{
    menubar = new QMenuBar(this);
    toolbar = new QToolBar("Navigation Toolbar", 0, this);
    statusbar = new QStatusBar(this);

    readHistory();
    readBookmarks();

    browser = new QTextBrowser( this );
    browser->setMimeSourceFactory(new HttpMimeSourceFactory());
    browser->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    connect( browser, SIGNAL( textChanged() ),
	     this, SLOT( textChanged() ) );

//    setCentralWidget( browser );
    QVBoxLayout *vlayout = new QVBoxLayout(this);
    vlayout->addWidget(menubar);
    vlayout->addWidget(toolbar);
    vlayout->addWidget(browser);
    vlayout->addWidget(statusbar);
    
    connect( browser, SIGNAL( highlighted( const QString&) ),
	     statusBar(), SLOT( message( const QString&)) );

    resize( 520, 600 );

    QPopupMenu* file = new QPopupMenu( this );
//    file->insertItem( tr("&New Window"), this, SLOT( newWindow() ), ALT | Key_N );
    file->insertItem( tr("&Open File"), this, SLOT( openFile() ), ALT | Key_O );
    file->insertItem( tr("&Print"), this, SLOT( print() ), ALT | Key_P );
    file->insertSeparator();
    file->insertItem( tr("&Close"), this, SLOT( close() ), ALT | Key_Q );
//    file->insertItem( tr("E&xit"), qApp, SLOT( closeAllWindows() ), ALT | Key_X );

    // The same three icons are used twice each.
    gstring img = pVisor->imgDir() + "back.xpm";
    QIconSet icon_back( QPixmap(img.c_str()) );
    img = pVisor->imgDir() + "forward.xpm";
    QIconSet icon_forward( QPixmap(img.c_str()) );
    img = pVisor->imgDir() + "home.xpm";
    QIconSet icon_home( QPixmap(img.c_str()) );

    QPopupMenu* go = new QPopupMenu( this );
    backwardId = go->insertItem( icon_back,
				 "&Backward", browser, SLOT( backward() ),
				 ALT | Key_Left );
    forwardId = go->insertItem( icon_forward,
				"&Forward", browser, SLOT( forward() ),
				ALT | Key_Right );
    go->insertItem( icon_home, "&Home", browser, SLOT( home() ) );

    QPopupMenu* help = new QPopupMenu( this );
    help->insertItem( "&About ...", this, SLOT( about() ) );

    hist = new QPopupMenu( this );
    QStringList::Iterator it = history.begin();
    for ( ; it != history.end(); ++it )
	mHistory[ hist->insertItem( *it ) ] = *it;
    connect( hist, SIGNAL( activated( int ) ),
	     this, SLOT( histChosen( int ) ) );

    bookm = new QPopupMenu( this );
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
/*    history.clear();
    QMap<int, QString>::Iterator it = mHistory.begin();
    for ( ; it != mHistory.end(); ++it )
	history.append( *it );

    QFile f( QDir::currentDirPath() + "/.history" );
    f.open( IO_WriteOnly );
    QDataStream s( &f );
    s << history;
    f.close();

    bookmarks.clear();
    QMap<int, QString>::Iterator it2 = mBookmarks.begin();
    for ( ; it2 != mBookmarks.end(); ++it2 )
	bookmarks.append( *it2 );

    QFile f2( QDir::currentDirPath() + "/.bookmarks" );
    f2.open( IO_WriteOnly );
    QDataStream s2( &f2 );
    s2 << bookmarks;
    f2.close();
*/
}

void HelpWindow::about()
{
    QMessageBox::about( this, "Help Window",
			"Allows to browse GEM-Selektor HTML documentation"
			);
}


void HelpWindow::aboutQt()
{
    QMessageBox::aboutQt( this, "QBrowser" );
}

void HelpWindow::openFile()
{
#ifndef QT_NO_FILEDIALOG 
    QString fn = QFileDialog::getOpenFileName( QString::null, QString::null, this );
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
	QPaintDeviceMetrics metrics(p.device());
	int dpix = metrics.logicalDpiX();
	int dpiy = metrics.logicalDpiY();
	const int margin = 72; // pt
	QRect body(margin*dpix/72, margin*dpiy/72,
		   metrics.width()-margin*dpix/72*2,
		   metrics.height()-margin*dpiy/72*2 );
	QFont font("times", 10);
	QSimpleRichText richText( browser->text(), font, browser->context(), browser->styleSheet(),
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
    if ( QFile::exists( QDir::currentDirPath() + "/.history" ) ) {
	QFile f( QDir::currentDirPath() + "/.history" );
	f.open( IO_ReadOnly );
	QDataStream s( &f );
	s >> history;
	f.close();
	while ( history.count() > 20 )
	    history.remove( history.begin() );
    }
}

void HelpWindow::readBookmarks()
{
    if ( QFile::exists( QDir::currentDirPath() + "/.bookmarks" ) ) {
	QFile f( QDir::currentDirPath() + "/.bookmarks" );
	f.open( IO_ReadOnly );
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
