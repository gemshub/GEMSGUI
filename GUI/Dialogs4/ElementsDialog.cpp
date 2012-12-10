//-------------------------------------------------------------------
// $Id: ElementsDialog.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of ElementsDialog class
//
// Copyright (C) 2001-2008  S.Dmytriyeva
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

#include <QtGui>

#include "ElementsDialog.h"
#include "SetFiltersDialog.h"
#include "service.h"
#include "visor.h"
#include "GemsMainWindow.h"
#include "m_icomp.h"
#include "m_param.h"

/*
TreeFileLine::TreeFileLine(int aRow, gstring aTag, gstring aVer, TreeFileLine* aParent)
{
    row = aRow;
    tag = aTag;
    ver = aVer;
    parent = aParent;
}

TreeFileLine::~TreeFileLine()
{
    qDeleteAll(children);
}


void TreeFileLine::printTest()
{
    cout << tag.c_str() << endl;
    for(int ii=0; ii<children.count(); ii++)
        children[ii]->printTest();

}



//--------------------------------------------------------------------------------------
//  class TTreeModel
//  class for represents the data set and is responsible for fetchin
//  the data is neaded for viewing and for writing back any changes.
//  Reading/writing data from/to TObject and TObjList classes
//---------------------------------------------------------------------------------------
FileNamesTreeModel::FileNamesTreeModel( TCStringArray aFilesData,
                        QObject * parent ):
        QStandardItemModel(parent)
{
  rootNode = 0;
  setupModelData(aFilesData);
}

FileNamesTreeModel::~FileNamesTreeModel()
{
  if(rootNode )
     delete rootNode;
}

TreeFileLine *FileNamesTreeModel::lineFromIndex(const QModelIndex &index) const
{
    if (index.isValid()) {
        return static_cast<TreeFileLine *>(index.internalPointer());
    } else {
        return rootNode;
    }
}


void FileNamesTreeModel::setupModelData(TCStringArray aFilesData)
{
    if(rootNode )
      delete rootNode;

    rootNode = new TreeFileLine(0, "default", "", 0);

    int ii, jj;
    TreeFileLine* pdb;
    TreeFileLine* pdb_child;
    gstring fname, tag, vers="";
    size_t pos1, pos2;

    for( ii=0; ii<aFilesData.GetCount(); ii++ )
    {
        fnamesData.Add( aFilesData[ii]);
        pdb = rootNode;
        fname = aFilesData[ii];

        //scip extension
        pos1 = fname.rfind(".");
        fname = fname.substr( 0, pos1+1 );

        // get version
        pos1 = fname.find(".Ver");
        if( pos1 != gstring::npos )
        {
            vers = fname.substr(pos1+1);
            fname = fname.substr(0, pos1+1 );
            pos2 = vers.find(".");
            vers = vers.substr(0, pos2);
        }
        else
            vers = "";

        // first tag name of chain
        pos1 = fname.find(".");
        pos2 = fname.find(".", pos1+1);
        while( pos2 != gstring::npos )
        {
          tag = fname.substr(pos1+1, pos2-pos1-1);
          pdb_child = 0;

          // test used tag before
          for( jj=0; jj<pdb->children.count(); jj++ )
          {
              if( tag == pdb->children[jj]->tag )
              {
                  pdb_child = pdb->children[jj];
                  break;
              }
          }
          if( !pdb_child )
          {
            pdb_child = new TreeFileLine( pdb->children.count(), tag, vers, pdb );
            pdb->children.append(pdb_child);

          }
          pos1=pos2;
          pos2 = fname.find(".", pos1+1);
          pdb = pdb_child;
        }

    }
   reset();
}



void FileNamesTreeModel::printTest()
{
    if (!rootNode)
        return;
    rootNode->printTest();
}

QModelIndex FileNamesTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!rootNode)
        return QModelIndex();
    TreeFileLine *parentItem = lineFromIndex( parent );
    return createIndex(row, column, parentItem->children[row]);
}


QModelIndex FileNamesTreeModel::parent(const QModelIndex& child) const
{
    if (!child.isValid())
        return QModelIndex();

    TreeFileLine *childItem = lineFromIndex(child);
    TreeFileLine *parentItem = childItem->parent;
    if (parentItem == rootNode )
        return QModelIndex();
    return createIndex(parentItem->row, 0, parentItem);
}

int FileNamesTreeModel::rowCount( const QModelIndex& parent ) const
{
   if (!rootNode)
       return 0;
  if (parent.column() > 0)
      return 10;
  TreeFileLine *parentItem = lineFromIndex( parent );
  return parentItem->children.count();
}

int FileNamesTreeModel::columnCount( const QModelIndex& parent ) const
{
  return 2;
}

QVariant FileNamesTreeModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
 if( role == Qt::DisplayRole  && orientation == Qt::Horizontal )
     if( section == 0 )
         return "Database Names";
     else
         return "Version";

  return QVariant();
}


QVariant FileNamesTreeModel::data( const QModelIndex& index, int role ) const
{
   if(!index.isValid())
     return QVariant();

   switch( role )
   { case Qt::DisplayRole:
     case Qt::EditRole:
             {   QString res;
                 if( index.column() == 0 )
                     res = QString( lineFromIndex(index)->tag.c_str() );
                  else
                     res = QString( lineFromIndex(index)->tag.c_str() );
                 cout << index.row() << res.toLatin1().data()<< endl;
                 return  res;
             }
      case Qt::ToolTipRole:
      case Qt::StatusTipRole:
      default: break;
   }

  return QVariant();
}

*/

