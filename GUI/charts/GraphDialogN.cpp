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


namespace jsonui {

/// The constructor
GraphDialog::GraphDialog( TCModule *pmodule, ChartData *data, const string& title ):
        QDialog( pmodule->window() ), pModule(pmodule), ui(new Ui::GraphDialogData),
        gr_data(data), isFragment(false)
{
    ui->setupUi(this);
    setWindowTitle( title.c_str() );

    // define plot window
    plot = new PlotChartView( gr_data, this);
    ui->verticalLayout_2->addWidget( plot);

   // Define legend table
    tbLegend = new DragTableWidget( this );
    tbLegend->setSelectionMode(QAbstractItemView::NoSelection);
    tbLegend->setColumnCount( 3 );
    tbLegend->setColumnWidth(0, 30 );
    tbLegend->setColumnWidth(1, 30 );
    tbLegend->horizontalHeader()->setStretchLastSection( true);
    tbLegend->verticalHeader()->setVisible(false);
    tbLegend->horizontalHeader()->setVisible(false);

    LabelDelegate *dgLegend = new LabelDelegate( gr_data );
    tbLegend->setItemDelegate(dgLegend);
    ui->verticalLayout->addWidget( tbLegend );

    ui->splitter->setStretchFactor(0, 5);
    ui->splitter->setStretchFactor(1, 1);

    // Insert labels in legend box
    if( gr_data->graphType == LineChart || gr_data->graphType == AreaChart )
      ShowLegend();

    connect(tbLegend, SIGNAL(cellClicked( int , int  ) ),
            this, SLOT(changeIcon( int, int )));
    connect(tbLegend, SIGNAL(cellChanged( int , int  ) ),
            this, SLOT(changeNdx( int, int )));

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
}

GraphDialog::~GraphDialog()
{
    delete tbLegend;
    delete ui;
}

void GraphDialog::closeEvent(QCloseEvent *ev)
{
    ev->accept();
    pVisorImp->closeMdiChild( this );
}

void GraphDialog::UpdatePlots( const char* title )
{
    if( title )
     setWindowTitle( title );
   plot->updateLines();
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
    if( gr_data->graphType == LineChart || gr_data->graphType == AreaChart )
      ShowLegend();

    plot->updateAll();
    //emit dataChanged( gr_data );

    connect(tbLegend, SIGNAL(cellClicked( int , int  ) ),
            this, SLOT(changeIcon( int, int )));
    connect(tbLegend, SIGNAL(cellChanged( int , int  ) ),
            this, SLOT(changeNdx( int, int )));
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
    plot->setFragment( isFragment );
}

void GraphDialog::CmSaveImage()
{
   QString  fileName  =  "";//  can be get current dir
            fileName  +=  "image.pdf";

    const QList<QByteArray> imageFormats =
        QImageWriter::supportedImageFormats();

    QStringList filter;
    filter.clear();
    filter += "PDF Documents (*.pdf)";
#ifndef QWT_NO_SVG
    filter += "SVG Documents (*.svg)";
#endif
    filter += "Postscript Documents (*.ps)";

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
        QPixmap pixMap = plot->grab();
        pixMap.save(fileName);
    }
}

/*

 //====
 QImage image(fn);
 QPainter painter(&image);
 painter.setRenderHint(QPainter::Antialiasing);
 scene.render(&painter);
 image.save("file_name.png")
 //========
         scene->clearSelection();                                                  // Selections would also render to the file
         scene->setSceneRect(scene->itemsBoundingRect());                          // Re-shrink the scene to it's bounding contents
         QImage image(scene->sceneRect().size().toSize(), QImage::Format_ARGB32);  // Create the image with the exact size of the shrunk scene
         image.fill(Qt::transparent);                                              // Start all pixels transparent

         QPainter painter(&image);
         scene->render(&painter);
         image.save("file_name.png");
//=====
         QString fileName= QFileDialog::getSaveFileName(this, "Save image", QCoreApplication::applicationDirPath(), "BMP Files (*.bmp);;JPEG (*.JPEG);;PNG (*.png)" );
             if (!fileName.isNull())
             {
                 QPixmap pixMap = this->ui->graphicsView->grab();
                 pixMap.save(fileName);
             }
*/


