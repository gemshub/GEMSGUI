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
#include <qdragobject.h>
#include <qimage.h>

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

/*! Labels in the legend frame, that allows to drag the name to the plotting area
*/

class DragLabel:
    public QLabel
{
    public:
	DragLabel(QWidget* parent):
	    QLabel(parent) {
	    }
	virtual ~DragLabel() {}

    protected:
	void mousePressEvent( QMouseEvent *e ) {
	    QTextDrag *drag = new QTextDrag( text(), this );
	    drag->setPixmap(QPixmap::grabWidget(this), QPoint(0,height()-5));
	    drag->drag();
	}
};

/*! Symbols in the legend frame, that allows to drag the name to the plotting area
*/

class SymbolLabel:
    public QLabel
{
	int type;
	int lineSize;
	QColor color;

    protected:
/*	void mousePressEvent( QMouseEvent *e ) {
	    QTextDrag *drag = new QTextDrag( text(), this );
	    drag->setPixmap(QPixmap::grabWidget(this), QPoint(0,height()-5));
	    drag->drag();
	}
*/
	void drawContents(QPainter* dc);
	
    public:
	SymbolLabel(QWidget* parent, const TPlotLine& line):
	    QLabel(parent) {
		setData(line);
	    }
	virtual ~SymbolLabel() {}

	void setData(const TPlotLine& ln) {
	    color = QColor(ln.red, ln.green, ln.blue );
            type = ln.type;
	    lineSize = ln.line_size;
	    update();
	}
};

void 
SymbolLabel::drawContents(QPainter* dc)
{
    const int size = 4;
    QRect rect(3, 3, width()-6, height()-6);
    QPoint center = rect.center();
    PlotTypeBtn::drawSymbol(dc, center, type, size, color);

    if( lineSize > 0 )
    {
	dc->drawLine( rect.x(), center.y(), center.x()-size, center.y());
	dc->drawLine( center.x()+size, center.y(), rect.width(), center.y());
    }
}


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

    plot = new TPlotWin(plotFrame,
		    FPoint(minX, minY),
                    FPoint(maxX, maxY),
			gr_data.title.c_str());
			
    plot->setGridCount(gr_data.axisType);
    plot->setAxisTitles(gr_data.xName.c_str(), gr_data.yName.c_str());

    // Insert labels in legend box
    int y = 20;
    for( uint ii=0, kk=0; ii<gr_data.plots.GetCount(); ii++, y+=10 )
    {
        for( int jj=0; jj<gr_data.plots[ii].getLinesNumber(); kk++, jj++, y+=20 )
        {
            SymbolLabel* pLabel1 = new SymbolLabel( pGrpLegend, gr_data.lines[kk]);
	    aSymbolLabels.Add( pLabel1 );
	    pLabel1->setGeometry( 3, y, 32, 17 );

            DragLabel* pLabel = new DragLabel( pGrpLegend );
	    aLegendLabels.Add( pLabel );
            pLabel->setGeometry( 3 + 35, y, 85, 17 );
            pLabel->setText( gr_data.lines[kk].name );
        }
    }

    Show();

//    setCaption( gr_data.title.c_str() );
}

GraphDialog::~GraphDialog()
{}


/* if this is for adding points - we don't need Clear() and Show() - just add
*/
void GraphDialog::ShowNew()
{
    plot->Clear();
    Show();
    plot->update();
    qApp->processEvents();
}


// show all graphs

void GraphDialog::Show()
{
    if( gr_data.isBackgr_color )
      plot->setBackgroundColor( QColor( gr_data.b_color[0],
    					gr_data.b_color[1], 
					gr_data.b_color[2]) );
    else
      plot->setBackgroundColor( Qt::white /*backgroundColor().dark(110)*/ );

    plot->setPlotBounds( FPoint(minX, minY),
                   FPoint(maxX, maxY));

    ShowPlots();

    update();

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
                */
		plot->Add(new PPoint(plot, pnts[i],
            			gr_data.getType(nLn), gr_data.getSize(nLn), getColor(nLn)));
		    
                if( gr_data.getLineSize(nLn) > 0 && i != 0 )
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
    GraphDialogData::resizeEvent(qpev);
    plot->resize(plotFrame->width(), plotFrame->height());
    update();
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

    for( uint ii=0, kk=0; ii<gr_data.plots.GetCount(); ii++ )
    {
        for( int jj=0; jj<gr_data.plots[ii].getLinesNumber(); kk++, jj++ )
        {
	    aSymbolLabels[kk]->setData( gr_data.lines[kk] );
	    aLegendLabels[kk]->setText( gr_data.lines[kk].name );
        }
    }
    pGrpLegend->update();

    plot->setAxisTitles(gr_data.xName.c_str(), gr_data.yName.c_str());
    plot->Clear();
    Show();
    plot->update();
}