//------------------------------------------------------------------------

void ElementsDialog::CmBack()
{
    stackedWidget->setCurrentIndex ( stackedWidget->currentIndex()-1 );
    resetNextButton();
    resetBackButton();
}

void ElementsDialog::CmNext()
{
    if(stackedWidget->currentIndex()==0)
     SetFiles();
    stackedWidget->setCurrentIndex ( stackedWidget->currentIndex()+1 );
    resetNextButton();
    resetBackButton();
}

void 	ElementsDialog::resetNextButton()
{
    if( stackedWidget->currentIndex() == stackedWidget->count() - 1 )
    {
        bNext->disconnect();
        bReset->show();
        connect( bNext, SIGNAL(clicked()), this, SLOT(CmOk()) );
        //pNext->setText("&Finish");
    }
    else
    {
        bNext->disconnect();
        bReset->hide();
        connect( bNext, SIGNAL(clicked()), this, SLOT(CmNext()) );
        //pNext->setText("&Next>");
    }
}

void 	ElementsDialog::resetBackButton()
{
    bBack->setEnabled( stackedWidget->currentIndex() > 0 );
}

ElementsDialog::ElementsDialog(QWidget* win, const char * prfName,
           const char* /*caption*/):
        QDialog( win ),
        prf_name ( prfName )
{
	setupUi(this);

    gstring str =
          "Configuration of a new Modeling Project  ";
         str +=  gstring(rt[RT_PARAM].FldKey(0), 0, rt[RT_PARAM].FldLen(0));;
         setWindowTitle( trUtf8(str.c_str()) );

         QObject::connect( bBack, SIGNAL(clicked()), this, SLOT(CmBack()));
         stackedWidget->setCurrentIndex (0);
         resetNextButton();
         resetBackButton();

   // build IComp list from template database
    TCIntArray aIndMT;
    TCStringArray aIC;
    TIComp* aICdata=(TIComp *)(&aMod[RT_ICOMP]);
    aICdata->GetElements( true, aIC, aIndMT );
    sf_data.ic_d.oldIComps.Clear();

    for( uint ii=0; ii<aIC.GetCount(); ii++ )
    {
     sf_data.ic_d.oldIComps.Add( aIC[ii] );
     el_data.oldIComps.Add( aIC[ii] );
     if( aIndMT[ii] == -1) // additional
     {
         aICkey2_sel.Add( aIC[ii] );
     }
     else //elements
     {
       aBtmId1_sel.Add( aIndMT[ii] );
     }
    }

// set up buttin groups
    bgElem = new QButtonGroup( bgElemBox);
    bgElem->setExclusive(false);
    bgElem->addButton(pbE_0, 0);
    bgElem->addButton(pbE_1, 1);
    bgElem->addButton(pbE_2, 2);
    bgElem->addButton(pbE_3, 3);
    bgElem->addButton(pbE_4, 4);
    bgElem->addButton(pbE_5, 5);
    bgElem->addButton(pbE_6, 6);
    bgElem->addButton(pbE_7, 7);
    bgElem->addButton(pbE_8, 8);
    bgElem->addButton(pbE_9, 9);
    bgElem->addButton(pbE_10, 10);
    bgElem->addButton(pbE_11, 11);
    bgElem->addButton(pbE_12, 12);
    bgElem->addButton(pbE_13, 13);
    bgElem->addButton(pbE_14, 14);
    bgElem->addButton(pbE_15, 15);
    bgElem->addButton(pbE_16, 16);
    bgElem->addButton(pbE_17, 17);
    bgElem->addButton(pbE_18, 18);
    bgElem->addButton(pbE_19, 19);
    bgElem->addButton(pbE_20, 20);
    bgElem->addButton(pbE_21, 21);
    bgElem->addButton(pbE_22, 22);
    bgElem->addButton(pbE_23, 23);
    bgElem->addButton(pbE_24, 24);
    bgElem->addButton(pbE_25, 25);
    bgElem->addButton(pbE_26, 26);
    bgElem->addButton(pbE_27, 27);
    bgElem->addButton(pbE_28, 28);
    bgElem->addButton(pbE_29, 29);
    bgElem->addButton(pbE_30, 30);
    bgElem->addButton(pbE_31, 31);
    bgElem->addButton(pbE_32, 32);
    bgElem->addButton(pbE_33, 33);
    bgElem->addButton(pbE_34, 34);
    bgElem->addButton(pbE_35, 35);
    bgElem->addButton(pbE_36, 36);
    bgElem->addButton(pbE_37, 37);
    bgElem->addButton(pbE_38, 38);
    bgElem->addButton(pbE_39, 39);
    bgElem->addButton(pbE_40, 40);
    bgElem->addButton(pbE_41, 41);
    bgElem->addButton(pbE_42, 42);
    bgElem->addButton(pbE_43, 43);
    bgElem->addButton(pbE_44, 44);
    bgElem->addButton(pbE_45, 45);
    bgElem->addButton(pbE_46, 46);
    bgElem->addButton(pbE_47, 47);
    bgElem->addButton(pbE_48, 48);
    bgElem->addButton(pbE_49, 49);
    bgElem->addButton(pbE_50, 50);
    bgElem->addButton(pbE_51, 51);
    bgElem->addButton(pbE_52, 52);
    bgElem->addButton(pbE_53, 53);
    bgElem->addButton(pbE_54, 54);
    bgElem->addButton(pbE_55, 55);
    bgElem->addButton(pbE_56, 56);
    bgElem->addButton(pbE_57, 57);
    bgElem->addButton(pbE_58, 58);
    bgElem->addButton(pbE_59, 59);
    bgElem->addButton(pbE_60, 60);
    bgElem->addButton(pbE_61, 61);
    bgElem->addButton(pbE_62, 62);
    bgElem->addButton(pbE_63, 63);
    bgElem->addButton(pbE_64, 64);
    bgElem->addButton(pbE_65, 65);
    bgElem->addButton(pbE_66, 66);
    bgElem->addButton(pbE_67, 67);
    bgElem->addButton(pbE_68, 68);
    bgElem->addButton(pbE_69, 69);
    bgElem->addButton(pbE_70, 70);
    bgElem->addButton(pbE_71, 71);
    bgElem->addButton(pbE_72, 72);
    bgElem->addButton(pbE_73, 73);
    bgElem->addButton(pbE_74, 74);
    bgElem->addButton(pbE_75, 75);
    bgElem->addButton(pbE_76, 76);
    bgElem->addButton(pbE_77, 77);
    bgElem->addButton(pbE_78, 78);
    bgElem->addButton(pbE_79, 79);
    bgElem->addButton(pbE_80, 80);
    bgElem->addButton(pbE_81, 81);
    bgElem->addButton(pbE_82, 82);
    bgElem->addButton(pbE_83, 83);
    bgElem->addButton(pbE_84, 84);
    bgElem->addButton(pbE_85, 85);
    bgElem->addButton(pbE_86, 86);
    bgElem->addButton(pbE_87, 87);
    bgElem->addButton(pbE_88, 88);
    bgElem->addButton(pbE_89, 89);
    bgElem->addButton(pbE_90, 90);
    bgElem->addButton(pbE_91, 91);
    bgElem->addButton(pbE_92, 92);
    bgElem->addButton(pbE_93, 93);
    bgElem->addButton(pbE_94, 94);
    bgElem->addButton(pbE_95, 95);
    bgElem->addButton(pbE_96, 96);
    bgElem->addButton(pbE_97, 97);
    bgElem->addButton(pbE_98, 98);

    bgElem->addButton(pbE_100, 100);
    bgElem->addButton(pbE_101, 101);

    bgOther = new QButtonGroup( bgOtherBox);
    bgOther->setExclusive(false);
     bgOther->addButton(pbA_1, 1);
     bgOther->addButton(pbA_2, 2);
     bgOther->addButton(pbA_3, 3);
     bgOther->addButton(pbA_4, 4);
     bgOther->addButton(pbA_5, 5);
     bgOther->addButton(pbA_6, 6);
     bgOther->addButton(pbA_7, 7);
     bgOther->addButton(pbA_8, 8);
     bgOther->addButton(pbA_9, 9);
     bgOther->addButton(pbA_10, 10);
     bgOther->addButton(pbA_11, 11);
//     bgOther->addButton(pbA_12, 12);
     bgOther->addButton(pbA_0, 0);

     EmptyData();

    // define page 1

    // set up default Open files names
    setOpenFilesAsDefault(); // <.kernel.> now, start define selNames
    // define list of files in DefaultDB (files_data)
    setFilesList();
    // define FTreeWidget with DefaultDB files names  files_data.flNames
    setTreeWidget();
    setSelectionTreeWidget();  // set up selection in wiget use selNames

    SetICompList();
    SetAqueous();

    // signals and slots connections
    connect( bHelp, SIGNAL( clicked() ), this, SLOT( CmHelp() ) );
    connect( bReset, SIGNAL( clicked() ), this, SLOT( CmSetFilters() ) );

    connect( cbAqueous, SIGNAL( clicked() ), this, SLOT( SetAqueous() ) );
    connect( cbSorption, SIGNAL( clicked() ),this, SLOT( SetSorption() ) );
}

