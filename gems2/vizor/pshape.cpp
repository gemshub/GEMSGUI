;//-------------------------------------------------------------------
// $Id$
//
// Implementation of PShape, PLine, PGrid, PPoint and TPlotWin classes
// (Plotting system)
//
// Copyright (C) 1996-2001  A.Rysin
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
#include <qpainter.h>
#include <qbrush.h>
#include <qfontmetrics.h>
#include <qfont.h>
#include <qdragobject.h>

#include <math.h>

#include "pshape.h"


// ----------------------------------------------
// PShape

PShape::PShape(TPlotWin* p, QColor col):
        par(p),
        color(col)
{}


PShape::~PShape()
{}

// ----------------------------------------------
// PLine

PLine::PLine(TPlotWin* par,
             const FPoint& pi1, const FPoint& pi2, QColor col, int line_size_):
        PShape(par,col),
        fp1(pi1), fp2(pi2), line_size( line_size_ )
{}


void
PLine::ConvertCoordinates()
{
    par->RealToVisible(fp1, p1);
    par->RealToVisible(fp2, p2);
}


void
PLine::paint(QPainter& dc)
{
    QPen pen(color, line_size);
    dc.setPen( pen );
    dc.moveTo(p1);
    dc.lineTo(p2);
}


// ----------------------------------------------
// PPoint class

PPoint::PPoint(TPlotWin* par, FPoint pi, int tp, int sz, QColor col):
        PShape(par, col),
        fp(pi), type(tp), size(sz)
{}


void
PPoint::ConvertCoordinates()
{
    par->RealToVisible(fp, p);
}

void
PPoint::paint(QPainter& dc)
{
    //  if( !draw )
    //    return;
    dc.setPen( QPen(color, 2) );
    dc.setBrush( QBrush::NoBrush );

    switch(type)
    {
    case	 P_POINT:        // point
        size = 0;
        dc.drawEllipse( QRect(p-QPoint(size,size),p+QPoint(size+1,size+1)) );
        break;
    case	 P_CROSSDIAG:    // X
        dc.drawLine( p.x()-size, p.y()+size, p.x()+size, p.y()-size);
        dc.drawLine( p.x()-size, p.y()-size, p.x()+size, p.y()+size);
        break;
    case	 P_SQUARE:       // square
        dc.drawRect(QRect(p-QPoint(size,size),p+QPoint(size+1,size+1)));
        break;
    case	 P_FILLSQUARE:   // fill square
        dc.drawRect(QRect(p-QPoint(size,size),p+QPoint(size+1,size+1)));
        dc.fillRect(QRect(p-QPoint(size,size),p+QPoint(size+1,size+1)),
                    QBrush(color));
        break;
    case  P_RHOMBUS:      // rhombus
        dc.drawLine( p.x()-size, p.y(), p.x(), p.y()-size);
        dc.drawLine( p.x(), p.y()-size, p.x()+size, p.y());
        dc.drawLine( p.x()+size, p.y(), p.x(), p.y()+size);
        dc.drawLine( p.x(), p.y()+size, p.x()-size, p.y());
        break;
    case  P_WYE:          // Y
        dc.drawLine( p.x()-size, p.y()-size, p.x(), p.y());
        dc.drawLine( p.x()+size, p.y()-size, p.x(), p.y());
        dc.drawLine( p.x(), p.y(), p.x(), p.y()+size);
        break;
    case	 P_STAR:         // star
        {
            dc.drawLine( p.x()-size, p.y(), p.x()+size, p.y());
            dc.drawLine( p.x(), p.y()-size, p.x(), p.y()+size);
            int scale = (int)((float)size*0.7+0.5);
            dc.drawLine( p.x()-scale, p.y()-scale, p.x()+scale, p.y()+scale);
            dc.drawLine( p.x()-scale, p.y()+scale, p.x()+scale, p.y()-scale);
            break;
        }
    case	 P_CIRCLE:        // circle
        dc.drawEllipse( QRect(p-QPoint(size,size),p+QPoint(size+1,size+1)) );
        break;
    case	 P_FILLCIRCLE:        // fill circle
        dc.drawEllipse( QRect(p-QPoint(size,size),p+QPoint(size+1,size+1)) );
        dc.setBrush( color );
        dc.drawEllipse( QRect(p-QPoint(size,size),p+QPoint(size+1,size+1)) );
        break;

    case  P_FILLRHOMBUS:      // fill rhombus
        QPointArray arr(4);
        dc.setBrush( color );
        arr.setPoint( 0, p.x()-size, p.y());
        arr.setPoint( 1, p.x(), p.y()-size);
        arr.setPoint( 2, p.x()+size, p.y());
        arr.setPoint( 3, p.x(), p.y()+size);
        dc.drawPolygon( arr, true);
        break;
    }
}


