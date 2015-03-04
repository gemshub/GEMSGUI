//-------------------------------------------------------------------
// $Id: ColorDialog.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of SymbolDialog class
//
// Copyright (C) 1996-2013  S.Dmytriyeva, A.Rysin
// Uses  string class (C) A.Rysin 1999
//
// This file is part of the GEM-Selektor GUI library which uses the
// Qt v.4 cross-platform App & UI framework (http://qt.nokia.com)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the terms of GEMS3 Development
// Quality Assurance Licence (GEMS3.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#include <QColorDialog>
#include <QRadioButton>

#include "SymbolDialog.h"
#include "GraphDialog.h"

SymbolDialog::SymbolDialog( TPlotLine& aData, QWidget* parent):
        QDialog( parent ), dat(aData)
{
    setupUi(this);

    smbGroup = new QButtonGroup();
    smbGroup->setExclusive(true);

    QRadioButton *pp;
    QIcon icon;
    TPlotLine plot_( "Scale",  0, 0, 0, 0, 0, 60 );

    for(int ii=0; ii<24; ii++ )
    {
      pp = new QRadioButton(groupBox);
      // make symbol icons by program
      plot_.setType( ii );
      paintIcon( icon, plot_ );
      pp->setIcon(icon);
      smbGroup->addButton(pp, ii);
      gridLayout->addWidget(pp, ii/6, ii%6, 1, 1);
    }

    pColor->setAutoFillBackground( true); 
    QPalette pl = pColor->palette();
    pl.setColor( QPalette::Window, QColor(dat.getColor()) );
    pColor->setPalette(pl);
    QObject::connect(PushButton12, SIGNAL(clicked()), this, SLOT(CmSelectColor()));
    pLine->setValue( dat.getLineSize() );
    pSize->setValue( dat.getSize() );
    smbGroup->button( dat.getType() )->setChecked( true );

    string s= "Customize plot curve: ";
    s+= dat.getName();
    setWindowTitle(s.c_str());
}

SymbolDialog::~SymbolDialog()
{}

void SymbolDialog::languageChange()
{
    retranslateUi(this);
}

TPlotLine& SymbolDialog::GetPlotLine()
{
    int type = 0;
    for( int ii=0; ii<24; ii++)
    {
        if( smbGroup->button(ii)->isChecked() )
        {
            type = ii;
            break;
        }
    }

    QColor cl = pColor->palette().color(QPalette::Window); //backgroundColor();
    dat.setChanges( type, pSize->value(), pLine->value(), cl);

    return dat;
}

void SymbolDialog::CmSelectColor()
{
	QPalette pl = pColor->palette();
	QColor cl = QColorDialog::getColor(pl.color(QPalette::Background), this);

    if( cl.isValid() )
    {    pl.setColor( QPalette::Background, cl);	
        pColor->setPalette(pl);
    }   
}

//--------------------- End of SymbolDialog.cpp ---------------------------