ElementsDialog::~ElementsDialog()
{}

void ElementsDialog::setOpenFilesAsDefault()
{
  selNames.Clear();
  selNames.Add(".nagra-psi");
}

void ElementsDialog::languageChange()
{
    retranslateUi(this);
}

void ElementsDialog::CmHelp()
{
    pVisorImp->OpenHelp( GEMS_ELEMENTS_HTML, WZSTEP, stackedWidget->currentIndex()+1 );
}

void ElementsDialog::CmSetFilters()
{
    // Here to call SetFiltersDialog !!
    openFilesSelection();
    SetData();
    sf_data.ic_d.newIComps.Clear();
    for(uint jj, ii=0; ii<el_data.ICrds.GetCount(); ii++ )
    {
      for( jj=0; jj<sf_data.ic_d.oldIComps.GetCount(); jj++)
        if( !memcmp( el_data.ICrds[ii].c_str(),
            sf_data.ic_d.oldIComps[jj].c_str(), MAXICNAME+MAXSYMB ) )
         break;
      if( jj== sf_data.ic_d.oldIComps.GetCount() )
        sf_data.ic_d.newIComps.Add( el_data.ICrds[ii] );
    }

    SetFiltersDialog  dlg( this, &files_data, &sf_data, prf_name.c_str() );
    dlg.exec();

   if(  files_data.changed  )  // we changed file cnf for icomp
   {
      ResetData();
      EmptyData();
      SetICompList();
      SetAqueous();
   }
}

