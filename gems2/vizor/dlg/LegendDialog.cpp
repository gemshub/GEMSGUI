//-------------------------------------------------------------------
// $Id$
//
// Implementation of LegendDialog, PlotTypeBtn classes
//
// Copyright (C) 1996-2001  A.Rysin, S.Dmytriyeva
// Uses  gstring class (C) A.Rysin 1999
//
// This file is part of the GEM-Vizor library which uses the
// Qt v.2.x GUI Toolkit (Troll Tech AS, http://www.trolltech.com)
// according to the Qt Duo Commercial license
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL)
//
// See http://les.web.psi.ch/Software/GEMS-PSI/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------
#include <stdio.h>

#include <qpixmap.h>
#include <qpainter.h>
#include <qbrush.h>
#include <qkeycode.h>
#include <qcolordialog.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qbuttongroup.h>
#include <qvalidator.h>
#include <qfontdialog.h>

#include "service.h"
#include "LegendDialog.h"
#include "dlg/ColorDialog.h"
#include "dlg/GraphDialog.h"
#include "visor_w.h"


//----------------------------------------------------------
// Plot Legend windows

LegendDialog::LegendDialog( GraphDialog * aGraph ):
        LegendDialogData(aGraph, 0, true),
        graph(&(aGraph->gr_data)), graph_dlg(aGraph)
{

    gstring cap = "Customize Graph: ";
    cap += graph->title;

    labelFont = pVisorImp->getAxisLabelFont();
    pLabelFont->setText(labelFont.toString());

    setCaption( cap.c_str() );
    pAxis->setValue( graph->axisType );

    oldGraphType = graph->graphType;
    comboBox1->setCurrentItem( graph->graphType );
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

    B_2 = new QButtonGroup( this, "ButtonGroup_1" );
    B_2->setGeometry( 10, 270, 380, 0 );
    B_2->setTitle( "Plots list" );
    connect( B_2, SIGNAL(clicked(int)), SLOT(SetLine(int)) );

    // Insert Lines : labels in legend box
    if( graph->graphType != ISOLINES )
      ShowLines();
    else
      ShowIsoline();

// offset itself to not close the hole plot
    QPoint center( parentWidget()->pos() );
    center.setX( center.x() + parentWidget()->width() / 2 );
    center.setY( center.y() + parentWidget()->height() / 2 );
    move( mapToGlobal( center ) );

    resize(width(), B_2->y() + B_2->height() + 10);
    update();

    backgroundColor = graph_dlg->getBackgrColor();
}


LegendDialog::~LegendDialog()
{}

/*
void LegendDialog::closeEvent( QCloseEvent* e)
{
    QDialog::closeEvent(e);
}
*/

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
      // delete old structure
        pScale.Clear();
        pLines.Clear();
        pNames.Clear();
        pLabels.Clear();
      // Insert labels in legend box
      if( oldGraphType/*graph->graphType*/ != ISOLINES )
        ShowLines( true );
      else
        ShowIsoline( true );
      update();  
  }
    catch( TError& xcpt )
    {
        vfMessage(this, xcpt.title, xcpt.mess);
        comboBox1->setCurrentItem( oldGraphType );

    }
}

void  LegendDialog::ShowLines( bool new_ )
{
    QLineEdit*    pName1;
    int y=290;
    for( uint ii=0, kk=0; ii<graph->plots.GetCount(); ii++, y+=10 )
    {

        QLabel* qtarch_Label_7;
        qtarch_Label_7 = new QLabel( this );
        qtarch_Label_7->setGeometry( 30, y, 140, 20 );
        qtarch_Label_7->setText( graph->plots[ii].getNames().c_str() );
        pLabels.Add( qtarch_Label_7 );

        if( new_ )
             qtarch_Label_7->show();

        for( int jj=0; jj<graph->plots[ii].getLinesNumber(); kk++, jj++, y+=20 )
        {
            pLines.Add(  new PlotTypeBtn( graph->lines[kk] , this ));
            pLines[kk].setGeometry( 130, y, 40, 20 );
            //connect( pLines[kk], SIGNAL(clicked()), SLOT(SetLine()) );
            B_2->insert(&pLines[kk]);
            if( new_ )
             pLines[kk].show();

            pName1 = new QLineEdit( this );
            pName1->setGeometry( 190, y, 150, 20 );
            pName1->setText( graph->lines[kk].name );
            pName1->setMaxLength( 15 );
            if( new_ )
             pName1->show();

            pNames.Add(pName1);
        }
    }
    B_2->resize(380, y-220);
}


