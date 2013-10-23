//-------------------------------------------------------------------
// $Id: model_w.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of TObjectModel, TObjectDelegate, TObjectTable,
// TCell, TCellInput, TCellCheck, TCellText classes
//
// Copyright (C) 2008 S.Dmytriyeva
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

//#define QT_KEYPAD_NAVIGATION
#include <QtGui>

#include "model_w.h"
#include "units.h"
#include "service.h"
#include "m_sdata.h"
#include "m_param.h"
#include "CalcDialog.h"
#include "CalcCheckDialog.h"
//#include "GemsMainWindow.h"

// temporary workaround to make empty value look better
//const gstring emptiness("---");
//const gstring short_emptiness("---");
//const int szSBAR = 16;		// size of TFiled 'Grid' ScrollBar

//--------------------------------------------------------------------------------------
//  class TObjectModel
//  class for represents the data set and is responsible for fetchin
//  the data is neaded for viewing and for writing back any changes.
//  Reading/writing data from/to TObject and TObjList classes
//---------------------------------------------------------------------------------------

TObjectModel::TObjectModel( const QList<FieldInfo>	aFlds,  QObject * parent ):
		QAbstractTableModel(parent) 	  
{
	flds = aFlds;
	connect( this, SIGNAL(dataChanged( const QModelIndex&, const QModelIndex& )),
			parent,  SLOT(objectChanged()) );	 
}


int TObjectModel::rCount() const
{
   int deltaN = 0, sizeN = flds[0].pObj->GetNS();
   for(int  ii=1; ii< flds.count(); ii++)
   {
         if(  flds[ii].place == Tied )
                 sizeN = max( sizeN, flds[ii].pObj->GetNS() );

         if(  flds[ii].place == Sticked || flds[ii].place == UndeTabl )
         {    deltaN += sizeN;
              sizeN = flds[ii].pObj->GetNS();
         }
   }
  return deltaN + sizeN;
}

int TObjectModel::rowCount( const QModelIndex & /*parent*/ ) const
{
     return rCount();
}	

int TObjectModel::cCount() const
{
   int ii=0;

   // test mutable fiels 12/03/13
   if( flds[ii].place == MutableB && flds.count() >= 4 )
   {
        //if(  aFlds[1].place == Tied && aFlds[2].place == UndeTabl && aFlds[3].place == Tied)
       if( flds[1].pObj->GetMS() >= flds[2].pObj->GetMS()+flds[3].pObj->GetMS())
         {   ii++;  }
   }

   int currentdeltaM = 0, deltaM = 0, sizeM = flds[ii].pObj->GetMS();
   ii++;
   for(  ii; ii< flds.count(); ii++)
   {
       if(  flds[ii].place == UndeTabl )
       { deltaM = max(deltaM, currentdeltaM + sizeM);
         currentdeltaM = 0;
         sizeM =0;
       }
       if(  flds[ii].place == Sticked || flds[ii].place == UndeTabl )
            sizeM = max( sizeM, flds[ii].pObj->GetMS() );
        if(  flds[ii].place == Tied )
        {    currentdeltaM += sizeM;
             sizeM = flds[ii].pObj->GetMS();
        }
  }
  return max(deltaM, currentdeltaM+sizeM );
}

int TObjectModel::columnCount( const QModelIndex & /*parent*/ ) const
{
   return cCount();
}	

int TObjectModel::getObjFromModel( int row, int col, 
                int& nO, int& iN, int &iM, Selection* sel ) const
{
   int iistart, ii = 0;
    // test mutable fiels 12/03/13
    if( flds[ii].place == MutableB && flds.count() >= 4 )
    {
        //if(  aFlds[1].place == Tied && aFlds[2].place == UndeTabl && aFlds[3].place == Tied)
        if( flds[1].pObj->GetMS() >= flds[2].pObj->GetMS()+flds[3].pObj->GetMS())
          {   ii++;  }
    }

  int deltaN = 0, sizeN = flds[ii].pObj->GetNS();
  int fuldeltaM = 0, deltaM = 0,sizeM = flds[ii].pObj->GetMS();
  
  nO = iN = iM = -1;
  
  iistart = ii;
  for( ii; ii< flds.count(); ii++)
  {
    if( ii > iistart && flds[ii].place == Tied )
    {	deltaM += sizeM;
	    sizeM = flds[ii].pObj->GetMS();
        sizeN = max( sizeN, flds[ii].pObj->GetNS() );
    }
    if( flds[ii].place == UndeTabl )
    {
       fuldeltaM += deltaM;
       deltaM = 0;
       sizeM = 0;
    }
    if( ii > iistart && ( flds[ii].place == Sticked || flds[ii].place == UndeTabl ) )
    {
    	deltaN += sizeN;
    	sizeN = flds[ii].pObj->GetNS();
        sizeM = max( sizeM, flds[ii].pObj->GetMS() );
    }
    if( ( deltaN + flds[ii].pObj->GetNS() > row ) &&
	    ( deltaM + flds[ii].pObj->GetMS() > col ) ) 
	 {
		 nO =  flds[ii].nO;
		 iN = row - deltaN;
		 iM = col - deltaM;
		 if( iN < 0 || iM < 0 )
		 {	 
		     nO = -1;
		 	 return -1; 
		 }
		 if( sel ) // get selection
                 {
                    // deltaM += fuldeltaM;
			   sel->N1 = max(deltaN, sel->N1 );
			   sel->N2 = min(deltaN + flds[ii].pObj->GetNS()-1, sel->N2 );
			   sel->M1 = max(deltaM, sel->M1 );
			   sel->M2 = min(deltaM + flds[ii].pObj->GetMS()-1, sel->M2 );
                           /*if( to_calc == 1 )
			   {
				 sel->N1 -= deltaN;
				 sel->N2 -= deltaN;
				 sel->M1 -= deltaM;
				 sel->M2 -= deltaM;
                           }*/
		 }
		 return ii;
	 }
  }
 return -1;
}

QString TObjectModel::getDescription( TObject* pObj, int N, int M) const
{
    if( !pObj )
      return "";
    
	gstring desc = pObj->GetDescription(N,M);
    QString s;
    if( pObj->GetNS()!=1 || pObj->GetMS()!=1 )
        s = QString("%1 [%2,%3] : %4").arg( pObj->GetKeywd(), QVariant(N).toString(), 
        		QVariant(M).toString(), desc.c_str() );
    else
        s = QString("%1 : %2").arg( pObj->GetKeywd(), desc.c_str() );

    return s;
}

