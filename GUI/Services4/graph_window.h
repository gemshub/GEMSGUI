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
#include "GraphDialogN.h"
//using namespace jsonui;
jsonui::SeriesLineData convertor( const TPlotLine& plotData );
TPlotLine convertor( const jsonui::SeriesLineData& serData );

#endif

class TCModule;

#ifdef USE_QWT


class GraphWindow
{


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
    void ShowGraph(const char * capAdd=nullptr); // new show all lines
    void *getGraphData() const;

};


GraphWindow *updateGraphWindow(  GraphWindow* graph_wnd,
                                 TCModule *pmodule, TIArray<TPlot>& aPlots,
                                 const char * aTitle,
                                 float *sizeReg,  float * sizePart,
                                 TPlotLine* aLinesDesc, short *aAxisType,
                                 const char *aXName, const char *aYName );
GraphWindow *updateGraphWindow(  GraphWindow* graph_wnd,
                                 TCModule *pmodule, TIArray<TPlot>& aPlots,
                                 const char * aTitle,
                                 const char *aXName, const char *aYName,
                                 TCStringArray line_names, int agraphType = LINES_POINTS   );


#else


jsonui::GraphDialog* updateGraphWindow(  jsonui::GraphDialog* graph_dlg,
                                         TCModule *pmodule, TIArray<TPlot>& aPlots,
                                         const char * aTitle,
                                         float *sizeReg,  float * sizePart,
                                         TPlotLine* aLinesDesc, short *aAxisType,
                                         const char *aXName, const char *aYName );

jsonui::GraphDialog* updateGraphWindow(  jsonui::GraphDialog* graph_dlg,
                                         TCModule *pmodule, TIArray<TPlot>& aPlots,
                                         const char * aTitle,
                                         const char *aXName, const char *aYname,
                                         TCStringArray line_names, int agraphType = LINES_POINTS  );
#endif

#endif // GRAPH_WINDOW_H
