//-------------------------------------------------------------------// $Id$
//
// Implementation of TPlot, GraphData and GraphWindow classes
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
#include <stdio.h>

#include "graph.h"
#include "v_module.h"
#include "visor_w.h"
#include "v_object.h"
#include "dlg/GraphDialog.h"

#include "gdatastream.h"

//---------------------------------------------------------------------------
// TPlot
//---------------------------------------------------------------------------


TPlot::TPlot( int aObjX, int aObjY ):
        nObjX(aObjX), nObjY(aObjY), first(0)
{

    int dNy, dMy, dY;
    foString = ( aObjX<0 || aObj[aObjX].GetN()>1 );

    dNy = aObj[aObjY].GetN();
    dMy = aObj[aObjY].GetM();

    if(aObjX<0) // numbers
        dX=dNy;
    else dX = aObj[aObjX].GetN()*aObj[aObjX].GetM();
    if( foString == true )
    {
        dY=dNy;
        dY1=dMy;
    } /* put graph by column */
    else
    {
        dY=dMy;
        dY1=dNy;
    } /* put graph by gstring */

    ErrorIf( dX!=dY, "Graphics demo", "Illegal size of objects.");

}


gstring 
TPlot::getName( int ii )
{
    vstr s(15);
    sprintf(s.p, "%s[%u]",aObj[nObjY].GetKeywd(), ii);
    return gstring(s.p);
}

gstring 
TPlot::getNames()
{
    vstr s(15);
    sprintf(s.p, "%s  %s",aObj[nObjX].GetKeywd(), aObj[nObjY].GetKeywd());
    return gstring(s.p);
}

TPlot::TPlot( TPlot& plt, int aFirst ):
        nObjX(plt.nObjX), nObjY(plt.nObjY), first(aFirst)
{

    foString = plt.foString;
    dX =plt.dX;
    dY1 =plt.dY1;
}

TPlot::~TPlot()
{}

// get array of points to draw one line
// return number of points in line

int TPlot::getPointLine( int j, TIArray<FPoint>& pnts )
{
    float x, y;
    pnts.Clear();

    for(int  ii=0; ii<dX; ii++ )
    {
        if( foString == true )  /* put graph by column */
        {
            if( nObjX < 0 )
                x = j;
            else x = aObj[nObjX].Get(ii,0);
            y = aObj[nObjY].Get(ii,j);
        }
        else    /* put graph by gstring */
        {
            x = aObj[nObjX].Get(0,ii);
            y = aObj[nObjY].Get(j,ii);
        }
        pnts.Add( new FPoint(x, y));
    }
    return dX;
}


// get point to draw one line
FPoint TPlot::getPoint( int line, int number )
{
    float x, y;

    if( foString == true )  /* put graph by column */
    {
        if( nObjX < 0 )
            x = number;
        else x = aObj[nObjX].Get(number,0);
        y = aObj[nObjY].Get(number, line);
    }
    else    /* put graph by gstring */
    {
        x = aObj[nObjX].Get(0, number);
        y = aObj[nObjY].Get(line, number);
    }

    return FPoint( x, y);
}

void TPlot::getMaxMin( float& minX, float& maxX, float& minY, float& maxY )
{
    float x,y;

    maxX = minX = aObj[nObjX].Get(0,0);
    maxY = minY = aObj[nObjY].Get(0,0);
    for( int j=0; j<dY1; j++)
        for( int ii =0; ii<dX; ii++)
        {
            if( foString == true )  /* put graph by column */
            {
                if( nObjX < 0 )
                    x = j;
                else x = aObj[nObjX].Get(ii,0);
                y = aObj[nObjY].Get(ii,j);
            }
            else    /* put graph by gstring */
            {
                x = aObj[nObjX].Get(0,ii);
                y = aObj[nObjY].Get(j,ii);
            }
            if( minX > x ) minX = x;
            if( maxX < x ) maxX = x;
            if( minY > y ) minY = y;
            if( maxY < y ) maxY = y;
        }
}

//----------------------------------------------------------------------------

/*!
   The constructor
*/
GraphData::GraphData( TIArray<TPlot>& aPlots, const char * aTitle,
               float *sizeReg,  float * sizePart,
               TPlotLine* aLinesDesc, short *aAxisType,
               const char *aXName, const char *aYName ):
        title(aTitle), axisType(aAxisType[0]),
        isBackgr_color(true)
{
    uint ii;
    int jj, nLines;

    xName = gstring( aXName, 0, 9);
    yName = gstring( aYName, 0, 9);
    // Insert Plots and lines description
    plots.Clear();
    lines.Clear();
    for( ii=0, nLines=0; ii<aPlots.GetCount(); ii++)
    {
        plots.Add( new TPlot(aPlots[ii], nLines ));
        for( jj=0; jj<plots[ii].getLinesNumber(); jj++, nLines++ )
        {
            if(aLinesDesc )
                lines.Add( new TPlotLine( aLinesDesc[nLines] ) );
            else
                lines.Add( new TPlotLine( plots[ii].getName(jj).c_str() ) );
            if( *lines[nLines].name == 0 )
                strncpy( lines[nLines].name, plots[ii].getName(jj).c_str(), 15 );
        }
    }

    for(int i=0; i<4; i++ )
    {
        region[i] = sizeReg[i];
        part[i]   = sizePart[i];
    }
    b_color[0] = aAxisType[1];
    b_color[1] = aAxisType[2];
    b_color[2] = aAxisType[3];
}

