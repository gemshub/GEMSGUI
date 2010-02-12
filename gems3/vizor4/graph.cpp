//-------------------------------------------------------------------
// $Id: graph.cpp 968 2007-12-13 13:23:32Z gems $
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
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------
#include <stdio.h>

#include "graph.h"
#include "v_module.h"
#include "visor_w.h"
#include "v_object.h"
#include "GraphDialog.h"

#include "gdatastream.h"


GColor::GColor( bool is_green, int i, int n)
{
    blue = 0;
    if( i<= n/2 )
      { red = 255;
        green = (2*i*255/n)%256;
      }
      else
         if( i <= 3*n/4 )
         { red = abs((255 - (4*(i-n/2)*255/n)))%256;
           green = 255;
         }
         else
         { red = 0;
           green = (255 - (4*(i-3*n/4)*255/(n+2)))%256;
         }

  if( !is_green )
  {
    blue = green;
    green = 0;
  }
}


//---------------------------------------------------------------------------
// TPlot
//---------------------------------------------------------------------------


TPlot::TPlot( int aObjX, int aObjY ):
        nObjX(aObjX), nObjY(aObjY), first(0)
{

    int dNy, dMy, dY;
    // foString = ( aObjX<0 || aObj[aObjX].GetN()>1 );
    foString = ( aObjX<0 || aObj[aObjX].GetN()>=1 );// SD 05/02/2007

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
                x = ii; //j;
            else if( aObj[nObjX].IsEmpty(ii,0) )
                   x = FLOAT_EMPTY;
                 else
                   x = aObj[nObjX].Get(ii,0);

            if( aObj[nObjY].IsEmpty(ii,j) )
              y = FLOAT_EMPTY;
            else
              y = aObj[nObjY].Get(ii,j);
        }
        else    /* put graph by gstring */
        {
           if( aObj[nObjX].IsEmpty(0,ii) )
             x = FLOAT_EMPTY;
           else
            x = aObj[nObjX].Get(0,ii);

           if( aObj[nObjY].IsEmpty(j,ii) )
            y = FLOAT_EMPTY;
           else
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
            else if( aObj[nObjX].IsEmpty(number,0) )
                   x = FLOAT_EMPTY;
                 else
                   x = aObj[nObjX].Get(number,0);

            if( aObj[nObjY].IsEmpty(number,line) )
              y = FLOAT_EMPTY;
            else
              y = aObj[nObjY].Get(number,line);
        }
        else    /* put graph by gstring */
        {
           if( aObj[nObjX].IsEmpty(0,number) )
             x = FLOAT_EMPTY;
           else
            x = aObj[nObjX].Get(0,number);

           if( aObj[nObjY].IsEmpty(line,number) )
            y = FLOAT_EMPTY;
           else
            y = aObj[nObjY].Get(line,number);
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

void TPlot::getMaxMinIso( float& minX, float& maxX, float& minY, float& maxY )
{
    float x,y;

    maxX = minX = aObj[nObjY].Get(0,0);
    maxY = minY = aObj[nObjY].Get(0,1);
    for( int ii =0; ii<dX; ii++)
    {
         x = aObj[nObjY].Get(ii,0);
         y = aObj[nObjY].Get(ii,1);
         if( minX > x ) minX = x;
         if( maxX < x ) maxX = x;
         if( minY > y ) minY = y;
         if( maxY < y ) maxY = y;
   }
}

void TPlot::getMaxMinIsoZ( float& minZ, float& maxZ )
{
    float z;

    maxZ = minZ = aObj[nObjY].Get(0,2);
    for( int ii =0; ii<dX; ii++)
    {
         z = aObj[nObjY].Get(ii,2);
         if( minZ > z ) minZ = z;
         if( maxZ < z ) maxZ = z;
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
        title(aTitle), axisType(aAxisType[0]), graphType(aAxisType[4]),
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
            if( lines[nLines].getName().empty() )
                lines[nLines].setName( plots[ii].getName(jj).c_str());
//            if( *lines[nLines].name == 0 )
//                strncpy( lines[nLines].name, plots[ii].getName(jj).c_str(), 15 );
        }
    }

   goodIsolineStructure( graphType );
   for(int i=0; i<4; i++ )
    {
        region[i] = sizeReg[i];
        part[i]   = sizePart[i];
    }
    b_color[0] = aAxisType[1];
    b_color[1] = aAxisType[2];
    b_color[2] = aAxisType[3];

    if( graphType == ISOLINES )
    {  setColorList();
       setScales();
     }
}

