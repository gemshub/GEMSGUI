//-------------------------------------------------------------------
// $Id: GtDemoWizard.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of GtDemoWizard class
//
// Copyright (C) 2005-2008 S.Dmytriyeva
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

#include <qcheckbox.h>
#include <qspinbox.h>
#include <qradiobutton.h>
#include <qlineedit.h>
#include <qvariant.h>

#include "GtDemoWizard.h"
#include "GemsMainWindow.h"
#include "m_param.h"
#include "service.h"

void GtDemoWizard::languageChange()
{
    retranslateUi(this);
}

void GtDemoWizard::CmBack()
{
    stackedWidget->setCurrentIndex ( stackedWidget->currentIndex()-1 );
    resetNextButton();
    resetBackButton();
}

void GtDemoWizard::CmNext()
{
    int ndx = stackedWidget->currentIndex();
    auto nLines = pageScript->getScriptLinesNum();
    if( ndx == 1 && nLines > 0)
    {        pGraphY->setValue( nLines );
             pGraphX->setValue( pageScript->getAbscissaNum() );
    }

    if( ndx == 0 ) // define graph window
    {
      CmChangePage2(0);
    }

    stackedWidget->setCurrentIndex ( stackedWidget->currentIndex()+1 );
    resetNextButton();
    resetBackButton();
}

void 	GtDemoWizard::resetNextButton()
{
	if( stackedWidget->currentIndex() == stackedWidget->count() - 1 )
	{	
		pNext->disconnect();
		connect( pNext, SIGNAL(clicked()), this, SLOT(accept()) );
		pNext->setText("&Finish");
	}
	else
	{	
		pNext->disconnect();
		connect( pNext, SIGNAL(clicked()), this, SLOT(CmNext()) );
		pNext->setText("&Next>");
	}
}

void 	GtDemoWizard::resetBackButton()
{
	pBack->setEnabled( stackedWidget->currentIndex() > 0 );
}


GtDemoWizard::GtDemoWizard( const char* pkey, int size[8], const char *ascript,
                            const char *proc_key, const char* aXname, const char* aYname, QWidget* parent):
            QDialog( parent ), script(ascript), pageScript(nullptr)
{
    setupUi(this);

    string str1= "GEM-Selektor GtDemo Setup:  ";
            str1 += pkey;
            setWindowTitle( str1.c_str() );

    allButtons = new QButtonGroup( groupBox_7 );
    allButtons->addButton(psIC, 0);
    allButtons->addButton(psDC, 1);
    allButtons->addButton(psBC, 2);
    allButtons->addButton(psRE, 3);
    allButtons->addButton(psRP, 4);
    allButtons->addButton(psPH, 5);
    allButtons->addButton(psST, 6);
    allButtons->addButton(psPE, 7);
    allButtons->addButton(psPB, 8);
    allButtons->addButton(psUT, 9);
    allButtons->addButton(psTR, 10);

    stackedWidget->setCurrentIndex (0);
    resetNextButton();
    resetBackButton();
    
//Page1
   nRT = -1;// size[0];
   switch( size[0]) //nRT - chain index
   {
     case RT_ICOMP:  psIC->setChecked( true ); break;
     case RT_DCOMP:  psDC->setChecked( true ); break;
     case RT_COMPOS: psBC->setChecked( true ); break;
     case RT_REACDC: psRE->setChecked( true ); break;
     case RT_RTPARM: psRP->setChecked( true ); break;
     case RT_PHASE:  psPH->setChecked( true ); break;
     default:
     case RT_SYSEQ:  psST->setChecked( true ); break;
     case RT_PROCES: psPE->setChecked( true ); break;
     case RT_UNSPACE: psPB->setChecked( true ); break;
     case RT_DUALTH: psUT->setChecked( true ); break;
     case RT_GEM2MT: psTR->setChecked( true ); break;
   }
//Page 2
   resetPageList(size[0], aXname, aYname);

//Page3
    pGraphY->setValue(size[6]);
    pGraphX->setValue(size[7]);
    pNwc->setValue(size[2]);
    pNqp->setValue(size[3]);
    pELine->setValue(size[4]);
    pECol->setValue(size[5]);
//Page4
    spinBox18->setValue(size[1]);
    lineEditProcesKey->setText( proc_key );

    QObject::connect( pHelp, SIGNAL(clicked()), this, SLOT(help()));
    QObject::connect( pBack, SIGNAL(clicked()), this, SLOT(CmBack()));
    QObject::connect( pNext, SIGNAL(clicked()), this, SLOT(CmNext()));
    //QObject::connect( allButtons, SIGNAL(buttonClicked(int)), this, SLOT(CmChangePage2(int)));

    //groupBox_7->toggled();

}


