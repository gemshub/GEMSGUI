/****************************************************************************
** Form implementation generated from reading ui file 'ListFilesDialog.ui'
**
** Created: Пнд Гру 17 12:44:08 2001
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "ListFilesDialogData.h"

#include <qvariant.h>
#include <qheader.h>
#include <qlabel.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a ListFilesDialogData which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
ListFilesDialogData::ListFilesDialogData( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "ListFilesDialogData" );
    resize( 581, 454 ); 
    setMinimumSize( QSize( 450, 400 ) );
    QFont f( font() );
    f.setFamily( "Lucida Sans Unicode" );
    setFont( f ); 
    setCaption( trUtf8( "Database File Configuration for Modelling Profile" ) );

    pLabel = new QLabel( this, "pLabel" );
    pLabel->setGeometry( QRect( 0, 10, 440, 20 ) ); 
    QFont pLabel_font(  pLabel->font() );
    pLabel_font.setPointSize( 11 );
    pLabel->setFont( pLabel_font ); 
    pLabel->setText( trUtf8( "Please, check files linkes to Profile:" ) );
    pLabel->setAlignment( int( QLabel::AlignCenter ) );

    pListFiles = new QListView( this, "pListFiles" );
    pListFiles->addColumn( trUtf8( "File Keyword" ) );
    pListFiles->addColumn( trUtf8( "File Path" ) );
    pListFiles->setGeometry( QRect( 10, 40, 560, 320 ) ); 
    pListFiles->setAllColumnsShowFocus( FALSE );
    pListFiles->setItemMargin( 1 );

    QWidget* privateLayoutWidget = new QWidget( this, "Layout3" );
    privateLayoutWidget->setGeometry( QRect( 10, 380, 560, 40 ) ); 
    Layout3 = new QHBoxLayout( privateLayoutWidget, 0, 6, "Layout3"); 

    pGO_OKButton = new QPushButton( privateLayoutWidget, "pGO_OKButton" );
    pGO_OKButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, pGO_OKButton->sizePolicy().hasHeightForWidth() ) );
    QFont pGO_OKButton_font(  pGO_OKButton->font() );
    pGO_OKButton_font.setPointSize( 10 );
    pGO_OKButton->setFont( pGO_OKButton_font ); 
    pGO_OKButton->setText( trUtf8( "&Ok" ) );
    pGO_OKButton->setDefault( TRUE );
    QToolTip::add( pGO_OKButton, trUtf8( "Files selections is finisded OK" ) );
    Layout3->addWidget( pGO_OKButton );

    pHelpButton = new QPushButton( privateLayoutWidget, "pHelpButton" );
    pHelpButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, pHelpButton->sizePolicy().hasHeightForWidth() ) );
    QFont pHelpButton_font(  pHelpButton->font() );
    pHelpButton_font.setPointSize( 10 );
    pHelpButton->setFont( pHelpButton_font ); 
    pHelpButton->setText( trUtf8( "&Help" ) );
    QToolTip::add( pHelpButton, trUtf8( "Learn about System Profile concept and profile configuration" ) );
    Layout3->addWidget( pHelpButton );

    pCancelButton = new QPushButton( privateLayoutWidget, "pCancelButton" );
    pCancelButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, pCancelButton->sizePolicy().hasHeightForWidth() ) );
    QFont pCancelButton_font(  pCancelButton->font() );
    pCancelButton_font.setPointSize( 10 );
    pCancelButton->setFont( pCancelButton_font ); 
    pCancelButton->setText( trUtf8( "&Cancel" ) );
    Layout3->addWidget( pCancelButton );

    // signals and slots connections
    connect( pGO_OKButton, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( pHelpButton, SIGNAL( clicked() ), this, SLOT( CmHelp() ) );
    connect( pCancelButton, SIGNAL( clicked() ), this, SLOT( reject() ) );

    // tab order
    setTabOrder( pListFiles, pGO_OKButton );
    setTabOrder( pGO_OKButton, pHelpButton );
    setTabOrder( pHelpButton, pCancelButton );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
ListFilesDialogData::~ListFilesDialogData()
{
    // no need to delete child widgets, Qt does it all for us
}

void ListFilesDialogData::CmHelp()
{
    qWarning( "ListFilesDialogData::CmHelp(): Not implemented yet!" );
}