QVariant TObjectModel::data( const QModelIndex& index, int role ) const
{
	int nO, iN, iM;
	
	if(!index.isValid())
	 return QVariant();	

        int ii = getObjFromModel( index.row(), index.column(), nO, iN, iM);

        switch( role )
        { case Qt::DisplayRole:
          case Qt::EditRole:
               if( nO == -1 )
                   return  QString("");
            return  QString( visualizeEmpty(aObj[nO].GetStringEmpty( iN, iM )).c_str() );
          case Qt::ToolTipRole:
          case Qt::StatusTipRole:
               if( nO >= 0 )
                     return  getDescription(&aObj[nO], iN, iM );
               break;
          case Qt::TextAlignmentRole:
               if(ii < 0 )
                   break;
               if( flds[ii].fType == ftText )
                  return int(Qt::AlignLeft | Qt::AlignTop);
                else if( flds[ii].fType == ftNumeric || flds[ii].fType == ftFloat )
                       return int(Qt::AlignRight | Qt::AlignVCenter);
                     else return int(Qt::AlignLeft | Qt::AlignVCenter);
          case  Qt::SizeHintRole:
                if(ii >= 0 )
                { // scroling for TextEdit fields
                  //22/04/2010 if( !(ii==0 && flds[ii].fType == ftText))
                   return QSize( wdF(flds[ii].fType, flds[ii].npos, flds[ii].edit),
                                htF(flds[ii].fType, flds[ii].maxN) );
                }
         default: break;
        }
        return QVariant();
}

bool TObjectModel::setData( const QModelIndex &index, const QVariant &value, int role)
{
	int nO, iN, iM;
 	
	if( index.isValid() && ( role == Qt::EditRole ) )
	{
          int iifld = getObjFromModel( index.row(), index.column(), nO, iN, iM);
          if( nO >= 0 &&  flds[iifld].edit == eYes)
	  {
		gstring txt = QVariant(value).toString().toLatin1().data();
	
              if( txt == emptiness /*|| txt == short_emptiness*/ )
	      aObj[nO].SetString( S_EMPTY, iN, iM );
	    else
	      aObj[nO].SetString( txt.c_str(), iN, iM );
	    emit dataChanged(index, index);
	  }	  
	 return true;
	} 
	return false;
}

Qt::ItemFlags TObjectModel::flags( const QModelIndex & index ) const  
{
    int nO, iN, iM;
    Qt::ItemFlags flags = QAbstractTableModel::flags(index);
    int ii = getObjFromModel( index.row(), index.column(), nO, iN, iM);
    if(ii >= 0 )
      if( flds[ii].edit == eYes )
        {  flags |= Qt::ItemIsEditable;
           return flags;
       }
  return (flags & ~Qt::ItemIsEditable);

}

QVariant TObjectModel::headerData( int section, Qt::Orientation orientation, int role ) const 
{
        int nO, iN, iM, ii=-1;
	//  fstream f_log("ipmlog.txt", ios::out|ios::app );

      if( orientation == Qt::Horizontal )
      {
        for(int i=0; i<rCount(); i++)
        {
          ii = getObjFromModel( i, section, nO, iN, iM);
          if( ii>=0 )
              break;
        }
      }
      else
          ii = getObjFromModel(  section, 0, nO, iN, iM);

      switch( role )
      { case Qt::DisplayRole:
          if(ii < 0 )
              break;
          if( orientation == Qt::Horizontal )
          {
           if( flds[ii].label && aObj[nO].GetNS() > 1  )
           {  if(aObj[nO].GetMS() > 1 )
                         return QString( "%1[%2]").arg(aObj[nO].GetKeywd()).arg(iM);
                 else
                         return aObj[nO].GetKeywd();
           }
          }
          else
          {   if( flds[ii].label && iN == 0  && aObj[nO].GetNS() == 1) // only line objects
                 return aObj[nO].GetKeywd();
              else
                  if( aObj[nO].GetNS() > 1 ) // only multi
                       return QVariant(iN);
           }
           break;
        case Qt::SizeHintRole:
           if(ii < 0 )
              return QSize(0,0);
           if( orientation == Qt::Horizontal )
           {
              if( flds[ii].label && aObj[nO].GetNS() > 1 /*&& iM == 0*/ )
                 return QSize( wdF(flds[ii].fType, flds[ii].npos, flds[ii].edit),
                                  htF(flds[ii].fType, 1 )+4 );
              else
                 return QSize( wdF(flds[ii].fType, flds[ii].npos, flds[ii].edit), 0);
           }
           else //if( orientation == Qt::Vertical )
                  {
                      if( flds[ii].label && iN == 0  && aObj[nO].GetNS() == 1) // only line objects
                         return QSize( wdF(ftString, 7, eNo ), htF(flds[ii].fType, flds[ii].maxN));
                      else  if( aObj[nO].GetNS() > 1 ) // only multi
                                return QSize( wdF(ftString, 4, eNo ), htF(flds[ii].fType, flds[ii].maxN));
                            else
                                return QSize( 0, htF(flds[ii].fType, flds[ii].maxN));
                   }
             break;
        case Qt::TextAlignmentRole:
               if(ii < 0 )
                   break;
               if( flds[ii].fType == ftText )
                  return int(Qt::AlignLeft | Qt::AlignTop);
               else return int(Qt::AlignLeft | Qt::AlignVCenter);
        default:
        break;
       }

	return QVariant();
}

// return place of object defined by Model
ePlaceMode TObjectModel::getObjectPlace()
{
#if QT_VERSION >= 0x050000

    beginResetModel();
    endResetModel();
#else
    reset();

#endif
    return flds[0].place;
}


//-------------------------------------------------------------------------------------
// class TObjectTable implements a table view that displays items from a model.
//-------------------------------------------------------------------------------------

