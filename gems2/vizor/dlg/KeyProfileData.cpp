/****************************************************************************
** Form implementation generated from reading ui file 'KeyProfile.ui'
**
** Created: Втр Лют 5 12:33:43 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "KeyProfileData.h"

#include <qvariant.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a KeyProfileData which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
KeyProfileData::KeyProfileData( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "KeyProfileData" );
    resize( 450, 436 ); 
    setMinimumSize( QSize( 450, 400 ) );
    QFont f( font() );
    f.setFamily( "Lucida Sans Unicode" );
    setFont( f ); 
    setCaption( trUtf8( "Profile key list" ) );

    pLabel = new QLabel( this, "pLabel" );
    pLabel->setGeometry( QRect( 0, 10, 438, 21 ) ); 
    QFont pLabel_font(  pLabel->font() );
    pLabel_font.setPointSize( 11 );
    pLabel->setFont( pLabel_font ); 
    pLabel->setText( trUtf8( "Please, select one Modelling System Profile:" ) );
    pLabel->setAlignment( int( QLabel::AlignCenter ) );

    pList = new QListBox( this, "pList" );
    pList->setGeometry( QRect( 0, 30, 440, 290 ) ); 
    pList->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)7, 0, 0, pList->sizePolicy().hasHeightForWidth() ) );
    QFont pList_font(  pList->font() );
    pList_font.setFamily( "Courier" );
    pList_font.setPointSize( 9 );
    pList->setFont( pList_font ); 
    QToolTip::add( pList, trUtf8( "" ) );

    QWidget* privateLayoutWidget = new QWidget( this, "Layout3" );
    privateLayoutWidget->setGeometry( QRect( 0, 380, 430, 37 ) ); 
    Layout3 = new QHBoxLayout( privateLayoutWidget, 0, 6, "Layout3"); 

    pGO_OKButton = new QPushButton( privateLayoutWidget, "pGO_OKButton" );
    pGO_OKButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, pGO_OKButton->sizePolicy().hasHeightForWidth() ) );
    QFont pGO_OKButton_font(  pGO_OKButton->font() );
    pGO_OKButton_font.setPointSize( 10 );
    pGO_OKButton->setFont( pGO_OKButton_font ); 
    pGO_OKButton->setText( trUtf8( "&Go!" ) );
    pGO_OKButton->setDefault( TRUE );
    QToolTip::add( pGO_OKButton, trUtf8( "This will open and load a selected System Profile" ) );
    Layout3->addWidget( pGO_OKButton );

    pNewProfBtn = new QPushButton( privateLayoutWidget, "pNewProfBtn" );
    pNewProfBtn->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, pNewProfBtn->sizePolicy().hasHeightForWidth() ) );
    QFont pNewProfBtn_font(  pNewProfBtn->font() );
    pNewProfBtn_font.setPointSize( 10 );
    pNewProfBtn->setFont( pNewProfBtn_font ); 
    pNewProfBtn->setText( trUtf8( "New &Profile..." ) );
    QToolTip::add( pNewProfBtn, trUtf8( "This will start the wizard for setting up a new System Profile" ) );
    Layout3->addWidget( pNewProfBtn );

    pHelpButton = new QPushButton( privateLayoutWidget, "pHelpButton" );
    pHelpButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, pHelpButton->sizePolicy().hasHeightForWidth() ) );
    QFont pHelpButton_font(  pHelpButton->font() );
    pHelpButton_font.setPointSize( 10 );
    pHelpButton->setFont( pHelpButton_font ); 
    pHelpButton->setText( trUtf8( "&Help" ) );
    QToolTip::add( pHelpButton, trUtf8( "Learn about System Profile concept and profile configuration" ) );
    Layout3->addWidget( pHelpButton );

    pCancelButton = new QPushButton( privateLayoutWidget, "pCancelButton" );
    pCancelButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, pCancelButton->sizePolicy().hasHeightForWidth() ) );
    QFont pCancelButton_font(  pCancelButton->font() );
    pCancelButton_font.setPointSize( 10 );
    pCancelButton->setFont( pCancelButton_font ); 
    pCancelButton->setText( trUtf8( "&Cancel" ) );
    Layout3->addWidget( pCancelButton );

    pAqGas = new QCheckBox( this, "pAqGas" );
    pAqGas->setGeometry( QRect( 10, 320, 430, 22 ) ); 
    QFont pAqGas_font(  pAqGas->font() );
    pAqGas_font.setPointSize( 10 );
    pAqGas->setFont( pAqGas_font ); 
    pAqGas->setText( trUtf8( "&Use old settings for aqueous/gas phases" ) );
    pAqGas->setChecked( TRUE );
    QToolTip::add( pAqGas, trUtf8( "Uncheck to use different models of aqueous and/or gas phases" ) );

    pFiles = new QCheckBox( this, "pFiles" );
    pFiles->setEnabled( TRUE );
    pFiles->setGeometry( QRect( 10, 340, 430, 22 ) ); 
    QFont pFiles_font(  pFiles->font() );
    pFiles_font.setPointSize( 10 );
    pFiles->setFont( pFiles_font ); 
    pFiles->setAcceptDrops( FALSE );
    pFiles->setText( trUtf8( "&Add new files to Profile" ) );
    pFiles->setChecked( FALSE );
    QToolTip::add( pFiles, trUtf8( "Uncheck to use different models of aqueous and/or gas phases" ) );

    // signals and slots connections
    connect( pNewProfBtn, SIGNAL( clicked() ), this, SLOT( CmNew() ) );
    connect( pGO_OKButton, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( pCancelButton, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( pList, SIGNAL( doubleClicked(QListBoxItem*) ), this, SLOT( accept() ) );
    connect( pHelpButton, SIGNAL( clicked() ), this, SLOT( CmHelp() ) );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
KeyProfileData::~KeyProfileData()
{
    // no need to delete child widgets, Qt does it all for us
}

void KeyProfileData::CmNew()
{
    qWarning( "KeyProfileData::CmNew(): Not implemented yet!" );
}

void KeyProfileData::CmHelp()
{
    qWarning( "KeyProfileData::CmHelp(): Not implemented yet!" );
}

