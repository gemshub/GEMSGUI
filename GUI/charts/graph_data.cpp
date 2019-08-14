//  This is JSONUI library+API (https://bitbucket.org/gems4/jsonui)
//
/// \file graph_data.cpp
/// Implementation of classes SeriesLineData and ChartData -
/// internal representation of graphic data
/// (Plotting system description)
//
// JSONUI is a C++ Qt5-based widget library and API aimed at implementing 
// the GUI for editing/viewing the structured data kept in a NoSQL database,
// with rendering documents to/from JSON/YAML/XML files, and importing/
// exporting data from/to arbitrary foreign formats (csv etc.). Graphical 
// presentation of tabular data (csv format fields) is also possible.
//
// Copyright (c) 2017 Svetlana Dmytriieva (svd@ciklum.com) and
//   Dmitrii Kulik (dmitrii.kulik@psi.ch)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU (Lesser) General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.
//
// JSONUI depends on the following open-source software products:
// JSONIO (https://bitbucket.org/gems4/jsonio); Qt5 (https://qt.io).
//

#include <cstdio>
#include <QJsonArray>
#include <QVector>
#include "graph_data.h"
#ifdef NO_JSONIO
#include "from_jsonio.h"
#else
#include "jsonio/jsondom.h"
#endif

using namespace std;

