/****************************************************************************
** Form implementation generated from reading ui file 'EQDialog.ui'
**
** Created: Mo 4. Mrz 11:35:28 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "EQDialogData.h"

#include <qvariant.h>
#include <qframe.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a EQDialogData which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 */
EQDialogData::EQDialogData( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "EQDialogData" );
    resize( 440, 441 ); 
    setMinimumSize( QSize( 440, 440 ) );
    setCaption( trUtf8( "EQDialogData" ) );
    QFont f( font() );
    f.setFamily( "Lucida Sans Unicode" );
    f.setPointSize( 8 );
    setFont( f ); 
    setBaseSize( QSize( 0, 0 ) );

    TextLabel2 = new QLabel( this, "TextLabel2" );
    TextLabel2->setGeometry( QRect( 40, 230, 171, 21 ) ); 
    TextLabel2->setText( trUtf8( "Under Construction:" ) );
    QFont TextLabel2_font(  TextLabel2->font() );
    TextLabel2_font.setPointSize( 9 );
    TextLabel2_font.setItalic( TRUE );
    TextLabel2->setFont( TextLabel2_font ); 

    pLogoImg = new QLabel( this, "pLogoImg" );
    pLogoImg->setGeometry( QRect( 10, 10, 60, 60 ) ); 
    pLogoImg->setScaledContents( TRUE );
    pLogoImg->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, pLogoImg->sizePolicy().hasHeightForWidth() ) );
    pLogoImg->setFrameShadow( QLabel::Raised );
    pLogoImg->setFrameShape( QLabel::Panel );
    pLogoImg->setAlignment( int( QLabel::AlignCenter ) );

    pTitleTL = new QLabel( this, "pTitleTL" );
    pTitleTL->setGeometry( QRect( 70, 10, 350, 60 ) ); 
    pTitleTL->setText( trUtf8( "Computation of Geochemical Equilibrium Models" ) );
    pTitleTL->setAlignment( int( QLabel::WordBreak | QLabel::AlignCenter ) );
    pTitleTL->setFrameShape( QLabel::Panel );
    pTitleTL->setFrameShadow( QLabel::Raised );
    QFont pTitleTL_font(  pTitleTL->font() );
    pTitleTL_font.setPointSize( 13 );
    pTitleTL_font.setBold( TRUE );
    pTitleTL_font.setItalic( TRUE );
    pTitleTL->setFont( pTitleTL_font ); 

    QWidget* privateLayoutWidget = new QWidget( this, "vbox1" );
    privateLayoutWidget->setGeometry( QRect( 10, 80, 410, 140 ) ); 
    vbox1 = new QVBoxLayout( privateLayoutWidget, 0, 0, "vbox1"); 

    pSysEqButton = new QPushButton( privateLayoutWidget, "pSysEqButton" );
    pSysEqButton->setText( trUtf8( "Single - &System Equilibria (SysEq)" ) );
    pSysEqButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, pSysEqButton->sizePolicy().hasHeightForWidth() ) );
    QFont pSysEqButton_font(  pSysEqButton->font() );
    pSysEqButton_font.setPointSize( 12 );
    pSysEqButton->setFont( pSysEqButton_font ); 
    QToolTip::add( pSysEqButton, trUtf8( "Calculation of equilibrium states in a single thermodynamic system configuration" ) );
    vbox1->addWidget( pSysEqButton );

    pProcessButton = new QPushButton( privateLayoutWidget, "pProcessButton" );
    pProcessButton->setText( trUtf8( "&Process Simulation (Process)" ) );
    pProcessButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, pProcessButton->sizePolicy().hasHeightForWidth() ) );
    QFont pProcessButton_font(  pProcessButton->font() );
    pProcessButton_font.setPointSize( 12 );
    pProcessButton->setFont( pProcessButton_font ); 
    QToolTip::add( pProcessButton, trUtf8( "Simulation of irreversible processes (batch GEM calculations)" ) );
    vbox1->addWidget( pProcessButton );

    pGtDemoButton = new QPushButton( privateLayoutWidget, "pGtDemoButton" );
    pGtDemoButton->setText( trUtf8( "&Data Sampling and Plotting (GtDemo) " ) );
    pGtDemoButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, pGtDemoButton->sizePolicy().hasHeightForWidth() ) );
    QFont pGtDemoButton_font(  pGtDemoButton->font() );
    pGtDemoButton_font.setPointSize( 12 );
    pGtDemoButton->setFont( pGtDemoButton_font ); 
    pGtDemoButton->setDefault( FALSE );
    QToolTip::add( pGtDemoButton, trUtf8( "Definition of data samplers and plots of the results" ) );
    vbox1->addWidget( pGtDemoButton );

    QWidget* privateLayoutWidget_2 = new QWidget( this, "hbox" );
    privateLayoutWidget_2->setGeometry( QRect( 10, 390, 410, 37 ) ); 
    hbox = new QHBoxLayout( privateLayoutWidget_2, 0, 6, "hbox"); 

    pBackButton = new QPushButton( privateLayoutWidget_2, "pBackButton" );
    pBackButton->setText( trUtf8( "&Back" ) );
    pBackButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, pBackButton->sizePolicy().hasHeightForWidth() ) );
    QFont pBackButton_font(  pBackButton->font() );
    pBackButton_font.setPointSize( 10 );
    pBackButton->setFont( pBackButton_font ); 
    pBackButton->setDefault( FALSE );
    QToolTip::add( pBackButton, trUtf8( "Back to 'Thermodynamic database management' dialog" ) );
    hbox->addWidget( pBackButton );

    pNumCtrlButton = new QPushButton( privateLayoutWidget_2, "pNumCtrlButton" );
    pNumCtrlButton->setText( trUtf8( "&Numerical Controls..." ) );
    pNumCtrlButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, pNumCtrlButton->sizePolicy().hasHeightForWidth() ) );
    QFont pNumCtrlButton_font(  pNumCtrlButton->font() );
    pNumCtrlButton_font.setPointSize( 10 );
    pNumCtrlButton->setFont( pNumCtrlButton_font ); 
    QToolTip::add( pNumCtrlButton, trUtf8( "Press to check/adjust controls of the numerical modules (saved in Profile record)" ) );
    hbox->addWidget( pNumCtrlButton );

    pHelpButton = new QPushButton( privateLayoutWidget_2, "pHelpButton" );
    pHelpButton->setText( trUtf8( "&Help" ) );
    pHelpButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, pHelpButton->sizePolicy().hasHeightForWidth() ) );
    QFont pHelpButton_font(  pHelpButton->font() );
    pHelpButton_font.setPointSize( 10 );
    pHelpButton->setFont( pHelpButton_font ); 
    pHelpButton->setDefault( TRUE );
    QToolTip::add( pHelpButton, trUtf8( "Learn about GEM chemical thermodynamic modelling technique" ) );
    hbox->addWidget( pHelpButton );

    QWidget* privateLayoutWidget_3 = new QWidget( this, "vbox2" );
    privateLayoutWidget_3->setGeometry( QRect( 40, 250, 380, 120 ) ); 
    vbox2 = new QVBoxLayout( privateLayoutWidget_3, 0, 0, "vbox2"); 

    pDutermButton = new QPushButton( privateLayoutWidget_3, "pDutermButton" );
    pDutermButton->setText( trUtf8( "Du&al Thermodynamics (DUtherm)" ) );
    pDutermButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, pDutermButton->sizePolicy().hasHeightForWidth() ) );
    QFont pDutermButton_font(  pDutermButton->font() );
    pDutermButton_font.setPointSize( 9 );
    pDutermButton->setFont( pDutermButton_font ); 
    QToolTip::add( pDutermButton, trUtf8( "" ) );
    vbox2->addWidget( pDutermButton );

    pProbeButton = new QPushButton( privateLayoutWidget_3, "pProbeButton" );
    pProbeButton->setText( trUtf8( "Adaptive Modelling in the &Uncertainty Space" ) );
    pProbeButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, pProbeButton->sizePolicy().hasHeightForWidth() ) );
    QFont pProbeButton_font(  pProbeButton->font() );
    pProbeButton_font.setPointSize( 9 );
    pProbeButton->setFont( pProbeButton_font ); 
    QToolTip::add( pProbeButton, trUtf8( "" ) );
    vbox2->addWidget( pProbeButton );

    pIntegButton = new QPushButton( privateLayoutWidget_3, "pIntegButton" );
    pIntegButton->setText( trUtf8( "Coupled &Mass Transport Modelling" ) );
    pIntegButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, pIntegButton->sizePolicy().hasHeightForWidth() ) );
    QFont pIntegButton_font(  pIntegButton->font() );
    pIntegButton_font.setPointSize( 9 );
    pIntegButton->setFont( pIntegButton_font ); 
    QToolTip::add( pIntegButton, trUtf8( "" ) );
    vbox2->addWidget( pIntegButton );

    Line1 = new QFrame( privateLayoutWidget_3, "Line1" );
    Line1->setFrameStyle( QFrame::HLine | QFrame::Sunken );
    vbox2->addWidget( Line1 );

    Line10 = new QFrame( this, "Line10" );
    Line10->setGeometry( QRect( 10, 370, 411, 20 ) ); 
    Line10->setFrameStyle( QFrame::HLine | QFrame::Sunken );

    // signals and slots connections
    connect( pBackButton, SIGNAL( clicked() ), this, SLOT( CmBack() ) );
    connect( pHelpButton, SIGNAL( clicked() ), this, SLOT( CmHelp() ) );
    connect( pSysEqButton, SIGNAL( clicked() ), this, SLOT( CmSystemDialog() ) );
    connect( pProcessButton, SIGNAL( clicked() ), this, SLOT( CmOpen_PROCESS() ) );
    connect( pGtDemoButton, SIGNAL( clicked() ), this, SLOT( CmOpen_GTDEMO() ) );
    connect( pDutermButton, SIGNAL( clicked() ), this, SLOT( CmOpen_DUTERM() ) );
    connect( pProbeButton, SIGNAL( clicked() ), this, SLOT( CmOpen_PROBE() ) );
    connect( pIntegButton, SIGNAL( clicked() ), this, SLOT( CmOpen_INTEG() ) );
    connect( pNumCtrlButton, SIGNAL( clicked() ), this, SLOT( CmAdvanced() ) );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
