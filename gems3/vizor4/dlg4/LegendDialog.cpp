//-------------------------------------------------------------------
// $Id: LegendDialog.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of LegendDialog, PlotTypeBtn classes
//
// Copyright (C) 1996-2008  A.Rysin, S.Dmytriyeva
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
#include <stdio.h>

#include <qcolordialog.h>
#include <qfontdialog.h>

#include "service.h"
#include "LegendDialog.h"
#include "ColorDialog.h"
#include "GraphDialog.h"
#include "visor_w.h"


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

   /// Qt 4 
    B_2 = new QButtonGroup( plotsLst );
    connect( B_2, SIGNAL(buttonClicked(int)), this, SLOT(SetLine(int)) );
    
    plotLayout = new QGridLayout( plotsLst );
    plotLayout->setHorizontalSpacing( 10 );
    plotLayout->setVerticalSpacing(0);
    plotLayout->setMargin(11);
    
    // Insert Lines : labels in legend box
    if( graph->graphType != ISOLINES )
      ShowLines();
    else
      ShowIsoline();

    adjustSize();
  //  update();
  /// end Qt4
    
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

       // delete old structure 
        pScale.Clear();
        pLines.Clear();
        pNames.Clear();
       
        // qt3t04 must be tested allocation ????
        delete plotsLst;
        plotsLst = new QGroupBox( this );
        plotsLst->setObjectName(QString::fromUtf8("plotsLst"));
        plotsLst->setTitle(QApplication::translate("LegendDialogData", "Plots List ", 0, QApplication::UnicodeUTF8));

        gridLayout->addWidget(plotsLst, 2, 0, 1, 1);

        B_2 = new QButtonGroup( plotsLst );
        connect( B_2, SIGNAL(buttonClicked(int)), this, SLOT(SetLine(int)) );
        
        plotLayout = new QGridLayout( plotsLst );
        plotLayout->setSpacing(6);
        plotLayout->setMargin(11);
        
       // Insert labels in legend box
      if( oldGraphType/*graph->graphType*/ != ISOLINES )
        ShowLines();
      else
        ShowIsoline();

      adjustSize();
      update();  
  }
    catch( TError& xcpt )
    {
        vfMessage(this, xcpt.title, xcpt.mess);
        comboBox1->setCurrentIndex( oldGraphType );

    }
}

void  LegendDialog::ShowLines()
{
    QLineEdit    *pName;
    QLabel       *label;
    PlotTypeBtn  *pButton;
    QFrame       *line;
    uint ll=0;
    
    for( uint ii=0, kk=0; ii<graph->plots.GetCount();  ii++ )
    {
        label = new QLabel;
        label->setText( graph->plots[ii].getNames().c_str() );
        plotLayout->addWidget(label, ll, 0, 1, 1);

        for( int jj=0; jj<graph->plots[ii].getLinesNumber(); ll++, kk++, jj++ )
        {
        	pButton = new  PlotTypeBtn( graph->lines[kk] );
        	plotLayout->addWidget(pButton, ll, 1, 1, 1);
            pLines.Add(  pButton );
            B_2->addButton(pButton, kk );

            pName = new QLineEdit;
            pName->setText( graph->lines[kk].name );
            pName->setMaxLength( 15 );
        	plotLayout->addWidget(pName, ll, 2, 1, 1);
            pNames.Add( pName );
        }

        line = new QFrame;
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        plotLayout->addWidget(line, ll++, 0, 1, 3);
    }
}


void  LegendDialog::ShowIsoline()
{
    QLineEdit*    pName;
    QLabel*       label;
    PlotTypeBtn*  pButton;

    if( graph->scale.GetCount() < 1 )
      graph->setColorList();

    // show scale of Isolines
    for( uint ii=0; ii<graph->scale.GetCount(); ii++ )
    {
       TPlotLine pl( "Scale",  P_FILLSQUARE, 0, 0, graph->scale[ii].red,
                     graph->scale[ii].green, graph->scale[ii].blue );
   	   
       pButton = new  PlotTypeBtn( pl , plotsLst );
   	   plotLayout->addWidget(pButton, ii, 0, 1, 1);
   	   pScale.Add(  pButton );
       B_2->addButton( pButton, ii );
       
       label = new QLabel( plotsLst );
       label->setText( graph_dlg->getTextIsoline(ii).c_str()  );
   	   plotLayout->addWidget(label, ii, 1, 1, 1);
    }

    // show symbols
    for( uint kk=0; kk<graph->lines.GetCount(); kk++ )
    {
    	pButton = new  PlotTypeBtn( graph->lines[kk] , plotsLst );
    	plotLayout->addWidget(pButton, kk, 2, 1, 1);
        pLines.Add(  pButton );
        B_2->addButton(pButton, kk + graph->scale.GetCount() );

        pName = new QLineEdit( plotsLst );
        pName->setText( graph->lines[kk].name );
        pName->setMaxLength( 15 );
    	plotLayout->addWidget(pName, kk, 3, 1, 1);
        pNames.Add( pName );
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

    if ( x0 >= xn || y0 >= yn
	    || fx0 >= fxn || fy0 >= fyn ) {
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

    // save scale
    for(uint ii=0; ii<pScale.GetCount(); ii++ )
    {
       QColor cl = pScale[ii]->getColor();
       graph->scale[ii].red = cl.red();
       graph->scale[ii].green = cl.green();
       graph->scale[ii].blue = cl.blue();
    }

    for(uint ii=0; ii<pLines.GetCount(); ii++ )
    {
        pLines[ii]->setName( pNames[ii]->text().toLatin1().data() );
        graph->lines[ii] = pLines[ii]->getData();
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
                                 pScale[nBut]->getColor(), this );
       if( backColor.isValid() )
          pScale[nBut]->setColor( backColor );
       return;
     }
     else nBut -= pScale.GetCount();
   }

  PlotTypeBtn* b = pLines[nBut];//((PlotTypeBtn*)focusProxy());

  b->setName( pNames[nBut]->text().toLatin1().data() );
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

