//-------------------------------------------------------------------
// $Id: qwtplot.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of TPlotWidget class using Qwt class library
//   see http://qwt.sf.net (plotting system implementation)
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

#include <QVBoxLayout>
#include <QPrinter>
#include <QPrintDialog>
#include <QFileDialog>
#include <QImageWriter>
#include <QMimeData>
#include <QDrag>

#include <qwt_plot_canvas.h>
#include <qwt_symbol.h>
#include <qwt_plot_renderer.h>
//#include <qwt_plot_layout.h>
#include <qwt_scale_widget.h>
#include <qwt_color_map.h>
#include <qwt_picker_machine.h>

#include "qwtplot.h"
#include "visor.h"
#include "GemsMainWindow.h"

void setQwtSymbol( QwtSymbol* symbol, int type, int size, QColor color  )
{
    // define symbol
    if( type > 14 )
    {
      type -= 14;
      // symbol->setBrush( color ); // nocolor
    }
    else
      symbol->setBrush( color );

    symbol->setStyle( (QwtSymbol::Style)type );
    symbol->setPen(  QPen( color, 2 ) );
    symbol->setSize(  size );
}


TPlotWidget::TPlotWidget( GraphData* aGr_data, QWidget *parent):
    QWidget(parent), gr_data(aGr_data)
{
    setAcceptDrops(true);
    m_grid = 0;
    m_plot = new QwtPlot();

    QVBoxLayout * const layout = new QVBoxLayout;
    layout->addWidget(m_plot);
    setLayout(layout);

    minX = gr_data->region[0];
    maxX = gr_data->region[1];
    minY = gr_data->region[2];
    maxY = gr_data->region[3];

    m_grid = new QwtPlotGrid;
    /// m_grid->enableXMin(true);
    m_grid->setMajorPen(QPen(Qt::black,0,Qt::DotLine));
    m_grid->setMinorPen(QPen(Qt::gray,0,Qt::DotLine));
    m_grid->attach(m_plot);

    QwtText text;
    //text.setFont(font);
    text.setText(QString("(c) GEMS"));
    c_marker = new QwtPlotMarker();
    c_marker->setValue( maxX, minY );
    c_marker->setLabel( text );
    c_marker->setLabelAlignment( Qt::AlignLeft | Qt::AlignTop );
    c_marker->attach( m_plot );

    showPlot();
    showPicker();
}


void TPlotWidget::showPlot()
{
   showGrid();

   switch( gr_data->graphType )
   {
     case LINES_POINTS: showPlotLines(); break;
     case CUMULATIVE: showCumulativeLines(); break;
     case ISOLINES: showIsoLines(); break;
     case LINES_3D: show3DLines(); break;
   }
   m_plot->replot();
}

void TPlotWidget::replotPlot( bool  isFragment )
{
    //clear lines
    for(int ii=0; ii<m_curves.count(); ii++)
    {
        if( m_curves[ii] )
         {   m_curves[ii]->detach();
             delete m_curves[ii];\
         }
    }
    m_curves.clear();

    // clear interval curves
    for(int ii=0; ii<m_intervalcurves.count(); ii++)
    {
        if( m_intervalcurves[ii] )
         {
            m_intervalcurves[ii]->detach();
            delete m_intervalcurves[ii];
         }
    }
    m_intervalcurves.clear();

    if(  isFragment == true )
    {
        minX = gr_data->part[0];
        maxX = gr_data->part[1];
        minY = gr_data->part[2];
        maxY = gr_data->part[3];
    }
    else
    {
        minX = gr_data->region[0];
        maxX = gr_data->region[1];
        minY = gr_data->region[2];
        maxY = gr_data->region[3];
    }

    showPlot();
}

void TPlotWidget::showGrid()
{
    axisFont = pVisorImp->getAxisLabelFont();
    //font.setBold(true);
    QwtText text;
    text.setFont(axisFont);

    text.setText(QString(gr_data->title.c_str()));
    m_plot->setTitle( text );

    text.setText(QString(gr_data->xName.c_str()));
    m_plot->setAxisTitle( QwtPlot::xBottom, text );
    m_plot->setAxisScale( QwtPlot::xBottom, minX, maxX );
    m_plot->setAxisMaxMajor( QwtPlot::xBottom, gr_data->axisTypeX );
    m_plot->setAxisMaxMinor( QwtPlot::xBottom, 4 );
    m_plot->setAxisFont( QwtPlot::xBottom,axisFont );

    text.setText(QString(gr_data->yName.c_str()));
    m_plot->setAxisTitle( QwtPlot::yLeft, text );
    m_plot->setAxisScale( QwtPlot::yLeft, minY, maxY );
    m_plot->setAxisMaxMajor( QwtPlot::yLeft, gr_data->axisTypeY );
    m_plot->setAxisMaxMinor( QwtPlot::yLeft, 4 );
    m_plot->setAxisFont( QwtPlot::yLeft,axisFont );

    if( c_marker )
    c_marker->setValue( maxX, minY );

    //if( gr_data->isBackgr_color )
    { QPalette canvasPalette( Qt::white );
      canvasPalette.setColor( QPalette::Background,
            QColor( gr_data->b_color[0],
                    gr_data->b_color[1],
                    gr_data->b_color[2])  );
      m_plot->canvas()->setPalette( canvasPalette );
    }
}


