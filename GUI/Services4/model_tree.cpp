//-------------------------------------------------------------------
// $Id: model_w.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of Tof TTreeModel, TTreeDelegate and TTreeView
//
// Copyright (C) 2009 S.Dmytriyeva
//
// This file is part of the GEM-Selektor GUI library which uses the
// Qt v.4 cross-platform App & UI framework (http://qt.nokia.com)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)  
//
// This file may be distributed under the GPL v.3 license

//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#include <QtGui>
#include <QHeaderView>

#include "model_tree.h"
#include "units.h"
#include "m_param.h"
#include "CalcDialog.h"
#include "CalcCheckDialog.h"
#include "GemsMainWindow.h"

TreeLine::TreeLine(int aiPh, int aiDC, int aiDCPh):
  iPh( aiPh), iDC( aiDC), iDC_Ph( aiDCPh), UGval('J')
{
    parent = 0;
}

TreeLine::~TreeLine()
{
    qDeleteAll(children);
}

//--------------------------------------------------------------------------------------
//  class TTreeModel
//  class for represents the data set and is responsible for fetchin
//  the data is neaded for viewing and for writing back any changes.
//  Reading/writing data from/to TObject and TObjList classes
//---------------------------------------------------------------------------------------
TTreeModel::TTreeModel( const QList<FieldInfo> aFldsPh, 
		                const QList<FieldInfo> aFldsDC,
		                const QStringList& aHdData,
		                QObject * parent ):
		QAbstractItemModel(parent), hdData( aHdData) 	  
{
  fldsPh = aFldsPh;
  fldsDC = aFldsDC;
  rootNode = 0;
  setupModelData();
  
  connect( this, SIGNAL(dataChanged( const QModelIndex&, const QModelIndex& )),
  			parent,  SLOT(objectChanged()) );	 
  }

TTreeModel::~TTreeModel()
{
  if(rootNode )
	 delete rootNode;
}

TreeLine *TTreeModel::lineFromIndex(const QModelIndex &index) const
{
    if (index.isValid()) {
        return static_cast<TreeLine *>(index.internalPointer());
    } else {
        return rootNode;
    }
}


void TTreeModel::setupModelData()
{
    if(rootNode )
	  delete rootNode;
    
    rootNode = new TreeLine(-1, -1 , -1 );
    
    int nO = fldsPh[1].nO;
    
    long int *l1_R = 0;
    short* l1_I = 0;
       
    if( nO ==  o_wi_l1 )
        l1_R = (long int *)aObj[ o_wi_l1 ].GetPtr();
    else 	
        l1_I = (short *)aObj[ nO/*o_mul1*/ ].GetPtr();
    
    int nPhases = aObj[ nO/*o_mul1*/ ].GetN();
    int ii, jj, je, jb;
    je = 0;
    
    for(  ii=0; ii<nPhases; ii++)
    {
    	TreeLine *prnt = new TreeLine(ii, -1 , -1 );
    	prnt->parent = rootNode;
    	rootNode->children.append( prnt ); 
        jb = je;
        if( nO ==  o_wi_l1 )
            je += l1_R[ii];
        else 	
            je += l1_I[ii];

      for( jj=jb; jj<je; jj++)
      {  
        TreeLine *chld = new TreeLine(ii, jj , jj-jb );
        chld->parent = prnt;
        prnt->children.append(chld);
      }
   }
#if QT_VERSION >= 0x050000

    beginResetModel();
    endResetModel();

#else
    reset();

#endif
}

QModelIndex TTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!rootNode)
        return QModelIndex();
    TreeLine *parentItem = lineFromIndex( parent );
    return createIndex(row, column, parentItem->children[row]);
}

QModelIndex TTreeModel::parent(const QModelIndex& child) const
{
    if (!child.isValid())
        return QModelIndex();
 
    TreeLine *childItem = lineFromIndex(child);
    TreeLine *parentItem = childItem->parent;
    if (parentItem == rootNode )
        return QModelIndex();
    return createIndex(parentItem->iPh, 0, parentItem);
}

int TTreeModel::rowCount( const QModelIndex& parent ) const
{
   if (!rootNode)
       return 0;
  if (parent.column() > 0)
      return 0;
  TreeLine *parentItem = lineFromIndex( parent );
  return parentItem->children.count();
}	

