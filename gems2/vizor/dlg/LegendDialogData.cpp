/****************************************************************************
** Form implementation generated from reading ui file 'LegendDialog.ui'
**
** Created: Пнд Гру 17 12:44:07 2001
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "LegendDialogData.h"

#include <qvariant.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a LegendDialogData which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
LegendDialogData::LegendDialogData( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "LegendDialogData" );
    resize( 531, 394 ); 
    QFont f( font() );
    f.setFamily( "Lucida Sans Unicode" );
    f.setPointSize( 9 );
    setFont( f ); 
    setCaption( trUtf8( "Legend and Configuration for: " ) );

    GroupBox1 = new QGroupBox( this, "GroupBox1" );
    GroupBox1->setGeometry( QRect( 10, 10, 410, 200 ) ); 
    GroupBox1->setTitle( trUtf8( "Axis Setup" ) );

    TextLabel1 = new QLabel( GroupBox1, "TextLabel1" );
    TextLabel1->setGeometry( QRect( 10, 30, 70, 20 ) ); 
    TextLabel1->setText( trUtf8( "Axis type" ) );

    pAxis = new QSpinBox( GroupBox1, "pAxis" );
    pAxis->setGeometry( QRect( 80, 30, 56, 23 ) ); 
    pAxis->setMaxValue( 20 );

    TextLabel1_2_2 = new QLabel( GroupBox1, "TextLabel1_2_2" );
    TextLabel1_2_2->setGeometry( QRect( 121, 70, 30, 20 ) ); 
    TextLabel1_2_2->setText( trUtf8( "x:" ) );

    TextLabel1_2_2_2 = new QLabel( GroupBox1, "TextLabel1_2_2_2" );
    TextLabel1_2_2_2->setGeometry( QRect( 121, 94, 30, 20 ) ); 
    TextLabel1_2_2_2->setText( trUtf8( "y:" ) );

    TextLabel1_2_2_3 = new QLabel( GroupBox1, "TextLabel1_2_2_3" );
    TextLabel1_2_2_3->setGeometry( QRect( 121, 140, 30, 20 ) ); 
    TextLabel1_2_2_3->setText( trUtf8( "x:" ) );

    TextLabel1_2_2_2_2 = new QLabel( GroupBox1, "TextLabel1_2_2_2_2" );
    TextLabel1_2_2_2_2->setGeometry( QRect( 121, 165, 30, 20 ) ); 
    TextLabel1_2_2_2_2->setText( trUtf8( "y:" ) );

    TextLabel1_2 = new QLabel( GroupBox1, "TextLabel1_2" );
    TextLabel1_2->setGeometry( QRect( 30, 70, 61, 20 ) ); 
    TextLabel1_2->setText( trUtf8( "Graph" ) );

    TextLabel1_2_3 = new QLabel( GroupBox1, "TextLabel1_2_3" );
    TextLabel1_2_3->setGeometry( QRect( 30, 140, 70, 20 ) ); 
    TextLabel1_2_3->setText( trUtf8( "Fragment" ) );

    pX0 = new QLineEdit( GroupBox1, "pX0" );
    pX0->setGeometry( QRect( 150, 70, 110, 22 ) ); 
    QToolTip::add( pX0, trUtf8( "Minimum" ) );

    pXn = new QLineEdit( GroupBox1, "pXn" );
    pXn->setGeometry( QRect( 280, 70, 110, 22 ) ); 
    QToolTip::add( pXn, trUtf8( "Maximum" ) );

    pY0 = new QLineEdit( GroupBox1, "pY0" );
    pY0->setGeometry( QRect( 150, 94, 110, 22 ) ); 
    QToolTip::add( pY0, trUtf8( "Minimum" ) );

    pYn = new QLineEdit( GroupBox1, "pYn" );
    pYn->setGeometry( QRect( 280, 94, 110, 22 ) ); 
    QToolTip::add( pYn, trUtf8( "Maximum" ) );

    pfX0 = new QLineEdit( GroupBox1, "pfX0" );
    pfX0->setGeometry( QRect( 150, 140, 110, 22 ) ); 
    QToolTip::add( pfX0, trUtf8( "Minimum" ) );

    pfXn = new QLineEdit( GroupBox1, "pfXn" );
    pfXn->setGeometry( QRect( 280, 140, 110, 22 ) ); 
    QToolTip::add( pfXn, trUtf8( "Maximum" ) );

    pfY0 = new QLineEdit( GroupBox1, "pfY0" );
    pfY0->setGeometry( QRect( 150, 165, 110, 22 ) ); 
    QToolTip::add( pfY0, trUtf8( "Minimum" ) );

    pfYn = new QLineEdit( GroupBox1, "pfYn" );
    pfYn->setGeometry( QRect( 280, 165, 110, 22 ) ); 
    QToolTip::add( pfYn, trUtf8( "Maximum" ) );

    pYname = new QLineEdit( GroupBox1, "pYname" );
    pYname->setGeometry( QRect( 280, 30, 110, 22 ) ); 
    pYname->setMaxLength( 9 );
    QToolTip::add( pYname, trUtf8( "Name of  y  axis" ) );

    pXname = new QLineEdit( GroupBox1, "pXname" );
    pXname->setGeometry( QRect( 150, 30, 110, 22 ) ); 
    pXname->setMaxLength( 9 );
    QToolTip::add( pXname, trUtf8( "Name of  x  axis" ) );

    pBtnApply = new QPushButton( this, "pBtnApply" );
    pBtnApply->setGeometry( QRect( 430, 60, 100, 30 ) ); 
    pBtnApply->setText( trUtf8( "&Apply" ) );
    QToolTip::add( pBtnApply, trUtf8( "Apply changes to the graph and do not close this dialog" ) );

    pBthCancel = new QPushButton( this, "pBthCancel" );
    pBthCancel->setGeometry( QRect( 430, 100, 100, 30 ) ); 
    pBthCancel->setText( trUtf8( "&Cancel" ) );
    QToolTip::add( pBthCancel, trUtf8( "Discard  changes and close this dialog" ) );

    pBtnBackgr = new QPushButton( this, "pBtnBackgr" );
    pBtnBackgr->setGeometry( QRect( 430, 140, 100, 30 ) ); 
    pBtnBackgr->setText( trUtf8( "&Background..." ) );
    QToolTip::add( pBtnBackgr, trUtf8( "Set background color of the graph area" ) );

    pBtnOk = new QPushButton( this, "pBtnOk" );
    pBtnOk->setGeometry( QRect( 430, 20, 100, 30 ) ); 
    pBtnOk->setText( trUtf8( "&OK" ) );
    pBtnOk->setDefault( TRUE );
    QToolTip::add( pBtnOk, trUtf8( "Apply changes to the graph and close this dialog" ) );

    // signals and slots connections
    connect( pBtnApply, SIGNAL( clicked() ), this, SLOT( CmApply() ) );
    connect( pBtnOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( pBthCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( pBtnBackgr, SIGNAL( clicked() ), this, SLOT( CmBackground() ) );

    // tab order
    setTabOrder( pAxis, pXname );
    setTabOrder( pXname, pYname );
    setTabOrder( pYname, pX0 );
    setTabOrder( pX0, pXn );
    setTabOrder( pXn, pY0 );
    setTabOrder( pY0, pYn );
    setTabOrder( pYn, pfX0 );
    setTabOrder( pfX0, pfXn );
    setTabOrder( pfXn, pfY0 );
    setTabOrder( pfY0, pfYn );
    setTabOrder( pfYn, pBtnOk );
    setTabOrder( pBtnOk, pBtnApply );
    setTabOrder( pBtnApply, pBthCancel );
    setTabOrder( pBthCancel, pBtnBackgr );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
LegendDialogData::~LegendDialogData()
{
    // no need to delete child widgets, Qt does it all for us
}

void LegendDialogData::CmApply()
{
    qWarning( "LegendDialogData::CmApply(): Not implemented yet!" );
}

void LegendDialogData::CmBackground()
{
    qWarning( "LegendDialogData::CmBackground(): Not implemented yet!" );
}

