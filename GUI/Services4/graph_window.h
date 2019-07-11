#ifndef GRAPH_WINDOW_H
#define GRAPH_WINDOW_H

#include "graph.h"

#ifndef USE_QWT
#include "graph_data.h"
#include "plot_model.h"
#endif

#ifdef USE_QWT
class GraphDialog;
#else
namespace jsonui {
class GraphDialog;
}
using namespace jsonui;
SeriesLineData convertor( const TPlotLine& plotData );

#endif

class TCModule;

class GraphWindow
{

#ifndef USE_QWT
    /// Description of 2D plotting widget
    std::shared_ptr<jsonui::ChartData> m_chartData;
    /// Description of 2D modelS
    std::vector<std::shared_ptr<PlotModel>> m_plotModels;

    ChartData *allocateData( TIArray<TPlot>& aPlots,
           const char * aTitle, const char *aXName, const char *aYName, int agraphType  );
#endif

public:

    GraphDialog *graph_dlg;



    GraphWindow( TCModule *pmodule, TIArray<TPlot>& aPlots, const char * title,
                 float *sizeReg, float *sizePrt,
                 TPlotLine * aLinesDesc, short *aAxisType,
                 const char *aXName = nullptr, const char *aYname = nullptr );

    GraphWindow( TCModule *pmodule, TIArray<TPlot>& aPlots, const char * title,
                 const char *aXName = nullptr, const char *aYname = nullptr,
                 TCStringArray line_names = 0, int agraphType = LINES_POINTS );

    ~GraphWindow();

    // Add new point to graph
    void AddPoint( int nPlot, int nPoint );
    void Show(const char * capAdd=nullptr); // new show all lines
    GraphData *getGraphData() const;

};


#endif // GRAPH_WINDOW_H
