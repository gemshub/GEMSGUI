/****************************************************************************
** Form implementation generated from reading ui file 'CalcCheckDialog.ui'
**
** Created: Do 28. Feb 13:33:33 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "CalcCheckDialogData.h"

#include <qvariant.h>
#include <qcombobox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a CalcCheckDialogData which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
CalcCheckDialogData::CalcCheckDialogData( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "CalcCheckDialogData" );
    resize( 289, 256 ); 
    setCaption( trUtf8( "Change Value to Set Cells" ) );
    QFont f( font() );
    f.setFamily( "Lucida Sans Unicode" );
    f.setPointSize( 8 );
    setFont( f ); 

    pValue = new QComboBox( FALSE, this, "pValue" );
    pValue->setGeometry( QRect( 140, 20, 130, 20 ) ); 
    QToolTip::add( pValue, trUtf8( "All selected cells will be set to a value that you type into this field" ) );

    TextLabel2 = new QLabel( this, "TextLabel2" );
    TextLabel2->setGeometry( QRect( 20, 20, 110, 20 ) ); 
    TextLabel2->setText( trUtf8( "Value to Set Cells" ) );
    QFont TextLabel2_font(  TextLabel2->font() );
    TextLabel2->setFont( TextLabel2_font ); 

    pObjectBox = new QGroupBox( this, "pObjectBox" );
    pObjectBox->setGeometry( QRect( 20, 60, 250, 140 ) ); 
    pObjectBox->setTitle( trUtf8( "Object Cell Selection" ) );
    QFont pObjectBox_font(  pObjectBox->font() );
    pObjectBox->setFont( pObjectBox_font ); 

    TextLabel1_2 = new QLabel( pObjectBox, "TextLabel1_2" );
    TextLabel1_2->setGeometry( QRect( 140, 20, 20, 20 ) ); 
    TextLabel1_2->setText( trUtf8( "to" ) );
    TextLabel1_2->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

    TextLabel1 = new QLabel( pObjectBox, "TextLabel1" );
    TextLabel1->setGeometry( QRect( 10, 20, 61, 20 ) ); 
    TextLabel1->setText( trUtf8( "N: from" ) );
    TextLabel1->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

    TextLabel1_2_2 = new QLabel( pObjectBox, "TextLabel1_2_2" );
    TextLabel1_2_2->setGeometry( QRect( 10, 60, 61, 20 ) ); 
    TextLabel1_2_2->setText( trUtf8( "M:" ) );
    TextLabel1_2_2->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

    pM = new QLabel( pObjectBox, "pM" );
    pM->setGeometry( QRect( 80, 60, 60, 20 ) ); 
    pM->setFrameShape( QLabel::Panel );
    pM->setFrameShadow( QLabel::Raised );
    pM->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignLeft ) );

    pN2 = new QSpinBox( pObjectBox, "pN2" );
    pN2->setGeometry( QRect( 170, 20, 60, 23 ) ); 

    pN1 = new QSpinBox( pObjectBox, "pN1" );
    pN1->setGeometry( QRect( 80, 20, 60, 23 ) ); 

    PushButton3 = new QPushButton( pObjectBox, "PushButton3" );
    PushButton3->setGeometry( QRect( 80, 100, 150, 28 ) ); 
    PushButton3->setText( trUtf8( "Select All Cells" ) );
    QToolTip::add( PushButton3, trUtf8( "Press this button to select all cells in a vector or matrix object " ) );

    PushButton1_2 = new QPushButton( this, "PushButton1_2" );
    PushButton1_2->setGeometry( QRect( 180, 210, 90, 28 ) ); 
    PushButton1_2->setText( trUtf8( "&Cancel" ) );
    QFont PushButton1_2_font(  PushButton1_2->font() );
    PushButton1_2_font.setPointSize( 9 );
    PushButton1_2->setFont( PushButton1_2_font ); 

    PushButton1 = new QPushButton( this, "PushButton1" );
    PushButton1->setGeometry( QRect( 20, 210, 90, 28 ) ); 
    PushButton1->setText( trUtf8( "&Ok" ) );
    PushButton1->setDefault( TRUE );
    QFont PushButton1_font(  PushButton1->font() );
    PushButton1_font.setPointSize( 9 );
    PushButton1->setFont( PushButton1_font ); 

    // signals and slots connections
    connect( PushButton1_2, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( PushButton3, SIGNAL( clicked() ), this, SLOT( CmSelectAll() ) );
    connect( PushButton1, SIGNAL( clicked() ), this, SLOT( CmOk() ) );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
CalcCheckDialogData::~CalcCheckDialogData()
{
    // no need to delete child widgets, Qt does it all for us
}

void CalcCheckDialogData::CmSelectAll()
{
    qWarning( "CalcCheckDialogData::CmSelectAll(): Not implemented yet!" );
}

void CalcCheckDialogData::CmOk()
{
    qWarning( "CalcCheckDialogData::CmOk(): Not implemented yet!" );
}

