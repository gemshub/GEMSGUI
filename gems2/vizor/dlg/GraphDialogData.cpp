/****************************************************************************
** Form implementation generated from reading ui file 'GraphDialog.ui'
**
** Created: Do 28. Feb 17:25:56 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "GraphDialogData.h"

#include <qvariant.h>
#include <qgroupbox.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a GraphDialogData which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
GraphDialogData::GraphDialogData( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "GraphDialogData" );
    resize( 610, 451 ); 
    setMinimumSize( QSize( 610, 450 ) );
    QFont f( font() );
    f.setFamily( "Lucida Sans Unicode" );
    f.setPointSize( 9 );
    setFont( f ); 
    setCaption( trUtf8( "GraphDialogData" ) );

    pGroupBox = new QGroupBox( this, "pGroupBox" );
    pGroupBox->setGeometry( QRect( 480, 10, 110, 140 ) ); 
    pGroupBox->setTitle( trUtf8( "" ) );
    pGroupBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, 0, 0, pGroupBox->sizePolicy().hasHeightForWidth() ) );
    pGroupBox->setFrameShape( QGroupBox::NoFrame );

    pFragmentBtn = new QPushButton( pGroupBox, "pFragmentBtn" );
    pFragmentBtn->setGeometry( QRect( 0, 35, 110, 33 ) ); 
    pFragmentBtn->setText( trUtf8( "&Fragment" ) );
    pFragmentBtn->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, 0, 0, pFragmentBtn->sizePolicy().hasHeightForWidth() ) );
    QFont pFragmentBtn_font(  pFragmentBtn->font() );
    pFragmentBtn_font.setPointSize( 10 );
    pFragmentBtn->setFont( pFragmentBtn_font ); 
    QToolTip::add( pFragmentBtn, trUtf8( "Shows a fragment of the plot - set up size and position under Configure..." ) );

    pBtn3 = new QPushButton( pGroupBox, "pBtn3" );
    pBtn3->setGeometry( QRect( 0, 70, 110, 33 ) ); 
    pBtn3->setText( trUtf8( "&Print..." ) );
    pBtn3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, 0, 0, pBtn3->sizePolicy().hasHeightForWidth() ) );
    QFont pBtn3_font(  pBtn3->font() );
    pBtn3_font.setPointSize( 10 );
    pBtn3->setFont( pBtn3_font ); 
    QToolTip::add( pBtn3, trUtf8( "Prints the plot and its legend (under construction)" ) );

    pBtn4 = new QPushButton( pGroupBox, "pBtn4" );
    pBtn4->setGeometry( QRect( 0, 106, 110, 33 ) ); 
    pBtn4->setText( trUtf8( "&Customize..." ) );
    pBtn4->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, 0, 0, pBtn4->sizePolicy().hasHeightForWidth() ) );
    QFont pBtn4_font(  pBtn4->font() );
    pBtn4_font.setPointSize( 10 );
    pBtn4->setFont( pBtn4_font ); 
    pBtn4->setToggleButton( FALSE );
    QToolTip::add( pBtn4, trUtf8( "Press to set up axii, colors, and graph legend symbols" ) );

    pBtn1 = new QPushButton( pGroupBox, "pBtn1" );
    pBtn1->setGeometry( QRect( 0, 0, 110, 33 ) ); 
    pBtn1->setText( trUtf8( "C&lose" ) );
    pBtn1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, 0, 0, pBtn1->sizePolicy().hasHeightForWidth() ) );
    QFont pBtn1_font(  pBtn1->font() );
    pBtn1_font.setPointSize( 10 );
    pBtn1->setFont( pBtn1_font ); 
    QToolTip::add( pBtn1, trUtf8( "Closes the plot and goes back to the parent window" ) );

    pGrpLegend = new QGroupBox( this, "pGrpLegend" );
    pGrpLegend->setGeometry( QRect( 480, 160, 110, 270 ) ); 
    pGrpLegend->setTitle( trUtf8( "Legend" ) );
    QFont pGrpLegend_font(  pGrpLegend->font() );
    pGrpLegend_font.setPointSize( 11 );
    pGrpLegend->setFont( pGrpLegend_font ); 
    pGrpLegend->setFrameShape( QGroupBox::Box );

    // signals and slots connections
    connect( pBtn4, SIGNAL( clicked() ), this, SLOT( CmLegend() ) );
    connect( pFragmentBtn, SIGNAL( clicked() ), this, SLOT( CmFragment() ) );
    connect( pBtn3, SIGNAL( clicked() ), this, SLOT( CmPrint() ) );
    connect( pBtn1, SIGNAL( clicked() ), this, SLOT( accept() ) );

    // tab order
    setTabOrder( pBtn1, pFragmentBtn );
    setTabOrder( pFragmentBtn, pBtn3 );
    setTabOrder( pBtn3, pBtn4 );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
GraphDialogData::~GraphDialogData()
{
    // no need to delete child widgets, Qt does it all for us
}

void GraphDialogData::CmFragment()
{
    qWarning( "GraphDialogData::CmFragment(): Not implemented yet!" );
}

void GraphDialogData::CmLegend()
{
    qWarning( "GraphDialogData::CmLegend(): Not implemented yet!" );
}

void GraphDialogData::CmPrint()
{
    qWarning( "GraphDialogData::CmPrint(): Not implemented yet!" );
}