TObjectTable::TObjectTable( const QList<FieldInfo> aFlds, 
		       QWidget * parent ):
	 QTableView( parent ), flds(aFlds)
{
    vScroll =false, hScroll=false;

    setFont( pVisorImp->getCellFont() );
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

#if QT_VERSION >= 0x050000

    verticalHeader()->setSectionResizeMode( QHeaderView::ResizeToContents/*QHeaderView::Stretch*/ );
    horizontalHeader()->setSectionResizeMode( QHeaderView::ResizeToContents/*QHeaderView::Stretch*/ );
#else
    verticalHeader()->setResizeMode( QHeaderView::ResizeToContents/*QHeaderView::Stretch*/ );
    horizontalHeader()->setResizeMode( QHeaderView::ResizeToContents/*QHeaderView::Stretch*/ );

#endif
    //setEditTriggers( QAbstractItemView::DoubleClicked|QAbstractItemView::AnyKeyPressed );
    setFocusPolicy(Qt::WheelFocus/*Qt::StrongFocus*/);
    setTabKeyNavigation( false );
    setContextMenuPolicy(Qt::CustomContextMenu);

        if( flds[0].fType == ftText )
        {
            setVerticalScrollMode( QAbstractItemView::ScrollPerPixel );
            setHorizontalScrollMode( QAbstractItemView::ScrollPerPixel );
            setEditTriggers( QAbstractItemView::SelectedClicked|
                             QAbstractItemView::DoubleClicked|QAbstractItemView::AnyKeyPressed );
        }
        else
        {
            setVerticalScrollMode( QAbstractItemView::ScrollPerItem );
            setHorizontalScrollMode( QAbstractItemView::ScrollPerItem );
            setEditTriggers( QAbstractItemView::DoubleClicked|QAbstractItemView::AnyKeyPressed );
        }
    
       connect( this, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(slotPopupContextMenu(QPoint)));
}

 void TObjectTable::updateStatus( const QModelIndex & current)
 {
   gstring txt = current.data(Qt::StatusTipRole).toString().toLatin1().data();
     pVisorImp->SetStatus( txt.c_str());
 }
 
 static bool no_menu_out = true;
 static bool no_menu_in = true;
 void TObjectTable::focusOutEvent( QFocusEvent * event )
 {
        if( no_menu_out )
	    clearSelection();
         no_menu_out = true;
	 QTableView::focusOutEvent( event );
 }
 
 void TObjectTable::focusInEvent( QFocusEvent * event )
  {
    updateStatus( currentIndex() );
    QTableView::focusInEvent( event );
   if( no_menu_in )
    selectionModel()->select( currentIndex(),
              QItemSelectionModel::SelectCurrent );
    no_menu_in = true;
  }

 void TObjectTable::currentChanged ( const QModelIndex & current, const QModelIndex & previous )
 {
	 updateStatus( current );
     QTableView::currentChanged(current, previous ); 	 
 }
