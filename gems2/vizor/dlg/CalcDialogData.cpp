/****************************************************************************
** Form implementation generated from reading ui file 'CalcDialog.ui'
**
** Created: Do 28. Feb 13:33:32 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "CalcDialogData.h"

#include <qvariant.h>
#include <qbuttongroup.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a CalcDialogData which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
CalcDialogData::CalcDialogData( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "CalcDialogData" );
    resize( 493, 284 ); 
    setCaption( trUtf8( "Data Object Cell Calculator" ) );
    QFont f( font() );
    f.setFamily( "Lucida Sans Unicode" );
    f.setPointSize( 8 );
    setFont( f ); 

    TextLabel1_3 = new QLabel( this, "TextLabel1_3" );
    TextLabel1_3->setGeometry( QRect( 240, 10, 170, 20 ) ); 
    TextLabel1_3->setText( trUtf8( "Value of Second Operand" ) );

    pButtonBox = new QButtonGroup( this, "pButtonBox" );
    pButtonBox->setGeometry( QRect( 20, 40, 200, 180 ) ); 
    pButtonBox->setTitle( trUtf8( "   Operation" ) );
    pButtonBox->setExclusive( TRUE );
    QToolTip::add( pButtonBox, trUtf8( "After any operation, previous contents of all selected cells will be lost!" ) );

    PushButton3 = new QPushButton( pButtonBox, "PushButton3" );
    PushButton3->setGeometry( QRect( 60, 50, 40, 28 ) ); 
    PushButton3->setText( trUtf8( "&-" ) );
    PushButton3->setToggleButton( TRUE );
    pButtonBox->insert( PushButton3, 2 );
    QToolTip::add( PushButton3, trUtf8( "Subtract a Value from Cell" ) );

    PushButton4 = new QPushButton( pButtonBox, "PushButton4" );
    PushButton4->setGeometry( QRect( 100, 50, 40, 28 ) ); 
    PushButton4->setText( trUtf8( "&*" ) );
    PushButton4->setToggleButton( TRUE );
    pButtonBox->insert( PushButton4, 3 );
    QToolTip::add( PushButton4, trUtf8( "Multiply Cell to a Value" ) );

    PushButton5 = new QPushButton( pButtonBox, "PushButton5" );
    PushButton5->setGeometry( QRect( 140, 50, 40, 28 ) ); 
    PushButton5->setText( trUtf8( "&/" ) );
    PushButton5->setToggleButton( TRUE );
    pButtonBox->insert( PushButton5, 4 );
    QToolTip::add( PushButton5, trUtf8( "Divide Cell by a Value (except 0)" ) );

    PushButton6 = new QPushButton( pButtonBox, "PushButton6" );
    PushButton6->setGeometry( QRect( 20, 80, 40, 28 ) ); 
    PushButton6->setText( trUtf8( "&^" ) );
    PushButton6->setToggleButton( TRUE );
    pButtonBox->insert( PushButton6, 5 );
    QToolTip::add( PushButton6, trUtf8( "Raise Cell to a Value Power" ) );

    PushButton7 = new QPushButton( pButtonBox, "PushButton7" );
    PushButton7->setGeometry( QRect( 100, 80, 40, 28 ) ); 
    PushButton7->setText( trUtf8( "&1/x" ) );
    PushButton7->setToggleButton( TRUE );
    pButtonBox->insert( PushButton7, 6 );
    QToolTip::add( PushButton7, trUtf8( "Invert Cell" ) );

    PushButton8 = new QPushButton( pButtonBox, "PushButton8" );
    PushButton8->setGeometry( QRect( 140, 80, 40, 28 ) ); 
    PushButton8->setText( trUtf8( "s&qrt" ) );
    PushButton8->setToggleButton( TRUE );
    pButtonBox->insert( PushButton8, 7 );
    QToolTip::add( PushButton8, trUtf8( "Square Root from (non-negative) Cell" ) );

    PushButton9 = new QPushButton( pButtonBox, "PushButton9" );
    PushButton9->setGeometry( QRect( 20, 110, 40, 28 ) ); 
    PushButton9->setText( trUtf8( "&log" ) );
    PushButton9->setToggleButton( TRUE );
    pButtonBox->insert( PushButton9, 8 );
    QToolTip::add( PushButton9, trUtf8( "Decimal Logarithm from (positive) Cell" ) );

    PushButton10 = new QPushButton( pButtonBox, "PushButton10" );
    PushButton10->setGeometry( QRect( 60, 110, 40, 28 ) ); 
    PushButton10->setText( trUtf8( "l&n" ) );
    PushButton10->setToggleButton( TRUE );
    pButtonBox->insert( PushButton10, 9 );
    QToolTip::add( PushButton10, trUtf8( "Natural Logarithm from (positive) Cell" ) );

    PushButton12 = new QPushButton( pButtonBox, "PushButton12" );
    PushButton12->setGeometry( QRect( 140, 110, 40, 28 ) ); 
    PushButton12->setText( trUtf8( "&exp" ) );
    PushButton12->setToggleButton( TRUE );
    pButtonBox->insert( PushButton12, 11 );
    QToolTip::add( PushButton12, trUtf8( "Exponent from Cell" ) );

    PushButton13 = new QPushButton( pButtonBox, "PushButton13" );
    PushButton13->setGeometry( QRect( 20, 140, 40, 28 ) ); 
    PushButton13->setText( trUtf8( "&sin" ) );
    PushButton13->setToggleButton( TRUE );
    pButtonBox->insert( PushButton13, 12 );
    QToolTip::add( PushButton13, trUtf8( "Sine from Cell (Radians)" ) );

    PushButton15 = new QPushButton( pButtonBox, "PushButton15" );
    PushButton15->setGeometry( QRect( 100, 140, 40, 28 ) ); 
    PushButton15->setText( trUtf8( "&tg" ) );
    PushButton15->setToggleButton( TRUE );
    pButtonBox->insert( PushButton15, 14 );
    QToolTip::add( PushButton15, trUtf8( "Tangent from Cell" ) );

    PushButton14 = new QPushButton( pButtonBox, "PushButton14" );
    PushButton14->setGeometry( QRect( 60, 140, 40, 28 ) ); 
    PushButton14->setText( trUtf8( "c&os" ) );
    PushButton14->setToggleButton( TRUE );
    pButtonBox->insert( PushButton14, 13 );
    QToolTip::add( PushButton14, trUtf8( "Cosine from Cell (Radians)" ) );

    PushButton11 = new QPushButton( pButtonBox, "PushButton11" );
    PushButton11->setGeometry( QRect( 100, 110, 40, 28 ) ); 
    PushButton11->setText( trUtf8( "1&0^x" ) );
    PushButton11->setToggleButton( TRUE );
    pButtonBox->insert( PushButton11, 10 );
    QToolTip::add( PushButton11, trUtf8( "Decimal Exponent from Cell" ) );

    pAssignBtn = new QPushButton( pButtonBox, "pAssignBtn" );
    pAssignBtn->setGeometry( QRect( 20, 20, 80, 30 ) ); 
    pAssignBtn->setText( trUtf8( "&=" ) );
    pAssignBtn->setToggleButton( TRUE );
    pAssignBtn->setOn( TRUE );
    pButtonBox->insert( pAssignBtn, 0 );
    QToolTip::add( pAssignBtn, trUtf8( "Set / Assign above Value to Cell" ) );

    PushButton2 = new QPushButton( pButtonBox, "PushButton2" );
    PushButton2->setGeometry( QRect( 20, 50, 40, 28 ) ); 
    PushButton2->setText( trUtf8( "&+" ) );
    PushButton2->setToggleButton( TRUE );
    pButtonBox->insert( PushButton2, 1 );
    QToolTip::add( PushButton2, trUtf8( "Add a Value to Cell" ) );

    pClearBtn = new QPushButton( pButtonBox, "pClearBtn" );
    pClearBtn->setGeometry( QRect( 140, 20, 40, 28 ) ); 
    pClearBtn->setText( trUtf8( "n.&d." ) );
    pClearBtn->setToggleButton( TRUE );
    QToolTip::add( pClearBtn, trUtf8( "Set Cell to 'No Data' Value" ) );

    pValue = new QLineEdit( this, "pValue" );
    pValue->setGeometry( QRect( 20, 10, 200, 22 ) ); 
    QToolTip::add( pValue, trUtf8( "Type in a value of second operand for binary operations" ) );

    TextLabel1_4 = new QLabel( this, "TextLabel1_4" );
    TextLabel1_4->setGeometry( QRect( 230, 200, 250, 21 ) ); 
    TextLabel1_4->setText( trUtf8( " Each cell is taken as  First Operand" ) );

    pObjectBox = new QGroupBox( this, "pObjectBox" );
    pObjectBox->setGeometry( QRect( 230, 40, 240, 150 ) ); 
    pObjectBox->setTitle( trUtf8( "Object Cell Selection Range" ) );
    QToolTip::add( pObjectBox, trUtf8( "Operations will be applied only to the selected range of cells" ) );

    PushButton22 = new QPushButton( pObjectBox, "PushButton22" );
    PushButton22->setGeometry( QRect( 70, 110, 150, 28 ) ); 
    PushButton22->setText( trUtf8( "Select &All Cells" ) );
    QToolTip::add( PushButton22, trUtf8( "Press this button to select all cells in a vector or matrix" ) );

    pM2 = new QSpinBox( pObjectBox, "pM2" );
    pM2->setGeometry( QRect( 160, 70, 60, 23 ) ); 

    TextLabel1_2_3 = new QLabel( pObjectBox, "TextLabel1_2_3" );
    TextLabel1_2_3->setGeometry( QRect( 130, 70, 20, 20 ) ); 
    TextLabel1_2_3->setText( trUtf8( "to" ) );
    TextLabel1_2_3->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

    pN2 = new QSpinBox( pObjectBox, "pN2" );
    pN2->setGeometry( QRect( 160, 30, 60, 23 ) ); 

    pN1 = new QSpinBox( pObjectBox, "pN1" );
    pN1->setGeometry( QRect( 70, 30, 60, 23 ) ); 

    TextLabel1_2 = new QLabel( pObjectBox, "TextLabel1_2" );
    TextLabel1_2->setGeometry( QRect( 130, 30, 20, 20 ) ); 
    TextLabel1_2->setText( trUtf8( "to" ) );
    TextLabel1_2->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

    pM1 = new QSpinBox( pObjectBox, "pM1" );
    pM1->setGeometry( QRect( 70, 70, 60, 23 ) ); 

    TextLabel1 = new QLabel( pObjectBox, "TextLabel1" );
    TextLabel1->setGeometry( QRect( 10, 30, 60, 20 ) ); 
    TextLabel1->setText( trUtf8( "N (rows)" ) );
    TextLabel1->setAlignment( int( QLabel::AlignCenter ) );

    TextLabel1_2_2 = new QLabel( pObjectBox, "TextLabel1_2_2" );
    TextLabel1_2_2->setGeometry( QRect( 10, 70, 60, 20 ) ); 
    TextLabel1_2_2->setText( trUtf8( "M (cols)" ) );
    TextLabel1_2_2->setAlignment( int( QLabel::AlignCenter ) );

    PushButton1_3 = new QPushButton( this, "PushButton1_3" );
    PushButton1_3->setGeometry( QRect( 40, 230, 120, 30 ) ); 
    PushButton1_3->setText( trUtf8( "&Ok - Do it!" ) );
    PushButton1_3->setDefault( TRUE );
    QFont PushButton1_3_font(  PushButton1_3->font() );
    PushButton1_3_font.setPointSize( 9 );
    PushButton1_3->setFont( PushButton1_3_font ); 

    PushButton1_2 = new QPushButton( this, "PushButton1_2" );
    PushButton1_2->setGeometry( QRect( 240, 230, 100, 30 ) ); 
    PushButton1_2->setText( trUtf8( "&Cancel" ) );
    QFont PushButton1_2_font(  PushButton1_2->font() );
    PushButton1_2_font.setPointSize( 9 );
    PushButton1_2->setFont( PushButton1_2_font ); 

    PushButton42 = new QPushButton( this, "PushButton42" );
    PushButton42->setGeometry( QRect( 371, 230, 100, 30 ) ); 
    PushButton42->setText( trUtf8( "&Help" ) );
    QFont PushButton42_font(  PushButton42->font() );
    PushButton42_font.setPointSize( 9 );
    PushButton42->setFont( PushButton42_font ); 

    // signals and slots connections
    connect( pClearBtn, SIGNAL( clicked() ), this, SLOT( unaryMode() ) );
    connect( PushButton7, SIGNAL( clicked() ), this, SLOT( unaryMode() ) );
    connect( PushButton8, SIGNAL( clicked() ), this, SLOT( unaryMode() ) );
    connect( PushButton9, SIGNAL( clicked() ), this, SLOT( unaryMode() ) );
    connect( PushButton10, SIGNAL( clicked() ), this, SLOT( unaryMode() ) );
    connect( PushButton11, SIGNAL( clicked() ), this, SLOT( unaryMode() ) );
    connect( PushButton12, SIGNAL( clicked() ), this, SLOT( unaryMode() ) );
    connect( PushButton13, SIGNAL( clicked() ), this, SLOT( unaryMode() ) );
    connect( PushButton14, SIGNAL( clicked() ), this, SLOT( unaryMode() ) );
    connect( PushButton15, SIGNAL( clicked() ), this, SLOT( unaryMode() ) );
    connect( PushButton22, SIGNAL( clicked() ), this, SLOT( selectAll() ) );
    connect( PushButton1_3, SIGNAL( clicked() ), this, SLOT( ok() ) );
    connect( PushButton1_2, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( PushButton1_2, SIGNAL( clicked() ), this, SLOT( unaryMode() ) );
    connect( pAssignBtn, SIGNAL( clicked() ), this, SLOT( binaryMode() ) );
    connect( PushButton2, SIGNAL( clicked() ), this, SLOT( binaryMode() ) );
    connect( PushButton3, SIGNAL( clicked() ), this, SLOT( binaryMode() ) );
    connect( PushButton4, SIGNAL( clicked() ), this, SLOT( binaryMode() ) );
    connect( PushButton5, SIGNAL( clicked() ), this, SLOT( binaryMode() ) );
    connect( PushButton6, SIGNAL( clicked() ), this, SLOT( binaryMode() ) );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
CalcDialogData::~CalcDialogData()
{
    // no need to delete child widgets, Qt does it all for us
}

void CalcDialogData::binaryMode()
{
    qWarning( "CalcDialogData::binaryMode(): Not implemented yet!" );
}

void CalcDialogData::ok()
{
    qWarning( "CalcDialogData::ok(): Not implemented yet!" );
}

void CalcDialogData::selectAll()
{
    qWarning( "CalcDialogData::selectAll(): Not implemented yet!" );
}

void CalcDialogData::unaryMode()
{
    qWarning( "CalcDialogData::unaryMode(): Not implemented yet!" );
}

