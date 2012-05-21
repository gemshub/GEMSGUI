//-------------------------------------------------------------------
// $Id: pshape.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of PShape, PLine, PGrid, PPoint and TPlotWin classes
// (Plotting system)
//
// Copyright (C) 1996-2008  A.Rysin
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
#include <cmath>

#include <qpainter.h>
#include <qbrush.h>
#include <qfontmetrics.h>
#include <qfont.h>
#include <QDropEvent>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QPixmap>
#include <QMouseEvent>
#include <QDragEnterEvent>

#include "pshape.h"
#include "GemsMainWindow.h"
#include "GraphDialog.h"

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

    QPen pen(color, abs(line_size));
    dc.setPen( pen );
    dc.drawLine( p1, p2 );
}

// ----------------------------------------------
// PPolygon class    (added Sveta 05/2005 )

PPolygon::PPolygon( TPlotWin* par, QColor col,
                    const FPoint& pi1, const FPoint& pi2,
                    const FPoint& pi3, const FPoint& pi4):
        PShape( par, col ),
        fp1(pi1), fp2(pi2), fp3(pi3), fp4(pi4)
{}

void
PPolygon::paint(QPainter& dc)
{
    QPoint p1 = par->RealToVisible(fp1);
    QPoint p2 = par->RealToVisible(fp2);
    QPoint p3 = par->RealToVisible(fp3);
    QPoint p4 = par->RealToVisible(fp4);

    QPolygon  a(4);
    a.setPoint(0, p1 );
    a.setPoint(1, p2 );
    a.setPoint(2, p3 );
    a.setPoint(3, p4 );
    QBrush brush( color, Qt::SolidPattern);
    dc.setBrush( brush );
    dc.setPen( Qt::NoPen );
    dc.drawPolygon( a );
    QPen pen(color, 2);
    dc.setPen( pen ); // dc.setPen( Qt::SolidLine );
//    dc.pen().setWidth(2);
    dc.drawLine( p2, p3 );
    dc.drawLine( p1, p4 );
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
    dc.setBrush( Qt::NoBrush );
    drawSymbol(&dc, point, type, size, color, 2);
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
//    QFont font = dc.font();
    QFont font = pVisorImp->getAxisLabelFont();
    if( !font.bold() )
	font.setBold(true);
    dc.setFont(font);

    dc.drawText( screenPoint, txt );

    // if after PText there are some objects painted
    // we have to (may) need to reset bold font
    // font.setBold(false);
    // dc.setFont(font);
}

bool
PText::contains(QPoint pt)
{
    QPoint screenPoint = par->RealToVisible(point);
    QFontMetrics fm(QPainter(par).fontMetrics());
    QRect rect(screenPoint, QSize(fm.width(txt), fm.height()));
    rect.translate(0, -fm.height());
    return rect.contains(pt);
}

void
PText::setPosition(QPoint screenPoint)
{
    point = par->VisibleToReal(screenPoint);
}


/* TPlotWin class is responcible for plotting area (lines, points, grid, labels..)
*/
TPlotWin::TPlotWin(QWidget* p, FPoint pt1, FPoint pt2, const char* title_):
        QWidget(p),
        x1(pt1.x), y1(pt1.y), x2(pt2.x), y2(pt2.y),
	title(title_)
{
    setAcceptDrops(true);
    setAutoFillBackground( true); 
    setGap();
    setPlotBounds(pt1, pt2);
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
        ax = (width()-leftGap-rightGap)/(x2-x1);
    else
        ax = (width()-leftGap-rightGap) / 0.0001;

    bx = ROUND(x1*ax) - leftGap;
    // height
    if( y2-y1 != 0 )
        ay = (height()-bottomGap-topGap)/(y2-y1);	// 0 is upper
    else
        ay = (height()-bottomGap-topGap) / 0.0001;

    by = /*ROUND(y1*ay)*/0 - topGap; // we shift here only on top gap - not bottom
}

void
TPlotWin::setGap()
{
    QFont font = pVisorImp->getAxisLabelFont();
    int fontSize = font.pointSize();
    topGap = fontSize*3;
    bottomGap = fontSize*3;
    leftGap = fontSize+8 + QFontMetrics(font).width("-0.000");//fontSize*4;
    rightGap = fontSize*2;
}

void
TPlotWin::setGridCount(int numGrids)
{
    gridCount = numGrids;
}

void
TPlotWin::setAxisTitles(const char* xTitle_, const char* yTitle_)
{
    xTitle = xTitle_;
    yTitle = yTitle_;
}

/*!
    Recalculates coordiantes after size change
*/
void
TPlotWin::resizeEvent(QResizeEvent* qpev)
{
    QWidget::resizeEvent(qpev);
}


/*!
    Paints contents of the plot
*/
void TPlotWin::paintEvent(QPaintEvent* qpev)
{
    setPlotBounds(FPoint(x1, y1), FPoint(x2, y2));
    QPainter dc(this);
    PaintToDC(dc);
}

