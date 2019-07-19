//-------------------------------------------------------------------
// $Id: graph.h 968 2007-12-13 13:23:32Z gems $
//
// Declarations of TPlotLine, TPlot, GraphData and GraphWindow classes
// (Plotting system description)
//
// Copyright (C) 2013  S.Dmytriyeva
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

#ifndef graph_data_old_h
#define graph_data_old_h

#include <math.h>

#include <QPointF>
#include <QVector>
#include <QColor>

#ifdef USE_QWT
#include <qwt_series_data.h>
#endif

#include  "gstring.h"
class GemDataStream;

//const int maxPLOT = 20;

enum GRAPHTYPES {
    LINES_POINTS = 0,
    CUMULATIVE   = 1,
    ISOLINES     =  2,
    LINES_3D     = 3    // future using in gem2mt
};

/**  new

enum GRAPHTYPES_ {
                 LineChart = 0,
                 AreaChart = 1,
                 BarChart  = 2,
                 Isolines  = 3,   // under construction
                 lines_3D  = 4    // for future using
};
*/

/// Description of one plot curve - the representation of a series of points in the x-y plane
class TPlotLine
{
    int type;   // type of points
    int sizes;  // size of points ( sizes % 100 ); size of lines ( sizes / 100 % 10 )
    int ndxX;   // column in Abscissa table

    int red;    // parts of class QColor
    int green;
    int blue;

    char name[16];

public:

    TPlotLine( const char *aName, int aPointType, int aPointSize,
               int aLineSize, int lineStyle, int usespline,
               int andx, const QColor& aColor  ):
        TPlotLine( aName, aPointType, aPointSize, aLineSize,
                   aColor.red(), aColor.green(), aColor.blue()  )
    {
       setIndex( andx );
       setSpline( usespline );
       setLineStyle( lineStyle );
    }

    TPlotLine( const char *aName = nullptr,
               int aPointType = 0, int aPointSize = 4, int aPutLine = 2,
               int aRed = 25, int aGreen = 0, int aBlue = 150  )
    {
        ndxX = 0;
        setChanges( aPointType, aPointSize, aPutLine, QColor(aRed, aGreen, aBlue) );
        memcpy( name, aName, 15);
        name[15] = '\0';
    }

    TPlotLine( int ii, int maxII, const char *aName = nullptr,
               int aPointType = 0, int aPointSize = 4, int aPutLine = 2,
               int andx = 0):
        type(aPointType), sizes(aPutLine*100+aPointSize), ndxX(andx)
    {
        QColor aColor;
        aColor.setHsv( 360/maxII*ii, 200, 200);
        setChanges( aPointType, aPointSize, aPutLine, aColor );
        memcpy( name, aName, 15);
        name[15] = '\0';
    }

    TPlotLine(const TPlotLine& plt ):
        type(plt.type), sizes(plt.sizes), ndxX(plt.ndxX),
        red(plt.red),   green(plt.green), blue(plt.blue)
    {
        memcpy( name, plt.name, 15);
    }

    const TPlotLine& operator=( const TPlotLine& p)
    {
        type = p.type;
        sizes = p.sizes;
        ndxX=p.ndxX;
        red = p.red;
        green = p.green;
        blue=p.blue;
        memcpy( name, p.name, 15);
        return *this;
    }

    int getType() const
    {
        return type;
    }

    void setType( int aType )
    {
        type = aType;
    }

    int getSize() const
    {
        return (sizes % 100);
    }

    int getLineSize() const
    {
        return (sizes /100 % 10);
    }

    int getSpline() const
    {
        return (sizes /10000 % 10);
    }

    int getLineStyle() const
    {
        auto style = sizes /1000 % 10;
        if(style == 9)
          style=-1;
        return style+1;
    }

    void setSpline( int spline )
    {
        sizes += spline*10000;
    }

    void setLineStyle( int style )
    {
        if( style <= 0)
          style = 9;
        else
          style--;
        sizes += style*1000;
    }

    QColor getColor() const
    {
        return QColor(red, green, blue);
    }

    void setChanges( int aPointType, int aPointSize, int aPutLine, QColor aColor )
    {
        type = aPointType;
        sizes = aPutLine*100+aPointSize;
        red =   aColor.red();
        green = aColor.green();
        blue  = aColor.blue();
    }

    void setName( const char *aName )
    {
        memcpy( name, aName, 15);
        name[15] = '\0';
    }

    gstring getName() const
    {
        return gstring(name, 0, 15);
    }

    int getIndex() const
    {
        return ndxX; // max( ndxX, -1);
    }

    void setIndex( int aNdxX )
    {
        //if( aNdxX < 0  )
        //    aNdxX = 0;
        ndxX = aNdxX;
    }

    void read(GemDataStream& stream);
    void write(GemDataStream& stream);
    void read(fstream& stream);
    void write(fstream& stream);
};


