/****************************************************************************
** Form implementation generated from reading ui file 'SelectDialog.ui'
**
** Created: Do 28. Feb 17:25:53 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "SelectDialogData.h"

#include <qvariant.h>
#include <qlabel.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a SelectDialogData which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
SelectDialogData::SelectDialogData( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "SelectDialogData" );
    resize( 642, 454 ); 
    setCaption( trUtf8( "Multiple Selection Dialog" ) );
    QFont f( font() );
    f.setFamily( "Courier" );
    f.setPointSize( 9 );
    setFont( f ); 
    setMinimumSize( QSize( 640, 420 ) );
    setBaseSize( QSize( 0, 0 ) );
    SelectDialogDataLayout = new QVBoxLayout( this, 11, 6, "SelectDialogDataLayout"); 

    pLabel = new QLabel( this, "pLabel" );
    pLabel->setText( trUtf8( "Please, mark/select one or more items:" ) );
    pLabel->setAlignment( int( QLabel::AlignCenter ) );
    QFont pLabel_font(  pLabel->font() );
    pLabel_font.setFamily( "Lucida Sans Unicode" );
    pLabel_font.setPointSize( 12 );
    pLabel->setFont( pLabel_font ); 
    SelectDialogDataLayout->addWidget( pLabel );

    pList = new QListBox( this, "pList" );
    QFont pList_font(  pList->font() );
    pList->setFont( pList_font ); 
    pList->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)7, 0, 0, pList->sizePolicy().hasHeightForWidth() ) );
    SelectDialogDataLayout->addWidget( pList );

    hbox = new QHBoxLayout( 0, 6, 20, "hbox"); 

    PushButton1 = new QPushButton( this, "PushButton1" );
    PushButton1->setText( trUtf8( "&Ok" ) );
    PushButton1->setDefault( TRUE );
    QFont PushButton1_font(  PushButton1->font() );
    PushButton1_font.setFamily( "Lucida Sans Unicode" );
    PushButton1_font.setPointSize( 11 );
    PushButton1->setFont( PushButton1_font ); 
    PushButton1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, PushButton1->sizePolicy().hasHeightForWidth() ) );
    hbox->addWidget( PushButton1 );

    pButton2 = new QPushButton( this, "pButton2" );
    pButton2->setText( trUtf8( "&Select All" ) );
    QFont pButton2_font(  pButton2->font() );
    pButton2_font.setFamily( "Lucida Sans Unicode" );
    pButton2_font.setPointSize( 11 );
    pButton2->setFont( pButton2_font ); 
    pButton2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, pButton2->sizePolicy().hasHeightForWidth() ) );
    hbox->addWidget( pButton2 );

    pButton3 = new QPushButton( this, "pButton3" );
    pButton3->setText( trUtf8( "C&lear All" ) );
    QFont pButton3_font(  pButton3->font() );
    pButton3_font.setFamily( "Lucida Sans Unicode" );
    pButton3_font.setPointSize( 11 );
    pButton3->setFont( pButton3_font ); 
    pButton3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, pButton3->sizePolicy().hasHeightForWidth() ) );
    hbox->addWidget( pButton3 );

    PushButton4 = new QPushButton( this, "PushButton4" );
    PushButton4->setText( trUtf8( "&Cancel" ) );
    QFont PushButton4_font(  PushButton4->font() );
    PushButton4_font.setFamily( "Lucida Sans Unicode" );
    PushButton4_font.setPointSize( 11 );
    PushButton4->setFont( PushButton4_font ); 
    PushButton4->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, PushButton4->sizePolicy().hasHeightForWidth() ) );
    hbox->addWidget( PushButton4 );
    SelectDialogDataLayout->addLayout( hbox );

    // signals and slots connections
    connect( PushButton1, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( PushButton4, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( pButton2, SIGNAL( clicked() ), this, SLOT( CmSelectAll() ) );
    connect( pButton3, SIGNAL( clicked() ), this, SLOT( CmClearAll() ) );
    connect( pList, SIGNAL( doubleClicked(QListBoxItem*) ), this, SLOT( accept() ) );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
SelectDialogData::~SelectDialogData()
{
    // no need to delete child widgets, Qt does it all for us
}

void SelectDialogData::CmSelectAll()
{
    qWarning( "SelectDialogData::CmSelectAll(): Not implemented yet!" );
}

void SelectDialogData::CmClearAll()
{
    qWarning( "SelectDialogData::CmClearAll(): Not implemented yet!" );
}