int TTreeModel::columnCount( const QModelIndex&  ) const
{
  return fldsPh.count();
}	

Qt::ItemFlags TTreeModel::flags( const QModelIndex& index ) const  
{
  Qt::ItemFlags flags = QAbstractItemModel::flags(index);
  int nO, iN, iM;
  FieldInfo fld = getInfo( index, iN, iM );
  nO = fld.nO;
  if( (fld.edit == eYes ) ) 
	  if( nO == -1 || aObj[nO].GetPtr() !=0 )
          {  flags |= Qt::ItemIsEditable;
             return flags;
          }
  return (flags & ~Qt::ItemIsEditable);
}

QVariant TTreeModel::headerData( int section, Qt::Orientation orientation, int role ) const 
{
 if( role == Qt::DisplayRole  && orientation == Qt::Horizontal )
	   return hdData[section];
  return QVariant();
}

void TTreeModel::setGOcorr(const QModelIndex& index, QString& value, int nO, int iN )
{
	char code = lineFromIndex( index )->UGval;
    double dat = value.toDouble();
        double tK = TMulti::sm->GetPM()->T; //TProfil::pm->tpp->TK;
	  
	switch( code )
	{
	     case 'J': //dat *=1.; J/mol
	           break;   
	     case 'k': dat *=1000.; // kJ/mol
	           break;   
	     case 'p': dat *=lg_to_ln*R_CONSTANT*tK; // pK
	           break;   
	     case 'l': dat *=(-1)*lg_to_ln*R_CONSTANT*tK; // logK
	           break;   
	     case 'n': dat *=(-1)*R_CONSTANT*tK; // lnK
	           break;   
	     case 'm': dat *=R_CONSTANT*tK; // mol/mol
	           break;   
	  }
	QString res = QString("%1").arg(dat);	
	gstring txt = res.toLatin1().data();
	aObj[nO].SetString( txt.c_str(), iN, 0 );
}

QString TTreeModel::getGOcorr( const QModelIndex& index, int nO, int iN ) const
{
    QString res =  QString::fromLatin1( visualizeEmpty(aObj[nO].GetStringEmpty( iN, 0 )).c_str() );
	char code = lineFromIndex(index)->UGval;
	double dat = res.toDouble();
        double tK = TMulti::sm->GetPM()->T; //TProfil::pm->tpp->TK;
	  
	switch( code )
	{
	     case 'J': //dat /=1.; J/mol
	           break;   
	     case 'k': dat /=1000.; // kJ/mol
	           break;   
	     case 'p': dat /=lg_to_ln*R_CONSTANT*tK; // pK
	           break;   
	     case 'l': dat /=(-1)*lg_to_ln*R_CONSTANT*tK; // logK
	           break;   
	     case 'n': dat /=(-1)*R_CONSTANT*tK; // lnK
	           break;   
	     case 'm': dat /=R_CONSTANT*tK; // mol/mol
	           break;   
	  }
	return QString("%1").arg(dat);	
}

QString TTreeModel::getObjValue(  int nO, int iN ) const
{
    QString res =  QString::fromLatin1( visualizeEmpty(aObj[nO].GetStringEmpty( iN, 0 )).c_str() );

    if( res == emptiness.c_str() )
    	return  res;

    double vl;
	switch( nO )
	{
// Input
	case o_musf:
	case o_musm:
	     res = res.remove(0, 12);
		 res = res.left(16);	
         break; 		
// Result	
	case o_wo_wx:
		  switch( aObj[o_wi_dcc].GetStringEmpty(iN)[0])
		  {
		    case DC_SCP_CONDEN:
		    case DC_AQ_SOLVENT:  /* mol fractions in solvent */
		    case DC_AQ_SOLVCOM:
		    case DC_GAS_COMP:
		    case DC_GAS_H2O:
		    case DC_GAS_CO2:     /* gases */
		    case DC_GAS_H2:      /* volume */
		    case DC_GAS_N2:
		    case DC_SOL_IDEAL:
		    case DC_SOL_MINOR:   /* volume */
		    case DC_SOL_MAJOR:
                    case DC_SOL_MINDEP:
                    case DC_SOL_MAJDEP:
		    case DC_PEL_CARRIER:
		    case DC_SUR_MINAL:
		    case DC_SUR_CARRIER: /* sorbent */
		          //  res = res; // o_wo_wx
		         break;
		    default: // molality
                res =  QString::fromLatin1( visualizeEmpty(aObj[o_wd_ym].GetStringEmpty( iN, 0 )).c_str() );
		         break; /* error in DC class code */
		 }
	    break;
	case o_wd_yla:
	    vl = res.toDouble();
	    if( vl > -150. && vl < 150. )
	      vl = exp( vl/0.434294481 );
	    else vl = 0.;
	    res = QString("%1").arg(vl);
	    break;
	case o_wo_lngam:
	    vl = res.toDouble();
	    if( vl > -200. && vl < 200. )
	      vl = exp( vl );
	    else vl = 0.;
	    res = QString("%1").arg(vl);
		break;
        case o_wo_falp:
            vl = res.toDouble();
            res = QString("%1").arg(vl, 0, 'g', 4);
                break;
    default:
    	break;
	}
	return res;
 }

