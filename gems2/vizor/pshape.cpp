//-------------------------------------------------------------------
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
#include "dlg/GraphDialog.h"


// ----------------------------------------------
// PShape class

PShape::PShape(TPlotWin* p, QColor col):
        par(p),
        color(col)
{}


PShape::~PShape()
{}

// ----------------------------------------------
// PLine class

PLine::PLine(TPlotWin* par,
             const FPoint& pi1, const FPoint& pi2, QColor col, int line_size_):
        PShape(par,col),
        fp1(pi1), fp2(pi2), line_size( line_size_ )
{}

void
PLine::paint(QPainter& dc)
{
    QPoint p1 = par->RealToVisible(fp1);
    QPoint p2 = par->RealToVisible(fp2);

    QPen pen(color, line_size);
    dc.setPen( pen );
    dc.moveTo(p1);
    dc.lineTo(p2);
}


// ----------------------------------------------
// PPoint class

PPoint::PPoint(TPlotWin* par, FPoint pi, int tp, int sz, QColor col):
        PShape(par, col),
        fp(pi), 
	type(tp), 
	size(sz)
{}


void
PPoint::paint(QPainter& dc)
{
    QPoint point = par->RealToVisible(fp);
    dc.setPen( QPen(color, 2) );
    dc.setBrush( QBrush::NoBrush );

    PlotTypeBtn::drawSymbol(&dc, point, type, size, color, 2);
}


/* PText is the class for the labels dragged to the plotting area
*/

PText::PText(TPlotWin* par, QPoint screenPoint, QColor col, QString text_):
        PShape(par, col),
	txt(text_)
{
    setPosition(screenPoint);
}

void
PText::paint(QPainter& dc)
{
    QPoint screenPoint = par->RealToVisible(point);

    dc.setPen( QPen(color, 2) );
    QFont font = dc.font();
    if( !font.bold() )
    {
	font.setBold(true);
    	dc.setFont(font);
    }

    dc.drawText( screenPoint, txt );
    
    // if after PText there are some objects painted
    // we have to (may) need to reset bold font
    // font = dc.font();
    // font.setBold(false);
    // dc.setFont(font);
}

bool
PText::contains(QPoint pt)
{
    QPoint screenPoint = par->RealToVisible(point);
    QFontMetrics fm(QPainter(par).fontMetrics());
    QRect rect(screenPoint, QSize(fm.width(txt), fm.height()));
    rect.moveBy(0, -fm.height());
    return rect.contains(pt);
}

void 
PText::setPosition(QPoint screenPoint)
{
    point = par->VisibleToReal(screenPoint);
}


/* TPlotWin class is responcible for plotting area (lines, points, grid, labels..)
*/
const int bottomGap = 20;
const int topGap = 20;
const int leftGap = 20;

TPlotWin::TPlotWin(QWidget* p, FPoint pt1, FPoint pt2, const char* title_):
        QWidget(p),
        x1(pt1.x), y1(pt1.y), x2(pt2.x), y2(pt2.y),
	title(title_)
{
    setAcceptDrops(TRUE);
//    QSizePolicy mySize( QSizePolicy::Expanding, QSizePolicy::Expanding );
//    setSizePolicy(mySize);
    
    setPlotBounds(pt1, pt2);

    show();
}

TPlotWin::~TPlotWin()
{
}

/*!
    Sets real coordinates of painting area and recalculates coefficients for screen points
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

    by = /*ROUND(y1*ay)*/0 - topGap; // we shift here only on top gap - not bottom
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
    setGeometry(2, 2, rect.width()-4, rect.height()-4);
    update();
}


/*!
    Paints contents of the plot
*/
void
TPlotWin::paintEvent(QPaintEvent* qpev)
{
    setPlotBounds(FPoint(x1, y1), FPoint(x2, y2));

    QPainter dc(this);
    PaintToDC(dc);
}

/*!
    Paints contents of the plot to the DC
*/
void
TPlotWin::PaintToDC(QPainter& dc)
{
    paintGrid(dc);

    int txtWidth = dc.fontMetrics().width(title);
    QPoint point((dc.window().width() - txtWidth)/2, 13);
    dc.drawText(point, title);

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
QPoint TPlotWin::RealToVisible(const FPoint& f)
{
    return QPoint((int)ROUND(ax * f.x - bx),
		(int)ROUND(ay * (y2-f.y) - by));
}

FPoint TPlotWin::VisibleToReal(const QPoint& vis)
{
    return FPoint(  (float(vis.x() + bx) / ax),
		    (y2 - (float(vis.y() + by) / ay)) );
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
//    QImage image;
    QString text;

//    if ( QImageDrag::decode(event, image) ) {
      //insertImageAt(image, event->pos());
//    }
//    else 
    if ( QTextDrag::decode(event, text) ) {

	int ii;
        for(ii=shapes.GetCount()-1; ii>=0; ii--) {
	    PText* txtLabel = dynamic_cast<PText*>(&shapes[ii]);
	    if( txtLabel != 0 && txtLabel->text() == text ) {
		txtLabel->setPosition(event->pos());
	    }
	}
	if( ii < 0 )
    	    shapes.Add(new PText(this, event->pos(), Qt::black, text));
      update();
    }
}

void 
TPlotWin::mousePressEvent( QMouseEvent *e ) {
//QWidget::mousePressEvent( e );

    for(int ii=shapes.GetCount()-1; ii>=0; ii--) {
	PText* txtLabel = dynamic_cast<PText*>(&shapes[ii]);
	if( txtLabel != 0 && txtLabel->contains(e->pos()) ) {

	    QFontMetrics fm = QPainter(this).fontMetrics();
	    QPixmap pixmap(fm.width(txtLabel->text()), fm.height());
	    QPainter dc(&pixmap, this);
//	    dc.setBackgroundMode(Qt::TransparentMode);
	    dc.setBackgroundColor(Qt::gray);
	    txtLabel->paint(dc);
	    //dc.setPen( Qt::black );
	    //dc.drawText(0, 0, txtLabel->text());

	    QTextDrag *drag = new QTextDrag( txtLabel->text(), this );
	    drag->setPixmap(pixmap, QPoint(0, pixmap.height()));
	    drag->drag();
	    break;
	}
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
