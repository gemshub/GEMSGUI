/****************************************************************************
** Form implementation generated from reading ui file 'ListFilesDialog.ui'
**
** Created: Thu Jan 17 13:31:00 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "ListFilesDialogData.h"

#include <qheader.h>
#include <qlabel.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a ListFilesDialogData which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
ListFilesDialogData::ListFilesDialogData( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "ListFilesDialogData" );
    resize( 570, 454 ); 
    setMinimumSize( QSize( 450, 400 ) );
    QFont f( font() );
    f.setFamily( "Lucida Sans Unicode" );
    setFont( f ); 
    setCaption( tr( "Database File Configuration for Modelling Profile" ) );

    QWidget* privateLayoutWidget = new QWidget( this, "Layout3" );
    privateLayoutWidget->setGeometry( QRect( 10, 410, 560, 37 ) ); 
    Layout3 = new QHBoxLayout( privateLayoutWidget ); 
    Layout3->setSpacing( 6 );
    Layout3->setMargin( 0 );

    pHelpButton = new QPushButton( privateLayoutWidget, "pHelpButton" );
    pHelpButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, pHelpButton->sizePolicy().hasHeightForWidth() ) );
    QFont pHelpButton_font(  pHelpButton->font() );
    pHelpButton_font.setPointSize( 10 );
    pHelpButton->setFont( pHelpButton_font ); 
    pHelpButton->setText( tr( "&Help" ) );
    QToolTip::add(  pHelpButton, tr( "Learn about System Profile concept and profile configuration" ) );
    Layout3->addWidget( pHelpButton );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout3->addItem( spacer );

    pGO_OKButton = new QPushButton( privateLayoutWidget, "pGO_OKButton" );
    pGO_OKButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, pGO_OKButton->sizePolicy().hasHeightForWidth() ) );
    QFont pGO_OKButton_font(  pGO_OKButton->font() );
    pGO_OKButton_font.setPointSize( 10 );
    pGO_OKButton->setFont( pGO_OKButton_font ); 
    pGO_OKButton->setText( tr( "&Ok" ) );
    pGO_OKButton->setDefault( TRUE );
    QToolTip::add(  pGO_OKButton, tr( "Files selections is finisded OK" ) );
    Layout3->addWidget( pGO_OKButton );

    pCancelButton = new QPushButton( privateLayoutWidget, "pCancelButton" );
    pCancelButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, pCancelButton->sizePolicy().hasHeightForWidth() ) );
    QFont pCancelButton_font(  pCancelButton->font() );
    pCancelButton_font.setPointSize( 10 );
    pCancelButton->setFont( pCancelButton_font ); 
    pCancelButton->setText( tr( "&Cancel" ) );
    Layout3->addWidget( pCancelButton );

    pListFiles = new QListView( this, "pListFiles" );
    pListFiles->addColumn( tr( "File Keyword" ) );
    pListFiles->addColumn( tr( "File Path" ) );
    pListFiles->setGeometry( QRect( 10, 30, 560, 370 ) ); 
    pListFiles->setAllColumnsShowFocus( FALSE );
    pListFiles->setItemMargin( 1 );

    pLabel = new QLabel( this, "pLabel" );
    pLabel->setGeometry( QRect( 0, 0, 440, 20 ) ); 
    QFont pLabel_font(  pLabel->font() );
    pLabel_font.setPointSize( 10 );
    pLabel->setFont( pLabel_font ); 
    pLabel->setText( tr( "   Please, check files linked to Profile:" ) );
    pLabel->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignLeft ) );

    // signals and slots connections
    connect( pGO_OKButton, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( pHelpButton, SIGNAL( clicked() ), this, SLOT( CmHelp() ) );
    connect( pCancelButton, SIGNAL( clicked() ), this, SLOT( reject() ) );

    // tab order
    setTabOrder( pListFiles, pGO_OKButton );
    setTabOrder( pGO_OKButton, pHelpButton );
    setTabOrder( pHelpButton, pCancelButton );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
ListFilesDialogData::~ListFilesDialogData()
{
    // no need to delete child widgets, Qt does it all for us
}

/*  
 *  Main event handler. Reimplemented to handle application
 *  font changes
 */
bool ListFilesDialogData::event( QEvent* ev )
{
    bool ret = QDialog::event( ev ); 
    if ( ev->type() == QEvent::ApplicationFontChange ) {
	QFont pHelpButton_font(  pHelpButton->font() );
	pHelpButton_font.setPointSize( 10 );
	pHelpButton->setFont( pHelpButton_font ); 
	QFont pGO_OKButton_font(  pGO_OKButton->font() );
	pGO_OKButton_font.setPointSize( 10 );
	pGO_OKButton->setFont( pGO_OKButton_font ); 
	QFont pCancelButton_font(  pCancelButton->font() );
	pCancelButton_font.setPointSize( 10 );
	pCancelButton->setFont( pCancelButton_font ); 
	QFont pLabel_font(  pLabel->font() );
	pLabel_font.setPointSize( 10 );
	pLabel->setFont( pLabel_font ); 
    }
    return ret;
}

void ListFilesDialogData::CmHelp()
{
    qWarning( "ListFilesDialogData::CmHelp(): Not implemented yet!" );
}

