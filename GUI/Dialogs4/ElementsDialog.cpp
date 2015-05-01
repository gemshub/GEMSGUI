//-------------------------------------------------------------------
// $Id: ElementsDialog.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of ElementsDialog class
//
// Copyright (C) 2001-2008  S.Dmytriyeva
// Uses  string class (C) A.Rysin 1999
//
// This file is part of the GEM-Selektor GUI library which uses the
// Qt v.4 cross-platform App & UI framework (http://qt.nokia.com)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the terms of GEMS4 Development
// Quality Assurance Licence (GEMS4.QAL)
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
        elmWindowData  data,   const char* /*caption*/):
        QDialog( win ),
        prf_name ( prfName ),
        el_data( data )
{
	setupUi(this);

    string str =
          "Basis configuration of a new Modelling Project  ";
         str +=  rt[RT_PARAM].FldKey(0);
         setWindowTitle( trUtf8(str.c_str()) );

         QObject::connect( bBack, SIGNAL(clicked()), this, SLOT(CmBack()));
         stackedWidget->setCurrentIndex (0);
         resetNextButton();
         resetBackButton();

   // build IComp list from template database
    vector<int> aIndMT;
    vector<string> aIC;
    TIComp* aICdata=(TIComp *)(aMod[RT_ICOMP]);
    aICdata->GetElements( true, aIC, aIndMT );
    sf_data.ic_d.oldIComps.clear();

    for( uint ii=0; ii<aIC.size(); ii++ )
    {
     sf_data.ic_d.oldIComps.push_back( aIC[ii] );
     el_data.oldIComps.push_back( aIC[ii] );
     if( aIndMT[ii] == -1) // additional
     {
         aICkey2_sel.push_back( aIC[ii] );
     }
     else //elements
     {
       aBtmId1_sel.push_back( aIndMT[ii] );
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
    cbAqueous->setChecked( el_data.flags[cbAqueous_] );
    cbGaseous->setChecked( el_data.flags[cbGaseous_]);
    cbFluid->setChecked( el_data.flags[cbFluid_]);
    cbPlasma->setChecked( el_data.flags[cbPlasma_]);
    cbSolids->setChecked( el_data.flags[cbSolids_]);
    cbSindis->setChecked( el_data.flags[cbSindis_]);
    cbLiquid->setChecked( el_data.flags[cbLiquid_]);
    cbSimelt->setChecked( el_data.flags[cbSimelt_]);
    cbSorption->setChecked( el_data.flags[cbSorption_]);
    cbPolyel->setChecked( el_data.flags[cbPolyel_]);
    cbHcarbl->setChecked( el_data.flags[cbHcarbl_]);
    cbSolutions->setChecked( el_data.flags[cbSolutions_]);
    cbIsotopes->setChecked( el_data.flags[cbIsotopes_]);
    //el_data.flags[cbRes_] = false;

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
    connect( cbIsotopes, SIGNAL( clicked() ),this, SLOT( SetIsotopes() ) );
}

ElementsDialog::~ElementsDialog()
{
#ifdef oldDBusing
 delete files_data.ftree;
#endif
}

void ElementsDialog::setOpenFilesAsDefault()
{
  selNames.clear();
  string defName;

  if( el_data.aSelNames.empty() )
  {  defName = ".";
     defName += pVisor->defaultBuiltinTDBL();
     selNames.push_back(defName);
  }
  else  // get from template project
  {
      size_t pos1, pos2;

      el_data.setFlags( el_data.aSelNames);

      pos1 =  el_data.aSelNames.find( "<TDBfilters> = " ); //15
      if( pos1 == string::npos )
      {
         defName = ".";
         defName += pVisor->defaultBuiltinTDBL();
         selNames.push_back(defName);
         return;
      }
      pos1 += 15;
      pos2 = el_data.aSelNames.find_first_of(",;", pos1 );
      while( pos2 != string::npos  )
      {
          defName = el_data.aSelNames.substr(pos1, pos2-pos1);
          strip(defName);
          selNames.push_back(defName);
          pos1 = pos2+1;
          pos2 = el_data.aSelNames.find_first_of(",;", pos1);
      }
  }
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
    sf_data.ic_d.newIComps.clear();
    for(uint jj, ii=0; ii<el_data.ICrds.size(); ii++ )
    {
      for( jj=0; jj<sf_data.ic_d.oldIComps.size(); jj++)
        if( !memcmp( el_data.ICrds[ii].c_str(),
            sf_data.ic_d.oldIComps[jj].c_str(), MAXICNAME+MAXSYMB ) )
         break;
      if( jj== sf_data.ic_d.oldIComps.size() )
        sf_data.ic_d.newIComps.push_back( el_data.ICrds[ii] );
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

void ElementsDialog::SetIsotopes()
{
    ResetData();
    EmptyData();
    SetICompList();
    SetAqueous();
}

void ElementsDialog::changeCheck( QStandardItem *pdb )
{
   int jj;
   if( pdb == pkern )
       return;

   QStandardItem *parent = pdb->parent();
   if( parent && parent != pkern )
   {
       /*if( ( parent->checkState() == Qt::Unchecked &&
             ( pdb->checkState() == Qt::PartiallyChecked || pdb->checkState() == Qt::Checked ) )
               || ( parent->checkState() == Qt::Checked && (
                 pdb->checkState() == Qt::PartiallyChecked || pdb->checkState() == Qt::Unchecked )))
                  parent->setCheckState(Qt::PartiallyChecked);
        */
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
    vector<int> aIndMT;
    vector<string> aIC;
    QAbstractButton* bb;
    int nmbOther=1;

    aBtmId1.clear();
    aICkey1.clear();
    aBtmId2.clear();
    aICkey2.clear();

     openFilesICOMP();
   // select all IComp keys and indMT (set indMT to -1 for additional)
    TIComp* aICdata=(TIComp *)(aMod[RT_ICOMP]);
    aICdata->GetElements( cbIsotopes->isChecked(), aIC, aIndMT );

    for( int ii=0; ii<aIC.size(); ii++ )
     if( aIndMT[ii] == -1) // additional
     {
       string name= string( aIC[ii], 0, rt[RT_ICOMP].FldLen(0) );
       strip(name);
       if( name != "Vol" )
       {
         ErrorIf( nmbOther>12, aIC[ii],
                       "More than 11 additional Independent Components!");
         bb = bgOther->button(nmbOther);
         bb->setText( tr( name.c_str() ) );
         bb->setEnabled( true );
         aBtmId2.push_back( nmbOther );
         aICkey2.push_back( aIC[ii] );
         nmbOther ++;
       }
       else  //Vol
       {
         bb = bgOther->button(0);
         bb->setText( tr( name.c_str() ) );
         bb->setEnabled( true );
         aBtmId2.push_back( 0 );
         aICkey2.push_back( aIC[ii] );
       }
     }
     else //elements
     {
       bb = bgElem->button(aIndMT[ii]);
       if( !bb )
        Error( aIC[ii], "E01EDrem: Invalid IComp ");
       aBtmId1.push_back( aIndMT[ii] );
       aICkey1.push_back( aIC[ii] );
       bb->setEnabled( true );
     }

  // set selection form template
  for( int ii=0; ii<aBtmId1_sel.size(); ii++ )
  {
       bb = bgElem->button(aBtmId1_sel[ii]);
       bb->setEnabled( false );
       bb->setChecked( true );
  }
  
  for( int ii=0; ii<aICkey2_sel.size(); ii++ )
  {
       int jj;
       string name= string( aICkey2_sel[ii], 0, rt[RT_ICOMP].FldLen(0) );
       strip(name);
       for( jj=0; jj<nmbOther; jj++ )
       {
         bb =  bgOther->button(jj);
         string ttl = bb->text().toUtf8().data();
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
         aBtmId2.push_back( nmbOther );
         aICkey2.push_back( aICkey2_sel[ii] );
         nmbOther ++;
       }
  }
}

/*! returns selection array  (IComp record keys )
    array is empty if nothing is selected
*/
void ElementsDialog::allSelected( vector<string>& aICkeys )
{
    uint ii;
    aICkeys.clear();

    SetSorption();
    SetAqueous();

    for( ii=0; ii<aBtmId1.size(); ii++ )
    {
     if( bgElem->button( aBtmId1[ii] )->isChecked()  )
        aICkeys.push_back( aICkey1[ii] );
    }

    for( ii=0; ii<aBtmId2.size(); ii++ )
    {
     if( bgOther->button( aBtmId2[ii] )->isChecked()  )
        aICkeys.push_back( aICkey2[ii] );
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

   //for(int ii=0; ii<14; ii++)
   //     cout <<  " " <<  el_data.flags[ii];
   //cout << endl;

    el_data.aSelNames = el_data.getFlags();
    el_data.aSelNames += "<TDBfilters> = ";
    for( int ii=0; ii<selNames.size(); ii++)
    {
        el_data.aSelNames += selNames[ii];
        if( ii<selNames.size()-1 )
            el_data.aSelNames += ",\n";
    }
    el_data.aSelNames += ";\n";

    allSelected( el_data.ICrds );
}

/// Set up files_data  (lists of all kernel files names and  opened kernel files )
void ElementsDialog::setFilesList()
{
   rt.buildFilesList( files_data, selNames);
}

/// Reset list of opened kernel files
void ElementsDialog::resetFilesSelection()
{

   //get new selection
   getSelectionTreeWidget();

   // save new selection
   rt.resetFilesSelection(files_data, selNames);
}

// Open files as difine in files_data selection
void ElementsDialog::openFilesSelection()
{
  vector<string> newSelKeywds;   // list of selected files
  int cnt=0;
  ErrorIf( files_data.selCnt.size()!=RT_PHASE+1, "",
               "E00EDrem: internal error");

 //files_data
   for(int i=RT_SDATA; i<=RT_PHASE; i++ )
   {
     newSelKeywds.clear();

     for(int ii=0; ii<files_data.selCnt[i]; ii++ )
      newSelKeywds.push_back(files_data.selKeywds[cnt+ii]);
     cnt += files_data.selCnt[i];
     // push_back project files keywds
     rt[i].GetProfileFileKeywds( prf_name.c_str(), newSelKeywds );
     //open all files
     rt[i].SetNewOpenFileList( newSelKeywds );
    }
}

/// Open only IComp files as define in files_data selection
void ElementsDialog::openFilesICOMP()
{
  vector<string> newSelKeywds;   // list of selected files
  int cnt=0;

 //files_data
   for(int i=RT_SDATA; i<=RT_PHASE; i++ )
   {
     if( i == RT_ICOMP )
     {  newSelKeywds.clear();
        for(int ii=0; ii<files_data.selCnt[i]; ii++ )
           newSelKeywds.push_back(files_data.selKeywds[cnt+ii]);
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

    setTree( files_data.ftree->lst, pkern);

    FtreeView->setModel(standardModel);
    FtreeView->collapseAll ();  //expandAll();
}

void ElementsDialog::setTree( vector<FileTree *>& lst, QStandardItem* pdb)
{
    int jj;
    QStandardItem* pdb_child;
    QList<QStandardItem *> rowItems;
    QString aTag, aVer;
\
    for( jj=0; jj<lst.size(); jj++ )
    {
        aTag = lst[jj]->keywd.c_str();
        aVer = lst[jj]->version.c_str();
        pdb_child = new QStandardItem( aTag );
        pdb_child->setCheckable( true );
  ////      pdb_child->setTristate( true );
        rowItems.clear();
        rowItems << pdb_child;
        rowItems << new QStandardItem(aVer);
        pdb->appendRow(rowItems);
        setTree( lst[jj]->lst, pdb_child);
    }
}

void ElementsDialog::getSelectionTreeWidget()
{
  selNames.clear();
  // get names from FTreeWidget
  string tag = ".";
  for(int jj=0; jj<pkern->rowCount(); jj++ )
      getTag( tag, pkern->child(jj));

  for(uint ii=0; ii<selNames.size(); ii++ )
      cout << selNames[ii].c_str() << endl;
}

void ElementsDialog::getTag( string tag, QStandardItem* pdb)
{
    if( !pdb )
      return;

    string tag1 = pdb->text().toUtf8().data();
    tag += tag1;

    if( pdb->checkState() == Qt::Checked)
        selNames.push_back( tag );
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

 string name;
 for(uint ii=0; ii<selNames.size(); ii++ )
     for(int jj=0; jj<pkern->rowCount(); jj++ )
     {
         name = selNames[ii];
         setTag( name, pkern->child(jj));
     }
}

void ElementsDialog::setTag( string fname, QStandardItem* pdb)
{
    if( !pdb )
      return;

    size_t pos1 = fname.find(".");
    size_t pos2 = fname.find(".", pos1+1);
    if( pos2 == pos1+1 )
        pos2 = fname.find(".", pos2+1);
    string tag = fname.substr(pos1+1, pos2-pos1-1);
    QString aTag = tag.c_str();

    if( aTag == pdb->text() )
    {
        if(pos2 == string::npos )
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

// --------------------- End ElementsDialog.cpp -------------------------

