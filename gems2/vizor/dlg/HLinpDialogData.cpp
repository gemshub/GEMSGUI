/****************************************************************************
** Form implementation generated from reading ui file 'HLinpDialog.ui'
**
** Created: Втр Лют 5 12:33:55 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "HLinpDialogData.h"

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
 *  Constructs a HLinpDialogData which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
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
    setCaption( trUtf8( "Phases and Species Profile List View" ) );

    QWidget* privateLayoutWidget = new QWidget( this, "hbox" );
    privateLayoutWidget->setGeometry( QRect( 40, 440, 670, 34 ) ); 
    hbox = new QHBoxLayout( privateLayoutWidget, 0, 10, "hbox"); 

    btnHelp = new QPushButton( privateLayoutWidget, "btnHelp" );
    btnHelp->setText( trUtf8( "&Help" ) );
    QToolTip::add( btnHelp, trUtf8( "hhhhhhhhh" ) );
    hbox->addWidget( btnHelp );

    btnPrint = new QPushButton( privateLayoutWidget, "btnPrint" );
    btnPrint->setText( trUtf8( "&Print" ) );
    QToolTip::add( btnPrint, trUtf8( "Print all phases and species " ) );
    hbox->addWidget( btnPrint );

    btnUndo = new QPushButton( privateLayoutWidget, "btnUndo" );
    btnUndo->setText( trUtf8( "&Undo" ) );
    QToolTip::add( btnUndo, trUtf8( "Restores selections and values, initial or created when BCC was pressed last time" ) );
    hbox->addWidget( btnUndo );

    btnBCC = new QPushButton( privateLayoutWidget, "btnBCC" );
    btnBCC->setText( trUtf8( "Test &BCC" ) );
    QToolTip::add( btnBCC, trUtf8( "Checks stoichiometry consistense for all selected species and updates selection" ) );
    hbox->addWidget( btnBCC );

    btnClose = new QPushButton( privateLayoutWidget, "btnClose" );
    btnClose->setText( trUtf8( "&OK/Close" ) );
    hbox->addWidget( btnClose );

    btnCancel = new QPushButton( privateLayoutWidget, "btnCancel" );
    btnCancel->setText( trUtf8( "&Cancel" ) );
    hbox->addWidget( btnCancel );

    ListView1 = new QListView( this, "ListView1" );
    ListView1->addColumn( trUtf8( "Phase/Species Name" ) );
    ListView1->addColumn( trUtf8( "" ) );
    ListView1->addColumn( trUtf8( "Type" ) );
    ListView1->addColumn( trUtf8( "On/Off" ) );
    ListView1->addColumn( trUtf8( "UC" ) );
    ListView1->addColumn( trUtf8( "Add toBC" ) );
    ListView1->addColumn( trUtf8( "UG" ) );
    ListView1->addColumn( trUtf8( "G0_corr" ) );
    ListView1->addColumn( trUtf8( "UK" ) );
    ListView1->addColumn( trUtf8( "Lower_K" ) );
    ListView1->addColumn( trUtf8( "Upper_K" ) );
    ListView1->addColumn( trUtf8( "Ktype" ) );
    QListViewItem * item = new QListViewItem( ListView1, 0 );
    item->setText( 0, trUtf8( "System" ) );

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
    ListView1->setAllColumnsShowFocus( TRUE );
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