namespace jsonui {


QColor colorAt(const QColor &start, const QColor &end, qreal pos)
{
    Q_ASSERT(pos >= 0.0 && pos <= 1.0);
    qreal r = start.redF() + ((end.redF() - start.redF()) * pos);
    qreal g = start.greenF() + ((end.greenF() - start.greenF()) * pos);
    qreal b = start.blueF() + ((end.blueF() - start.blueF()) * pos);
    QColor c;
    c.setRgbF(r, g, b);
    return c;
}

//---------------------------------------------------------------------------
// SeriesLineData
//---------------------------------------------------------------------------

#ifndef NO_JSONIO
void SeriesLineData::toJsonNode( jsonio::JsonDom *object ) const
{
    object->appendInt( "gpt", markerShape );
    object->appendInt( "gps", markerSize );
    object->appendInt( "gls", penSize );
    object->appendInt( "glt", penStyle );
    object->appendInt( "gsp", spline );
    object->appendInt( "gndx", xcolumn );
    object->appendInt( "grd", red );
    object->appendInt( "ggr",  green );
    object->appendInt( "gbl",  blue );
    object->appendString( "gnm",  name );
}

void SeriesLineData::fromJsonNode( const jsonio::JsonDom *object )
{
    if(!object->findValue( "gpt", markerShape ) )
        markerShape = 0;
    if(!object->findValue( "gps", markerSize ) )
        markerSize = 4;
    if(!object->findValue( "gls", penSize ) )
        penSize = 2;
    if(!object->findValue( "glt", penStyle ) )
        penStyle = 0;
    if(!object->findValue( "gsp", spline ) )
        spline = 0;
    if(!object->findValue( "gndx", xcolumn ) )
        xcolumn = -1;
    if(!object->findValue( "grd", red ) )
        red = 25;
    if(!object->findValue( "ggr", green ) )
        green = 0;
    if(!object->findValue( "gbl", blue ) )
        blue = 150;
    name = "";
    object->findValue( "gnm", name );
}
#endif


void SeriesLineData::toJsonObject(QJsonObject& json) const
{
    json["gpt"] = markerShape;
    json["gps"] = markerSize;
    json["gls"] = penSize;
    json["glt"] = penStyle;
    json["gsp"] = spline;
    json["gndx"] = xcolumn;
    json["grd"] = red;
    json["ggr"] = green;
    json["gbl"] =  blue;
    json["gnm"] =  name.c_str();
}

void SeriesLineData::fromJsonObject(const QJsonObject& json)
{
    markerShape = json[ "gpt" ].toInt( 0 );
    markerSize = json[ "gps" ].toInt( 4 );
    penSize = json[ "gls" ].toInt( 2 );
    penStyle = json[ "glt" ].toInt( 0 );
    spline = json[ "gsp" ].toInt( 0 );
    xcolumn = json[ "gndx" ].toInt( -1 );
    red = json[ "grd" ].toInt( 25 );
    green = json[ "ggr" ].toInt( 0 );
    blue = json[ "gbl" ].toInt( 150 );
    name = json["gnm"].toString("").toStdString();
}

//---------------------------------------------------------------------------
// ChartData
//---------------------------------------------------------------------------

void ChartData::setGraphType(int newtype)
{
    graphType = newtype;
    for( auto& model: modelsdata)
        model->setGraphType(newtype);
}

void ChartData::setMinMaxRegion( double reg[4] )
{
    region[0] = reg[0];
    region[1] = reg[1];
    region[2] = reg[2];
    region[3] = reg[3];
    part[0] = reg[0]+(reg[1]-reg[0])/3;
    part[1] = reg[1]-(reg[1]-reg[0])/3;
    part[2] = reg[2]+(reg[3]-reg[2])/3;
    part[3] = reg[3]-(reg[3]-reg[2])/3;

}

#ifndef NO_JSONIO
void ChartData::toJsonNode( jsonio::JsonDom *object ) const
{
    int ii;
    object->appendString( "title", title );
    object->appendInt( "graphType", graphType );

    // define grid of plot
    object->appendInt( "axisTypeX", axisTypeX );
    object->appendInt( "axisTypeY", axisTypeY );
    object->appendString( "axisFont", axisFont.toString().toStdString() );
    object->appendString( "xName", xName );
    object->appendString( "yName", yName );

    auto arr = object->appendArray( "region");
    for( ii=0; ii<4; ii++)
        arr->appendDouble( to_string(ii), region[ii] );

    arr = object->appendArray("part");
    for( ii=0; ii<4; ii++)
        arr->appendDouble( to_string(ii), part[ii] );

    arr = object->appendArray( "b_color");
    for( ii=0; ii<3; ii++)
        arr->appendInt( to_string(ii), b_color[ii] );

    // define curves
    arr = object->appendArray( "lines");
    for(uint ii=0; ii<linesdata.size(); ii++)
    {
        auto obj = arr->appendObject( to_string(ii) );
        linesdata[ii].toJsonNode( obj );
    }

    arr = object->appendArray(  "models");
    for(uint ii=0; ii<modelsdata.size(); ii++)
    {
        auto obj = arr->appendObject( to_string(ii) );
        modelsdata[ii]->toJsonNode( obj );
    }
}

void ChartData::fromJsonNode( const jsonio::JsonDom *object )
{
    size_t ii;
    if( !object->findValue( "title", title ) )
        title = "title";
    if(!object->findValue( "graphType", graphType ) )
        graphType = LineChart;

    // define grid of plot
    if(!object->findValue( "axisTypeX", axisTypeX ) )
        axisTypeX = 5;
    if(!object->findValue( "axisTypeY", axisTypeY ) )
        axisTypeY = 5;
    string fntName;
    if( object->findValue( "axisFont", fntName ) )
        axisFont.fromString( fntName.c_str() );
    if( !object->findValue( "xName", xName ) )
        xName = "x";
    if( !object->findValue( "yName", yName ) )
        yName = "y";

    auto arr  = object->field( "region" );
    if(arr != nullptr)
        for( ii=0; ii<4; ii++)
            if(!arr->findValue( to_string(ii), region[ii] ) )
                region[ii] = 0;
    arr  = object->field( "part" );
    if(arr != nullptr)
        for( ii=0; ii<4; ii++)
            if(!arr->findValue( to_string(ii), part[ii] ) )
                part[ii] = 0;
    arr  = object->field( "b_color" );
    if(arr != nullptr)
        for( ii=0; ii<3; ii++)
            if(!arr->findValue( to_string(ii), b_color[ii] ) )
                b_color[ii] = 255;

    linesdata.clear();
    SeriesLineData linebuf;
    arr  = object->field( "lines" );
    if(arr != nullptr)
        for(ii=0; ii<arr->getChildrenCount(); ii++)
        {
            linebuf.fromJsonNode( arr->getChild(ii) );
            linesdata.push_back(  linebuf );
        }

    arr  = object->field( "models" );
    if(arr != nullptr)
        for(ii=0; ii<arr->getChildrenCount(); ii++)
        {
            if( ii >= modelsdata.size())
                break;
            modelsdata[ii]->fromJsonNode( arr->getChild(ii) );
        }
    // refresh model type
    setGraphType(graphType);
}
#endif

void ChartData::toJsonObject(QJsonObject& json) const
{
    json[ "title" ] =  title.c_str();
    json[ "graphType" ] = graphType;
    json[ "axisTypeX" ] = axisTypeX;
    json[ "axisTypeY" ] = axisTypeY;
    json[ "axisFont" ] = axisFont.toString();
    json[ "xName" ] =  xName.c_str();
    json[ "yName" ] = yName.c_str();

    QJsonArray regArray;
    QJsonArray partArray;
    for(uint ii=0; ii<4; ii++) {
        regArray.append(region[ii]);
        partArray.append(part[ii]);
    }
    json["region"] = regArray;
    json["part"] = partArray;

    QJsonArray colorArray;
    for(uint ii=0; ii<3; ii++)
        colorArray.append(b_color[ii]);
    json["b_color"] = colorArray;

    QJsonArray linesArray;
    for(uint ii=0; ii<linesdata.size(); ii++)
    {
        QJsonObject lnObject;
        linesdata[ii].toJsonObject(lnObject);
        linesArray.append(lnObject);
    }
    json["lines"] = linesArray;

    QJsonArray modelArray;
    for(uint ii=0; ii<modelsdata.size(); ii++)
    {
        QJsonObject lnObject;
        modelsdata[ii]->toJsonObject(lnObject);
        modelArray.append(lnObject);
    }
    json["models"] = modelArray;
}

void ChartData::fromJsonObject(const QJsonObject& json)
{
    title = json[ "title" ].toString( "Graph title" ).toStdString();
    graphType = json[ "graphType" ].toInt( LineChart );
    axisTypeX = json[ "axisTypeX" ].toInt( 5 );
    axisTypeY = json[ "axisTypeY" ].toInt( 5 );
    QString fntname = json[ "axisFont" ].toString( "Sans Serif" );
    axisFont.fromString( fntname );
    xName = json[ "xName" ].toString( "x" ).toStdString();
    yName = json[ "yName" ].toString( "y" ).toStdString();

    QJsonArray regArray = json["region"].toArray();
    QJsonArray partArray = json["part"].toArray();
    if( regArray.size() > 3 && partArray.size() > 3 )
        for(size_t ii=0; ii<4; ii++)
        {
            region[ii] = regArray[ii].toDouble();
            part[ii] = partArray[ii].toDouble();
        }

    QJsonArray colorArray = json["b_color"].toArray();
    if( colorArray.size() > 2 )
        for(size_t ii=0; ii<3; ii++)
            b_color[ii] = colorArray[ii].toInt();

    linesdata.clear();
    SeriesLineData linebuf;
    QJsonArray linesArray = json["lines"].toArray();
    for(int ii=0; ii<linesArray.size(); ii++)
    {
        QJsonObject lnObject = linesArray[ii].toObject();
        linebuf.fromJsonObject(lnObject);
        linesdata.push_back(  linebuf );
    }

    linesArray = json["models"].toArray();
    for(int ii=0; ii<linesArray.size(); ii++)
    {
        if( ii >= static_cast<int>(modelsdata.size()) )
            break;
        QJsonObject lnObject = linesArray[ii].toObject();
        modelsdata[ii]->fromJsonObject(lnObject);
    }
    // refresh model type
    setGraphType(graphType);
}

void ChartData::updateXSelections()
{
    size_t defined_lines = linesdata.size();
    size_t nLines = 0;

    for( size_t ii=0; ii<modelsdata.size(); ii++)
    {
        auto numxColms = modelsdata[ii]->getAbscissaNumber();
        auto nLinN =  modelsdata[ii]->getSeriesNumber();
        for( size_t jj=0; jj<nLinN; jj++, nLines++ )
        {
            jsonio::jsonioErrIf( nLines >= defined_lines, "updateXSelections", "Error into graph data.." );
            if( linesdata[nLines].getXColumn() >= numxColms )
                linesdata[nLines].setXColumn( -1 );
        }
    }
}

void ChartData::updateYSelections( bool updateNames )
{
    size_t defined_lines = linesdata.size();
    size_t nLines = 0;

    for( size_t ii=0; ii<modelsdata.size(); ii++)
    {
        auto nLinN =  modelsdata[ii]->getSeriesNumber();
        for( uint jj=0; jj<nLinN; jj++, nLines++ )
        {
            if( nLines >= defined_lines )
                linesdata.push_back( SeriesLineData( jj, nLinN, modelsdata[ii]->getName(nLines)  ) );
            else
                if( updateNames )
                    linesdata[nLines].setName( modelsdata[ii]->getName(nLines) );
        }
    }
    linesdata.resize(nLines);
}



} // namespace jsonui

//--------------------- End of graph_data.cpp ---------------------------
