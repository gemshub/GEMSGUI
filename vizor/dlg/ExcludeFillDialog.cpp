//-------------------------------------------------------------------
// $Id$
//
// Implementation of ExcludeFillDialog class
//
// Copyright (C) 1996-2001   S.Dmytriyeva
// Uses  gstring class (C) A.Rysin 1999
//
// This file is part of the GEM-Vizor library which uses the
// Qt v.2.x GUI Toolkit (Troll Tech AS, http://www.trolltech.com)
// according to the Qt Duo Commercial license
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#include "ExcludeFillDialog.h"

#include <qvariant.h>
#include <qcheckbox.h>
#include <qbuttongroup.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

#include "v_user.h"
#include "visor_w.h"


const char* GEMS_FILL_HTML = "exclude_fill";

ExcludeFillDialog::ExcludeFillDialog(QWidget* win, const char* caption,
      TCStringArray& aICkeys, double fill_data ):
        QDialog(win, 0, true)
{
    resize( 629, 421 );
    setCaption( trUtf8( caption ) );
    setSizeGripEnabled( TRUE );

    QFont btn_font( font() );
    btn_font.setFamily( "Arial" );
    btn_font.setBold( true );

    TextLabel1 = new QLabel( this, "TextLabel1" );
    TextLabel1->setGeometry( QRect( 150, 40, 359, 60 ) );
    TextLabel1->setText( trUtf8( "Moles of some independent components (IC) \n"
"are missing in the calculated bulk composition \n"
"vector (B_[i] < Pa_DB)!" ) );

    TextLabel1_3_2 = new QLabel( this, "TextLabel1_3_2" );
    TextLabel1_3_2->setGeometry( QRect( 130, 100, 250, 31 ) );
    TextLabel1_3_2->setText( trUtf8( "P O S S I B L E   A C T I O N S :" ) );

    TextLabel1_3 = new QLabel( this, "TextLabel1_3" );
    TextLabel1_3->setGeometry( QRect( 130, 10, 140, 31 ) );
    TextLabel1_3->setText( trUtf8( "W A R N I N G !" ) );

    QWidget* privateLayoutWidget = new QWidget( this, "Layout3" );
    privateLayoutWidget->setGeometry( QRect( 520, 20, 95, 390 ) );
    Layout3 = new QVBoxLayout( privateLayoutWidget, 0, 6, "Layout3");

    pExcludeAll = new QPushButton( privateLayoutWidget, "pExcludeAll" );
    pExcludeAll->setText( trUtf8( "&Exclude All" ) );
    pExcludeAll->setAutoDefault( TRUE );
    Layout3->addWidget( pExcludeAll );

    pFillAll = new QPushButton( privateLayoutWidget, "pFillAll" );
    pFillAll->setText( trUtf8( "&Retain All" ) );
    pFillAll->setAccel( 276824146 );
    pFillAll->setAutoDefault( TRUE );
    Layout3->addWidget( pFillAll );

    buttonOk = new QPushButton( privateLayoutWidget, "buttonOk" );
    buttonOk->setFont( btn_font );
    buttonOk->setText( trUtf8( "Check/&Ok" ) );
    buttonOk->setAccel( 276824143 );
    buttonOk->setAutoDefault( TRUE );
    buttonOk->setDefault( TRUE );
    Layout3->addWidget( buttonOk );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    Layout3->addItem( spacer );

    buttonHelp = new QPushButton( privateLayoutWidget, "buttonHelp" );
    buttonHelp->setFont( btn_font );
    buttonHelp->setText( trUtf8( "&Help" ) );
    buttonHelp->setAccel( 276824136 );
    buttonHelp->setAutoDefault( TRUE );
    Layout3->addWidget( buttonHelp );

    buttonCancel = new QPushButton( privateLayoutWidget, "buttonCancel" );
    buttonCancel->setFont( btn_font );
    buttonCancel->setText( trUtf8( "&Cancel" ) );
    buttonCancel->setAccel( 276824131 );
    buttonCancel->setAutoDefault( TRUE );
    Layout3->addWidget( buttonCancel );

    TextLabel1_2_2 = new QLabel( this, "TextLabel1_2_2" );
    TextLabel1_2_2->setGeometry( QRect( 130, 190, 384, 80 ) );
    TextLabel1_2_2->setText( trUtf8( "*  RETAIN All missing elements of the system bulk \n"
"   composition  by inserting fill-out mole quantity \n"
"   (below) into respective elements of  bi_  vector;" ) );

    TextLabel4 = new QLabel( this, "TextLabel4" );
    TextLabel4->setGeometry( QRect( 130, 260, 388, 90 ) );
    TextLabel4->setText( trUtf8( "*  CHECK some independent component symbols \n"
"    to retain them in the system (by setting Fill-out \n"
"    mole values). After pressing \"Ok\", unchecked \n"
"    IComps will be excluded." ) );

    TextLabel1_2 = new QLabel( this, "TextLabel1_2" );
    TextLabel1_2->setGeometry( QRect( 130, 130, 366, 60 ) );
    TextLabel1_2->setText( trUtf8( "*   EXCLUDE All these independent components, \n"
"    their dependent components and  phases from \n"
"    the chemical system definition;" ) );

    TextLabel3 = new QLabel( this, "TextLabel3" );
    TextLabel3->setGeometry( QRect( 140, 370, 140, 20 ) );
    TextLabel3->setText( trUtf8( "Edit Fill-out value:" ) );

    LineEdit1 = new QLineEdit( this, "LineEdit1" );
    LineEdit1->setGeometry( QRect( 280, 370, 110, 30 ) );
    vstr buf(200);
    sprintf( buf, "%g", fill_data );
    LineEdit1->setText( trUtf8( buf.p ) );

////
    GroupBox1 = new QButtonGroup( this, "pButtonBox" );

    GroupBox1->setGeometry( QRect( 10, 10, 110, 390 ) );
    GroupBox1->setTitle( trUtf8( "Missing IC" ) );

    uint ii;
    for( ii=0; ii<aICkeys.GetCount(); ii++ )
    {
      CheckBox1 = new QCheckBox( GroupBox1, aICkeys[ii].c_str() );
      CheckBox1->setGeometry( QRect( 10, 30+ii*20, 90, 20 ) );
      CheckBox1->setText( trUtf8( aICkeys[ii].c_str() ) );
      GroupBox1->insert( CheckBox1, ii );
    }

    if( (30+ii*20) > 390 )
    {
       GroupBox1->setGeometry( QRect( 10, 10, 110, (50+ii*20) ) );
//       resize( 640, (50+ii*20) );
    }

    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( pExcludeAll, SIGNAL( clicked() ), this, SLOT( CmExcludeAll() ) );
    connect( pFillAll, SIGNAL( clicked() ), this, SLOT( CmFillAll() ) );
    connect( buttonHelp, SIGNAL( clicked() ), this, SLOT( CmHelp() ) );
}

ExcludeFillDialog::~ExcludeFillDialog()
{
}

void
ExcludeFillDialog::CmHelp()
{
    pVisorImp->OpenHelp( GEMS_FILL_HTML, 0, this/*, true*/ );
}


void
ExcludeFillDialog::CmFillAll()
{
 for(int ii=0; ii<GroupBox1->count(); ii++ )
     ((QCheckBox*)(GroupBox1->find(ii)))->setChecked( true );
}


void
ExcludeFillDialog::CmExcludeAll()
{
 for(int ii=0; ii<GroupBox1->count(); ii++ )
     ((QCheckBox*)(GroupBox1->find(ii)))->setChecked( false );
}

TOArray<bool>
ExcludeFillDialog::getFillType()
{
 TOArray<bool> arr;

 for(int ii=0; ii<GroupBox1->count(); ii++ )
     arr.Add( ((QCheckBox*)(GroupBox1->find(ii)))->isChecked() );

 return arr;
}

double
ExcludeFillDialog::getFillValue()
{
 const char* buf = LineEdit1->text();
 double dat;
 sscanf( buf, "%lg", &dat );
 return dat;
}

// --------------------- End ExcludeFillDialog.cpp -------------------------

