/****************************************************************************
** Form implementation generated from reading ui file 'SettingsDialog.ui'
**
** Created: Sun May 6 19:57:27 2001
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "SettingsDialogData.h"

#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
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
    resize( 590, 281 ); 
    setMinimumSize( QSize( 590, 280 ) );
    QFont f( font() );
    f.setFamily( "Lucida Sans Unicode" );
    f.setPointSize( 9 );
    setFont( f ); 
    setCaption( tr( "Set GEM-Selektor preferences" ) );

    QWidget* privateLayoutWidget = new QWidget( this, "Layout5" );
    privateLayoutWidget->setGeometry( QRect( 10, 10, 570, 37 ) ); 
    Layout5 = new QHBoxLayout( privateLayoutWidget ); 
    Layout5->setSpacing( 6 );
    Layout5->setMargin( 0 );

    TextLabel1_3 = new QLabel( privateLayoutWidget, "TextLabel1_3" );
    TextLabel1_3->setText( tr( "Font" ) );
    Layout5->addWidget( TextLabel1_3 );

    pFontRawName = new QLineEdit( privateLayoutWidget, "pFontRawName" );
    QToolTip::add(  pFontRawName, tr( "Currently applied font for data screen forms" ) );
    Layout5->addWidget( pFontRawName );

    pButtonChFont = new QPushButton( privateLayoutWidget, "pButtonChFont" );
    QFont pButtonChFont_font(  pButtonChFont->font() );
    pButtonChFont_font.setPointSize( 10 );
    pButtonChFont->setFont( pButtonChFont_font ); 
    pButtonChFont->setText( tr( "C&hange font..." ) );
    QToolTip::add(  pButtonChFont, tr( "Here you can change font for the data screen forms" ) );
    Layout5->addWidget( pButtonChFont );

    QWidget* privateLayoutWidget_2 = new QWidget( this, "Layout28" );
    privateLayoutWidget_2->setGeometry( QRect( 10, 230, 560, 37 ) ); 
    Layout28 = new QHBoxLayout( privateLayoutWidget_2 ); 
    Layout28->setSpacing( 6 );
    Layout28->setMargin( 0 );

    pButtonOK = new QPushButton( privateLayoutWidget_2, "pButtonOK" );
    QFont pButtonOK_font(  pButtonOK->font() );
    pButtonOK->setFont( pButtonOK_font ); 
    pButtonOK->setText( tr( "&Ok" ) );
    pButtonOK->setDefault( TRUE );
    Layout28->addWidget( pButtonOK );

    pButtonApply = new QPushButton( privateLayoutWidget_2, "pButtonApply" );
    QFont pButtonApply_font(  pButtonApply->font() );
    pButtonApply_font.setPointSize( 10 );
    pButtonApply->setFont( pButtonApply_font ); 
    pButtonApply->setText( tr( "&Apply" ) );
    Layout28->addWidget( pButtonApply );

    pButtonHelp = new QPushButton( privateLayoutWidget_2, "pButtonHelp" );
    QFont pButtonHelp_font(  pButtonHelp->font() );
    pButtonHelp_font.setPointSize( 10 );
    pButtonHelp->setFont( pButtonHelp_font ); 
    pButtonHelp->setText( tr( "&Help" ) );
    Layout28->addWidget( pButtonHelp );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout28->addItem( spacer );

    pButtonCancel = new QPushButton( privateLayoutWidget_2, "pButtonCancel" );
    QFont pButtonCancel_font(  pButtonCancel->font() );
    pButtonCancel_font.setPointSize( 10 );
    pButtonCancel->setFont( pButtonCancel_font ); 
    pButtonCancel->setText( tr( "&Cancel" ) );
    Layout28->addWidget( pButtonCancel );

    QWidget* privateLayoutWidget_3 = new QWidget( this, "vbox1" );
    privateLayoutWidget_3->setGeometry( QRect( 10, 60, 568, 80 ) ); 
    vbox1 = new QGridLayout( privateLayoutWidget_3 ); 
    vbox1->setSpacing( 6 );
    vbox1->setMargin( 0 );

    pButtonBrwDB = new QPushButton( privateLayoutWidget_3, "pButtonBrwDB" );
    QFont pButtonBrwDB_font(  pButtonBrwDB->font() );
    pButtonBrwDB_font.setPointSize( 10 );
    pButtonBrwDB->setFont( pButtonBrwDB_font ); 
    pButtonBrwDB->setText( tr( "Browse ..." ) );
    pButtonBrwDB->setDefault( FALSE );

    vbox1->addWidget( pButtonBrwDB, 0, 2 );

    pUserDBDir = new QLineEdit( privateLayoutWidget_3, "pUserDBDir" );
    pUserDBDir->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, pUserDBDir->sizePolicy().hasHeightForWidth() ) );
    pUserDBDir->setMinimumSize( QSize( 250, 0 ) );
    QToolTip::add(  pUserDBDir, tr( "Path to the directory in which the user keeps the model application profiles" ) );

    vbox1->addWidget( pUserDBDir, 1, 1 );

    TextLabel2 = new QLabel( privateLayoutWidget_3, "TextLabel2" );
    TextLabel2->setText( tr( " Core database directory" ) );

    vbox1->addWidget( TextLabel2, 0, 0 );

    TextLabel2_2 = new QLabel( privateLayoutWidget_3, "TextLabel2_2" );
    TextLabel2_2->setText( tr( " User's profile directory" ) );

    vbox1->addWidget( TextLabel2_2, 1, 0 );

    pButtonBrwProf = new QPushButton( privateLayoutWidget_3, "pButtonBrwProf" );
    QFont pButtonBrwProf_font(  pButtonBrwProf->font() );
    pButtonBrwProf_font.setPointSize( 10 );
    pButtonBrwProf->setFont( pButtonBrwProf_font ); 
    pButtonBrwProf->setText( tr( "Browse ..." ) );

    vbox1->addWidget( pButtonBrwProf, 1, 2 );

    pDBDir = new QLineEdit( privateLayoutWidget_3, "pDBDir" );
    pDBDir->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, pDBDir->sizePolicy().hasHeightForWidth() ) );
    pDBDir->setMinimumSize( QSize( 250, 0 ) );
    QToolTip::add(  pDBDir, tr( "Path to the directory with configuration and core thermodynamic database files" ) );

    vbox1->addWidget( pDBDir, 0, 1 );

    QWidget* privateLayoutWidget_4 = new QWidget( this, "Layout5" );
    privateLayoutWidget_4->setGeometry( QRect( 11, 157, 219, 60 ) ); 
    Layout5_2 = new QGridLayout( privateLayoutWidget_4 ); 
    Layout5_2->setSpacing( 6 );
    Layout5_2->setMargin( 0 );

    TextLabel1_4 = new QLabel( privateLayoutWidget_4, "TextLabel1_4" );
    TextLabel1_4->setText( tr( "digits" ) );

    Layout5_2->addWidget( TextLabel1_4, 0, 2 );

    pUpdateInterval = new QSpinBox( privateLayoutWidget_4, "pUpdateInterval" );
    pUpdateInterval->setMaxValue( 100 );
    pUpdateInterval->setMinValue( 0 );
    pUpdateInterval->setLineStep( 2 );
    pUpdateInterval->setValue( 10 );
    QToolTip::add(  pUpdateInterval, tr( "Interval (in centiseconds) for updating the dialogs and screen forms during GEM IPM calculations" ) );

    Layout5_2->addWidget( pUpdateInterval, 1, 1 );

    TextLabel1 = new QLabel( privateLayoutWidget_4, "TextLabel1" );
    TextLabel1->setText( tr( "Precision of 'real'" ) );

    Layout5_2->addWidget( TextLabel1, 0, 0 );

    TextLabel1_2 = new QLabel( privateLayoutWidget_4, "TextLabel1_2" );
    TextLabel1_2->setText( tr( "Run update interval" ) );

    Layout5_2->addWidget( TextLabel1_2, 1, 0 );

    pNumDigits = new QSpinBox( privateLayoutWidget_4, "pNumDigits" );
    pNumDigits->setMaxValue( 20 );
    pNumDigits->setValue( 7 );
    QToolTip::add(  pNumDigits, tr( "Number of decimal digits shown in screenform cells for the real floating-point numbers" ) );

    Layout5_2->addWidget( pNumDigits, 0, 1 );

    TextLabel1_2_2 = new QLabel( privateLayoutWidget_4, "TextLabel1_2_2" );
    TextLabel1_2_2->setText( tr( "1/10 seconds" ) );

    Layout5_2->addWidget( TextLabel1_2_2, 1, 2 );

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
	QFont pButtonChFont_font(  pButtonChFont->font() );
	pButtonChFont_font.setPointSize( 10 );
	pButtonChFont->setFont( pButtonChFont_font ); 
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
	QFont pButtonBrwDB_font(  pButtonBrwDB->font() );
	pButtonBrwDB_font.setPointSize( 10 );
	pButtonBrwDB->setFont( pButtonBrwDB_font ); 
	QFont pButtonBrwProf_font(  pButtonBrwProf->font() );
	pButtonBrwProf_font.setPointSize( 10 );
	pButtonBrwProf->setFont( pButtonBrwProf_font ); 
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

