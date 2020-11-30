//-------------------------------------------------------------------
// $Id: graph.h 968 2007-12-13 13:23:32Z gems $
//
// Declarations of TPlotLine, TPlot, GraphData and GraphWindow classes
// (Plotting system description)
//
// Copyright (C) 2013  S.Dmytriyeva
//
// This file is part of the GEM-Selektor GUI library which uses the
// Qt v.4 cross-platform App & UI framework (https://qt.io/download-open-source)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the GPL v.3 license

//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#ifndef graph_data_old_h
#define graph_data_old_h

#include <math.h>
#include <vector>
#include <fstream>
class QPointF;
#include <QtGui/QColor>
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

    std::string getName() const
    {
        return std::string(name, 0, 15);
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
    void read(std::fstream& stream);
    void write(std::fstream& stream);
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
    std::string getName( int ii);

    // first index in lines list
    int getFirstLine() const
    {  return first;  }

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


    /// Get point from one line to paint  (ndxX - column in Abscissa table)
    QPointF getPoint( int line, int number, int ndxX );
    /// Get min and max values x,y for one curve line
    void getMaxMinLine( double& minX, double& maxX, double& minY, double& maxY,
                        int line, int ndxAbs );

};

#endif   // graph_data_old_h

