/****************************************************************************
** Form implementation generated from reading ui file 'DBAdvDialog.ui'
**
** Created: Mo 4. Mrz 11:35:33 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "DBAdvDialogData.h"

#include <qvariant.h>
#include <qframe.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a DBAdvDialogData which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 */
DBAdvDialogData::DBAdvDialogData( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "DBAdvDialogData" );
    resize( 420, 501 ); 
    setMinimumSize( QSize( 420, 500 ) );
    setCaption( trUtf8( "DBAdvDialogData" ) );
    QFont f( font() );
    f.setFamily( "Lucida Sans Unicode" );
    f.setPointSize( 9 );
    setFont( f ); 
    setBaseSize( QSize( 0, 0 ) );
    QToolTip::add( this, trUtf8( "Get miscellaneous information about maintaining databases" ) );

    Line1 = new QFrame( this, "Line1" );
    Line1->setGeometry( QRect( 10, 430, 390, 20 ) ); 
    Line1->setFrameStyle( QFrame::HLine | QFrame::Sunken );
    Line1->setFrameShadow( QFrame::Sunken );
    Line1->setProperty( "frameShape", (int)QFrame::HLine );

    QWidget* privateLayoutWidget = new QWidget( this, "vbox2" );
    privateLayoutWidget->setGeometry( QRect( 40, 330, 360, 100 ) ); 
    vbox2 = new QVBoxLayout( privateLayoutWidget, 0, 0, "vbox2"); 

    pButtonDUTerm = new QPushButton( privateLayoutWidget, "pButtonDUTerm" );
    pButtonDUTerm->setText( trUtf8( "Du&al Thermodynamics (DUtherm)" ) );
    pButtonDUTerm->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, pButtonDUTerm->sizePolicy().hasHeightForWidth() ) );
    QFont pButtonDUTerm_font(  pButtonDUTerm->font() );
    pButtonDUTerm_font.setPointSize( 8 );
    pButtonDUTerm->setFont( pButtonDUTerm_font ); 
    QToolTip::add( pButtonDUTerm, trUtf8( "" ) );
    vbox2->addWidget( pButtonDUTerm );

    pButtonAdProbe = new QPushButton( privateLayoutWidget, "pButtonAdProbe" );
    pButtonAdProbe->setText( trUtf8( "Adaptive Models and &Uncertainty Spaces " ) );
    pButtonAdProbe->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, pButtonAdProbe->sizePolicy().hasHeightForWidth() ) );
    QFont pButtonAdProbe_font(  pButtonAdProbe->font() );
    pButtonAdProbe_font.setPointSize( 8 );
    pButtonAdProbe->setFont( pButtonAdProbe_font ); 
    QToolTip::add( pButtonAdProbe, trUtf8( "" ) );
    vbox2->addWidget( pButtonAdProbe );

    pButtonInteg = new QPushButton( privateLayoutWidget, "pButtonInteg" );
    pButtonInteg->setText( trUtf8( "Coupled &Mass Transport in a Megasystem" ) );
    pButtonInteg->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, pButtonInteg->sizePolicy().hasHeightForWidth() ) );
    QFont pButtonInteg_font(  pButtonInteg->font() );
    pButtonInteg_font.setPointSize( 8 );
    pButtonInteg->setFont( pButtonInteg_font ); 
    QToolTip::add( pButtonInteg, trUtf8( "" ) );
    vbox2->addWidget( pButtonInteg );

    TextLabel1_2 = new QLabel( this, "TextLabel1_2" );
    TextLabel1_2->setGeometry( QRect( 40, 310, 160, 21 ) ); 
    TextLabel1_2->setText( trUtf8( "Under Construction:" ) );
    QFont TextLabel1_2_font(  TextLabel1_2->font() );
    TextLabel1_2_font.setItalic( TRUE );
    TextLabel1_2->setFont( TextLabel1_2_font ); 

    QWidget* privateLayoutWidget_2 = new QWidget( this, "vbox1" );
    privateLayoutWidget_2->setGeometry( QRect( 10, 80, 390, 220 ) ); 
    vbox1 = new QVBoxLayout( privateLayoutWidget_2, 0, 0, "vbox1"); 

    pButtonSDref = new QPushButton( privateLayoutWidget_2, "pButtonSDref" );
    pButtonSDref->setText( trUtf8( "B&ibliography (SDref)" ) );
    pButtonSDref->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, pButtonSDref->sizePolicy().hasHeightForWidth() ) );
    QFont pButtonSDref_font(  pButtonSDref->font() );
    pButtonSDref_font.setPointSize( 11 );
    pButtonSDref->setFont( pButtonSDref_font ); 
    QToolTip::add( pButtonSDref, trUtf8( "Full references (opt. abstracts) to sources of input data" ) );
    vbox1->addWidget( pButtonSDref );

    pButtonConst = new QPushButton( privateLayoutWidget_2, "pButtonConst" );
    pButtonConst->setText( trUtf8( "Numerical &Constants (Const)" ) );
    pButtonConst->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, pButtonConst->sizePolicy().hasHeightForWidth() ) );
    QFont pButtonConst_font(  pButtonConst->font() );
    pButtonConst_font.setPointSize( 11 );
    pButtonConst->setFont( pButtonConst_font ); 
    QToolTip::add( pButtonConst, trUtf8( "Some tables of constants used in thermodynamic calculations" ) );
    vbox1->addWidget( pButtonConst );

    pButtonProfile = new QPushButton( privateLayoutWidget_2, "pButtonProfile" );
    pButtonProfile->setText( trUtf8( "Chemical Model &Profile (Profile)" ) );
    pButtonProfile->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, pButtonProfile->sizePolicy().hasHeightForWidth() ) );
    QFont pButtonProfile_font(  pButtonProfile->font() );
    pButtonProfile_font.setPointSize( 11 );
    pButtonProfile->setFont( pButtonProfile_font ); 
    QToolTip::add( pButtonProfile, trUtf8( "Modeling application profile of database files, phases and species" ) );
    vbox1->addWidget( pButtonProfile );

    pButtonSysEq = new QPushButton( privateLayoutWidget_2, "pButtonSysEq" );
    pButtonSysEq->setText( trUtf8( "Single Chemical Sys&tem (SysEq)" ) );
    pButtonSysEq->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, pButtonSysEq->sizePolicy().hasHeightForWidth() ) );
    QFont pButtonSysEq_font(  pButtonSysEq->font() );
    pButtonSysEq_font.setPointSize( 11 );
    pButtonSysEq->setFont( pButtonSysEq_font ); 
    QToolTip::add( pButtonSysEq, trUtf8( "Definitions of chemical systems and equilibrium states" ) );
    vbox1->addWidget( pButtonSysEq );

    pButtonProcess = new QPushButton( privateLayoutWidget_2, "pButtonProcess" );
    pButtonProcess->setText( trUtf8( "&Process Simulators (Process)" ) );
    pButtonProcess->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, pButtonProcess->sizePolicy().hasHeightForWidth() ) );
    QFont pButtonProcess_font(  pButtonProcess->font() );
    pButtonProcess_font.setPointSize( 11 );
    pButtonProcess->setFont( pButtonProcess_font ); 
    QToolTip::add( pButtonProcess, trUtf8( "Definitions of process simulators (batch GEM calculations)" ) );
    vbox1->addWidget( pButtonProcess );

    pButtonGTdemo = new QPushButton( privateLayoutWidget_2, "pButtonGTdemo" );
    pButtonGTdemo->setText( trUtf8( "Data &Samplers and Plotters (GTdemo)" ) );
    pButtonGTdemo->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, pButtonGTdemo->sizePolicy().hasHeightForWidth() ) );
    QFont pButtonGTdemo_font(  pButtonGTdemo->font() );
    pButtonGTdemo_font.setPointSize( 11 );
    pButtonGTdemo->setFont( pButtonGTdemo_font ); 
    QToolTip::add( pButtonGTdemo, trUtf8( "Definitions of data samplers and graphics" ) );
    vbox1->addWidget( pButtonGTdemo );

    QWidget* privateLayoutWidget_3 = new QWidget( this, "Layout20" );
    privateLayoutWidget_3->setGeometry( QRect( 10, 450, 390, 37 ) ); 
    Layout20 = new QHBoxLayout( privateLayoutWidget_3, 0, 6, "Layout20"); 

    pButtonBack = new QPushButton( privateLayoutWidget_3, "pButtonBack" );
    pButtonBack->setText( trUtf8( "&Back" ) );
    pButtonBack->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, pButtonBack->sizePolicy().hasHeightForWidth() ) );
    QFont pButtonBack_font(  pButtonBack->font() );
    pButtonBack_font.setPointSize( 10 );
    pButtonBack->setFont( pButtonBack_font ); 
    pButtonBack->setDefault( FALSE );
    QToolTip::add( pButtonBack, trUtf8( "Back to 'Thermodynamic database management' dialog" ) );
    Layout20->addWidget( pButtonBack );

    pButtonHelp = new QPushButton( privateLayoutWidget_3, "pButtonHelp" );
    pButtonHelp->setText( trUtf8( "&Help" ) );
    pButtonHelp->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, pButtonHelp->sizePolicy().hasHeightForWidth() ) );
    QFont pButtonHelp_font(  pButtonHelp->font() );
    pButtonHelp_font.setPointSize( 10 );
    pButtonHelp->setFont( pButtonHelp_font ); 
    pButtonHelp->setDefault( TRUE );
    Layout20->addWidget( pButtonHelp );

    pButtonFormats = new QPushButton( privateLayoutWidget_3, "pButtonFormats" );
    pButtonFormats->setText( trUtf8( "Formats" ) );
    QFont pButtonFormats_font(  pButtonFormats->font() );
    pButtonFormats_font.setPointSize( 10 );
    pButtonFormats->setFont( pButtonFormats_font ); 
    pButtonFormats->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, pButtonFormats->sizePolicy().hasHeightForWidth() ) );
    QToolTip::add( pButtonFormats, trUtf8( "Get descriptions of database and other file formats" ) );
    Layout20->addWidget( pButtonFormats );

    tlTitle = new QLabel( this, "tlTitle" );
    tlTitle->setGeometry( QRect( 70, 10, 330, 60 ) ); 
    tlTitle->setText( trUtf8( "Database Management (2)" ) );
    tlTitle->setAlignment( int( QLabel::AlignCenter ) );
    tlTitle->setFrameShape( QLabel::Panel );
    tlTitle->setFrameShadow( QLabel::Raised );
    QFont tlTitle_font(  tlTitle->font() );
    tlTitle_font.setPointSize( 13 );
    tlTitle_font.setBold( TRUE );
    tlTitle_font.setItalic( TRUE );
    tlTitle->setFont( tlTitle_font ); 

    pLogoImg = new QLabel( this, "pLogoImg" );
    pLogoImg->setGeometry( QRect( 10, 10, 60, 60 ) ); 
    pLogoImg->setScaledContents( TRUE );
    pLogoImg->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, pLogoImg->sizePolicy().hasHeightForWidth() ) );
    pLogoImg->setFrameShadow( QLabel::Raised );
    pLogoImg->setFrameShape( QLabel::Panel );
    pLogoImg->setAlignment( int( QLabel::AlignCenter ) );

    // signals and slots connections
    connect( pButtonBack, SIGNAL( clicked() ), this, SLOT( CmBack() ) );
    connect( pButtonHelp, SIGNAL( clicked() ), this, SLOT( CmHelp() ) );
    connect( pButtonFormats, SIGNAL( clicked() ), this, SLOT( CmFormats() ) );
    connect( pButtonSDref, SIGNAL( clicked() ), this, SLOT( CmOpen_SData() ) );
    connect( pButtonConst, SIGNAL( clicked() ), this, SLOT( CmOpen_Const() ) );
    connect( pButtonProfile, SIGNAL( clicked() ), this, SLOT( CmOpen_PROFILE() ) );
    connect( pButtonSysEq, SIGNAL( clicked() ), this, SLOT( CmThermoSystem() ) );
    connect( pButtonProcess, SIGNAL( clicked() ), this, SLOT( CmOpen_PROCESS() ) );
    connect( pButtonGTdemo, SIGNAL( clicked() ), this, SLOT( CmOpen_GTDEMO() ) );
    connect( pButtonDUTerm, SIGNAL( clicked() ), this, SLOT( CmOpen_DUTERM() ) );
    connect( pButtonAdProbe, SIGNAL( clicked() ), this, SLOT( CmOpen_PROBE() ) );
    connect( pButtonInteg, SIGNAL( clicked() ), this, SLOT( CmOpen_INTEG() ) );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
