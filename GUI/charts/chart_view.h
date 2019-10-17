//  This is JSONUI library+API (https://bitbucket.org/gems4/jsonui)
//
/// \file chart_view.h
/// Declaration of the PlotChartView class -  manages the graphical
/// representation of the chart's series and axes
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

#ifndef CHARTVIEW_H
#define CHARTVIEW_H

#include <memory>
#include <QtCharts/QChartView>

QT_CHARTS_USE_NAMESPACE

namespace jsonui {

class PlotChartViewPrivate;
class ChartData;

/// Manages the graphical representation of the chart's series and axes
class PlotChartView : public QChartView
{
    Q_OBJECT

public slots:

    void updateLine( size_t line );
    void updateAll();
    void updateLines();
    void highlightLine( size_t line, bool enable  );
    void updateFragment( bool isFragment );

signals:
    void fragmentChanged(QRectF  rect);

public:

    explicit PlotChartView( ChartData *graphdata, QWidget *parent = nullptr);
    ~PlotChartView();

    void renderDocument(const QString &title, const QString &fileName);
    void resetFragment( bool isFragment );

protected:

    PlotChartViewPrivate* pdata;
    QRubberBand *rubberBand;

    void dragEnterEvent(QDragEnterEvent* event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent* event);
    void mouseReleaseEvent(QMouseEvent *event);

};

} // namespace jsonui

#endif // CHARTVIEW_H
