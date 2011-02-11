//-------------------------------------------------------------------
// $Id: GraphDialog.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of GraphDialog class (Plotting system)
//
// Copyright (C) 1996-2008  A.Rysin, S.Dmytriyeva
// Uses  gstring class (C) A.Rysin 1999
//
// This file is part of the GEM-Vizor library which uses the
// Qt v.2.x GUI Toolkit (Troll Tech AS, http://www.trolltech.com)
// according to the Qt Duo Commercial license
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#include <stdio.h>

#include <qpainter.h>
#include <qprinter.h>
#include <qfiledialog.h>
#include <QPixmap>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QImageWriter>
#include <QSpacerItem>

#include "GraphDialog.h"
#include "LegendDialog.h"
#include "visor_w.h"
#include "v_object.h"
#include "visor.h"
#include "service.h"

/*! Empric function to select new color for every new plotting line
*/
inline
QColor plotColor(int n)
{
    return QColor( (n*30)%256, (128+(n*33))%256, (256-(n*37))%256);
}


/*! Labels in the legend frame, that allows to drag the name to the plotting area
*/

class DragLabel: public QLabel
{
    public:
	DragLabel(QWidget* parent):
	    QLabel(parent) 
	{ 
		setAcceptDrops(true);	    
	}
	
	virtual ~DragLabel() {}

    protected:

    void startDrag()
    {
        QFontMetrics fm(this->font());
        //QFontMetrics fm = QPainter(nativeParentWidget () /*this*/).fontMetrics();
        QPixmap pixmap(fm.width(text())+2, fm.height()+2);
        pixmap.fill(QColor(0, 0, 0, 0));
        QPainter dc(&pixmap);
        dc.drawText(2, fm.height()-2, text()); 
    	

      QMimeData * mimeData = new QMimeData;
      mimeData->setText( text() );
      QDrag *drag = new QDrag(this);
      drag->setMimeData( mimeData );
      drag->setPixmap(pixmap/*QPixmap::grabWidget(this)*/);
      drag->setHotSpot( QPoint(0, pixmap.height()) ); 
      drag->start();
     }

    void mousePressEvent( QMouseEvent *e ) 
    {
	    if(e->buttons() == Qt::LeftButton )
            startDrag();
	    QLabel::mousePressEvent(e);
	}
};

/*! Symbols in the legend frame, that allows to drag the name to the plotting area
*/

class SymbolLabel: public QLabel
{
	int type;
	int lineSize;
	QColor color;

 protected:

 	void paintEvent( QPaintEvent * event ); 

 public:
	SymbolLabel(QWidget* parent, const TPlotLine& line):
	    QLabel(parent) 
	{
		setData(line);
	}
	virtual ~SymbolLabel() {}

	void setData(const TPlotLine& ln) 
	{
	    color = QColor(ln.red, ln.green, ln.blue );
            type = ln.type;
	   lineSize = ln.line_size;
	   update();
	}
};

void SymbolLabel::paintEvent( QPaintEvent * event )
{
	QPainter dc(this);
	const int size = 4;
    QRect rect(3, 3, width()-6, height()-6);
    QPoint center = rect.center();
    PlotTypeBtn::drawSymbol(&dc, center, type, size, color);

    if( lineSize > 0 )
    {
	dc.drawLine( rect.x(), center.y(), center.x()-size, center.y());
	dc.drawLine( center.x()+size, center.y(), rect.width(), center.y());
    }
        // column, not crossing the point mark
    if( lineSize < 0 )
    {
        dc.setPen( QPen(color, 9) );
        dc.drawLine( center.x(), center.y(), center.x(), height()-2);
     }
}

