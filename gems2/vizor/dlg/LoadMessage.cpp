//-------------------------------------------------------------------
// Id: gems/vizor/dlg/LoadMessage.cpp  version 2.0.0    2001
//
// Implementation of LoadMessage class
//
// Created : 970207    Modified: 010908
//
// Copyright (C) 1996-2001  A.Rysin, S.Dmytriyeva
// Uses  gstring class (C) A.Rysin 1999
//
// This file is part of the GEM-Vizor library which uses the
// Qt v.2.x GUI Toolkit (Troll Tech AS, http://www.trolltech.com)
// according to the Qt Duo Commercial license #1435515
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL)
//
// See http://les.web.psi.ch/Software/GEMS-PSI/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#include "LoadMessage.h"
#include <iostream>

LoadMessage* LoadMessage::pDia = 0;

LoadMessage::LoadMessage( QWidget* parent,
             const char* name,
             const char* msg,
             int prog,
             int total):
               QProgressDialog(parent, name)
{
       pDia = this;

       // reset();
        if( total > 0)
         setTotalSteps(total);
        else
         setTotalSteps(100);
        setLabelText(msg);
        setProgress(prog);
        qApp->processEvents();
        setCaption( name );

}

void LoadMessage::Update( const char* mess, int prog, int total )
{
        if( total > 0)
         setTotalSteps(total);
        setLabelText(mess);
        setProgress(prog);
        qApp->processEvents();
}

void LoadMessage::closeEvent(QCloseEvent* ev)
{
      pDia = 0;
      QProgressDialog::closeEvent(ev);
}

//--------------------- End of LoadMessage.cpp ---------------------------