void ElementsDialog::CmOk()
{
       TProfil::pm->useAqPhase = el_data.flags[cbAqueous_];  // Added 16.06.03 
       TProfil::pm->useGasPhase = el_data.flags[cbGaseous_];
   accept();
}

void ElementsDialog::EmptyData()
{
 int ii;
 for( ii=0; ii<bgOther->buttons().count(); ii++ )
 {     bgOther->button(ii)->setEnabled( false );
       bgOther->button(ii)->setText( tr( "...") );
 }
 for( ii=0; ii<99/*bgElem->buttons()->count()-1*/; ii++ ) // we have not 99
      bgElem->button(ii)->setEnabled( false );

 cbAqueous->setChecked( true);
 cbGaseous->setChecked( true);
 cbFluid->setChecked( true);
 cbPlasma->setChecked( false);
 cbSolids->setChecked( true);
 cbSindis->setChecked( true);
 cbLiquid->setChecked( true);
 cbSimelt->setChecked( false);
 cbSorption->setChecked( false);
 cbPolyel->setChecked( false);
 cbHcarbl->setChecked( false);
 cbSolutions->setChecked( false);
 cbIsotopes->setChecked( false);
 //el_data.flags[cbRes_] = false;
}


void ElementsDialog::ResetData()
{
 int ii;
 for( ii=0; ii<bgOther->buttons().count(); ii++ )
   bgOther->button(ii)->setChecked ( false );
 for( ii=1; ii<99/*bgElem->buttons()->count()-1*/; ii++ ) // 0 always
      bgElem->button(ii)->setChecked ( false );
}