/*!
    Paints contents of the plot to the DC
*/
void TPlotWin::PaintToDC(QPainter& dc)
{
    dc.setClipRect( geometry() );
    paintGrid(dc);

    int txtWidth = dc.fontMetrics().width(title);
    QPoint point((width() - txtWidth)/2, topGap*2/3-3 /*QFontMetrics(font).xHeight()*2+7*/);
    dc.drawText(point, title);

    QRect canvas;
    canvas.setTop(topGap);
    canvas.setLeft(leftGap);
    canvas.setRight(width()-rightGap);
    canvas.setBottom(height()-bottomGap);
    dc.setClipRect( canvas );

    for( uint ii=0; ii<shapes.GetCount(); ii++ )
        shapes[ii].paint(dc);
}


void TPlotWin::paintGrid(QPainter& dc)
{
    if( gridCount <= 0 || gridCount > 20 )
        return;
    QRect canvas =geometry();
    canvas.setWidth(width()-rightGap);
    canvas.setHeight(height()-bottomGap);

    QPen pen( Qt::black, 1 );
    pen.setStyle( Qt::DotLine );
    dc.setPen( pen );

    // need float grid interval to make it precise
    float gridX = (float(width()-leftGap-rightGap) / gridCount);
    float gridY = (float(height()-topGap-bottomGap) / gridCount);

    QFont font = pVisorImp->getAxisLabelFont();

    QString cr("(c) GEMS");
    int fontSize = font.pointSize();
    font.setPointSize(9);
    dc.setFont(font);
    dc.drawText( width() - QFontMetrics(font).width(cr) - 2, height() - 4, cr);

    font.setPointSize(fontSize);
    font.setBold(true);
    dc.setFont(font);
    QFontMetrics fm(dc.fontMetrics());
    dc.drawText( (width() - fm.width(xTitle))/2, height()-6, xTitle);
    dc.rotate(-90);
    dc.drawText( -(height() + fm.width(yTitle))/2, fontSize+4, yTitle);
    dc.rotate(90);

    font.setBold(false);
    dc.setFont(font);

    int txtWidth;
    QString str;
    int x_pos = 0;
    int y_pos = canvas.height()+fontSize+4;

    if( gridX )
        for( int ii=0; ii<=gridCount ; ii++ )
        {
         x_pos = (int)ROUND(leftGap + gridX * ii);
         dc.drawLine( x_pos, topGap,  x_pos, canvas.height() );
         str.sprintf("%.4g", x1 + (ii * (x2 - x1)) / gridCount);
         txtWidth = dc.fontMetrics().width(str);
         dc.drawText( x_pos-txtWidth/2, y_pos, str);
        }

    x_pos = 12;
    if( gridY )
        for( int ii=0; ii<=gridCount ; ii++ )
        {
        y_pos = (int)ROUND(topGap + gridY * ii);
        dc.drawLine( leftGap, y_pos, canvas.width(), y_pos );
        str.sprintf("%.4g", y1 + ((gridCount - ii) * (y2 - y1)) / gridCount);
        txtWidth = dc.fontMetrics().width(str);
        x_pos = leftGap-2-txtWidth;
        dc.drawText( x_pos, y_pos, str);
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

void TPlotWin::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("text/plain")) {
        if (event->source() == this) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        } else {
            event->acceptProposedAction();
        }
    } else {
        event->ignore();
    }
}

void TPlotWin::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat("text/plain")) {
        if (event->source() == this) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        } else {
            event->acceptProposedAction();
        }
    } else {
        event->ignore();
    }
}

void TPlotWin::dropEvent(QDropEvent* event)
{
   if (event->mimeData()->hasFormat("text/plain"))
    {
	 QString text = event->mimeData()->text();  
	 int ii;
     for(ii=shapes.GetCount()-1; ii>=0; ii--) 
     {
         PText* txtLabel = dynamic_cast<PText*> (&shapes[ii]);
	    if( txtLabel != 0 && txtLabel->text() == text )
	    {
		   txtLabel->setPosition(event->pos());
		   break;
	    }
	}
	if( ii < 0 )
    	    shapes.Add(new PText(this, event->pos(), Qt::black, text));
      update();
    }
}

void TPlotWin::mousePressEvent( QMouseEvent *e )
{
    for(int ii=shapes.GetCount()-1; ii>=0; ii--)
    {
      PText* txtLabel = dynamic_cast<PText*>((&shapes[ii]));
      if( txtLabel != 0 && txtLabel->text() != "" )
       if( txtLabel->contains(e->pos()) )
       {
         QFontMetrics fm = QPainter(this).fontMetrics();
         QPixmap pixmap(fm.width(txtLabel->text())+2, fm.height()+2);
         pixmap.fill(QColor(0, 0, 0, 0));
         QPainter dc(&pixmap);
         dc.drawText(2, fm.height()-2, txtLabel->text()); 
         
        QMimeData * mimeData = new QMimeData;
        mimeData->setText( txtLabel->text() );
        QDrag *drag = new QDrag(this);
        drag->setMimeData( mimeData );
        drag->setPixmap(pixmap);
        drag->setHotSpot( QPoint(0, pixmap.height()) ); 
        drag->start();
        break;
	}
   } 	
   QWidget::mousePressEvent( e );
}


//--------------------- End of pshape.cpp ---------------------------
