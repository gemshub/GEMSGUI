/****************************************************************************
** Form implementation generated from reading ui file 'SystemDialog.ui'
**
** Created: Fri Nov 16 14:53:51 2001
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "SystemDialogData.h"

#include <qvariant.h>   // first for gcc 2.7.2
#include <qcheckbox.h>
#include <qframe.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlcdnumber.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a SystemDialogData which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 */
SystemDialogData::SystemDialogData( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "SystemDialogData" );
    resize( 640, 481 ); 
    setMinimumSize( QSize( 640, 480 ) );
    QFont f( font() );
    f.setFamily( "Lucida Sans Unicode" );
    f.setPointSize( 8 );
    setFont( f ); 
    setCaption( trUtf8( "SystemDialogData" ) );

    QWidget* privateLayoutWidget = new QWidget( this, "Layout5" );
    privateLayoutWidget->setGeometry( QRect( 10, 10, 550, 52 ) ); 
    Layout5 = new QHBoxLayout( privateLayoutWidget, 0, 3, "Layout5"); 

    pImg1 = new QLabel( privateLayoutWidget, "pImg1" );
    pImg1->setMinimumSize( QSize( 50, 50 ) );
    pImg1->setMaximumSize( QSize( 50, 50 ) );
    pImg1->setFrameShape( QLabel::Panel );
    pImg1->setFrameShadow( QLabel::Raised );
    Layout5->addWidget( pImg1 );

    TextLabelTit = new QLabel( privateLayoutWidget, "TextLabelTit" );
    QFont TextLabelTit_font(  TextLabelTit->font() );
    TextLabelTit_font.setPointSize( 15 );
    TextLabelTit_font.setBold( TRUE );
    TextLabelTit_font.setItalic( TRUE );
    TextLabelTit->setFont( TextLabelTit_font ); 
    TextLabelTit->setFrameShape( QLabel::Box );
    TextLabelTit->setFrameShadow( QLabel::Raised );
    TextLabelTit->setText( trUtf8( "Single Thermodynamic System " ) );
    TextLabelTit->setAlignment( int( QLabel::WordBreak | QLabel::AlignCenter ) );
    Layout5->addWidget( TextLabelTit );

    btnClose = new QPushButton( this, "btnClose" );
    btnClose->setGeometry( QRect( 570, 10, 60, 30 ) ); 
    QFont btnClose_font(  btnClose->font() );
    btnClose_font.setPointSize( 10 );
    btnClose->setFont( btnClose_font ); 
    btnClose->setText( trUtf8( "Cl&ose" ) );

    GroupBoxPrint = new QGroupBox( this, "GroupBoxPrint" );
    GroupBoxPrint->setGeometry( QRect( 10, 270, 120, 150 ) ); 
    QFont GroupBoxPrint_font(  GroupBoxPrint->font() );
    GroupBoxPrint_font.setPointSize( 9 );
    GroupBoxPrint->setFont( GroupBoxPrint_font ); 
    GroupBoxPrint->setFrameShape( QGroupBox::WinPanel );
    GroupBoxPrint->setTitle( trUtf8( "Print/Export" ) );

    btnPrintTD = new QPushButton( GroupBoxPrint, "btnPrintTD" );
    btnPrintTD->setGeometry( QRect( 10, 30, 100, 30 ) ); 
    btnPrintTD->setText( trUtf8( "&Therm.Data" ) );
    btnPrintTD->setDefault( FALSE );
    QToolTip::add( btnPrintTD, trUtf8( "Print thermodynamic data calculated at actual T and P" ) );

    btnPrintSys = new QPushButton( GroupBoxPrint, "btnPrintSys" );
    btnPrintSys->setGeometry( QRect( 10, 70, 100, 30 ) ); 
    btnPrintSys->setText( trUtf8( "S&ystem" ) );
    QToolTip::add( btnPrintSys, trUtf8( "Print bulk composition with selection of phases and species with additional constraints set on them" ) );

    btnPrintEqSt = new QPushButton( GroupBoxPrint, "btnPrintEqSt" );
    btnPrintEqSt->setGeometry( QRect( 10, 110, 100, 30 ) ); 
    btnPrintEqSt->setText( trUtf8( "&Eq. State" ) );
    QToolTip::add( btnPrintEqSt, trUtf8( "Print full results of GEM IPM calculation of equilibrium state " ) );

    FrameTPV = new QFrame( this, "FrameTPV" );
    FrameTPV->setGeometry( QRect( 500, 110, 130, 110 ) ); 
    FrameTPV->setFrameShape( QFrame::WinPanel );
    FrameTPV->setFrameShadow( QFrame::Sunken );

    TextLabelT = new QLabel( FrameTPV, "TextLabelT" );
    TextLabelT->setGeometry( QRect( 10, 10, 20, 31 ) ); 
    QFont TextLabelT_font(  TextLabelT->font() );
    TextLabelT_font.setPointSize( 12 );
    TextLabelT_font.setBold( TRUE );
    TextLabelT_font.setItalic( TRUE );
    TextLabelT->setFont( TextLabelT_font ); 
    TextLabelT->setText( trUtf8( "T" ) );
    TextLabelT->setAlignment( int( QLabel::AlignCenter ) );

    TextLabelP = new QLabel( FrameTPV, "TextLabelP" );
    TextLabelP->setGeometry( QRect( 10, 40, 20, 31 ) ); 
    QFont TextLabelP_font(  TextLabelP->font() );
    TextLabelP_font.setPointSize( 12 );
    TextLabelP_font.setBold( TRUE );
    TextLabelP_font.setItalic( TRUE );
    TextLabelP->setFont( TextLabelP_font ); 
    TextLabelP->setText( trUtf8( "P" ) );
    TextLabelP->setAlignment( int( QLabel::AlignCenter ) );

    TextLabelV = new QLabel( FrameTPV, "TextLabelV" );
    TextLabelV->setGeometry( QRect( 10, 70, 20, 31 ) ); 
    QFont TextLabelV_font(  TextLabelV->font() );
    TextLabelV_font.setPointSize( 12 );
    TextLabelV_font.setBold( TRUE );
    TextLabelV_font.setItalic( TRUE );
    TextLabelV->setFont( TextLabelV_font ); 
    TextLabelV->setText( trUtf8( "V" ) );
    TextLabelV->setAlignment( int( QLabel::AlignCenter ) );

    LCDNumberV = new QLCDNumber( FrameTPV, "LCDNumberV" );
    LCDNumberV->setGeometry( QRect( 40, 70, 80, 30 ) ); 
    LCDNumberV->setFrameShape( QLCDNumber::WinPanel );
    LCDNumberV->setSmallDecimalPoint( TRUE );
    LCDNumberV->setNumDigits( 7 );
    LCDNumberV->setSegmentStyle( QLCDNumber::Flat );
    QToolTip::add( LCDNumberV, trUtf8( "Input volume constraint for the system, cm3, or 0 (if unconstrained) " ) );

    LCDNumberP = new QLCDNumber( FrameTPV, "LCDNumberP" );
    LCDNumberP->setGeometry( QRect( 40, 40, 80, 30 ) ); 
    LCDNumberP->setFrameShape( QLCDNumber::WinPanel );
    LCDNumberP->setSmallDecimalPoint( TRUE );
    LCDNumberP->setNumDigits( 7 );
    LCDNumberP->setSegmentStyle( QLCDNumber::Flat );
    LCDNumberP->setProperty( "intValue", 1 );
    QToolTip::add( LCDNumberP, trUtf8( "Input pressure, bar or 0 for P_sat" ) );

    LCDNumberT = new QLCDNumber( FrameTPV, "LCDNumberT" );
    LCDNumberT->setGeometry( QRect( 40, 10, 80, 30 ) ); 
    LCDNumberT->setFrameShape( QLCDNumber::WinPanel );
    LCDNumberT->setSmallDecimalPoint( TRUE );
    LCDNumberT->setNumDigits( 7 );
    LCDNumberT->setSegmentStyle( QLCDNumber::Flat );
    LCDNumberT->setProperty( "intValue", 298 );
    QToolTip::add( LCDNumberT, trUtf8( "Input temperature, K" ) );

    TextLabelRkey = new QLabel( this, "TextLabelRkey" );
    TextLabelRkey->setGeometry( QRect( 140, 70, 90, 31 ) ); 
    QFont TextLabelRkey_font(  TextLabelRkey->font() );
    TextLabelRkey_font.setPointSize( 9 );
    TextLabelRkey->setFont( TextLabelRkey_font ); 
    TextLabelRkey->setText( trUtf8( "SysEq Rkey" ) );
    TextLabelRkey->setAlignment( int( QLabel::AlignCenter ) );

    pRKey = new QLabel( this, "pRKey" );
    pRKey->setGeometry( QRect( 230, 70, 400, 30 ) ); 
    QPalette pal;
    QColorGroup cg;
    cg.setColor( QColorGroup::Foreground, black );
    cg.setColor( QColorGroup::Button, QColor( 192, 192, 192) );
    cg.setColor( QColorGroup::Light, white );
    cg.setColor( QColorGroup::Midlight, QColor( 223, 223, 223) );
    cg.setColor( QColorGroup::Dark, QColor( 96, 96, 96) );
    cg.setColor( QColorGroup::Mid, QColor( 128, 128, 128) );
    cg.setColor( QColorGroup::Text, black );
    cg.setColor( QColorGroup::BrightText, white );
    cg.setColor( QColorGroup::ButtonText, black );
    cg.setColor( QColorGroup::Base, white );
    cg.setColor( QColorGroup::Background, QColor( 208, 181, 189) );
    cg.setColor( QColorGroup::Shadow, black );
    cg.setColor( QColorGroup::Highlight, QColor( 0, 0, 128) );
    cg.setColor( QColorGroup::HighlightedText, white );
    pal.setActive( cg );
    cg.setColor( QColorGroup::Foreground, black );
    cg.setColor( QColorGroup::Button, QColor( 192, 192, 192) );
    cg.setColor( QColorGroup::Light, white );
    cg.setColor( QColorGroup::Midlight, QColor( 220, 220, 220) );
    cg.setColor( QColorGroup::Dark, QColor( 96, 96, 96) );
    cg.setColor( QColorGroup::Mid, QColor( 128, 128, 128) );
    cg.setColor( QColorGroup::Text, black );
    cg.setColor( QColorGroup::BrightText, white );
    cg.setColor( QColorGroup::ButtonText, black );
    cg.setColor( QColorGroup::Base, white );
    cg.setColor( QColorGroup::Background, QColor( 208, 181, 189) );
    cg.setColor( QColorGroup::Shadow, black );
    cg.setColor( QColorGroup::Highlight, QColor( 0, 0, 128) );
    cg.setColor( QColorGroup::HighlightedText, white );
    pal.setInactive( cg );
    cg.setColor( QColorGroup::Foreground, QColor( 128, 128, 128) );
    cg.setColor( QColorGroup::Button, QColor( 192, 192, 192) );
    cg.setColor( QColorGroup::Light, white );
    cg.setColor( QColorGroup::Midlight, QColor( 220, 220, 220) );
    cg.setColor( QColorGroup::Dark, QColor( 96, 96, 96) );
    cg.setColor( QColorGroup::Mid, QColor( 128, 128, 128) );
    cg.setColor( QColorGroup::Text, black );
    cg.setColor( QColorGroup::BrightText, white );
    cg.setColor( QColorGroup::ButtonText, QColor( 128, 128, 128) );
    cg.setColor( QColorGroup::Base, white );
    cg.setColor( QColorGroup::Background, QColor( 208, 181, 189) );
    cg.setColor( QColorGroup::Shadow, black );
    cg.setColor( QColorGroup::Highlight, QColor( 0, 0, 128) );
    cg.setColor( QColorGroup::HighlightedText, white );
    pal.setDisabled( cg );
    pRKey->setPalette( pal );
    QFont pRKey_font(  pRKey->font() );
    pRKey_font.setPointSize( 12 );
    pRKey->setFont( pRKey_font ); 
    pRKey->setFrameShape( QLabel::Box );
    pRKey->setFrameShadow( QLabel::Sunken );
    pRKey->setText( trUtf8( ":" ) );
    QToolTip::add( pRKey, trUtf8( "Key of SysEq record, currently loaded or to be inserted into database" ) );

    GroupBoxRec = new QGroupBox( this, "GroupBoxRec" );
    GroupBoxRec->setGeometry( QRect( 10, 70, 120, 190 ) ); 
    QFont GroupBoxRec_font(  GroupBoxRec->font() );
    GroupBoxRec_font.setPointSize( 9 );
    GroupBoxRec->setFont( GroupBoxRec_font ); 
    GroupBoxRec->setFrameShape( QGroupBox::WinPanel );
    GroupBoxRec->setTitle( trUtf8( "Records" ) );

    btnSysEqNew = new QPushButton( GroupBoxRec, "btnSysEqNew" );
    btnSysEqNew->setGeometry( QRect( 10, 70, 100, 30 ) ); 
    QFont btnSysEqNew_font(  btnSysEqNew->font() );
    btnSysEqNew_font.setPointSize( 10 );
    btnSysEqNew->setFont( btnSysEqNew_font ); 
    btnSysEqNew->setText( trUtf8( "&Create..." ) );
    QToolTip::add( btnSysEqNew, trUtf8( "Press to create a new SysEq record to be inserted into the database" ) );

    btnSysEqRem = new QPushButton( GroupBoxRec, "btnSysEqRem" );
    btnSysEqRem->setGeometry( QRect( 10, 110, 100, 30 ) ); 
    QFont btnSysEqRem_font(  btnSysEqRem->font() );
    btnSysEqRem_font.setPointSize( 10 );
    btnSysEqRem->setFont( btnSysEqRem_font ); 
    btnSysEqRem->setText( trUtf8( "&Remake..." ) );
    QToolTip::add( btnSysEqRem, trUtf8( "Press to re-configure currently loaded SysEq record" ) );

    btnSysEqSave = new QPushButton( GroupBoxRec, "btnSysEqSave" );
    btnSysEqSave->setGeometry( QRect( 10, 150, 100, 30 ) ); 
    QFont btnSysEqSave_font(  btnSysEqSave->font() );
    btnSysEqSave_font.setPointSize( 10 );
    btnSysEqSave->setFont( btnSysEqSave_font ); 
    btnSysEqSave->setText( trUtf8( "&Save" ) );
    btnSysEqSave->setDefault( TRUE );
    QToolTip::add( btnSysEqSave, trUtf8( "Press to save contents of currently loaded record to database file" ) );

    btnSysEqLoad = new QPushButton( GroupBoxRec, "btnSysEqLoad" );
    btnSysEqLoad->setGeometry( QRect( 10, 30, 100, 30 ) ); 
    QFont btnSysEqLoad_font(  btnSysEqLoad->font() );
    btnSysEqLoad_font.setPointSize( 10 );
    btnSysEqLoad->setFont( btnSysEqLoad_font ); 
    btnSysEqLoad->setText( trUtf8( "&Load..." ) );
    QToolTip::add( btnSysEqLoad, trUtf8( "Press to select and load another SysEq record from the database" ) );

    QWidget* privateLayoutWidget_2 = new QWidget( this, "Layout5" );
    privateLayoutWidget_2->setGeometry( QRect( 140, 120, 345, 340 ) ); 
    Layout5_2 = new QVBoxLayout( privateLayoutWidget_2, 0, 6, "Layout5_2"); 

    GroupBoxInp = new QGroupBox( privateLayoutWidget_2, "GroupBoxInp" );
    QFont GroupBoxInp_font(  GroupBoxInp->font() );
    GroupBoxInp_font.setPointSize( 10 );
    GroupBoxInp_font.setItalic( TRUE );
    GroupBoxInp->setFont( GroupBoxInp_font ); 
    GroupBoxInp->setFrameShape( QGroupBox::WinPanel );
    GroupBoxInp->setFrameShadow( QGroupBox::Raised );
    GroupBoxInp->setTitle( trUtf8( "Input: System Definition" ) );
    GroupBoxInp->setColumnLayout(0, Qt::Vertical );
    GroupBoxInp->layout()->setSpacing( 6 );
    GroupBoxInp->layout()->setMargin( 11 );
    GroupBoxInpLayout = new QVBoxLayout( GroupBoxInp->layout() );
    GroupBoxInpLayout->setAlignment( Qt::AlignTop );

    btnInpTDview = new QPushButton( GroupBoxInp, "btnInpTDview" );
    btnInpTDview->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, btnInpTDview->sizePolicy().hasHeightForWidth() ) );
    QFont btnInpTDview_font(  btnInpTDview->font() );
    btnInpTDview_font.setPointSize( 12 );
    btnInpTDview_font.setItalic( FALSE );
    btnInpTDview->setFont( btnInpTDview_font ); 
    btnInpTDview->setText( trUtf8( "Thermodynamic Data at T, P " ) );
    QToolTip::add( btnInpTDview, trUtf8( "Press to browse the list of all species in the profile with values of g0, v0 ... at actual T,P" ) );
    GroupBoxInpLayout->addWidget( btnInpTDview );

    btnInpBCview = new QPushButton( GroupBoxInp, "btnInpBCview" );
    btnInpBCview->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, btnInpBCview->sizePolicy().hasHeightForWidth() ) );
    QFont btnInpBCview_font(  btnInpBCview->font() );
    btnInpBCview_font.setPointSize( 12 );
    btnInpBCview_font.setItalic( FALSE );
    btnInpBCview->setFont( btnInpBCview_font ); 
    btnInpBCview->setText( trUtf8( "Bulk Composition (b) Setup" ) );
    QToolTip::add( btnInpBCview, trUtf8( "Press to specify bulk composition of the system and additional constraints" ) );
    GroupBoxInpLayout->addWidget( btnInpBCview );

    btnInpPhDCview = new QPushButton( GroupBoxInp, "btnInpPhDCview" );
    btnInpPhDCview->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, btnInpPhDCview->sizePolicy().hasHeightForWidth() ) );
    QFont btnInpPhDCview_font(  btnInpPhDCview->font() );
    btnInpPhDCview_font.setPointSize( 12 );
    btnInpPhDCview_font.setItalic( FALSE );
    btnInpPhDCview->setFont( btnInpPhDCview_font ); 
    btnInpPhDCview->setText( trUtf8( "Phases and Species Setup" ) );
    QToolTip::add( btnInpPhDCview, trUtf8( "Under construction - please, use the above button for now..." ) );
    GroupBoxInpLayout->addWidget( btnInpPhDCview );
    Layout5_2->addWidget( GroupBoxInp );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    Layout5_2->addItem( spacer );

    GroupBoxRes = new QGroupBox( privateLayoutWidget_2, "GroupBoxRes" );
    QFont GroupBoxRes_font(  GroupBoxRes->font() );
    GroupBoxRes_font.setPointSize( 10 );
    GroupBoxRes_font.setItalic( TRUE );
    GroupBoxRes->setFont( GroupBoxRes_font ); 
    GroupBoxRes->setFrameShape( QGroupBox::WinPanel );
    GroupBoxRes->setFrameShadow( QGroupBox::Raised );
    GroupBoxRes->setTitle( trUtf8( "Results: Equilibrium State" ) );
    GroupBoxRes->setColumnLayout(0, Qt::Vertical );
    GroupBoxRes->layout()->setSpacing( 6 );
    GroupBoxRes->layout()->setMargin( 11 );
    GroupBoxResLayout = new QVBoxLayout( GroupBoxRes->layout() );
    GroupBoxResLayout->setAlignment( Qt::AlignTop );

    btnResICview = new QPushButton( GroupBoxRes, "btnResICview" );
    btnResICview->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, btnResICview->sizePolicy().hasHeightForWidth() ) );
    QFont btnResICview_font(  btnResICview->font() );
    btnResICview_font.setPointSize( 12 );
    btnResICview_font.setItalic( FALSE );
    btnResICview->setFont( btnResICview_font ); 
    btnResICview->setText( trUtf8( "Dual Solution u; pH, pe, IS" ) );
    QToolTip::add( btnResICview, trUtf8( "Press to view general parameters and values related to independent components" ) );
    GroupBoxResLayout->addWidget( btnResICview );

    btnResDCview = new QPushButton( GroupBoxRes, "btnResDCview" );
    btnResDCview->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, btnResDCview->sizePolicy().hasHeightForWidth() ) );
    QFont btnResDCview_font(  btnResDCview->font() );
    btnResDCview_font.setPointSize( 12 );
    btnResDCview_font.setItalic( FALSE );
    btnResDCview->setFont( btnResDCview_font ); 
    btnResDCview->setText( trUtf8( " Prime Solution (speciation) " ) );
    QToolTip::add( btnResDCview, trUtf8( "Under construction - please, use the above button for now..." ) );
    GroupBoxResLayout->addWidget( btnResDCview );

    btnDebugView = new QPushButton( GroupBoxRes, "btnDebugView" );
    btnDebugView->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, btnDebugView->sizePolicy().hasHeightForWidth() ) );
    QFont btnDebugView_font(  btnDebugView->font() );
    btnDebugView_font.setPointSize( 12 );
    btnDebugView_font.setItalic( FALSE );
    btnDebugView->setFont( btnDebugView_font ); 
    btnDebugView->setText( trUtf8( "Details for Experts" ) );
    QToolTip::add( btnDebugView, trUtf8( "Press to get access to debugging screen forms (A matrix etc.)" ) );
    GroupBoxResLayout->addWidget( btnDebugView );
    Layout5_2->addWidget( GroupBoxRes );

    GroupBoxIPM = new QGroupBox( this, "GroupBoxIPM" );
    GroupBoxIPM->setGeometry( QRect( 500, 230, 130, 240 ) ); 
    QFont GroupBoxIPM_font(  GroupBoxIPM->font() );
    GroupBoxIPM_font.setPointSize( 9 );
    GroupBoxIPM->setFont( GroupBoxIPM_font ); 
    GroupBoxIPM->setFrameShape( QGroupBox::WinPanel );
    GroupBoxIPM->setTitle( trUtf8( "Calculations:" ) );

    btnRunIPM = new QPushButton( GroupBoxIPM, "btnRunIPM" );
    btnRunIPM->setGeometry( QRect( 10, 100, 80, 70 ) ); 
    cg.setColor( QColorGroup::Foreground, black );
    cg.setColor( QColorGroup::Button, QColor( 161, 207, 162) );
    cg.setColor( QColorGroup::Light, QColor( 253, 255, 253) );
    cg.setColor( QColorGroup::Midlight, QColor( 207, 231, 207) );
    cg.setColor( QColorGroup::Dark, QColor( 80, 103, 80) );
    cg.setColor( QColorGroup::Mid, QColor( 107, 138, 108) );
    cg.setColor( QColorGroup::Text, black );
    cg.setColor( QColorGroup::BrightText, white );
    cg.setColor( QColorGroup::ButtonText, black );
    cg.setColor( QColorGroup::Base, white );
    cg.setColor( QColorGroup::Background, QColor( 192, 192, 192) );
    cg.setColor( QColorGroup::Shadow, black );
    cg.setColor( QColorGroup::Highlight, QColor( 0, 0, 128) );
    cg.setColor( QColorGroup::HighlightedText, white );
    pal.setActive( cg );
    cg.setColor( QColorGroup::Foreground, black );
    cg.setColor( QColorGroup::Button, QColor( 161, 207, 162) );
    cg.setColor( QColorGroup::Light, QColor( 253, 255, 253) );
    cg.setColor( QColorGroup::Midlight, QColor( 185, 238, 186) );
    cg.setColor( QColorGroup::Dark, QColor( 80, 103, 80) );
    cg.setColor( QColorGroup::Mid, QColor( 107, 138, 108) );
    cg.setColor( QColorGroup::Text, black );
    cg.setColor( QColorGroup::BrightText, white );
    cg.setColor( QColorGroup::ButtonText, black );
    cg.setColor( QColorGroup::Base, white );
    cg.setColor( QColorGroup::Background, QColor( 192, 192, 192) );
    cg.setColor( QColorGroup::Shadow, black );
    cg.setColor( QColorGroup::Highlight, QColor( 0, 0, 128) );
    cg.setColor( QColorGroup::HighlightedText, white );
    pal.setInactive( cg );
    cg.setColor( QColorGroup::Foreground, QColor( 128, 128, 128) );
    cg.setColor( QColorGroup::Button, QColor( 161, 207, 162) );
    cg.setColor( QColorGroup::Light, QColor( 253, 255, 253) );
    cg.setColor( QColorGroup::Midlight, QColor( 185, 238, 186) );
    cg.setColor( QColorGroup::Dark, QColor( 80, 103, 80) );
    cg.setColor( QColorGroup::Mid, QColor( 107, 138, 108) );
    cg.setColor( QColorGroup::Text, black );
    cg.setColor( QColorGroup::BrightText, white );
    cg.setColor( QColorGroup::ButtonText, QColor( 128, 128, 128) );
    cg.setColor( QColorGroup::Base, white );
    cg.setColor( QColorGroup::Background, QColor( 192, 192, 192) );
    cg.setColor( QColorGroup::Shadow, black );
    cg.setColor( QColorGroup::Highlight, QColor( 0, 0, 128) );
    cg.setColor( QColorGroup::HighlightedText, white );
    pal.setDisabled( cg );
    btnRunIPM->setPalette( pal );
    QFont btnRunIPM_font(  btnRunIPM->font() );
    btnRunIPM_font.setPointSize( 18 );
    btnRunIPM_font.setBold( TRUE );
    btnRunIPM->setFont( btnRunIPM_font ); 
    btnRunIPM->setText( trUtf8( "&IPM" ) );
    QToolTip::add( btnRunIPM, trUtf8( "Press to perform calculation of equilibrium state using GEM IPM algorithm" ) );

    btnMoreIPM = new QPushButton( GroupBoxIPM, "btnMoreIPM" );
    btnMoreIPM->setGeometry( QRect( 90, 110, 30, 50 ) ); 
    QFont btnMoreIPM_font(  btnMoreIPM->font() );
    btnMoreIPM_font.setPointSize( 22 );
    btnMoreIPM->setFont( btnMoreIPM_font ); 
    btnMoreIPM->setText( trUtf8( "?" ) );
    QToolTip::add( btnMoreIPM, trUtf8( "More about Convex Programming GEM and the IPM algorithms" ) );

    btnCalcBCC = new QPushButton( GroupBoxIPM, "btnCalcBCC" );
    btnCalcBCC->setGeometry( QRect( 10, 30, 60, 60 ) ); 
    cg.setColor( QColorGroup::Foreground, black );
    cg.setColor( QColorGroup::Button, QColor( 146, 191, 192) );
    cg.setColor( QColorGroup::Light, QColor( 227, 255, 255) );
    cg.setColor( QColorGroup::Midlight, QColor( 186, 223, 223) );
    cg.setColor( QColorGroup::Dark, QColor( 73, 96, 96) );
    cg.setColor( QColorGroup::Mid, QColor( 97, 127, 128) );
    cg.setColor( QColorGroup::Text, black );
    cg.setColor( QColorGroup::BrightText, white );
    cg.setColor( QColorGroup::ButtonText, black );
    cg.setColor( QColorGroup::Base, white );
    cg.setColor( QColorGroup::Background, QColor( 192, 192, 192) );
    cg.setColor( QColorGroup::Shadow, black );
    cg.setColor( QColorGroup::Highlight, QColor( 0, 0, 128) );
    cg.setColor( QColorGroup::HighlightedText, white );
    pal.setActive( cg );
    cg.setColor( QColorGroup::Foreground, black );
    cg.setColor( QColorGroup::Button, QColor( 146, 191, 192) );
    cg.setColor( QColorGroup::Light, QColor( 227, 255, 255) );
    cg.setColor( QColorGroup::Midlight, QColor( 167, 219, 220) );
    cg.setColor( QColorGroup::Dark, QColor( 73, 96, 96) );
    cg.setColor( QColorGroup::Mid, QColor( 97, 127, 128) );
    cg.setColor( QColorGroup::Text, black );
    cg.setColor( QColorGroup::BrightText, white );
    cg.setColor( QColorGroup::ButtonText, black );
    cg.setColor( QColorGroup::Base, white );
    cg.setColor( QColorGroup::Background, QColor( 192, 192, 192) );
    cg.setColor( QColorGroup::Shadow, black );
    cg.setColor( QColorGroup::Highlight, QColor( 0, 0, 128) );
    cg.setColor( QColorGroup::HighlightedText, white );
    pal.setInactive( cg );
    cg.setColor( QColorGroup::Foreground, QColor( 128, 128, 128) );
    cg.setColor( QColorGroup::Button, QColor( 146, 191, 192) );
    cg.setColor( QColorGroup::Light, QColor( 227, 255, 255) );
    cg.setColor( QColorGroup::Midlight, QColor( 167, 219, 220) );
    cg.setColor( QColorGroup::Dark, QColor( 73, 96, 96) );
    cg.setColor( QColorGroup::Mid, QColor( 97, 127, 128) );
    cg.setColor( QColorGroup::Text, black );
    cg.setColor( QColorGroup::BrightText, white );
    cg.setColor( QColorGroup::ButtonText, QColor( 128, 128, 128) );
    cg.setColor( QColorGroup::Base, white );
    cg.setColor( QColorGroup::Background, QColor( 192, 192, 192) );
    cg.setColor( QColorGroup::Shadow, black );
    cg.setColor( QColorGroup::Highlight, QColor( 0, 0, 128) );
    cg.setColor( QColorGroup::HighlightedText, white );
    pal.setDisabled( cg );
    btnCalcBCC->setPalette( pal );
    QFont btnCalcBCC_font(  btnCalcBCC->font() );
    btnCalcBCC_font.setPointSize( 15 );
    btnCalcBCC_font.setBold( TRUE );
    btnCalcBCC->setFont( btnCalcBCC_font ); 
    btnCalcBCC->setText( trUtf8( "&BCC" ) );
    QToolTip::add( btnCalcBCC, trUtf8( "Bulk Composition Calculation " ) );
    QWhatsThis::add( btnCalcBCC, trUtf8( "This button activates calculation of the bulk elemental molar composition (vector b) of the system, followed by de-selection of all species whose stoichiometry (chemical formula) is not consistent with the stoichiometry basis for the whole system. \n"
"Accordingly, this operation changes dimensionalities of lists of independent components, dependent components, and phases to be taken into calculation of equilibrium state. In such a case, it is safer to press the BCC button first, then check whether the new selection of species and phases is OK (modify if necessary and press BCC again), and finally press the IPM button to calculate the equilibrium state." ) );

    btnMoreBCC = new QPushButton( GroupBoxIPM, "btnMoreBCC" );
    btnMoreBCC->setGeometry( QRect( 70, 40, 30, 40 ) ); 
    QFont btnMoreBCC_font(  btnMoreBCC->font() );
    btnMoreBCC_font.setPointSize( 18 );
    btnMoreBCC->setFont( btnMoreBCC_font ); 
    btnMoreBCC->setText( trUtf8( "?" ) );
    QToolTip::add( btnMoreBCC, trUtf8( "More about composition of thermodynamic systems" ) );

    pSimplexCheck = new QCheckBox( GroupBoxIPM, "pSimplexCheck" );
    pSimplexCheck->setGeometry( QRect( 10, 170, 110, 20 ) ); 
    pSimplexCheck->setText( trUtf8( "Simplex " ) );
    pSimplexCheck->setChecked( TRUE );
    QToolTip::add( pSimplexCheck, trUtf8( "Check to use automatic initial approximation by simplex method; Uncheck to use previous results as IPM initial approximation " ) );

    pStepwiseCheck = new QCheckBox( GroupBoxIPM, "pStepwiseCheck" );
    pStepwiseCheck->setGeometry( QRect( 10, 210, 110, 20 ) ); 
    pStepwiseCheck->setText( trUtf8( "Stepwise " ) );
    pStepwiseCheck->setTristate( FALSE );
    QToolTip::add( pStepwiseCheck, trUtf8( "Check to run GEM IPM module in a stepwise (data debugging) mode" ) );

    pRaisedCheck = new QCheckBox( GroupBoxIPM, "pRaisedCheck" );
    pRaisedCheck->setGeometry( QRect( 10, 190, 110, 21 ) ); 
    pRaisedCheck->setText( trUtf8( "Precise" ) );
    pRaisedCheck->setChecked( TRUE );
    QToolTip::add( pRaisedCheck, trUtf8( "Check to raise all species into FIA Domain; Uncheck - use only species with x_j > 0" ) );

    btnHelp = new QPushButton( this, "btnHelp" );
    btnHelp->setGeometry( QRect( 10, 430, 120, 40 ) ); 
    QFont btnHelp_font(  btnHelp->font() );
    btnHelp_font.setPointSize( 11 );
    btnHelp->setFont( btnHelp_font ); 
    btnHelp->setText( trUtf8( "&Help" ) );
    QToolTip::add( btnHelp, trUtf8( "More about definition of thermodynamic systems" ) );

    // signals and slots connections
    connect( btnInpBCview, SIGNAL( clicked() ), this, SLOT( CmOpen_SYSTEM_ICOMP() ) );
    connect( btnInpPhDCview, SIGNAL( clicked() ), this, SLOT( CmOpen_inp_PHASE_HL() ) );
    connect( btnInpTDview, SIGNAL( clicked() ), this, SLOT( CmOpen_MTPARAM() ) );
    connect( btnCalcBCC, SIGNAL( clicked() ), this, SLOT( CmRunBCC() ) );
    connect( btnMoreBCC, SIGNAL( clicked() ), this, SLOT( CmMoreBCC() ) );
    connect( btnResDCview, SIGNAL( clicked() ), this, SLOT( CmOpen_res_PHASE_HL() ) );
    connect( btnDebugView, SIGNAL( clicked() ), this, SLOT( CmOpen_MULTI() ) );
    connect( btnResICview, SIGNAL( clicked() ), this, SLOT( CmOpen_EQSTAT_ICOMP() ) );
    connect( btnSysEqLoad, SIGNAL( clicked() ), this, SLOT( CmSelect() ) );
    connect( btnSysEqNew, SIGNAL( clicked() ), this, SLOT( CmNew() ) );
    connect( btnSysEqRem, SIGNAL( clicked() ), this, SLOT( CmRemake() ) );
    connect( btnSysEqSave, SIGNAL( clicked() ), this, SLOT( CmSave() ) );
    connect( btnPrintTD, SIGNAL( clicked() ), this, SLOT( CmPrintMtparam() ) );
    connect( btnPrintSys, SIGNAL( clicked() ), this, SLOT( CmPrintSystem() ) );
    connect( btnPrintEqSt, SIGNAL( clicked() ), this, SLOT( CmPrintEqstat() ) );
    connect( btnHelp, SIGNAL( clicked() ), this, SLOT( CmHelp() ) );
    connect( btnClose, SIGNAL( clicked() ), this, SLOT( CmBack() ) );
    connect( btnRunIPM, SIGNAL( clicked() ), this, SLOT( CmRunIPM() ) );
    connect( btnMoreIPM, SIGNAL( clicked() ), this, SLOT( CmMoreIPM() ) );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
