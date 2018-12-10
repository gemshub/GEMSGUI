//-------------------------------------------------------------------
// $Id: GraphDialog.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of GraphDialog class (Plotting system)
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

#include <cstdio>
#include <QPainter>
#include <QPrinter>
#include <QPrintDialog>
#include <QPixmap>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QColorDialog>
#include <QMimeData>
#include <QDrag>
#include <QHeaderView>

#include <qwt_symbol.h>
#include <qwt_picker_machine.h>

#include "GraphDialog.h"
#include "LegendDialog.h"
#include "SymbolDialog.h"
#include "GemsMainWindow.h"
#include "visor.h"
#include "service.h"
#include "model_w.h"

void paintIcon( QIcon &icon, TPlotLine &plLine )
{
        int dsize = 40;

        QPixmap pic(dsize, dsize);
        pic.fill(QColor(0, 0, 0, 0));
        QPainter painter(&pic);

        QRect rect(0,0,dsize, dsize);
        QColor color( plLine.getColor() );
        QPen pen = QPen( color, 2 );
        painter.setPen( pen );
        QPoint center = rect.center();
        int size = 10;

        //--painter.fillRect(rect, Qt::white);
        QwtSymbol* symbol = new QwtSymbol;
        setQwtSymbol( symbol, plLine.getType(), 20, color  );
        symbol->drawSymbol( &painter, QPointF( dsize/2, dsize/2));

        // line, not crossing the point mark
        if(plLine.getLineSize() > 0 )
        {
            painter.drawLine( rect.x(), center.y(), center.x()-size, center.y());
            painter.drawLine( center.x()+size, center.y(), rect.width(), center.y());
        }

        /* column, not crossing the point mark
        if(plLine.getLineSize() < 0 )
        {
            painter.setPen( QPen(color, 9) );
            painter.drawLine( center.x(), center.y(), center.x(), rect.height()-2);
        }
        */

        icon.addPixmap(pic);
}

//==========================================================================================

/// The constructor
GraphDialog::GraphDialog(TCModule *pmodule, GraphData& data):
        QDialog( pmodule->window() ),
        pModule(pmodule),  isFragment(false), gr_data(data)
{
	setupUi(this);
    gstring cap = "GEM-Selektor v.3 Graphics Dialog";
    setWindowTitle( cap.c_str() );

    // define plot window
    plot = new TPlotWidget( &gr_data, this );
    verticalLayout_2->addWidget( plot);

   // Define legend table
    tbLegend = new DragTableWidget( this, this );
    tbLegend->setSizePolicy(  QSizePolicy::Preferred, QSizePolicy::Preferred  );
    tbLegend->setSelectionMode(QAbstractItemView::NoSelection);
    tbLegend->setColumnCount( 3 );
    //tbLegend->horizontalHeader()->setMinimumSectionSize( 10 );
    tbLegend->setColumnWidth(0, 30 );
    tbLegend->setColumnWidth(1, 30 );
    tbLegend->horizontalHeader()->setStretchLastSection( true);
    //tbLegend->setColumnWidth(1, wdF( ftString, 15, eNo ) );
    tbLegend->setMaximumWidth( 80+wdF( ftString, 15, eNo ) );
    tbLegend->verticalHeader()->setVisible(false);
    tbLegend->horizontalHeader()->setVisible(false);


    QVector<int> first;
    QVector<int> maxXndx;
    gr_data.getIndexes( first, maxXndx );
    LabelDelegate *dgLegend = new LabelDelegate( first, maxXndx );
    tbLegend->setItemDelegate(dgLegend);
    verticalLayout->addWidget( tbLegend );

    // Insert labels in legend box
    if( gr_data.graphType != ISOLINES )
      ShowLegend();
    else
      ShowIsolineLegend();

    connect(tbLegend, SIGNAL(cellClicked( int , int  ) ),
            this, SLOT(changeIcon( int, int )));
    connect(tbLegend, SIGNAL(cellChanged( int , int  ) ),
            this, SLOT(changeNdx( int, int )));
    QObject::connect(pHelp, SIGNAL(clicked()), this, SLOT(CmHelp()));
}

GraphDialog::~GraphDialog()
{}

void GraphDialog::languageChange()
{
    retranslateUi(this);
}


void GraphDialog::selectedFragment( const QRectF & rect)
{
    if( rect.height() < plot->getPlot()->axisInterval(QwtPlot::yLeft).width() /100   ||
        rect.width() < plot->getPlot()->axisInterval(QwtPlot::xBottom).width()/100  )
         return;

    gr_data.part[0] = rect.left();
    gr_data.part[1] = rect.right();
    gr_data.part[2] = rect.top();
    gr_data.part[3] = rect.bottom();
    isFragment = false;
    CmFragment();
    SaveGraphData();
}

