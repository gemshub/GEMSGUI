//-------------------------------------------------------------------
// $Id$
//
// Declarations of TPlot, GraphData and GraphWindow classes
// (Plotting system)
//
// Copyright (C) 1996-2001  S.Dmytriyeva, A.Rysin
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
#ifndef graph_data_h
#define graph_data_h

#include  "array.h"
#include  "v_module.h"

const int maxPLOT = 20;

struct TPlotLine
{
    int type;  // type of points
    int size;  // size of points
    int line_size;   // draw connected points

    int red;    // parts of class QColor
    int green;
    int blue;

    char name[15];


    TPlotLine( const char *aName = 0,
               int aPointType = 6, int aPointSize = 2, int aPutLine = 2,
               int aRed = 25, int aGreen = 0, int aBlue = 150
             ):
            type(aPointType), size(aPointSize), line_size(aPutLine),
            red(aRed),        green(aGreen), blue(aBlue)
    {
        strncpy( name, aName, 14);
        name[14] = '\0';
    }

    TPlotLine(const TPlotLine& plt ):
            type(plt.type), size(plt.size), line_size(plt.line_size),
            red(plt.red),        green(plt.green), blue(plt.blue)
    {
        strncpy( name, plt.name, 15);
    }
    const TPlotLine& operator=( const TPlotLine& p)
    {
        type = p.type, size = p.size, line_size=p.line_size,
        red = p.red,   green = p.green, blue=p.blue;
        strncpy( name, p.name, 15);
        return *this;
    }
};


class FPoint;

// description of one plot in graph screen
class TPlot
{

    int nObjX; // indexes in TObjList aObj
    int nObjY;

    TIArray<TPlotLine> lines;

    // work variables
    int dX;   // number of point in one line
    int dY1;  // number of lines
    bool foString;
    int first;

public:


    TPlot( int aObjX, int aObjY );
    TPlot( TPlot& plt, int aFirst );
    ~TPlot();


    int getLinesNumber() const
    {
        return dY1;
    }   // size from nObjY
    int getFirstLine() const
    {
        return first;
    }   // first index in lines list
    int getPointLine( int ii, TIArray<FPoint>& pnts ); // get one line to paint
    FPoint getPoint( int line, int number ); // get point from one line to paint
    void getMaxMin( float& minX, float& maxX, float& minY, float& maxY );

    gstring getName( int ii);
    gstring getNames();
};

//----------------------------------------------------------------------------
struct GraphData
{

    gstring title;

    TIArray<TPlot> plots;     // objects to demo
    TIArray<TPlotLine> lines; // descriptions of all lines

    int axisType;
    gstring xName;
    gstring yName;

    float region[4];
    float part[4];
    bool isBackgr_color;
    int b_color[3]; // red, green, blue

    GraphData(  TIArray<TPlot>& aPlots, const char * title,
            float *sizeReg, float *sizePrt,
            TPlotLine * aLinesDesc, short *aAxisType,
            const char *aXName = 0, const char *aYname = 0 );
    GraphData( TIArray<TPlot>& aPlots, const char * title,
            const char *aXName = 0, const char *aYname = 0,
            TCStringArray line_names = 0 );
    GraphData( GraphData& data );
    ~GraphData();

    int getSize( int ii) const
    {
        return lines[ii].size;
    }
    int getType( int ii) const
    {
        return lines[ii].type;
    }
    int getLineSize(int ii) const
    {
        return lines[ii].line_size;
    }
    const char * getName( int ii) const
    {
        return lines[ii].name;
    }
};

class GraphDialog;

class GraphWindow
{

public:

  GraphDialog *graph_dlg;

//  GraphData data;
  GraphWindow( TCModule *pmodule, TIArray<TPlot>& aPlots, const char * title,
            float *sizeReg, float *sizePrt,
            TPlotLine * aLinesDesc, short *aAxisType,
            const char *aXName = 0, const char *aYname = 0 );
  GraphWindow( TCModule *pmodule, TIArray<TPlot>& aPlots, const char * title,
            const char *aXName = 0, const char *aYname = 0,
            TCStringArray line_names = 0 );
  ~GraphWindow();

  void AddPoint( int nPlot, int nPoint, bool no_mt ); // Add new point to graph
  void Show(); // new show all lines
  GraphData *getGraphData() const;

};


#endif   // _graph_data_h