PText::PText(TPlotWin* par, QPoint pi, QColor col, QString text_):
        PShape(par, col),
        point(pi),
	text(text_)
{}

void
PText::paint(QPainter& dc)
{
    dc.setPen( QPen(color, 2) );
    QFont font = dc.font();
    font.setBold(true);
    dc.setFont(font);

    dc.drawText( point, text );
}


// TPlotWin class
const int bottomGap = 20;
const int topGap = 20;
const int leftGap = 20;

TPlotWin::TPlotWin(QWidget* p, QRect rec, FPoint pt1, FPoint pt2, gstring title_):
        QWidget(p),
        x1(pt1.x), y1(pt1.y), x2(pt2.x), y2(pt2.y),
//	canvasRect(rec),
	title(title_)
{
    setAcceptDrops(TRUE);

    QRect rect = parentWidget()->geometry();
    setGeometry(rect.x()+2, rect.y()+2, rect.width()-4, rect.height()-4);
    setPlotBounds(pt1, pt2);

// seems like geomtry is not set up before show() call
// so we have to call init() later
    show();
    init();
}

TPlotWin::~TPlotWin()
{
}

void
TPlotWin::init() 
{
//    QFontMetrics fm(font());
    const QFont fn("");
    QFontMetrics fm(fn);
    int txtWidth = fm.width(title.c_str());
    PText* txtLabel = new PText(this, 
	    QPoint((parentWidget()->width()-4 - txtWidth)/2, 13), Qt::black, title.c_str());
    shapes.Add(txtLabel);
    cerr << "width " << parentWidget()->width() << " txtWidth " << txtWidth << endl;
}

/*!
    Sets painting area for plot and converts all coordinates
*/
void
TPlotWin::setPlotBounds(FPoint pt1, FPoint pt2)
{
    x1 = pt1.x;
    x2 = pt2.x;
    y1 = pt1.y;
    y2 = pt2.y;

    // width
    if( x2-x1 != 0 )
        ax = (geometry().width()-leftGap)/(x2-x1);
    else
        ax = (geometry().width()-leftGap) / 0.0001;

    bx = ROUND(x1*ax) - leftGap;
    // height
    if( y2-y1 != 0 )
        ay = (geometry().height()-bottomGap-topGap)/(y2-y1);	// 0 is upper
    else
        ay = (geometry().height()-bottomGap-topGap) / 0.0001;

    by = ROUND(y1*ay) - topGap; // we shift here only on top gap - not bottom

    for( uint ii=0; ii<shapes.GetCount(); ii++ )
        shapes[ii].ConvertCoordinates();
}

void 
TPlotWin::setGridCount(int numGrids)
{
    gridCount = numGrids;    
}

/*!
    Recalculates coordiantes after size change
*/
void
TPlotWin::resizeEvent(QResizeEvent* qpev)
{
    QRect rect = parentWidget()->geometry();
    setGeometry(rect.x()+2, rect.y()+2, rect.width()-4, rect.height()-4);
    setPlotBounds(FPoint(x1, y1), FPoint(x2, y2));
    update();
}


/*!
    Paints contents of the plot
*/
void
TPlotWin::paintEvent(QPaintEvent* qpev)
{
    //  dc.SetBkMode(TRANSPARENT);
    QPainter dc(this);
    PaintToDC(dc, dc.window());
}

/*!
    Paints contents of the plot to the DC within canvas rectangle
*/
void
TPlotWin::PaintToDC(QPainter& dc, QRect DC_canvas)
{
    paintGrid(dc);

    for( uint ii=0; ii<shapes.GetCount(); ii++ )
        shapes[ii].paint(dc);
}