void TPlotWidget::showPlotLines()
{
  uint ii;
  int jj, nLine;
  for( ii=0, nLine =0; ii < gr_data->plots.GetCount(); ii++)
   for( jj=0; jj < gr_data->plots[ii].getLinesNumber(); jj++, nLine++ )
   {
       showPlotLine( nLine, ii, jj );
   }
}

void TPlotWidget::showPlotLine( int nLine, int nPlot, int nLineinPlot )
{

    if( gr_data->getIndex(nLine) < 0 )
    {
        m_curves.insert(nLine,0);
        return;
    }

    //define curve
    QwtPlotCurve *m_curve = new QwtPlotCurve(QString( gr_data->getName(nLine).c_str() ));
    m_curve->setRenderHint(QwtPlotItem::RenderAntialiased); //sglazhivanie
    QPen pen = QPen( gr_data->getColor(nLine) );
    pen.setWidth( gr_data->getLineSize(nLine) );
    m_curve->setPen( pen );
    if( gr_data->getLineSize(nLine) == 0 )
       m_curve->setStyle( QwtPlotCurve::NoCurve );

    // define symbol
    QwtSymbol* symbol = new QwtSymbol;
    setQwtSymbol( symbol, gr_data->getType(nLine),
                  gr_data->getSize(nLine), gr_data->getColor(nLine)  );
    m_curve->setSymbol( symbol );

    // get array of not empty points
    QVector<QPointF> points;
    gr_data->plots[nPlot].getPointLine( nLineinPlot, points, gr_data->getIndex(nLine) );
    m_curve->setSamples( points );

    m_curve->attach(m_plot);
    m_curves.insert(nLine,m_curve);
    //m_curves.append(m_curve);
}

void TPlotWidget::replotPlotLine( int nLine )
{
    if( gr_data->graphType == LINES_POINTS )
    { //define curve

      if( m_curves[nLine] )
      {   m_curves[nLine]->detach();
          delete m_curves[nLine];
      }
      m_curves.removeAt(nLine);

      int nPlot = gr_data->getPlot( nLine );
      int nLineinPlot = nLine -gr_data->plots[nPlot].getFirstLine();
      showPlotLine( nLine, nPlot, nLineinPlot );
      m_plot->replot();
    }
    else
    {
        // clear interval curves
        for(int ii=0; ii<m_intervalcurves.count(); ii++)
        {
            if( m_intervalcurves[ii] )
            {    m_intervalcurves[ii]->detach();
                delete m_intervalcurves[ii];
             }
        }
        m_intervalcurves.clear();
        showPlot();
    }

}

void TPlotWidget::addPoint( int nPlot, int nPoint )
{
    QwtPlotMarker *d_marker1;
    QwtSymbol* symbol;
    int nLine = gr_data->plots[nPlot].getFirstLine();

    for( int jj=0; jj<gr_data->plots[nPlot].getLinesNumber(); jj++, nLine++ )
    {
        if(gr_data->getIndex(nLine) < 0 )
          continue;

        QPointF pos = gr_data->plots[nPlot].getPoint( jj, nPoint, gr_data->getIndex(nLine) );
        if( pos.x() == DOUBLE_EMPTY || pos.y() == DOUBLE_EMPTY )
              continue;
        symbol = new QwtSymbol;
        setQwtSymbol( symbol, gr_data->getType(nLine),
                      gr_data->getSize(nLine), gr_data->getColor(nLine)  );
        d_marker1 = new QwtPlotMarker();
        d_marker1->setValue( pos.x(), pos.y() );
        d_marker1->setSymbol( symbol );
        d_marker1->attach( m_plot );
        m_points.append(d_marker1);
        /*
        QwtPlotCurve *m_curve = new QwtPlotCurve();
        m_curve->setStyle( QwtPlotCurve::NoCurve );
        QwtSymbol* symbol = new QwtSymbol;
        setQwtSymbol( symbol, gr_data->getType(nLine),
                      gr_data->getSize(nLine), gr_data->getColor(nLine)  );
        m_curve->setSymbol( symbol );
        QVector<QPointF> points;
        points << pos;
        m_curve->setSamples( points );
        m_curve->attach(m_plot);
        */
    }
    m_plot->replot();
}

