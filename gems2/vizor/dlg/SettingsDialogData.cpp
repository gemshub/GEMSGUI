/****************************************************************************
** Form implementation generated from reading ui file 'SettingsDialog.ui'
**
** Created: Fr 1. Mrz 14:05:41 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "SettingsDialogData.h"

#include <qvariant.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qspinbox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a SettingsDialogData which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
SettingsDialogData::SettingsDialogData( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "SettingsDialogData" );
    resize( 580, 310 ); 
    setMinimumSize( QSize( 580, 310 ) );
    QFont f( font() );
    f.setFamily( "Lucida Sans Unicode" );
    f.setPointSize( 9 );
    setFont( f ); 
    setCaption( trUtf8( "Set GEM-Selektor preferences" ) );

    QWidget* privateLayoutWidget = new QWidget( this, "Layout28" );
    privateLayoutWidget->setGeometry( QRect( 10, 260, 560, 37 ) ); 
    Layout28 = new QHBoxLayout( privateLayoutWidget, 0, 6, "Layout28"); 

    pButtonOK = new QPushButton( privateLayoutWidget, "pButtonOK" );
    QFont pButtonOK_font(  pButtonOK->font() );
    pButtonOK->setFont( pButtonOK_font ); 
    pButtonOK->setText( trUtf8( "&Ok" ) );
    pButtonOK->setDefault( TRUE );
    Layout28->addWidget( pButtonOK );

    pButtonApply = new QPushButton( privateLayoutWidget, "pButtonApply" );
    QFont pButtonApply_font(  pButtonApply->font() );
    pButtonApply_font.setPointSize( 10 );
    pButtonApply->setFont( pButtonApply_font ); 
    pButtonApply->setText( trUtf8( "&Apply" ) );
    Layout28->addWidget( pButtonApply );

    pButtonHelp = new QPushButton( privateLayoutWidget, "pButtonHelp" );
    QFont pButtonHelp_font(  pButtonHelp->font() );
    pButtonHelp_font.setPointSize( 10 );
    pButtonHelp->setFont( pButtonHelp_font ); 
    pButtonHelp->setText( trUtf8( "&Help" ) );
    Layout28->addWidget( pButtonHelp );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout28->addItem( spacer );

    pButtonCancel = new QPushButton( privateLayoutWidget, "pButtonCancel" );
    QFont pButtonCancel_font(  pButtonCancel->font() );
    pButtonCancel_font.setPointSize( 10 );
    pButtonCancel->setFont( pButtonCancel_font ); 
    pButtonCancel->setText( trUtf8( "&Cancel" ) );
    Layout28->addWidget( pButtonCancel );

    QWidget* privateLayoutWidget_2 = new QWidget( this, "Layout5" );
    privateLayoutWidget_2->setGeometry( QRect( 10, 210, 560, 37 ) ); 
    Layout5 = new QHBoxLayout( privateLayoutWidget_2, 0, 6, "Layout5"); 

    TextLabel1_3 = new QLabel( privateLayoutWidget_2, "TextLabel1_3" );
    TextLabel1_3->setText( trUtf8( "Font" ) );
    Layout5->addWidget( TextLabel1_3 );

    pFontRawName = new QLineEdit( privateLayoutWidget_2, "pFontRawName" );
    QToolTip::add( pFontRawName, trUtf8( "Currently applied font for data screen forms" ) );
    Layout5->addWidget( pFontRawName );

    pButtonChFont = new QPushButton( privateLayoutWidget_2, "pButtonChFont" );
    pButtonChFont->setText( trUtf8( "C&hange font..." ) );
    QToolTip::add( pButtonChFont, trUtf8( "Here you can change font for the data screen forms" ) );
    Layout5->addWidget( pButtonChFont );

    ButtonGroup1 = new QButtonGroup( this, "ButtonGroup1" );
    ButtonGroup1->setGeometry( QRect( 10, 100, 310, 101 ) ); 
    ButtonGroup1->setTitle( trUtf8( "Creating/Upgrading System Profiles" ) );

    rbNewPrMode = new QRadioButton( ButtonGroup1, "rbNewPrMode" );
    rbNewPrMode->setGeometry( QRect( 10, 20, 290, 31 ) ); 
    rbNewPrMode->setText( trUtf8( "Make an independent profile   " ) );
    rbNewPrMode->setChecked( TRUE );
    QToolTip::add( rbNewPrMode, trUtf8( "Switch on to select elements and copy all related data from default database into new profile" ) );

    rbOldPrMode = new QRadioButton( ButtonGroup1, "rbOldPrMode" );
    rbOldPrMode->setGeometry( QRect( 10, 50, 290, 31 ) ); 
    rbOldPrMode->setText( trUtf8( "Link default database files to profile" ) );
    QToolTip::add( rbOldPrMode, trUtf8( "Switch on to select files from default database and link them to form a new profile" ) );

    GroupBox1 = new QGroupBox( this, "GroupBox1" );
    GroupBox1->setGeometry( QRect( 330, 100, 240, 101 ) ); 
    GroupBox1->setTitle( trUtf8( "Displaying Data" ) );

    TextLabel1_2_2 = new QLabel( GroupBox1, "TextLabel1_2_2" );
    TextLabel1_2_2->setGeometry( QRect( 131, 50, 100, 26 ) ); 
    TextLabel1_2_2->setText( trUtf8( "in 1/10 sec" ) );

    TextLabel1_2 = new QLabel( GroupBox1, "TextLabel1_2" );
    TextLabel1_2->setGeometry( QRect( 10, 50, 70, 26 ) ); 
    TextLabel1_2->setText( trUtf8( "Update" ) );

    TextLabel1 = new QLabel( GroupBox1, "TextLabel1" );
    TextLabel1->setGeometry( QRect( 11, 20, 50, 26 ) ); 
    TextLabel1->setText( trUtf8( "Show" ) );

    pUpdateInterval = new QSpinBox( GroupBox1, "pUpdateInterval" );
    pUpdateInterval->setGeometry( QRect( 71, 50, 50, 26 ) ); 
    pUpdateInterval->setMaxValue( 100 );
    pUpdateInterval->setMinValue( 0 );
    pUpdateInterval->setLineStep( 2 );
    pUpdateInterval->setValue( 10 );
    QToolTip::add( pUpdateInterval, trUtf8( "Interval (in centiseconds) for updating the dialogs and screen forms during GEM IPM calculations" ) );

    pNumDigits = new QSpinBox( GroupBox1, "pNumDigits" );
    pNumDigits->setGeometry( QRect( 71, 20, 50, 26 ) ); 
    pNumDigits->setMaxValue( 20 );
    pNumDigits->setValue( 7 );
    QToolTip::add( pNumDigits, trUtf8( "Number of decimal digits shown in screenform cells for the real floating-point numbers" ) );

    TextLabel1_4 = new QLabel( GroupBox1, "TextLabel1_4" );
    TextLabel1_4->setGeometry( QRect( 131, 20, 100, 26 ) ); 
    TextLabel1_4->setText( trUtf8( "digits in 'real'" ) );

    QWidget* privateLayoutWidget_3 = new QWidget( this, "Layout4" );
    privateLayoutWidget_3->setGeometry( QRect( 2, 9, 570, 77 ) ); 
    Layout4 = new QGridLayout( privateLayoutWidget_3, 1, 1, 0, 6, "Layout4"); 

    pConfigAutosave = new QCheckBox( privateLayoutWidget_3, "pConfigAutosave" );
    pConfigAutosave->setText( trUtf8( "Save configuration at exit" ) );
    pConfigAutosave->setChecked( TRUE );

    Layout4->addWidget( pConfigAutosave, 2, 0 );

    TextLabel2 = new QLabel( privateLayoutWidget_3, "TextLabel2" );
    TextLabel2->setText( trUtf8( " Core database directory" ) );

    Layout4->addWidget( TextLabel2, 0, 0 );

    pUserDBDir = new QLabel( privateLayoutWidget_3, "pUserDBDir" );
    pUserDBDir->setFrameShape( QLabel::Box );
    pUserDBDir->setFrameShadow( QLabel::Sunken );
    pUserDBDir->setText( trUtf8( "TextLabel1" ) );

    Layout4->addWidget( pUserDBDir, 1, 1 );

    pSysDBDir = new QLabel( privateLayoutWidget_3, "pSysDBDir" );
    pSysDBDir->setFrameShape( QLabel::Box );
    pSysDBDir->setFrameShadow( QLabel::Sunken );
    pSysDBDir->setText( trUtf8( "TextLabel1" ) );

    Layout4->addWidget( pSysDBDir, 0, 1 );

    TextLabel2_2 = new QLabel( privateLayoutWidget_3, "TextLabel2_2" );
    TextLabel2_2->setText( trUtf8( " User's profile directory" ) );

    Layout4->addWidget( TextLabel2_2, 1, 0 );

    // signals and slots connections
    connect( pButtonOK, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( pButtonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( pButtonApply, SIGNAL( clicked() ), this, SLOT( CmApply() ) );
    connect( pButtonHelp, SIGNAL( clicked() ), this, SLOT( CmHelp() ) );
    connect( pButtonChFont, SIGNAL( clicked() ), this, SLOT( CmChangeFont() ) );

    // tab order
    setTabOrder( pButtonApply, pButtonOK );
    setTabOrder( pButtonOK, pButtonCancel );
    setTabOrder( pButtonCancel, pFontRawName );
    setTabOrder( pFontRawName, pButtonChFont );
    setTabOrder( pButtonChFont, pUpdateInterval );
    setTabOrder( pUpdateInterval, pNumDigits );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
SettingsDialogData::~SettingsDialogData()
{
    // no need to delete child widgets, Qt does it all for us
}

void SettingsDialogData::CmChangeFont()
{
    qWarning( "SettingsDialogData::CmChangeFont(): Not implemented yet!" );
}

void SettingsDialogData::CmSysDBDirSelect()
{
    qWarning( "SettingsDialogData::CmSysDBDirSelect(): Not implemented yet!" );
}

void SettingsDialogData::CmUserDBDirSelect()
{
    qWarning( "SettingsDialogData::CmUserDBDirSelect(): Not implemented yet!" );
}

void SettingsDialogData::CmApply()
{
    qWarning( "SettingsDialogData::CmApply(): Not implemented yet!" );
}

void SettingsDialogData::CmHelp()
{
    qWarning( "SettingsDialogData::CmHelp(): Not implemented yet!" );
}