/*!
   The constructor
*/
GraphDialog::GraphDialog(TCModule *pmodule, GraphData& data):
        QDialog(pmodule->window() ),
        pModule(pmodule),  isFragment(false), gr_data(data)
{
	
	setupUi(this);
    //oldGraphType = gr_data.graphType;
    minX = gr_data.region[0];
    maxX = gr_data.region[1];
    minY = gr_data.region[2];
    maxY = gr_data.region[3];

    if( maxX == minX ) maxX += 1.;
    if( maxY == minY ) maxY += 1.;

    plot = new TPlotWin( plotFrame,  FPoint(minX, minY),
                    FPoint(maxX, maxY), gr_data.title.c_str());

    plot->setGridCount(gr_data.axisType);
    plot->setAxisTitles(gr_data.xName.c_str(), gr_data.yName.c_str());

    QObject::connect(pHelp, SIGNAL(clicked()), this, SLOT(CmHelp()));

    // Insert labels in legend box
    if( gr_data.graphType != ISOLINES )
      ShowLegend();
    else
      ShowIsolineLegend();

    Show();

//    setCaption( gr_data.title.c_str() );
}

GraphDialog::~GraphDialog()
{}

void GraphDialog::languageChange()
{
    retranslateUi(this);
}

// Insert labels in legend box
void GraphDialog::ShowLegend()
{
	 SymbolLabel* pLabelS;
	 DragLabel* pLabelD;
	 QFrame *line;
	 uint ll = 0, kk = 0;
	 
	 pGrp = new QGridLayout( pGrpLegend );
	 pGrp->setSpacing(6);
	 pGrp->setMargin(11);
  
	// Insert labels in legend box
    for( uint ii=0; ii<gr_data.plots.GetCount(); ii++ )
    {
        for( int jj=0; jj<gr_data.plots[ii].getLinesNumber(); ll++, kk++, jj++ )
        {
            pLabelS = new SymbolLabel( pGrpLegend, gr_data.lines[kk]);
//	        aSymbolLabels.Add( pLabelS);
	        pGrp->addWidget(pLabelS, ll, 0);

            pLabelD = new DragLabel( pGrpLegend );
//	        aLegendLabels.Add( pLabelD );
            pLabelD->setText( QString( gr_data.lines[kk].getName().c_str()) );
            pGrp->addWidget(pLabelD, ll, 1);
        }

       line = new QFrame(pGrpLegend);
       line->setFrameShape(QFrame::HLine);
       line->setFrameShadow(QFrame::Sunken);
       pGrp->addWidget(line, ll++, 0, 1, 2);
    }
    pGrp->setRowStretch ( ll, 1 ) ;
}

gstring GraphDialog::getTextIsoline(int ii)
{
    vstr buf(200);
    int nObjY = gr_data.plots[1].getObjY();
    memset( buf, 0, 150 );
    sprintf(buf, " %.4g : %.4g",
            aObj[nObjY].Get(ii,0), aObj[nObjY].Get(ii,1));
    return gstring(buf.p);
}


// Insert labels in legend box (Isoline graph)
void GraphDialog::ShowIsolineLegend()
{
	 SymbolLabel* pLabelS;
	 DragLabel* pLabelD;
	 
	 uint ii;
	 pGrp = new QGridLayout( pGrpLegend );
	 pGrp->setSpacing(6);
	 pGrp->setMargin(11);

    // Added scale lines
    for( ii = 0; ii <gr_data.scale.GetCount(); ii++ )
    {
      TPlotLine pl( "Scale",  P_FILLSQUARE, 0, 0, gr_data.scale[ii].red,
                     gr_data.scale[ii].green, gr_data.scale[ii].blue );
      gstring str = getTextIsoline(ii);

      pLabelS = new SymbolLabel( pGrpLegend, pl );
//      aSymbolLabels.Add( pLabelS );
      pGrp->addWidget(pLabelS, ii, 0);

      pLabelD = new DragLabel( pGrpLegend );
//      aLegendLabels.Add( pLabelD );
      pLabelD->setText( str.c_str() );
      pGrp->addWidget(pLabelD, ii, 1);
    }
    pGrp->setRowStretch ( ii, 1 ) ;
}


