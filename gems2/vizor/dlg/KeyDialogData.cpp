/****************************************************************************
** Form implementation generated from reading ui file 'KeyDialog.ui'
**
** Created: Fri Nov 16 14:53:52 2001
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "KeyDialogData.h"

#include <qvariant.h>   // first for gcc 2.7.2
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
    setCaption( trUtf8( "List of database record keys" ) );
    QFont f( font() );
    f.setFamily( "Courier" );
    f.setPointSize( 9 );
    setFont( f ); 
    KeyDialogDataLayout = new QVBoxLayout( this, 11, 6, "KeyDialogDataLayout"); 

    pLabel = new QLabel( this, "pLabel" );
    pLabel->setText( trUtf8( "Please, select one record key" ) );
    pLabel->setAlignment( int( QLabel::AlignCenter ) );
    QFont pLabel_font(  pLabel->font() );
    pLabel_font.setFamily( "Lucida Sans Unicode" );
    pLabel_font.setPointSize( 12 );
    pLabel->setFont( pLabel_font ); 
    KeyDialogDataLayout->addWidget( pLabel );

    pList = new QListBox( this, "pList" );
    QFont pList_font(  pList->font() );
    pList->setFont( pList_font ); 
    KeyDialogDataLayout->addWidget( pList );

    Layout6 = new QHBoxLayout( 0, 0, 6, "Layout6"); 

    PushButton1 = new QPushButton( this, "PushButton1" );
    PushButton1->setText( trUtf8( "&Ok" ) );
    PushButton1->setDefault( TRUE );
    QFont PushButton1_font(  PushButton1->font() );
    PushButton1_font.setFamily( "Lucida Sans Unicode" );
    PushButton1_font.setPointSize( 12 );
    PushButton1->setFont( PushButton1_font ); 
    PushButton1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, PushButton1->sizePolicy().hasHeightForWidth() ) );
    Layout6->addWidget( PushButton1 );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout6->addItem( spacer );

    pFilterButton = new QPushButton( this, "pFilterButton" );
    pFilterButton->setText( trUtf8( "&Filter..." ) );
    QFont pFilterButton_font(  pFilterButton->font() );
    pFilterButton_font.setFamily( "Lucida Sans Unicode" );
    pFilterButton_font.setPointSize( 12 );
    pFilterButton->setFont( pFilterButton_font ); 
    pFilterButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, pFilterButton->sizePolicy().hasHeightForWidth() ) );
    QToolTip::add( pFilterButton, trUtf8( "Here you can specify a template to filter a large list of record keys" ) );
    Layout6->addWidget( pFilterButton );
    QSpacerItem* spacer_2 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout6->addItem( spacer_2 );

    PushButton3 = new QPushButton( this, "PushButton3" );
    PushButton3->setText( trUtf8( "&Cancel" ) );
    QFont PushButton3_font(  PushButton3->font() );
    PushButton3_font.setFamily( "Lucida Sans Unicode" );
    PushButton3_font.setPointSize( 12 );
    PushButton3->setFont( PushButton3_font ); 
    PushButton3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, PushButton3->sizePolicy().hasHeightForWidth() ) );
    Layout6->addWidget( PushButton3 );
    KeyDialogDataLayout->addLayout( Layout6 );

    // signals and slots connections
    connect( PushButton1, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( PushButton3, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( pFilterButton, SIGNAL( clicked() ), this, SLOT( CmFilter() ) );
    connect( pList, SIGNAL( doubleClicked(QListBoxItem*) ), this, SLOT( accept() ) );
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