/// Description of one plot in graph screen
/// a series of curves in the x-y plane
class TPlot
{
    int nObjX; /// index of Abscissa in TObjList aObj
    int nObjY; /// index of Ordinate in TObjList aObj

    // work variables
    int dX;   // number of point in one line
    int nAbs; //number of different Abscissa lines
    int dY1;  // number of lines (Ordinate)
    bool foString;
    int first;

public:

    TPlot( int aObjX, int aObjY );
    TPlot( TPlot& plt, int aFirst );
    ~TPlot();

    bool getfoString() const
    { return foString; }

    /// Get number of points for one curve
    int getdX() const
    { return dX; }

    /// Get number of Abscissa
    int getNAbs() const
    { return nAbs; }

    /// Get number of curves
    int getLinesNumber() const
    { return dY1;   }

    /// Return string with Ordinate name and line index
    gstring getName( int ii);

    // first index in lines list
    int getFirstLine() const
    {  return first;  }

#ifdef USE_QWT
    int getObjX() const { return nObjX; }
    int getObjY() const { return nObjY; }

    /// Get one line to paint (ndxX - column in Abscissa table)
    int getPointLine( int line, QVector<QPointF>& points, int ndxX );
    /// Get one line to paint cumulative curve (ndxX - column in Abscissa table)
    int getPointTube( int line, QVector<QwtIntervalSample>& points, int ndxAbs );

    /// obsolete
    //void getMaxMin( QPointF& min, QPointF& max );
    //void getMaxMinIso( QPointF& min, QPointF& max );
#else

    size_t getObjX() const { return fabs(nObjX); }
    size_t getObjY() const { return fabs(nObjY); }

    /// Get object value for model ( col < getNAbs() for Abscissa )
    double getValue( int row, int  col ) const;
    /// Get object name for model ( col < getNAbs() for Abscissa )
    QString getColumnName( int col ) const;

    /// Abscissa columns list
    std::vector<int> xColumns() const;
    /// Ordinate columns list
    std::vector<int> yColumns() const;

#endif

    /// Get point from one line to paint  (ndxX - column in Abscissa table)
    QPointF getPoint( int line, int number, int ndxX );
    /// Get min and max values x,y for one curve line
    void getMaxMinLine( QPointF& min, QPointF& max, int line, int ndxX );

};

#ifdef USE_QWT

/// Description of 2-D plotting widget.
/// Description include the curves (TPlotLine, TPlot) settings, the grid settings, the isoline structure settings
struct GraphData
{
    gstring title;
    int graphType;      /// GRAPHTYPES ( 0-line by line, 1- cumulative, 2 - isolines )

    // define grid of plot
    int axisTypeX;
    int axisTypeY;
    gstring xName;
    gstring yName;
    double region[4];
    double part[4];

    // define background color
    // bool isBackgr_color;
    int b_color[3]; // red, green, blue

    // define curves
    TIArray<TPlot> plots;     /// objects to demo
    TIArray<TPlotLine> lines; /// descriptions of all lines

    // data to isoline plots
    TIArray<QColor> scale;    // scale colors for isolines

public:

    GraphData(  TIArray<TPlot>& aPlots, const char * title,
                float *sizeReg, float *sizePrt,
                TPlotLine * aLinesDesc, short *aAxisType,
                const char *aXName = nullptr, const char *aYname = nullptr );

    GraphData( TIArray<TPlot>& aPlots, const char * title,
               const char *aXName = nullptr, const char *aYname = nullptr,
               TCStringArray line_names = 0, int agraphType = LINES_POINTS );

    GraphData( GraphData& data );

    ~GraphData();

    int getSize( int line ) const
    { return lines[line].getSize();   }

    int getType( int line ) const
    {  return lines[line].getType();  }

    int getLineSize(int line ) const
    {   return lines[line].getLineSize();   }

    int getIndex(int line ) const
    {
        int ndx = lines[line].getIndex();
        if( plots[ getPlot( line ) ].getNAbs() <= ndx )
            ndx = 0;
        return ndx;
    }

    void setIndex(int line, int index  ) const
    {   lines[line].setIndex(index);   }

    gstring getName( int line ) const
    {   return lines[line].getName();   }

    void setName( int line, const char *aName ) const
    {   lines[line].setName( aName );   }

    QColor getColor( int line ) const
    {   return lines[line].getColor();  }

    int getPlot(int line ) const
    {   uint ii=1;
        for( ; ii<plots.GetCount(); ii++)
            if( line < plots[ii].getFirstLine() )
                break;
        return (ii-1);
    }

    void getIndexes( QVector<int>& first, QVector<int>& maxXndx );

    /// set default grid data
    void adjustAxis( double& min, double& max, int& numTicks);

    // functions to isoline plot
    void setColorList();
    void getColorList();
    void setScales();
    bool goodIsolineStructure( int aGraphType );
    QColor getColorIsoline(int ii) const
    {
        return scale[ii];
    }
    double getValueIsoline(int ii);
    void setValueIsoline(double val, int ii);

};
#endif

#endif   // graph_data_old_h

