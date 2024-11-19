//-------------------------------------------------------------------
// $Id: ElementsDialog.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of ElementsDialog class
//
// Copyright (C) 2001-2008  S.Dmytriyeva
//
// This file is part of the GEM-Selektor GUI library which uses the
// Qt v.4 cross-platform App & UI framework (https://qt.io/download-open-source)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the GPL v.3 license

//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#include <QtGui>

#include "ui_ElementsDialog4.h"
#include "ElementsDialog.h"
#include "SetFiltersDialog.h"
#include "service.h"
#include "visor.h"
#include "GemsMainWindow.h"
#include "m_icomp.h"
#include "m_param.h"

void ElementsDialog::CmBack()
{
    ui->stackedWidget->setCurrentIndex ( ui->stackedWidget->currentIndex()-1 );
    resetNextButton();
    resetBackButton();
}

void ElementsDialog::CmNext()
{
    if(ui->stackedWidget->currentIndex()==0)
        SetFiles();
    ui->stackedWidget->setCurrentIndex ( ui->stackedWidget->currentIndex()+1 );
    resetNextButton();
    resetBackButton();
}

void 	ElementsDialog::resetNextButton()
{
    if( ui->stackedWidget->currentIndex() == ui->stackedWidget->count() - 1 )
    {
        ui->bNext->disconnect();
        ui->bReset->show();
        connect( ui->bNext, SIGNAL(clicked()), this, SLOT(CmOk()) );
        //pNext->setText("&Finish");
    }
    else
    {
        ui->bNext->disconnect();
        ui->bReset->hide();
        connect( ui->bNext, SIGNAL(clicked()), this, SLOT(CmNext()) );
        //pNext->setText("&Next>");
    }
}

void 	ElementsDialog::resetBackButton()
{
    ui->bBack->setEnabled( ui->stackedWidget->currentIndex() > 0 );
}

