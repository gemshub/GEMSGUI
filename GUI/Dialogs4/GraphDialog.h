//-------------------------------------------------------------------
// $Id: GraphDialog.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of GraphDialog classes (Plotting system)
//
// Copyright (C) 1996-2008  A.Rysin, S.Dmytriyeva
// Uses  gstring class (C) A.Rysin 1999
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
#ifndef graph_dialog_h
#define graph_dialog_h

#include <qcolor.h>
#include <QResizeEvent>
#include <QDialog>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QItemDelegate>

#include "ui_GraphDialog4.h"
#include  "pshape.h"
#include  "graph.h"

// Widgets for legend and plotting dialogs

void drawSymbol(QPainter* painter, const QPoint& center,
         int type, int size, const QColor& color, int width=1);

void paintIcon( QIcon &icon, TPlotLine &plLine );

//=========================================================================
// Added for new legend table


class DragTableWidget: public QTableWidget
{
    QPoint startPos;

    public:
        DragTableWidget(QWidget* parent):
            QTableWidget(parent) {}

        virtual ~DragTableWidget() {}

    protected:

    void startDrag();
    void mousePressEvent( QMouseEvent *e );
    void mouseMoveEvent( QMouseEvent *e );
};



// class LabelDelegate
// individual items in views are rendered and edited using delegates

class LabelDelegate: public QItemDelegate
{
   Q_OBJECT

 public:

   LabelDelegate( QObject * parent = 0 );
   QWidget *createEditor(QWidget *parent,
                         const QStyleOptionViewItem &option,
                         const QModelIndex &index) const;
};

//=========================================================================

class GraphDialog: public QDialog, public Ui::GraphDialogData
{
    Q_OBJECT

    TPlotWin* plot;
    TCModule *pModule;
    DragTableWidget *tbLegend;
    LabelDelegate *dgLegend;

    // work part
    bool isFragment;
    float minX, minY;
    float maxX, maxY;
    bool isoline_put;
    bool lines_put;


    void Show();
    void ShowPlots();
    void ShowLegend();
    void ShowIsolineLegend();
    //void paintIcon( QIcon &icon, TPlotLine &plLine );

    void closeEvent(QCloseEvent*);


protected slots:
    void changeIcon( int row, int column );
    virtual void languageChange();

    virtual void CmFragment();
    virtual void CmLegend();
    virtual void CmPrint();
    virtual void CmSave();
    virtual void CmHelp();

protected:
    void resizeEvent(QResizeEvent* qpev);

public:

    GraphData gr_data;

    GraphDialog(TCModule *pmodule, GraphData& data);
    ~GraphDialog();

    gstring moduleName() const
    {  return  (gstring("gr")+pModule->GetName());   }

    gstring iconFile() const
    {  return  pModule->GetIcon();   }


    int rtNum() const
    {  return  pModule->rtNum();   }

    gstring mainModuleName() const
    {  return  pModule->GetName();   }


    void ShowNew( const char *capAdd );
    void Apply();    // Update changes
    void ApplyLegend();
    void AddPoint( int nPlot, int nPoint, bool no_mt ); // Add new point to graph
    //void paintIcon( QIcon &icon, TPlotLine &plLine );

    QColor getColor(int ii) const
    {
        return QColor( gr_data.lines[ii].red,
        	    gr_data.lines[ii].green, gr_data.lines[ii].blue);
    }

    QColor getColorIsoline(int ii) const
    {
        return QColor( gr_data.scale[ii].red,
        	    gr_data.scale[ii].green, gr_data.scale[ii].blue);
    }

    gstring getTextIsoline(int ii);

    QColor getBackgrColor();
    void setBackgrColor( QColor color );
};



#endif   // graph_dialog_h