void TPlotWidget::setFragment( bool showFragment )
{
    if( showFragment )
    {
        minX = gr_data->part[0];
        maxX = gr_data->part[1];
        minY = gr_data->part[2];
        maxY = gr_data->part[3];
    }
    else
    {
        minX = gr_data->region[0];
        maxX = gr_data->region[1];
        minY = gr_data->region[2];
        maxY = gr_data->region[3];
    }

    m_plot->setAxisScale( QwtPlot::xBottom, minX, maxX );
    m_plot->setAxisScale( QwtPlot::yLeft, minY, maxY );
    c_marker->setValue( maxX, minY );
    m_plot->replot();
}

/*
void TPlotWidget::printPlot()
{
    QPrinter printer( QPrinter::HighResolution );
    printer.setOrientation( QPrinter::Landscape );
    //printer.setOutputFileName( "spectrogram.pdf" );
    QPrintDialog dialog( &printer );
    if ( dialog.exec() )
    {
        QwtPlotRenderer renderer;
        renderer.setDiscardFlag( QwtPlotRenderer::DiscardBackground, false );
        renderer.setLayoutFlag( QwtPlotRenderer::FrameWithScales, true );
        renderer.renderTo( m_plot, printer );
  }
}
*/

void TPlotWidget::savePlot()
{
    QString fileName =   pVisor->localDir().c_str();
            fileName +=  "image.pdf";

#ifndef QT_NO_FILEDIALOG
    const QList<QByteArray> imageFormats =
        QImageWriter::supportedImageFormats();

    QStringList filter;
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
            imageFilter = "Images (*.";
            imageFilter += imageFormats[i];
            imageFilter += ")";
            filter += imageFilter;
        }
    }

    QString selectedFilter;
    fileName = QFileDialog::getSaveFileName(
        this, "Saving Graphics Image", fileName,
        filter.join( ";;" ), &selectedFilter, QFileDialog::DontConfirmOverwrite );
#endif
    if ( !fileName.isEmpty() )
    {
        QwtPlotRenderer renderer;
        renderer.renderDocument( m_plot, fileName, QSizeF( 200, 200 ), 85 );

        gstring path = fileName.toLatin1().data();
        gstring dir;
        gstring name;
        gstring newname;
        u_splitpath( path, dir, name, newname );
        dir  += "/";
        pVisor->setLocalDir( dir );

    }

}


QPointF TPlotWidget::transform( QPoint pos )
{
    QPointF value;
    int dx = m_plot->axisWidget(QwtPlot::yLeft)->size().width();
    int dy = m_plot->axisWidget(QwtPlot::xTop)->size().height();
    dx += (int)m_plot->transform( QwtPlot::xBottom, minX ) ;
    dy += (int)m_plot->transform( QwtPlot::yLeft, maxY ) ;
    value.setX( m_plot->invTransform( QwtPlot::xBottom, pos.x()-dx ) );
    value.setY( m_plot->invTransform( QwtPlot::yLeft, pos.y()-dy ) );
    return value;
}

void TPlotWidget::showLegendText( QPointF pos, QString text_ )
{
    QwtText text;
    text.setFont(axisFont);
    text.setText(text_);
    QwtPlotMarker *d_marker1 = new QwtPlotMarker();
    d_marker1->setValue( pos );
    d_marker1->setLabel( text );
    d_marker1->setLabelAlignment( Qt::AlignRight | Qt::AlignTop );
    d_marker1->attach( m_plot );
    m_labels.append(d_marker1);
}

void TPlotWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("text/plain")) {
        if (event->source() == this) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        } else {
            event->acceptProposedAction();
        }
    } else {
        event->ignore();
    }
}

void TPlotWidget::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat("text/plain")) {
        if (event->source() == this) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        } else {
            event->acceptProposedAction();
        }
    } else {
        event->ignore();
    }
}

void TPlotWidget::dropEvent(QDropEvent* event)
{
    if (event->mimeData()->hasFormat("text/plain"))
    {
     QString text_ = event->mimeData()->text();
     int ii;
     for(ii=m_labels.count()-1; ii>=0; ii--)
     {
        if( m_labels[ii]->label().text() == text_ )
        {
           m_labels[ii]->setValue(transform( event->pos()));
           break;
        }
     }
     if( ii < 0 )
     {
         showLegendText( transform( event->pos()),  text_ );
     }
     m_plot->replot();
  }
}