DBAdvDialogData::~DBAdvDialogData()
{
    // no need to delete child widgets, Qt does it all for us
}

void DBAdvDialogData::CmBack()
{
    qWarning( "DBAdvDialogData::CmBack(): Not implemented yet!" );
}

void DBAdvDialogData::CmHelp()
{
    qWarning( "DBAdvDialogData::CmHelp(): Not implemented yet!" );
}

void DBAdvDialogData::CmFormats()
{
    qWarning( "DBAdvDialogData::CmFormats(): Not implemented yet!" );
}

void DBAdvDialogData::CmOpen_PROCESS()
{
    qWarning( "DBAdvDialogData::CmOpen_PROCESS(): Not implemented yet!" );
}

void DBAdvDialogData::CmOpen_Const()
{
    qWarning( "DBAdvDialogData::CmOpen_Const(): Not implemented yet!" );
}

void DBAdvDialogData::CmOpen_DUTERM()
{
    qWarning( "DBAdvDialogData::CmOpen_DUTERM(): Not implemented yet!" );
}

void DBAdvDialogData::CmOpen_GTDEMO()
{
    qWarning( "DBAdvDialogData::CmOpen_GTDEMO(): Not implemented yet!" );
}

void DBAdvDialogData::CmOpen_INTEG()
{
    qWarning( "DBAdvDialogData::CmOpen_INTEG(): Not implemented yet!" );
}

void DBAdvDialogData::CmOpen_PROBE()
{
    qWarning( "DBAdvDialogData::CmOpen_PROBE(): Not implemented yet!" );
}

void DBAdvDialogData::CmOpen_PROFILE()
{
    qWarning( "DBAdvDialogData::CmOpen_PROFILE(): Not implemented yet!" );
}

void DBAdvDialogData::CmOpen_SData()
{
    qWarning( "DBAdvDialogData::CmOpen_SData(): Not implemented yet!" );
}

void DBAdvDialogData::CmThermoSystem()
{
    qWarning( "DBAdvDialogData::CmThermoSystem(): Not implemented yet!" );
}