/*22/04/2010
 bool TObjectTable::edit( const QModelIndex & index, EditTrigger trigger, QEvent * event )
 {
    vScroll =false, hScroll=false;
    if( flds[0].fType == ftText && event &&
         ( event->type() == QEvent::KeyPress ||
           event->type() == QEvent::MouseButtonDblClick) )
    {
        //scrollToTop();
        verticalScrollBar()->setValue(verticalScrollBar()->minimum());
        horizontalScrollBar()->setValue(horizontalScrollBar()->minimum());
        vScroll = verticalScrollBar()->isVisible();
        verticalScrollBar()->setVisible(false);
        hScroll = horizontalScrollBar()->isVisible();
        horizontalScrollBar()->setVisible(false);
        cout << event->type() << "  vScroll  " << vScroll << "  hScroll  " << hScroll << endl;
    }
   bool ret = QAbstractItemView::edit( index, trigger, event );
   return ret;
 }

 void TObjectTable::closeEditor( QWidget * editor, QAbstractItemDelegate::EndEditHint hint )
 {
   if( vScroll )
       verticalScrollBar()->setVisible(true);
   if( hScroll )
       horizontalScrollBar()->setVisible(true);
    vScroll =false, hScroll=false;
    QAbstractItemView::closeEditor( editor, hint );
}
22/04/2010*/

 // return current size of object defined by Model
 void TObjectTable::getObjectSize( int& rowSize, int& colSize )
 {
        int fullrowSize, fullcolSize;
        int ii=0, col_ii = 0, row_ii=0;

        // test mutable fiels 12/03/13
        if( flds[ii].place == MutableB && flds.count() >= 4 )
        {
             //if(  aFlds[1].place == Tied && aFlds[2].place == UndeTabl && aFlds[3].place == Tied)
            if( flds[1].pObj->GetMS() >= flds[2].pObj->GetMS()+flds[3].pObj->GetMS())
              {   ii++;  }
        }
        int sizeN = flds[ii].pObj->GetNS();
        int sizeM = flds[ii].pObj->GetMS();

// scroling for TextEdit fields
       //22/04/2010
      if( flds[ii].fType == ftText )
      { if(sizeN > 0 )
          {      colSize =  wdF(flds[ii].fType, flds[ii].npos, flds[ii].edit)+1;
                 rowSize  =  htF(flds[ii].fType, flds[ii].maxN )+1;
          }
            else colSize = rowSize =0;
        }
        else
	{	
      colSize = ( horizontalHeader()->sectionSize(0) * min(flds[ii].maxM, sizeM ) );
      rowSize  = ( verticalHeader()->sectionSize(0) * min(flds[ii].maxN, sizeN ) );
	}

    fullcolSize = ( horizontalHeader()->sectionSize(0) *  sizeM  );
 	fullrowSize  = ( verticalHeader()->sectionSize(0) *  sizeN  );
 	
    int maxColSize = colSize;
    int maxfullcolSize = fullcolSize;

    ii++;
    for( ii; ii< flds.count(); ii++)
 	{
        if(  flds[ii].place == Tied )
 		 {
            sizeN = max( sizeN, flds[ii].pObj->GetNS() );
            col_ii += sizeM;
 			sizeM = flds[ii].pObj->GetMS();

            if(flds[ii].pObj->GetNS() == 0 ||sizeM ==0 )
              continue;

            if(fullrowSize == 0 || fullcolSize == 0) // first not empty
            {
              rowSize  += ( verticalHeader()->sectionSize(row_ii) * min(flds[ii].maxN, sizeN ) );
              fullrowSize  += ( verticalHeader()->sectionSize(row_ii) *  sizeN  );
            }

            colSize += ( horizontalHeader()->sectionSize(col_ii) * min(abs(flds[ii].maxM), sizeM ) );
 			fullcolSize += ( horizontalHeader()->sectionSize(col_ii) * sizeM  );
         }

         if( flds[ii].place == UndeTabl )
         {
                     maxColSize = max( maxColSize, colSize);
                     maxfullcolSize = max( maxfullcolSize, fullcolSize);
                     col_ii=0;
                     colSize = 0;
                     fullcolSize =0;
                     sizeM = 0;
         }

         if(  flds[ii].place == Sticked || flds[ii].place == UndeTabl )
 		 {
 			 sizeM = max( sizeM, flds[ii].pObj->GetMS() );
 			 row_ii += sizeN;
 			 sizeN = flds[ii].pObj->GetNS();

             if( flds[ii].pObj->GetMS() == 0 || sizeN ==0 )
               continue;

             rowSize += ( verticalHeader()->sectionSize( row_ii ) * min(abs(flds[ii].maxN), sizeN ) );
 	  		 fullrowSize += ( verticalHeader()->sectionSize( row_ii ) *  sizeN  );
             if(fullcolSize == 0 /*&& maxfullcolSize==0*/ ) // first not empty
             {
               colSize = ( horizontalHeader()->sectionSize(col_ii) * min(flds[ii].maxM, sizeM ) );
               fullcolSize = ( horizontalHeader()->sectionSize(col_ii) *  sizeM  );
             }
          }
 	}	
 	
    colSize = max( maxColSize, colSize);
    fullcolSize = max( maxfullcolSize, fullcolSize);

 	if( fullcolSize > colSize )
        {
                rowSize += fontMetrics().height()+1;
                //rowSize += horizontalScrollBar()->height();//szSBAR; (changed after update from 30 to 16 )
  //	cout << "font metric" << QFontMetrics( pVisorImp->getCellFont() ).height() 
  //	<< "horizontalScrollBar()->height();" << horizontalScrollBar()->height() << endl;
 	}	
 	
 	if( fullrowSize > rowSize )
        {	colSize += fontMetrics().width('M')*2+1;
                //colSize += verticalScrollBar()->width();//szSBAR;
 	}	
 	
 	// for frame
 	if( colSize > 0 && rowSize > 0 )
 	{ colSize +=  4;
 	  rowSize +=  4;
 	}  

        int  colHead = model()->headerData( 0,  Qt::Vertical, Qt::SizeHintRole ).toSize().width();
        for( ii=1; ii<model()->rowCount(); ii++ )
            colHead = max( colHead, model()->headerData( ii,  Qt::Vertical, Qt::SizeHintRole ).toSize().width());
        colSize += colHead;

        int  rowHead = model()->headerData( 0,  Qt::Horizontal, Qt::SizeHintRole ).toSize().height();
        for( ii=1; ii<model()->columnCount(); ii++ )
            rowHead = max( rowHead, model()->headerData( ii,  Qt::Horizontal, Qt::SizeHintRole ).toSize().height());
        rowSize += rowHead;
        // 	colSize += verticalHeader()->width();
        //   rowSize += horizontalHeader()->height();
    
 //if(  flds.count() > 3 )
 //     gstring(flds[2].pObj->GetKeywd(), 0, 4).find("mCIb") != gstring::npos
 //    )
 //  cout << "colSize " << colSize  << " rowSize " << rowSize << endl;
}

 void TObjectTable::slotPopupContextMenu(const QPoint &pos)
 {
     QModelIndex index = indexAt( pos );
     TObjectModel* model =(TObjectModel *)index.model();
 	int iN, iM;
 	FieldInfo fld =  model->getInfo( index.row(), index.column(), iN, iM);
     if(iN == -1 || iM == -1 )
     	return;
 	
     QMenu *menu = new QMenu(this);
     no_menu_out = false;
     no_menu_in = false;

     QAction* act =  new QAction(tr("&Help"), this);
     act->setIcon(QIcon(":/menu/Icons/ShowHelpWindowIcon.png"));
     act->setShortcut(tr("F1"));
     act->setStatusTip(tr("Help to the specified cell"));
     connect(act, SIGNAL(triggered()), this, SLOT(CmHelp()));
     menu->addAction(act);
         
     if( fld.fType == ftRef && fld.edit == eYes )
       {
         act =  new QAction(tr("&SDRef-Record"), this);
         act->setShortcut(tr("F2"));
         act->setStatusTip(tr("Open/add SDref record"));
         connect(act, SIGNAL(triggered()), this, SLOT(CmSDRef()));
         menu->addAction(act);
        }	
     	
     if( fld.fType == ftRecord )
        {
          act =  new QAction(tr("&Show record"), this);
          act->setShortcut(tr("F7"));
          act->setStatusTip(tr("Show record for current key"));
          connect(act, SIGNAL(triggered()), this, SLOT(CmDComp()));
          menu->addAction(act);
        }	
        
     if( fld.fType == ftRecord && (fld.pObj->GetKeywd()[0] == 'D' || fld.pObj->GetKeywd()[0] == 'P'))
        {
          act =  new QAction(tr("&Show phase"), this);
          act->setShortcut(tr("F5"));
          act->setStatusTip(tr("Show phase/dcomp list for current key"));
          connect(act, SIGNAL(triggered()), this, SLOT(CmShowPhaseKey()));
          menu->addAction(act);
        }


     if( (fld.fType == ftFloat || fld.fType == ftNumeric ||
        		fld.fType == ftCheckBox) && fld.edit == eYes )
         {
     	    menu->addSeparator();
            act =  new QAction(tr("&Calculator"), this);
            act->setShortcut(tr("F8"));
            act->setStatusTip(tr("Use calculator  to the specified cells"));
       	    connect(act, SIGNAL(triggered()), this, SLOT(CmCalc()));
            menu->addAction(act);
         }
        
      menu->addSeparator();
        
      if( model->columnCount( index ) > 1 )
      {		
       	act =  new QAction(tr("Select &row"), this);
        act->setShortcut(tr("Ctrl+R"));
        act->setStatusTip(tr("Select current row"));
  	    connect(act, SIGNAL(triggered()), this, SLOT(SelectRow()));
        menu->addAction(act);
      }

      if( model->rowCount( index ) > 1 )
      {
        act =  new QAction(tr("Select co&lumn"), this);
        act->setShortcut(tr("Ctrl+L"));
        act->setStatusTip(tr("Select current column"));
  	    connect(act, SIGNAL(triggered()), this, SLOT(SelectColumn()));
        menu->addAction(act);
      } 
      
        act =  new QAction(tr("Select &object"), this);
        act->setShortcut(tr("Ctrl+O"));
        act->setStatusTip(tr("Select current object"));
  	    connect(act, SIGNAL(triggered()), this, SLOT(SelectObject()));
        menu->addAction(act);

        act =  new QAction(tr("Select &all"), this);
        act->setShortcut(tr("Ctrl+A"));
        act->setStatusTip(tr("Select all cells"));
  	    connect(act, SIGNAL(triggered()), this, SLOT(SelectAll()));
        menu->addAction(act);

        menu->addSeparator();

        if( fld.edit == eYes )
        {
            act =  new QAction(tr("C&ut"), this);
            act->setShortcut(tr("Ctrl+U"));
            act->setStatusTip(tr("Cut selected cells"));
      	    connect(act, SIGNAL(triggered()), this, SLOT(CutData()));
            menu->addAction(act);
        }

        act =  new QAction(tr("&Copy"), this);
        act->setShortcut(tr("Ctrl+C"));
        act->setStatusTip(tr("Copy selected cells"));
  	    connect(act, SIGNAL(triggered()), this, SLOT(CopyData()));
        menu->addAction(act);

        act =  new QAction(tr("Copy wit&h names"), this);
        act->setShortcut(tr("Ctrl+H"));
        act->setStatusTip(tr("Copy selected header&cells"));
  	    connect(act, SIGNAL(triggered()), this, SLOT(CopyDataHeader()));
        menu->addAction(act);

        if( fld.edit == eYes )
        {
            act =  new QAction(tr("&Paste"), this);
            act->setShortcut(tr("Ctrl+V"));
            act->setStatusTip(tr("Paste to selected area"));
      	    connect(act, SIGNAL(triggered()), this, SLOT(PasteData()));
            menu->addAction(act);
 
            act =  new QAction(tr("Paste &transposed"), this);
            act->setShortcut(tr("Ctrl+T"));
            act->setStatusTip(tr("Paste transposed to selected area"));
      	    connect(act, SIGNAL(triggered()), this, SLOT(PasteTransposedData()));
            menu->addAction(act);

            act =  new QAction(tr("&Clear"), this);
            act->setShortcut(tr("Ctrl+Del"));
            act->setStatusTip(tr("Clear selected cells"));
      	    connect(act, SIGNAL(triggered()), this, SLOT(ClearData()));
            menu->addAction(act);
        }
        
        menu->exec( viewport()->mapToGlobal(pos) );
        delete menu;
 }

 void TObjectTable::keyPressEvent(QKeyEvent* e)
 {
    if ( e->modifiers() & Qt::ControlModifier ) 
    {
		switch ( e->key() ) 
		{
		  case Qt::Key_R:
		    SelectRow();
		    return;
		  case Qt::Key_L:
		    SelectColumn();
		    return;
		  case Qt::Key_O:
		    SelectObject();
		    return;
		  case Qt::Key_A:
		    SelectAll();
		    return;
		  case Qt::Key_U:
		    CutData();
		    return;
		  case Qt::Key_C:
		    CopyData();
		    return;
		  case Qt::Key_H:
		    CopyDataHeader();
		    return;
                  case Qt::Key_V:
                    { PasteData();
                      QModelIndex index = currentIndex();
                      qobject_cast<TObjectModel *>(model())->getObjectPlace();
                      setCurrentIndex ( index );
                      selectionModel()->select( index,
                         QItemSelectionModel::SelectCurrent );
                    }
                    return;
		  case Qt::Key_T:
                    { PasteTransposedData();
                      QModelIndex index = currentIndex();
                      qobject_cast<TObjectModel *>(model())->getObjectPlace();
                      setCurrentIndex ( index );
                      selectionModel()->select( index,
                         QItemSelectionModel::SelectCurrent );
                    }
                   return;
		  case Qt::Key_Delete:
		    ClearData();
                    return;
		}
	}
    switch( e->key() )
 	{
 	  case Qt::Key_F1:
 	        CmHelp();
 	        return;
 	  case Qt::Key_F8:
 	         CmCalc();
 	          return;
      case Qt::Key_F2:
 	         CmSDRef();
 	         return;
      case Qt::Key_F7:
 	         CmDComp();
 	         return;
    case Qt::Key_F5:
           CmShowPhaseKey();
           return;
 	 }
 	QTableView::keyPressEvent(e);
 }

 // Help on F1 pressed on data field
 void TObjectTable::CmHelp()
 {
    QModelIndex index = currentIndex();
 	int iN, iM;
 	FieldInfo fld =  ((TObjectModel *)(index.model() ))->getInfo( index.row(), index.column(), iN, iM);
    if(iN == -1 || iM == -1 )
     	return;
     
     //gstring item = fld.pObj->GetFullName(iN,iM); // for old indexation
     gstring item = fld.pObj->GetHelpLink(iN,iM);
     pVisorImp->OpenHelp( 0, item.c_str());
 }

 // Show SDRef-Record on F2 pressed on data field
 void TObjectTable::CmSDRef()
 {
  QModelIndex index = currentIndex();
  int iN, iM;
  FieldInfo fld =  ((TObjectModel *)(index.model() ))->getInfo( index.row(), index.column(), iN, iM);
  if(iN == -1 || iM == -1 || fld.fType != ftRef || fld.edit != eYes )
         return;
  
  gstring str = index.data(Qt::EditRole).toString().toLatin1().data();
  try
  {
         bool patt = false;
         //save new SDrefs
         gstring sd_key;
         if( !strcmp( fld.pObj->GetKeywd(), "SDrefs") )
             sd_key = gstring( TSData::pm->GetKey() );

         if( str.find_first_of("*?") != gstring::npos )  // pattern
         { // Sveta
             str = TSData::pm->GetKeyofRecord( str.c_str(), "Get key", KEY_NEW);
             if( str.empty() )
                 return;
             patt = true;
         }
         if( patt )
         {
        	 TObjectModel *  model = ((TObjectModel *)(index.model() ));
        	 model->setData(index, str.c_str(),  Qt::EditRole);
             //save new SDrefs
             if( !strcmp(fld.pObj->GetKeywd(), "SDrefs") )
             {
                 // setValue();
                 if( sd_key.find_first_of("*?") == gstring::npos )  // pattern
                     TSData::pm->RecSave( sd_key.c_str(), false );
             }
         }
         time_t tim;
         TSData::pm->TryRecInp( str.c_str(), tim, 0 );

         //if( rt[RT_SDATA].Find( str.c_str() ) >= 0)
         //    TSData::pm->RecInput( str.c_str() );

         pVisorImp->OpenModule(topLevelWidget(), RT_SDATA,0,true);
         TSData::pm->Update();
     }
     catch( TError& err )
     {
         vfMessage(topLevelWidget(), err.title, err.mess );
     }
 }

