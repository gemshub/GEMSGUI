#ifdef USE_QWT
#include "GraphDialog.h"
#else
#include "graph.h"
#include "GraphDialogN.h"
#endif
#include "GemsMainWindow.h"
#include "graph_window.h"

//---------------------------------------------------------------------------

///   The constructor
GraphWindow::GraphWindow(TCModule *pmodule, TIArray<TPlot>& aPlots,
                         const char * aTitle,
                         float *sizeReg,  float * sizePart,
                         TPlotLine* aLinesDesc, short *aAxisType,
                         const char *aXName, const char *aYName )
{

#ifdef USE_QWT
    GraphData data(aPlots, aTitle, sizeReg, sizePart,
                   aLinesDesc, aAxisType, aXName, aYName);
    graph_dlg = new GraphDialog( pmodule, data );
#else

    /// Descriptions of model extracting data
    std::vector<std::shared_ptr<jsonui::ChartDataModel>> chartModels;

    for ( uint ii=0; ii<aPlots.GetCount(); ++ii)
    {
      m_plotModels.push_back( std::shared_ptr<PlotModel>( new PlotModel( aPlots[ii] )) );
      chartModels.push_back( std::shared_ptr<ChartDataModel>( new ChartDataModel( m_plotModels.back().get() )) );
      chartModels.back()->setXColumns(aPlots[ii].xColumns());
      chartModels.back()->setYColumns(aPlots[ii].yColumns(), true);
    }
    m_chartData.reset(new ChartData( chartModels, "Graph for window", "x", "y" ));

    graph_dlg = new GraphDialog( pmodule, m_chartData.get(), nullptr );
    //connect( graph_dlg, SIGNAL( dataChanged(ChartData* ) ),
    //         _page,  SLOT( saveGraphData( ChartData* ) ) );
    //connect( _page, SIGNAL( updateGraphWindow() ),
    //         graph_dlg,  SLOT( UpdateAll() ) );
#endif

    pVisorImp->openMdiChild( graph_dlg );
}

///   The constructor
GraphWindow::GraphWindow(TCModule *pmodule, TIArray<TPlot>& aPlots,
                         const char * aTitle,
                         const char *aXName, const char *aYName,
                         TCStringArray line_names, int agraphType  )
{
#ifdef USE_QWT
    GraphData  data( aPlots, aTitle, aXName, aYName, line_names, agraphType );
    graph_dlg = new GraphDialog(pmodule, data);
#else

    /// Descriptions of model extracting data
    std::vector<std::shared_ptr<jsonui::ChartDataModel>> chartModels;

    for ( uint ii=0; ii<aPlots.GetCount(); ++ii)
    {
      m_plotModels.push_back( std::shared_ptr<PlotModel>( new PlotModel( aPlots[ii] )) );
      chartModels.push_back( std::shared_ptr<ChartDataModel>( new ChartDataModel( m_plotModels.back().get() )) );
      chartModels.back()->setXColumns(aPlots[ii].xColumns());
      chartModels.back()->setYColumns(aPlots[ii].yColumns(), true);
    }
    m_chartData.reset(new ChartData( chartModels, "Graph for window", "x", "y" ));

    graph_dlg = new GraphDialog( pmodule, m_chartData.get(), nullptr );
    //connect( graph_dlg, SIGNAL( dataChanged(ChartData* ) ),
    //         _page,  SLOT( saveGraphData( ChartData* ) ) );
    //connect( _page, SIGNAL( updateGraphWindow() ),
    //         graph_dlg,  SLOT( UpdateAll() ) );
#endif
    pVisorImp->openMdiChild( graph_dlg );
}

GraphWindow::~GraphWindow()
{
}

void GraphWindow::AddPoint( int nPlot, int nPoint )
{
#ifdef USE_QWT
    graph_dlg->AddPoint( nPlot, nPoint );
#endif
}

void GraphWindow::Show( const char * capAdd )
{
#ifdef USE_QWT
    if( graph_dlg )
        graph_dlg->ShowNew(capAdd);
#endif
}

GraphData *GraphWindow::getGraphData() const
{
#ifdef USE_QWT
    return &graph_dlg->gr_data;
#else
    return nullptr;
#endif
}

