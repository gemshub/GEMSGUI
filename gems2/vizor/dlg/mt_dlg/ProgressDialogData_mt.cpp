/****************************************************************************
** Form implementation generated from reading ui file 'ProgressDialog_mt.ui'
**
** Created: Sun Sep 9 17:06:57 2001
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "ProgressDialogData_mt.h"

#include <qframe.h>
#include <qlabel.h>
#include <qprogressbar.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a ProgressDialogData which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
ProgressDialogData::ProgressDialogData( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "ProgressDialogData" );
    resize( 310, 461 ); 
    setMinimumSize( QSize( 310, 460 ) );
    QFont f( font() );
    f.setFamily( "Lucida Sans Unicode" );
    f.setPointSize( 9 );
    setFont( f ); 
    setCaption( tr( "ProgressDialogData" ) );

    TextLabel1 = new QLabel( this, "TextLabel1" );
    TextLabel1->setGeometry( QRect( 10, 10, 240, 20 ) ); 
    QFont TextLabel1_font(  TextLabel1->font() );
    TextLabel1_font.setPointSize( 11 );
    TextLabel1->setFont( TextLabel1_font ); 
    TextLabel1->setText( tr( "IPM Minimization Progress" ) );

    TextLabel3_3 = new QLabel( this, "TextLabel3_3" );
    TextLabel3_3->setGeometry( QRect( 10, 70, 80, 21 ) ); 
    QFont TextLabel3_3_font(  TextLabel3_3->font() );
    TextLabel3_3_font.setPointSize( 12 );
    TextLabel3_3->setFont( TextLabel3_3_font ); 
    TextLabel3_3->setText( tr( "System: " ) );

    QWidget* privateLayoutWidget = new QWidget( this, "vbox" );
    privateLayoutWidget->setGeometry( QRect( 90, 130, 210, 261 ) ); 
    vbox = new QGridLayout( privateLayoutWidget ); 
    vbox->setSpacing( 0 );
    vbox->setMargin( 0 );

    TextLabel3_2_2_3 = new QLabel( privateLayoutWidget, "TextLabel3_2_2_3" );
    QFont TextLabel3_2_2_3_font(  TextLabel3_2_2_3->font() );
    TextLabel3_2_2_3_font.setPointSize( 12 );
    TextLabel3_2_2_3->setFont( TextLabel3_2_2_3_font ); 
    TextLabel3_2_2_3->setText( tr( "pe  " ) );
    TextLabel3_2_2_3->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

    vbox->addWidget( TextLabel3_2_2_3, 5, 0 );

    TextLabel3_2 = new QLabel( privateLayoutWidget, "TextLabel3_2" );
    QFont TextLabel3_2_font(  TextLabel3_2->font() );
    TextLabel3_2_font.setPointSize( 12 );
    TextLabel3_2->setFont( TextLabel3_2_font ); 
    TextLabel3_2->setText( tr( "Gas  " ) );
    TextLabel3_2->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

    vbox->addWidget( TextLabel3_2, 1, 0 );

    pSolid = new QLabel( privateLayoutWidget, "pSolid" );
    QFont pSolid_font(  pSolid->font() );
    pSolid_font.setPointSize( 11 );
    pSolid->setFont( pSolid_font ); 
    pSolid->setFrameShape( QLabel::Box );
    pSolid->setFrameShadow( QLabel::Raised );
    pSolid->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );
    QToolTip::add(  pSolid, tr( "Total mass of solid phases in the system (g)" ) );

    vbox->addWidget( pSolid, 3, 1 );

    pPE = new QLabel( privateLayoutWidget, "pPE" );
    QFont pPE_font(  pPE->font() );
    pPE_font.setPointSize( 11 );
    pPE->setFont( pPE_font ); 
    pPE->setFrameShape( QLabel::Box );
    pPE->setFrameShadow( QLabel::Raised );
    pPE->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );
    QToolTip::add(  pPE, tr( "pe value of aqueous electrolyte, molal" ) );

    vbox->addWidget( pPE, 5, 1 );

    TextLabel3_2_3 = new QLabel( privateLayoutWidget, "TextLabel3_2_3" );
    QFont TextLabel3_2_3_font(  TextLabel3_2_3->font() );
    TextLabel3_2_3_font.setPointSize( 12 );
    TextLabel3_2_3->setFont( TextLabel3_2_3_font ); 
    TextLabel3_2_3->setText( tr( "pH  " ) );
    TextLabel3_2_3->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

    vbox->addWidget( TextLabel3_2_3, 4, 0 );

    pGas = new QLabel( privateLayoutWidget, "pGas" );
    QFont pGas_font(  pGas->font() );
    pGas_font.setPointSize( 11 );
    pGas->setFont( pGas_font ); 
    pGas->setFrameShape( QLabel::Box );
    pGas->setFrameShadow( QLabel::Raised );
    pGas->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );
    QToolTip::add(  pGas, tr( "Mass of gas (in grams) in the system" ) );

    vbox->addWidget( pGas, 1, 1 );

    pWater = new QLabel( privateLayoutWidget, "pWater" );
    QFont pWater_font(  pWater->font() );
    pWater_font.setPointSize( 11 );
    pWater->setFont( pWater_font ); 
    pWater->setFrameShape( QLabel::Box );
    pWater->setFrameShadow( QLabel::Raised );
    pWater->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );
    QToolTip::add(  pWater, tr( "Total mass of liquid phases in the system (g)" ) );

    vbox->addWidget( pWater, 2, 1 );

    TextLabel3_2_2_2_2 = new QLabel( privateLayoutWidget, "TextLabel3_2_2_2_2" );
    QFont TextLabel3_2_2_2_2_font(  TextLabel3_2_2_2_2->font() );
    TextLabel3_2_2_2_2_font.setPointSize( 12 );
    TextLabel3_2_2_2_2->setFont( TextLabel3_2_2_2_2_font ); 
    TextLabel3_2_2_2_2->setText( tr( "IS  " ) );
    TextLabel3_2_2_2_2->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

    vbox->addWidget( TextLabel3_2_2_2_2, 6, 0 );

    pIT = new QLabel( privateLayoutWidget, "pIT" );
    QFont pIT_font(  pIT->font() );
    pIT_font.setPointSize( 11 );
    pIT->setFont( pIT_font ); 
    pIT->setFrameShape( QLabel::Box );
    pIT->setFrameShadow( QLabel::Raised );
    pIT->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );
    QToolTip::add(  pIT, tr( "Number of IPM iterations performed until the last update of dialog fields" ) );

    vbox->addWidget( pIT, 0, 1 );

    pPH = new QLabel( privateLayoutWidget, "pPH" );
    QFont pPH_font(  pPH->font() );
    pPH_font.setPointSize( 11 );
    pPH->setFont( pPH_font ); 
    pPH->setFrameShape( QLabel::Box );
    pPH->setFrameShadow( QLabel::Raised );
    pPH->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );
    QToolTip::add(  pPH, tr( "pH value of aqueous electrolyte, molal (activity scale)" ) );

    vbox->addWidget( pPH, 4, 1 );

    pIC = new QLabel( privateLayoutWidget, "pIC" );
    QFont pIC_font(  pIC->font() );
    pIC_font.setPointSize( 11 );
    pIC->setFont( pIC_font ); 
    pIC->setFrameShape( QLabel::Box );
    pIC->setFrameShadow( QLabel::Raised );
    pIC->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );
    QToolTip::add(  pIC, tr( "Effective ionic strength of aqueous electrolyte, molal" ) );

    vbox->addWidget( pIC, 6, 1 );

    TextLabel3_2_2_2 = new QLabel( privateLayoutWidget, "TextLabel3_2_2_2" );
    QFont TextLabel3_2_2_2_font(  TextLabel3_2_2_2->font() );
    TextLabel3_2_2_2_font.setPointSize( 12 );
    TextLabel3_2_2_2->setFont( TextLabel3_2_2_2_font ); 
    TextLabel3_2_2_2->setText( tr( "Solid  " ) );
    TextLabel3_2_2_2->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

    vbox->addWidget( TextLabel3_2_2_2, 3, 0 );

    TextLabel3_2_2 = new QLabel( privateLayoutWidget, "TextLabel3_2_2" );
    QFont TextLabel3_2_2_font(  TextLabel3_2_2->font() );
    TextLabel3_2_2_font.setPointSize( 12 );
    TextLabel3_2_2->setFont( TextLabel3_2_2_font ); 
    TextLabel3_2_2->setText( tr( "Liquid  " ) );
    TextLabel3_2_2->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

    vbox->addWidget( TextLabel3_2_2, 2, 0 );

    TextLabel3 = new QLabel( privateLayoutWidget, "TextLabel3" );
    QFont TextLabel3_font(  TextLabel3->font() );
    TextLabel3_font.setPointSize( 12 );
    TextLabel3->setFont( TextLabel3_font ); 
    TextLabel3->setText( tr( "Iter  " ) );
    TextLabel3->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

    vbox->addWidget( TextLabel3, 0, 0 );

    Line1 = new QFrame( this, "Line1" );
    Line1->setGeometry( QRect( 10, 390, 290, 20 ) ); 
    Line1->setFrameStyle( QFrame::HLine | QFrame::Sunken );

    QWidget* privateLayoutWidget_2 = new QWidget( this, "Layout4" );
    privateLayoutWidget_2->setGeometry( QRect( 21, 411, 270, 39 ) ); 
    Layout4 = new QHBoxLayout( privateLayoutWidget_2 ); 
    Layout4->setSpacing( 6 );
    Layout4->setMargin( 0 );

    pStep = new QPushButton( privateLayoutWidget_2, "pStep" );
    pStep->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, pStep->sizePolicy().hasHeightForWidth() ) );
    QFont pStep_font(  pStep->font() );
    pStep_font.setPointSize( 12 );
    pStep->setFont( pStep_font ); 
    pStep->setText( tr( "&Step" ) );
    QToolTip::add(  pStep, tr( "Make next iteration in Stepwise mode" ) );
    Layout4->addWidget( pStep );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout4->addItem( spacer );

    pClose = new QPushButton( privateLayoutWidget_2, "pClose" );
    pClose->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, pClose->sizePolicy().hasHeightForWidth() ) );
    QFont pClose_font(  pClose->font() );
    pClose_font.setPointSize( 12 );
    pClose->setFont( pClose_font ); 
    pClose->setText( tr( "&Cancel" ) );
    QToolTip::add(  pClose, tr( "Do not save IPM results to database (or cancel IPM run in Stepwise mode)" ) );
    Layout4->addWidget( pClose );

    pBottle = new QFrame( this, "pBottle" );
    pBottle->setGeometry( QRect( 10, 130, 80, 260 ) ); 
    pBottle->setFrameShape( QFrame::StyledPanel );
    pBottle->setFrameShadow( QFrame::Raised );
    QToolTip::add(  pBottle, tr( "Shows current mass ratio of gas (white), liquid (blue) and solid (black) phases " ) );

    pProgress = new QProgressBar( this, "pProgress" );
    pProgress->setGeometry( QRect( 10, 30, 290, 31 ) ); 
    QFont pProgress_font(  pProgress->font() );
    pProgress_font.setPointSize( 12 );
    pProgress->setFont( pProgress_font ); 
    pProgress->setTotalSteps( 7 );
    pProgress->setProgress( 1 );
    QToolTip::add(  pProgress, tr( "Shows how far the GEM IPM proceeded from FIA to G(x) minimum" ) );

    pKey = new QLabel( this, "pKey" );
    pKey->setGeometry( QRect( 10, 90, 290, 30 ) ); 
    QFont pKey_font(  pKey->font() );
    pKey_font.setPointSize( 10 );
    pKey->setFont( pKey_font ); 
    pKey->setFrameShape( QLabel::StyledPanel );
    pKey->setFrameShadow( QLabel::Sunken );
    pKey->setText( tr( ":" ) );
    QToolTip::add(  pKey, tr( "Key of the currently processed SysEq database record" ) );

    pAccept = new QPushButton( this, "pAccept" );
    pAccept->setGeometry( QRect( 20, 410, 80, 37 ) ); 
    pAccept->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, pAccept->sizePolicy().hasHeightForWidth() ) );
    QFont pAccept_font(  pAccept->font() );
    pAccept_font.setPointSize( 12 );
    pAccept->setFont( pAccept_font ); 
    pAccept->setText( tr( "&Accept" ) );
    QToolTip::add(  pAccept, tr( "Save IPM results to database" ) );

    // signals and slots connections
    connect( pStep, SIGNAL( clicked() ), this, SLOT( CmStep() ) );
    connect( pClose, SIGNAL( clicked() ), this, SLOT( CmClose() ) );
    connect( pAccept, SIGNAL( clicked() ), this, SLOT( CmAccept() ) );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
ProgressDialogData::~ProgressDialogData()
{
    // no need to delete child widgets, Qt does it all for us
}

/*  
 *  Main event handler. Reimplemented to handle application
 *  font changes
 */
