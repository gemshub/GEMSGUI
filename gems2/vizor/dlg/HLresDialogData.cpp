/****************************************************************************
** Form implementation generated from reading ui file 'HLresDialog.ui'
**
** Created: Do 28. Feb 13:33:37 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "HLresDialogData.h"

#include <qvariant.h>
#include <qheader.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>


/* 
 *  Constructs a HLresDialogData which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
HLresDialogData::HLresDialogData( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "HLresDialogData" );
    resize( 720, 499 ); 
    setMinimumSize( QSize( 600, 450 ) );
    QFont f( font() );
    f.setFamily( "Lucida Sans Unicode" );
    f.setPointSize( 9 );
    setFont( f ); 
    setCaption( trUtf8( "Results of Calculation of Equilibrium State: Phases and Species List View" ) );

    QWidget* privateLayoutWidget = new QWidget( this, "hbox_2" );
    privateLayoutWidget->setGeometry( QRect( 50, 440, 630, 30 ) ); 
    hbox_2 = new QHBoxLayout( privateLayoutWidget, 0, -1, "hbox_2"); 

    btnHelp = new QPushButton( privateLayoutWidget, "btnHelp" );
    btnHelp->setText( trUtf8( "&Help" ) );
    QToolTip::add( btnHelp, trUtf8( "Help tooltip" ) );
    QWhatsThis::add( btnHelp, trUtf8( "Help whats this" ) );
    hbox_2->addWidget( btnHelp );

    btnUndo = new QPushButton( privateLayoutWidget, "btnUndo" );
    btnUndo->setText( trUtf8( "&Refresh" ) );
    QToolTip::add( btnUndo, trUtf8( "Restores selections and values, initial or created when BCC was pressed last time" ) );
    hbox_2->addWidget( btnUndo );

    btnPrint = new QPushButton( privateLayoutWidget, "btnPrint" );
    btnPrint->setText( trUtf8( "&Print" ) );
    QToolTip::add( btnPrint, trUtf8( "Checks stoichiometry consistense for all selected species and updates selection" ) );
    hbox_2->addWidget( btnPrint );

    btnClose = new QPushButton( privateLayoutWidget, "btnClose" );
    btnClose->setText( trUtf8( "&Close" ) );
    hbox_2->addWidget( btnClose );

    ListView1 = new QListView( this, "ListView1" );
    ListView1->addColumn( trUtf8( "Phase, Component Name" ) );
    ListView1->addColumn( trUtf8( "" ) );
    ListView1->addColumn( trUtf8( "Type" ) );
    ListView1->addColumn( trUtf8( "X (moles)" ) );
    ListView1->addColumn( trUtf8( "Stability Cr." ) );
    ListView1->addColumn( trUtf8( "log_activity" ) );
    ListView1->addColumn( trUtf8( "Concentration" ) );
    ListView1->addColumn( trUtf8( "ln_gamma" ) );
    QListViewItem * item = new QListViewItem( ListView1, 0 );
    item->setText( 0, trUtf8( "System" ) );

    ListView1->setGeometry( QRect( 10, 10, 700, 410 ) ); 
    QFont ListView1_font(  ListView1->font() );
    ListView1->setFont( ListView1_font ); 
    ListView1->setFocusPolicy( QListView::ClickFocus );
    ListView1->setAcceptDrops( FALSE );
    ListView1->setFrameShape( QListView::StyledPanel );
    ListView1->setMargin( 2 );
    ListView1->setMidLineWidth( 2 );
    ListView1->setResizePolicy( QScrollView::Manual );
    ListView1->setSelectionMode( QListView::Single );
    ListView1->setAllColumnsShowFocus( TRUE );
    ListView1->setShowSortIndicator( TRUE );
    ListView1->setItemMargin( 2 );
    ListView1->setRootIsDecorated( TRUE );

    // signals and slots connections
    connect( btnHelp, SIGNAL( clicked() ), this, SLOT( CmHelp() ) );
    connect( btnClose, SIGNAL( clicked() ), this, SLOT( CmClose() ) );
    connect( btnPrint, SIGNAL( clicked() ), this, SLOT( CmPrint() ) );
    connect( btnUndo, SIGNAL( clicked() ), this, SLOT( CmRefresh() ) );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
HLresDialogData::~HLresDialogData()
{
    // no need to delete child widgets, Qt does it all for us
}

void HLresDialogData::CmClose()
{
    qWarning( "HLresDialogData::CmClose(): Not implemented yet!" );
}

void HLresDialogData::CmHelp()
{
    qWarning( "HLresDialogData::CmHelp(): Not implemented yet!" );
}

void HLresDialogData::CmPrint()
{
    qWarning( "HLresDialogData::CmPrint(): Not implemented yet!" );
}

void HLresDialogData::CmRefresh()
{
    qWarning( "HLresDialogData::CmRefresh(): Not implemented yet!" );
}