QString TTreeModel::getDescription( int nO, int N) const
{
    if( nO == -1 )
      return "";
    
    TObject& pObj = aObj[nO]; 
	gstring desc = pObj.GetDescription(N,0);
	const char *keyWd = pObj.GetKeywd();
	switch( nO )
	{
// Result	
	case o_wo_wx:
		  switch( aObj[o_wi_dcc].GetStringEmpty(N)[0])
		  {
		    case DC_SCP_CONDEN:
		    case DC_AQ_SOLVENT:  /* mol fractions in solvent */
		    case DC_AQ_SOLVCOM:
		    case DC_GAS_COMP:
		    case DC_GAS_H2O:
		    case DC_GAS_CO2:     /* gases */
		    case DC_GAS_H2:      /* volume */
		    case DC_GAS_N2:
		    case DC_SOL_IDEAL:
		    case DC_SOL_MINOR:   /* volume */
		    case DC_SOL_MAJOR:
		    case DC_PEL_CARRIER:
		    case DC_SUR_MINAL:
		    case DC_SUR_CARRIER: /* sorbent */
		          //  res = res; // o_wo_wx
		         break;
		    default: // molality
		        // pObj = aObj[o_wd_ym]; 
		    	desc = aObj[o_wd_ym].GetDescription(N,0);
		    	keyWd = aObj[o_wd_ym].GetKeywd();
		    	break; /* error in DC class code */
		 }
	    break;
	case o_wd_yla:
    	desc = "General activity (fugacity) of Dependent Components at equilibrium state ";
    	keyWd = "";
	    break;
	case o_wo_lngam:
	    desc = "Calculated general (rational) DC activity coefficients in all phases ";
    	keyWd = "";
		break;
    default:
    	break;
	}
	QString s;
    if( pObj.GetNS()!=1  )
        s = QString("%1 [%2] : %4").arg( keyWd, 
        		QVariant(N).toString(),	 desc.c_str() );
    else
        s = QString("%1 : %2").arg( keyWd, desc.c_str() );

    return s;
}

QVariant TTreeModel::data( const QModelIndex& index, int role ) const
{
	int nO, iN, iM;
	
	if(!index.isValid())
	 return QVariant();	

   FieldInfo fld = getInfo( index, iN, iM );
   nO = fld.nO;

   switch( role )
   { case Qt::DisplayRole:
     case Qt::EditRole:
            {  QString res = "";
               if( nO >= 0)
               {  if( index.column()== 7 )
                     res = getGOcorr( index, nO, iN );
                  else
                     res = getObjValue( /*index,*/ nO, iN );
               }
               else if( index.column()== 6 && fldsPh.count() > 7 ) // only Input Window
                       res = QString( lineFromIndex(index)->UGval);
               return  res;
             }
      case Qt::ToolTipRole:
      case Qt::StatusTipRole:
              if( nO >= 0 )
                  return  getDescription(nO, iN );
              else if( index.column()== 6 && fldsPh.count() > 7 ) // only Input Window
                       return  QString("J value change");
              break;
      /*case Qt::TextAlignmentRole:
               if( fld.fType == ftNumeric || fld.fType == ftFloat )
                       return int(Qt::AlignRight | Qt::AlignVCenter);
               else return int(Qt::AlignLeft | Qt::AlignVCenter);*/
      default: break;
        }
/*
   if( role == Qt::DisplayRole  || role == Qt::EditRole )
   {
	  QString res = "";
	  if( nO >= 0)
	  {  if( index.column()== 7 )
		  res = getGOcorr( index, nO, iN );
	     else
          res = getObjValue( *index*, nO, iN );
	  }
	  else if( index.column()== 6 && fldsPh.count() > 7 ) // only Input Window
		       res = QString( lineFromIndex(index)->UGval);	
	  return  res;
	  
	} else
		if( role == Qt::ToolTipRole || role == Qt::StatusTipRole )
		{
	      if( nO >= 0 )
	    	  return  getDescription(nO, iN );
	      else if( index.column()== 6 && fldsPh.count() > 7 ) // only Input Window
	    	       return  QString("J value change");
                }*/

   return QVariant();
}

