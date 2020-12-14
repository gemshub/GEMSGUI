#include <QObject>
#include "v_mod.h"
#include "graph.h"
#include "GraphDialogN.h"
#include "GemsMainWindow.h"
#include "graph_window.h"

void  adjustAxis( double& min, double& max, int& numTicks);
ChartData *allocateData( const std::vector<TPlot>& aPlots,
                         std::vector<std::shared_ptr<PlotModel>>& plotModels,
                         const char * aTitle, const char *aXName, const char *aYName, int agraphType  );

SeriesLineData convertor( const TPlotLine& plotData )
{
    SeriesLineData data( plotData.getName().c_str(),
                         plotData.getType()%20, plotData.getSize(),
                         plotData.getLineSize(),
                         plotData.getLineStyle(), plotData.getSpline(), // could be add: penStyle  end  spline now default
                         plotData.getColor()  );
    data.setXColumn(plotData.getIndex());
    return data;
}

TPlotLine convertor( const SeriesLineData& serData )
{
    TPlotLine data( serData.getName().c_str(), serData.getMarkerShape(), serData.getMarkerSize(),
                    serData.getPenSize(), serData.getPenStyle(), serData.getSpline(), serData.getXColumn(),
                    // could be add: penStyle  end  spline
                    serData.getColor()  );
    return data;
}



ChartData *allocateData( const std::vector<TPlot>& aPlots,
                         std::vector<std::shared_ptr<PlotModel>>& plotModels,
                         const char * aTitle, const char *aXName, const char *aYName, int agraphType  )
{
    /// Descriptions of model extracting data
    std::vector<std::shared_ptr<jsonui::ChartDataModel>> chartModels;

    for (size_t ii=0; ii<aPlots.size(); ++ii)
    {
        plotModels.push_back( std::shared_ptr<PlotModel>( new PlotModel( aPlots[ii] )) );
        chartModels.push_back( std::shared_ptr<ChartDataModel>( new ChartDataModel( plotModels.back().get() )) );
        chartModels.back()->setXColumns(aPlots[ii].xColumns());
        chartModels.back()->setYColumns(aPlots[ii].yColumns(), true);
    }
    std::string XName = std::string(aXName, 0, MAXAXISNAME);
    if( XName.empty() )
        XName = "abscissa";
    std::string YName = std::string(aYName, 0, MAXAXISNAME);
    if( YName.empty() )
        YName = "ordinate";

    return new ChartData( chartModels, aTitle, XName, YName, agraphType );
}


///   The constructor
GraphDialog* updateGraphWindow(  GraphDialog* graph_dlg,
                                 TCModule *pmodule, std::vector<TPlot>& aPlots,
                                 const char * aTitle,
                                 float *sizeReg,  float * sizePart,
                                 TPlotLine* aLinesDesc, short *aAxisType,
                                 const char *aXName, const char *aYName )
{

    /// Description of 2D plotting widget
    std::shared_ptr<jsonui::ChartData> m_chartData;
    /// Description of 2D modelS
    std::vector<std::shared_ptr<PlotModel>> plotModels;

    // alloc memory
    m_chartData.reset( allocateData( aPlots, plotModels, aTitle, aXName, aYName, aAxisType[4] ));
    // get data from settings
    m_chartData->setBackgroundColor( QColor(aAxisType[1], aAxisType[2], aAxisType[3]) );
    m_chartData->setAxisTypes( aAxisType[0], aAxisType[5] );
    for(int i=0; i<4; i++ )
    {
        m_chartData->region[i] = static_cast<double>(sizeReg[i]);
        m_chartData->part[i]   = static_cast<double>(sizePart[i]);
    }

    for (size_t ii=0; ii<m_chartData->linesNumber(); ++ii)
    {
        auto seriesdata = convertor(aLinesDesc[ii]);
        auto plot =  m_chartData->getPlot( ii );
        if( plot >= 0 &&  plotModels[plot]->absCount() <= seriesdata.getXColumn() )
            seriesdata.setXColumn(0);
        m_chartData->setLineData( ii, seriesdata );
    }

    if( graph_dlg )
        graph_dlg->close();
    //if( !graph_dlg )
    {
        graph_dlg = new GraphDialog( pmodule, m_chartData, plotModels  );
        TCModuleImp *topw =	 qobject_cast<TCModuleImp *>( pmodule->window());
        if( topw )
        {
            QObject::connect( graph_dlg, SIGNAL( dataChanged( jsonui::ChartData* ) ),
                              topw,  SLOT( saveGraphData( jsonui::ChartData* ) ) );
            //connect( _page, SIGNAL( updateGraphWindow() ),
            //         graph_dlg,  SLOT( UpdateAll() ) );
        }
    }
    //else {
    //    graph_dlg->resetGraphDialog( m_chartData, plotModels);
    //}
    pVisorImp->openMdiChild( graph_dlg );
    return graph_dlg;
}