GtDemoWizard::~GtDemoWizard()
{}

void   GtDemoWizard::getSizes( int size[8] )
{
    size[0] = getnRT();

    size[1]= spinBox18->value();
    size[2]= pNwc->value();
    size[3]= pNqp->value();
    size[4]= pELine->value();
    size[5]= pECol->value();
    size[6]= pGraphY->value();
    size[7]= pGraphX->value();
}

int   GtDemoWizard::getnRT()
{
    int newRT;

    if( psST->isChecked() )
        newRT = RT_SYSEQ;
    else
        if( psPE->isChecked() )
            newRT = RT_PROCES;
        else
            if( psPB->isChecked() )
                newRT = RT_UNSPACE;
            else
                if( psIC->isChecked() )
                    newRT = RT_ICOMP;
                else
                    if( psDC->isChecked() )
                        newRT = RT_DCOMP;
                    else
                        if( psBC->isChecked() )
                            newRT = RT_COMPOS;
                        else
                            if( psRE->isChecked() )
                                newRT = RT_REACDC;
                            else
                                if( psRP->isChecked() )
                                    newRT = RT_RTPARM;
                                else
                                    if( psPH->isChecked() )
                                        newRT = RT_PHASE;
                                    else
                                        if( psUT->isChecked() )
                                            newRT = RT_DUALTH;
                                        else
                                            newRT = RT_GEM2MT;

    return newRT;
}

void GtDemoWizard::CmChangePage2(int)
{
   int	newRT = getnRT();
   resetPageList(  newRT );
}

string
GtDemoWizard::getPrKey()
{
  string str = lineEditProcesKey->text().toStdString();
  return str;
}

void GtDemoWizard::help()
{
  pVisorImp->OpenHelp( GM_GTDEMO_WZ_HTML, WZSTEP, stackedWidget->currentIndex()+1 );
}

//==============================================================================

equatSetupData eqd( "x0", "y0", "jR", "jR", true );



// work with lists
void GtDemoWizard::resetPageList( int newRT,const char* aXname, const char* aYname )
{

    TIArray<pagesSetupData> scalarsList;
    TIArray<pagesSetupData> pgData;

    if( nRT == newRT )
      return;

    nRT = newRT;

    if( nRT >= RT_SYSEQ ) // read first record in DB
    {
        if(  rt[RT_SYSEQ].GetStatus() != ONEF_)
        {
            TCStringArray aRklist;
            TCIntArray anRk;
            auto Nr = rt[RT_SYSEQ].GetKeyList( ALLKEY, aRklist, anRk );
            if( Nr > 0 )
              TProfil::pm->loadSystat( aRklist[0].c_str() );
        }
        GetListsnRT( MD_MULTI, pgData,  scalarsList );
        GetListsnRT( MD_SYSTEM, pgData,  scalarsList );
        GetListsnRT( MD_MTPARM, pgData,  scalarsList );
        GetListsnRT( MD_RMULTS, pgData,  scalarsList );
        GetListsnRT( nRT, pgData,  scalarsList );
    }
    else
    {
        GetListsnRT( nRT, pgData,  scalarsList );
    }

    GetListsnRT( RT_GTDEMO, pgData,  scalarsList );


    if( pageScript )
      pageScript->resetPageList( newRT, pgData, scalarsList );
    else
    {
      pageScript = new EquatSetup( page_3, eqd, nRT, pgData, scalarsList,
                                   script.c_str(), aXname, aYname  );
      verticalLayout_2->addWidget(pageScript);
    }

}


//--------------------- End of GtDemoWizard.cpp ---------------------------
