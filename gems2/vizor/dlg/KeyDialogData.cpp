/****************************************************************************
** Form implementation generated from reading ui file 'KeyDialog.ui'
**
** Created: Fr 1. Mrz 14:05:40 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "KeyDialogData.h"

#include <qvariant.h>
#include <qlabel.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a KeyDialogData which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
KeyDialogData::KeyDialogData( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "KeyDialogData" );
    resize( 640, 421 ); 
    setMinimumSize( QSize( 640, 420 ) );
    QFont f( font() );
    f.setFamily( "Courier" );
    f.setPointSize( 9 );
    setFont( f ); 
    setCaption( trUtf8( "List of database record keys" ) );

    pList = new QListBox( this, "pList" );
    pList->setGeometry( QRect( 10, 40, 600, 320 ) ); 
    QFont pList_font(  pList->font() );
    pList->setFont( pList_font ); 

    pLabel = new QLabel( this, "pLabel" );
    pLabel->setGeometry( QRect( -1, 9, 618, 23 ) ); 
    QFont pLabel_font(  pLabel->font() );
    pLabel_font.setFamily( "Lucida Sans Unicode" );
    pLabel_font.setPointSize( 12 );
    pLabel->setFont( pLabel_font ); 
    pLabel->setText( trUtf8( "Please, select one record key" ) );
    pLabel->setAlignment( int( QLabel::AlignCenter ) );

    QWidget* privateLayoutWidget = new QWidget( this, "Layout2" );
    privateLayoutWidget->setGeometry( QRect( 20, 370, 590, 37 ) ); 
    Layout2 = new QHBoxLayout( privateLayoutWidget, 0, 6, "Layout2"); 

    PushButton3 = new QPushButton( privateLayoutWidget, "PushButton3" );
    PushButton3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, PushButton3->sizePolicy().hasHeightForWidth() ) );
    QFont PushButton3_font(  PushButton3->font() );
    PushButton3_font.setFamily( "Lucida Sans Unicode" );
    PushButton3_font.setPointSize( 12 );
    PushButton3->setFont( PushButton3_font ); 
    PushButton3->setText( trUtf8( "&Cancel" ) );
    Layout2->addWidget( PushButton3 );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout2->addItem( spacer );

    pFilterButton = new QPushButton( privateLayoutWidget, "pFilterButton" );
    pFilterButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, pFilterButton->sizePolicy().hasHeightForWidth() ) );
    QFont pFilterButton_font(  pFilterButton->font() );
    pFilterButton_font.setFamily( "Lucida Sans Unicode" );
    pFilterButton_font.setPointSize( 12 );
    pFilterButton->setFont( pFilterButton_font ); 
    pFilterButton->setText( trUtf8( "&Filter..." ) );
    QToolTip::add( pFilterButton, trUtf8( "Here you can specify a template to filter a large list of record keys" ) );
    Layout2->addWidget( pFilterButton );
    QSpacerItem* spacer_2 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout2->addItem( spacer_2 );

    pButton2 = new QPushButton( privateLayoutWidget, "pButton2" );
    pButton2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, pButton2->sizePolicy().hasHeightForWidth() ) );
    QFont pButton2_font(  pButton2->font() );
    pButton2_font.setFamily( "Lucida Sans Unicode" );
    pButton2_font.setPointSize( 12 );
    pButton2->setFont( pButton2_font ); 
    pButton2->setText( trUtf8( "&Select All" ) );
    QToolTip::add( pButton2, trUtf8( "Here you can selected all list of record keys" ) );
    Layout2->addWidget( pButton2 );
    QSpacerItem* spacer_3 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout2->addItem( spacer_3 );

    pButton3 = new QPushButton( privateLayoutWidget, "pButton3" );
    pButton3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, pButton3->sizePolicy().hasHeightForWidth() ) );
    QFont pButton3_font(  pButton3->font() );
    pButton3_font.setFamily( "Lucida Sans Unicode" );
    pButton3_font.setPointSize( 12 );
    pButton3->setFont( pButton3_font ); 
    pButton3->setText( trUtf8( "C&lear All" ) );
    QToolTip::add( pButton3, trUtf8( "Here you can clearing all selection" ) );
    Layout2->addWidget( pButton3 );
    QSpacerItem* spacer_4 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout2->addItem( spacer_4 );

    PushButton1 = new QPushButton( privateLayoutWidget, "PushButton1" );
    PushButton1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, PushButton1->sizePolicy().hasHeightForWidth() ) );
    QFont PushButton1_font(  PushButton1->font() );
    PushButton1_font.setFamily( "Lucida Sans Unicode" );
    PushButton1_font.setPointSize( 12 );
    PushButton1->setFont( PushButton1_font ); 
    PushButton1->setText( trUtf8( "&Ok" ) );
    PushButton1->setDefault( TRUE );
    Layout2->addWidget( PushButton1 );

    // signals and slots connections
    connect( PushButton1, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( PushButton3, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( pFilterButton, SIGNAL( clicked() ), this, SLOT( CmFilter() ) );
    connect( pList, SIGNAL( doubleClicked(QListBoxItem*) ), this, SLOT( accept() ) );
    connect( pButton2, SIGNAL( clicked() ), this, SLOT( CmSelectAll() ) );
    connect( pButton3, SIGNAL( clicked() ), this, SLOT( CmClearAll() ) );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
KeyDialogData::~KeyDialogData()
{
    // no need to delete child widgets, Qt does it all for us
}

void KeyDialogData::CmFilter()
{
    qWarning( "KeyDialogData::CmFilter(): Not implemented yet!" );
}

void KeyDialogData::CmSelectAll()
{
    qWarning( "KeyDialogData::CmSelectAll(): Not implemented yet!" );
}

void KeyDialogData::CmClearAll()
{
    qWarning( "KeyDialogData::CmClearAll(): Not implemented yet!" );
}