bool ElementsDialog::isAqueous() const
{
 return cbAqueous->isChecked();
}

void ElementsDialog::SetAqueous()
{
 if( cbAqueous->isChecked() )
 {
   pbE_0->setChecked(true);
   pbE_1->setChecked(true);
   pbE_8->setChecked(true);
 }
}

bool ElementsDialog::isSorption() const
{
 return cbSorption->isChecked();
}

void ElementsDialog::SetSorption()
{
 if( cbSorption->isChecked() )
 {
  cbAqueous->setChecked( true );
  SetAqueous();
 }
}

void ElementsDialog::changeCheck( QStandardItem *pdb )
{
   int jj;
   if( pdb == pkern )
       return;

   QStandardItem *parent = pdb->parent();
   if( parent && parent != pkern )
   {
       if( ( parent->checkState() == Qt::Unchecked && pdb->checkState() == Qt::PartiallyChecked )
        || ( parent->checkState() == Qt::Checked &&  pdb->checkState() == Qt::PartiallyChecked ))
           parent->setCheckState(Qt::PartiallyChecked);
      else
      { if( ( parent->checkState() == Qt::Unchecked || parent->checkState() == Qt::PartiallyChecked ) &&  pdb->checkState() == Qt::Checked  )
        {
           for( jj=0; jj<parent->rowCount(); jj++ )
               if(  parent->child(jj)->checkState() != Qt::Checked )
                   break;
           if(jj<parent->rowCount() )
             parent->setCheckState(Qt::PartiallyChecked);
           else
            parent->setCheckState(Qt::Checked);
       }

       if( ( parent->checkState() == Qt::Checked || parent->checkState() == Qt::PartiallyChecked ) &&  pdb->checkState() == Qt::Unchecked  )
        {
           for( jj=0; jj<parent->rowCount(); jj++ )
               if(  parent->child(jj)->checkState() != Qt::Unchecked )
                   break;
           if(jj<parent->rowCount() )
             parent->setCheckState(Qt::PartiallyChecked);
           else
            parent->setCheckState(Qt::Unchecked);
       }
      }
    }
    if( pdb->checkState() != Qt::PartiallyChecked )
    { for(int jj=0; jj<pdb->rowCount(); jj++ )
         pdb->child(jj)->setCheckState(pdb->checkState());
    }
}

void ElementsDialog::SetFiles()
{
  resetFilesSelection();
  ResetData();
  EmptyData();
  SetICompList();
  SetAqueous();
}