ElementsDialog::ElementsDialog(QWidget* win, const char * prfName,
                               elmWindowData  data1,   const char* /*caption*/):
    QDialog( win ),
    prf_name ( prfName ),
    el_data( data1 ),
    ui(new Ui::ElementsDialogData)
{
    ui->setupUi(this);

    string str =
            "Basis configuration of a new Modelling Project  ";
    str +=  TProfil::pm->projectName();
    setWindowTitle( str.c_str() );

    QObject::connect( ui->bBack, SIGNAL(clicked()), this, SLOT(CmBack()));
    ui->stackedWidget->setCurrentIndex (0);
    resetNextButton();
    resetBackButton();

    // build IComp list from template database
    TCIntArray aIndMT;
    TCStringArray aIC;
    TIComp* aICdata=dynamic_cast<TIComp*>(aMod[RT_ICOMP].get());
    aICdata->GetElements( true, aIC, aIndMT );
    sf_data.ic_d.oldIComps.clear();

    for( size_t ii=0; ii<aIC.size(); ii++ )
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
    bgElem = new QButtonGroup( ui->bgElemBox);
    bgElem->setExclusive(false);
    bgElem->addButton( ui->pbE_0, 0);
    bgElem->addButton( ui->pbE_1, 1);
    bgElem->addButton( ui->pbE_2, 2);
    bgElem->addButton( ui->pbE_3, 3);
    bgElem->addButton( ui->pbE_4, 4);
    bgElem->addButton( ui->pbE_5, 5);
    bgElem->addButton( ui->pbE_6, 6);
    bgElem->addButton( ui->pbE_7, 7);
    bgElem->addButton( ui->pbE_8, 8);
    bgElem->addButton( ui->pbE_9, 9);
    bgElem->addButton( ui->pbE_10, 10);
    bgElem->addButton( ui->pbE_11, 11);
    bgElem->addButton( ui->pbE_12, 12);
    bgElem->addButton( ui->pbE_13, 13);
    bgElem->addButton( ui->pbE_14, 14);
    bgElem->addButton( ui->pbE_15, 15);
    bgElem->addButton( ui->pbE_16, 16);
    bgElem->addButton( ui->pbE_17, 17);
    bgElem->addButton( ui->pbE_18, 18);
    bgElem->addButton( ui->pbE_19, 19);
    bgElem->addButton( ui->pbE_20, 20);
    bgElem->addButton( ui->pbE_21, 21);
    bgElem->addButton( ui->pbE_22, 22);
    bgElem->addButton( ui->pbE_23, 23);
    bgElem->addButton( ui->pbE_24, 24);
    bgElem->addButton( ui->pbE_25, 25);
    bgElem->addButton( ui->pbE_26, 26);
    bgElem->addButton( ui->pbE_27, 27);
    bgElem->addButton( ui->pbE_28, 28);
    bgElem->addButton( ui->pbE_29, 29);
    bgElem->addButton( ui->pbE_30, 30);
    bgElem->addButton( ui->pbE_31, 31);
    bgElem->addButton( ui->pbE_32, 32);
    bgElem->addButton( ui->pbE_33, 33);
    bgElem->addButton( ui->pbE_34, 34);
    bgElem->addButton( ui->pbE_35, 35);
    bgElem->addButton( ui->pbE_36, 36);
    bgElem->addButton( ui->pbE_37, 37);
    bgElem->addButton( ui->pbE_38, 38);
    bgElem->addButton( ui->pbE_39, 39);
    bgElem->addButton( ui->pbE_40, 40);
    bgElem->addButton( ui->pbE_41, 41);
    bgElem->addButton( ui->pbE_42, 42);
    bgElem->addButton( ui->pbE_43, 43);
    bgElem->addButton( ui->pbE_44, 44);
    bgElem->addButton( ui->pbE_45, 45);
    bgElem->addButton( ui->pbE_46, 46);
    bgElem->addButton( ui->pbE_47, 47);
    bgElem->addButton( ui->pbE_48, 48);
    bgElem->addButton( ui->pbE_49, 49);
    bgElem->addButton( ui->pbE_50, 50);
    bgElem->addButton( ui->pbE_51, 51);
    bgElem->addButton( ui->pbE_52, 52);
    bgElem->addButton( ui->pbE_53, 53);
    bgElem->addButton( ui->pbE_54, 54);
    bgElem->addButton( ui->pbE_55, 55);
    bgElem->addButton( ui->pbE_56, 56);
    bgElem->addButton( ui->pbE_57, 57);
    bgElem->addButton( ui->pbE_58, 58);
    bgElem->addButton( ui->pbE_59, 59);
    bgElem->addButton( ui->pbE_60, 60);
    bgElem->addButton( ui->pbE_61, 61);
    bgElem->addButton( ui->pbE_62, 62);
    bgElem->addButton( ui->pbE_63, 63);
    bgElem->addButton( ui->pbE_64, 64);
    bgElem->addButton( ui->pbE_65, 65);
    bgElem->addButton( ui->pbE_66, 66);
    bgElem->addButton( ui->pbE_67, 67);
    bgElem->addButton( ui->pbE_68, 68);
    bgElem->addButton( ui->pbE_69, 69);
    bgElem->addButton( ui->pbE_70, 70);
    bgElem->addButton( ui->pbE_71, 71);
    bgElem->addButton( ui->pbE_72, 72);
    bgElem->addButton( ui->pbE_73, 73);
    bgElem->addButton( ui->pbE_74, 74);
    bgElem->addButton( ui->pbE_75, 75);
    bgElem->addButton( ui->pbE_76, 76);
    bgElem->addButton( ui->pbE_77, 77);
    bgElem->addButton( ui->pbE_78, 78);
    bgElem->addButton( ui->pbE_79, 79);
    bgElem->addButton( ui->pbE_80, 80);
    bgElem->addButton( ui->pbE_81, 81);
    bgElem->addButton( ui->pbE_82, 82);
    bgElem->addButton( ui->pbE_83, 83);
    bgElem->addButton( ui->pbE_84, 84);
    bgElem->addButton( ui->pbE_85, 85);
    bgElem->addButton( ui->pbE_86, 86);
    bgElem->addButton( ui->pbE_87, 87);
    bgElem->addButton( ui->pbE_88, 88);
    bgElem->addButton( ui->pbE_89, 89);
    bgElem->addButton( ui->pbE_90, 90);
    bgElem->addButton( ui->pbE_91, 91);
    bgElem->addButton( ui->pbE_92, 92);
    bgElem->addButton( ui->pbE_93, 93);
    bgElem->addButton( ui->pbE_94, 94);
    bgElem->addButton( ui->pbE_95, 95);
    bgElem->addButton( ui->pbE_96, 96);
    bgElem->addButton( ui->pbE_97, 97);
    bgElem->addButton( ui->pbE_98, 98);

    bgElem->addButton( ui->pbE_100, 100);
    bgElem->addButton( ui->pbE_101, 101);

    bgOther = new QButtonGroup( ui->bgOtherBox);
    bgOther->setExclusive(false);
    bgOther->addButton( ui->pbA_01, 1);
    bgOther->addButton( ui->pbA_02, 2);
    bgOther->addButton( ui->pbA_03, 3);
    bgOther->addButton( ui->pbA_04, 4);
    bgOther->addButton( ui->pbA_05, 5);
    bgOther->addButton( ui->pbA_06, 6);
    bgOther->addButton( ui->pbA_07, 7);
    bgOther->addButton( ui->pbA_08, 8);
    bgOther->addButton( ui->pbA_09, 9);
    bgOther->addButton( ui->pbA_10, 10);
    bgOther->addButton( ui->pbA_11, 11);
    bgOther->addButton( ui->pbA_12, 12);
    bgOther->addButton( ui->pbA_13, 13);
    bgOther->addButton( ui->pbA_14, 14);
    bgOther->addButton( ui->pbA_15, 15);
    bgOther->addButton( ui->pbA_16, 16);
    bgOther->addButton( ui->pbA_17, 17);
    bgOther->addButton( ui->pbA_18, 18);
    bgOther->addButton( ui->pbA_19, 19);
    bgOther->addButton( ui->pbA_20, 20);
    bgOther->addButton( ui->pbA_21, 21);
    bgOther->addButton( ui->pbA_22, 22);
    bgOther->addButton( ui->pbA_23, 23);
    //     bgOther->addButton( ui->pbA_12, 12);
    bgOther->addButton( ui->pbA_00, 0);

    EmptyData();

    // define page 1

    // set up default Open files names
    setOpenFilesAsDefault(); // <.kernel.> now, start define selNames
    ui->cbAqueous->setChecked( el_data.flags[cbAqueous_] );
    ui->cbGaseous->setChecked( el_data.flags[cbGaseous_]);
    ui->cbFluid->setChecked( el_data.flags[cbFluid_]);
    ui->cbPlasma->setChecked( el_data.flags[cbPlasma_]);
    ui->cbSolids->setChecked( el_data.flags[cbSolids_]);
    ui->cbSindis->setChecked( el_data.flags[cbSindis_]);
    ui->cbLiquid->setChecked( el_data.flags[cbLiquid_]);
    ui->cbSimelt->setChecked( el_data.flags[cbSimelt_]);
    ui->cbSorption->setChecked( el_data.flags[cbSorption_]);
    ui->cbPolyel->setChecked( el_data.flags[cbPolyel_]);
    ui->cbHcarbl->setChecked( el_data.flags[cbHcarbl_]);
    ui->cbSolutions->setChecked( el_data.flags[cbSolutions_]);
    ui->cbIsotopes->setChecked( el_data.flags[cbIsotopes_]);
    //el_data.flags[cbRes_] = false;

    // define list of files in DefaultDB (files_data)
    setFilesList();
    // define FTreeWidget with DefaultDB files names  files_data.flNames
    setTreeWidget();
    setSelectionTreeWidget();  // set up selection in wiget use selNames

    SetICompList();
    SetAqueous();

    // signals and slots connections
    QObject::connect(ui->pCancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect( ui->bHelp, SIGNAL(clicked()), this, SLOT( CmHelp() ) );
    connect( ui->bReset, SIGNAL(clicked()), this, SLOT( CmSetFilters() ) );
    connect( ui->bSelectAll, SIGNAL(clicked()), this, SLOT( CmSelectAll() ) );
    connect( ui->bClearSelection, SIGNAL(clicked()), this, SLOT( CmClearSelection() ) );

    connect( ui->cbAqueous, SIGNAL(clicked()), this, SLOT( SetAqueous() ) );
    connect( ui->cbSorption, SIGNAL(clicked()),this, SLOT( SetSorption() ) );
    connect( ui->cbIsotopes, SIGNAL(clicked()),this, SLOT( SetIsotopes() ) );
}

ElementsDialog::~ElementsDialog()
{
    delete ui;
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
            strip( defName );
            selNames.push_back(defName);
            pos1 = pos2+1;
            pos2 = el_data.aSelNames.find_first_of(",;", pos1);
        }
    }
}