void GraphDialog::closeEvent(QCloseEvent* ev)
{
    // close module
    ev->accept();
    pVisorImp->closeMdiChild( this );
}

// Insert labels in legend box
void GraphDialog::ShowLegend()
{
    QTableWidgetItem *itemL, *itemN;
    Qt::ItemFlags flags;

    ((LabelDelegate *)tbLegend->itemDelegate())->setIsoline(false);
    tbLegend->setAcceptDrops(true);
    tbLegend->setEditTriggers( QAbstractItemView::DoubleClicked|QAbstractItemView::AnyKeyPressed );

    tbLegend->clear();
    tbLegend->setRowCount(gr_data.lines.GetCount());
    for(uint ii=0; ii<gr_data.lines.GetCount(); ii++ )
    {
        tbLegend->setRowHeight(ii, htF(ftString, 0)+4);

        QIcon icon;
        paintIcon( icon, gr_data.lines[ii] );
        itemL = new QTableWidgetItem(icon, "");
        flags = itemL->flags();
        itemL->setFlags(flags & ~Qt::ItemIsEditable);
        itemL->setToolTip("Legend column 1");
        tbLegend->setItem(ii, 0, itemL );

        itemN = new QTableWidgetItem(tr("%1").arg(gr_data.getIndex( ii )));
        itemN->setToolTip("Legend column 2");
        tbLegend->setItem(ii, 1, itemN );

        itemN = new QTableWidgetItem(tr("%1").arg(gr_data.getName( ii ).c_str()));
        itemN->setToolTip("Legend column 3");
        tbLegend->setItem(ii, 2, itemN );
    }

}

// Insert labels in legend box (Isoline graph)
void GraphDialog::ShowIsolineLegend()
{
    QTableWidgetItem *itemL, *itemN;
    Qt::ItemFlags flags;

    ((LabelDelegate *)tbLegend->itemDelegate())->setIsoline(true);
    tbLegend->setAcceptDrops(false);
    //tbLegend->setEditTriggers( QAbstractItemView::NoEditTriggers );
    tbLegend->setEditTriggers( QAbstractItemView::DoubleClicked|QAbstractItemView::AnyKeyPressed );

    tbLegend->clear();
    tbLegend->setRowCount(gr_data.scale.GetCount());
    for(uint ii=0; ii<gr_data.scale.GetCount(); ii++ )
    {
        tbLegend->setRowHeight(ii, htF(ftString, 0)+4);

        TPlotLine pl( "Scale",  QwtSymbol::Rect, 0, 0, gr_data.scale[ii].red(),
                      gr_data.scale[ii].green(), gr_data.scale[ii].blue() );
        QIcon icon;
        paintIcon( icon, pl );
        itemL = new QTableWidgetItem(icon, "");
        flags = itemL->flags();
        itemL->setFlags(flags & ~Qt::ItemIsEditable);
        itemL->setToolTip("Legend column 1");

        tbLegend->setItem(ii, 0, itemL );
        itemN = new QTableWidgetItem(tr("%1").arg(gr_data.getValueIsoline(ii)));
        itemN->setToolTip("Legend column 3");
        tbLegend->setItem(ii, 2, itemN );
    }
 }

void GraphDialog::changeIcon( int row, int column )
{
    if( column == 0 )
    {
        if( gr_data.graphType != ISOLINES )
        {
            SymbolDialog cd( gr_data.lines[row], this);
            if( cd.exec() )
            {
                gr_data.lines[row] =  cd.GetPlotLine();
                QIcon icon;// = tbLegend->item(row, column)->icon();
                paintIcon( icon, gr_data.lines[row] );
                tbLegend->item(row, column)->setIcon(icon);
                plot->replotPlotLine(row);
                SaveGraphData();
            }
        }else
        {
            // select isoline color
            QColor color(gr_data.scale[row]);
            QColor backColor = QColorDialog::getColor( color, this );
            if( backColor.isValid() )
            {
                gr_data.scale[row] =backColor;
                TPlotLine pl( "Scale",  QwtSymbol::Rect, 0, 0, gr_data.scale[row].red(),
                              gr_data.scale[row].green(), gr_data.scale[row].blue() );
                QIcon icon;
                paintIcon( icon, pl );
                tbLegend->item(row, column)->setIcon(icon);
                Apply();
            }
        }
    } else if( column ==  2 )
    {
       highlightRow( row, column );
    }
}