SystemDialogData::~SystemDialogData()
{
    // no need to delete child widgets, Qt does it all for us
}

void SystemDialogData::CmBack()
{
    qWarning( "SystemDialogData::CmBack(): Not implemented yet!" );
}

void SystemDialogData::CmSelect()
{
    qWarning( "SystemDialogData::CmSelect(): Not implemented yet!" );
}

void SystemDialogData::CmNew()
{
    qWarning( "SystemDialogData::CmNew(): Not implemented yet!" );
}

void SystemDialogData::CmRemake()
{
    qWarning( "SystemDialogData::CmRemake(): Not implemented yet!" );
}

void SystemDialogData::CmSave()
{
    qWarning( "SystemDialogData::CmSave(): Not implemented yet!" );
}

void SystemDialogData::CmRunIPM()
{
    qWarning( "SystemDialogData::CmRunIPM(): Not implemented yet!" );
}

void SystemDialogData::CmMoreIPM()
{
    qWarning( "SystemDialogData::CmMoreIPM(): Not implemented yet!" );
}

void SystemDialogData::CmMoreBCC()
{
    qWarning( "SystemDialogData::CmMoreBCC(): Not implemented yet!" );
}

void SystemDialogData::CmRunBCC()
{
    qWarning( "SystemDialogData::CmRunBCC(): Not implemented yet!" );
}