/*!
   The constructor
*/
GraphData::GraphData( TIArray<TPlot>& aPlots, const char * aTitle,
               const char *aXName, const char *aYName,
               TCStringArray line_names, int agraphType ):
        title(aTitle), axisType(5), graphType( agraphType ),
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
        int nLinN = plots[ii].getLinesNumber();
        for( int jj=0; jj<nLinN; jj++, nLines++ )
        {
          if( nLines < line_names.GetCount() )
            lines.Add( new TPlotLine( jj, nLinN, line_names[nLines].c_str() ) );
          else
            lines.Add( new TPlotLine( jj, nLinN, plots[ii].getName(jj).c_str() ) );
        }
    }
    goodIsolineStructure( graphType );

    if( graphType == ISOLINES )
    {
       setColorList();
       plots[0].getMaxMinIso(  minX, maxX, minY, maxY );
       setScales();
    }
    else
    { // set default min-max region

      plots[0].getMaxMin(  minX, maxX, minY, maxY );

      for( uint ii=1; ii<aPlots.GetCount(); ii++)
      {
         plots[ii].getMaxMin(  min1, max1, min2, max2 );
         if( minX > min1 ) minX = min1;
         if( maxX < max1 ) maxX = max1;
         if( minY > min2 ) minY = min2;
         if( maxY < max2 ) maxY = max2;
      }
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
        title(data.title), axisType(data.axisType), graphType(data.graphType),
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
    goodIsolineStructure( graphType );

    for(int i=0; i<4; i++ )
    {
        region[i] = data.region[i];
        part[i]   = data.part[i];
    }
    b_color[0] = data.b_color[0];
    b_color[1] = data.b_color[1];
    b_color[2] = data.b_color[2];

    if( graphType == ISOLINES )
      {
          scale.Clear();
          for( ii=0; ii<data.scale.GetCount(); ii++)
            scale.Add( new GColor( data.scale[ii] ));
      }
}


GraphData::~GraphData()
{}


// get array of points to draw one columns
// return number of points in column

int GraphData::getPointCol( int i, TIArray<FPoint>& pnts1 )
{
    int  nObjX, nObjY, nLn = 0;
    float x, y=0.;
    pnts1.Clear();

    for(uint ii=0; ii<1/*plots.GetCount()*/; ii++)
    {
       nObjX = plots[ii].getObjX();
       nObjY = plots[ii].getObjY();

       if( plots[ii].getfoString() == true )  /* put graph by column */
       {
          if( nObjX < 0 )
             x = i;
          else if( aObj[nObjX].IsEmpty(i,0) )
                 x = FLOAT_EMPTY;
               else
                 x = aObj[nObjX].Get(i,0);
       }
       else    /* put graph by gstring */
       {
           if( aObj[nObjX].IsEmpty(0,i) )
             x = FLOAT_EMPTY;
           else
            x = aObj[nObjX].Get(0,i);
        }

       pnts1.Add( new FPoint(x, y));
       for( int jj=0; jj<plots[ii].getLinesNumber(); jj++, nLn++ )
       {
         if( plots[ii].getfoString() == true )  /* put graph by column */
           {
             if( aObj[nObjY].IsEmpty(i,jj) )
               y += 0;//FLOAT_EMPTY;
             else
               y += aObj[nObjY].Get(i,jj);
           }
          else    /* put graph by gstring */
          {
            if( aObj[nObjY].IsEmpty(jj,i) )
              y += 0; //FLOAT_EMPTY;
            else
              y += aObj[nObjY].Get(jj,i);
          }
         pnts1.Add( new FPoint(x, y));
      }
    }
    return nLn;
}