// Calculator on F8 pressed on data field
void TObjectTable::CmCalc()
{
  QModelIndex index = currentIndex();
  TObjectModel *  model = ((TObjectModel *)(currentIndex().model() ));
  Selection sel = getSelectionRange();
  QModelIndex wIndex;
  
  int iN, iM;
  FieldInfo fld =  ((TObjectModel *)(index.model() ))->getInfo( 
                  index.row(), index.column(), iN, iM, &sel );
  if(iN == -1 || iM == -1 || fld.edit != eYes )
	   	return;

  if(  fld.fType == ftFloat || fld.fType == ftNumeric )
  {
      QString res;
      CalcDialog calc(topLevelWidget(), fld.nO );
      if( calc.exec() )
      {
          for(int nn=sel.N1; nn<=sel.N2; nn++)
             for(int mm=sel.M1; mm<=sel.M2; mm++ )
              {
 //     	 fld.pObj->Put(calc.fun(fld.pObj->Get(nn, mm)), nn, mm);
                 wIndex = 	index.sibling( nn, mm );
                 res = calc.fun( wIndex.data(Qt::EditRole).toDouble() );
                 model->setData(wIndex, res, Qt::EditRole);
             }
     }
  }
  
  if(  fld.fType == ftCheckBox )
  {
	// check selection
	gstring Vals = aUnits[fld.npos].getVals(iM);
        CalcCheckDialog calc(topLevelWidget(), fld.nO, Vals);

    if( calc.exec() )
    {
      int ii = calc.fun();
      for(int nn=sel.N1; nn<=sel.N2; nn++)
      {
          wIndex = 	index.sibling( nn, sel.M1 );
          model->setData(wIndex, QString(gstring(Vals, ii, 1).c_str()), Qt::EditRole);
        // fld.pObj->SetString( gstring(Vals, ii, 1).c_str(), nn, iM);
      }
    }
  }
 }

 // Show record on F7 pressed on data field
  void TObjectTable::CmDComp()
  {
    QModelIndex index = currentIndex();
    int iN, iM;
    FieldInfo fld =  ((TObjectModel *)(index.model() ))->getInfo( index.row(), index.column(), iN, iM);
    if(iN == -1 || iM == -1 || fld.fType != ftRecord )
 	   	return;
    try {
         TProfil::pm->ShowDBWindow( fld.pObj->GetKeywd(), iN);
    }
     catch( TError& err )
     {
         vfMessage(topLevelWidget(), err.title, err.mess );
     }
  }

  // Show phase key or dcomp list on F8 pressed on data field
   void TObjectTable::CmShowPhaseKey()
   {
     QModelIndex index = currentIndex();
     int iN, iM;
     FieldInfo fld =  ((TObjectModel *)(index.model() ))->getInfo( index.row(), index.column(), iN, iM);
     if(iN == -1 || iM == -1 || fld.fType != ftRecord )
         return;
     try {
          TProfil::pm->ShowPhaseWindow( topLevelWidget(), fld.pObj->GetKeywd(), iN);
     }
      catch( TError& err )
      {
          vfMessage(topLevelWidget(), err.title, err.mess );
      }
   }

  void TObjectTable::SelectRow()
  {
    //QModelIndex index = currentIndex();
    //selectRow( index.row() );
    selectionModel()->select( currentIndex(),
                QItemSelectionModel::Rows|QItemSelectionModel::Select );
  }

  void TObjectTable::SelectColumn()
  {
   // QModelIndex index = currentIndex();
   // selectColumn( index.column() );
    selectionModel()->select( currentIndex(),
                QItemSelectionModel::Columns|QItemSelectionModel::Select );
  }

  void TObjectTable::SelectAll()
  {
	  selectAll();
  }

  void TObjectTable::SelectObject()
  {
    QModelIndex index = currentIndex();
    QItemSelectionModel* selmodel = selectionModel();
    Selection sel(0, index.model()->rowCount( index )-1, 0, index.model()->columnCount( index )-1 );
    int iN, iM;
    FieldInfo fld =  ((TObjectModel *)(index.model() ))->getInfo( 
    		 index.row(), index.column(), iN, iM, &sel );
    if(iN == -1 || iM == -1  )
 	   	return;
    
    QModelIndex topLeft = index.sibling( sel.N1, sel.M1 );
    QModelIndex bottomRight = index.sibling( sel.N2, sel.M2 );
    
    selmodel->select( QItemSelection( topLeft, bottomRight ), 
    		QItemSelectionModel::SelectCurrent );
  }

  void TObjectTable::CutData()
  {
	  CopyData();
	  ClearData();
  }

  void TObjectTable::ClearData()
  {
 	 TObjectModel *  model = ((TObjectModel *)(currentIndex().model() ));
     foreach( QModelIndex ndx,  selectedIndexes()  )
           model->setData(ndx, emptiness.c_str(),  Qt::EditRole);
     // update();
 }

 void TObjectTable::CopyData()
 {
	Selection sel = getSelectionRange();
	QString clipText = createString( sel );
    QApplication::clipboard()->setText(clipText/*, QClipboard::Clipboard*/);
 }

 void TObjectTable::CopyDataHeader()
 {
	Selection sel = getSelectionRange();
	QString clipText = createHeader();
	clipText += createString( sel );
    QApplication::clipboard()->setText(clipText/*, QClipboard::Clipboard*/);
 }
 
 void TObjectTable::PasteData()
 {
     Selection sel = getSelectionRange( true );
    pasteIntoArea( sel, false);
 }

 void TObjectTable::PasteTransposedData()
 {
    Selection sel = getSelectionRange( true );
	pasteIntoArea( sel, true);
 }
 
 //------------------------------------------------------------------------ 
  QString TObjectTable::createString( Selection& sel )
  {
      int ii, jj;
	  QModelIndex index = currentIndex();
	  QString cText;
	  QString clipText;
	  
	  for(  ii=sel.N1; ii<=sel.N2; ii++ )
	  {
		if( ii > sel.N1 )
          clipText += splitRow;
  
		for( jj=sel.M1; jj<=sel.M2; jj++ )
		{
		  QModelIndex wIndex = 	index.sibling( ii, jj );
		  // selected all region if( selmodel->isSelected( wIndex ) )
		  if( jj > sel.M1 )
            clipText += splitCol;
		  cText = wIndex.data(Qt::EditRole).toString();
		  if( cText == emptiness.c_str() )
			  cText = "  ";//"\r"; 
	     clipText += cText;
		}
	 }
	return clipText;  
  }

  QString TObjectTable::createHeader()
  {
    QString cText;
	QString clipText;
    int col;
    bool frst = true;
    for( col = 0; col < model()->columnCount( rootIndex() ); col++ ) 
	{
	    if( selectionModel()->columnIntersectsSelection( col,  rootIndex() ) )
       	{
   		  if( !frst )
            clipText += splitCol;
   		  frst = false;
   		  cText = model()->headerData( col, Qt::Horizontal, Qt::DisplayRole ).toString();
   		  if( cText == emptiness.c_str() )
   			  cText = "  ";//"\r"; 
   	      clipText += cText;
	    }
	 }  
     if( !frst )
        clipText += splitRow;
	return clipText;  
  }

  Selection TObjectTable::getSelectionRange( bool paste_ )
  {
     QModelIndex index = currentIndex();
 	// TObjectModel *  model = ((TObjectModel *)(index.model() ));

	  if( !paste_ && !selectionModel()->hasSelection()  )
		return Selection( index.row(), index.row(), index.column(), index.column() );

 	 int N1=-1, N2=0, M1=-1, M2=0; 
 	 foreach( QModelIndex ndx,  selectedIndexes()  )
 	 {
 	   if( N1 == -1 || M1 == -1)
 	   {  
 		  N1 = ndx.row();
 	      M1 = ndx.column();
 	   } 
 	   if( N1 > ndx.row() ) N1 = ndx.row();	 
 	   if( N2 < ndx.row() ) N2 = ndx.row();	 
 	   if( M1 > ndx.column() ) M1 = ndx.column();	 
 	   if( M2 < ndx.column() ) M2 = ndx.column();	 
  	 };
  
 	 if( paste_ && ( !selectionModel()->hasSelection() ||
            ( N1==N2 && M1==M2 ) ) ) // only one selected => all for end of table
             return Selection( index.row(), index.model()->rowCount( index )-1,
            index.column(), index.model()->columnCount( index )-1 );

  	 return Selection( N1, N2, M1, M2 );
  }

  void  TObjectTable::setFromString(char splitrow, const QString& str,
          Selection sel, bool transpose) throw(TError)
  {
     TObjectModel *  model = ((TObjectModel *)(currentIndex().model() ));
     if( str.isEmpty() )
  	    return;
  	
     QModelIndex wIndex;
     const QStringList rows = str.split(splitrow, QString::KeepEmptyParts);

     int ii, jj;
     int rowNum = sel.N1;
     const int nLimit = (transpose) ? (sel.N1 + sel.M2-sel.M1) : sel.N2;

        for( int it = 0; it < rows.count() && rowNum <= nLimit; it++, rowNum++)
  	{
  	    //if( rows[it].isEmpty() ) sd 29/10/2008 
  		// continue;

  	    const QStringList cells = rows[it].split('\t', QString::KeepEmptyParts);
  	    int cellNum = sel.M1;
  	    const int mLimit = (transpose) ? (sel.M1 + sel.N2-sel.N1) : sel.M2;
  	    for( int cellIt = 0;  cellIt < cells.count() && cellNum <= mLimit; cellIt++, cellNum++) 
  	    {
  		  gstring value = (const char*)cells[ cellIt ].toLatin1().data();
                  value.strip();
  		  if( value.empty() || value == emptiness )
  		    value = S_EMPTY;
    	  
  		   if( transpose ) 
  		   { ii = (cellNum-sel.M1)+sel.N1;
                     jj = (rowNum-sel.N1)+sel.M1;
  		   }
  		   else
  		   { ii = rowNum;
                     jj = cellNum;
  		   }

                   wIndex = 	currentIndex().sibling( ii, jj );
                   model->setData(wIndex, QString(value.c_str()), Qt::EditRole);
  		}
  	}
 }

  void TObjectTable::pasteIntoArea( Selection& sel, bool transpose)
  {
      QString clipboard = QApplication::clipboard()->text(QClipboard::Clipboard);
      char splitrow = splitRow;

      int lastCR = clipboard.lastIndexOf(splitrow);
      if( lastCR < 0 )
      {
#ifdef __APPLE__
       splitrow = '\n';
#else
      splitrow = '\r';
#endif
         lastCR = clipboard.lastIndexOf(splitrow);
      }
      if( lastCR == clipboard.length() - 1 )
  	  clipboard.remove(lastCR, 1);
      QString undoString;

      try 
      {
         const QStringList rows = clipboard.split(splitrow);
         const int clipN = rows.count();
         const bool largerN = transpose ?
           (clipN > (sel.M2 - sel.M1 + 1)) : (clipN > (sel.N2 - sel.N1 +1 ));
      
         int rowNum = sel.N1;
     	 bool largerM = false;
         for(int it = 0; it < rows.count(); it++, rowNum++)
  	     {
           int clipM = rows[it].count(splitCol) + 1;
  	       largerM = transpose ? (clipM > (sel.N2 - sel.N1 + 1 )) : (clipM > (sel.M2 - sel.M1 + 1));
  	       if( largerM )
  		      break;
  	     }
         if( largerN || largerM )
         {
  	      if( vfQuestion3(topLevelWidget(), "Object paste", 
  		    "Pasting contents has larger dimensions then the object!", 
  		    "Discard extra cells", "Cancel", "") != VF3_1 )
  		  return;
         }
         undoString = createString( sel );
      
         setFromString(splitrow, clipboard, sel, transpose);
         // update();
         // groupUndoContents = undoString;	// for possible group undo
    }
    catch(TError& ex) 
    {
  	  vfMessage(topLevelWidget(), "Object paste error", ex.mess , vfErr);
  	  if( !undoString.isEmpty() )
            try { setFromString( splitRow, undoString, sel, false); }
                  catch(...) {}
  	    
    }
  }


 //-------------------------------------------------------------------------------------
 // TObjectDelegate -  individuak items in views are rendered and edited using delegates
 //-------------------------------------------------------------------------------------

 TObjectDelegate::TObjectDelegate( TObjectTable *aTabl, QObject * parent ):
        QItemDelegate( parent ), tabl(aTabl)
 {
 }


 bool TObjectDelegate::eventFilter(QObject *object, QEvent *event) 
 {
    if (event->type() == QEvent::KeyPress)
     {
        QTextEdit *editor = qobject_cast<QTextEdit *>( object);
        if( editor )
        {
          switch (static_cast<const QKeyEvent*>(event)->key())
           {
            case Qt::Key_F2:   // exit from text editor
              QMetaObject::invokeMethod(this, "_q_commitDataAndCloseEditor",
                 Qt::QueuedConnection, Q_ARG(QWidget*, editor));
              return false;
            case Qt::Key_Enter:  // not exit
               return false;
            default:
                break;
            }
         }
     }
    return QItemDelegate::eventFilter(object, event);
 }

 /*
 void TObjectDelegate::commitAndCloseLineEditor()
 {
 	TCellInput *editor = static_cast<TCellInput *>( sender());
     emit commitData( editor );
     emit closeEditor( editor );
 }

 void TObjectDelegate::commitAndCloseCheckEditor()
 {
     QComboBox *editor = qobject_cast<QComboBox *>( sender());
 	emit commitData( editor );
     emit closeEditor( editor );
 }

 void TObjectDelegate::commitAndCloseTextEditor()
 {
     QTextEdit *editor = qobject_cast<QTextEdit *>( sender());
 	emit commitData( editor );
     emit closeEditor( editor );
 }
 */