bool TTreeModel::setData( const QModelIndex& index, const QVariant& value, int role)
{
	int nO, iN, iM;

	if( index.isValid() && ( role == Qt::EditRole ) )
	{
	   FieldInfo fld = getInfo( index, iN, iM );
	   nO = fld.nO;
	   
           if( nO >= 0 )
	   {
             QString val = QVariant(value).toString();
             gstring txt = val.toLatin1().data();
	
            if( index.column()== 3 ) // + or - (switch all dcomps with phase)
             {   char old_data = aObj[nO].GetStringEmpty( iN, iM )[0];
                 childChanged( index, value, old_data);
             }

            if( txt == emptiness /*|| txt == short_emptiness*/ )
	      aObj[nO].SetString( S_EMPTY, iN, iM );
             else
             {   if( index.column()== 7 )
	 	   		setGOcorr( index, val, nO, iN );
                 else
	            aObj[nO].SetString( txt.c_str(), iN, iM );
             }

            emit dataChanged(index, index); // set flag the system changed
	   }	  
	    else
		  if( index.column()== 6 )
		  {	 
			 lineFromIndex(index)->UGval = QVariant(value).toString().toLatin1()[0];
		  }	 
	 return true;
	} 
	return false;
}

void TTreeModel::childChanged(const QModelIndex& index, const QVariant& value, char old_data)
{
    if( index.column()== 3 ) // + or - (switch all dcomps with phase)
    {
        gstring txt = QVariant(value).toString().toLatin1().data();

       if( txt[0] == '-' ||
               (( txt[0] == '+' || txt[0] == '*' ) && old_data == '-' ))
        {
            TreeLine *parentItem = lineFromIndex( index );
            for (int  rw = 0; rw < parentItem->children.count(); rw++ )
               setData( createIndex(rw, index.column(), parentItem->children[rw] ), value, Qt::EditRole);
        }
    }
}


//-------------------------------------------------------------------------------------
// TTreeDelegate -  individuak items in views are rendered and edited using delegates
//-------------------------------------------------------------------------------------

TTreeDelegate::TTreeDelegate( QObject * parent ):
	QItemDelegate( parent )
{
}

// Editing QTreeView for objects in System page
QWidget *TTreeDelegate::createEditor(QWidget *parent,
        const QStyleOptionViewItem &option,
        const QModelIndex &index) const
{
	int iN, iM; 
	
	FieldInfo fld =  ((TTreeModel *)(index.model() ))->getInfo( index, iN, iM);
    if( iN >= 0 && iM >= 0)
    {
    	switch( fld.fType )
        {
          case ftCheckBox:
           { TCellCheck* editor =  new TCellCheck( fld, iN, iM, parent);
             return editor;
           }  
        default:
        	{ TCellInput* editor =  new TCellInput( fld, iN, iM, parent);
              return editor;
        	}  
        }
    }	
   return QAbstractItemDelegate::createEditor( parent, option,  index );
}

void TTreeDelegate::setEditorData(QWidget *editor,
                                  const QModelIndex &index) const
{
	TCell *cellEdit = dynamic_cast<TCell*>(editor);
	if( cellEdit)
	{   
	    cellEdit->setData( index.data(Qt::EditRole).toString());
	}    
}

void TTreeDelegate::setModelData(QWidget *editor,
                                 QAbstractItemModel *model,
                                 const QModelIndex &index) const
{
	TCell *cellEdit = dynamic_cast<TCell*>(editor);
	if( cellEdit)
	{	
 	  model->setData(index, cellEdit->getData(),  Qt::EditRole);
	}   
 }