bool GraphData::goodIsolineStructure( int aGraphType )
{
  if( aGraphType == ISOLINES &&
      !(plots.GetCount() >= 2 &&
        plots[0].getLinesNumber() >= 3 &&
        plots[1].getLinesNumber() >= 5
        ) )
   {
     gstring str = "Illegal sizes of graphic arrays:";
     if(plots.GetCount() >= 1)
             str += aObj[ plots[0].getObjY()].GetKeywd();
     else
             str = "Must be two objects in graphic";
     str += " and ";
     if(plots.GetCount() >= 2)
             str += aObj[ plots[1].getObjY()].GetKeywd();
     else
             str = "Must be two objects in graphic";
     Error( " ISOLINES graphic", str.c_str() );
    }
  return true;
}

// setup color scale for ISOLINE graphics
// get data from plots[1] object, colums 2, 3, 4

void GraphData::setColorList()
{

  int cred, cgreen, cblue;
  int nObjY = plots[1].getObjY();
  scale.Clear();

  for( int ii=0; ii< plots[1].getdX(); ii++ )
  {
     cred = (int)aObj[nObjY].Get(ii,2);
     cgreen = (int)aObj[nObjY].Get(ii,3);
     cblue = (int)aObj[nObjY].Get(ii,4);

     if( ( cred ==0 && cgreen == 0 && cblue == 0 ) ||
         cred < 0 || cgreen < 0 || cblue < 0 ||
         cred > 255 || cgreen > 255 || cblue > 255 )

       scale.Add( new GColor( true, ii, plots[1].getdX()) );
     else
       scale.Add( new GColor( cred, cgreen, cblue ));
  }

}

// put color scale for ISOLINE graphics to object plots[1]
//  colums 2, 3, 4

void GraphData::getColorList()
{

  int nObjY = plots[1].getObjY();
  for( int ii=0; ii< plots[1].getdX(); ii++ )
  {
     if( ii >= (int)scale.GetCount() )
       break;
     aObj[nObjY].Put( scale[ii].red, ii, 2 );
     aObj[nObjY].Put( scale[ii].green, ii, 3 );
     aObj[nObjY].Put( scale[ii].blue,  ii, 4 );
  }

}


// put sizes scale for ISOLINE graphics to object plots[1]
//  colums 0,1

void GraphData::setScales()
{
  float minZ, maxZ;

  plots[0].getMaxMinIsoZ(  minZ, maxZ );


  int nObjY = plots[1].getObjY();
  float delta = (maxZ - minZ ) / plots[1].getdX();

  if( aObj[nObjY].Get(0,0) !=0 || aObj[nObjY].Get(0,1) != 0. )
    return;

  for( int ii=0; ii< plots[1].getdX(); ii++ )
  {
     aObj[nObjY].Put( maxZ-delta*(ii+1), ii, 0 );
     aObj[nObjY].Put( maxZ-delta*ii, ii, 1 );
  }
  aObj[nObjY].Put( minZ, plots[1].getdX()-1, 0 );

}


// find color by value z in point ii
//  if z not in defined range, undefined color

int GraphData::getColorLine(int ii )
{
  float z1, z2, z;

  int jj, nObjY = plots[1].getObjY();

  z = aObj[plots[0].getObjY()].Get(ii,2);

  for(  jj=0; jj< plots[1].getdX(); jj++ )
  {
     z1 = aObj[nObjY].Get(jj,0);
     z2 = aObj[nObjY].Get(jj,1);

     if( z <= z2 && z >= z1)
       return jj;
  }
  return -1;

}


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
               TCStringArray line_names, int agraphType  )
{
   GraphData  data( aPlots, aTitle, aXName, aYName, line_names, agraphType );
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
