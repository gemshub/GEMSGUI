#include <QObject>
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
    // alloc memory
    m_chartData.reset( allocateData( aPlots, aTitle, aXName, aYName, aAxisType[4] ));
    // get data from settings
    m_chartData->setBackgroundColor( QColor(aAxisType[1], aAxisType[2], aAxisType[3]) );
    m_chartData->setAxisTypes( aAxisType[0], aAxisType[5] );
    for(int i=0; i<4; i++ )
    {
        m_chartData->region[i] = static_cast<double>(sizeReg[i]);
        m_chartData->part[i]   = static_cast<double>(sizePart[i]);
    }

    for (uint ii=0; ii<m_chartData->linesNumber(); ++ii)
    {
        auto seriesdata = convertor(aLinesDesc[ii]);
        auto plot =  m_chartData->getPlot( ii );
        if( plot >= 0 && m_plotModels[plot]->absCount() <= seriesdata.getXColumn() )
            seriesdata.setXColumn(0);
        m_chartData->setLineData( ii, seriesdata );
    }

    graph_dlg = new GraphDialog( pmodule, m_chartData.get() );
    TCModuleImp *topw =	 qobject_cast<TCModuleImp *>( pmodule->window());
    if( topw )
    {

        QObject::connect( graph_dlg, SIGNAL( dataChanged( jsonui::ChartData* ) ),
                          topw,  SLOT( saveGraphData( jsonui::ChartData* ) ) );
        //connect( _page, SIGNAL( updateGraphWindow() ),
        //         graph_dlg,  SLOT( UpdateAll() ) );
    }
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
    // alloc memory
    m_chartData.reset( allocateData( aPlots, aTitle, aXName, aYName, agraphType ));

    // change names
    for (uint ii=0; ii<line_names.GetCount(); ++ii)
    {
        if( ii > m_chartData->linesNumber() )
            break;
        m_chartData->setLineData( ii,  std::string(line_names[ii].c_str())  );
    }

    graph_dlg = new GraphDialog( pmodule, m_chartData.get() );
    TCModuleImp *topw =	 qobject_cast<TCModuleImp *>( pmodule->window());
    if( topw )
    {

        QObject::connect( graph_dlg, SIGNAL( dataChanged( jsonui::ChartData* ) ),
                          topw,  SLOT( saveGraphData( jsonui::ChartData* ) ) );
        //connect( _page, SIGNAL( updateGraphWindow() ),
        //         graph_dlg,  SLOT( UpdateAll() ) );
    }
#endif
    pVisorImp->openMdiChild( graph_dlg );
}

GraphWindow::~GraphWindow()
{
    cout << "delete graph_dlg" << endl;
    if(graph_dlg)
        graph_dlg->close();
    delete graph_dlg;
}

#ifndef USE_QWT


SeriesLineData convertor( const TPlotLine& plotData )
{
    SeriesLineData data( plotData.getName().c_str(),
                         plotData.getType(), plotData.getSize(),
                         plotData.getLineSize(),
                         1, 0, // could be add: penStyle  end  spline now default
                         plotData.getColor()  );
    data.setXColumn(plotData.getIndex());
    return data;
}

TPlotLine convertor( const SeriesLineData& serData )
{
    TPlotLine data( serData.getName().c_str(), serData.getMarkerShape(), serData.getMarkerSize(),
                    serData.getPenSize(), serData.getXColumn(),
                    // could be add: penStyle  end  spline
                    serData.getColor()  );
    return data;
}


ChartData *GraphWindow::allocateData( TIArray<TPlot>& aPlots,
                                      const char * aTitle, const char *aXName, const char *aYName, int agraphType  )
{
    uint ii;
    /// Descriptions of model extracting data
    std::vector<std::shared_ptr<jsonui::ChartDataModel>> chartModels;

    for ( ii=0; ii<aPlots.GetCount(); ++ii)
    {
        m_plotModels.push_back( std::shared_ptr<PlotModel>( new PlotModel( aPlots[ii] )) );
        chartModels.push_back( std::shared_ptr<ChartDataModel>( new ChartDataModel( m_plotModels.back().get() )) );
        chartModels.back()->setXColumns(aPlots[ii].xColumns());
        chartModels.back()->setYColumns(aPlots[ii].yColumns(), true);
    }
    return new ChartData( chartModels, aTitle, aXName, aYName, agraphType );
}


#endif

void GraphWindow::AddPoint( int nPlot, int nPoint )
{
    if( graph_dlg ) {

#ifdef USE_QWT
        graph_dlg->AddPoint( nPlot, nPoint );
#else
        for( const auto& datamodel: m_plotModels)
            datamodel->resetMatrixData();
#endif
    }
}

void GraphWindow::ShowGraph( const char * capAdd )
{
    if( graph_dlg ){
#ifdef USE_QWT
        graph_dlg->ShowNew(capAdd);
#else
        //graph_dlg->UpdatePlots(capAdd);
        graph_dlg->setWindowTitle(capAdd);
        for( const auto& datamodel: m_plotModels)
            datamodel->resetMatrixData();
#endif
    }
}

void *GraphWindow::getGraphData() const
{
#ifdef USE_QWT
    return &graph_dlg->gr_data;
#else
    return m_chartData.get();
#endif
}