/* if this is for adding points - we don't need Clear() and Show() - just add
*/
void GraphDialog::ShowNew()
{
    plot->Clear();
    Show();
    plot->update();
    qApp->processEvents();
}


// show all graphs

void GraphDialog::Show()
{
	QPalette pl = plot->palette();

    if( gr_data.isBackgr_color )
    	pl.setColor( QPalette::Window/*QPalette::Background*/,
    			QColor( gr_data.b_color[0],
    					gr_data.b_color[1],
					gr_data.b_color[2]) );
    else
    	pl.setColor( QPalette::Window/*QPalette::Background*/,
    			Qt::white /*backgroundColor().dark(110)*/ );

    plot->setPalette(pl);

    plot->setPlotBounds( FPoint(minX, minY),
                   FPoint(maxX, maxY));

    ShowPlots();

    update();

}

// show plots
void GraphDialog::ShowPlots()
{
  gstring cap = "GEM-Selektor v.3 Graphics Dialog";

  switch( gr_data.graphType )
  {
   case LINES_POINTS:
       cap+= " (symbol/line plots)";
       setWindowTitle( cap.c_str() );
       for(uint ii=0, nLn =0; ii<gr_data.plots.GetCount(); ii++)
         for( int jj=0; jj<gr_data.plots[ii].getLinesNumber(); jj++, nLn++ )
         {
            TIArray<FPoint> pnts;
            gr_data.plots[ii].getPointLine( jj, pnts );
            for(uint i=0; i<pnts.GetCount(); i++)
            { //check location into min-max region
                /*      if( pnts[i].x >= minX && pnts[i].x<= maxX &&
                          pnts[i].y >= minY && pnts[i].y<= maxY )
                */
                if( pnts[i].IsEmpty() )
                  continue;
		plot->Add(new PPoint(plot, pnts[i],
            			gr_data.getType(nLn), gr_data.getSize(nLn), getColor(nLn)));

                if( gr_data.getLineSize(nLn) > 0 && i != 0 )
                { // insert line
                   if( !pnts[i-1].IsEmpty())
                    plot->Add(new PLine(plot, pnts[i-1], pnts[i],
                		    getColor(nLn), gr_data.getLineSize(nLn) ));
                }
                if( gr_data.getLineSize(nLn) < 0  )
                { // insert line
                    FPoint pp( pnts[i].x, gr_data.region[2] );
                    plot->Add(new PLine(plot, pp, pnts[i],
                		    getColor(nLn), gr_data.getLineSize(nLn) ));
                }
            }
        }
        break;
   case CUMULATIVE:
        {
         cap+= " (cumulative plot)";
         setWindowTitle( cap.c_str() );
         TIArray<FPoint> pnts0;
         TIArray<FPoint> pnts1;
//         if( gr_data.plots.GetCount() >1 )
//           vfMessage( this, "Graph demo",
//            "Tried  making the cumulative graphic with experimental data");

         gr_data.getPointCol( 0, pnts1 );
         for(int  ii=1; ii<gr_data.plots[0].getdX(); ii++ )
         {
           pnts0.Clear();
           for(uint kk=0; kk<pnts1.GetCount(); kk++)
             pnts0.Add( new FPoint(pnts1[kk]) );
           gr_data.getPointCol( ii, pnts1 );

           for(uint i=1; i<pnts1.GetCount(); i++)
           {
               if( pnts1[i].IsEmpty() ||
                 (  pnts0[i-1].y - pnts0[i].y == 0 &&
                    pnts1[i-1].y - pnts1[i].y == 0  ))
                  continue;
		plot->Add(new PPolygon(plot, getColor(i-1),
                     pnts0[i-1],pnts0[i],pnts1[i],pnts1[i-1]));
           }
         }
        }
        break;
   case ISOLINES:
        {
         int nLn, nScale;
         float x, y;
//         bool put_points = false;
         cap+= " (level line diagram) ";
         setWindowTitle( cap.c_str() );
/*
         for(int  ii=0; ii<gr_data.plots[0].getdX(); ii++ )
         {
           nLn = (int)aObj[gr_data.plots[0].getObjX()].Get(ii,0);
           if( nLn < 0)
           {
             put_points = true;
             break;
           }
         }
*/
         for(int  ii=0; ii<gr_data.plots[0].getdX(); ii++ )
         {
           // get point
           x = aObj[gr_data.plots[0].getObjY()].Get(ii,0);
           y = aObj[gr_data.plots[0].getObjY()].Get(ii,1);
           FPoint pnt(x,y);
           // get symbol type
           nLn = (int)aObj[gr_data.plots[0].getObjX()].Get(ii,0);
           // get scale color
           nScale = gr_data.getColorLine( ii );

           if( pnt.IsEmpty() || nLn == 0 ||
               (uint)abs(nLn) > gr_data.lines.GetCount() ||
               nScale < 0 )
             continue;   // undefined point or point type

           plot->Add(new PPoint(plot, pnt, gr_data.getType(abs(nLn)-1),
             gr_data.getSize(abs(nLn)-1), getColorIsoline(nScale)));

           if( /*put_points &&*/ nLn > 0 ) // get points from filters
           {
             int size = gr_data.getSize(abs(nLn)-1) / 3;
                 if( !size ) size = 1;
             plot->Add(new PPoint(plot, pnt, P_CROSSDIAG, size, Qt::black));
           }


         }
        }
        break;
  }
}

