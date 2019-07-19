#include <QObject>
#ifdef USE_QWT
#include "GraphDialog.h"
#else
#include "v_mod.h"
#include "graph.h"
#include "GraphDialogN.h"
#endif
#include "GemsMainWindow.h"
#include "graph_window.h"

//---------------------------------------------------------------------------

#ifdef USE_QWT

///   The constructor
GraphWindow::GraphWindow(TCModule *pmodule, TIArray<TPlot>& aPlots,
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
GraphWindow::GraphWindow(TCModule *pmodule, TIArray<TPlot>& aPlots,
                         const char * aTitle,
                         const char *aXName, const char *aYName,
                         TCStringArray line_names, int agraphType  )
{
    GraphData  data( aPlots, aTitle, aXName, aYName, line_names, agraphType );
    graph_dlg = new GraphDialog(pmodule, data);
    pVisorImp->openMdiChild( graph_dlg );
}

GraphWindow::~GraphWindow()
{
    cout << "delete graph_dlg" << endl;
    if(graph_dlg)
        graph_dlg->close();
    delete graph_dlg;
}

void GraphWindow::AddPoint( int nPlot, int nPoint )
{
    if( graph_dlg )
    {
        graph_dlg->AddPoint( nPlot, nPoint );
    }
}

void GraphWindow::ShowGraph( const char * capAdd )
{
    if( graph_dlg )
    {
        graph_dlg->ShowNew(capAdd);
    }
}

void *GraphWindow::getGraphData() const
{
    return &graph_dlg->gr_data;
}

// ------------------------------------------------------------------------------------------------

///   The constructor
GraphWindow *updateGraphWindow(  GraphWindow* graph_wnd,
                                 TCModule *pmodule, TIArray<TPlot>& aPlots,
                                 const char * aTitle,
                                 float *sizeReg,  float * sizePart,
                                 TPlotLine* aLinesDesc, short *aAxisType,
                                 const char *aXName, const char *aYName )
{
    if( graph_wnd )
        delete graph_wnd;
    graph_wnd = new GraphWindow(pmodule, aPlots, aTitle,
                                sizeReg, sizePart, aLinesDesc,  aAxisType, aXName,  aYName );
    return graph_wnd;
}

///   The constructor
GraphWindow *updateGraphWindow(  GraphWindow* graph_wnd,
                         TCModule *pmodule, TIArray<TPlot>& aPlots,
                         const char * aTitle,
                         const char *aXName, const char *aYName,
                         TCStringArray line_names, int agraphType  )
{
    if( graph_wnd )
        delete graph_wnd;
    graph_wnd = new GraphWindow(pmodule, aPlots, aTitle,
                                aXName, aYName, line_names, agraphType );
    return graph_wnd;
}

#else
//------------------------------------------------------------------------


SeriesLineData convertor( const TPlotLine& plotData )
{
    SeriesLineData data( plotData.getName().c_str(),
                         plotData.getType(), plotData.getSize(),
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



ChartData *allocateData( const TIArray<TPlot>& aPlots,
                         std::vector<std::shared_ptr<PlotModel>>& plotModels,
                         const char * aTitle, const char *aXName, const char *aYName, int agraphType  )
{
    /// Descriptions of model extracting data
    std::vector<std::shared_ptr<jsonui::ChartDataModel>> chartModels;

    for (size_t ii=0; ii<aPlots.GetCount(); ++ii)
    {
        plotModels.push_back( std::shared_ptr<PlotModel>( new PlotModel( aPlots[ii] )) );
        chartModels.push_back( std::shared_ptr<ChartDataModel>( new ChartDataModel( plotModels.back().get() )) );
        chartModels.back()->setXColumns(aPlots[ii].xColumns());
        chartModels.back()->setYColumns(aPlots[ii].yColumns(), true);
    }
    return new ChartData( chartModels, aTitle, std::string(aXName, MAXAXISNAME), std::string(aYName, MAXAXISNAME), agraphType );
}


///   The constructor
GraphDialog* updateGraphWindow(  GraphDialog* graph_dlg,
                                 TCModule *pmodule, TIArray<TPlot>& aPlots,
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
    {
      graph_dlg->close();
      delete graph_dlg;
      graph_dlg = nullptr;
    }
    if( !graph_dlg )
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
    else {
        graph_dlg->resetGraphDialog( m_chartData, plotModels);
    }
    pVisorImp->openMdiChild( graph_dlg );
    return graph_dlg;
}

///   The constructor
GraphDialog* updateGraphWindow(  GraphDialog* graph_dlg,
                                 TCModule *pmodule, TIArray<TPlot>& aPlots,
                                 const char * aTitle,
                                 const char *aXName, const char *aYName,
                                 TCStringArray line_names, int agraphType  )
{
    /// Description of 2D plotting widget
    std::shared_ptr<jsonui::ChartData> m_chartData;
    /// Description of 2D modelS
    std::vector<std::shared_ptr<PlotModel>> plotModels;

    // alloc memory
    m_chartData.reset( allocateData( aPlots, plotModels, aTitle, aXName, aYName, agraphType ));
    // change names
    for (size_t ii=0; ii<line_names.GetCount(); ++ii)
    {
        if( ii > m_chartData->linesNumber() )
            break;
        m_chartData->setLineData( ii,  std::string(line_names[ii].c_str())  );
    }
    if( graph_dlg )
    {
      graph_dlg->close();
      delete graph_dlg;
      graph_dlg = nullptr;
    }
    if( !graph_dlg )
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
    else {
        graph_dlg->resetGraphDialog( m_chartData, plotModels);
    }
    pVisorImp->openMdiChild( graph_dlg );
    return graph_dlg;
}

#endif
