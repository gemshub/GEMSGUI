#ifndef GRAPH_WINDOW_H
#define GRAPH_WINDOW_H

#include "graph.h"
#include "graph_data.h"
#include "plot_model.h"
#include "GraphDialogN.h"
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
                                         TCStringArray line_names,
                                         const std::vector<TPlotLine>& def_plt_lines,
                                         int agraphType = LINES_POINTS  );

#endif // GRAPH_WINDOW_H