bool ProgressDialogData::event( QEvent* ev )
{
    bool ret = QDialog::event( ev ); 
    if ( ev->type() == QEvent::ApplicationFontChange ) {
	QFont TextLabel1_font(  TextLabel1->font() );
	TextLabel1_font.setPointSize( 11 );
	TextLabel1->setFont( TextLabel1_font ); 
	QFont TextLabel3_3_font(  TextLabel3_3->font() );
	TextLabel3_3_font.setPointSize( 12 );
	TextLabel3_3->setFont( TextLabel3_3_font ); 
	QFont TextLabel3_2_2_3_font(  TextLabel3_2_2_3->font() );
	TextLabel3_2_2_3_font.setPointSize( 12 );
	TextLabel3_2_2_3->setFont( TextLabel3_2_2_3_font ); 
	QFont TextLabel3_2_font(  TextLabel3_2->font() );
	TextLabel3_2_font.setPointSize( 12 );
	TextLabel3_2->setFont( TextLabel3_2_font ); 
	QFont pSolid_font(  pSolid->font() );
	pSolid_font.setPointSize( 11 );
	pSolid->setFont( pSolid_font ); 
	QFont pPE_font(  pPE->font() );
	pPE_font.setPointSize( 11 );
	pPE->setFont( pPE_font ); 
	QFont TextLabel3_2_3_font(  TextLabel3_2_3->font() );
	TextLabel3_2_3_font.setPointSize( 12 );
	TextLabel3_2_3->setFont( TextLabel3_2_3_font ); 
	QFont pGas_font(  pGas->font() );
	pGas_font.setPointSize( 11 );
	pGas->setFont( pGas_font ); 
	QFont pWater_font(  pWater->font() );
	pWater_font.setPointSize( 11 );
	pWater->setFont( pWater_font ); 
	QFont TextLabel3_2_2_2_2_font(  TextLabel3_2_2_2_2->font() );
	TextLabel3_2_2_2_2_font.setPointSize( 12 );
	TextLabel3_2_2_2_2->setFont( TextLabel3_2_2_2_2_font ); 
	QFont pIT_font(  pIT->font() );
	pIT_font.setPointSize( 11 );
	pIT->setFont( pIT_font ); 
	QFont pPH_font(  pPH->font() );
	pPH_font.setPointSize( 11 );
	pPH->setFont( pPH_font ); 
	QFont pIC_font(  pIC->font() );
	pIC_font.setPointSize( 11 );
	pIC->setFont( pIC_font ); 
	QFont TextLabel3_2_2_2_font(  TextLabel3_2_2_2->font() );
	TextLabel3_2_2_2_font.setPointSize( 12 );
	TextLabel3_2_2_2->setFont( TextLabel3_2_2_2_font ); 
	QFont TextLabel3_2_2_font(  TextLabel3_2_2->font() );
	TextLabel3_2_2_font.setPointSize( 12 );
	TextLabel3_2_2->setFont( TextLabel3_2_2_font ); 
	QFont TextLabel3_font(  TextLabel3->font() );
	TextLabel3_font.setPointSize( 12 );
	TextLabel3->setFont( TextLabel3_font ); 
	QFont pStep_font(  pStep->font() );
	pStep_font.setPointSize( 12 );
	pStep->setFont( pStep_font ); 
	QFont pClose_font(  pClose->font() );
	pClose_font.setPointSize( 12 );
	pClose->setFont( pClose_font ); 
	QFont pProgress_font(  pProgress->font() );
	pProgress_font.setPointSize( 12 );
	pProgress->setFont( pProgress_font ); 
	QFont pKey_font(  pKey->font() );
	pKey_font.setPointSize( 10 );
	pKey->setFont( pKey_font ); 
	QFont pAccept_font(  pAccept->font() );
	pAccept_font.setPointSize( 12 );
	pAccept->setFont( pAccept_font ); 
    }
    return ret;
}

void ProgressDialogData::CmAccept()
{
    qWarning( "ProgressDialogData::CmAccept(): Not implemented yet!" );
}

void ProgressDialogData::CmClose()
{
    qWarning( "ProgressDialogData::CmClose(): Not implemented yet!" );
}

void ProgressDialogData::CmStep()
{
    qWarning( "ProgressDialogData::CmStep(): Not implemented yet!" );
}

