/****************************************************************************
** Form implementation generated from reading ui file 'SettingsDialog.ui'
**
** Created: Sun Dec 9 17:41:31 2001
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "SettingsDialogData.h"

#include <qbuttongroup.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qspinbox.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a SettingsDialogData which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
SettingsDialogData::SettingsDialogData( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "SettingsDialogData" );
    resize( 580, 311 ); 
    setMinimumSize( QSize( 580, 310 ) );
    QFont f( font() );
    f.setFamily( "Lucida Sans Unicode" );
    f.setPointSize( 9 );
    setFont( f ); 
    setCaption( tr( "Set GEM-Selektor preferences" ) );

    QWidget* privateLayoutWidget = new QWidget( this, "Layout28" );
    privateLayoutWidget->setGeometry( QRect( 10, 260, 560, 37 ) ); 
    Layout28 = new QHBoxLayout( privateLayoutWidget ); 
    Layout28->setSpacing( 6 );
    Layout28->setMargin( 0 );

    pButtonOK = new QPushButton( privateLayoutWidget, "pButtonOK" );
    QFont pButtonOK_font(  pButtonOK->font() );
    pButtonOK->setFont( pButtonOK_font ); 
    pButtonOK->setText( tr( "&Ok" ) );
    pButtonOK->setDefault( TRUE );
    Layout28->addWidget( pButtonOK );

    pButtonApply = new QPushButton( privateLayoutWidget, "pButtonApply" );
    QFont pButtonApply_font(  pButtonApply->font() );
    pButtonApply_font.setPointSize( 10 );
    pButtonApply->setFont( pButtonApply_font ); 
    pButtonApply->setText( tr( "&Apply" ) );
    Layout28->addWidget( pButtonApply );

    pButtonHelp = new QPushButton( privateLayoutWidget, "pButtonHelp" );
    QFont pButtonHelp_font(  pButtonHelp->font() );
    pButtonHelp_font.setPointSize( 10 );
    pButtonHelp->setFont( pButtonHelp_font ); 
    pButtonHelp->setText( tr( "&Help" ) );
    Layout28->addWidget( pButtonHelp );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout28->addItem( spacer );

    pButtonCancel = new QPushButton( privateLayoutWidget, "pButtonCancel" );
    QFont pButtonCancel_font(  pButtonCancel->font() );
    pButtonCancel_font.setPointSize( 10 );
    pButtonCancel->setFont( pButtonCancel_font ); 
    pButtonCancel->setText( tr( "&Cancel" ) );
    Layout28->addWidget( pButtonCancel );

    QWidget* privateLayoutWidget_2 = new QWidget( this, "vbox1" );
    privateLayoutWidget_2->setGeometry( QRect( 10, 10, 559, 80 ) ); 
    vbox1 = new QGridLayout( privateLayoutWidget_2 ); 
    vbox1->setSpacing( 6 );
    vbox1->setMargin( 0 );

    pButtonBrwDB = new QPushButton( privateLayoutWidget_2, "pButtonBrwDB" );
    pButtonBrwDB->setText( tr( "Browse ..." ) );
    pButtonBrwDB->setDefault( FALSE );

    vbox1->addWidget( pButtonBrwDB, 0, 2 );

    pUserDBDir = new QLineEdit( privateLayoutWidget_2, "pUserDBDir" );
    pUserDBDir->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, pUserDBDir->sizePolicy().hasHeightForWidth() ) );
    pUserDBDir->setMinimumSize( QSize( 250, 0 ) );
    QToolTip::add(  pUserDBDir, tr( "Path to the directory in which the user keeps the model application profiles" ) );

    vbox1->addWidget( pUserDBDir, 1, 1 );

    TextLabel2 = new QLabel( privateLayoutWidget_2, "TextLabel2" );
    TextLabel2->setText( tr( " Core database directory" ) );

    vbox1->addWidget( TextLabel2, 0, 0 );

    TextLabel2_2 = new QLabel( privateLayoutWidget_2, "TextLabel2_2" );
    TextLabel2_2->setText( tr( " User's profile directory" ) );

    vbox1->addWidget( TextLabel2_2, 1, 0 );

    pButtonBrwProf = new QPushButton( privateLayoutWidget_2, "pButtonBrwProf" );
    pButtonBrwProf->setText( tr( "Browse ..." ) );

    vbox1->addWidget( pButtonBrwProf, 1, 2 );

    pDBDir = new QLineEdit( privateLayoutWidget_2, "pDBDir" );
    pDBDir->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, pDBDir->sizePolicy().hasHeightForWidth() ) );
    pDBDir->setMinimumSize( QSize( 250, 0 ) );
    QToolTip::add(  pDBDir, tr( "Path to the directory with configuration and core thermodynamic database files" ) );

    vbox1->addWidget( pDBDir, 0, 1 );

    QWidget* privateLayoutWidget_3 = new QWidget( this, "Layout5" );
    privateLayoutWidget_3->setGeometry( QRect( 10, 210, 560, 37 ) ); 
    Layout5 = new QHBoxLayout( privateLayoutWidget_3 ); 
    Layout5->setSpacing( 6 );
    Layout5->setMargin( 0 );

    TextLabel1_3 = new QLabel( privateLayoutWidget_3, "TextLabel1_3" );
    TextLabel1_3->setText( tr( "Font" ) );
    Layout5->addWidget( TextLabel1_3 );

    pFontRawName = new QLineEdit( privateLayoutWidget_3, "pFontRawName" );
    QToolTip::add(  pFontRawName, tr( "Currently applied font for data screen forms" ) );
    Layout5->addWidget( pFontRawName );

    pButtonChFont = new QPushButton( privateLayoutWidget_3, "pButtonChFont" );
    pButtonChFont->setText( tr( "C&hange font..." ) );
    QToolTip::add(  pButtonChFont, tr( "Here you can change font for the data screen forms" ) );
    Layout5->addWidget( pButtonChFont );

    ButtonGroup1 = new QButtonGroup( this, "ButtonGroup1" );
    ButtonGroup1->setGeometry( QRect( 10, 100, 310, 101 ) ); 
    ButtonGroup1->setTitle( tr( "Creating/Upgrading System Profiles" ) );

    rbNewPrMode = new QRadioButton( ButtonGroup1, "rbNewPrMode" );
    rbNewPrMode->setGeometry( QRect( 10, 30, 290, 31 ) ); 
    rbNewPrMode->setText( tr( "Make an independent profile   " ) );
    rbNewPrMode->setChecked( TRUE );
    QToolTip::add(  rbNewPrMode, tr( "Switch on to select elements and copy all related data from default database into new profile" ) );

    rbOldPrMode = new QRadioButton( ButtonGroup1, "rbOldPrMode" );
    rbOldPrMode->setGeometry( QRect( 10, 60, 290, 31 ) ); 
    rbOldPrMode->setText( tr( "Link default database files to profile" ) );
    QToolTip::add(  rbOldPrMode, tr( "Switch on to select files from default database and link them to form a new profile" ) );

    GroupBox1 = new QGroupBox( this, "GroupBox1" );
    GroupBox1->setGeometry( QRect( 330, 100, 240, 101 ) ); 
    GroupBox1->setTitle( tr( "Displaying Data" ) );

    TextLabel1_4 = new QLabel( GroupBox1, "TextLabel1_4" );
    TextLabel1_4->setGeometry( QRect( 130, 30, 100, 26 ) ); 
    TextLabel1_4->setText( tr( "digits in 'real'" ) );

    TextLabel1_2 = new QLabel( GroupBox1, "TextLabel1_2" );
    TextLabel1_2->setGeometry( QRect( 9, 60, 70, 26 ) ); 
    TextLabel1_2->setText( tr( "Update" ) );

    TextLabel1_2_2 = new QLabel( GroupBox1, "TextLabel1_2_2" );
    TextLabel1_2_2->setGeometry( QRect( 130, 60, 100, 26 ) ); 
    TextLabel1_2_2->setText( tr( "in 1/10 sec" ) );

    TextLabel1 = new QLabel( GroupBox1, "TextLabel1" );
    TextLabel1->setGeometry( QRect( 10, 30, 50, 26 ) ); 
    TextLabel1->setText( tr( "Show" ) );

    pNumDigits = new QSpinBox( GroupBox1, "pNumDigits" );
    pNumDigits->setGeometry( QRect( 70, 30, 50, 26 ) ); 
    pNumDigits->setMaxValue( 20 );
    pNumDigits->setValue( 7 );
    QToolTip::add(  pNumDigits, tr( "Number of decimal digits shown in screenform cells for the real floating-point numbers" ) );

    pUpdateInterval = new QSpinBox( GroupBox1, "pUpdateInterval" );
    pUpdateInterval->setGeometry( QRect( 70, 60, 50, 26 ) ); 
    pUpdateInterval->setMaxValue( 100 );
    pUpdateInterval->setMinValue( 0 );
    pUpdateInterval->setLineStep( 2 );
    pUpdateInterval->setValue( 10 );
    QToolTip::add(  pUpdateInterval, tr( "Interval (in centiseconds) for updating the dialogs and screen forms during GEM IPM calculations" ) );

    // signals and slots connections
    connect( pButtonBrwDB, SIGNAL( clicked() ), this, SLOT( CmSysDBDirSelect() ) );
    connect( pButtonBrwProf, SIGNAL( clicked() ), this, SLOT( CmUserDBDirSelect() ) );
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
    setTabOrder( pButtonChFont, pButtonBrwDB );
    setTabOrder( pButtonBrwDB, pUserDBDir );
    setTabOrder( pUserDBDir, pButtonBrwProf );
    setTabOrder( pButtonBrwProf, pDBDir );
    setTabOrder( pDBDir, pUpdateInterval );
    setTabOrder( pUpdateInterval, pNumDigits );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
SettingsDialogData::~SettingsDialogData()
{
    // no need to delete child widgets, Qt does it all for us
}

/*  
 *  Main event handler. Reimplemented to handle application
 *  font changes
 */
bool SettingsDialogData::event( QEvent* ev )
{
    bool ret = QDialog::event( ev ); 
    if ( ev->type() == QEvent::ApplicationFontChange ) {
	QFont pButtonOK_font(  pButtonOK->font() );
	pButtonOK->setFont( pButtonOK_font ); 
	QFont pButtonApply_font(  pButtonApply->font() );
	pButtonApply_font.setPointSize( 10 );
	pButtonApply->setFont( pButtonApply_font ); 
	QFont pButtonHelp_font(  pButtonHelp->font() );
	pButtonHelp_font.setPointSize( 10 );
	pButtonHelp->setFont( pButtonHelp_font ); 
	QFont pButtonCancel_font(  pButtonCancel->font() );
	pButtonCancel_font.setPointSize( 10 );
	pButtonCancel->setFont( pButtonCancel_font ); 
    }
    return ret;
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

