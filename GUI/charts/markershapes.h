//  This is JSONUI library+API (https://bitbucket.org/gems4/jsonui)
//
/// \file markershapes.h
/// Declaration of the MarkerShapes class to define the shape of the marker
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

#ifndef MARKERSHAPES_H
#define MARKERSHAPES_H

#include <QPainterPath>

namespace jsonui {

/// Class to define the shape of the marker used to render the points in the series
class MarkerShapes
{

    friend MarkerShapes& shapes();

    std::vector<QPainterPath>  markers;

    MarkerShapes();

    enum TriangleType
    {
        TriangleLeft,
        TriangleRight,
        TriangleUp,
        TriangleDown
    };

    QPolygonF trianglePolygon( TriangleType type, const QSize& size );

public:

    /// This enum value describes the shape used when rendering marker items.
    enum Style
    {
        // Poligons ( filled and not filled )
        MsCircle,        ///< Circle
        MsRectangle,     ///< Rectangle
        MsDiamond,       ///< Diamond
        MsDTriangle,     ///< Triangle pointing downwards
        MsUTriangle,     ///< Triangle pointing upwards
        MsLTriangle,     ///< Triangle pointing left
        MsRTriangle,     ///< Triangle pointing right
        MsStar,          ///< Five-pointed star

        // Symbols
        MsCross,     ///< Cross (+)
        MsXCross,    ///< Diagonal cross (X)
        MsHLine,     ///< Horizontal line
        MsVLine,     ///< Vertical line
        MsLineStar,  ///< Symbol star (*)
        NoSymbol_     ///< No Style. The symbol cannot be drawn.
    };

    std::size_t size() const
    {
        return markers.size();
    }

    const QPainterPath& shape( size_t ndx ) const
    {
        return markers[ndx%markers.size()];
    }

    void addShape( const QPainterPath& ashape )
    {
        markers.push_back(ashape);
    }

};

MarkerShapes& shapes()
{
    static MarkerShapes  msh;
    return msh;
}

} // namespace jsonui

#endif // MARKERSHAPES_H
