//  This is JSONUI library+API (https://bitbucket.org/gems4/jsonui)
//
/// \file chart_view.cpp
/// Implementation of the PlotChartView, PlotChartViewPrivate classes
/// manages the graphical representation of the chart's series and axes
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

#include <QMimeData>
#include <QDragEnterEvent>
#include <QDrag>
#include <QtCharts/QAreaSeries>
#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QVXYModelMapper>
#include <QtCharts/QValueAxis>
#include <QGraphicsLayout>
#include <QRubberBand>

#include <QFileInfo>
#include <QSizeF>
#include <QPrinter>
#include <QSvgGenerator>
#include <QPixmap>

#include "graph_data.h"
#include "chart_view.h"
#ifdef NO_JSONIO
#include "from_jsonio.h"
#else
#include "jsonio/nservice.h"
#endif

namespace jsonui {

class PlotChartViewPrivate
{

public:

    explicit PlotChartViewPrivate( ChartData *graphdata,
                                   QChartView *parent, QChart* achart ):
        view(parent), chart(achart), gr_data(graphdata)
    {  }

    ~PlotChartViewPrivate()
    {
        clearAll();
    }

    void showPlot()
    {
        showPlotInternal();
        makeGrid();
    }
    void updateSeries( size_t nline );
    void highlightSeries( size_t line, bool enable  );
    void updateAll()
    {
        clearAll();
        showPlot();
    }

    void updateLines()
    {
        clearLines();
        showPlotInternal();
        attachAxis();
    }

    void updateFragment( bool newFragment )
    {
        if(isFragment != newFragment )
        {
            isFragment = newFragment;
            updateMinMax();
        }
    }

    void setFragment( bool newFragment )
    {
        isFragment = newFragment;
    }

    void updateGrid();
    void updateMinMax();
    void attachAxis();

    void addLabel(  const QPointF& pointF, const QString& label )
    {
        if( mapLabels.find(label) != mapLabels.end() )
            chart->removeSeries(mapLabels[label].get());

        QScatterSeries *series  = newScatterLabel( pointF, label );
        chart->addSeries(series);
        series->attachAxis(axisX);
        series->attachAxis(axisY);
        mapLabels[label] = std::shared_ptr<QScatterSeries>(series);
    }

    void addPoint(  const QPointF& pointF, const QString& label )
    {
        if( show_point )
            chart->removeSeries(show_point.get());

        QScatterSeries *series  = newScatterLabel( pointF, label );
        chart->addSeries(series);
        series->attachAxis(axisX);
        series->attachAxis(axisY);
        show_point.reset(series);
    }


protected:

    QChartView *view;
    QChart* chart;
    ChartData *gr_data;
    QValueAxis *axisX =nullptr;
    QValueAxis *axisY =nullptr;

    std::vector<std::shared_ptr<QXYSeries> >       gr_series;
    std::vector<std::shared_ptr<QVXYModelMapper> > series_mapper;
    std::vector<std::shared_ptr<QScatterSeries> >  gr_points;
    std::vector<std::shared_ptr<QVXYModelMapper> > points_mapper;
    std::vector<std::shared_ptr<QAreaSeries> >     gr_areas;
    std::map<QString,std::shared_ptr<QScatterSeries> > mapLabels;
    std::shared_ptr<QScatterSeries> show_point;

    double generated_region[4];
    bool isFragment = false;

private:

    void clearLines();
    void clearAxis();
    void clearAll()
    {
        clearLines();
        clearAxis();
    }

    QXYSeries *newSeriesLine( const SeriesLineData& linedata );
    QScatterSeries *newScatterSeries( const SeriesLineData& linedata );
    void updateScatterSeries( QScatterSeries* series, const SeriesLineData& linedata );
    void mapSeriesLine( QXYSeries *series, QVXYModelMapper *mapper,
                        ChartDataModel* chmodel, int ycolumn, int xcolumn );

    void addPlotLine( ChartDataModel* chmodel, int ycolumn, const SeriesLineData& linedata );
    void addScatterSeries( ChartDataModel* chmodel, int ycolumn, const SeriesLineData& linedata   );

    void showPlotLines();
    void makeGrid();
    void showPlotInternal();

    QScatterSeries* newScatterLabel( const QPointF& pointF, const QString& label );

