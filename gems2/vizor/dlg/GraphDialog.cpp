//-------------------------------------------------------------------
// $Id$
//
// Implementation of GraphDialog class (Plotting system)
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

#include <qframe.h>
#include <qpainter.h>
#include <qprinter.h>
#include <qapplication.h>
#include <qgroupbox.h>
#include <qbuttongroup.h>
#include <qlabel.h>
#include <qpaintdevicemetrics.h>
#include <qsimplerichtext.h>
#include <qdragobject.h>

#include "GraphDialog.h"
#include "LegendDialog.h"
#include "visor_w.h"
#include "v_object.h"

/*! Empric function to select new color for every new plotting line
*/
inline
QColor plotColor(int n)
{
    return QColor( (n*30)%256, (128+(n*33))%256, (256-(n*37))%256);
}

/*! Label of the legend frame, that allows to drag the name to the plotting area
*/

class DragLabel:
    public QLabel
{
    public:
	DragLabel(QWidget* parent):
	    QLabel(parent) {
	    }

    protected:
	void mousePressEvent( QMouseEvent * /*e*/ ) {
	    QTextDrag *drag = new QTextDrag( text(), this );
	    //drag->setPixmap(QPixmap(picture_xpm),QPoint(8,8));
	    drag->dragCopy();
	}
};


/*!
   The constructor
*/
GraphDialog::GraphDialog(TCModule *pmodule, GraphData& data):
        GraphDialogData(pmodule->window(), NULL, false /* true =modal */ ),
        pModule(pmodule),  isFragment(false), gr_data(data)
{

    minX = gr_data.region[0];
    maxX = gr_data.region[1];
    minY = gr_data.region[2];
    maxY = gr_data.region[3];

    if( maxX == minX ) maxX += 1.;
    if( maxY == minY ) maxY += 1.;

    resize(490, 310);
    QRect win(10, 5, width()-100, height()-20);

    plot = new TPlotWin(plotFrame, win,
		    FPoint(minX, minY),
                    FPoint(maxX, maxY),
			gr_data.title);
			
    plot->setGridCount(gr_data.axisType);

    Show();

//    setCaption( gr_data.title.c_str() );
}

GraphDialog::~GraphDialog()
{}


// show all graphs

void GraphDialog::Show()
{
    if( gr_data.isBackgr_color )
      plot->setBackgroundColor( QColor( gr_data.b_color[0],
    					gr_data.b_color[1], 
					gr_data.b_color[2]) );
    else
      plot->setBackgroundColor( backgroundColor().dark(110) );

    ShowPlots();

    // Insert Lines for legend box

    int y=30; //420;
    for( uint ii=0, kk=0; ii<gr_data.plots.GetCount(); ii++, y+=10 )
    {
        for( int jj=0; jj<gr_data.plots[ii].getLinesNumber(); kk++, jj++, y+=25 )
        {
            PlotTypeBtn* pBtn = new PlotTypeBtn( gr_data.lines[kk], pGrpLegend ); //this );
	    aLegendButtons.Add( pBtn );
            pBtn->setFlat(true);
	    pBtn->setGeometry( 3, y, 32, 15 );

            QLabel* pLabel = new DragLabel( pGrpLegend );
	    aLegendLabels.Add( pLabel );
            pLabel->setGeometry( 3+35, y, 65, 20 );
            pLabel->setText( gr_data.lines[kk].name );
        }
    }
    update();

    plot->setPlotBounds( FPoint(minX, minY),
                   FPoint(maxX, maxY));
}

// show plots
void GraphDialog::ShowPlots()
{
    for(uint ii=0, nLn =0; ii<gr_data.plots.GetCount(); ii++)
        for( int jj=0; jj<gr_data.plots[ii].getLinesNumber(); jj++, nLn++ )
        {
            TIArray<FPoint> pnts;
            gr_data.plots[ii].getPointLine( jj, pnts );
            for(uint i=0; i<pnts.GetCount(); i++)
            { //check location into min-max region
                /*      if( pnts[i].x >= minX && pnts[i].x<= maxX &&
                          pnts[i].y >= minY && pnts[i].y<= maxY )
                */      plot->Add(new PPoint(plot, pnts[i],
                    gr_data.getType(nLn), gr_data.getSize(nLn),
                    getColor(nLn)));
                if( gr_data.getLineSize(nLn)>0 && i != 0)
                { // insert line
                    plot->Add(new PLine(plot, pnts[i-1], pnts[i],
                    getColor(nLn), gr_data.getLineSize(nLn) ));
                }
            }
        }
}