void SystemDialogData::CmPrintMtparam()
{
    qWarning( "SystemDialogData::CmPrintMtparam(): Not implemented yet!" );
}

void SystemDialogData::CmPrintSystem()
{
    qWarning( "SystemDialogData::CmPrintSystem(): Not implemented yet!" );
}

void SystemDialogData::CmPrintEqstat()
{
    qWarning( "SystemDialogData::CmPrintEqstat(): Not implemented yet!" );
}

void SystemDialogData::CmHelp()
{
    qWarning( "SystemDialogData::CmHelp(): Not implemented yet!" );
}

void SystemDialogData::CmOpen_MTPARAM()
{
    qWarning( "SystemDialogData::CmOpen_MTPARAM(): Not implemented yet!" );
}

void SystemDialogData::CmOpen_SYSTEM_ICOMP()
{
    qWarning( "SystemDialogData::CmOpen_SYSTEM_ICOMP(): Not implemented yet!" );
}

void SystemDialogData::CmOpen_inp_PHASE_HL()
{
    qWarning( "SystemDialogData::CmOpen_inp_PHASE_HL(): Not implemented yet!" );
}

void SystemDialogData::CmOpen_EQSTAT_ICOMP()
{
    qWarning( "SystemDialogData::CmOpen_EQSTAT_ICOMP(): Not implemented yet!" );
}

void SystemDialogData::CmOpen_res_PHASE_HL()
{
    qWarning( "SystemDialogData::CmOpen_res_PHASE_HL(): Not implemented yet!" );
}

void SystemDialogData::CmOpen_MULTI()
{
    qWarning( "SystemDialogData::CmOpen_MULTI(): Not implemented yet!" );
}