///   The constructor
GraphDialog* updateGraphWindow(  GraphDialog* graph_dlg,
                                 TCModule *pmodule, std::vector<TPlot>& aPlots,
                                 const char * aTitle,
                                 const char *aXName, const char *aYName,
                                 TCStringArray line_names,
                                 const std::vector<TPlotLine>& def_plt_lines,
                                 int agraphType  )
{
    /// Description of 2D plotting widget
    std::shared_ptr<jsonui::ChartData> m_chartData;
    /// Description of 2D modelS
    std::vector<std::shared_ptr<PlotModel>> plotModels;

    // alloc memory
    m_chartData.reset( allocateData( aPlots, plotModels, aTitle, aXName, aYName, agraphType ));
    // change names
    for (size_t ii=0; ii<line_names.size(); ++ii)
    {
        int plot =  m_chartData->getPlot( ii );
        if( plot >= 0 && plot < static_cast<int>(def_plt_lines.size()) )
        {
            TPlotLine defpl(ii, line_names.size(), "",
                            def_plt_lines[plot].getType(),
                            def_plt_lines[plot].getSize(),
                            def_plt_lines[plot].getLineSize());
            auto seriesdata = convertor(defpl);

            if( plot >= 0 &&  plotModels[plot]->absCount() <= seriesdata.getXColumn() )
                seriesdata.setXColumn(0);
            m_chartData->setLineData( ii, seriesdata );
        }
        if( ii > m_chartData->linesNumber() )
            break;
        m_chartData->setLineData( ii,  std::string(line_names[ii].c_str())  );
    }

    // set up axis
    double minX=DOUBLE_EMPTY, maxX=DOUBLE_EMPTY,
            minY=DOUBLE_EMPTY, maxY=DOUBLE_EMPTY;

    for( size_t ii=0, nLines=0; ii<aPlots.size(); ii++)
        for( int jj=0; jj<aPlots[ii].getLinesNumber(); jj++, nLines++ )
        {
            aPlots[ii].getMaxMinLine( minX, maxX, minY, maxY, jj, m_chartData->lineData(nLines).getXColumn() );
        }

    adjustAxis(minX, maxX, m_chartData->axisTypeX);
    adjustAxis(minY, maxY, m_chartData->axisTypeY);

    m_chartData->region[0] = minX;
    m_chartData->region[1] = maxX;
    m_chartData->region[2] = minY;
    m_chartData->region[3] = maxY;
    m_chartData->part[0] = minX+(maxX-minX)/3;
    m_chartData->part[1] = maxX-(maxX-minX)/3;
    m_chartData->part[2] = minY+(maxY-minY)/3;
    m_chartData->part[3] = maxY-(maxY-minY)/3;
    // end setup axis

    if( graph_dlg )
        graph_dlg->close();
    //if( !graph_dlg )
    {
        graph_dlg = new GraphDialog( pmodule, m_chartData, plotModels  );
        TCModuleImp *topw =	 qobject_cast<TCModuleImp *>( pmodule->window());
        if( topw )
        {
            QObject::connect( graph_dlg, SIGNAL( dataChanged( jsonui::ChartData* ) ),
                              topw,  SLOT( saveGraphData( jsonui::ChartData* ) ) );
            //connect( _page, SIGNAL( updateGraphWindow() ),
            //         graph_dlg,  SLOT( UpdateAll() ) );
        }
    }
    //else {
    //    graph_dlg->resetGraphDialog( m_chartData, plotModels);
    //}
    pVisorImp->openMdiChild( graph_dlg );
    return graph_dlg;
}

void  adjustAxis( double& min, double& max, int& numTicks)
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