    void showAreaChart();
    void updateSeriesLine( size_t nline );
    void updateAreaLine( size_t nline );

};



void PlotChartViewPrivate::clearLines()
{
    mapLabels.clear();
    series_mapper.clear();
    gr_series.clear();
    points_mapper.clear();
    gr_points.clear();
    gr_areas.clear();
    chart->removeAllSeries();
}

void PlotChartViewPrivate::clearAxis()
{
    chart->removeAxis(axisX);
    chart->removeAxis(axisY);
    delete axisX;
    axisX =nullptr;
    delete axisY;
    axisY =nullptr;
}

QXYSeries* PlotChartViewPrivate::newSeriesLine( const SeriesLineData& linedata )
{
    QXYSeries* series = nullptr;
    if( linedata.getPenSize() <= 0 )
        return series;

    if( linedata.getSpline()  )
        series = new QSplineSeries;
    else
        series = new QLineSeries;

    series->setName(linedata.getName().c_str());
    QPen pen = series->pen();
    getLinePen( pen, linedata  );
    series->setPen(pen);
    return series;
}


QScatterSeries* PlotChartViewPrivate::newScatterSeries( const SeriesLineData& linedata )
{
    QScatterSeries *series = nullptr;

    if( linedata.getMarkerSize() <= 0/*2*/ )
        return series;

    series = new QScatterSeries;
    updateScatterSeries( series, linedata );
    return series;
}

void PlotChartViewPrivate::updateScatterSeries( QScatterSeries* series, const SeriesLineData& linedata )
{
    series->setName( linedata.getName().c_str());
    series->setPen( QPen(Qt::transparent));
    series->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
    auto msize = linedata.getMarkerSize()+2;
    series->setMarkerSize(msize);
    series->setBrush( markerShapeImage( linedata ).scaled(msize, msize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void PlotChartViewPrivate::mapSeriesLine( QXYSeries *series,
                                          QVXYModelMapper *mapper, ChartDataModel* chmodel,
                                          int ycolumn, int xcolumn )
{
    mapper->setXColumn(xcolumn+1);
    mapper->setYColumn(ycolumn+1);
    mapper->setSeries(series);
    mapper->setModel(chmodel);
}

void PlotChartViewPrivate::addPlotLine(
        ChartDataModel* chmodel, int ycolumn, const SeriesLineData& linedata   )
{
    // init series
    QXYSeries *series = newSeriesLine( linedata );
    QVXYModelMapper *mapper = new QVXYModelMapper;
    mapSeriesLine( series, mapper, chmodel, ycolumn, chmodel->getXColumn(linedata.getXColumn()) );
    if( series )
        chart->addSeries(series);
    gr_series.push_back(std::shared_ptr<QXYSeries>(series));
    series_mapper.push_back(std::shared_ptr<QVXYModelMapper>(mapper));
}

void PlotChartViewPrivate::addScatterSeries(
        ChartDataModel* chmodel, int ycolumn, const SeriesLineData& linedata   )
{
    // init series
    QScatterSeries *series = newScatterSeries( linedata );
    QVXYModelMapper *mapper = new QVXYModelMapper;
    mapSeriesLine( series, mapper, chmodel, ycolumn, chmodel->getXColumn(linedata.getXColumn()) );
    if( series )
        chart->addSeries(series);
    gr_points.push_back(std::shared_ptr<QScatterSeries>(series));
    points_mapper.push_back(std::shared_ptr<QVXYModelMapper>(mapper));
}


void PlotChartViewPrivate::showPlotLines()
{
    size_t ii, nline;
    for( ii=0, nline =0; ii < gr_data->modelsNumber(); ii++)
    {
        auto  srmodel = gr_data->modelData( ii );
        for(size_t jj=0; jj < srmodel->getSeriesNumber(); jj++, nline++ )
        {
            addPlotLine( srmodel, srmodel->getYColumn(jj), gr_data->lineData(nline) );
            addScatterSeries( srmodel, srmodel->getYColumn(jj), gr_data->lineData(nline)   );
        }
    }
}

void PlotChartViewPrivate::showAreaChart()
{
    // The lower series initialized to zero values
    QLineSeries *lowerSeries = nullptr;
    size_t ii, nline;

    for( ii=0, nline =0; ii < gr_data->modelsNumber(); ii++)
    {
        auto  srmodel = gr_data->modelData( ii );
        for(size_t jj=0; jj < srmodel->getSeriesNumber(); jj++, nline++ )
        {
            auto linedata = gr_data->lineData(nline);

            if( linedata.getXColumn()  < -1 )
                continue;

            linedata.setLineChanges( 1, 1, 0 );
            addPlotLine( srmodel, srmodel->getYColumn(jj), linedata   );

            QLineSeries *upperSeries = dynamic_cast<QLineSeries *>(gr_series.back().get());
            if( upperSeries )
            {
                QAreaSeries *area = new QAreaSeries(upperSeries, lowerSeries);
                // define colors
                area->setName(linedata.getName().c_str());
                QPen pen = area->pen();
                getLinePen( pen,  linedata  );
                area->setPen(pen);
                area->setColor(pen.color());
                area->setOpacity(0.5);

                chart->addSeries(area);
                gr_areas.push_back(std::shared_ptr<QAreaSeries>(area));
                lowerSeries = upperSeries;
            }
        }
    }
}

void PlotChartViewPrivate::showPlotInternal()
{
    switch( gr_data->getGraphType() )
    {
    case LineChart:
        showPlotLines();
        break;
    case AreaChart:
        showAreaChart();
        break;
    case BarChart:
    case Isolines:
    case lines_3D:
        break;
    }
}

void PlotChartViewPrivate::updateMinMax()
{
    if( !axisX || !axisY)
        return;

    if( isFragment )
    {
        if( !jsonio::essentiallyEqual( gr_data->part[0], gr_data->part[1]) &&
                !jsonio::essentiallyEqual( gr_data->part[2], gr_data->part[3]) )
        {
            axisY->setRange(gr_data->part[2], gr_data->part[3]);
            axisX->setRange(gr_data->part[0], gr_data->part[1]);
        }
    }
    else
    {
        if( !jsonio::essentiallyEqual(gr_data->region[0],gr_data->region[1]) &&
                !jsonio::essentiallyEqual(gr_data->region[2], gr_data->region[3]) )
        {
            axisX->setRange(gr_data->region[0], gr_data->region[1]);
            axisY->setRange(gr_data->region[2], gr_data->region[3]);
        }
        else
        {
            axisX->setRange(generated_region[0], generated_region[1]);
            axisY->setRange(generated_region[2], generated_region[3]);
        }
    }
}

void PlotChartViewPrivate::updateGrid()
{
    chart->setFont(gr_data->axisFont);
    auto titleFont = gr_data->axisFont;
    titleFont.setPointSize(titleFont.pointSize()+2);
    chart->setTitleFont(titleFont);
    chart->setTitle( gr_data->title.c_str() );

    if( !axisX || !axisY)
        return;

    updateMinMax();

    chart->setBackgroundBrush( gr_data->getBackgroundColor() );

    axisX->setTickCount( gr_data->axisTypeX+1 );
    //axisX->setMinorTickCount(4);
    axisX->setTitleFont( titleFont );
    axisX->setLabelsFont( gr_data->axisFont );
    axisX->setTitleText( gr_data->xName.c_str() );
    auto penX = axisX->linePen();
    penX.setWidth(3);
    penX.setColor(Qt::darkGray);
    axisX->setLinePen(penX);

    axisY->setTickCount( gr_data->axisTypeY+1 );
    //axisY->setMinorTickCount(4);
    axisY->setTitleFont( titleFont );
    axisY->setLabelsFont( gr_data->axisFont );
    axisY->setTitleText( gr_data->yName.c_str() );
    axisY->setLinePen(penX);

    // must be setPen(QChartPrivate::defaultPen()) for lines and points
    // and default background
    // chart->setTheme(QChart::ChartThemeLight);
}

void PlotChartViewPrivate::attachAxis()
{
    if( !axisX || !axisY)
        return;

    for( uint ii=0; ii<gr_series.size(); ii++ )
    {
        if( gr_series[ii].get() )
        {
            gr_series[ii]->attachAxis(axisX);
            gr_series[ii]->attachAxis(axisY);
        }
    }
    for( uint ii=0; ii<gr_points.size(); ii++ )
    {
        if( gr_points[ii].get() )
        {
            gr_points[ii]->attachAxis(axisX);
            gr_points[ii]->attachAxis(axisY);
        }
    }
    for( uint ii=0; ii<gr_areas.size(); ii++ )
    {
        if( gr_areas[ii].get() )
        {
            gr_areas[ii]->attachAxis(axisX);
            gr_areas[ii]->attachAxis(axisY);
        }
    }
}

void PlotChartViewPrivate::makeGrid()
{
    if(  jsonio::essentiallyEqual( gr_data->region[0], gr_data->region[1]) ||
         jsonio::essentiallyEqual( gr_data->region[2], gr_data->region[3]) )
    {    // default
        chart->createDefaultAxes();
        auto axises = chart->axes(Qt::Horizontal);
        if( axises.size() > 0 )
            axisX =  dynamic_cast<QValueAxis*>(axises[0]);
        axises = chart->axes(Qt::Vertical);
        if( axises.size() > 0 )
            axisY =  dynamic_cast<QValueAxis*>(axises[0]);

        generated_region[0] = axisX->min();
        generated_region[1] = axisX->max();
        generated_region[2] = axisY->min();
        generated_region[3] = axisY->max();
        gr_data->part[0] = 0;
        gr_data->part[1] = 0;
        gr_data->part[2] = 0;
        gr_data->part[3] = 0;

    } else
    {
        axisX = new QValueAxis;
        chart->addAxis(axisX, Qt::AlignBottom);
        axisY = new QValueAxis;
        chart->addAxis(axisY, Qt::AlignLeft);
        attachAxis();
    }
    updateGrid();
}

void PlotChartViewPrivate::updateSeries( size_t nline )
{
    switch( gr_data->getGraphType() )
    {
    case LineChart:
        updateSeriesLine( nline );
        break;
    case AreaChart:
        updateAreaLine( nline );
        break;
    case BarChart:
    case Isolines:
    case lines_3D:
        break;
    }
}

void PlotChartViewPrivate::highlightSeries( size_t line, bool enable )
{
    if( gr_data->getGraphType() != LineChart || line >= gr_data->linesNumber() )
        return;
    auto  linedata = gr_data->lineData( line );

    // update series lines
    QXYSeries *series =  gr_series[line].get();
    if( series )
    {
        QPen pen = series->pen();
        getLinePen( pen, linedata  );
        if( enable )
            pen.setWidth(linedata.getPenSize()*2);
        series->setPen(pen);
    }

    QScatterSeries *scatterseries = gr_points[line].get();
    if( scatterseries )
    {
        auto shsize = linedata.getMarkerSize();
        if( enable )
            shsize *=2;
        scatterseries->setBrush( markerShapeImage( linedata ).scaled( shsize,shsize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        scatterseries->setMarkerSize(shsize);
    }
}


void PlotChartViewPrivate::updateSeriesLine( size_t nline )
{
    if( nline >= gr_data->linesNumber() )
        return;

    auto nplot =  gr_data->getPlot( nline );
    if( nplot < 0 )
        return;
    auto  srmodel = gr_data->modelData( static_cast<size_t>(nplot) );
    auto  linedata = gr_data->lineData( nline );

    // update series lines
    QXYSeries *series = newSeriesLine(linedata );
    // delete old series
    if( gr_series[nline].get())
        chart->removeSeries(gr_series[nline].get());

    gr_series[nline].reset( series );
    series_mapper[nline]->setSeries(series);
    series_mapper[nline]->setXColumn( srmodel->getXColumn(linedata.getXColumn())+1);
    if( series )
    {
        chart->addSeries(series);
        series->attachAxis(axisX);
        series->attachAxis(axisY);
    }

    QScatterSeries *scatterseries = newScatterSeries( linedata );
    // delete old series
    if( gr_points[nline].get())
        chart->removeSeries(gr_points[nline].get());

    gr_points[nline].reset( scatterseries );
    points_mapper[nline]->setSeries(scatterseries);
    points_mapper[nline]->setXColumn( srmodel->getXColumn(linedata.getXColumn())+1);
    if( scatterseries )
    {
        chart->addSeries(scatterseries);
        scatterseries->attachAxis(axisX);
        scatterseries->attachAxis(axisY);
    }
}

void PlotChartViewPrivate::updateAreaLine( size_t nline )
{
    if( nline >= gr_data->linesNumber() )
        return;

    gr_areas[nline]->setName(gr_data->lineData(nline).getName().c_str());
    QPen pen = gr_areas[nline]->pen();
    getLinePen( pen, gr_data->lineData(nline)  );
    gr_areas[nline]->setPen(pen);
    gr_areas[nline]->setColor(pen.color());
}


QScatterSeries* PlotChartViewPrivate::newScatterLabel(
        const QPointF& pointF, const QString& label )
{
    QScatterSeries *series  =  new QScatterSeries;
    QFontMetrics fm(gr_data->axisFont);
    int size = fm.horizontalAdvance(label)+2;

    series->setName( label );
    series->setPen( QPen(Qt::transparent));
    series->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
    series->setMarkerSize(size);
    series->setBrush( textImage( gr_data->axisFont, label ));

    auto pointNew = pointF;
    pointNew.setX(pointNew.x()+size/2.);
    auto pointV = chart->mapToValue(pointNew );
    series->append(pointV);
    return series;
}


//-------------------------------------------------------------------


PlotChartView::PlotChartView( ChartData *graphdata, QWidget *parent) :
    QChartView(new QChart(), parent),
    pdata( new PlotChartViewPrivate( graphdata, this, chart() ) )
{
    setRubberBand(QChartView::RectangleRubberBand);
    setAcceptDrops(true);

    pdata->showPlot();
    chart()->setDropShadowEnabled(false);
    chart()->legend()->hide();
    chart()->layout()->setContentsMargins(0, 0, 0, 0);
    chart()->setMargins( QMargins( 5,5, 0, 0) );
    // chart()->legend()->setMarkerShape(QLegend::MarkerShapeFromSeries);

    rubberBand = findChild<QRubberBand *>();
}

PlotChartView::~PlotChartView()
{
    delete pdata;
}

void PlotChartView::updateLine(size_t line )
{
    pdata->updateSeries( line );
}

void PlotChartView::updateAll()
{
    pdata->updateAll();
}

void PlotChartView::updateLines()
{
    //pdata->updateAll();
    pdata->updateLines();
}

void PlotChartView::updateFragment( bool isFragment )
{
    pdata->updateFragment( isFragment );
}

void PlotChartView::resetFragment( bool isFragment )
{
    pdata->setFragment( isFragment );
}

void PlotChartView::highlightLine(size_t line, bool enable)
{
    pdata->highlightSeries(line, enable);
}

void PlotChartView::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("text/plain")) {
        if (event->source() == this) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        } else {
            event->acceptProposedAction();
        }
    } else {
        event->ignore();
    }
}

void PlotChartView::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat("text/plain")) {
        if (event->source() == this) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        } else {
            event->acceptProposedAction();
        }
    } else {
        event->ignore();
    }
}

