//-------------------------------------------------------------------
// $Id: graph.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of TPlotLine, TPlot, GraphData and GraphWindow classes
// (Plotting system description)
//
// Copyright (C) 1996-2001  S.Dmytriyeva, A.Rysin
// Uses  string class (C) A.Rysin 1999
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

#include <cstdio>
#include "GraphDialog.h"
#include "GemsMainWindow.h"
#include "gdatastream.h"
#include "v_ejdb.h"
#include "v_object.h"

//---------------------------------------------------------------------------
// TPlotLine
//---------------------------------------------------------------------------

void TPlotLine::read(GemDataStream& stream)
{
    stream >> type;
    stream >> sizes;
    stream >> ndxX;
    stream >> red >> green >> blue;
    stream.readArray(name, sizeof(name));
}

void TPlotLine::write(GemDataStream& stream)
{
    stream << type;
    stream << sizes;
    stream << ndxX;
    stream << red << green << blue;
    stream.writeArray(name, sizeof(name));
}

void TPlotLine::read(fstream& stream)
{
    stream >> type;
    stream >> sizes;
    stream >> ndxX;
    stream >> red >> green >> blue;
    stream.get( name, 16, '\n');
 }

void TPlotLine::write(fstream& stream)
{
    stream << type << " ";
    stream << sizes << " ";
    stream << ndxX << " ";
    stream << red << " " << green << " " << blue << " ";
    stream << name;
    stream << endl;
}

/*QJsonObject TPlotLine::toJsonObject()
{
   QJsonObject obj;
   obj.insert("gtp", QJsonValue(type) );
   obj.insert("gsz", QJsonValue(sizes) );
   obj.insert("gndx", QJsonValue(ndxX) );
   obj.insert("grd", QJsonValue(red) );
   obj.insert("ggr",  QJsonValue(green) );
   obj.insert("gbl",  QJsonValue(blue) );
   obj.insert("gnm",  QJsonValue(QString(name)) );
   return obj;
}

void TPlotLine::fromJsonObject( QJsonObject obj )
{
    type = obj.value("gtp").toDouble(1);
    sizes = obj.value("gsz").toDouble(4);
    ndxX = obj.value("gndx").toDouble(1);
    red = obj.value("grd").toDouble(0);
    green = obj.value("ggr").toDouble(0);
    blue = obj.value("gbl").toDouble(0);
    memcpy( name, obj.value("gnm").toString().toUtf8().data(), 15);
}
*/

void TPlotLine::toBsonObject( bson *obj )
{
   bson_append_int( obj, "gtp", type );
   bson_append_int( obj, "gsz", sizes );
   bson_append_int( obj, "gndx", ndxX );
   bson_append_int( obj, "grd", red );
   bson_append_int( obj, "ggr",  green );
   bson_append_int( obj, "gbl",  blue );
   bson_append_string( obj, "gnm",  name );
}

void TPlotLine::fromBsonObject( const char *obj )
{
    if(!bson_find_value( obj, "gtp", type ) )
        Error( "E011BSon: ", "Undefined gtp.");
    if(!bson_find_value( obj, "gsz", sizes ) )
        Error( "E012BSon: ", "Undefined gsz.");
    if(!bson_find_value( obj, "gndx", ndxX ) )
        Error( "E013BSon: ", "Undefined gndx.");
    if(!bson_find_value( obj, "grd", red ) )
        Error( "E014BSon: ", "Undefined grd.");
    if(!bson_find_value( obj, "ggr", green ) )
        Error( "E01501BSon: ", "Undefined ggr.");
    if(!bson_find_value( obj, "gbl", blue ) )
        Error( "E016BSon: ", "Undefined gbl.");
    string valStr;
    bson_find_string( obj, "gnm", valStr );
    memcpy( name, valStr.c_str(), 15);
}

//---------------------------------------------------------------------------
// TPlot
//---------------------------------------------------------------------------