void TPlotWidget::mousePressEvent( QMouseEvent *e )
{
    QPointF pos = transform( e->pos());

    for(int ii=m_labels.count()-1; ii>=0; ii--)
    {
        QPointF posl = m_labels[ii]->value();
        QSizeF dxdy = m_labels[ii]->label().textSize( axisFont	);
        //cout << " x = " << pos.x() << " y = " << pos.y() << endl;
        //cout << " top = " << posl.x() << " left = " << posl.y() << endl;
        //cout << " width = " << dxdy.width() << " height = " << dxdy.height() << endl;

        if( pos.x() >= posl.x() && pos.x()< posl.x()+dxdy.width() &&
            pos.y() >= posl.y() && pos.y()< posl.y()+dxdy.height() )
        {
         QFontMetrics fm = QPainter(this).fontMetrics();
         QPixmap pixmap(fm.width(m_labels[ii]->label().text())+2, fm.height()+2);
         pixmap.fill(QColor(0, 0, 0, 0));
         QPainter dc(&pixmap);
         dc.drawText(2, fm.height()-2, m_labels[ii]->label().text());

          QMimeData * mimeData = new QMimeData;
          mimeData->setText( m_labels[ii]->label().text() );
          QDrag *drag = new QDrag(this);
          drag->setMimeData( mimeData );
          drag->setPixmap(pixmap);
          drag->setHotSpot( QPoint(0, pixmap.height()) );
          drag->start();
          break;
        }
   }
   QWidget::mousePressEvent( e );
}


void TPlotWidget::showCumulativeLines()
{
  uint ii = 0;
  int jj, nLine = 0;
  QVector<QwtIntervalSample> points;

  for( jj = 0; jj< gr_data->plots[ii].getdX(); jj++)
  {
    QPointF pos = gr_data->plots[ii].getPoint( 0, jj, gr_data->getIndex(0) );
    points.append( QwtIntervalSample( pos.x(),
                   QwtInterval( gr_data->region[2], gr_data->region[2] ) ));
  }

  //for( ii=0, nLine =0; ii < gr_data->plots.GetCount(); ii++)
   for( jj=0; jj < gr_data->plots[ii].getLinesNumber(); jj++, nLine++ )
   {

     if(gr_data->getIndex(nLine) < 0 )
     {
         m_intervalcurves.insert(nLine,0);
         continue;
     }
    //define curve
    QwtPlotIntervalCurve *m_curve = new QwtPlotIntervalCurve(QString( gr_data->getName(nLine).c_str() ));
    m_curve->setRenderHint(QwtPlotItem::RenderAntialiased); //sglazhivanie
    m_curve->setStyle( QwtPlotIntervalCurve::Tube );
    QPen pen = QPen( gr_data->getColor(nLine) );
    pen.setWidth( gr_data->getLineSize(nLine) );
    m_curve->setPen( pen );
    QBrush brush = QBrush( gr_data->getColor(nLine) );
    m_curve->setBrush( brush );

    // get array of not empty points
    gr_data->plots[ii].getPointTube( nLine, points,  gr_data->getIndex(nLine) );
    m_curve->setSamples( points );

    m_curve->attach(m_plot);
    m_intervalcurves.insert(nLine,m_curve);
    //m_curves.append(m_curve);
   }
}


void TPlotWidget::showPicker()
{
   m_picker = new QwtPlotPicker( QwtPlot::xBottom, QwtPlot::yLeft,
       QwtPlotPicker::RectRubberBand, QwtPicker::AlwaysOn,
       m_plot->canvas() );
   QwtPickerDragRectMachine *picker_mathine = new QwtPickerDragRectMachine();
   picker_mathine->setState(10);
   m_picker->setStateMachine( picker_mathine);
   m_picker->setRubberBandPen( QColor( Qt::red ) );
   m_picker->setRubberBand( QwtPlotPicker::RectRubberBand );
   m_picker->setTrackerMode(QwtPicker::ActiveOnly);
   m_picker->setTrackerPen( QColor( Qt::black ) );
   connect( m_picker, SIGNAL( selected( const QRectF & ) ), parent(),
         SLOT( selectedFragment( const QRectF & ) ) );

}


//under working---------------------------------------------------------------------

class SpectrogramData: public QwtRasterData
{
   QVector< QwtPoint3D >  	points; //order by x list

public:
    SpectrogramData(GraphData* aGr_data);

    virtual double value( double x, double y ) const;

};


