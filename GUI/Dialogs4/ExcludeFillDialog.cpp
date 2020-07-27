//-------------------------------------------------------------------
// $Id: ExcludeFillDialog.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of ExcludeFillDialog class
//
// Copyright (C) 1996-2008   S.Dmytriyeva
//
// This file is part of the GEM-Selektor GUI library which uses the
// Qt v.4 cross-platform App & UI framework (https://qt.io/download-open-source)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the GPL v.3 license

//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#include <qstring.h>
#include <qvalidator.h>
#include <QGroupBox>
#include <QScrollArea>

#include "ExcludeFillDialog.h"
#include "GemsMainWindow.h"
#include "service.h"


ExcludeFillDialog::ExcludeFillDialog(QWidget* win, const char* caption,
      TCStringArray& aICkeys, double fill_data ):
        QDialog(win )
{
   setWindowModality(Qt::WindowModal);
    
	setWindowTitle( trUtf8( caption ) );
    // qt3to4 setSizeGripEnabled( TRUE );

    // set up labels
    QVBoxLayout* labelsBox = new QVBoxLayout();
    QLabel*     label1;
    
    label1 = new QLabel( this );
    label1->setText( trUtf8( "C O N F L I C T   W A R N I N G !" ) );
    labelsBox->addWidget( label1 );
    
    label1 = new QLabel( this );
    label1->setText( trUtf8( "Mole amounts of some Independent Components (IC) \n"
                             "   are missing in the calculated bulk composition vector\n"
                             "   (B_[i] < Pa_DB)!\n" ) );
    labelsBox->addWidget( label1 );
    
    label1 = new QLabel( this );
    label1->setText( trUtf8( "P O S S I B L E   A C T I O N S :" ) );
    labelsBox->addWidget( label1 );

    label1 = new QLabel( this );
    label1->setText( trUtf8( "* EXCLUDE ALL these ICs together with DCs that\n"
"    contain them and some Phases made of those DCs;" ) );
    labelsBox->addWidget( label1 );

    label1 = new QLabel( this );
    label1->setText( trUtf8( "* RETAIN ALL missing ICs by inserting a default \n"
"   mole amount (below) into bi_ vector cells;" ) );
    labelsBox->addWidget( label1 );

    label1 = new QLabel( this );
    label1->setText( trUtf8( "* CHECK some boxes to keep these ICs in the\n"
"   system by inserting a default mole amount into bi_;\n"
"   unchecked ICs will be turned off together with all\n"
"   DCs that contain them.\n" ) );
    labelsBox->addWidget( label1 );

    QHBoxLayout* editBox = new QHBoxLayout();

    label1 = new QLabel( this );
    label1->setText( trUtf8( "Default amount, mol (editable):" ) );
    editBox->addWidget( label1 );

    LineEdit = new QLineEdit( this );
    LineEdit->setValidator( new QDoubleValidator( LineEdit ) );
    // vstr_buf(200);  sprintf( buf, "%g", fill_data ); LineEdit->setText( trUtf8( buf.p ) );
    QString str;
    LineEdit->setText( str.setNum( fill_data, 'g' ));
    editBox->addWidget( LineEdit );
    labelsBox->addLayout(editBox);   

   // buttoms layout 
    QVBoxLayout* buttonBox = new QVBoxLayout();
    
    pExcludeAll = new QPushButton( this );
    pExcludeAll->setText( trUtf8( "&EXCLUDE ALL" ) );
    pExcludeAll->setAutoDefault( true );
    buttonBox->addWidget( pExcludeAll );

    pFillAll = new QPushButton( this );
    pFillAll->setText( trUtf8( "&RETAIN ALL" ) );
    pFillAll->setAutoDefault( true );
    buttonBox->addWidget( pFillAll );

    buttonOk = new QPushButton( this );
    buttonOk->setText( trUtf8( "CHECK/&Ok" ) );
    buttonOk->setAutoDefault( true );
    buttonOk->setDefault( true );
    buttonBox->addWidget( buttonOk );

    buttonBox->addStretch();

    buttonHelp = new QPushButton( this );
    buttonHelp->setText( trUtf8( "&Learn more..." ) );
    buttonHelp->setAutoDefault( true );
    buttonBox->addWidget( buttonHelp );

    buttonCancel = new QPushButton( this );
    buttonCancel->setText( trUtf8( "&Cancel" ) );
    buttonCancel->setAutoDefault( true );
    buttonBox->addWidget( buttonCancel );

/// Missin IC
    QVBoxLayout* scrollBox = new QVBoxLayout();
    label1 = new QLabel( this );
    label1->setText( trUtf8( "Missing ICs" ) );
    scrollBox->addWidget( label1 );

    QGroupBox* grpBox = new QGroupBox( this );
    //grpBox->setMaximumHeight(400);
    //grpBox->setTitle( trUtf8( "Missing IC" ) );
    GroupBox1 = new QButtonGroup( grpBox );
    GroupBox1->setExclusive(false);
    QVBoxLayout *plotLayout = new QVBoxLayout( grpBox );
    plotLayout->setSpacing(6);
    plotLayout->setMargin(11);
    
    QCheckBox* CheckBox1;
    for(size_t ii=0; ii<aICkeys.GetCount(); ii++ )
    {
      CheckBox1 = new QCheckBox( grpBox );
      CheckBox1->setText( trUtf8( aICkeys[ii].c_str() ) );
      GroupBox1->addButton( CheckBox1, ii );
      plotLayout->addWidget( CheckBox1 );
    }
    QScrollArea *scroll = new QScrollArea(this);
    scroll->setWidget(grpBox);
    scrollBox->addWidget( scroll );

    QHBoxLayout* mainBox = new QHBoxLayout(this);
    //mainBox->addWidget( scroll/*grpBox*/ );
    mainBox->addLayout(scrollBox);
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
    pVisorImp->OpenHelp( GEMS_MISSING_HTML );
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