void
GraphDialog::AddPoint( int nPlot, int nPoint, bool no_mt )
{

  qApp->lock();
  if( nPlot>=0 && nPoint>=0 )
  {
    int nLn = gr_data.plots[nPlot].getFirstLine();

    for( int jj=0; jj<gr_data.plots[nPlot].getLinesNumber(); jj++, nLn++ )
    {
        FPoint pnt = gr_data.plots[nPlot].getPoint( jj, nPoint );
        plot->Add(new PPoint(plot, pnt, gr_data.getType(nLn),
                       gr_data.getSize(nLn), getColor(nLn)));
    }

    plot->update();
  }
  qApp->unlock();
  if( no_mt)
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

void
GraphDialog::CmSave()
{
    QString filter;
    QStrList formatList = QImageIO::outputFormats();

    if( formatList.contains("PNG") )
	filter.append("PNG Files (*.png)");
    
    QStrListIterator it( formatList );
    for( ; it != 0; ++it ) {
	QString str = *it;
	if( str == "PNG" )
	  continue;
	if( !filter.isEmpty() )
	    filter.append(";;");
	filter.append(str);
	filter.append(" Files (*.");
	filter.append(str.lower());
	filter.append(")");
    }

    QString selectedFilter;
    QString fn = QFileDialog::getSaveFileName(
          ".", filter, this, "savedlg", "Saving Plotting Image", &selectedFilter );
    if ( !fn.isEmpty() )
    {
	QString format( selectedFilter.left(3) );
	if( !fn.contains('.') )
	{
	    fn.append(".");
	    fn.append(format.lower());
	}
	QPixmap::grabWidget(plot).save(fn, format);
    }
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
        QPainter painter(&prt);

#ifndef __unix
#if QT_VERSION < 3
    qt_set_sane_printing( true );
#endif
#endif

        QPaintDeviceMetrics metrics(painter.device());
//        int dpix = metrics.logicalDpiX();
//        int dpiy = metrics.logicalDpiY();
//        const int margin = 10;//72; // pt

//        QRect body(margin*dpix/72, margin*dpiy/72,
//        metrics.width()-margin*dpix/72*2,
//        metrics.height()-margin*dpiy/72*2 );

        painter.setPen(Qt::black);
	painter.setFont(pVisorImp->getAxisLabelFont());

//	painter.scale(metrics.width()/double(plot->width()), metrics.height()/double(plot->height()));
	painter.translate(40, 50);
	painter.scale(metrics.logicalDpiX()/72., metrics.logicalDpiY()/72.); // seems to be always = 1
	plot->PaintToDC(painter);

/*	QPixmap pixmap(metrics.width(), metrics.height());
	QPainter painter1(&pixmap);
        painter1.setPen(Qt::black);
	painter1.setFont(QFont("Arial", 12));

	painter.scale(metrics.width()/double(plot->width()), metrics.height()/double(plot->height()));
	painter1.translate(40, 50);
	painter1.scale(metrics.logicalDpiX()/72., metrics.logicalDpiY()/72.); // seems to be always = 1
	plot->PaintToDC(painter1);
	painter1.flush();
	painter.drawPixmap(40, 50, pixmap);
*/
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
//    update();
    plot->update();
}

QColor
GraphDialog::getBackgrColor()
{
    return  plot->backgroundColor();
}


//*************************************************
//--------- PlotTypeBtn -------
//*************************************************

void PlotTypeBtn::drawButtonLabel(QPainter* paint)
{
    QColor color( plLine.red, plLine.green, plLine.blue);

    QRect rect(3, 3, width()-6, height()-6);
    QPoint center = rect.center();

    int size = 4;//plLine.size;

    drawSymbol(paint, center, plLine.type, size, color);
/*
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
*/
    // line, not crossing the point mark
    if(plLine.line_size > 0 )
    {
        paint->drawLine( rect.x(), center.y(), center.x()-size, center.y());
        paint->drawLine( center.x()+size, center.y(), rect.width(), center.y());
    }

    QPushButton::drawButtonLabel(paint);
}


// static
void 
PlotTypeBtn::drawSymbol(QPainter *paint, const QPoint& center, 
			    int type, int size, const QColor& color, int width)
{
    //paint->setBrush( QBrush::NoBrush );
    paint->setPen( QPen(color, width) );
    switch(type)
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
