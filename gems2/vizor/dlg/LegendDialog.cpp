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
#include <qlabel.h>
#include <qpushbutton.h>
#include <qbuttongroup.h>
#include <qvalidator.h>

#include "service.h"
#include "LegendDialog.h"
#include "dlg/ColorDialog.h"
#include "dlg/GraphDialog.h"


void PlotTypeBtn::drawButtonLabel(QPainter * paint)
{

    QPoint p(18,9);
    QColor color( plLine.red, plLine.green, plLine.blue);


    int size = 4;//plLine.size;

    /*
      paint->setBrush( QBrush::NoBrush );
    */
    paint->setPen( color );
    switch(plLine.type)
    {
    case	 P_POINT:        // point
        size = 2;
        paint->drawEllipse( QRect(p-QPoint(size,size),p+QPoint(size+1,size+1)) );
        break;
    case	 P_CROSSDIAG:    // X
        paint->drawLine( p.x()-size, p.y()+size, p.x()+size, p.y()-size);
        paint->drawLine( p.x()-size, p.y()-size, p.x()+size, p.y()+size);
        break;
    case	 P_SQUARE:       // square
        paint->drawRect(QRect(p-QPoint(size,size),p+QPoint(size,size)));
        break;
    case	 P_FILLSQUARE:   // fill square
        paint->drawRect(QRect(p-QPoint(size,size),p+QPoint(size,size)));
        paint->fillRect(QRect(p-QPoint(size,size),p+QPoint(size,size)),
                        QBrush(color));
        break;
    case  P_RHOMBUS:      // rhombus
        size++;
        paint->drawLine( p.x()-size, p.y(), p.x(), p.y()-size);
        paint->drawLine( p.x(), p.y()-size, p.x()+size, p.y());
        paint->drawLine( p.x()+size, p.y(), p.x(), p.y()+size);
        paint->drawLine( p.x(), p.y()+size, p.x()-size, p.y());
        break;
    case  P_WYE:          // Y
        paint->drawLine( p.x()-size, p.y()-size, p.x(), p.y());
        paint->drawLine( p.x()+size, p.y()-size, p.x(), p.y());
        paint->drawLine( p.x(), p.y(), p.x(), p.y()+size);
        break;
    case	 P_STAR:         // star
        {
            paint->drawLine( p.x()-size, p.y(), p.x()+size, p.y());
            paint->drawLine( p.x(), p.y()-size, p.x(), p.y()+size);
            int scale = (int)((float)size*0.7+0.5);
            paint->drawLine( p.x()-scale, p.y()-scale, p.x()+scale, p.y()+scale);
            paint->drawLine( p.x()-scale, p.y()+scale, p.x()+scale, p.y()-scale);
            break;
        }
    case	 P_CIRCLE:        // circle
        paint->drawEllipse( QRect(p-QPoint(size,size),p+QPoint(size,size)) );
        break;
    case	 P_FILLCIRCLE:        // fill circle
        paint->drawEllipse( QRect(p-QPoint(size,size),p+QPoint(size+1,size+1)) );
        paint->setBrush( color );
        paint->drawEllipse( QRect(p-QPoint(size,size),p+QPoint(size+1,size+1)) );
        break;
    case  P_FILLRHOMBUS:      // fill rhombus
        size++;
        QPointArray arr(4);
        paint->setBrush( color );
        arr.setPoint( 0, p.x()-size, p.y());
        arr.setPoint( 1, p.x(), p.y()-size);
        arr.setPoint( 2, p.x()+size, p.y());
        arr.setPoint( 3, p.x(), p.y()+size);
        paint->drawPolygon( arr, true);
        break;
    }
    if(plLine.line_size > 0 )
    {
        paint->drawLine( 0, 9, 14, 9);
        paint->drawLine( 22, 9, 36, 9);
    }

    QPushButton::drawButtonLabel(paint);

}

void PlotTypeBtn::setData( TPlotLine& ln)
{
    plLine = ln;
    update();
}

void PlotTypeBtn::setName( const char* name)
{
    strncpy(plLine.name, name, 15 );
}


//----------------------------------------------------------
// Plot Legend windows

LegendDialog::LegendDialog( GraphDialog * aGraph ):
        LegendDialogData(aGraph, 0, true),
        graph(&(aGraph->gr_data)), graph_dlg(aGraph)
{

    gstring cap = "Customize Graph: ";
    cap += graph->title;

    setCaption( cap.c_str() );
    pAxis->setValue( graph->axisType );

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

    QButtonGroup* B_2;
    B_2 = new QButtonGroup( this, "ButtonGroup_1" );
    B_2->setGeometry( 10, 230, 380, 0 );
    B_2->setTitle( "Plots list" );
    connect( B_2, SIGNAL(clicked(int)), SLOT(SetLine(int)) );

    // Insert Lines
    QLineEdit*    pName1;

    int y=250;
    for( uint ii=0, kk=0; ii<graph->plots.GetCount(); ii++, y+=10 )
    {

        QLabel* qtarch_Label_7;
        qtarch_Label_7 = new QLabel( this, "Label_7" );
        qtarch_Label_7->setGeometry( 30, y, 140, 20 );
        qtarch_Label_7->setText( graph->plots[ii].getNames().c_str() );

        for( int jj=0; jj<graph->plots[ii].getLinesNumber(); kk++, jj++, y+=20 )
        {
            pLines.Add(  new PlotTypeBtn( graph->lines[kk] , this ));
            pLines[kk].setGeometry( 130, y, 40, 20 );
            //connect( pLines[kk], SIGNAL(clicked()), SLOT(SetLine()) );
            B_2->insert(&pLines[kk]);

            pName1 = new QLineEdit( this );
            pName1->setGeometry( 190, y, 150, 20 );
            pName1->setText( graph->lines[kk].name );
            pName1->setMaxLength( 15 );

            pNames.Add(pName1);
        }
    }
    B_2->resize(380, y-220);

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
    PlotTypeBtn* b = &pLines[ii];//((PlotTypeBtn*)focusProxy());

    b->setName( pNames[ii].text() );
    ColorDialog cd( b->getData(), this);//, "Select color");
    if( cd.exec() )
        b->setData(  cd.GetPlotLine()) ;
}
//--------------------- End of LegendDialog.cpp ---------------------------