void ElementsDialog::SetICompList()
{
    TCIntArray aIndMT;
    TCStringArray aIC;
    QAbstractButton* bb;
    int nmbOther=1;

    aBtmId1.Clear();
    aICkey1.Clear();
    aBtmId2.Clear();
    aICkey2.Clear();

     openFilesICOMP();
   // select all IComp keys and indMT (set indMT to -1 for additional)
    TIComp* aICdata=(TIComp *)(&aMod[RT_ICOMP]);
    aICdata->GetElements( cbIsotopes->isChecked(), aIC, aIndMT );

    for( uint ii=0; ii<aIC.GetCount(); ii++ )
     if( aIndMT[ii] == -1) // additional
     {
       gstring name= gstring( aIC[ii], 0, rt[RT_ICOMP].FldLen(0) );
       name.strip();
       if( name != "Vol" )
       {
         ErrorIf( nmbOther>12, aIC[ii].c_str(),
                       "More than 11 additional Independent Components!");
         bb = bgOther->button(nmbOther);
         bb->setText( tr( name.c_str() ) );
         bb->setEnabled( true );
         aBtmId2.Add( nmbOther );
         aICkey2.Add( aIC[ii] );
         nmbOther ++;
       }
       else  //Vol
       {
         bb = bgOther->button(0);
         bb->setText( tr( name.c_str() ) );
         bb->setEnabled( true );
         aBtmId2.Add( 0 );
         aICkey2.Add( aIC[ii] );
       }
     }
     else //elements
     {
       bb = bgElem->button(aIndMT[ii]);
       if( !bb )
        Error( aIC[ii].c_str(), "E01EDrem: Invalid IComp ");
       aBtmId1.Add( aIndMT[ii] );
       aICkey1.Add( aIC[ii] );
       bb->setEnabled( true );
     }

  // set selection form template
  for( uint ii=0; ii<aBtmId1_sel.GetCount(); ii++ )
  {
       bb = bgElem->button(aBtmId1_sel[ii]);
       bb->setEnabled( false );
       bb->setChecked( true );
  }
  
  for( uint ii=0; ii<aICkey2_sel.GetCount(); ii++ )
  {
       int jj;
       gstring name= gstring( aICkey2_sel[ii],
                      0, rt[RT_ICOMP].FldLen(0) );
       name.strip();
       for( jj=0; jj<nmbOther; jj++ )
       {
         bb =  bgOther->button(jj);
         gstring ttl = bb->text().toLatin1().data();
         if( name == ttl )
         {
          bb->setEnabled( false );
          bb->setChecked( true );
          break;
         }
       }
       if( jj==nmbOther )
       {
         bb = bgOther->button(nmbOther);
         bb->setText( tr( name.c_str() ) );
         bb->setEnabled( false );
         bb->setChecked( true );
         aBtmId2.Add( nmbOther );
         aICkey2.Add( aICkey2_sel[ii] );
         nmbOther ++;
       }
  }
}

/*! returns selection array  (IComp record keys )
    array is empty if nothing is selected
*/
void ElementsDialog::allSelected( TCStringArray& aICkeys )
{
    uint ii;
    aICkeys.Clear();

    SetSorption();
    SetAqueous();

    for( ii=0; ii<aBtmId1.GetCount(); ii++ )
    {
     if( bgElem->button( aBtmId1[ii] )->isChecked()  )
        aICkeys.Add( aICkey1[ii] );
    }

    for( ii=0; ii<aBtmId2.GetCount(); ii++ )
    {
     if( bgOther->button( aBtmId2[ii] )->isChecked()  )
        aICkeys.Add( aICkey2[ii] );
    }
}

const setFiltersData&
ElementsDialog::getFilters()
{  // open all files
   openFilesSelection();
   return sf_data;
}

const elmWindowData&
ElementsDialog::getData()
{
 SetData();
 return  el_data;
}

void ElementsDialog::SetData()
{

    el_data.flags[cbAqueous_] = cbAqueous->isChecked();
    el_data.flags[cbGaseous_] =cbGaseous->isChecked();
    el_data.flags[cbFluid_] = cbFluid->isChecked();
    el_data.flags[cbPlasma_] = cbPlasma->isChecked();
    el_data.flags[cbSolids_] = cbSolids->isChecked();
    el_data.flags[cbSindis_] = cbSindis->isChecked();
    el_data.flags[cbLiquid_] = cbLiquid->isChecked();
    el_data.flags[cbSimelt_] = cbSimelt->isChecked();
    el_data.flags[cbSorption_] = cbSorption->isChecked();
    el_data.flags[cbPolyel_] = cbPolyel->isChecked();
    el_data.flags[cbHcarbl_] = cbHcarbl->isChecked();
    el_data.flags[cbSolutions_] = cbSolutions->isChecked();
    el_data.flags[cbIsotopes_] = cbIsotopes->isChecked();
    el_data.flags[cbRes_] = false;
  allSelected( el_data.ICrds );
}

