/****************************************************************************
** Form implementation generated from reading ui file 'DBDialog.ui'
**
** Created: Do 28. Feb 17:25:49 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "DBDialogData.h"

#include <qvariant.h>
#include <qframe.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a DBDialogData which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 */
DBDialogData::DBDialogData( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "DBDialogData" );
    resize( 440, 441 ); 
    setMinimumSize( QSize( 440, 440 ) );
    setCaption( trUtf8( "DBDialogData" ) );
    QFont f( font() );
    f.setFamily( "Lucida Sans Unicode" );
    f.setPointSize( 8 );
    setFont( f ); 
    setBaseSize( QSize( 0, 0 ) );

    pImg1 = new QLabel( this, "pImg1" );
    pImg1->setGeometry( QRect( 10, 10, 60, 60 ) ); 
    pImg1->setFrameShadow( QLabel::Raised );
    pImg1->setFrameShape( QLabel::Panel );
    pImg1->setScaledContents( TRUE );
    pImg1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, pImg1->sizePolicy().hasHeightForWidth() ) );
    pImg1->setAlignment( int( QLabel::AlignCenter ) );

    QWidget* privateLayoutWidget = new QWidget( this, "hbox" );
    privateLayoutWidget->setGeometry( QRect( 20, 390, 390, 37 ) ); 
    hbox = new QHBoxLayout( privateLayoutWidget, 0, 6, "hbox"); 

    PushButton3 = new QPushButton( privateLayoutWidget, "PushButton3" );
    PushButton3->setText( trUtf8( "&Back" ) );
    PushButton3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, PushButton3->sizePolicy().hasHeightForWidth() ) );
    QFont PushButton3_font(  PushButton3->font() );
    PushButton3_font.setPointSize( 10 );
    PushButton3->setFont( PushButton3_font ); 
    PushButton3->setDefault( FALSE );
    QToolTip::add( PushButton3, trUtf8( "Returns to GEM-Selektor Main dialog" ) );
    hbox->addWidget( PushButton3 );

    PushButton4 = new QPushButton( privateLayoutWidget, "PushButton4" );
    PushButton4->setText( trUtf8( "&Other Data" ) );
    PushButton4->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, PushButton4->sizePolicy().hasHeightForWidth() ) );
    QFont PushButton4_font(  PushButton4->font() );
    PushButton4_font.setPointSize( 10 );
    PushButton4->setFont( PushButton4_font ); 
    QToolTip::add( PushButton4, trUtf8( "Press to manage records of other data types" ) );
    hbox->addWidget( PushButton4 );

    PushButton5 = new QPushButton( privateLayoutWidget, "PushButton5" );
    PushButton5->setText( trUtf8( "&Help" ) );
    PushButton5->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, PushButton5->sizePolicy().hasHeightForWidth() ) );
    QFont PushButton5_font(  PushButton5->font() );
    PushButton5_font.setPointSize( 10 );
    PushButton5->setFont( PushButton5_font ); 
    PushButton5->setDefault( TRUE );
    QToolTip::add( PushButton5, trUtf8( "Learn about structure and access mechanisms of thermodynamic database" ) );
    hbox->addWidget( PushButton5 );

    TextLabel1 = new QLabel( this, "TextLabel1" );
    TextLabel1->setGeometry( QRect( 70, 10, 360, 60 ) ); 
    TextLabel1->setText( trUtf8( "Thermodynamic Database Management" ) );
    TextLabel1->setAlignment( int( QLabel::WordBreak | QLabel::AlignCenter ) );
    TextLabel1->setFrameShape( QLabel::Panel );
    TextLabel1->setFrameShadow( QLabel::Raised );
    QFont TextLabel1_font(  TextLabel1->font() );
    TextLabel1_font.setPointSize( 13 );
    TextLabel1_font.setBold( TRUE );
    TextLabel1_font.setItalic( TRUE );
    TextLabel1->setFont( TextLabel1_font ); 

    QWidget* privateLayoutWidget_2 = new QWidget( this, "vbox" );
    privateLayoutWidget_2->setGeometry( QRect( 10, 130, 420, 120 ) ); 
    vbox = new QVBoxLayout( privateLayoutWidget_2, 0, 2, "vbox"); 

    PushButton1_2 = new QPushButton( privateLayoutWidget_2, "PushButton1_2" );
    PushButton1_2->setText( trUtf8( "&Dependent Components (DComp)" ) );
    PushButton1_2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, PushButton1_2->sizePolicy().hasHeightForWidth() ) );
    QFont PushButton1_2_font(  PushButton1_2->font() );
    PushButton1_2_font.setPointSize( 12 );
    PushButton1_2->setFont( PushButton1_2_font ); 
    QToolTip::add( PushButton1_2, trUtf8( "Formulae and standard-state thermochemical / EOS data  for species" ) );
    vbox->addWidget( PushButton1_2 );

    PushButton2_2 = new QPushButton( privateLayoutWidget_2, "PushButton2_2" );
    PushButton2_2->setText( trUtf8( "Dependent Components (&ReacDC)" ) );
    PushButton2_2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, PushButton2_2->sizePolicy().hasHeightForWidth() ) );
    QFont PushButton2_2_font(  PushButton2_2->font() );
    PushButton2_2_font.setPointSize( 12 );
    PushButton2_2->setFont( PushButton2_2_font ); 
    QToolTip::add( PushButton2_2, trUtf8( "Formulae and standard-state data for a species defined via reactions with other species" ) );
    vbox->addWidget( PushButton2_2 );

    PushButton1_2_2 = new QPushButton( privateLayoutWidget_2, "PushButton1_2_2" );
    PushButton1_2_2->setText( trUtf8( "&Temperature/Pressure Tables (RTparm)" ) );
    PushButton1_2_2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, PushButton1_2_2->sizePolicy().hasHeightForWidth() ) );
    QFont PushButton1_2_2_font(  PushButton1_2_2->font() );
    PushButton1_2_2_font.setPointSize( 12 );
    PushButton1_2_2->setFont( PushButton1_2_2_font ); 
    QToolTip::add( PushButton1_2_2, trUtf8( "Definitions of T-P grid, calculation and plotting of thermodynamic data for dependent components" ) );
    vbox->addWidget( PushButton1_2_2 );

    PushButton1 = new QPushButton( this, "PushButton1" );
    PushButton1->setGeometry( QRect( 10, 80, 420, 40 ) ); 
    PushButton1->setText( trUtf8( "&Independent Components (IComp)" ) );
    PushButton1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, PushButton1->sizePolicy().hasHeightForWidth() ) );
    QFont PushButton1_font(  PushButton1->font() );
    PushButton1_font.setPointSize( 12 );
    PushButton1->setFont( PushButton1_font ); 
    PushButton1->setDefault( FALSE );
    QToolTip::add( PushButton1, trUtf8( "Basic data describing stoichiometry units (eg. chemical elements)" ) );

    PushButton2_2_2 = new QPushButton( this, "PushButton2_2_2" );
    PushButton2_2_2->setGeometry( QRect( 10, 260, 420, 40 ) ); 
    PushButton2_2_2->setText( trUtf8( " Thermodynamic &Phases (Phase)" ) );
    PushButton2_2_2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, PushButton2_2_2->sizePolicy().hasHeightForWidth() ) );
    QFont PushButton2_2_2_font(  PushButton2_2_2->font() );
    PushButton2_2_2_font.setPointSize( 12 );
    PushButton2_2_2->setFont( PushButton2_2_2_font ); 
    QToolTip::add( PushButton2_2_2, trUtf8( "Definitions of single- and multi-component phases to construct chemical systems" ) );

    PushButton2 = new QPushButton( this, "PushButton2" );
    PushButton2->setGeometry( QRect( 10, 310, 420, 40 ) ); 
    PushButton2->setText( trUtf8( "Predefined &Compositions (Compos)" ) );
    PushButton2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, PushButton2->sizePolicy().hasHeightForWidth() ) );
    QFont PushButton2_font(  PushButton2->font() );
    PushButton2_font.setPointSize( 12 );
    PushButton2->setFont( PushButton2_font ); 
    QToolTip::add( PushButton2, trUtf8( "Definitions of stoichiometry entities for setting up bulk composition of chemical systems" ) );

    Line1 = new QFrame( this, "Line1" );
    Line1->setGeometry( QRect( 10, 360, 410, 20 ) ); 
    Line1->setFrameStyle( QFrame::HLine | QFrame::Sunken );

    // signals and slots connections
    connect( PushButton3, SIGNAL( clicked() ), this, SLOT( CmBack() ) );
    connect( PushButton4, SIGNAL( clicked() ), this, SLOT( CmAdvanced() ) );
    connect( PushButton5, SIGNAL( clicked() ), this, SLOT( CmHelp() ) );
    connect( PushButton1, SIGNAL( clicked() ), this, SLOT( CmOpen_ICOMP() ) );
    connect( PushButton2, SIGNAL( clicked() ), this, SLOT( CmOpen_COMPOS() ) );
    connect( PushButton1_2, SIGNAL( clicked() ), this, SLOT( CmOpen_DCOMP() ) );
    connect( PushButton2_2, SIGNAL( clicked() ), this, SLOT( CmOpen_REACDC() ) );
    connect( PushButton1_2_2, SIGNAL( clicked() ), this, SLOT( CmOpen_RTPARM() ) );
    connect( PushButton2_2_2, SIGNAL( clicked() ), this, SLOT( CmOpen_PHASE() ) );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
