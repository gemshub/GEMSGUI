/****************************************************************************
** Form implementation generated from reading ui file 'MainDialog.ui'
**
** Created: Mo 4. Mrz 11:35:27 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "MainDialogData.h"

#include <qvariant.h>
#include <qframe.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a MainDialogData which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 */
MainDialogData::MainDialogData( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "MainDialogData" );
    resize( 500, 321 ); 
    setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)3, 0, 0, sizePolicy().hasHeightForWidth() ) );
    setMinimumSize( QSize( 500, 320 ) );
    setCaption( trUtf8( "GEM-Selektor" ) );
    QFont f( font() );
    f.setFamily( "Lucida Sans Unicode" );
    f.setPointSize( 9 );
    setFont( f ); 

    QWidget* privateLayoutWidget = new QWidget( this, "Layout9" );
    privateLayoutWidget->setGeometry( QRect( 10, 270, 480, 37 ) ); 
    Layout9 = new QHBoxLayout( privateLayoutWidget, 0, 6, "Layout9"); 

    pButtonExit = new QPushButton( privateLayoutWidget, "pButtonExit" );
    pButtonExit->setText( trUtf8( "&Exit" ) );
    pButtonExit->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, pButtonExit->sizePolicy().hasHeightForWidth() ) );
    QFont pButtonExit_font(  pButtonExit->font() );
    pButtonExit_font.setPointSize( 10 );
    pButtonExit->setFont( pButtonExit_font ); 
    pButtonExit->setDefault( FALSE );
    Layout9->addWidget( pButtonExit );

    pButtonSettings = new QPushButton( privateLayoutWidget, "pButtonSettings" );
    pButtonSettings->setText( trUtf8( "&Preferences..." ) );
    pButtonSettings->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, pButtonSettings->sizePolicy().hasHeightForWidth() ) );
    QFont pButtonSettings_font(  pButtonSettings->font() );
    pButtonSettings_font.setPointSize( 10 );
    pButtonSettings->setFont( pButtonSettings_font ); 
    QToolTip::add( pButtonSettings, trUtf8( "Press to adjust fonts on screen forms and set work directories" ) );
    Layout9->addWidget( pButtonSettings );

    pButtonHelp = new QPushButton( privateLayoutWidget, "pButtonHelp" );
    pButtonHelp->setText( trUtf8( "Get &Started" ) );
    QFont pButtonHelp_font(  pButtonHelp->font() );
    pButtonHelp_font.setPointSize( 10 );
    pButtonHelp->setFont( pButtonHelp_font ); 
    pButtonHelp->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, pButtonHelp->sizePolicy().hasHeightForWidth() ) );
    QToolTip::add( pButtonHelp, trUtf8( "Press, if you are new to GEM-Selektor " ) );
    Layout9->addWidget( pButtonHelp );

    pButtonAbout = new QPushButton( privateLayoutWidget, "pButtonAbout" );
    pButtonAbout->setText( trUtf8( "&About" ) );
    pButtonAbout->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, pButtonAbout->sizePolicy().hasHeightForWidth() ) );
    QFont pButtonAbout_font(  pButtonAbout->font() );
    pButtonAbout_font.setPointSize( 10 );
    pButtonAbout->setFont( pButtonAbout_font ); 
    pButtonAbout->setDefault( FALSE );
    QToolTip::add( pButtonAbout, trUtf8( "About GEM-Selektor and its development" ) );
    Layout9->addWidget( pButtonAbout );

    Line1 = new QFrame( this, "Line1" );
    Line1->setGeometry( QRect( 10, 240, 480, 20 ) ); 
    Line1->setFrameStyle( QFrame::HLine | QFrame::Sunken );
    Line1->setFrameShadow( QFrame::Sunken );
    Line1->setProperty( "frameShape", (int)QFrame::HLine );

    pButtonDB = new QPushButton( this, "pButtonDB" );
    pButtonDB->setGeometry( QRect( 30, 170, 428, 66 ) ); 
    pButtonDB->setText( trUtf8( "&Thermodynamic Database" ) );
    pButtonDB->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, pButtonDB->sizePolicy().hasHeightForWidth() ) );
    QFont pButtonDB_font(  pButtonDB->font() );
    pButtonDB_font.setPointSize( 16 );
    pButtonDB->setFont( pButtonDB_font ); 
    QToolTip::add( pButtonDB, trUtf8( "Press this button to manage database records" ) );

    pButtonEQ = new QPushButton( this, "pButtonEQ" );
    pButtonEQ->setGeometry( QRect( 30, 100, 428, 66 ) ); 
    pButtonEQ->setText( trUtf8( "&Computation of Equilibria" ) );
    pButtonEQ->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, pButtonEQ->sizePolicy().hasHeightForWidth() ) );
    QFont pButtonEQ_font(  pButtonEQ->font() );
    pButtonEQ_font.setPointSize( 16 );
    pButtonEQ->setFont( pButtonEQ_font ); 
    QToolTip::add( pButtonEQ, trUtf8( "Press this button to calculate thermodynamic models" ) );

    pLogoImg = new QLabel( this, "pLogoImg" );
    pLogoImg->setGeometry( QRect( 10, 20, 60, 60 ) ); 
    pLogoImg->setScaledContents( TRUE );
    pLogoImg->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, pLogoImg->sizePolicy().hasHeightForWidth() ) );
    pLogoImg->setFrameShadow( QLabel::Raised );
    pLogoImg->setFrameShape( QLabel::Panel );
    pLogoImg->setAlignment( int( QLabel::AlignCenter ) );

    TextLabel1 = new QLabel( this, "TextLabel1" );
    TextLabel1->setGeometry( QRect( 70, 20, 420, 60 ) ); 
    TextLabel1->setText( trUtf8( "Geochemical Equilibrium Modelling by Gibbs Energy Minimization" ) );
    TextLabel1->setAlignment( int( QLabel::WordBreak | QLabel::AlignCenter ) );
    TextLabel1->setFrameShape( QLabel::Panel );
    TextLabel1->setFrameShadow( QLabel::Raised );
    QFont TextLabel1_font(  TextLabel1->font() );
    TextLabel1_font.setPointSize( 13 );
    TextLabel1_font.setBold( TRUE );
    TextLabel1_font.setItalic( TRUE );
    TextLabel1->setFont( TextLabel1_font ); 
    TextLabel1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)1, 0, 0, TextLabel1->sizePolicy().hasHeightForWidth() ) );

    // signals and slots connections
    connect( pButtonEQ, SIGNAL( clicked() ), this, SLOT( CmEQDialog() ) );
    connect( pButtonExit, SIGNAL( clicked() ), this, SLOT( CmExit() ) );
    connect( pButtonAbout, SIGNAL( clicked() ), this, SLOT( CmAbout() ) );
    connect( pButtonHelp, SIGNAL( clicked() ), this, SLOT( CmHelp() ) );
    connect( pButtonSettings, SIGNAL( clicked() ), this, SLOT( CmSettings() ) );
    connect( pButtonDB, SIGNAL( clicked() ), this, SLOT( CmDBDialog() ) );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
MainDialogData::~MainDialogData()
{
    // no need to delete child widgets, Qt does it all for us
}

void MainDialogData::CmDBDialog()
{
    qWarning( "MainDialogData::CmDBDialog(): Not implemented yet!" );
}

void MainDialogData::CmEQDialog()
{
    qWarning( "MainDialogData::CmEQDialog(): Not implemented yet!" );
}

void MainDialogData::CmExit()
{
    qWarning( "MainDialogData::CmExit(): Not implemented yet!" );
}

void MainDialogData::CmHelp()
{
    qWarning( "MainDialogData::CmHelp(): Not implemented yet!" );
}

void MainDialogData::CmAbout()
{
    qWarning( "MainDialogData::CmAbout(): Not implemented yet!" );
}

void MainDialogData::CmSettings()
{
    qWarning( "MainDialogData::CmSettings(): Not implemented yet!" );
}