void ElementsDialog::CmSelectAll()
{
    int ii;
    for( ii=0; ii<bgOther->buttons().count(); ii++ )
    {
       if(bgOther->button(ii)->isEnabled())
           bgOther->button(ii)->setChecked (true);
    }
    for( ii=1; ii<99/*bgElem->buttons()->count()-1*/; ii++ ) // 0 always
    {
       if(bgElem->button(ii)->isEnabled())
           bgElem->button(ii)->setChecked (true);
    }
}

void ElementsDialog::CmClearSelection()
{
    int ii;
    for( ii=0; ii<bgOther->buttons().count(); ii++ )
    {
       if(bgOther->button(ii)->isEnabled())
           bgOther->button(ii)->setChecked (false);
    }
    for( ii=1; ii<99/*bgElem->buttons()->count()-1*/; ii++ ) // 0 always
    {
       if(bgElem->button(ii)->isEnabled())
           bgElem->button(ii)->setChecked(false);
    }
}

void ElementsDialog::CmHelp()
{
    pVisorImp->OpenHelp( GEMS_ELEMENTS_HTML, WZSTEP, ui->stackedWidget->currentIndex()+1 );
}

void ElementsDialog::CmSetFilters()
{
    // Here to call SetFiltersDialog !!
    openFilesSelection();
    SetData();
    sf_data.ic_d.newIComps.clear();
    for(size_t jj, ii=0; ii<el_data.ICrds.size(); ii++ )
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
    return ui->cbAqueous->isChecked();
}