/*22/04/2010
void TObjectDelegate::updateEditorGeometry( QWidget * editor,
     const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    QItemDelegate::updateEditorGeometry( editor, option,index);
 // type 1
 //   int wdf, htf;
 //   if( tabl->isText())
 //   { tabl->getObjectSize( wdf, htf );
 //     editor->resize( htf, wdf);
 //   }
 // type 2
  int iN, iM;
  FieldInfo fld =  ((TObjectModel *)(index.model() ))->getInfo( index.row(), index.column(), iN, iM);
  if(fld.fType == ftText )
     editor->resize( wdF(fld.fType, fld.npos, fld.edit)+4, htF(fld.fType, fld.maxN )+4 );
}
*/
 // Editing QTableView for objects in TCPage
 QWidget *TObjectDelegate::createEditor(QWidget *parent,
         const QStyleOptionViewItem &option,
         const QModelIndex &index) const
 {
 	int iN, iM; 
 	
 	//const TObjectModel * mod = static_cast<TObjectModel *>(index.internalPointer() ); 
 	FieldInfo fld =  ((TObjectModel *)(index.model() ))->getInfo( index.row(), index.column(), iN, iM);
     if( iN >= 0 && iM >= 0)
     {
     	switch( fld.fType )
         {
           case ftCheckBox:
            { TCellCheck* editor =  new TCellCheck( fld, iN, iM, parent);
 //             connect( editor, SLOT(editingFinished()), this, SLOT(commitAndCloseCheckEditor()));
              return editor;
            }  
          case ftText:
          {	TCellText* editor =  new TCellText( fld, iN, iM, parent);
 //            connect( editor, SLOT(editingFinished()), this, SLOT(commitAndCloseTextEditor()));
             return editor;
          }   
         default:
         	{ TCellInput* editor =  new TCellInput( fld, iN, iM, parent);
 //              connect( (QLineEdit *)editor, SLOT(editingFinished()), this, SLOT(commitAndCloseLineEditor()));
               return editor;
         	}  
         }
     }	
    return QAbstractItemDelegate::createEditor( parent, option,  index );
 }

 void TObjectDelegate::setEditorData(QWidget *editor,
                                   const QModelIndex &index) const
 {
 	TCell *cellEdit = dynamic_cast<TCell*>(editor);
 	if( cellEdit)
 	{   
 	    cellEdit->setData( index.data(Qt::EditRole).toString());
 	}    
 }

 void TObjectDelegate::setModelData(QWidget *editor,
                                  QAbstractItemModel *model,
                                  const QModelIndex &index) const
 {
 	TCell *cellEdit = dynamic_cast<TCell*>(editor);
 	if( cellEdit)
 	{	
          if( cellEdit->dataCh() )
              model->setData(index, cellEdit->getData(),  Qt::EditRole);
 	}   
  }

 