void
GraphDialog::resizeEvent(QResizeEvent* qpev)
{
    QDialog::resizeEvent(qpev);
    plot->resize(plotFrame->width(), plotFrame->height());
    update();
}

void
GraphDialog::CmLegend()
{
    LegendDialog legend( this );
    legend.exec();
}


void
GraphDialog::Apply()
{
    if( !pModule->SaveGraphData( &gr_data ) )
       return;

    plot->setGridCount(gr_data.axisType);

    if(  isFragment == true )
    {
        minX = gr_data.part[0];
        maxX = gr_data.part[1];
        minY = gr_data.part[2];
        maxY = gr_data.part[3];
    }
    else
    {
        minX = gr_data.region[0];
        maxX = gr_data.region[1];
        minY = gr_data.region[2];
        maxY = gr_data.region[3];
    }

    if( maxX == minX ) maxX += 1.;
    if( maxY == minY ) maxY += 1.;

// qt3to4 delete old widgets  ?????
 //   aSymbolLabels.Clear();
 //   aLegendLabels.Clear();
    
    delete pGrpLegend;  // must be deleted all children widgets
    pGrpLegend = new QGroupBox( this );
    pGrpLegend->setTitle(QApplication::translate("GraphDialogData", "Legend", 0, QApplication::UnicodeUTF8));
    gridLayout->addWidget(pGrpLegend, 0, 6, 1, 3);
    
    // Insert labels in legend box
    if( gr_data.graphType != ISOLINES )
      ShowLegend();
    else
      ShowIsolineLegend();

    pGrpLegend->update();

    plot->setAxisTitles(gr_data.xName.c_str(), gr_data.yName.c_str());
    plot->Clear();
    Show();
    plot->update();

}


void
GraphDialog::AddPoint( int nPlot, int nPoint, bool no_mt )
{

  if( gr_data.graphType )
   return;
 //qt3to4 qApp->lock();
  if( nPlot>=0 && nPoint>=0 )
  {
    int nLn = gr_data.plots[nPlot].getFirstLine();

    for( int jj=0; jj<gr_data.plots[nPlot].getLinesNumber(); jj++, nLn++ )
    {
        FPoint pnt = gr_data.plots[nPlot].getPoint( jj, nPoint );
        if( !pnt.IsEmpty() )
            plot->Add(new PPoint(plot, pnt, gr_data.getType(nLn),
                       gr_data.getSize(nLn), getColor(nLn)));
    }

    plot->update();
  }
  //qt3to4  qApp->unlock();
  if( no_mt)
    qApp->processEvents();
}