TPlot::TPlot( int aObjX, int aObjY ):
     nObjX(aObjX), nObjY(aObjY), first(0)
{
    int dNy, dMy, dY;

    foString = ( aObjX < 0 || aObj[aObjX].GetN() >= 1 );

    dNy = aObj[aObjY].GetN();
    dMy = aObj[aObjY].GetM();

    if( foString == true )
    {
        dY=dNy;
        dY1=dMy;

        if( aObjX < 0 ) // numbers
        {    dX = dNy;  nAbs = 1;   }
        else
        {    dX = aObj[aObjX].GetN();
             nAbs = aObj[aObjX].GetM();
        }

    } // put graph by column
    else
    {
        dY=dMy;
        dY1=dNy;
        dX = aObj[aObjX].GetM();
        nAbs = aObj[aObjX].GetN();
    } // put graph by string

    ErrorIf( dX!=dY, "Graphics demo", "Invalid size of objects.");
}

TPlot::TPlot( const TPlot& plt, int aFirst ):
        nObjX(plt.nObjX), nObjY(plt.nObjY), first(aFirst)
{
    foString = plt.foString;
    dX =plt.dX;
    nAbs = plt.nAbs;
    dY1 =plt.dY1;
}

TPlot::~TPlot()
{}

string TPlot::getName( int ii )
{
    char s[40];
    sprintf(s, "%s[%u]",aObj[nObjY].GetKeywd(), ii);
    return string(s);
}

// get point to draw one line
QPointF TPlot::getPoint( int line, int number, int ndxAbs )
{
    double x, y;

    if(ndxAbs >= nAbs )
        ndxAbs = 0;

    if( ndxAbs < 0 )
        return QPointF( DOUBLE_EMPTY, DOUBLE_EMPTY );

    if( foString == true )  // put graph by column
    {
        if( nObjX < 0 )
            x = number;
        else
            x = aObj[nObjX].GetEmpty( number, ndxAbs );

        y = aObj[nObjY].GetEmpty( number, line );
    }
    else    // put graph by string
    {
       x = aObj[nObjX].GetEmpty( ndxAbs, number );
       y = aObj[nObjY].GetEmpty( line, number );
    }

    return QPointF( x, y);
}

// Build array of points to draw one line
// return number of points in line
int TPlot::getPointLine( int j, QVector<QPointF>& points, int ndxAbs )
{
    QPointF pos;
    points.clear();

    for(int  ii=0; ii<dX; ii++ )
    {   pos = getPoint( j, ii, ndxAbs );
        //if( pos.x() == DOUBLE_EMPTY || pos.y() == DOUBLE_EMPTY )
        //  continue;
        points.append(pos);
    }
    return dX;
}

// Build array of points to draw one line
// return number of points in line
int TPlot::getPointTube( int line, QVector<QwtIntervalSample>& points, int ndxAbs )
{
    QPointF pos;
    double miny, maxy;

    for(int  ii=0; ii<dX; ii++ )
    {
        pos = getPoint( line, ii, ndxAbs );
        miny = points[ii].interval.maxValue();

        if( pos.x() == DOUBLE_EMPTY || pos.y() == DOUBLE_EMPTY )
          maxy = miny;
        else
          maxy = miny+pos.y();
        points[ii].interval.setMinValue( miny );
        points[ii].interval.setMaxValue( maxy );
    }
    return dX;
}


// Find min and max values x,y for one curve line
void TPlot::getMaxMinLine( QPointF& min, QPointF& max, int line, int ndxAbs )
{
    QPointF point;
    int jj = line;

    min = getPoint( jj, 0, ndxAbs );
    max = min;
    for( int ii =0; ii<dX; ii++)
    {
           point = getPoint( jj, ii, ndxAbs );
           if( point.x() == DOUBLE_EMPTY || point.y() == DOUBLE_EMPTY )
             continue;
           if( min.x() > point.x() || min.x() == DOUBLE_EMPTY  )
               min.setX( point.x() );
           if( max.x() < point.x() || max.x() == DOUBLE_EMPTY )
               max.setX( point.x() );
           if( min.y() > point.y() || min.y() == DOUBLE_EMPTY )
                min.setY( point.y() );
           if( max.y() < point.y() || max.y() == DOUBLE_EMPTY )
               max.setY( point.y() );
    }
}