DBDialogData::~DBDialogData()
{
    // no need to delete child widgets, Qt does it all for us
}

void DBDialogData::CmAdvanced()
{
    qWarning( "DBDialogData::CmAdvanced(): Not implemented yet!" );
}

void DBDialogData::CmBack()
{
    qWarning( "DBDialogData::CmBack(): Not implemented yet!" );
}

void DBDialogData::CmHelp()
{
    qWarning( "DBDialogData::CmHelp(): Not implemented yet!" );
}

void DBDialogData::CmOpen_COMPOS()
{
    qWarning( "DBDialogData::CmOpen_COMPOS(): Not implemented yet!" );
}

void DBDialogData::CmOpen_DCOMP()
{
    qWarning( "DBDialogData::CmOpen_DCOMP(): Not implemented yet!" );
}

void DBDialogData::CmOpen_ICOMP()
{
    qWarning( "DBDialogData::CmOpen_ICOMP(): Not implemented yet!" );
}

void DBDialogData::CmOpen_REACDC()
{
    qWarning( "DBDialogData::CmOpen_REACDC(): Not implemented yet!" );
}

void DBDialogData::CmOpen_RTPARM()
{
    qWarning( "DBDialogData::CmOpen_RTPARM(): Not implemented yet!" );
}

void DBDialogData::CmOpen_PHASE()
{
    qWarning( "DBDialogData::CmOpen_PHASE(): Not implemented yet!" );
}