void
GraphDialog::CmFragment()
{

    if(  isFragment == false )
    {
        minX = gr_data.part[0];
        maxX = gr_data.part[1];
        minY = gr_data.part[2];
        maxY = gr_data.part[3];
        isFragment = true;
        pFragmentBtn->setText("&Full");
    }
    else
    {
        minX = gr_data.region[0];
        maxX = gr_data.region[1];
        minY = gr_data.region[2];
        maxY = gr_data.region[3];
        isFragment = false;
        pFragmentBtn->setText("&Fragment");
    }

    if( maxX == minX ) maxX += 1.;
    if( maxY == minY ) maxY += 1.;

    plot->setPlotBounds( FPoint(minX, minY),
                   FPoint(maxX, maxY));
    plot->update();
    //date();
}

void
GraphDialog::CmSave()
{
    QString filter;
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

        path /*const char* fl*/ = fn.toLatin1().data();
        //path = fl;
        gstring dir;
        gstring name;
        gstring newname;
        u_splitpath( path, dir, name, newname );
        dir  += "/";
        pVisor->setLocalDir( dir );

    }
}

#ifndef __unix
#if QT_VERSION < 3
extern void qt_set_sane_printing( bool b );
#endif
#endif

//Print graph screen to printer device
void
GraphDialog::CmPrint()
{
    QPrinter prt;

  //qt3to4  if( prt.setup() ) printerState()
    {
        QPainter painter(&prt);

#ifndef __unix
#if QT_VERSION < 3
    qt_set_sane_printing( true );
#endif
#endif

//        Q3PaintDeviceMetrics metrics(painter.device());

    painter.setPen(Qt::black);
	painter.setFont(pVisorImp->getAxisLabelFont());

	painter.translate(40, 50);
	painter.scale(painter.device()->logicalDpiX()/72., painter.device()->logicalDpiY()/72.); // seems to be always = 1
	plot->PaintToDC(painter);

    }
}

void
GraphDialog::setBackgrColor( QColor color )
{
    // Sveta 19/08/01 Save background
    gr_data.b_color[0] =   color.red();
    gr_data.b_color[1] = color.green();
    gr_data.b_color[2]  = color.blue();
    gr_data.isBackgr_color = true;

	QPalette pl = plot->palette();
    pl.setColor( QPalette::Window/*QPalette::Background*/, color );	
    plot->setPalette(pl);

//    plot->setBackgroundColor( color );
//    update();
    plot->update();
}

QColor
GraphDialog::getBackgrColor()
{
    return  plot->palette().color(QPalette::Window/*QPalette::Background*/); //backgroundColor();
}

void GraphDialog::CmHelp()
{
    pVisorImp->OpenHelp( GEMS_GRAPH_HTML );
}

//*************************************************
//--------- PlotTypeBtn -------
//*************************************************

void PlotTypeBtn::paintEvent( QPaintEvent * event ) 
{
	QPushButton::paintEvent( event );
	QPainter dc(this);
	drawButtonLabel(&dc);
}

void PlotTypeBtn::drawButtonLabel(QPainter* paint)
{
    QColor color( plLine.red, plLine.green, plLine.blue);

    QRect rect(3, 3, width()-6, height()-6);
    QPoint center = rect.center();

    int size = 4;//plLine.size;

    drawSymbol(paint, center, plLine.type, size, color);
    // line, not crossing the point mark
    if(plLine.line_size > 0 )
    {
        paint->drawLine( rect.x(), center.y(), center.x()-size, center.y());
        paint->drawLine( center.x()+size, center.y(), rect.width(), center.y());
    }
    // column, not crossing the point mark
    if(plLine.line_size < 0 )
    {
        paint->setPen( QPen(color, 9) );
        paint->drawLine( center.x(), center.y(), center.x(), height()-2);
    }

 //   QPushButton::drawButtonLabel(paint);
}


