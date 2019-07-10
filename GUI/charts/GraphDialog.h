//  This is JSONUI library+API (https://bitbucket.org/gems4/jsonui)
//
/// \file GraphDialog.h
/// Declaration of class GraphDialog plotting system main dialog
//
// JSONUI is a C++ Qt5-based widget library and API aimed at implementing 
// the GUI for editing/viewing the structured data kept in a NoSQL database,
// with rendering documents to/from JSON/YAML/XML files, and importing/
// exporting data from/to arbitrary foreign formats (csv etc.). Graphical 
// presentation of tabular data (csv format fields) is also possible.
//
// Copyright (c) 2017 Svetlana Dmytriieva (svd@ciklum.com) and
//   Dmitrii Kulik (dmitrii.kulik@psi.ch)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU (Lesser) General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.
//
// JSONUI depends on the following open-source software products:
// JSONIO (https://bitbucket.org/gems4/jsonio); Qt5 (https://qt.io).
//

#ifndef graph_dialog_h
#define graph_dialog_h

#include <QDialog>
#include <QTableWidget>
#include <QItemDelegate>

namespace Ui {
class GraphDialogData;
}

void helpWin( const std::string& name, const std::string& item );

namespace jsonui {

class DragTableWidget;
class LabelDelegate;
class PlotChartView;
class ChartData;

/// \class GraphDialog plotting system main dialog
class GraphDialog: public QDialog
{
    friend class LegendDialog;

    Q_OBJECT

    void ShowLegend();
//    void ShowIsolineLegend();

protected slots:

    void changeIcon( int row, int column );
    void changeNdx( int row, int column );

    void CmFragment();
    void CmLegend();
    void CmPrint();
    void CmSaveImage();
    void CmHelp();

public slots:

    /// Update graphic and legend
    void UpdateAll(const char* title=nullptr );

signals:
    void dataChanged( ChartData *achartData );

public:
     GraphDialog( ChartData *data, QWidget *parent,
                  const char *title= "Graphics Dialog" );
    ~GraphDialog();

    /// Update all graphic lines with new title
    void UpdatePlots( const char *title );

private:

    Ui::GraphDialogData *ui;
    QPushButton* bFragment;
    PlotChartView* plot;
    ChartData* gr_data;

    DragTableWidget* tbLegend;
    LabelDelegate* dgLegend;

    // work part
    bool isFragment;
};


//=========================================================================

/// Drag Table Widget - added for legend table
class DragTableWidget: public QTableWidget
{
    QPoint startPos;

    public:
        DragTableWidget(QWidget* parent):
            QTableWidget(parent) {}

        virtual ~DragTableWidget() {}

    protected:

    void startDrag(/*Qt::DropActions supportedActions*/);
    void mousePressEvent( QMouseEvent *e );
    void mouseMoveEvent( QMouseEvent *e );
};

/// \class LabelDelegate
/// Individual items in views are rendered and edited using delegates
class LabelDelegate: public QItemDelegate
{
   Q_OBJECT

  ChartData *grData;

 public:

   LabelDelegate( ChartData *agr_data, QObject * parent = nullptr ):
       QItemDelegate( parent ), grData(agr_data)
   {}

   QWidget *createEditor(QWidget *parent,
                         const QStyleOptionViewItem &option,
                         const QModelIndex &index) const;
   void setModelData(QWidget *editor, QAbstractItemModel *model,
                                    const QModelIndex &index) const;
};

} // namespace jsonui

#endif   // graph_dialog_h
