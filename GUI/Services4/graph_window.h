#ifndef GRAPH_WINDOW_H
#define GRAPH_WINDOW_H

#include "graph.h"
#include "graph_data.h"

#ifndef NO_GUI
#include "GraphDialogN.h"
#else

namespace jsonui {

class GraphDialog {
public:
    void AddPoint(size_t nPlot, int nPoint);
    void ShowGraph(const char * capAdd = nullptr);
};

}
#endif

jsonui::SeriesLineData convertor( const TPlotLine& plotData );
TPlotLine convertor( const jsonui::SeriesLineData& serData );
class TCModule;



jsonui::GraphDialog* updateGraphWindow(  jsonui::GraphDialog* graph_dlg,
                                         TCModule *pmodule, std::vector<TPlot>& aPlots,
                                         const char * aTitle,
                                         float *sizeReg,  float * sizePart,
                                         TPlotLine* aLinesDesc, short *aAxisType,
                                         const char *aXName, const char *aYName );

jsonui::GraphDialog* updateGraphWindow(  jsonui::GraphDialog* graph_dlg,
                                         TCModule *pmodule, std::vector<TPlot>& aPlots,
                                         const char * aTitle,
                                         const char *aXName, const char *aYname,
                                         std::vector<std::string> line_names,
                                         const std::vector<TPlotLine>& def_plt_lines,
                                         int agraphType = LINES_POINTS  );

#endif // GRAPH_WINDOW_H