// static
void
PlotTypeBtn::drawSymbol(QPainter *paint, const QPoint& center,
			    int type, int size, const QColor& color, int width)
{
    //paint->setBrush( QBrush::NoBrush );
    paint->setPen( QPen(color, width) );
    switch(type)
    {
    case	 P_POINT:        // point
        size = 2;
        paint->drawEllipse( QRect(center-QPoint(size,size), center+QPoint(size+1,size+1)) );
        break;
    case	 P_CROSSDIAG:    // X
        paint->drawLine( center.x()-size, center.y()+size, center.x()+size+1, center.y()-size-1);
        paint->drawLine( center.x()-size, center.y()-size, center.x()+size+1, center.y()+size+1);
        break;
    case	 P_SQUARE:       // square
        paint->drawRect(QRect(center-QPoint(size,size), center+QPoint(size,size)));
        break;
    case	 P_FILLSQUARE:   // fill square
        paint->drawRect(QRect(center-QPoint(size,size), center+QPoint(size,size)));
        paint->fillRect(QRect(center-QPoint(size,size), center+QPoint(size,size)), QBrush(color));
        break;
    case  P_RHOMBUS:      // rhombus
        size++;
        paint->drawLine( center.x()-size, center.y(), center.x(), center.y()-size);
        paint->drawLine( center.x(), center.y()-size, center.x()+size, center.y());
        paint->drawLine( center.x()+size, center.y(), center.x(), center.y()+size);
        paint->drawLine( center.x(), center.y()+size, center.x()-size, center.y());
        break;
    case  P_WYE:          // Y
        paint->drawLine( center.x()-size, center.y()-size, center.x(), center.y());
        paint->drawLine( center.x()+size, center.y()-size, center.x(), center.y());
        paint->drawLine( center.x(), center.y(), center.x(), center.y()+size);
        break;
    case	 P_STAR:         // star
        {
            paint->drawLine( center.x()-size, center.y(), center.x()+size, center.y());
            paint->drawLine( center.x(), center.y()-size, center.x(), center.y()+size);
            int scale = (int)ROUND((float)size*0.7+0.5);
            paint->drawLine( center.x()-scale, center.y()-scale, center.x()+scale, center.y()+scale);
            paint->drawLine( center.x()-scale, center.y()+scale, center.x()+scale, center.y()-scale);
            break;
        }
    case	 P_CIRCLE:        // circle
        paint->drawEllipse( QRect(center-QPoint(size,size), center+QPoint(size,size)) );
        break;
    case	 P_FILLCIRCLE:        // fill circle
        paint->drawEllipse( QRect(center-QPoint(size,size), center+QPoint(size,size)) );
        //center+QPoint(size+1,size+1)) );
        paint->setBrush( color );
        paint->drawEllipse( QRect(center-QPoint(size,size), center+QPoint(size,size)) );
        // center+QPoint(size+1,size+1)) );
        break;
    case  P_FILLRHOMBUS:      // fill rhombus
        size++;
        QPolygon arr(4);
        paint->setBrush( color );
        arr.setPoint( 0, center.x()-size, center.y());
        arr.setPoint( 1, center.x(), center.y()-size);
        arr.setPoint( 2, center.x()+size, center.y());
        arr.setPoint( 3, center.x(), center.y()+size);
        paint->drawPolygon(arr);
        break;
    }
}

void PlotTypeBtn::setData( TPlotLine& ln)
{
    plLine = ln;
    update();
}

void PlotTypeBtn::setColor( QColor& cl)
{
   plLine.red = cl.red();
   plLine.green = cl.green();
   plLine.blue = cl.blue();
   update();
}


void PlotTypeBtn::setName( const char* name)
{
    plLine.setName(name);
    //strncpy(plLine.name, name, 15 );
}



//--------------------- End of GraphDialog.cpp ---------------------------