void GraphDialog::changeNdx( int row, int column )
{
    if( column == 1 &&  gr_data.graphType != ISOLINES )
    {
       int ndxX = tbLegend->item(row, column)->text().toInt();
       gr_data.setIndex( row, ndxX );
       plot->replotPlotLine(row);
       SaveGraphData();
   }
    if( column == 2 )
    {
      if( gr_data.graphType != ISOLINES )
      {
        gstring  name = tbLegend->item(row, column)->text().toLatin1().data();
        gr_data.setName( row, name.c_str() );
        SaveGraphData();
      }
      else
      {
         double  val = tbLegend->item(row, column)->text().toDouble();
         gr_data.setValueIsoline( val, row );
         SaveGraphData();
      }
   }

}

void GraphDialog::highlightRow( int row, int column )
{
  if( gr_data.graphType == ISOLINES || column != 2)
      return;

  if( activeRow == row ) // toggle
  {
    restoreRow();
    return;
  }
  restoreRow();
  const TPlotLine& ldt = gr_data.lines[row];
  gr_data.lines[row].setChanges( ldt.getType(), ldt.getSize()*2, ldt.getLineSize()*2, ldt.getColor() );
  activeRow = row;
  plot->replotPlotLine(activeRow);
}

void GraphDialog::restoreRow()
{
 if( activeRow >= 0 )
 {
    auto row = activeRow;
    const TPlotLine& ldt = gr_data.lines[row];
    gr_data.lines[row].setChanges( ldt.getType(), ldt.getSize()/2, ldt.getLineSize()/2, ldt.getColor() );
    activeRow = -1;
    plot->replotPlotLine(row);
 }
}


// If this is for adding points - we don't need Clear() and Show() - just add
void GraphDialog::ShowNew( const char* capAdd )
{
    gstring cap = "GEM-Selektor v.3 Graphics Dialog";
    //gstring cap = windowTitle().toLatin1().data();
    if(capAdd)
       cap += capAdd;
    setWindowTitle( cap.c_str() );

   plot->replotPlot( isFragment);
}


void GraphDialog::AddPoint( int nPlot, int nPoint )
{
  if( gr_data.graphType != LINES_POINTS )
        return;

  if( nPlot>=0 && nPoint>=0 )
     plot->addPoint(nPlot,  nPoint);
}


void  GraphDialog::SaveGraphData()
{
    if( !pModule->SaveGraphData( &gr_data ) )
      cout << "Graph Data cannot be saved" << endl; // message "Changes cannot be saved"
}

void GraphDialog::Apply()
{
    disconnect(tbLegend, SIGNAL(cellClicked( int , int  ) ),
            this, SLOT(changeIcon( int, int )));
    disconnect(tbLegend, SIGNAL(cellChanged( int , int  ) ),
            this, SLOT(changeNdx( int, int )));

    // Insert labels in legend box
    if( gr_data.graphType != ISOLINES )
      ShowLegend();
    else
      ShowIsolineLegend();

    plot->replotPlot( isFragment);
    SaveGraphData();

    connect(tbLegend, SIGNAL(cellClicked( int , int  ) ),
            this, SLOT(changeIcon( int, int )));
    connect(tbLegend, SIGNAL(cellChanged( int , int  ) ),
            this, SLOT(changeNdx( int, int )));

}

void GraphDialog::setBackgrColor( QColor color )
{
    gr_data.b_color[0] =   color.red();
    gr_data.b_color[1] =   color.green();
    gr_data.b_color[2]  =  color.blue();
 //   gr_data.isBackgr_color = true;
}

QColor GraphDialog::getBackgrColor()
{
   return  QColor( gr_data.b_color[0], gr_data.b_color[1], gr_data.b_color[2] );
   //plot->palette().color(QPalette::Window);
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
        pFragmentBtn->setText("&Full");
    }
    else
    {
        isFragment = false;
        pFragmentBtn->setText("&Fragment");
    }
    plot->setFragment( isFragment );
}

