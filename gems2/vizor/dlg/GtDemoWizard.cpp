//-------------------------------------------------------------------
// $$
//
// Implementation of GtDemoWizard class
//
// Copyright (C) 2005 S.Dmytriyeva
//
// This file is part of the GEM-Vizor library which uses the
// Qt v.2.x GUI Toolkit (Troll Tech AS, http://www.trolltech.com)
// according to the Qt Duo Commercial license
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL)
//
// See http://les.web.psi.ch/Software/GEMS-PSI/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

const char *GTDEMO_HTML = "gtdemo_setup";

#include <qcheckbox.h>
#include <qspinbox.h>
#include <qradiobutton.h>


#include "GtDemoWizard.h"
#include "gstring.h"
#include "visor_w.h"
#include "v_mod.h"


#define Inherited GtDemoWizardData

GtDemoWizard::GtDemoWizard( const char* pkey, int size[7], QWidget* parent):
        Inherited( parent, 0, true )
{
    gstring str1= "Setup to Create GtDemo:  ";
            str1 += pkey;
    setCaption( str1.c_str() );

    setFinishEnabled( WizardPage3, true);

//Page1
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

//Page2
    pGraph->setValue(size[6]);
    pNwc->setValue(size[2]);
    pNqp->setValue(size[3]);
    pELine->setValue(size[4]);
    pECol->setValue(size[5]);
//Page3
    spinBox18->setValue(size[1]);

}


GtDemoWizard::~GtDemoWizard()
{}

void   GtDemoWizard::getSizes( int size[7] )
{

   if( psST->isChecked() )
    size[0] = RT_SYSEQ;
   else
    if( psPE->isChecked() )
      size[0] = RT_PROCES;
    else
     if( psPB->isChecked() )
          size[0] = RT_UNSPACE;
     else
      if( psIC->isChecked() )
          size[0] = RT_ICOMP;
      else
       if( psDC->isChecked() )
          size[0] = RT_DCOMP;
       else
        if( psBC->isChecked() )
             size[0] = RT_COMPOS;
        else
         if( psRE->isChecked() )
             size[0] = RT_REACDC;
         else
          if( psRP->isChecked() )
             size[0] = RT_RTPARM;
          else
           if( psPH->isChecked() )
               size[0] = RT_PHASE;
           else
            if( psUT->isChecked() )
               size[0] = RT_DUALTH;
            else
               size[0] = RT_GEM2MT;

    size[1]= spinBox18->value();
    size[2]= pNwc->value();
    size[3]= pNqp->value();
    size[6]= pGraph->value();
    size[4]= pELine->value();
    size[5]= pECol->value();

}



void
GtDemoWizard::help()
{
  pVisorImp->OpenHelp( GTDEMO_HTML, 0, this/*, true*/ );
}

//--------------------- End of GtDemoWizard.cpp ---------------------------