void
GraphDialog::resizeEvent(QResizeEvent* qpev)
{
//    pGroupBox->move(10, height() - pGroupBox->height() - 10);
//    pGrpLegend->move(width()-115, 10);

//    plot->resize(width()-130, height()-20);
    update();
}

void GraphDialog::ShowNew()
{
    plot->Clear();
    plot->init();
    Show();
    plot->update();
    qApp->processEvents();
}

void
GraphDialog::CmLegend()
{
    LegendDialog legend( this );
    legend.exec();
}


void
GraphDialog::Apply()
{
    if( !pModule->SaveGraphData( &gr_data ) )
       return;

    plot->setGridCount(gr_data.axisType);

    if(  isFragment == true )
    {
        minX = gr_data.part[0];
        maxX = gr_data.part[1];
        minY = gr_data.part[2];
        maxY = gr_data.part[3];
    }
    else
    {
        minX = gr_data.region[0];
        maxX = gr_data.region[1];
        minY = gr_data.region[2];
        maxY = gr_data.region[3];
    }

    if( maxX == minX ) maxX += 1.;
    if( maxY == minY ) maxY += 1.;

    int y=30; //420;
    for( uint ii=0, kk=0; ii<gr_data.plots.GetCount(); ii++, y+=10 )
    {
        for( int jj=0; jj<gr_data.plots[ii].getLinesNumber(); kk++, jj++, y+=25 )
        {
	    aLegendButtons[kk].setData( gr_data.lines[kk] );
	    aLegendLabels[kk].setText( gr_data.lines[kk].name );
        }
    }


    plot->Clear();
    plot->init();
    Show();
    plot->update();
    pGrpLegend->update();
}


void
GraphDialog::AddPoint( int nPlot, int nPoint )
{
    int nLn = gr_data.plots[nPlot].getFirstLine();

    for( int jj=0; jj<gr_data.plots[nPlot].getLinesNumber(); jj++, nLn++ )
    {
        FPoint pnt = gr_data.plots[nPlot].getPoint( jj, nPoint );
        plot->Add(new PPoint(plot, pnt, gr_data.getType(nLn),
                       gr_data.getSize(nLn), getColor(nLn)));
    }

    plot->update();
    qApp->processEvents();
}

void
GraphDialog::CmFragment()
{

    if(  isFragment == false )
    {
        minX = gr_data.part[0];
        maxX = gr_data.part[1];
        minY = gr_data.part[2];
        maxY = gr_data.part[3];
        isFragment = true;
        pFragmentBtn->setText("&Full");
    }
    else
    {
        minX = gr_data.region[0];
        maxX = gr_data.region[1];
        minY = gr_data.region[2];
        maxY = gr_data.region[3];
        isFragment = false;
        pFragmentBtn->setText("&Fragment");
    }

    if( maxX == minX ) maxX += 1.;
    if( maxY == minY ) maxY += 1.;

    plot->setPlotBounds( FPoint(minX, minY),
                   FPoint(maxX, maxY));
    plot->update();
    //date();
}


#ifndef __unix
#if QT_VERSION < 3
extern void qt_set_sane_printing( bool b );
#endif
#endif

//Print graph screen to printer device
void
GraphDialog::CmPrint()
{
    QPrinter prt;

    if( prt.setup() )
    {
        QPainter p(&prt);

#ifndef __unix
#if QT_VERSION < 3
    qt_set_sane_printing( true );
#endif
#endif

        QPaintDeviceMetrics metrics(p.device());
//        int dpix = metrics.logicalDpiX();
//        int dpiy = metrics.logicalDpiY();
//        const int margin = 10;//72; // pt

//        QRect body(margin*dpix/72, margin*dpiy/72,
//        metrics.width()-margin*dpix/72*2,
//        metrics.height()-margin*dpiy/72*2 );

        QColor c;
        c.setHsv( 0, 0, 0) ;//(1 * 255)/2, 255/2, 255 );// rainbow effect
        p.setPen(c);
	p.setFont(QFont("Times", 10));

// fake - not used
//	QRect win( body.x()+40, body.height()+20, body.width()-130, body.height()-50);

//	p.scale(metrics.width()/double(plot->width()), metrics.height()/double(plot->height()));
	p.translate(40, 50);
	p.scale(metrics.logicalDpiX()/72., metrics.logicalDpiY()/72.); // seems to be always = 1
/*        p.drawText( 5, plot->height() + 20, px0->text() );
        p.drawText( plot->width() - 30, plot->height() + 20, px1->text() );
        p.drawText( -20, plot->height() - 15, py0->text() );
        p.drawText( -20, 5, py1->text() );
*/
	QRect win;
	plot->PaintToDC(p, win);

    }
}