//-------------------------------------------------------------------------------------
// class TTreeView implements a tree view that displays items from a model.
//-------------------------------------------------------------------------------------

TTreeView::TTreeView( QWidget * parent ):
	 QTreeView( parent )
{
	    setFont( pVisorImp->getCellFont() );
	//    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
#if QT_VERSION >= 0x050000
    header()->setSectionsClickable(true);
    header()->setSectionResizeMode( /*QHeaderView::ResizeToContents*/QHeaderView::Interactive );

#else
     header()->setResizeMode( /*QHeaderView::ResizeToContents*/QHeaderView::Interactive );

#endif
	    setEditTriggers( QAbstractItemView::DoubleClicked|QAbstractItemView::AnyKeyPressed );
	    setFocusPolicy(Qt::StrongFocus);
	    setTabKeyNavigation( false );
        setContextMenuPolicy(Qt::CustomContextMenu);
        setSelectionMode( QAbstractItemView::/*MultiSelection*/ExtendedSelection );
        setSelectionBehavior( QAbstractItemView::SelectItems );
        setIndentation( 35 );

       connect( this, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(slotPopupContextMenu(QPoint)));
       connect( header(), SIGNAL(sectionClicked(int)),
            this, SLOT(changeCurrent(int)));
}

void TTreeView::changeCurrent( int section )
{
    QModelIndex index = model()->index( 0, section, rootIndex());
    setCurrentIndex(index);
}