void GraphDialog::CmSave()
{
    /*QString filter;
    QList<QByteArray> formatList = QImageWriter::supportedImageFormats ();

   if( formatList.contains("png") )
        filter.append("png Files (*.png)");

   QListIterator<QByteArray> it( formatList );
    while( it.hasNext() )
    {
      QString str = it.next();
      if( str == "png" )
         continue;
      if( !filter.isEmpty() )
        filter.append(";;");
      filter.append(str);
      filter.append(" Files (*.");
      filter.append(str.toLower());
      filter.append(")");
   }

    QString selectedFilter;
    gstring path = pVisor->localDir();

    QString fn = QFileDialog::getSaveFileName(this, "Saving Plotting Image",
                  path.c_str(), filter, &selectedFilter );
    if ( !fn.isEmpty() )
    {
      QString format( selectedFilter.left(3) );
      if( !fn.contains('.') )
      {
        fn.append(".");
        fn.append(format.toLower());
      }
      QPixmap::grabWidget(plot).save(fn, format.toLatin1().data());


        path = fn.toLatin1().data();
        gstring dir;
        gstring name;
        gstring newname;
        u_splitpath( path, dir, name, newname );
        dir  += "/";
        pVisor->setLocalDir( dir );
    }*/
   plot->savePlot();

}

// Print graph screen to printer device
void GraphDialog::CmPrint()
{
    QPrinter printer( QPrinter::PrinterResolution/*QPrinter::HighResolution*/ );


    QPrintDialog dialog( &printer );
    if ( dialog.exec()  )
    {
        QPainter painter;
        painter.begin(&printer);
        double xscale = printer.pageRect().width()/double(framePrn->width());
        double yscale = printer.pageRect().height()/double(framePrn->height());
        double scale = qMin(xscale, yscale);
        painter.translate(printer.paperRect().x() + printer.pageRect().width()/2,
                          printer.paperRect().y() + printer.pageRect().height()/2);
        painter.scale(scale, scale);
        painter.translate(-framePrn->width()/2, -framePrn->height()/2);

        groupBox->hide();
        framePrn->render( &painter );
        groupBox->show();
       //plot->printPlot();
    }
}

void GraphDialog::CmHelp()
{
    pVisorImp->OpenHelp( GEMS_GRAPH_HTML );
}

//=======================================================================================
// Added for new legend table

void DragTableWidget::startDrag()
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

void DragTableWidget::focusOutEvent(QFocusEvent* e )
{
    topDlg->restoreRow();
    QTableWidget::focusOutEvent(e);
 }

void DragTableWidget::mousePressEvent( QMouseEvent *e )
{
        if(e->buttons() & Qt::LeftButton )
        {
            startPos = e->pos();
        }
        QTableWidget::mousePressEvent(e);
 }

void DragTableWidget::mouseMoveEvent( QMouseEvent *e )
{
        if(e->buttons() & Qt::LeftButton )
        { int delta = (e->pos()-startPos).manhattanLength();
            if( delta >= QApplication::startDragDistance() )
              startDrag();
        }
        QTableWidget::mouseMoveEvent(e);
}


LabelDelegate::LabelDelegate( QVector<int> afirst,
                              QVector<int> amaxXndx, QObject * parent ):
    QItemDelegate( parent ), first(afirst), maxXndx(amaxXndx)
{
}

// Editing QTableView for objects in TCPage
QWidget *LabelDelegate::createEditor(QWidget *parent,
        const QStyleOptionViewItem &/*option*/,
        const QModelIndex &index) const
{

    if( index.column() == 2 )
    {
       QLineEdit *editor =  new QLineEdit( parent);
       if( isIsoline )
           editor->setValidator( new QDoubleValidator() );
       editor->setMaxLength( 15 );
       return editor;
       //return QItemDelegate::createEditor( parent, option,  index );
    }
    if( !isIsoline && index.column() == 1 )
    {
      int ii, row= index.row();
      int nAbs = maxXndx[0];
      for(  ii=1; ii<first.count(); ii++ )
          if( row < first[ii])
            break;
      nAbs = maxXndx[ii-1];
      //if(nAbs <= 1)
      //  return 0;

      QComboBox* editor =  new QComboBox( parent);
      editor->addItem(tr("%1").arg(-1));

      for( int ii=0; ii<nAbs; ii++ )
          editor->addItem(tr("%1").arg(ii));
     // editor->addItem(tr("%1").arg(-1));
      return editor;
    }
    return 0;
}

void LabelDelegate::setModelData(QWidget *editor,
                                 QAbstractItemModel *model,
                                 const QModelIndex &index) const
{
   if( index.column() == 1)
   {
       QComboBox *cellEdit = dynamic_cast<QComboBox*>(editor);
        // if( cellEdit->dataCh() )
             model->setData(index, cellEdit->currentText(),  Qt::EditRole);
   }
   else
       QItemDelegate::setModelData( editor, model, index);
 }

//--------------------- End of GraphDialog.cpp ---------------------------