//------------------------------------------------
// TCell
//------------------------------------------------
TCell::TCell(FieldInfo afld, int in, int im):
    fld(afld), 
    iN(in), iM(im)
{
   edited = false;
   th = 0;
}

// Help on F1 pressed on data field
void TCell::CmHelp()
{
  if(th)
  {	TObjectTable *topw = qobject_cast<TObjectTable *>( th->parentWidget());
         if( topw )
            topw->CmHelp();
  }
}

//----------------------------------------------
// TIntValidator,
//----------------------------------------------

class TIntValidator : public QIntValidator
{
   // Q_OBJECT

public:
    TIntValidator(QObject * parent):QIntValidator( parent){};
    ~TIntValidator(){};

    QValidator::State validate(QString &, int &) const;

};

QValidator::State TIntValidator::validate(QString &input, int &pos) const
{
  if (input == "--" )
        return Intermediate;
  if (input == "---" )
        return Acceptable;

 return QIntValidator::validate(input, pos);
}

class TDoubleValidator : public QDoubleValidator
{
   // Q_OBJECT

public:
    TDoubleValidator(QObject * parent):QDoubleValidator( parent){};
    ~TDoubleValidator(){};

    QValidator::State validate(QString &, int &) const;

};

QValidator::State TDoubleValidator::validate(QString &input, int &pos) const
{
  if (input == "--" )
        return Intermediate;
  if (input == "---" )
        return Acceptable;

 return QDoubleValidator::validate(input, pos);
}