// Print graph screen to printer device
void GraphDialog::CmPrint()
{

    QPrinter printer( QPrinter::PrinterResolution/*QPrinter::HighResolution*/ );

    QPrintDialog dialog( &printer );
    if ( dialog.exec()  )
    {

        //if( QPrinter::Landscape != printer.orientation() )
         //   printer.setOrientation(QPrinter::Landscape);

        QPainter painter;
        painter.begin(&printer);
        double xscale = printer.pageRect().width()/double(ui->framePrn->width());
        double yscale = printer.pageRect().height()/double(ui->framePrn->height());
        double scale = qMin(xscale, yscale);
        painter.translate(printer.paperRect().x() + printer.pageRect().width()/2,
                          printer.paperRect().y() + printer.pageRect().height()/2);
        painter.scale(scale, scale);
        painter.translate(-ui->framePrn->width()/2, -ui->framePrn->height()/2);

        /*QPainter painter(&printer);

        double dx = (double)(printer.widthMM()*printer.logicalDpiX())
                    /(double)(framePrn->widthMM()*framePrn->logicalDpiX());
        double dy = (double)(printer.heightMM()*printer.logicalDpiY())
                    /(double)(framePrn->heightMM()*framePrn->logicalDpiY());

        //cout << "dx  " << dx << " dy  " << dy << endl;
        dx = min(dx, dy);
        painter.scale(dx, dx);
        painter.setClipRect(framePrn->geometry());
        */
        ui->groupBox->hide();
        ui->framePrn->render( &painter );
        ui->groupBox->show();
    }

}

void GraphDialog::CmHelp()
{
    helpWin( "GraphDialogJsonui", "" );
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
     if( gr_data->graphType == LineChart || gr_data->graphType == AreaChart )
     {
            SymbolDialog cd( gr_data->lineData(row), this);
            if( cd.exec() )
            {
               gr_data->setLineData( row, cd.GetPlotLine() );
               auto icon = markerShapeIcon(gr_data->lineData(row) );
               tbLegend->item(rowi, column)->setIcon(icon);
               plot->updateLine(row);
               emit dataChanged( gr_data );
            }
     }
   }    else if( column ==  2 && gr_data->graphType == LineChart )
            {
               highlightRow( row );
            }

}

void GraphDialog::changeNdx( int rowi, int column )
{
    auto row = static_cast<size_t>(rowi);
    if( column == 1 &&  gr_data->graphType == LineChart )
    {
       auto ndxX = tbLegend->item(rowi, column)->text();
       gr_data->setLineData( row, ndxX );
       plot->updateLine( row );
       emit dataChanged( gr_data );
    }
    if( column == 2 /*&&  gr_data->graphType == LineChart*/ )
    {
        std::string  name = tbLegend->item(rowi, column)->text().toStdString();
        gr_data->setLineData(static_cast<size_t>(row), name );
        emit dataChanged( gr_data );
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


//=======================================================================================
// Added for new legend table

void DragTableWidget::startDragN(/*Qt::DropActions supportedActions*/)
{
    if( currentColumn() == 2 )
    {
        QTableWidgetItem *item = currentItem();
        QString text_ = item->text();

        QFontMetrics fm(this->font());
        QPixmap pixmap(fm.width(text_)+2, fm.height()+2);
        pixmap.fill(QColor(0, 0, 0, 0));
        QPainter dc(&pixmap);
        dc.drawText(2, fm.height()-2, text_ );
        dc.end();

        QMimeData * mimeData = new QMimeData;
        mimeData->setText( text_ );
        QDrag *drag = new QDrag(this);
        drag->setMimeData( mimeData );
        drag->setPixmap(pixmap/*QPixmap::grabWidget(this)*/);
        drag->setHotSpot( QPoint(0, pixmap.height()) );
        drag->start();
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
        int ndx_plot =  grData->getPlot( static_cast<size_t>(index.row()) );
        if( ndx_plot >= 0 )
        {
            QStringList cbList = grData->modelData(static_cast<size_t>(ndx_plot))->getAbscissaIndexes();
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
