//-------------------------------------------------------------------
// $Id: pshape.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of PShape, PLine, PGrid, PPoint, TShapeArr
// and TPlotWin classes (Plotting system)
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
#ifndef _pshape_h_
#define _pshape_h_

#include <qdialog.h>
#include <qcolor.h>
#include <QResizeEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMouseEvent>
#include <QPaintEvent>
#include <cmath>

#include "v_user.h"
#include "v_vals.h"


/*!
    Class FPoint represents point with floating point coordinates as in calculations
*/
struct FPoint
{
    float x, y;

    FPoint()
    {
        x = y = 0;
    }
    FPoint(float xx, float yy)
    {
        x = xx, y = yy;
    }
    FPoint(const FPoint& p)
    {
        x = p.x, y = p.y;
    }

    const FPoint& operator=(const FPoint& p)
    {
        x = p.x, y = p.y;
        return *this;
    }
    bool operator==(const FPoint& p)
    {
        return (x == p.x && y == p.y);
    }

    bool IsEmpty()
    {
      return ( x == FLOAT_EMPTY || y == FLOAT_EMPTY );
    }
};


class TPlotWin;

/*!
    Abstract class representing shapes (lines, dots ...) on the plot window
*/
class PShape
{
protected:
    TPlotWin* par;
    QColor color;
    bool operator ==(PShape&)
    {
        return false;
    }

public:
    PShape(TPlotWin* par, QColor col);
    virtual ~PShape();

    virtual void paint(QPainter& dc) = 0;

    void SetColor(QColor col) {
        color = col;
    }
//    virtual void ConvertCoordinates()=0;
    virtual QString text() {
    return "";
   }

};


/*!
    Class represents Line on the plot
*/
class PLine:
            public PShape
{
    FPoint fp1, fp2;
//    QPoint p1, p2;
    int line_size;

public:
    PLine(TPlotWin* par,
          const FPoint& pi1, const FPoint& pi2, QColor col, int size);

    virtual void paint(QPainter& dc);
//    void ConvertCoordinates();
};

/*!
    Class represents Polygon on the plot (added Sveta 05/2005 )
*/
class PPolygon:
            public PShape
{
    FPoint fp1, fp2, fp3, fp4;

public:
    PPolygon(TPlotWin* par, QColor col,
          const FPoint& pi1, const FPoint& pi2,
          const FPoint& pi3, const FPoint& pi4 );

    virtual void paint(QPainter& dc);

};


enum PlotPointType { // using point types
    P_POINT,        // point
    P_CROSSDIAG,    // X
    P_SQUARE,       // square
    P_FILLSQUARE,   // fill square
    P_RHOMBUS,      // rhombus
    P_WYE,          // Y
    P_STAR,         // star
    P_CIRCLE,       // circle
    P_FILLRHOMBUS,  // fill rhombus
    P_FILLCIRCLE    // fill circle
};

/*!
    Class represents Point on the plot
*/
class PPoint:
            public PShape
{
    FPoint fp;
    int type;
    int size;
public:
    PPoint(TPlotWin* par, FPoint pi, int tp, int sz, QColor col);

    virtual void paint(QPainter& dc);
};

/*!
    Class represents some Text on the plot
    Uses only screen coordinates
*/
class PText:
            public PShape
{
    FPoint point;
    QString txt;

public:
    PText(TPlotWin* par, QPoint pi, QColor col, QString text);

    virtual void paint(QPainter& dc);
    bool contains(QPoint pt);
    void setPosition(QPoint pt);
    QString text() {
	return txt;
    }
};


/*!
    Plotting area window
*/
class TPlotWin:
            public QWidget
{
    float	x1, y1, x2, y2;
    float	ax, ay;
    int		bx, by;
    QString	title;
    QString	xTitle;
    QString	yTitle;
    TIArray<PShape> shapes;
    int gridCount;

    int bottomGap;
    int topGap;
    int leftGap;
    int rightGap;

protected:
    virtual void paintEvent(QPaintEvent* qpev);
    virtual void resizeEvent(QResizeEvent* qpev);
    virtual void mousePressEvent( QMouseEvent *e );

public:
    TPlotWin(QWidget* p, FPoint pt1, FPoint pt2, const char* title);
    ~TPlotWin();

    void setGap();
    void setPlotBounds(FPoint pt1, FPoint pt2);
    void setGridCount(int numGrids);
    void setAxisTitles(const char* xTitle, const char* yTitle);

    void PaintToDC(QPainter& dc);
    void paintGrid(QPainter& dc);
    void Clear() {
	shapes.Clear();
    }

    int Add(PShape* p) {
        return shapes.Add(p);
    }

    QPoint RealToVisible(const FPoint& real);
    FPoint VisibleToReal(const QPoint& screen);

    void dragEnterEvent(QDragEnterEvent* event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent* event);
};


#endif    // _pshape_h