/// Set up files_data  (lists of all kernel files names and  opened kernel files )
void ElementsDialog::setFilesList()
{
   size_t pos1, pos2;
   int cnt, cnt_sel, ind;

   for(int i=RT_SDATA; i<=RT_PHASE; i++ )
    {
        if( aMod[i].IsSubModule() )
            continue;
        TCStringArray names;
        TCIntArray indx;
        TCIntArray sel;
        rt[i].GetFileList(closef|openf|oldself, names, indx, sel);
        cnt = 0;
        cnt_sel = 0;
        for(int ii=names.GetCount()-1; ii>=0; ii-- )
        {
          // select only DB.default files
          if( names[ii].find( pVisor->sysDBDir())== gstring::npos )
              continue;
          // get 2 colums
          pos1 = names[ii].find_first_of(" ");
          pos2 = names[ii].rfind("/");
          files_data.flKeywds.Add( names[ii].substr( 0, pos1 ) );
          files_data.flNames.Add( names[ii].substr( pos2+1 ).c_str() );
          cnt++;
          ind = files_data.flKeywds.GetCount()-1;
          if( i == RT_SDATA || i == RT_CONST ||
              isOpenFile( files_data.flNames[ind] ) == 1 )
          {
            files_data.selKeywds.Add( files_data.flKeywds[ind] );
            cnt_sel++;
          }
        }
        files_data.flCnt.Add( cnt );
        files_data.selCnt.Add( cnt_sel );
    }
}

/// Reset list of opened kernel files
void ElementsDialog::resetFilesSelection()
{
  TCStringArray newSelKeywds;   // list of selected files
  TCIntArray    newSelCnt;      // count of newly selected files for type
   int cnt=0;
   int cnt2=0;

   //get new selection
   getSelectionTreeWidget();

   for(uint i=0; i<files_data.flCnt.GetCount(); i++ )
    {
        int cnt_sel = 0;
        for(int ii=0; ii<files_data.flCnt[i]; ii++ )
        {
          if( isOpenFile( files_data.flNames[cnt+ii] ) )
          {
             newSelKeywds.Add( files_data.flKeywds[cnt+ii] );
             cnt_sel++;
          }
        }
        cnt += files_data.flCnt[i];
        cnt2 += files_data.selCnt[i];
        newSelCnt.Add( cnt_sel );
    }

    files_data.selKeywds.Clear();
    files_data.selCnt.Clear();
    for(uint ii=0; ii<newSelCnt.GetCount(); ii++ )
       files_data.selCnt.Add( newSelCnt[ii] );
    for(uint ii=0; ii<newSelKeywds.GetCount(); ii++ )
       files_data.selKeywds.Add( newSelKeywds[ii] );
}

// Open files as difine in files_data selection
void ElementsDialog::openFilesSelection()
{
  TCStringArray newSelKeywds;   // list of selected files
  int cnt=0;
  ErrorIf( files_data.selCnt.GetCount()!=RT_PHASE+1, "",
               "E00EDrem: internal error");

 //files_data
   for(int i=RT_SDATA; i<=RT_PHASE; i++ )
   {
     newSelKeywds.Clear();

     for(int ii=0; ii<files_data.selCnt[i]; ii++ )
      newSelKeywds.Add(files_data.selKeywds[cnt+ii]);
     cnt += files_data.selCnt[i];
     // add project files keywds
     rt[i].GetProfileFileKeywds( prf_name.c_str(), newSelKeywds );
     //open all files
     rt[i].SetNewOpenFileList( newSelKeywds );
    }
}

/// Open only IComp files as define in files_data selection
void ElementsDialog::openFilesICOMP()
{
  TCStringArray newSelKeywds;   // list of selected files
  int cnt=0;

 //files_data
   for(int i=RT_SDATA; i<=RT_PHASE; i++ )
   {
     if( i == RT_ICOMP )
     {  newSelKeywds.Clear();
        for(int ii=0; ii<files_data.selCnt[i]; ii++ )
           newSelKeywds.Add(files_data.selKeywds[cnt+ii]);
        rt[i].SetNewOpenFileList( newSelKeywds );
        break;
     }
     cnt += files_data.selCnt[i];
   }
}