void  LegendDialog::ShowIsoline( bool new_ )
{
    QLineEdit*    pName1;
    int x = 30, y=290;

    if( graph->scale.GetCount() < 1 )
      graph->setColorList();

    // show scale of Isolines
    for( uint ii=0; ii<graph->scale.GetCount(); ii++, y+=20 )
    {
       TPlotLine pl( "Scale",  P_FILLSQUARE, 0, 0, graph->scale[ii].red,
                     graph->scale[ii].green, graph->scale[ii].blue );
       pScale.Add(  new PlotTypeBtn( pl , this ));
       //QPushButton *btn = new  QPushButton( this );
       //btn->setBackgroundColor(graph_dlg->getColorIsoline(ii));
       //btn->setPaletteBackgroundColor(graph_dlg->getColorIsoline(ii));
       pScale[ii].setGeometry( x, y, 20, 20 );
       //pScale.Add( btn );
       B_2->insert(&pScale[ii]);
       if( new_ )
          pScale[ii].show();

       QLabel* qtarch_Label_7 = new QLabel( this );
       qtarch_Label_7->setGeometry( x+20, y, 100, 20 );
       qtarch_Label_7->setText( graph_dlg->getTextIsoline(ii).c_str() );
       pLabels.Add( qtarch_Label_7 );
       if( new_ )
          qtarch_Label_7->show();


    }

    // show symbols
    int y1 = 290;
    x = 160;
    for( uint kk=0; kk<graph->lines.GetCount(); kk++, y1+=20 )
    {
      pLines.Add(  new PlotTypeBtn( graph->lines[kk] , this ));
      pLines[kk].setGeometry( x, y1, 40, 20 );
      B_2->insert(&pLines[kk]);
      if( new_ )
          pLines[kk].show();

      pName1 = new QLineEdit( this );
      pName1->setGeometry( x+50, y1, 150, 20 );
      pName1->setText( graph->lines[kk].name );
      pName1->setMaxLength( 15 );
      pNames.Add(pName1);
      if( new_ )
          pName1->show();
    }
    B_2->resize(380, max(y1,y)-220);
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

    if ( x0 >= xn || y0 >= yn
	    || fx0 >= fxn || fy0 >= fyn ) {
	vfMessage( this, "Plot parameters",
		"Incorrect intervals, please check!", vfErr);
	return 1;
    }

    graph->axisType = pAxis->value();
    graph->graphType = comboBox1->currentItem();
    graph->xName = gstring( pXname->text() );
    graph->yName = gstring( pYname->text() );

    graph->region[0] = x0;
    graph->region[1] = xn;
    graph->region[2] = y0;
    graph->region[3] = yn;

    graph->part[0] = fx0;
    graph->part[1] = fxn;
    graph->part[2] = fy0;
    graph->part[3] = fyn;

    // save scale
    for(uint ii=0; ii<pScale.GetCount(); ii++ )
    {
       QColor cl = pScale[ii].getColor();
       graph->scale[ii].red = cl.red();
       graph->scale[ii].green = cl.green();
       graph->scale[ii].blue = cl.blue();
    }

    for(uint ii=0; ii<pLines.GetCount(); ii++ )
    {
        pLines[ii].setName( pNames[ii].text() );
        graph->lines[ii] = pLines[ii].getData();
    }

    graph_dlg->setBackgrColor( backgroundColor );

    graph_dlg->Apply();

    return 0;
}


void LegendDialog::SetLine(int ii)
{
  uint nBut = ii;

  if( oldGraphType == ISOLINES )
  {
     if( nBut < pScale.GetCount() )
     {
      // select isoline color
       QColor backColor = QColorDialog::getColor(
                                 pScale[nBut].getColor(), this );
       if( backColor.isValid() )
          pScale[nBut].setColor( backColor );
       return;
     }
     else nBut -= pScale.GetCount();
   }

  PlotTypeBtn* b = &pLines[nBut];//((PlotTypeBtn*)focusProxy());

  b->setName( pNames[nBut].text() );
  ColorDialog cd( b->getData(), this);//, "Select color");
  if( cd.exec() )
        b->setData(  cd.GetPlotLine()) ;
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


//--------------------- End of LegendDialog.cpp ---------------------------