void ElementsDialog::SetAqueous()
{
    if( ui->cbAqueous->isChecked() )
    {
        ui->pbE_0->setChecked(true);
        ui->pbE_1->setChecked(true);
        ui->pbE_8->setChecked(true);
    }
}

bool ElementsDialog::isSorption() const
{
    return ui->cbSorption->isChecked();
}

void ElementsDialog::SetSorption()
{
    if( ui->cbSorption->isChecked() )
    {
        ui->cbAqueous->setChecked( true );
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
    {
        for( jj=0; jj<pdb->rowCount(); jj++ )
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

    aBtmId1.clear();
    aICkey1.clear();
    aBtmId2.clear();
    aICkey2.clear();

    openFilesICOMP();
    // select all IComp keys and indMT (set indMT to -1 for additional)
    TIComp* aICdata=dynamic_cast<TIComp*>(aMod[RT_ICOMP].get());
    aICdata->GetElements( ui->cbIsotopes->isChecked(), aIC, aIndMT );

    for( size_t ii=0; ii<aIC.size(); ii++ )
        if( aIndMT[ii] == -1) // additional
        {
            string name= std::string( aIC[ii], 0, rt[RT_ICOMP]->FldLen(0) );
            strip( name );
            if( name != "Vol" )
            {
                ErrorIf( nmbOther>23, aIC[ii].c_str(),
                         "More than 23 additional Independent Components!");
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
    for( size_t ii=0; ii<aBtmId1_sel.size(); ii++ )
    {
        bb = bgElem->button(aBtmId1_sel[ii]);
        bb->setEnabled( false );
        bb->setChecked( true );
    }

    for( size_t ii=0; ii<aICkey2_sel.size(); ii++ )
    {
        int jj;
        string name= string( aICkey2_sel[ii],
                             0, rt[RT_ICOMP]->FldLen(0) );
        strip( name );
        for( jj=0; jj<nmbOther; jj++ )
        {
            bb =  bgOther->button(jj);
            string ttl = bb->text().toStdString();
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
void ElementsDialog::allSelected( TCStringArray& aICkeys )
{
    size_t ii;
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
    el_data.flags[cbAqueous_] = ui->cbAqueous->isChecked();
    el_data.flags[cbGaseous_] =ui->cbGaseous->isChecked();
    el_data.flags[cbFluid_] = ui->cbFluid->isChecked();
    el_data.flags[cbPlasma_] = ui->cbPlasma->isChecked();
    el_data.flags[cbSolids_] = ui->cbSolids->isChecked();
    el_data.flags[cbSindis_] = ui->cbSindis->isChecked();
    el_data.flags[cbLiquid_] = ui->cbLiquid->isChecked();
    el_data.flags[cbSimelt_] = ui->cbSimelt->isChecked();
    el_data.flags[cbSorption_] = ui->cbSorption->isChecked();
    el_data.flags[cbPolyel_] = ui->cbPolyel->isChecked();
    el_data.flags[cbHcarbl_] = ui->cbHcarbl->isChecked();
    el_data.flags[cbSolutions_] = ui->cbSolutions->isChecked();
    el_data.flags[cbIsotopes_] = ui->cbIsotopes->isChecked();
    el_data.flags[cbRes_] = false;

    if(gui_logger->should_log(spdlog::level::debug)) {
        stringstream buf;
        for(int ii=0; ii<14; ii++)
            buf <<  " " <<  el_data.flags[ii];
        gui_logger->debug("el_data.flags: {}", buf.str());
    }

    el_data.aSelNames = el_data.getFlags();
    el_data.aSelNames += "<TDBfilters> = ";
    for( size_t ii=0; ii<selNames.size(); ii++)
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
    size_t pos1, pos2;
    int cnt, cnt_sel;
    size_t ind;

    for(uint i=RT_SDATA; i<=RT_PHASE; i++ )
    {
        if( aMod[i]->IsSubModule() )
            continue;
        TCStringArray names;
        TCIntArray indx;
        TCIntArray sel;
        rt[i]->GetFileList(closef|openf|oldself, names, indx, sel);
        cnt = 0;
        cnt_sel = 0;
        for(size_t ii=0; ii<names.size(); ii++ )
        {
            // select only DB.default files
            if( names[ii].find( pVisor->sysDBDir())== string::npos )
                continue;
            // get 2 colums
            pos1 = names[ii].find_first_of(" ");
            pos2 = names[ii].rfind("/");
            files_data.flKeywds.push_back( names[ii].substr( 0, pos1 ) );
            files_data.flNames.push_back( names[ii].substr( pos2+1 ).c_str() );
            cnt++;
            ind = files_data.flKeywds.size()-1;
            if( i == RT_SDATA || i == RT_CONST ||
                    isOpenFile( files_data.flNames[ind] ) == 1 )
            {
                files_data.selKeywds.push_back( files_data.flKeywds[ind] );
                cnt_sel++;
            }
        }
        files_data.flCnt.push_back( cnt );
        files_data.selCnt.push_back( cnt_sel );
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

    for(size_t i=0; i<files_data.flCnt.size(); i++ )
    {
        int cnt_sel = 0;
        for(int ii=0; ii<files_data.flCnt[i]; ii++ )
        {
            if( isOpenFile( files_data.flNames[cnt+ii] ) )
            {
                newSelKeywds.push_back( files_data.flKeywds[cnt+ii] );
                cnt_sel++;
            }
        }
        cnt += files_data.flCnt[i];
        cnt2 += files_data.selCnt[i];
        newSelCnt.push_back( cnt_sel );
    }

    files_data.selKeywds.clear();
    files_data.selCnt.clear();
    for(size_t ii=0; ii<newSelCnt.size(); ii++ )
        files_data.selCnt.push_back( newSelCnt[ii] );
    for(size_t ii=0; ii<newSelKeywds.size(); ii++ )
        files_data.selKeywds.push_back( newSelKeywds[ii] );
}

// Open files as difine in files_data selection
void ElementsDialog::openFilesSelection()
{
    TCStringArray newSelKeywds;   // list of selected files
    int cnt=0;
    ErrorIf( files_data.selCnt.size()!=RT_PHASE+1, "",
             "E00EDrem: internal error");

    //files_data
    for(size_t i=RT_SDATA; i<=RT_PHASE; i++ )
    {
        newSelKeywds.clear();

        for(int ii=0; ii<files_data.selCnt[i]; ii++ )
            newSelKeywds.push_back(files_data.selKeywds[cnt+ii]);
        cnt += files_data.selCnt[i];
        // add project files keywds
        rt[i]->GetProfileFileKeywds( prf_name.c_str(), newSelKeywds );
        //open all files
        rt[i]->SetNewOpenFileList( newSelKeywds );
    }
}

/// Open only IComp files as define in files_data selection
void ElementsDialog::openFilesICOMP()
{
    TCStringArray newSelKeywds;   // list of selected files
    int cnt=0;

    //files_data
    for(size_t i=RT_SDATA; i<=RT_PHASE; i++ )
    {
        if( i == RT_ICOMP )
        {  newSelKeywds.clear();
            for(int ii=0; ii<files_data.selCnt[i]; ii++ )
                newSelKeywds.push_back(files_data.selKeywds[cnt+ii]);
            rt[i]->SetNewOpenFileList( newSelKeywds );
            break;
        }
        cnt += files_data.selCnt[i];
    }
}

void ElementsDialog::setTreeWidget()
{
    ui->FtreeView->setFont( pVisorImp->getCellFont() );
    ui->FtreeView->setSelectionMode(QAbstractItemView::MultiSelection);    //setMultiSelection(true);
    ui->FtreeView->setSortingEnabled( false );                            //setSorting(-1);
    ui->FtreeView->setRootIsDecorated(true);
    ui->FtreeView->setAllColumnsShowFocus(true);

    QStandardItemModel *standardModel = new QStandardItemModel;
    connect( standardModel, SIGNAL(itemChanged(QStandardItem*)),this, SLOT(changeCheck(QStandardItem*)) );
    pkern = standardModel->invisibleRootItem();
    standardModel->setHorizontalHeaderLabels( QStringList() <<  "Built-in Database"   <<  "Version");

    size_t ii;
    int jj;
    QStandardItem* pdb;
    QStandardItem* pdb_child;
    QList<QStandardItem *> rowItems;

    QString aTag, aVer;
    string fname, tag, vers="";
    size_t pos1, pos2, pos3;

    for( ii=0; ii<files_data.flNames.size(); ii++ )
    {
        pdb = pkern;
        fname = files_data.flNames[ii];

        //scip extension
        pos1 = fname.rfind(".");
        fname = fname.substr( 0, pos1+1 );

        // get version
        pos1 = fname.find(".ver");
        if( pos1 != string::npos )
        {
            vers = fname.substr(pos1+1+3);
            fname = fname.substr(0, pos1+1 );
            pos2 = vers.rfind(".");
            vers = vers.substr(0, pos2);
        }
        else
            vers = "not versioned";

        // first tag name of chain
        pos1 = fname.find(".");
        pos2 = fname.find(".", pos1+1);
        while( pos2 != string::npos )
        {
            tag = fname.substr(pos1+1, pos2-pos1-1);
            aTag = tag.c_str();
            pdb_child = nullptr;
            pos3 = fname.find(".", pos2+1);
            if( pos3 != string::npos)
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
                //            pdb_child->setTristate( true );
                rowItems.clear();
                rowItems << pdb_child;
                rowItems << new QStandardItem(aVer);
                pdb->appendRow(rowItems);

            }
            pos1=pos2;
            pos2 = fname.find(".", pos1+1);
            pdb = pdb_child;
        }
        aTag = ".";
        if( pdb->rowCount()<1 ||  aTag != pdb->child(0)->text() )
        {  pdb_child = new QStandardItem( aTag );
            pdb_child->setCheckable( true );
            //          pdb_child->setTristate( true );
            pdb->insertRow(0, pdb_child);
        }
    }

    // delete last "."
    aTag = ".";
    deleteTag( aTag, pkern);

    ui->FtreeView->setModel(standardModel);
    ui->FtreeView->collapseAll ();  //expandAll();
}

void ElementsDialog::deleteTag( QString aTag, QStandardItem* pdb)
{
    if( !pdb )
        return;

    if( pdb->rowCount() == 1 &&  aTag == pdb->child(0)->text() )
        pdb->removeRow(0);
    else
    {
        for(int jj=0; jj<pdb->rowCount(); jj++ )
            deleteTag( aTag, pdb->child(jj));
    }
}


void ElementsDialog::getSelectionTreeWidget()
{
    selNames.clear();
    // get names from FTreeWidget
    string tag = ".";
    for(int jj=0; jj<pkern->rowCount(); jj++ )
        getTag( tag, pkern->child(jj));

    if(gui_logger->should_log(spdlog::level::debug)) {
        stringstream buf;
        for(size_t ii=0; ii<selNames.size(); ii++ )
            buf << " " << selNames[ii] << endl;
        gui_logger->debug("names from FTreeWidget: {}", buf.str());
    }
}

void ElementsDialog::getTag( string tag, QStandardItem* pdb)
{
    if( !pdb )
        return;

    string tag1 = pdb->text().toStdString();
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
    for(size_t ii=0; ii<selNames.size(); ii++ )
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

/// Returns; boolean true if a keyword was found in the file name, false otherwise
///    for each of open file keywords;
int ElementsDialog::isOpenFile(string& name)
{
    std::string fname = name;

    //scip extension
    size_t pos1 = fname.rfind(".");
    fname = fname.substr( 0, pos1 );
    // scip version
    pos1 = fname.find(".ver");
    if( pos1 != string::npos )
        fname = fname.substr(0, pos1 );

    // first tag name of chain
    pos1 = fname.find(".");
    fname = fname.substr( pos1+1 );

    gui_logger->debug("ElementsDialog::isOpenFile({})", fname);
    for(size_t ii=0; ii < selNames.size(); ii++ )
    {
        if(  name.find( selNames[ii] ) != string::npos )
            return 1;

        if(  selNames[ii].find(fname) != string::npos )
            return 1;
    }
    return 0;
}

// --------------------- End ElementsDialog.cpp -------------------------