EQDialogData::~EQDialogData()
{
    // no need to delete child widgets, Qt does it all for us
}

void EQDialogData::CmBack()
{
    qWarning( "EQDialogData::CmBack(): Not implemented yet!" );
}

void EQDialogData::CmHelp()
{
    qWarning( "EQDialogData::CmHelp(): Not implemented yet!" );
}

void EQDialogData::CmOpen_DUTERM()
{
    qWarning( "EQDialogData::CmOpen_DUTERM(): Not implemented yet!" );
}

void EQDialogData::CmOpen_GTDEMO()
{
    qWarning( "EQDialogData::CmOpen_GTDEMO(): Not implemented yet!" );
}

void EQDialogData::CmOpen_INTEG()
{
    qWarning( "EQDialogData::CmOpen_INTEG(): Not implemented yet!" );
}

void EQDialogData::CmOpen_PROBE()
{
    qWarning( "EQDialogData::CmOpen_PROBE(): Not implemented yet!" );
}

void EQDialogData::CmOpen_PROCESS()
{
    qWarning( "EQDialogData::CmOpen_PROCESS(): Not implemented yet!" );
}

void EQDialogData::CmSystemDialog()
{
    qWarning( "EQDialogData::CmSystemDialog(): Not implemented yet!" );
}

void EQDialogData::CmAdvanced()
{
    qWarning( "EQDialogData::CmAdvanced(): Not implemented yet!" );
}

