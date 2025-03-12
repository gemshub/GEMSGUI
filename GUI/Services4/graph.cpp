//-------------------------------------------------------------------
// $Id: graph.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of TPlotLine, TPlot, GraphData and GraphWindow classes
// (Plotting system description)
//
// Copyright (C) 1996-2001  S.Dmytriyeva, A.Rysin
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

#include <cstdio>
#include <limits>
#include <QJsonObject>
#include "GEMS3K/gdatastream.h"
#include "GEMS3K/v_detail.h"
#include "graph.h"
#include "GemsMainWindow.h"

void helpWin( const std::string& name, const std::string& item );

void helpWin( const std::string& name, const std::string& item )
{
    pVisorImp->OpenHelp( name.c_str(), item.c_str() );
}

//---------------------------------------------------------------------------
// TPlotLine
//---------------------------------------------------------------------------

void TPlotLine::toJsonObject( QJsonObject& obj ) const
{
    obj[ "gtp" ] = type;
    obj[ "gsz" ] = sizes;
    obj[ "gndx"] = ndxX;
    obj[ "grd"] = red;
    obj[ "ggr"] = green;
    obj[ "gbl"] = blue;
    obj[ "gnm"] =  name;
}

void TPlotLine::fromJsonObject(const QJsonObject &obj)
{
    type = obj[ "gtp" ].toInt( 4 );
    sizes = obj[ "gsz" ].toInt( 2 );
    ndxX = obj[ "gndx" ].toInt( -1 );
    red = obj[ "grd" ].toInt( 25 );
    green = obj[ "ggr" ].toInt( 0 );
    blue = obj[ "gbl" ].toInt( 150 );
    auto name_str = obj["gnm"].toString("line").toStdString();
    memcpy( name, name_str.c_str(), 15);
}

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
    char buf[256];
    stream >> type;
    stream >> sizes;
    stream >> ndxX;
    stream >> red >> green >> blue;
    stream.get( buf, 255, '\n');
    memcpy( name, ( buf[0]==' '? buf+1: buf ), 15);
    name[15] = '\0';
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

//---------------------------------------------------------------------------
// TPlot
//---------------------------------------------------------------------------

TPlot::TPlot( int aObjX, int aObjY ):
    nObjX(aObjX), nObjY(aObjY), first(0)
{
    int dNy, dMy, dY;

    foString = ( aObjX < 0 || aObj[aObjX]->GetN() >= 1 );

    dNy = aObj[aObjY]->GetN();
    dMy = aObj[aObjY]->GetM();

    if( foString == true )
    {
        dY=dNy;
        dY1=dMy;

        if( aObjX < 0 ) // numbers
        {    dX = dNy;  nAbs = 1;   }
        else
        {    dX = aObj[aObjX]->GetN();
            nAbs = aObj[aObjX]->GetM();
        }

    } // put graph by column
    else
    {
        dY=dMy;
        dY1=dNy;
        dX = aObj[aObjX]->GetM();
        nAbs = aObj[aObjX]->GetN();
    } // put graph by string

    ErrorIf( dX!=dY, "Graphics demo", "Invalid size of objects.");
}

TPlot::TPlot( TPlot& plt, int aFirst ):
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
    char s[80];
    sprintf(s, "%s[%u]", aObj[nObjY]->GetKeywd(), ii);
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
            x = aObj[nObjX]->GetEmpty( number, ndxAbs );

        y = aObj[nObjY]->GetEmpty( number, line );
    }
    else    // put graph by string
    {
        x = aObj[nObjX]->GetEmpty( ndxAbs, number );
        y = aObj[nObjY]->GetEmpty( line, number );
    }

    return QPointF( x, y);
}

double TPlot::getValue(int row, int col) const
{
    double value = DOUBLE_EMPTY;

    if( foString == true )  // put graph by column
    {
        if( col < getNAbs() )
            value = aObj[getObjX()]->GetEmpty( row, col );
        else
            value = aObj[getObjY()]->GetEmpty( row, col-getNAbs() );
    }
    else    // put graph by string
    {
        if( col < getNAbs() )
            value = aObj[getObjX()]->GetEmpty( col, row );
        else
            value = aObj[getObjY()]->GetEmpty( col-getNAbs(), row );
    }

#ifndef OVERFLOW_EXCEPT
    if( IsDoubleEmpty( value ) && row > 0  )
        return std::numeric_limits<double>::lowest();
    else
#endif
        return value;
}

QString TPlot::getColumnName(int col) const
{
    QString value;

    if( col < getNAbs() )
    {
        if( getNAbs() > 1 )
            value = QString("%1[%2]").arg(aObj[getObjX()]->GetKeywd()).arg(col);
        else
            value = QString("%1").arg(aObj[getObjX()]->GetKeywd());
    }
    else
        value = QString("%1[%2]").arg(aObj[getObjY()]->GetKeywd()).arg(col-getNAbs() );
    return value;
}

std::vector<int> TPlot::xColumns() const
{
   std::vector<int> xvec;
   for( int ii=0; ii<getNAbs(); ++ii )
     xvec.push_back(ii);
   return xvec;
}

std::vector<int> TPlot::yColumns() const
{
    std::vector<int> xvec;
    for( int ii=0; ii<getLinesNumber(); ++ii )
      xvec.push_back(ii+getNAbs());
    return xvec;
}


// Find min and max values x,y for one curve line
void TPlot::getMaxMinLine( double& minX, double& maxX, double& minY, double& maxY,
                           int line, int ndxAbs )
{
    QPointF point;
    int jj = line;

    for( int ii =0; ii<dX; ii++)
    {
        point = getPoint( jj, ii, ndxAbs );
        if( IsDoubleEmpty( point.x()) || IsDoubleEmpty( point.y() ) )
            continue;
        if( minX > point.x() || IsDoubleEmpty( minX) )
            minX = point.x();
        if( maxX < point.x() || IsDoubleEmpty( maxX ) )
            maxX = point.x();
        if( minY > point.y() || IsDoubleEmpty( minY ) )
            minY = point.y();
        if( maxY < point.y() || IsDoubleEmpty( maxY ) )
            maxY = point.y();
    }
}


//--------------------- End of graph.cpp ---------------------------
