//-------------------------------------------------------------------
// $Id: LegendDialog.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of LegendDialog classes
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
#include <cstdio>
#include <qcolordialog.h>
#include <qfontdialog.h>

#include "service.h"
#include "LegendDialog.h"
#include "GemsMainWindow.h"


//----------------------------------------------------------
// Plot Legend windows

LegendDialog::LegendDialog( GraphDialog * aGraph ):
        QDialog(aGraph),
        graph(&(aGraph->gr_data)), graph_dlg(aGraph)
{

    setupUi(this);
    QObject::connect(pBtnChangeFont, SIGNAL(clicked()), this, SLOT(CmChangeLabelFont()));

    gstring cap = "Customize Graph: ";
    cap += graph->title;

    labelFont = pVisorImp->getAxisLabelFont();
    pLabelFont->setText(labelFont.toString());

    setWindowTitle( cap.c_str() );
    pAxis->setValue( graph->axisType );

    oldGraphType = graph->graphType;
    comboBox1->setCurrentIndex( graph->graphType );
    connect( comboBox1, SIGNAL(activated/*highlighted*/(int)), SLOT(CmChangeGraphType(int)) );

    pXname->setText( graph->xName.c_str() );
    pYname->setText( graph->yName.c_str() );

    QString str;
    pX0->setValidator( new QDoubleValidator( pX0 ) );
    pX0->setText( str.setNum (graph->region[0]) );

    pXn->setValidator( new QDoubleValidator( pXn ) );
    pXn->setText( str.setNum (graph->region[1]) );

    pY0->setValidator( new QDoubleValidator( pY0 ) );
    pY0->setText( str.setNum (graph->region[2]) );

    pYn->setValidator( new QDoubleValidator( pYn ) );
    pYn->setText( str.setNum (graph->region[3]) );

    pfX0->setValidator( new QDoubleValidator( pfX0 ) );
    pfX0->setText( str.setNum (graph->part[0]) );

    pfXn->setValidator( new QDoubleValidator( pfXn ) );
    pfXn->setText( str.setNum (graph->part[1]) );

    pfY0->setValidator( new QDoubleValidator( pfY0 ) );
    pfY0->setText( str.setNum (graph->part[2]) );

    pfYn->setValidator( new QDoubleValidator( pfYn ) );
    pfYn->setText( str.setNum (graph->part[3]) );

    connect(pHelp, SIGNAL(clicked()), this, SLOT(CmHelp()));
    
    backgroundColor = graph_dlg->getBackgrColor();
}


LegendDialog::~LegendDialog()
{}

void LegendDialog::languageChange()
{
    retranslateUi(this);
}


void  LegendDialog::CmChangeGraphType( int new_type )
{
  if( new_type == oldGraphType ||
      ( new_type < ISOLINES && oldGraphType < ISOLINES ) )
  {
    oldGraphType = new_type;
    return; //not need change legend
  }
  try
    {
       graph->goodIsolineStructure(new_type);
       oldGraphType = new_type;

    }
    catch( TError& xcpt )
    {
        vfMessage(this, xcpt.title, xcpt.mess);
        comboBox1->setCurrentIndex( oldGraphType );

    }
}


void LegendDialog::accept()
{
    if( apply() )
	return;

    QDialog::accept();
}

void LegendDialog::CmBackground()
{
    QColor backColor = QColorDialog::getColor( backgroundColor, this );
    if( backColor.isValid() )
        backgroundColor = backColor;
}


void LegendDialog::CmApply()
{
    apply();
}

int LegendDialog::apply()
{
    pVisorImp->setAxisLabelFont(labelFont);

    double x0 =  pX0->text().toDouble();
    double xn =  pXn->text().toDouble();
    double y0 =  pY0->text().toDouble();
    double yn =  pYn->text().toDouble();

    double fx0 =  pfX0->text().toDouble();
    double fxn =  pfXn->text().toDouble();
    double fy0 =  pfY0->text().toDouble();
    double fyn =  pfYn->text().toDouble();

    if ( x0 >= xn || y0 >= yn || fx0 >= fxn || fy0 >= fyn )
    {
	vfMessage( this, "Plot parameters",
		"Incorrect intervals, please check!", vfErr);
	return 1;
    }

    graph->axisType = pAxis->value();
    graph->graphType = comboBox1->currentIndex();
    graph->xName = gstring( pXname->text().toLatin1().data() );
    graph->yName = gstring( pYname->text().toLatin1().data() );

    graph->region[0] = x0;
    graph->region[1] = xn;
    graph->region[2] = y0;
    graph->region[3] = yn;

    graph->part[0] = fx0;
    graph->part[1] = fxn;
    graph->part[2] = fy0;
    graph->part[3] = fyn;

    graph_dlg->setBackgrColor( backgroundColor );

    graph_dlg->Apply();

    return 0;
}



void LegendDialog::CmChangeLabelFont()
{
    bool ok;
    QFont selected_font = QFontDialog::getFont(&ok, labelFont, this);

    if( ok )
    {
        labelFont = selected_font;
        pLabelFont->setText( labelFont.toString() );
    }
}

void LegendDialog::CmHelp()
{
    pVisorImp->OpenHelp( GEMS_GRAPH_CUS_HTML );
}

//--------------------- End of LegendDialog.cpp ---------------------------

