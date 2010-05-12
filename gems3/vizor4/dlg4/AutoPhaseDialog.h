//-------------------------------------------------------------------
// $Id: AutoPhaseDialog.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of AutoPhaseDialog class
//
// (C) 2003-2008  S.Dmytriyeva
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

#ifndef AutoPhaseDialog_included
#define AutoPhaseDialog_included

#include <QDialog>

#include "ui_AutoPhaseDialog4.h"
#include "gstring.h"

class AutoPhaseDialog : public QDialog, public Ui::AutoPhaseDialogData
{
    Q_OBJECT

public slots:
//    void CmCheck();

public:
    AutoPhaseDialog(
         const char* pr_key, char acode, char gcode, QWidget* parent = NULL);
    virtual ~AutoPhaseDialog();

    void set_apar( float apar[8] );
    void set_akey( gstring& a_key );
    void set_gkey( gstring& g_key );

    void get_apar( float apar[8] );
    void get_akey( gstring& a_key );
    void get_gkey( gstring& g_key );

    char get_acode();
    char get_gcode();

   
private:
   float a_param[8];  // Changed size from 4 to 8 on 22.05.2009 (DK,TW)
   char aqu_code;
   char gas_code;
   gstring aqu_key;
   gstring gas_key;

protected slots:
    virtual void CmHelp();
    virtual void languageChange();
    virtual void CmCheck();

};

#endif // AutoPhaseDialog_included