/*!
   The constructor
*/
GraphData::GraphData( TIArray<TPlot>& aPlots, const char * aTitle,
               const char *aXName, const char *aYName,
               TCStringArray line_names):
        title(aTitle), axisType(5),
        isBackgr_color(false)
{
    float min1, max1, min2, max2;
    float minX, maxX, minY, maxY;


    xName = gstring( aXName, 0, 9);
    yName = gstring( aYName, 0, 9);
    // Insert Plots and lines description
    plots.Clear();
    lines.Clear();
    for( uint ii=0, nLines=0; ii<aPlots.GetCount(); ii++)
    {
        plots.Add( new TPlot(aPlots[ii], nLines ));
        for( int jj=0; jj<plots[ii].getLinesNumber(); jj++, nLines++ )
        {
          if( nLines < line_names.GetCount() )
            lines.Add( new TPlotLine( line_names[nLines].c_str() ) );
          else
            lines.Add( new TPlotLine( plots[ii].getName(jj).c_str() ) );
        }
    }

    // set default min-max region

    plots[0].getMaxMin(  minX, maxX, minY, maxY );

    for( uint ii=1; ii<aPlots.GetCount(); ii++)
    {
        plots[ii].getMaxMin(  min1, max1, min2, max2 );
        if( minX > min1 ) minX = min1;
        if( maxX < max1 ) maxX = max1;
        if( minY > min2 ) minY = min2;
        if( maxY < max2 ) maxY = max2;
    }

    if( maxX == minX ) maxX += 1.;
    if( maxY == minY ) maxY += 1.;

    part[0] = minX+(maxX-minX)/3;
    part[1] = maxX-(maxX-minX)/3;
    part[2] = minY+(maxY-minY)/3;
    part[3] = maxY-(maxY-minY)/3;


    region[0] = minX;
    region[1] = maxX;
    region[2] = minY;
    region[3] = maxY;

}

GraphData::GraphData( GraphData& data ):
        title(data.title), axisType(data.axisType),
        xName(data.xName), yName(data.yName),
        isBackgr_color(data.isBackgr_color)
{
    uint ii;
    int jj, nLines;

    // Insert Plots and lines description
    plots.Clear();
    lines.Clear();
    for( ii=0, nLines=0; ii<data.plots.GetCount(); ii++)
    {
        plots.Add( new TPlot(data.plots[ii], nLines ));
        for( jj=0; jj<plots[ii].getLinesNumber(); jj++, nLines++ )
             lines.Add( new TPlotLine( data.lines[nLines] ) );
    }

    for(int i=0; i<4; i++ )
    {
        region[i] = data.region[i];
        part[i]   = data.part[i];
    }
    b_color[0] = data.b_color[0];
    b_color[1] = data.b_color[1];
    b_color[2] = data.b_color[2];
}


GraphData::~GraphData()
{}

//---------------------------------------------------------------------------

/*!
   The constructor
*/
GraphWindow::GraphWindow(TCModule *pmodule, TIArray<TPlot>& aPlots,
               const char * aTitle,
               float *sizeReg,  float * sizePart,
               TPlotLine* aLinesDesc, short *aAxisType,
               const char *aXName, const char *aYName )
{

   GraphData data(aPlots, aTitle, sizeReg, sizePart,
               aLinesDesc, aAxisType, aXName, aYName);
   graph_dlg = new GraphDialog( pmodule, data );
   graph_dlg->show();
}

/*!
   The constructor
*/
GraphWindow::GraphWindow(TCModule *pmodule, TIArray<TPlot>& aPlots,
               const char * aTitle,
               const char *aXName, const char *aYName,
               TCStringArray line_names  )
{
    GraphData  data( aPlots, aTitle, aXName, aYName, line_names );
   graph_dlg = new GraphDialog(pmodule, data);
   graph_dlg->show();
}

GraphWindow::~GraphWindow()
{}

void
GraphWindow::AddPoint( int nPlot, int nPoint, bool no_mt )
{
   graph_dlg->AddPoint( nPlot, nPoint, no_mt );
}

void
GraphWindow::Show()
{
   graph_dlg->ShowNew();
}


GraphData *
GraphWindow::getGraphData() const
{
 return &graph_dlg->gr_data;
}



void TPlotLine::read(GemDataStream& stream)
{
    stream >> type;
    stream >> size;
    stream >> line_size;
    stream >> red >> green >> blue;
    stream.readArray(name, sizeof(name));
}

void TPlotLine::write(GemDataStream& stream)
{
    stream << type;
    stream << size;
    stream << line_size;
    stream << red << green << blue;
    stream.writeArray(name, sizeof(name));
}


//--------------------- End of graph.cpp ---------------------------
