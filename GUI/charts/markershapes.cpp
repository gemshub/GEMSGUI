//  This is JSONUI library+API (https://bitbucket.org/gems4/jsonui)
//
/// \file markershapes.cpp
/// Implementation of the MarkerShapes class to define the shape of the marker
///  used to render the points in the series
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

#include <cmath>
#include <QIcon>
#include <QImage>
#include <QPen>
#include <QPainter>
#include <QFont>
#include <QtCore/QtMath>
#include "markershapes.h"
#include "graph_data.h"

const double Pi = 3.14159;

namespace jsonui {

QImage markerShapeImage( const SeriesLineData& linedata )
{
    QPainterPath imagePath = shapes().shape( static_cast<size_t>(linedata.getMarkerShape()) );

    QImage image( 32, 32, QImage::Format_ARGB32);
    image.fill(Qt::transparent);

    QPainter painter(&image);
    painter.setRenderHint( QPainter::Antialiasing );
    QPen pen = QPen( linedata.getColor(), 3 );
    pen.setJoinStyle( Qt::MiterJoin );
    painter.setPen( pen );

    if( static_cast<size_t>(linedata.getMarkerShape()) < shapes().size() )
        painter.setBrush( linedata.getColor() );
    painter.drawPath(imagePath);

    return image;
}


QImage textImage( const QFont& font, const QString& text )
{
    QFontMetrics fm(font);
    int pixelsWide = fm.horizontalAdvance(text)+2;
    int pixelsHigh = fm.height();
    int size, ascent = 0;
    if( pixelsWide > pixelsHigh )
    {
      size  =  pixelsWide;
      ascent =fm.ascent()+(pixelsWide-pixelsHigh)/2;
    }
    else
    {
      size  =  pixelsHigh;
      ascent =pixelsHigh-fm.descent();
    }
    QPainterPath textPath;
    textPath.addText(1, ascent, font, text );

    QImage image(size, size, QImage::Format_ARGB32);
    image.fill(Qt::transparent);

    QPainter painter(&image);
    painter.setRenderHint( QPainter::Antialiasing );
    painter.setPen( QPen( QColor("#323232"), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin) );
    painter.setBrush( QColor("#323232") );
    painter.drawPath(textPath);

    //image.save("TextImage.png");
    return image;
}


QIcon markerShapeIcon( const SeriesLineData& linedata )
{
    QIcon icon;
    int dsize = 32;
    int size = dsize/2;

    QPixmap pic(dsize, dsize);
    pic.fill(QColor(0, 0, 0, 0));
    QPainter painter(&pic);

    QRect rect(0,0,dsize, dsize);
    painter.setPen( QPen( linedata.getColor(), 2 ) );
    painter.drawImage( QRectF( QPointF(size/2, size/2), QSizeF(size,size)),
                       markerShapeImage( linedata ));

    if(linedata.getPenSize() > 0 )
    {
        QPoint center = rect.center();
        painter.drawLine( rect.x(), center.y(), center.x()-size/2, center.y());
        painter.drawLine( center.x()+size/2, center.y(), rect.width(), center.y());
    }
    icon.addPixmap(pic);
    return icon;
}

// If you know size then you can simply use:
// QPixmap pixmap = icon.pixmap(requiredImageSize);
// QPixmap pixmap = icon.pixmap(icon.actualSize(QSize(32, 32)));

void getLinePen( QPen& pen, const SeriesLineData& linedata  )
{
    pen.setColor(linedata.getColor() );
    pen.setWidth(linedata.getPenSize());
    Qt::PenStyle style = static_cast<Qt::PenStyle>(linedata.getPenStyle());
    pen.setStyle(style);
}


MarkerShapes::MarkerShapes()
{
    QFont timesFont("Times", 40);
    timesFont.setStyleStrategy(QFont::ForceOutline);

    // MsCircle,        ///< Circle
    QPainterPath circlePath;
    circlePath.addEllipse(2, 2, 28, 28);
    circlePath.closeSubpath();
    markers.push_back(circlePath);

    // MsRectangle,     ///< Rectangle
    QPainterPath rectPath;
    rectPath.addRect(2,2,28,28);
    rectPath.closeSubpath();
    markers.push_back(rectPath);

    // MsDiamond,       ///< Diamond
    QPainterPath diamondPath;
    diamondPath.moveTo(2, 16.0);
    diamondPath.lineTo(16.0, 30.0);
    diamondPath.lineTo(30.0, 16.0);
    diamondPath.lineTo(16.0, 2.0);
    diamondPath.closeSubpath();
    markers.push_back(diamondPath);

    //MsDTriangle,     ///< Triangle pointing downwards
    QPainterPath dTrianglePath;
    dTrianglePath.addPolygon(trianglePolygon( TriangleDown, QSize(30,30) ));
    dTrianglePath.closeSubpath();
    markers.push_back(dTrianglePath);
    //MsUTriangle,     ///< Triangle pointing upwards
    QPainterPath uTrianglePath;
    uTrianglePath.addPolygon(trianglePolygon( TriangleUp, QSize(30,30) ));
    uTrianglePath.closeSubpath();
    markers.push_back(uTrianglePath);
    //MsLTriangle,     ///< Triangle pointing left
    QPainterPath lTrianglePath;
    lTrianglePath.addPolygon(trianglePolygon( TriangleLeft, QSize(30,30) ));
    lTrianglePath.closeSubpath();
    markers.push_back(lTrianglePath);
    //MsRTriangle,     ///< Triangle pointing right
    QPainterPath rTrianglePath;
    rTrianglePath.addPolygon(trianglePolygon( TriangleRight, QSize(30,30) ));
    rTrianglePath.closeSubpath();
    markers.push_back(rTrianglePath);

    // MsStar,          ///< Five-pointed star
    QPainterPath starPath;
    starPath.moveTo(30, 15);
    for (int i = 1; i < 5; ++i) {
        starPath.lineTo(15 + 16 * qCos(0.8 * i * Pi),
                        15 + 16 * qSin(0.8 * i * Pi));
    }
    starPath.closeSubpath();
    markers.push_back(starPath);

    //MsCross,     ///< Cross (+)
    QPainterPath crossPath;
    crossPath.addText(0, 30, timesFont, "+" );
    markers.push_back(crossPath);

    //MsXCross,    ///< Diagonal cross (X)
    QPainterPath xcrossPath;
    xcrossPath.addText(0, 30, timesFont, "x" );
    markers.push_back(xcrossPath);

    // MsHLine,     ///< Horizontal line
    QPainterPath hlinePath;
    hlinePath.moveTo(0, 15.0);
    hlinePath.lineTo(30.0, 15.0);
    hlinePath.closeSubpath();
    markers.push_back(hlinePath);

    // MsVLine,     ///< Vertical line
    QPainterPath vlinePath;
    vlinePath.moveTo(15, 0.0);
    vlinePath.lineTo(15, 30.0);
    vlinePath.closeSubpath();
    markers.push_back(vlinePath);

    // MsLineStar,  ///< Symbol star (*)
    QPainterPath textPath;
    textPath.addText(0, 40, timesFont, "*" );
    markers.push_back(textPath);

}


QPolygonF MarkerShapes::trianglePolygon(
        TriangleType type, const QSize& size )
{
    QPolygonF triangle( 3 );
    QPointF *trianglePoints = triangle.data();
    double sw2 = 0.5 * size.width();
    double sh2 = 0.5 * size.height();
    double x = size.width()/2.;
    double y = size.height()/2.;
    const double x1 = x - sw2+2;
    const double x2 = x1 + size.width()-2;
    const double y1 = y - sh2+2;
    const double y2 = y1 + size.height()-2;

    switch ( type )
    {
    case TriangleLeft:
    {
        trianglePoints[0].rx() = x2;
        trianglePoints[0].ry() = y1;

        trianglePoints[1].rx() = x1;
        trianglePoints[1].ry() = y;

        trianglePoints[2].rx() = x2;
        trianglePoints[2].ry() = y2;
        break;
    }
    case TriangleRight:
    {
        trianglePoints[0].rx() = x1;
        trianglePoints[0].ry() = y1;

        trianglePoints[1].rx() = x2;
        trianglePoints[1].ry() = y;

        trianglePoints[2].rx() = x1;
        trianglePoints[2].ry() = y2;

        break;
    }
    case TriangleUp:
    {
        trianglePoints[0].rx() = x1;
        trianglePoints[0].ry() = y2;

        trianglePoints[1].rx() = x;
        trianglePoints[1].ry() = y1;

        trianglePoints[2].rx() = x2;
        trianglePoints[2].ry() = y2;

        break;
    }
    case TriangleDown:
    {
        trianglePoints[0].rx() = x1;
        trianglePoints[0].ry() = y1;

        trianglePoints[1].rx() = x;
        trianglePoints[1].ry() = y2;

        trianglePoints[2].rx() = x2;
        trianglePoints[2].ry() = y1;

        break;
    }
    }
    return triangle;
}


} // namespace jsonui

