//  This is JSONUI library+API (https://bitbucket.org/gems4/jsonui)
//
/// \file GraphDialog.cpp
/// Implementation of class GraphDialog plotting system main dialog
//
// JSONUI is a C++ Qt5-based widget library and API aimed at implementing 
// the GUI for editing/viewing the structured data kept in a NoSQL database,
// with rendering documents to/from JSON/YAML/XML files, and importing/
// exporting data from/to arbitrary foreign formats (csv etc.). Graphical 
// presentation of tabular data (csv format fields) is also possible.
//
// Copyright (c) 2015-2016 Svetlana Dmytriieva (svd@ciklum.com) and 
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

#include <QPrintDialog>
#include <QColorDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QMimeData>
#include <QMouseEvent>
#include <QDrag>
#include <QFileDialog>
#include <QFileInfo>
#include <QImageWriter>
#include <QtPrintSupport/QPrinter>
#include <QPainter>
#include <QHeaderView>

#include "ui_GraphDialog4N.h"
#include "GraphDialogN.h"
#include "LegendDialogN.h"
#include "SymbolDialogN.h"
#include "chart_view.h"

#include "GemsMainWindow.h"
#include "visor.h"

namespace jsonui {

/// The constructor
GraphDialog::GraphDialog( TCModule *pmodule, const std::shared_ptr<jsonui::ChartData>& data1,
                          const std::vector<std::shared_ptr<PlotModel>>& aplotModels,
                          const string& title ):
        QDialog( pmodule->window() ), pModule(pmodule), gr_data(data1), plotModels(aplotModels),
        ui(new Ui::GraphDialogData),
        isFragment(false)
{
    ui->setupUi(this);
    setWindowTitle( title.c_str() );
    setAttribute( Qt::WA_DeleteOnClose );

    ui->splitter->setStretchFactor(0, 4);
    ui->splitter->setStretchFactor(1, 0);

    // Define legend table
    tbLegend = new DragTableWidget( this );
    tbLegend->setSelectionMode(QAbstractItemView::NoSelection);
    tbLegend->setColumnCount( 3 );
    tbLegend->setColumnWidth(0, 35 );
    tbLegend->setColumnWidth(1, 35 );
    tbLegend->horizontalHeader()->setStretchLastSection( true);
    tbLegend->verticalHeader()->setVisible(false);
    tbLegend->horizontalHeader()->setVisible(false);

    LabelDelegate *dgLegend = new LabelDelegate( this );
    tbLegend->setItemDelegate(dgLegend);
    ui->verticalLayout->addWidget( tbLegend );

    // define plot window
    plot = new PlotChartView( gr_data.get(), this);
    ui->verticalLayout_2->addWidget( plot);
    // Insert labels in legend box
    if( gr_data->getGraphType() == LineChart || gr_data->getGraphType() == AreaChart )
      ShowLegend();


    connect(tbLegend, SIGNAL(cellClicked(int,int)),
            this, SLOT(changeIcon(int,int)));
    connect(tbLegend, SIGNAL(cellChanged(int,int)),
            this, SLOT(changeNdx(int,int)));

    bFragment =ui->buttonBox->addButton("&Fragment", QDialogButtonBox::ActionRole );
    QPushButton* bCustomize =ui->buttonBox->addButton("&Customize", QDialogButtonBox::ActionRole );
    QPushButton* bPrint =ui->buttonBox->addButton("&Print", QDialogButtonBox::ActionRole );
    QPushButton* bSaveImage =ui->buttonBox->addButton("&Save Image", QDialogButtonBox::ActionRole );

    //QObject::connect( ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    QObject::connect( ui->buttonBox->button(QDialogButtonBox::Close), SIGNAL(clicked()), this, SLOT(close()));
    QObject::connect( ui->buttonBox, SIGNAL(helpRequested()), this, SLOT(CmHelp()));

    QObject::connect( bFragment, SIGNAL(clicked()), this, SLOT(CmFragment()));
    QObject::connect( bCustomize, SIGNAL(clicked()), this, SLOT(CmLegend()));
    QObject::connect( bPrint, SIGNAL(clicked()), this, SLOT(CmPrint()));
    QObject::connect( bSaveImage, SIGNAL(clicked()), this, SLOT(CmSaveImage()));
    QObject::connect( plot, SIGNAL(savetoFile()), this, SLOT(CmSaveImage()));

    QObject::connect( plot, SIGNAL(fragmentChanged(QRectF)), this, SLOT(updateFragment(QRectF)));

}

GraphDialog::~GraphDialog()
{
    delete tbLegend;
    delete ui;
}

void GraphDialog::resetGraphDialog(const std::shared_ptr<ChartData> &data1,
                                   const std::vector<std::shared_ptr<PlotModel> > &aplotModels,
                                   const string &title)
{
  gr_data = data1;
  plotModels = aplotModels;
  UpdateAll(title.c_str() );
}

void GraphDialog::closeEvent(QCloseEvent *ev)
{
    pModule->ClearGraphDialog();
    pVisorImp->closeMdiChild( this );
    ev->accept();
}

void GraphDialog::UpdatePlots( const char* title )
{
    if( title )
     setWindowTitle( title );
    plot->updateLines();
}

void GraphDialog::AddPoint(size_t nmodel, int npoint )
{
    auto model = (nmodel<plotModels.size() ? plotModels[nmodel]: nullptr);
    if( model )
    {
        auto indexTop = model->index(npoint, 0);
        auto indexBottom = model->index(npoint, model->columnCount( indexTop )-1);
        emit model->dataChanged( indexTop, indexBottom );
    }
    //for( const auto& datamodel: plotModels)
    //     datamodel->resetMatrixData();
}

void GraphDialog::ShowGraph(const char *capAdd)
{
    setWindowTitle(capAdd);
    for( const auto& datamodel: plotModels)
        datamodel->resetMatrixData();
}

void GraphDialog::UpdateAll(const char* title )
{
    if( title )
     setWindowTitle( title );

    disconnect(tbLegend, SIGNAL(cellClicked( int , int  ) ),
            this, SLOT(changeIcon( int, int )));
    disconnect(tbLegend, SIGNAL(cellChanged( int , int  ) ),
            this, SLOT(changeNdx( int, int )));

    // Insert labels in legend box
    if( gr_data->getGraphType() == LineChart || gr_data->getGraphType() == AreaChart )
      ShowLegend();

    plot->updateAll();
    //emit dataChanged( gr_data );

    connect(tbLegend, SIGNAL(cellClicked(int,int)),
            this, SLOT(changeIcon(int,int)));
    connect(tbLegend, SIGNAL(cellChanged(int,int)),
            this, SLOT(changeNdx(int,int)));
}


// slots ------------------------------------

void GraphDialog::CmLegend()
{
    LegendDialog legend( this );
    legend.exec();
}

void GraphDialog::CmFragment()
{
    if(  isFragment == false )
    {
        isFragment = true;
        bFragment->setText("&Full");
    }
    else
    {
        isFragment = false;
        bFragment->setText("&Fragment");
    }
    plot->updateFragment( isFragment );
}

void GraphDialog::CmSaveImage()
{
    auto fname_default = gr_data->title.substr(0, 30);
    if( fname_default.empty() )
        fname_default =  "My_plot";
    QString  fileName  =  pVisor->filePathFromName(fname_default, "pdf").c_str();

    const QList<QByteArray> imageFormats =
            QImageWriter::supportedImageFormats();

    QStringList filter;
    filter.clear();
    filter += "PDF Documents (*.pdf)";
    filter += "SVG Documents (*.svg)";

    if ( imageFormats.size() > 0 )
    {
        QString imageFilter;
        for ( int i = 0; i < imageFormats.size(); i++ )
        {

            imageFilter = imageFormats[i].toUpper();
            imageFilter += "   Image (*.";
            imageFilter +=  imageFormats[i];
            imageFilter += ")";
            filter += imageFilter;
        }
    }

    QString selectedFilter;
    fileName = QFileDialog::getSaveFileName(
                this, "Saving Graphics Image", fileName,
                filter.join( ";;" ), &selectedFilter, QFileDialog::DontConfirmOverwrite );

    if ( !fileName.isEmpty() )
    {
        QFileInfo finfo(fileName);
        // can be saved current finfo.Dir()
        if( finfo.suffix().isEmpty() ) // solwing for linux
        {
            int posb = selectedFilter.lastIndexOf(".");
            int pose = selectedFilter.indexOf(")", posb);
            QString ext = selectedFilter.mid(posb+1, pose-posb-1);
            fileName += "."+ext;
        }

        pVisor->setLocalDir( finfo.dir().absolutePath().toStdString() );
        plot->renderDocument( gr_data->title.c_str(), fileName );
    }
}

// Print graph screen to printer device
void GraphDialog::CmPrint()
{

    QPrinter printer( QPrinter::PrinterResolution/*QPrinter::HighResolution*/ );

    QPrintDialog dialog( &printer );
    if ( dialog.exec()  )
    {
        printer.setDocName( gr_data->title.c_str() );
        QPainter p( &printer );
        plot->render( &p );
    }

}

void GraphDialog::CmHelp()
{
    helpWin( "gems_graph", "" );
}

// Insert labels in legend box
void GraphDialog::ShowLegend()
{
    QTableWidgetItem *itemL, *itemN;
    Qt::ItemFlags flags;

    tbLegend->setAcceptDrops(true);
    tbLegend->setEditTriggers( QAbstractItemView::DoubleClicked|QAbstractItemView::AnyKeyPressed );

    tbLegend->clear();
    tbLegend->setRowCount( gr_data->linesNumber() );

    int nLines = 0;
    for( size_t mii=0; mii<gr_data->modelsNumber(); mii++ )
    {
        auto nLinN =  gr_data->modelData( mii )->getSeriesNumber();
        for( uint jj=0; jj<nLinN; jj++, nLines++ )
        {
            tbLegend->setRowHeight(nLines, 21/*htF(ftString, 0)+4*/);

           auto icon = markerShapeIcon(gr_data->lineData(nLines) );
           itemL = new QTableWidgetItem(icon, "");
           flags = itemL->flags();
           itemL->setFlags(flags & ~Qt::ItemIsEditable);
           itemL->setToolTip("Legend column 1");
           tbLegend->setItem(nLines, 0, itemL );

           auto indexName = gr_data->modelData( mii )->abscissaIndexName(gr_data->lineData(nLines).getXColumn());
           itemN = new QTableWidgetItem(tr("%1").arg(indexName));
           itemN->setToolTip("Legend column 2");
           tbLegend->setItem(nLines, 1, itemN );

           itemN = new QTableWidgetItem(tr("%1").arg(gr_data->lineData(nLines).getName().c_str()));
           itemN->setToolTip("Legend column 3");
           tbLegend->setItem(nLines, 2, itemN );
         }
    }
}

void GraphDialog::changeIcon( int rowi, int column )
{
     auto row = static_cast<size_t>(rowi);
    if( column == 0 )
    {
     if( gr_data->getGraphType() == LineChart || gr_data->getGraphType() == AreaChart )
     {
            SymbolDialog cd( gr_data->lineData(row), this);
            if( cd.exec() )
            {
               gr_data->setLineData( row, cd.GetPlotLine() );
               auto icon = markerShapeIcon(gr_data->lineData(row) );
               tbLegend->item(rowi, column)->setIcon(icon);
               plot->updateLine(row);
               emit dataChanged( gr_data.get() );
            }
     }
   }    else if( column ==  2  )
            {
               highlightRow( row );
            }

}

void GraphDialog::changeNdx( int rowi, int column )
{
    auto row = static_cast<size_t>(rowi);
    if( column == 1 &&  ( gr_data->getGraphType() == LineChart || gr_data->getGraphType() == AreaChart ) )
    {
       auto ndxX = tbLegend->item(rowi, column)->text();
       gr_data->setLineData( row, ndxX );
       if( gr_data->getGraphType() == LineChart )
            plot->updateLine( row );
       else if( gr_data->getGraphType() == AreaChart )
              plot->updateAll();
       emit dataChanged( gr_data.get() );
    }
    if( column == 2 /*&&  gr_data->graphType == LineChart*/ )
    {
        std::string  name = tbLegend->item(rowi, column)->text().toStdString();
        gr_data->setLineData(static_cast<size_t>(row), name );
        emit dataChanged( gr_data.get() );
    }
}

void GraphDialog::highlightRow( size_t row )
{
    if( activeRow == row ) // toggle
    {
        restoreRow();
        return;
    }
    restoreRow();
    activeRow = row;
    plot->highlightLine(row, true);
}

void GraphDialog::restoreRow()
{
    if( activeRow != string::npos)
        plot->highlightLine(activeRow, false);
    activeRow = string::npos;
}

void GraphDialog::updateFragment(QRectF  rect)
{
    gr_data->part[0] = rect.left();
    gr_data->part[1] = rect.right();
    gr_data->part[2] = rect.bottom();
    gr_data->part[3] = rect.top();
    isFragment = true;
    bFragment->setText("&Full");
    plot->resetFragment( isFragment );
    emit dataChanged( gr_data.get() );
}

//=======================================================================================
// Added for new legend table

void DragTableWidget::startDragN(/*Qt::DropActions supportedActions*/)
{
    if( currentColumn() == 2 )
    {
        QTableWidgetItem *item = currentItem();
        QString text_ = item->text();

        QFontMetrics fm(this->font());
        QPixmap pixmap(fm.horizontalAdvance(text_)+2, fm.height()+2);
        pixmap.fill(QColor(0, 0, 0, 0));
        QPainter dc(&pixmap);
        dc.drawText(2, fm.height()-2, text_ );
        dc.end();

        QMimeData * mimeData = new QMimeData;
        mimeData->setText( text_ );
        QDrag *drag = new QDrag(this);
        drag->setMimeData( mimeData );
        drag->setPixmap(pixmap/*QPixmap::grabWidget(this)*/);
        drag->setHotSpot( QPoint(0, pixmap.height()/2) );
        //drag->setHotSpot( QPoint( pixmap.width() / 2, pixmap.height() / 2 ) );
        drag->exec();
     }
 }

void DragTableWidget::mousePressEvent( QMouseEvent *e )
{
        if(e->buttons() & Qt::LeftButton )
          startPos = e->pos();
        QTableWidget::mousePressEvent(e);
 }

void DragTableWidget::mouseMoveEvent( QMouseEvent *e )
{
        if(e->buttons() & Qt::LeftButton )
        { int delta = (e->pos()-startPos).manhattanLength();
            if( delta >= QApplication::startDragDistance() )
              startDragN();
        }
        QTableWidget::mouseMoveEvent(e);
}

void DragTableWidget::focusOutEvent(QFocusEvent *event)
{
    topDlg->restoreRow();
    QTableWidget::focusOutEvent(event);
}

//--------------------------------------------------------------------------

// Editing  Table of Labels
QWidget *LabelDelegate::createEditor(QWidget *parent,
                                     const QStyleOptionViewItem &,
                                     const QModelIndex &index) const
{

    if( index.column() == 2 )
    {
        QLineEdit *editor =  new QLineEdit( parent);
        editor->setMaxLength( 30 );
        return editor;
    }
    if( index.column() == 1 )
    {
        int ndx_plot = topDlg->gr_data->getPlot( static_cast<size_t>(index.row()) );
        if( ndx_plot >= 0 )
        {
            QStringList cbList = topDlg->gr_data->modelData(static_cast<size_t>(ndx_plot))->getAbscissaIndexes();
            if( cbList.count() < 2 ) // only one value correct
                return nullptr;
            QComboBox* editor =  new QComboBox( parent );
            editor->addItems(cbList);
            return editor;
        }
    }
    return nullptr;
}

void LabelDelegate::setModelData(QWidget *editor,
                                 QAbstractItemModel *model,
                                 const QModelIndex &index) const
{
   if( index.column() == 1)
   {
       QComboBox *cellEdit = dynamic_cast<QComboBox*>(editor);
       if( cellEdit )
             model->setData(index, cellEdit->currentText(),  Qt::EditRole);
   }
   else
       QItemDelegate::setModelData( editor, model, index);
 }

} // namespace jsonui

//--------------------- End of GraphDialog.cpp ---------------------------