// must be changed ---------------------------------------------------------

/*void TPlot::getMaxMinIso( QPointF& min, QPointF& max )
{
    double x,y;
    x = aObj[nObjY].Get(0,0);
    min.setX( x );
    max.setX( x );
    y = aObj[nObjY].Get(0,1);
    min.setY( y );
    max.setY( y );

    for( int ii =0; ii<dX; ii++)
    {
         x = aObj[nObjY].Get(ii,0);
         y = aObj[nObjY].Get(ii,1);
         if( min.x() > x ) min.setX( x );
         if( max.x() < x ) max.setX( x );
         if( min.y() > y ) min.setY( y );
         if( max.y() < y ) max.setY( y );
   }
}
*/

//---------------------------------------------------------------------------
// GraphData
//---------------------------------------------------------------------------

/// The constructor
GraphData::GraphData( const vector<TPlot>& aPlots, const char * aTitle,
               float *sizeReg,  float * sizePart,
               TPlotLine* aLinesDesc, short *aAxisType,
               const char *aXName, const char *aYName ):
        title(aTitle), axisTypeX(aAxisType[0]), axisTypeY(aAxisType[5]),
        graphType(aAxisType[4])//,  isBackgr_color(true)
{
    int ii;
    int jj, nLines;

    xName = string( aXName, 0, 9);
    yName = string( aYName, 0, 9);

    // Insert Plots and curves description
    plots.clear();
    lines.clear();
    for( ii=0, nLines=0; ii<aPlots.size(); ii++)
    {
        plots.push_back( TPlot(aPlots[ii], nLines ));
        for( jj=0; jj<plots[ii].getLinesNumber(); jj++, nLines++ )
        {
            if(aLinesDesc )
                lines.push_back( TPlotLine( aLinesDesc[nLines] ) );
            else
                lines.push_back( TPlotLine( ii, plots[ii].getLinesNumber(), plots[ii].getName(jj).c_str() ) );

            if( lines[nLines].getName().empty() )
                lines[nLines].setName( plots[ii].getName(jj).c_str());
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

   if( graphType == ISOLINES )
   {
        goodIsolineStructure( graphType );
        setColorList();
        setScales();
   }
}

/// The constructor
GraphData::GraphData( GraphData& data ):
        title(data.title), axisTypeX(data.axisTypeX), axisTypeY(data.axisTypeY),
        graphType(data.graphType), xName(data.xName), yName(data.yName)
        //isBackgr_color(data.isBackgr_color)
{
    int ii;
    int jj, nLines;

    // Insert Plots and curves description
    plots.clear();
    lines.clear();
    for( ii=0, nLines=0; ii<data.plots.size(); ii++)
    {
        plots.push_back( TPlot(data.plots[ii], nLines ));
        for( jj=0; jj<plots[ii].getLinesNumber(); jj++, nLines++ )
             lines.push_back( TPlotLine( data.lines[nLines] ) );
    }

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
         goodIsolineStructure( graphType );
         scale.clear();
         for( ii=0; ii<data.scale.size(); ii++)
            scale.push_back( QColor( data.scale[ii] ));
    }
 }

/// The constructor
GraphData::GraphData( const vector<TPlot>&  aPlots, const char * aTitle,
               const char *aXName, const char *aYName,
               const vector<string>& line_names, int agraphType ):
        title(aTitle), axisTypeX(4), axisTypeY(4), graphType( agraphType )
        // isBackgr_color(false)
{
    int ii;
    int jj, nLines, ndxAbs;
    double minX, maxX, minY, maxY;
    QPointF min, max;

    xName = string( aXName, 0, 9);
    yName = string( aYName, 0, 9);

    // Insert Plots and curves description
    plots.clear();
    lines.clear();
    for( ii=0, nLines=0; ii<aPlots.size(); ii++)
    {
        plots.push_back( TPlot(aPlots[ii], nLines ));
        int nLinN = plots[ii].getLinesNumber();
        for( int jj=0; jj<nLinN; jj++, nLines++ )
        {
           if( jj < plots[ii].getNAbs() )
             ndxAbs = jj;
          else  ndxAbs = 0;
          if( nLines < line_names.size() )
            lines.push_back( TPlotLine( jj, nLinN, line_names[nLines].c_str(), 0, 4,2, ndxAbs ) );
          else
            lines.push_back( TPlotLine( jj, nLinN, plots[ii].getName(jj).c_str(), 0, 4,2, ndxAbs  ) );
        }
    }

    if( graphType == ISOLINES )
    {
       goodIsolineStructure( graphType );
       //plots[0].getMaxMinIso(  min, max );
       //       minX = min.x();
       //       maxX = max.x();
       plots[0].getMaxMinLine( min, max, 0, 0 );
       minX = min.y();
       maxX = max.y();
       plots[0].getMaxMinLine( min, max, 1, 0 );
       minY = min.y();
       maxY = max.y();
       setColorList();
       setScales();
    }
    else
    { // set default min-max region
      plots[0].getMaxMinLine(  min, max, 0, getIndex(0) );
      minX = min.x();
      maxX = max.x();
      minY = min.y();
      maxY = max.y();

      for( ii=0, nLines=0; ii<aPlots.size(); ii++)
       for( jj=0; jj<plots[ii].getLinesNumber(); jj++, nLines++ )
       {  plots[ii].getMaxMinLine( min, max, jj, getIndex(nLines) );
         if( minX > min.x() ) minX = min.x();
         if( maxX < max.x() ) maxX = max.x();
         if( minY > min.y() ) minY = min.y();
         if( maxY < max.y() ) maxY = max.y();
       }
    }

   if( maxX == minX ) maxX += 1.;
   if( maxY == minY ) maxY += 1.;

   adjustAxis(minX, maxX, axisTypeX);
   adjustAxis(minY, maxY, axisTypeY);

   region[0] = minX;
   region[1] = maxX;
   region[2] = minY;
   region[3] = maxY;
   part[0] = minX+(maxX-minX)/3;
   part[1] = maxX-(maxX-minX)/3;
   part[2] = minY+(maxY-minY)/3;
   part[3] = maxY-(maxY-minY)/3;

   QColor color(Qt::white);
   b_color[0] = color.red();
   b_color[1] = color.green();
   b_color[2] = color.blue();

}

GraphData::~GraphData()
{}

void GraphData::getIndexes( QVector<int>& first, QVector<int>& maxXndx )
{
    for(int ii=0; ii<plots.size(); ii++)
    {
        first.append( plots[ii].getFirstLine() );
        maxXndx.append( plots[ii].getNAbs() );
    }

}

void  GraphData::adjustAxis( double& min, double& max, int& numTicks)
{
    const int minTicks = 4;
    double grossStep = (max-min)/minTicks;
    double step = pow(10. , floor(log10(grossStep)));

    if(5*step < grossStep )
    {
        step *=5;
    }
    else if(2*step < grossStep )
           step *=2;

    numTicks = int(ceil(max/step)-floor(min/step));
    if( numTicks < minTicks)
       numTicks = minTicks;
    min = floor(min/step)*step;
    max = ceil(max/step)*step;
}

// must be changed -------------------------------------------

bool GraphData::goodIsolineStructure( int aGraphType )
{
  if( aGraphType == ISOLINES &&
      !(plots.size() >= 2 &&
        plots[0].getLinesNumber() >= 3 &&
        plots[1].getLinesNumber() >= 5
        ) )
   {
     string str = "Invalid sizes of graphic arrays:";
     if(plots.size() >= 1)
             str += aObj[ plots[0].getObjY()].GetKeywd();
     else
             str = "Must be two objects in graphic";
     str += " and ";
     if(plots.size() >= 2)
             str += aObj[ plots[1].getObjY()].GetKeywd();
     else
             str = "Must be two objects in graphic";
     Error( " ISOLINES graphic", str );
    }
  return true;
}

// setup color scale for ISOLINE graphics
// get data from plots[1] object, colums 2, 3, 4
void GraphData::setColorList()
{
  int cred, cgreen, cblue;
  int nObjY = plots[1].getObjY();
  scale.clear();

  for( int ii=0; ii< plots[1].getdX(); ii++ )
  {
     cred = (int)aObj[nObjY].Get(ii,2);
     cgreen = (int)aObj[nObjY].Get(ii,3);
     cblue = (int)aObj[nObjY].Get(ii,4);

     if( ( cred ==0 && cgreen == 0 && cblue == 0 ) ||
         cred < 0 || cgreen < 0 || cblue < 0 ||
         cred > 255 || cgreen > 255 || cblue > 255 )
     { //default
         if(!ii)
            scale.push_back( QColor( Qt::red ) );
         else if( ii == plots[1].getdX()-1 )
               scale.push_back( QColor( Qt::darkGreen ) );
              else
                scale.push_back( QColor( Qt::yellow ) );
     }
     else
       scale.push_back( QColor( cred, cgreen, cblue ));
  }

}

double GraphData::getValueIsoline(int ii)
{
    return aObj[plots[1].getObjY()].Get(ii,0);
}

void GraphData::setValueIsoline(double val, int ii)
{
    aObj[plots[1].getObjY()].Put( val, ii,0);
}

// put color scale for ISOLINE graphics to object plots[1]
//  colums 2, 3, 4
void GraphData::getColorList()
{
  int nObjY = plots[1].getObjY();
  for( int ii=0; ii< plots[1].getdX(); ii++ )
  {
     if( ii >= (int)scale.size() )
       break;
     aObj[nObjY].Put( scale[ii].red(), ii, 2 );
     aObj[nObjY].Put( scale[ii].green(), ii, 3 );
     aObj[nObjY].Put( scale[ii].blue(),  ii, 4 );
  }
}

// put sizes scale for ISOLINE graphics to object plots[1]
//  colum 0
void GraphData::setScales()
{
  int nObjY = plots[1].getObjY();
  double delta = 1. / (plots[1].getdX()-1);

  for( int ii=0; ii< plots[1].getdX()-1; ii++ )
  {
    if( aObj[nObjY].Get(ii,0) == 0 )
     aObj[nObjY].Put( 1.-delta*(ii), ii, 0 );
  }
  aObj[nObjY].Put( 0., plots[1].getdX()-1, 0 );
}


//---------------------------------------------------------------------------

///   The constructor
GraphWindow::GraphWindow(TCModule *pmodule,
               const vector<TPlot>& aPlots,
               const char * aTitle,
               float *sizeReg,  float * sizePart,
               TPlotLine* aLinesDesc, short *aAxisType,
               const char *aXName, const char *aYName )
{

       GraphData data(aPlots, aTitle, sizeReg, sizePart,
                aLinesDesc, aAxisType, aXName, aYName);
       graph_dlg = new GraphDialog( pmodule, data );
       pVisorImp->openMdiChild( graph_dlg );
}

///   The constructor
GraphWindow::GraphWindow( TCModule *pmodule,
                         const vector<TPlot>& aPlots, const char * title,
                      const char *aXName, const char *aYName,
                      const vector<string>& line_names,
                      int agraphType  )
{
   GraphData  data( aPlots, title, aXName, aYName, line_names, agraphType );
   graph_dlg = new GraphDialog(pmodule, data);
   pVisorImp->openMdiChild( graph_dlg );
}

GraphWindow::~GraphWindow()
{
}

void GraphWindow::AddPoint( int nPlot, int nPoint )
{
   graph_dlg->AddPoint( nPlot, nPoint );
}

void GraphWindow::Show( const char * capAdd )
{
    if( graph_dlg )
     graph_dlg->ShowNew(capAdd);
}

GraphData *GraphWindow::getGraphData() const
{
  return &graph_dlg->gr_data;
}

//--------------------- End of graph.cpp ---------------------------