void PlotChartView::dropEvent( QDropEvent* event )
{
    if (event->mimeData()->hasFormat("text/plain"))
    {
        QString text_ = event->mimeData()->text();
        auto posF =  event->posF();
        pdata->addLabel( posF, text_ );
        //std::cout << "pos " << pos.x() <<  " " << pos.y() << "Test drop" << text_.toStdString() << std::endl;
    }
}

void PlotChartView::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton)
    {
        /*QPointF fp = chart()->mapToValue(event->pos());
        QString label = QString("%1:%2").arg( fp.x()).arg( fp.y());
        pdata->addPoint( event->pos(), label);*/
        event->accept();
        return;
    }
    else
        if (event->button() == Qt::LeftButton)
        {
            if( rubberBand && rubberBand->isVisible() )
            {
                if( rubberBand->height() < 3 ||  rubberBand->width() < 3 )
                {
                    rubberBand->hide();
                    event->accept();
                    return;
                }

                QPointF fp = chart()->mapToValue(rubberBand->geometry().topLeft());
                QPointF tp = chart()->mapToValue(rubberBand->geometry().bottomRight());
                QRectF  rect(fp, tp);
                emit fragmentChanged(rect);
            }
        }

    QChartView::mouseReleaseEvent(event);
}


/*!
  Render graphical representation of the chart's series and axes to a file

  Supported formats are:

  - pdf\n
    Portable Document Format PDF
  - ps\n
    Postcript
  - svg\n
    Scalable Vector Graphics SVG
  - all image formats supported by Qt\n
    see QImageWriter::supportedImageFormats()

  \param document title
  \param fileName Path of the file, where the document will be stored

*/
void PlotChartView::renderDocument( const QString &title, const QString &fileName )
{
    const QString fmt = QFileInfo( fileName ).suffix().toLower();
    if ( fmt == "pdf" )
    {
#ifndef QT_NO_PRINTER
        QPrinter printer(QPrinter::HighResolution);
        printer.setDocName( title );
        printer.setOutputFormat( QPrinter::PdfFormat );
        printer.setOutputFileName( fileName );
        printer.setColorMode( QPrinter::Color );
        printer.setFullPage( true );
        printer.setPageSize( QPageSize(QPageSize::A4) );

        QPainter p( &printer );
        render( &p );
#endif
    }
    else if ( fmt == "svg" )
    {
#ifndef QWT_NO_SVG
        QSvgGenerator generator;
        generator.setTitle( title );
        generator.setFileName( fileName );
        generator.setSize( size() );
        generator.setViewBox( rect() );
        QPainter p(&generator);
        render(&p);
#endif
    }
    else
    {
        QPixmap picture = grab();
        picture.save(fileName);
    }
}



} // namespace jsonui
