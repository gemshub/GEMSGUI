#include "graph.h"
#include "graph_window.h"

TPlotLine convertor( const jsonui::SeriesLineData& serData )
{
    TPlotLine data( serData.getName().c_str(), serData.getMarkerShape(), serData.getMarkerSize(),
                    serData.getPenSize(), serData.getPenStyle(), serData.getSpline(), serData.getXColumn(),
                    // could be add: penStyle  end  spline
                    serData.getColor()  );
    return data;
}


jsonui::GraphDialog* updateGraphWindow(  jsonui::GraphDialog* graph_dlg,
                                 TCModule *pmodule, std::vector<TPlot>& aPlots,
                                 const char * aTitle,
                                 float *sizeReg,  float * sizePart,
                                 TPlotLine* aLinesDesc, short *aAxisType,
                                 const char *aXName, const char *aYName )
{

    return nullptr;
}

jsonui::GraphDialog* updateGraphWindow(  jsonui::GraphDialog* graph_dlg,
                                       TCModule *pmodule, std::vector<TPlot>& aPlots,
                                       const char * aTitle,
                                       const char *aXName, const char *aYname,
                                       std::vector<std::string> line_names,
                                       const std::vector<TPlotLine>& def_plt_lines,
                                       int agraphType)
{

    return nullptr;
}
