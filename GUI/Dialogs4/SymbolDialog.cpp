//-------------------------------------------------------------------
// $Id: ColorDialog.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of SymbolDialog class
//
// Copyright (C) 1996-2008  A.Rysin, S.Dmytriyeva
// Uses  gstring class (C) A.Rysin 1999
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

#include <qcolordialog.h>
#include <qframe.h>
#include <qradiobutton.h>
#include <qspinbox.h>
#include <qvariant.h>

#include "SymbolDialog.h"
#include "GraphDialog.h"

SymbolDialog::SymbolDialog( TPlotLine& aData, QWidget* parent) :
        QDialog( parent ), dat(aData)
{
    setupUi(this);

    // make symbol icons by program
    QIcon icon;
    TPlotLine plot_( "Scale",  P_POINT, 0, 0, 0, 0, 127 );
    paintIcon( icon, plot_ );
    p0->setIcon(icon);
    plot_.type = P_CROSSDIAG;
    paintIcon( icon, plot_ );
    p1->setIcon(icon);
    plot_.type = P_SQUARE;
    paintIcon( icon, plot_ );
    p2->setIcon(icon);
    plot_.type = P_FILLSQUARE;
    paintIcon( icon, plot_ );
    p3->setIcon(icon);
    plot_.type = P_RHOMBUS;
    paintIcon( icon, plot_ );
    p4->setIcon(icon);
    plot_.type = P_WYE;
    paintIcon( icon, plot_ );
    p5->setIcon(icon);
    plot_.type = P_STAR;
    paintIcon( icon, plot_ );
    p6->setIcon(icon);
    plot_.type = P_CIRCLE;
    paintIcon( icon, plot_ );
    p7->setIcon(icon);
    plot_.type = P_FILLRHOMBUS;
    paintIcon( icon, plot_ );
    p8->setIcon(icon);
    plot_.type = P_FILLCIRCLE;
    paintIcon( icon, plot_ );
    p9->setIcon(icon);


    pColor->setAutoFillBackground( true); 
    QPalette pl = pColor->palette();
    pl.setColor( QPalette::Window, QColor (dat.red, dat.green, dat.blue));	
    pColor->setPalette(pl);
    QObject::connect(PushButton12, SIGNAL(clicked()), this, SLOT(CmSelectColor()));
    pLine->setValue( dat.line_size );
    pSize->setValue( dat.size );
    switch( dat.type)
    {
    case 0:
        p0->setChecked(true);
        break;
    case 1:
        p1->setChecked(true);
        break;
    case 2:
        p2->setChecked(true);
        break;
    case 3:
        p3->setChecked(true);
        break;
    case 4:
        p4->setChecked(true);
        break;
    case 5:
        p5->setChecked(true);
        break;
    case 6:
        p6->setChecked(true);
        break;
    case 7:
        p7->setChecked(true);
        break;
    case 8:
        p8->setChecked(true);
        break;
    case 9:
        p9->setChecked(true);
        break;
    }
    gstring s= "Set up plot line: ";
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

    //  if(p0->isChecked() )
    dat.type = 0;
    if(p1->isChecked() )
        dat.type = 1;
    if(p2->isChecked() )
        dat.type = 2;
    if(p3->isChecked() )
        dat.type = 3;
    if(p4->isChecked() )
        dat.type = 4;
    if(p5->isChecked() )
        dat.type = 5;
    if(p6->isChecked() )
        dat.type = 6;
    if(p7->isChecked() )
        dat.type = 7;

    if(p8->isChecked() )
        dat.type = 8;
    if(p9->isChecked() )
        dat.type = 9;

    dat.line_size  = pLine->value();
    dat.size  = pSize->value();
    QColor cl = pColor->palette().color(QPalette::Window); //backgroundColor();
    dat.red =   cl.red();
    dat.green = cl.green();
    dat.blue  = cl.blue();

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

