#include "graph.h"
#include "graph_window.h"
#include "visor.h"

namespace jsonui {

void colorAt(int /*ndx*/, int /*size*/, int& /*red*/, int& /*green*/, int& /*blue*/)
{}

void GraphDialog::AddPoint(size_t nPlot, int nPoint)
{
    gui_logger->debug("GraphDialog::AddPoint {} {}", nPlot, nPoint);
}

void GraphDialog::ShowGraph(const char *capAdd)
{
    gui_logger->debug("GraphDialog::ShowGraph {}", capAdd);
}

}

TPlotLine convertor( const jsonui::SeriesLineData& serData )
{
    TPlotLine data( serData.getName().c_str(), serData.getMarkerShape(), serData.getMarkerSize(),
                    serData.getPenSize(), serData.getPenStyle(), serData.getSpline(), serData.getXColumn(),
                    // could be add: penStyle  end  spline
                    serData.getRed(), serData.getGreen(), serData.getBlue());
    return data;
}


jsonui::GraphDialog* updateGraphWindow(  jsonui::GraphDialog* /*graph_dlg*/,
                                 TCModule* /*pmodule*/, std::vector<TPlot>& /*aPlots*/,
                                 const char* /*aTitle*/,
                                 float* /*sizeReg*/, float* /*sizePart*/,
                                 TPlotLine* /*aLinesDesc*/, short* /*aAxisType*/,
                                 const char* /*aXName*/, const char* /*aYName*/)
{

    return nullptr;
}

jsonui::GraphDialog* updateGraphWindow(  jsonui::GraphDialog* /*graph_dlg*/,
                                       TCModule* /*pmodule*/, std::vector<TPlot>& /*aPlots*/,
                                       const char* /*aTitle*/,
                                       const char* /*aXName*/, const char* /*aYname*/,
                                       std::vector<std::string> /*line_names*/,
                                       const std::vector<TPlotLine>& /*def_plt_lines*/,
                                       int /*agraphType*/)
{

    return nullptr;
}