void
GraphDialog::setBackgrColor( QColor color )
{
    // Sveta 19/08/01 Save background
    gr_data.b_color[0] =   color.red();
    gr_data.b_color[1] = color.green();
    gr_data.b_color[2]  = color.blue();
    gr_data.isBackgr_color = true;

    plot->setBackgroundColor( color );
    update();
    plot->update();
}

QColor
GraphDialog::getBackgrColor()
{
    return  plot->backgroundColor();
}

// ----------------------------
//--------- PlotTypeBtn -------

void PlotTypeBtn::drawButtonLabel(QPainter * paint)
{
    QColor color( plLine.red, plLine.green, plLine.blue);

    QRect rect(3, 3, width()-6, height()-6);
    QPoint center = rect.center();

    int size = 4;//plLine.size;

    //paint->setBrush( QBrush::NoBrush );
    paint->setPen( color );
    switch(plLine.type)
    {
    case	 P_POINT:        // point
        size = 2;
        paint->drawEllipse( QRect(center-QPoint(size,size), center+QPoint(size+1,size+1)) );
        break;
    case	 P_CROSSDIAG:    // X
        paint->drawLine( center.x()-size, center.y()+size, center.x()+size, center.y()-size);
        paint->drawLine( center.x()-size, center.y()-size, center.x()+size, center.y()+size);
        break;
    case	 P_SQUARE:       // square
        paint->drawRect(QRect(center-QPoint(size,size), center+QPoint(size,size)));
        break;
    case	 P_FILLSQUARE:   // fill square
        paint->drawRect(QRect(center-QPoint(size,size), center+QPoint(size,size)));
        paint->fillRect(QRect(center-QPoint(size,size), center+QPoint(size,size)), QBrush(color));
        break;
    case  P_RHOMBUS:      // rhombus
        size++;
        paint->drawLine( center.x()-size, center.y(), center.x(), center.y()-size);
        paint->drawLine( center.x(), center.y()-size, center.x()+size, center.y());
        paint->drawLine( center.x()+size, center.y(), center.x(), center.y()+size);
        paint->drawLine( center.x(), center.y()+size, center.x()-size, center.y());
        break;
    case  P_WYE:          // Y
        paint->drawLine( center.x()-size, center.y()-size, center.x(), center.y());
        paint->drawLine( center.x()+size, center.y()-size, center.x(), center.y());
        paint->drawLine( center.x(), center.y(), center.x(), center.y()+size);
        break;
    case	 P_STAR:         // star
        {
            paint->drawLine( center.x()-size, center.y(), center.x()+size, center.y());
            paint->drawLine( center.x(), center.y()-size, center.x(), center.y()+size);
            int scale = (int)ROUND((float)size*0.7+0.5);
            paint->drawLine( center.x()-scale, center.y()-scale, center.x()+scale, center.y()+scale);
            paint->drawLine( center.x()-scale, center.y()+scale, center.x()+scale, center.y()-scale);
            break;
        }
    case	 P_CIRCLE:        // circle
        paint->drawEllipse( QRect(center-QPoint(size,size), center+QPoint(size,size)) );
        break;
    case	 P_FILLCIRCLE:        // fill circle
        paint->drawEllipse( QRect(center-QPoint(size,size), center+QPoint(size+1,size+1)) );
        paint->setBrush( color );
        paint->drawEllipse( QRect(center-QPoint(size,size), center+QPoint(size+1,size+1)) );
        break;
    case  P_FILLRHOMBUS:      // fill rhombus
        size++;
        QPointArray arr(4);
        paint->setBrush( color );
        arr.setPoint( 0, center.x()-size, center.y());
        arr.setPoint( 1, center.x(), center.y()-size);
        arr.setPoint( 2, center.x()+size, center.y());
        arr.setPoint( 3, center.x(), center.y()+size);
        paint->drawPolygon(arr, true);
        break;
    }

    // line, not crossing the point mark
    if(plLine.line_size > 0 )
    {
        paint->drawLine( rect.x(), center.y(), center.x()-size, center.y());
        paint->drawLine( center.x()+size, center.y(), rect.width(), center.y());
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



//--------------------- End of GraphDialog.cpp ---------------------------