SpectrogramData::SpectrogramData(GraphData* aGr_data)
{
    int ii, jj;
    double x, y, z;
    double minX, minY, minZ;
    double maxX, maxY, maxZ;

    minX = maxX  =  aObj[aGr_data->plots[0].getObjY()].GetEmpty(0,0);
    minY = maxY  =  aObj[aGr_data->plots[0].getObjY()].GetEmpty(0,1);
    minZ = maxZ  =  aObj[aGr_data->plots[0].getObjY()].GetEmpty(0,2);

    // build points array
    for( ii = 0; ii< aGr_data->plots[0].getdX(); ii++)
    {
        x =  aObj[aGr_data->plots[0].getObjY()].GetEmpty(ii,0);
        y =  aObj[aGr_data->plots[0].getObjY()].GetEmpty(ii,1);
        z =  aObj[aGr_data->plots[0].getObjY()].GetEmpty(ii,2);
        if( x == DOUBLE_EMPTY || y == DOUBLE_EMPTY || z== DOUBLE_EMPTY )
            continue;

        if( minX > x ) minX = x;
        if( maxX < x ) maxX = x;
        if( minY > y ) minY = y;
        if( maxY < y ) maxY = y;
        if( minZ > z ) minZ = z;
        if( maxZ < z ) maxZ = z;

        for( jj = 0; jj<points.count(); jj++ )
        {
            if( x < points[jj].x() )
            { // new x
                break;
            }
            if( x == points[jj].x() )
            {
                if( y <= points[jj].y() )
                    break;
            }
        }
        if( x != points[jj].x() && y != points[jj].y())
            points.insert(jj, QwtPoint3D(x,y,z) );

    }

    setInterval( Qt::XAxis, QwtInterval( minX, maxX ) );
    setInterval( Qt::YAxis, QwtInterval( minY, maxY ) );
    setInterval( Qt::ZAxis, QwtInterval( minZ, maxZ ) );
}

double SpectrogramData::value( double x, double y ) const
{
    int jj, ndxMin = 0;
    double dX, dXmin = pow( points[0].x() - x, 2. );
    double dY, dYmin = pow( points[0].y() - y, 2. );

    for( jj = 0; jj<points.count(); jj++ )
    {
        dX = pow( points[jj].x() - x, 2. );
        dY = pow( points[jj].y() - y, 2. );
        if( dX+dY < dXmin+dYmin )
        {
           dXmin = dX;
           dYmin = dY;
           ndxMin = jj;
        }
        if( dX > dXmin+dYmin  )
          break;
    }

    return points[ndxMin].z();
}

class ColorMap: public QwtLinearColorMap
{
public:
    ColorMap():
        QwtLinearColorMap( Qt::darkCyan, Qt::red )
    {
        addColorStop( 0.1, Qt::cyan );
        addColorStop( 0.6, Qt::green );
        addColorStop( 0.95, Qt::yellow );
    }
};

void TPlotWidget::showIsoLines()
{
    d_spectrogram = new QwtPlotSpectrogram();
    d_spectrogram->setRenderThreadCount( 0 ); // use system specific thread count

    d_spectrogram->setColorMap( new ColorMap() );

    d_spectrogram->setData( new SpectrogramData( gr_data ) );
    d_spectrogram->attach( m_plot );

    QList<double> contourLevels;
    for ( double level = 0.5; level < 10.0; level += 1.0 )
        contourLevels += level;
    d_spectrogram->setContourLevels( contourLevels );

    const QwtInterval zInterval = d_spectrogram->data()->interval( Qt::ZAxis );
    // A color bar on the right axis
    QwtScaleWidget *rightAxis = m_plot->axisWidget( QwtPlot::yRight );
    rightAxis->setTitle( "Intensity" );
    rightAxis->setColorBarEnabled( true );
    rightAxis->setColorMap( zInterval, new ColorMap() );

    m_plot->setAxisScale( QwtPlot::yRight, zInterval.minValue(), zInterval.maxValue() );
    m_plot->enableAxis( QwtPlot::yRight );

    //plotLayout()->setAlignCanvasToScales( true );
    m_plot->replot();
}

/*void Plot::showContour( bool on )
{
    d_spectrogram->setDisplayMode( QwtPlotSpectrogram::ContourMode, on );
    replot();
}

void Plot::showSpectrogram( bool on )
{
    d_spectrogram->setDisplayMode( QwtPlotSpectrogram::ImageMode, on );
    d_spectrogram->setDefaultContourPen( on ? QPen() : QPen( Qt::NoPen ) );
    replot();
}
*/

//=====================================================================================
