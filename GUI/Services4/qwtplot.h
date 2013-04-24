//-------------------------------------------------------------------
// $Id: qwtplot.h 968 2007-12-13 13:23:32Z gems $
//
// Declarations of TPlotWidget class
// (Plotting system implementation)
//
// Copyright (C) 2013  S.Dmytriyeva
//
// This file is part of the GEM-Selektor GUI library which uses the
// Qt v.4 cross-platform App & UI framework (http://qt.nokia.com)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the terms of GEMS3 Development
// Quality Assurance Licence (GEMS3.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#ifndef QWTPLOTWIDGET_H
#define QWTPLOTWIDGET_H

#include <QWidget>

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_intervalcurve.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_point_3d.h>
#include <qwt_plot_picker.h>

#include "graph.h"

void setQwtSymbol( QwtSymbol* symbol, int type, int size, QColor color  );

///   Plotting area window
class TPlotWidget: public QWidget
{

    GraphData *gr_data;

    // data for different types of graphics
    // clear if replot
    QMultiMap<QString, QwtPlotCurve*> m_curves;
    //QList<QwtPlotCurve*> m_curves;
    QList<QwtPlotIntervalCurve*> m_intervalcurves;
    QwtPlotSpectrogram *d_spectrogram;

    // addition data
    // not clear if replot
    QList<QwtPlotMarker*> m_points;
    QList<QwtPlotMarker*> m_labels;

    // internal for all types of graphics
    QwtPlot *m_plot;
    QwtPlotGrid *m_grid;
    QwtPlotMarker *c_marker;
    QwtPlotPicker *m_picker;

    double minX;
    double maxX;
    double minY;
    double maxY;
    QFont axisFont;

    void showGrid();
    void showPlotLines();
    void showPlotLine( int nLine, int nPlot, int nLineinPlot );
    QwtPlotCurve *newCurve( int nLine );


    QPointF transform( QPoint pos );
    void showLegendText( QPointF pos, QString text );

    // for future
    void showIsoLines();
    void showCumulativeLines();
    void show3DLines(){}

public:

    TPlotWidget(GraphData* aGr_data, QWidget *parent = 0);
    ~TPlotWidget()
    {}

    void addPoint( int nPlot, int nPoint );
    void showPlot();
    void replotPlot( bool  isFragment );
    void replotPlotLine( int nLine );
    void setFragment( bool showFragment );
    // void printPlot();
    void savePlot();
    void showPicker();
    QwtPlot *getPlot() const
    { return m_plot; }

protected:
    virtual void mousePressEvent( QMouseEvent *e );
    void dragEnterEvent(QDragEnterEvent* event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent* event);
};


#endif // QWTPLOTWIDGET_H
