//-------------------------------------------------------------------
// $Id$
//
// Declaration of GraphDialog, PlotTypeBtn classes (Plotting system)
//
// Copyright (C) 1996-2001  A.Rysin, S.Dmytriyeva
// Uses  gstring class (C) A.Rysin 1999
//
// This file is part of the GEM-Vizor library which uses the
// Qt v.2.x GUI Toolkit (Troll Tech AS, http://www.trolltech.com)
// according to the Qt Duo Commercial license  
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL)
//
// See http://les.web.psi.ch/Software/GEMS-PSI/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------
#ifndef graph_dialog_h
#define graph_dialog_h

#include <errno.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qdialog.h>
#include <qcolor.h>

#include "GraphDialogData.h"

#include  "array.h"
#include  "v_module.h"
#include  "pshape.h"
#include  "graph.h"

// Widgets for legend and plotting dialogs

class PlotTypeBtn:
            public QPushButton
{
    TPlotLine plLine;

protected:
    void drawButtonLabel(QPainter* paint);

public:
    PlotTypeBtn(  TPlotLine& ln, QWidget* parent=0, const char* name=0):
            QPushButton( parent, name ), plLine(ln)
    { }


    void setName( const char* );
    void setData( TPlotLine& ln);
    TPlotLine& getData()
    {
        return plLine;
    }
};


class GraphDialog:
            public GraphDialogData
{
    Q_OBJECT

    TPlotWin* plot;
    TCModule *pModule;

    // work part
    bool isFragment;
    float minX, minY;
    float maxX, maxY;
    TIArray<PlotTypeBtn> aLegendButtons;

    void Show();
    void ShowPlots();


protected slots:

    void CmLegend();
    void CmFragment();
    void CmPrint();

protected:
    void resizeEvent(QResizeEvent* qpev);

public:

    GraphData gr_data;

    GraphDialog( TCModule *pmodule, GraphData& data);
    ~GraphDialog();


    void ShowNew();
    void Apply();    // Update changes
    void AddPoint( int nPlot, int nPoint ); // Add new point to graph

    QColor getColor(int ii) const
    {
        return QColor( gr_data.lines[ii].red,
        	    gr_data.lines[ii].green, gr_data.lines[ii].blue);
    }

    QColor getBackgrColor();
    void setBackgrColor( QColor color );
};


#endif   // graph_dialog_h
