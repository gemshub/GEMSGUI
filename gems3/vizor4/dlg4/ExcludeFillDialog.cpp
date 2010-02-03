//-------------------------------------------------------------------
// $Id: ExcludeFillDialog.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of ExcludeFillDialog class
//
// Copyright (C) 1996-2008   S.Dmytriyeva
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

#include <qstring.h>
#include <qvalidator.h>
#include <QGroupBox>

#include "v_user.h"
#include "visor_w.h"


const char* GEMS_FILL_HTML = "exclude_fill";

ExcludeFillDialog::ExcludeFillDialog(QWidget* win, const char* caption,
      TCStringArray& aICkeys, double fill_data ):
        QDialog(win )
{
    //resize( 629, 421 );
    
	setWindowTitle( trUtf8( caption ) );
    // qt3to4 setSizeGripEnabled( TRUE );

    // set up labels
    QVBoxLayout* labelsBox = new QVBoxLayout();
    QLabel*     label1;
    
    label1 = new QLabel( this );
    label1->setText( trUtf8( "W A R N I N G !" ) );
    labelsBox->addWidget( label1 );
    
    label1 = new QLabel( this );
    label1->setText( trUtf8( "Moles of some independent components (IC) \n"
                             "are missing in the calculated bulk composition \n"
                             "vector (B_[i] < Pa_DB)!" ) );
    labelsBox->addWidget( label1 );
    
    label1 = new QLabel( this );
    label1->setText( trUtf8( "P O S S I B L E   A C T I O N S :" ) );
    labelsBox->addWidget( label1 );

    label1 = new QLabel( this );
    label1->setText( trUtf8( "*   EXCLUDE All these independent components, \n"
"    their dependent components and  phases from \n"
"    the chemical system definition;" ) );
    labelsBox->addWidget( label1 );

    label1 = new QLabel( this );
    label1->setText( trUtf8( "*  RETAIN All missing elements of the system bulk \n"
"   composition  by inserting fill-out mole quantity \n"
"   (below) into respective elements of  bi_  vector;" ) );
    labelsBox->addWidget( label1 );

    label1 = new QLabel( this );
    label1->setText( trUtf8( "*  CHECK some independent component symbols \n"
"    to retain them in the system (by setting Fill-out \n"
"    mole values). After pressing \"Ok\", unchecked \n"
"    IComps will be excluded." ) );
    labelsBox->addWidget( label1 );

    QHBoxLayout* editBox = new QHBoxLayout();

    label1 = new QLabel( this );
    label1->setText( trUtf8( "Edit Fill-out value:" ) );
    editBox->addWidget( label1 );

    LineEdit = new QLineEdit( this );
    LineEdit->setValidator( new QDoubleValidator( LineEdit ) );
    // vstr buf(200);  sprintf( buf, "%g", fill_data ); LineEdit->setText( trUtf8( buf.p ) );
    QString str;
    LineEdit->setText( str.setNum( fill_data, 'g' ));
    editBox->addWidget( LineEdit );
    labelsBox->addLayout(editBox);   

   // buttoms layout 
    QVBoxLayout* buttonBox = new QVBoxLayout();
    
    pExcludeAll = new QPushButton( this );
    pExcludeAll->setText( trUtf8( "&Exclude All" ) );
    pExcludeAll->setAutoDefault( TRUE );
    buttonBox->addWidget( pExcludeAll );

    pFillAll = new QPushButton( this );
    pFillAll->setText( trUtf8( "&Retain All" ) );
    pFillAll->setAutoDefault( TRUE );
    buttonBox->addWidget( pFillAll );

    buttonOk = new QPushButton( this );
    buttonOk->setText( trUtf8( "Check/&Ok" ) );
    buttonOk->setAutoDefault( TRUE );
    buttonOk->setDefault( TRUE );
    buttonBox->addWidget( buttonOk );

    buttonBox->addStretch();

    buttonHelp = new QPushButton( this );
    buttonHelp->setText( trUtf8( "&Help" ) );
    buttonHelp->setAutoDefault( TRUE );
    buttonBox->addWidget( buttonHelp );

    buttonCancel = new QPushButton( this );
    buttonCancel->setText( trUtf8( "&Cancel" ) );
    buttonCancel->setAutoDefault( TRUE );
    buttonBox->addWidget( buttonCancel );

/// Missin IC
    QGroupBox* grpBox = new QGroupBox( this );
    grpBox->setTitle( trUtf8( "Missing IC" ) );
    GroupBox1 = new QButtonGroup( grpBox );
    GroupBox1->setExclusive(false);
    QVBoxLayout *plotLayout = new QVBoxLayout( grpBox );
    plotLayout->setSpacing(6);
    plotLayout->setMargin(11);
    
    QCheckBox* CheckBox1;
    for(uint ii=0; ii<aICkeys.GetCount(); ii++ )
    {
      CheckBox1 = new QCheckBox( grpBox );
      CheckBox1->setText( trUtf8( aICkeys[ii].c_str() ) );
      GroupBox1->addButton( CheckBox1, ii );
      plotLayout->addWidget( CheckBox1 );
    }

    QHBoxLayout* mainBox = new QHBoxLayout(this);
    mainBox->addWidget( grpBox );
    mainBox->addLayout(labelsBox);   
    mainBox->addLayout(buttonBox);   
    
    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( pExcludeAll, SIGNAL( clicked() ), this, SLOT( CmExcludeAll() ) );
    connect( pFillAll, SIGNAL( clicked() ), this, SLOT( CmFillAll() ) );
    connect( buttonHelp, SIGNAL( clicked() ), this, SLOT( CmHelp() ) );
    
    adjustSize();
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
 for(int ii=0; ii<GroupBox1->buttons().count(); ii++ )
     GroupBox1->button(ii)->setChecked( true );
}


void
ExcludeFillDialog::CmExcludeAll()
{
 for(int ii=0; ii<GroupBox1->buttons().count(); ii++ )
     GroupBox1->button(ii)->setChecked( false );
}

TOArray<bool>
ExcludeFillDialog::getFillType()
{
 TOArray<bool> arr;

 for(int ii=0; ii<GroupBox1->buttons().count(); ii++ )
     arr.Add( GroupBox1->button(ii)->isChecked() );

 return arr;
}

double
ExcludeFillDialog::getFillValue()
{
 double dat = LineEdit->text().toDouble();
 return dat;
}

// --------------------- End ExcludeFillDialog.cpp -------------------------

