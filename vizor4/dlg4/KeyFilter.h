//-------------------------------------------------------------------
// $Id: KeyFilter.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of KeyFilter class
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
#ifndef _KeyFilter_h_
#define _KeyFilter_h_

#include <qdialog.h>
#include <qlineedit.h>
#include <qlabel.h>

#include "array.h"

#ifndef _visor_w_h
#include "visor_w.h"
#endif

#ifndef _visor_h_
#include "visor.h"
#endif

#include "v_module.h"

//=============================================
// KeyEdit dialog
//=============================================

class KeyFilter:   public QDialog
{
    Q_OBJECT

    TIArray<QLineEdit> aEdit;
    QLineEdit *fullKey;

    int iRt;
 //   int iEd;
    bool allowTemplates;

protected slots:
    void EvSetAll();
    void EvGetList();
    void CmOk();
    void CmHelp();    
    void setKeyLine();

protected:
    gstring SetKeyString();

public:
    KeyFilter(QWidget* win, int iRt, const char* keyFilter,
              const char* caption, bool allowTemplates=true );

    gstring getFilter();
};

#endif   // _KeyFilter_h_
