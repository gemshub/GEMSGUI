//-------------------------------------------------------------------
// $Id: AutoPhaseDialog.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of AutoPhaseDialog class
//
// (C) 2003-2008  S.Dmytriyeva
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

#ifndef AutoPhaseDialog_included
#define AutoPhaseDialog_included

#include <QDialog>

#include "ui_AutoPhaseDialog4.h"

class AutoPhaseDialog : public QDialog, public Ui::AutoPhaseDialogData
{
    Q_OBJECT

public slots:
//    void CmCheck();

public:
    AutoPhaseDialog(
         const char* pr_key, char acode, char gcode, QWidget* parent = nullptr);
    virtual ~AutoPhaseDialog();

    void set_apar( float apar[8] );
    void set_akey( std::string& a_key );
    void set_gkey( std::string& g_key );

    void get_apar( float apar[8] );
    void get_akey( std::string& a_key );
    void get_gkey( std::string& g_key );

    char get_acode();
    char get_gcode();

   
private:
   float a_param[8];  // Changed size from 4 to 8 on 22.05.2009 (DK,TW)
   char aqu_code;
   char gas_code;
   std::string aqu_key;
   std::string gas_key;

protected slots:
    virtual void CmHelp();
    virtual void languageChange();
    virtual void CmCheck();

};

#endif // AutoPhaseDialog_included

