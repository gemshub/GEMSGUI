/****************************************************************************
** Form implementation generated from reading ui file 'HLinpDialog.ui'
**
** Created: Wed Aug 29 13:37:33 2001
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "HLinpDialogData.h"

#include <qheader.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qlistview.h>


/* 
 *  Constructs a HLinpDialogData which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
HLinpDialogData::HLinpDialogData( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "HLinpDialogData" );
    resize( 790, 490 ); 
    setMinimumSize( QSize( 600, 450 ) );
    QFont f( font() );
    f.setFamily( "Lucida Sans Unicode" );
    f.setPointSize( 9 );
    setFont( f ); 
    setCaption( tr( "Phases and Species Profile List View" ) );

    QWidget* privateLayoutWidget = new QWidget( this, "hbox" );
    privateLayoutWidget->setGeometry( QRect( 40, 440, 670, 34 ) ); 
    hbox = new QHBoxLayout( privateLayoutWidget ); 
    hbox->setSpacing( 10 );
    hbox->setMargin( 0 );

    btnHelp = new QPushButton( privateLayoutWidget, "btnHelp" );
    btnHelp->setText( tr( "&Help" ) );
    QToolTip::add(  btnHelp, tr( "hhhhhhhhh" ) );
    hbox->addWidget( btnHelp );

    btnPrint = new QPushButton( privateLayoutWidget, "btnPrint" );
    btnPrint->setText( tr( "&Print" ) );
    QToolTip::add(  btnPrint, tr( "Print all phases and species " ) );
    hbox->addWidget( btnPrint );

    btnUndo = new QPushButton( privateLayoutWidget, "btnUndo" );
    btnUndo->setText( tr( "&Undo" ) );
    QToolTip::add(  btnUndo, tr( "Restores selections and values, initial or created when BCC was pressed last time" ) );
    hbox->addWidget( btnUndo );

    btnBCC = new QPushButton( privateLayoutWidget, "btnBCC" );
    btnBCC->setText( tr( "Test &BCC" ) );
    QToolTip::add(  btnBCC, tr( "Checks stoichiometry consistense for all selected species and updates selection" ) );
    hbox->addWidget( btnBCC );

    btnClose = new QPushButton( privateLayoutWidget, "btnClose" );
    btnClose->setText( tr( "&OK/Close" ) );
    hbox->addWidget( btnClose );

    btnCancel = new QPushButton( privateLayoutWidget, "btnCancel" );
    btnCancel->setText( tr( "&Cancel" ) );
    hbox->addWidget( btnCancel );

        ListView1 = new QListView( this, "ListView1" );
    ListView1->addColumn( tr( "Phase/Species Name" ) );
    ListView1->addColumn( tr( "" ) );
    ListView1->addColumn( tr( "Type" ) );
    ListView1->addColumn( tr( "On/Off" ) );
    ListView1->addColumn( tr( "UC" ) );
    ListView1->addColumn( tr( "Add toBC" ) );
    ListView1->addColumn( tr( "UG" ) );
    ListView1->addColumn( tr( "G0_corr" ) );
    ListView1->addColumn( tr( "UK" ) );
    ListView1->addColumn( tr( "Lower_K" ) );
    ListView1->addColumn( tr( "Upper_K" ) );
    ListView1->addColumn( tr( "Ktype" ) );
    QListViewItem * item = new QListViewItem( ListView1, 0 );
    item->setText( 0, tr( "System" ) );

    ListView1->setGeometry( QRect( 10, 10, 760, 420 ) );
    QFont ListView1_font(  ListView1->font() );
    ListView1->setFont( ListView1_font );
    ListView1->setFocusPolicy( QListView::ClickFocus );
    ListView1->setAcceptDrops( FALSE );
    ListView1->setFrameShape( QListView::StyledPanel );
    ListView1->setMargin( 2 );
    ListView1->setMidLineWidth( 2 );
    ListView1->setResizePolicy( QScrollView::Manual );
    ListView1->setSelectionMode( QListView::Single );
    ListView1->setAllColumnsShowFocus( FALSE/*TRUE*/ );
    ListView1->setShowSortIndicator( TRUE );
    ListView1->setItemMargin( 2 );
    ListView1->setRootIsDecorated( TRUE );

    // signals and slots connections
    connect( btnHelp, SIGNAL( clicked() ), this, SLOT( CmHelp() ) );
    connect( btnPrint, SIGNAL( clicked() ), this, SLOT( CmPrint() ) );
    connect( btnUndo, SIGNAL( clicked() ), this, SLOT( CmUndo() ) );
    connect( btnBCC, SIGNAL( clicked() ), this, SLOT( CmBCC() ) );
    connect( btnClose, SIGNAL( clicked() ), this, SLOT( CmOK() ) );
    connect( btnCancel, SIGNAL( clicked() ), this, SLOT( CmCancel() ) );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
HLinpDialogData::~HLinpDialogData()
{
    // no need to delete child widgets, Qt does it all for us
}

/*  
 *  Main event handler. Reimplemented to handle application
 *  font changes
 */
bool HLinpDialogData::event( QEvent* ev )
{
    bool ret = QDialog::event( ev ); 
    if ( ev->type() == QEvent::ApplicationFontChange ) {
	QFont ListView1_font(  ListView1->font() );
	ListView1->setFont( ListView1_font ); 
    }
    return ret;
}

void HLinpDialogData::CmBCC()
{
    qWarning( "HLinpDialogData::CmBCC(): Not implemented yet!" );
}

void HLinpDialogData::CmCancel()
{
    qWarning( "HLinpDialogData::CmCancel(): Not implemented yet!" );
}

void HLinpDialogData::CmHelp()
{
    qWarning( "HLinpDialogData::CmHelp(): Not implemented yet!" );
}

void HLinpDialogData::CmOK()
{
    qWarning( "HLinpDialogData::CmOK(): Not implemented yet!" );
}

void HLinpDialogData::CmPrint()
{
    qWarning( "HLinpDialogData::CmPrint(): Not implemented yet!" );
}

void HLinpDialogData::CmUndo()
{
    qWarning( "HLinpDialogData::CmUndo(): Not implemented yet!" );
}