//------------------------------------------------
// TCellInput
//------------------------------------------------

TCellInput::TCellInput( FieldInfo afld, int in, int im, QWidget * parent ):
    QLineEdit( parent ),
    TCell( afld, in, im)
                
{
    th = this;

    switch( fld.fType )
    {
    case ftNumeric:
        setValidator( new TIntValidator( this ) );
        break;
    case ftFloat:
    	{ 
                TDoubleValidator* dVal = new TDoubleValidator( this );
    	  	dVal->setDecimals( afld.npos-2 );
    	    setValidator( dVal );
            break;
    	}    
    default:
        ;
    }
    if(fld.nO >= 0)
      this->setToolTip(fld.pObj->GetDescription(iN,iM).c_str() );

    connect( this, SIGNAL(textEdited( const QString& )),
            this, SLOT(setCh()));
}

void
TCellInput::keyPressEvent(QKeyEvent* e)
{
    switch( e->key() )
    {
       case Qt::Key_F1:
              CmHelp();
              return;
    }
    QLineEdit::keyPressEvent(e);
}

//==========================================
// TCellCheck
//==========================================

TCellCheck::TCellCheck( FieldInfo afld, int in, int im, QWidget * parent ):
    QComboBox( parent ),
    TCell( afld, in, im),
    Vals(aUnits[afld.npos].getVals(im))            
{
    th = this;
    
    for(uint ii=0; ii<Vals.length(); ii++)
    {
        gstring s(Vals, ii, 1);
        addItem ( s.c_str() );
    }
    resize( size().width()*4, size().height() );
    setEditable( false );
    if(fld.nO >= 0)
     this->setToolTip(fld.pObj->GetDescription(iN,iM).c_str() );
    // connect( this, SIGNAL(currentIndexChanged(const QString&)),
   //         this, SLOT(setChanged()));

}

void TCellCheck::keyPressEvent(QKeyEvent* e)
{
    switch( e->key() )
    {
    case Qt::Key_F1:
        CmHelp();
        return;
    }
    QComboBox::keyPressEvent(e);
}

//------------------------------------------------
// TCellText
//------------------------------------------------
TCellText::TCellText(FieldInfo afld, int in, int im, QWidget * parent):
        QTextEdit( parent ),
        TCell( afld, in, im)
{
    setLineWrapMode( QTextEdit::NoWrap );
    if(fld.nO >= 0)
    this->setToolTip(fld.pObj->GetDescription(iN,iM).c_str() );
    // signal textChanged () ???? => edited
    connect( this, SIGNAL(textChanged()), this, SLOT(setCh()));
}

void TCellText::EvChange()
{
    if( toPlainText().length() >= 32000+1+1 ) 
    {
        vfMessage(topLevelWidget(), "Text object", "String is too long for this object", vfErr);
	    undo();
    }
}

bool TCellText::eventFilter(QObject *obj, QEvent *ev)
{
  switch (ev->type())
  {
        case QEvent::KeyPress:
        case QEvent::KeyRelease:
        {
           switch (static_cast<const QKeyEvent*>(ev)->key())
           {
            case Qt::Key_Escape:
            //case Qt::Key_Enter:
            //case Qt::Key_Return:
                ev->accept();
                return false;
            default:
                break;
            }
        }
        break;
        default:
        break;
    }
    return QTextEdit::eventFilter(obj, ev);
}

void TCellText::keyPressEvent(QKeyEvent* e)
{
    switch( e->key() )
    {
    case Qt::Key_F1:
        CmHelp();
        return;
    }
    QTextEdit::keyPressEvent(e);
}

//---------------------- End of file  model_w.cpp ---------------------------