void
TPlotWin::paintGrid(QPainter& dc)
{
    if( gridCount <= 0 || gridCount > 20 )
        return;
    //  dc.SetBkMode(TRANSPARENT);

    QRect canvas = geometry(); //this->getCanvasRect();
    QPoint offset(leftGap, topGap);
    
    QPen pen( Qt::black, 1 );
    pen.setStyle( QPen::DotLine );
    dc.setPen( pen );

    // need float grid interval to make it precise
    float gridX = (float(canvas.width()-leftGap) / gridCount);
    float gridY = (float(canvas.height()-bottomGap-topGap) / gridCount);

    if( gridX )
        for( int ii=0; ii<=gridCount ; ii++ )
        {
	    int x_pos = (int)ROUND(offset.x() + gridX * ii);
            dc.moveTo( x_pos, 0 );
            dc.lineTo( x_pos, canvas.height() );
	    QString str;
	    str.sprintf("%.3g", x1 + (ii * (x2 - x1)) / gridCount);
	    dc.drawText( x_pos + 2, canvas.height() - 7, str);
        }

    if( gridY )
        for( int ii=0; ii<=gridCount ; ii++ )
        {
	    int y_pos = (int)ROUND(offset.y() + gridY * ii);
            dc.moveTo( 0, y_pos );
            dc.lineTo( canvas.width(), y_pos );
	    QString str;
	    str.sprintf("%.3g", y1 + ((gridCount - ii) * (y2 - y1)) / gridCount);
	    dc.drawText( 2, y_pos - 1, str);
        }
}


/*!
    Converts floating point coordinates to integer respecting to current plotting area
    Also does reversing by OY, because 0,0 on screen is upper left - not lower left as in real world
*/
void TPlotWin::RealToVisible(const FPoint& f, QPoint& to)
{
    to.setX((int)ROUND(ax * f.x - bx));
    to.setY((int)ROUND(ay * (y2-f.y) - by));
}

void
TPlotWin::dragEnterEvent(QDragEnterEvent* event)
{
    event->accept(
        QTextDrag::canDecode(event) 
//       || QImageDrag::canDecode(event)
    );
}

void
TPlotWin::dropEvent(QDropEvent* event)
{
    QImage image;
    QString text;

    if ( QImageDrag::decode(event, image) ) {
      //insertImageAt(image, event->pos());
    }
    else if ( QTextDrag::decode(event, text) ) {
      shapes.Add(new PText(this, event->pos(), Qt::black, text));
      update();
    }
}


// ----------------------------------------------
// PGrid
/*
PGrid::PGrid(TPlotWin* p, QColor col, int nLines ):
        PShape(p, col),
        gridCount(nLines)
{}


void
PGrid::ConvertCoordinates()
{
//    par->RealToVisible(FPoint(0,0), offset);
}


void
PGrid::paint(QPainter& dc)
{

    if( gridCount <= 0 || gridCount > 20 )
        return;
    //  dc.SetBkMode(TRANSPARENT);

    QRect canvas = par->getCanvasRect();
//    QPoint zero;
//    par->RealToVisible( FPoint(0,0), zero );
    
    QPen pen( color, 1 );
    pen.setStyle( QPen::DotLine );

    dc.setPen( pen );
    offset.setX(0);
    offset.setY(0);
    grid.setX((canvas.width()-1)/gridCount);
    grid.setY((canvas.height()-3)/gridCount);

    int deltaX = (canvas.width()-1)%gridCount;
    int deltaY = (canvas.height()-3)%gridCount;

    if( grid.x() )
        for( int ii=offset.x(); ii<=canvas.width() ; ii+=grid.x() )
        {
            if( deltaX > 0 )
            {
                ii++;
                deltaX--;
            }
            dc.moveTo( QPoint(ii, 0) );
            dc.lineTo( QPoint(ii, canvas.height()) );
        }

    if( grid.y() )
        for( int ii=offset.y(); ii<=canvas.height() ; ii+=grid.y() )
        {
            if( deltaY > 0 )
            {
                ii++;
                deltaY--;
            }
            dc.moveTo( QPoint(0, ii) );
            dc.lineTo( QPoint(canvas.width(), ii) );
        }
}
*/

//--------------------- End of pshape.cpp ---------------------------
