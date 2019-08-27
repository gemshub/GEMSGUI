//-------------------------------------------------------------------
// $Id: ChangeKeyDialog.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of ChangeKeyDialog class
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

#include <qcombobox.h>
#include <qvariant.h>

#include "GemsMainWindow.h"
#include "service.h"
#include "ChangeKeyDialog.h"


ChangeKeyDialog::ChangeKeyDialog(QWidget* win, int nsymb, const char* /*caption*/ ):
        QDialog( win), nSymbols(nsymb)
{
	setupUi(this);
//    setCaption( caption );

      lTo1->setMaxLength( nSymbols );
      lfirst->setMaxLength( nSymbols );
      llast->setMaxLength( nSymbols );
      nfirst->setMaximum(nSymbols-1);
      nlast->setMaximum(nSymbols-1);

      connect( lTo1, SIGNAL(editingFinished ()), this, SLOT(SetToString()) );
      connect( lfirst, SIGNAL(editingFinished ()), this, SLOT(SetToString()) );
      connect( llast, SIGNAL(editingFinished ()), this, SLOT(SetToString()) );
      connect( bInv1, SIGNAL(toggled( bool )), this, SLOT(SetToString()) );
      connect( badd1, SIGNAL(toggled( bool )), this, SLOT(SetToString()) );
      connect( bchange1, SIGNAL(toggled( bool )), this, SLOT(SetToString()) );
      connect( chfirst, SIGNAL(toggled( bool )), this, SLOT(SetToString()) );
      connect( chlast, SIGNAL(toggled( bool )), this, SLOT(SetToString()) );

      connect( nfirst, SIGNAL(valueChanged( int )), this, SLOT(SetFromString()) );
      connect( nlast, SIGNAL(valueChanged( int )), this, SLOT(SetFromString()) );

      connect( rAll, SIGNAL(toggled( bool )), this, SLOT(SetAll( bool )) );
      connect( rPart, SIGNAL(toggled( bool )), this, SLOT(SetPart( bool )) );
}


ChangeKeyDialog::~ChangeKeyDialog()
{}

void ChangeKeyDialog::languageChange()
{
    retranslateUi(this);
}


gstring
ChangeKeyDialog::getTemplFrom()
{
 SetFromString();
 gstring ret = templFrom->text().toLatin1().data();
 return ret;
}

gstring
ChangeKeyDialog::getTemplTo()
{
 SetToString();
 gstring ret = templTo->text().toLatin1().data();
 return ret;
}

void ChangeKeyDialog::CmHelp()
{
   pVisorImp->OpenHelp( GEMS_TEMPL_HTML );
}


void ChangeKeyDialog::SetFromString()
{
    gstring ret;

    if(rAll->isChecked())
      ret = "*";
    else //  ??*???, *?, ??*
     {
       auto n = nfirst->value();
       ret = gstring(n, '?');
       ret+= "*";
       n = nlast->value();
       ret += gstring(n, '?');
     }

    templFrom->setText( ret.c_str() );
}

void ChangeKeyDialog::SetToString()
{
    gstring ret="";
    gstring part;

    if(rAll->isChecked()) // *abc, abc, invcase
     {
      if( bInv1->isChecked())
        ret = "invcase";
      else
      {
        if( badd1->isChecked())
            ret = "*";
        part = lTo1->text().toLatin1().data();
        ret += part;
       }
     }
    else //  ab*abc, invcase*a, bb*invcase, *bb, a*
     {
       if(chfirst->isChecked())
         ret = "invcase";
       else
         ret =  lfirst->text().toLatin1().data();
       ret += "*";
       if(chlast->isChecked())
         ret += "invcase";
       else
         ret +=  llast->text().toLatin1().data();
      }

      templTo->setText( ret.c_str() );
}

void ChangeKeyDialog::SetAll( bool vl )
{
   group1->setEnabled(vl);
   rPart->setChecked(!vl);
   SetFromString();
   SetToString();
}

void ChangeKeyDialog::SetPart( bool vl )
{
   group1_2->setEnabled(vl);
   rAll->setChecked(!vl);
   SetFromString();
   SetToString();
}

//--------------------- End of ChangeKeyDialog.cpp ---------------------------

