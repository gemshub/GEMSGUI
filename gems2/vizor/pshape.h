//-------------------------------------------------------------------
// $Id$
//
// Declaration of PShape, PLine, PGrid, PPoint, TShapeArr
// and TPlotWin classes (Plotting system)
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
#ifndef _pshape_h_
#define _pshape_h_

#include <qdialog.h>
#include <qcolor.h>
#include <math.h>

#include "v_user.h"
#include "array.h"


/*!
    Class FPoint represents point with floating point coordinates as in calculations
*/
struct FPoint
{
    float x,y;

    FPoint()
    {
        x=y=0;
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

    void SetColor(QColor col)
    {
        color = col;
    }
    virtual void paint(QPainter& dc)=0;
    virtual void ConvertCoordinates()=0;
};


/*!
    Class represents Line on the plot
*/
class PLine:
            public PShape
{
    FPoint fp1, fp2;
    QPoint p1, p2;
    int line_size;

public:
    PLine(TPlotWin* par,
          const FPoint& pi1, const FPoint& pi2, QColor col, int size);

    virtual void paint(QPainter& dc);
    void ConvertCoordinates();
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
    //  bool draw;
    FPoint fp;
    QPoint p;
    int type;
    int size;
public:
    PPoint(TPlotWin* par, FPoint pi, int tp, int sz, QColor col);

    virtual void paint(QPainter& dc);
    void ConvertCoordinates();
};


/*!
    Class represents Grid on the plot

class PGrid:
            public PShape
{
    QPoint grid;
    QPoint offset;

    int NumberLines;

public:
    PGrid(TPlotWin* par, QColor col, int nLines = 2 );

    virtual void paint(QPainter& dc);
    void ConvertCoordinates();
};
*/

/*!
    Array of drawings on the plot


class TShapeArr:
            public TIArray<PShape>
{
public:
    TShapeArr():
            TIArray<PShape>(20)
    {}
};
*/

/*!
    Plotting area window
*/
class TPlotWin:
            public QWidget
{
    float	x1, y1, x2, y2;
    float	ax, ay;
    int		bx, by;
    QRect	canvasRect;
    gstring	title;
    TIArray<PShape> shapes;

protected:
    virtual void paintEvent(QPaintEvent* qpev);
    virtual void resizeEvent(QResizeEvent* qpev);

public:
    TPlotWin(QWidget* p, QRect rec, FPoint pt1, FPoint pt2, gstring title);
    ~TPlotWin();

    void setPlotBounds(FPoint pt1, FPoint pt2);
    void PaintToDC(QPainter& dc, QRect canvas);
    void paintGrid(QPainter& dc);
    void Clear();

    int Add(PShape* p)
    {
        p->ConvertCoordinates();
        return shapes.Add(p);
    }
//    void mul(const FPoint& f, QPoint& to);
    void RealToVisible(const FPoint& f, QPoint& to);
    const QRect& getCanvasRect() { return canvasRect; }
};


#endif    // _pshape_h