//Printing all items to txt file
void TTreeView::printList( fstream& ff )
 {
	 int row, rw, col;
	 QModelIndex index, childIndex;
     for( row = 0; row < model()->rowCount( rootIndex() ); row++ ) 
     {
       	 childIndex = model()->index( row, 0, rootIndex());
       	 for( col = 0; col < model()->columnCount( rootIndex() ); col++ ) 
         {
            index = model()->index(row, col, rootIndex() );
            QString value = model()->data(index).toString();
            ff << value.toLatin1().data() << " ";
         }  
    	 ff << endl;
    	 
         for (rw = 0; rw < model()->rowCount( childIndex ); rw++ ) 
         {
        	 for (col = 0; col < model()->columnCount( childIndex ); col++ ) 
             {
                index = model()->index(rw, col, childIndex );
                QString value = model()->data(index).toString();
                ff << value.toLatin1().data() << " ";
             }  
        	 ff << endl;
         }
     }
 }

 void TTreeView::slotPopupContextMenu(const QPoint &pos)
 {
    QModelIndex index = indexAt( pos );
    TTreeModel* model =(TTreeModel *)index.model();
 	int iN, iM;
 	FieldInfo fld =  model->getInfo( index, iN, iM);
     if(iN == -1 || iM == -1 )
     	return;
 	
     QMenu *menu = new QMenu(this);
  //   no_menu1 = false;
  
     QAction* act =  new QAction(tr("&Help"), this);
     act->setIcon(QIcon(":/menu/Icons/ShowHelpWindowIcon.png"));
     act->setShortcut(tr("F1"));
     act->setStatusTip(tr("Help for specified cell"));
     connect(act, SIGNAL(triggered()), this, SLOT(CmHelp()));
     menu->addAction(act);
          	    
     if( (fld.fType == ftFloat || fld.fType == ftNumeric || 
        		fld.fType == ftCheckBox) && fld.edit == eYes )
         {
     	    menu->addSeparator();
            act =  new QAction(tr("&Calculator"), this);
            act->setShortcut(tr("F8"));
            act->setStatusTip(tr("Use Calculator for specified cells"));
       	    connect(act, SIGNAL(triggered()), this, SLOT(CmCalc()));
            menu->addAction(act);
         }
 /*     if( fld.fType == ftRecord )
         {
           act =  new QAction(tr("&Show record"), this);
           act->setShortcut(tr("F7"));
           act->setStatusTip(tr("Go to the specified cell"));
           connect(act, SIGNAL(triggered()), this, SLOT(CmDComp()));
           menu->addAction(act);
         }	
   */      
       menu->addSeparator();
         
   	   act =  new QAction(tr("Select &row"), this);

        act->setShortcut(tr("Ctrl+R"));
        act->setStatusTip(tr("Select current row"));
   	    connect(act, SIGNAL(triggered()), this, SLOT(SelectRow()));
        menu->addAction(act);
 
        act =  new QAction(tr("Select co&lumn"), this);
        act->setShortcut(tr("Ctrl+L"));
        act->setStatusTip(tr("Select curent column"));
   	    connect(act, SIGNAL(triggered()), this, SLOT(SelectColumn()));
        menu->addAction(act);
       
        act =  new QAction(tr("Select &group"), this);
        act->setShortcut(tr("Ctrl+G"));
        act->setStatusTip(tr("Select current phase"));
   	    connect(act, SIGNAL(triggered()), this, SLOT(SelectGroup()));
        menu->addAction(act);

        act =  new QAction(tr("Select &all"), this);
        act->setShortcut(tr("Ctrl+A"));
        act->setStatusTip(tr("Select all"));
   	    connect(act, SIGNAL(triggered()), this, SLOT(SelectAll()));
        menu->addAction(act);

        menu->addSeparator();

        act =  new QAction(tr("&Copy"), this);
        act->setShortcut(tr("Ctrl+C"));
        act->setStatusTip(tr("Copy selected data"));
   	    connect(act, SIGNAL(triggered()), this, SLOT(CopyData()));
        menu->addAction(act);

        act =  new QAction(tr("Copy wit&h names"), this);
        act->setShortcut(tr("Ctrl+H"));
        act->setStatusTip(tr("Copy selected header&cells"));
  	    connect(act, SIGNAL(triggered()), this, SLOT(CopyDataHeader()));
        menu->addAction(act);

  
      menu->exec( viewport()->mapToGlobal(pos) );
      delete menu;
 }

  void TTreeView::keyPressEvent(QKeyEvent* e)
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
			  case Qt::Key_G:
			    SelectGroup();
			    return;
			  case Qt::Key_A:
			    SelectAll();
			    return;
			  case Qt::Key_C:
			    CopyData();
			    return;
			  case Qt::Key_H:
			    CopyDataHeader();
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
 	}
 	QTreeView::keyPressEvent(e);
 }
   
  void TTreeView::SelectRow()
  {
    selectionModel()->select( currentIndex(), 
    		QItemSelectionModel::Rows|QItemSelectionModel::Select ); 
  }

  void TTreeView::SelectColumn()
  {
   // selectionModel()->select( currentIndex(), 
   // 		QItemSelectionModel::Columns|QItemSelectionModel::Select ); 
   int col = currentIndex().column();
   int row, rw;
   QModelIndex index, childIndex;
   for( row = 0; row < model()->rowCount( rootIndex() ); row++ ) 
   {
       childIndex = model()->index( row, col, rootIndex());
       selectionModel()->select( childIndex, QItemSelectionModel::Select ); 
       childIndex = model()->index( row, 0, rootIndex());
       for (rw = 0; rw < model()->rowCount( childIndex ); rw++ ) 
       {
           index = model()->index(rw, col, childIndex );
           selectionModel()->select( index, QItemSelectionModel::Select ); 
	     }
   }   
  }

  void TTreeView::SelectAll()
  {
	//  selectAll();
	 int row, rw;
	 QModelIndex index, childIndex;
     for( row = 0; row < model()->rowCount( rootIndex() ); row++ ) 
     {
       	 childIndex = model()->index( row, 0, rootIndex());
         selectionModel()->select( childIndex, 
         		QItemSelectionModel::Rows|QItemSelectionModel::Select ); 
         for (rw = 0; rw < model()->rowCount( childIndex ); rw++ ) 
         {
             index = model()->index(rw, 0, childIndex );
             selectionModel()->select( index, 
             		QItemSelectionModel::Rows|QItemSelectionModel::Select ); 
	     }
     }   
  }

  void TTreeView::SelectGroup()
  {
    QModelIndex parIndex = model()->parent(currentIndex());
    for(int row = 0; row < model()->rowCount( parIndex ); row++ ) 
     {
    	QModelIndex  childIndex = model()->index( row, 0, parIndex);
        selectionModel()->select( childIndex, 
        		QItemSelectionModel::Rows|QItemSelectionModel::Select ); 
    	
     }	
 }

  QString TTreeView::createString()
  {
      QString cText;
	  QString clipText;
	  QModelIndexList selIndexes = selectedIndexes();	  

	 int row, rw, col;
	 QModelIndex index, childIndex;
     for( row = 0; row < model()->rowCount( rootIndex() ); row++ ) 
     {
       	 childIndex = model()->index( row, 0, rootIndex());
         bool frst = true;
       	 for( col = 0; col < model()->columnCount( rootIndex() ); col++ ) 
	         {
	            index = model()->index(row, col, rootIndex() );
	          	if( selIndexes.contains( index ) )
	          	{
	      		  if( !frst )
                      clipText += splitCol;
	      		  frst = false;
	      		  cText = model()->data(index).toString();
	      		  if( cText == emptiness.c_str() )
                       cText = "  ";//"\r";
                  clipText += cText;
	          	}
	         }  
          if( !frst )
               clipText += splitRow;

          for (rw = 0; rw < model()->rowCount( childIndex ); rw++ )
          {
	    	 frst = true; 
             for (col = 0; col < model()->columnCount( childIndex ); col++ ) 
	         {
	             index = model()->index(rw, col, childIndex );
		       	 if( selIndexes.contains( index ) )
		         {
		      	   if( !frst )
                      clipText += splitCol;
		      	    frst = false;
		      		cText = model()->data(index).toString();
		      		if( cText == emptiness.c_str() )
		      			  cText = "  ";//"\r"; 
		      	    clipText += cText;
		         }
          }
          if( !frst )
              clipText += splitRow;
	   }
      
     }
	return clipText;  
  }

  QString TTreeView::createHeader()
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

  void TTreeView::CopyData()
  {
 	QString clipText = createString();
    QApplication::clipboard()->setText(clipText/*, QClipboard::Clipboard*/);
  }

  void TTreeView::CopyDataHeader()
  {
 	QString clipText = createHeader();
	clipText += createString();
    QApplication::clipboard()->setText(clipText/*, QClipboard::Clipboard*/);
  }

  void TTreeView::CmCalc()
  {
    QModelIndex index = currentIndex();
    TTreeModel* model =(TTreeModel *)index.model();
 	int iN, iM, col = index.column();
	FieldInfo fld =  model->getInfo( index, iN, iM);
    if( fld.edit != eYes )
  	   	return;
    QModelIndex parIndex = model->parent(currentIndex());
    QModelIndexList selIndexes = selectedIndexes();

    if(  fld.fType == ftFloat || fld.fType == ftNumeric )
     {
       CalcDialog calc(topLevelWidget(), fld.nO );
       if( calc.exec() )
        {
    	   for(int row = 0; row < model->rowCount( parIndex ); row++ ) 
    	   {
    	     index = model->index(row, col, parIndex );
    	   	 if( selIndexes.contains( index ) )
    	   	 {
                QString res = calc.fun( index.data(Qt::EditRole).toDouble() );
                model->setData(index, res, Qt::EditRole);
    	      }	 
    	  }  	
        }
     }
  
    if(  fld.fType == ftCheckBox )
    {
      gstring Vals = aUnits[fld.npos].getVals(iM);
      CalcCheckDialog calc(topLevelWidget(), fld.nO, Vals);

      if( calc.exec() )
      {
    	  int ii = calc.fun();
       	  QString res  = gstring(Vals, ii, 1).c_str();
       	  for(int row = 0; row < model->rowCount( parIndex ); row++ ) 
     	  {
     	    index = model->index(row, col, parIndex );
     	   	 if( selIndexes.contains( index ) )
     	   	 {
     	        // double res = calc.fun( index.data(Qt::EditRole).toDouble() );
     	        model->setData(index, res, Qt::EditRole);         
     	      }	 
     	  }  	
       }
    }

  }

  // Help on F1 pressed on data field
    void TTreeView::CmHelp()
    {
      QModelIndex index = currentIndex();
      //if( !index.isValid() )
      //   index = model()->index(0, 0, rootIndex() );
      int iN, iM;
      FieldInfo fld =  ((TTreeModel *)(index.model() ))->getInfo( index, iN, iM);
      if(iN == -1 || iM == -1 || fld.nO < 0 )
        	return;
        
      //gstring item = fld.pObj->GetFullName(iN,iM); // for old indexation
      gstring item = fld.pObj->GetHelpLink(iN,iM);
      pVisorImp->OpenHelp( 0, item.c_str());
    }
  
//---------------------- End of file  model_tree.cpp ---------------------------