void ElementsDialog::setTreeWidget()
{
    FtreeView->setFont( pVisorImp->getCellFont() );
    FtreeView->setSelectionMode(QAbstractItemView::MultiSelection);    //setMultiSelection(true);
    FtreeView->setSortingEnabled( false );                            //setSorting(-1);
    FtreeView->setRootIsDecorated(true);
    FtreeView->setAllColumnsShowFocus(true);

    QStandardItemModel *standardModel = new QStandardItemModel;
    connect( standardModel, SIGNAL( itemChanged( QStandardItem * ) ),this, SLOT( changeCheck( QStandardItem * ) ) );
    pkern = standardModel->invisibleRootItem();
    standardModel->setHorizontalHeaderLabels( QStringList() <<  "Built-in Database"   <<  "Version");

    int ii, jj;
    QStandardItem* pdb;
    QStandardItem* pdb_child;
    QList<QStandardItem *> rowItems;

    QString aTag, aVer;
    gstring fname, tag, vers="";
    size_t pos1, pos2, pos3;

    for( ii=0; ii<files_data.flNames.GetCount(); ii++ )
    {
        pdb = pkern;
        fname = files_data.flNames[ii];

        //scip extension
        pos1 = fname.rfind(".");
        fname = fname.substr( 0, pos1+1 );

        // get version
        pos1 = fname.find(".ver");
        if( pos1 != gstring::npos )
        {
            vers = fname.substr(pos1+1);
            fname = fname.substr(0, pos1+1 );
            pos2 = vers.find(".");
            vers = vers.substr(0, pos2);
        }
        else
            vers = "not versioned";

        // first tag name of chain
        pos1 = fname.find(".");
        pos2 = fname.find(".", pos1+1);
        while( pos2 != gstring::npos )
        {
          tag = fname.substr(pos1+1, pos2-pos1-1);
          aTag = tag.c_str();
          pdb_child = 0;
          pos3 = fname.find(".", pos2+1);
          if( pos3 != gstring::npos)
              aVer = "";
          else aVer = vers.c_str();

          // test used tag before
          for( jj=0; jj<pdb->rowCount(); jj++ )
          {
              if( aTag == pdb->child(jj)->text() )
              {
                  pdb_child = pdb->child(jj);
                  break;
              }
          }
          if( !pdb_child )
          {
            pdb_child = new QStandardItem( aTag );
            pdb_child->setCheckable( true );
            pdb_child->setTristate( true );
            rowItems.clear();
            rowItems << pdb_child;
            rowItems << new QStandardItem(aVer);
            pdb->appendRow(rowItems);

          }
          pos1=pos2;
          pos2 = fname.find(".", pos1+1);
          pdb = pdb_child;
       }
   }

    FtreeView->setModel(standardModel);
    FtreeView->expandAll();
}


void ElementsDialog::getSelectionTreeWidget()
{
  selNames.Clear();
  // get names from FTreeWidget
  gstring tag = ".";
  for(int jj=0; jj<pkern->rowCount(); jj++ )
      getTag( tag, pkern->child(jj));

  for(int ii=0; ii<selNames.GetCount(); ii++ )
      cout << selNames[ii].c_str() << endl;
}

void ElementsDialog::getTag( gstring tag, QStandardItem* pdb)
{
    if( !pdb )
      return;

    gstring tag1 = pdb->text().toLatin1().data();
    tag += tag1;

    if( pdb->checkState() == Qt::Checked)
        selNames.Add( tag );
    else
    {
        tag += ".";
        for(int jj=0; jj<pdb->rowCount(); jj++ )
            getTag( tag, pdb->child(jj));
    }
}

void ElementsDialog::setSelectionTreeWidget()
{
  // clear all check in ftreeWidget ??!!

 gstring name;
 for(int ii=0; ii<selNames.GetCount(); ii++ )
     for(int jj=0; jj<pkern->rowCount(); jj++ )
     {
         name = selNames[ii];
         setTag( name, pkern->child(jj));
     }
}

void ElementsDialog::setTag( gstring fname, QStandardItem* pdb)
{
    if( !pdb )
      return;

    size_t pos1 = fname.find(".");
    size_t pos2 = fname.find(".", pos1+1);
    gstring tag = fname.substr(pos1+1, pos2-pos1-1);
    QString aTag = tag.c_str();

    if( aTag == pdb->text() )
    {
        if(pos2 == gstring::npos )
           pdb->setCheckState( Qt::Checked );
        else
        {
           for(int jj=0; jj<pdb->rowCount(); jj++ )
           {
             tag = fname.substr(pos2);
             setTag( tag, pdb->child(jj));
           }
        }
    }
}

/// Returns; boolean true if a keyword was found in the file name, false otherwise
///    for each of open file keywords;
int ElementsDialog::isOpenFile( gstring& name  )
{
    for(int ii=0; ii < selNames.GetCount(); ii++ )
        if(  name.find( selNames[ii] ) != gstring::npos )
            return 1;
    return 0;
}

// --------------------- End ElementsDialog.cpp -------------------------

