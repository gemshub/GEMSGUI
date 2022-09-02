//-------------------------------------------------------------------
// $Id: GtDemoWizard.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of GtDemoWizard class
//
// Copyright (C) 2005-2021  S.Dmytriyeva
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

#ifndef GtDemoWizard_included
#define GtDemoWizard_included

#include <QDialog>
#include "table_model.h"
#include "EquatSetupWidget.h"

namespace Ui {
class GtDemoWizardData;
}


class GtDemoWizard : public QDialog
{
    Q_OBJECT

    Ui::GtDemoWizardData *ui;
    QButtonGroup *allButtons;
    int curentRT;

    int equatRT = -1;
    string script;
    EquatSetup *pageScript;

    KeyModel* process_keys_model = nullptr;
    KeysTableProxy* process_keys_table = nullptr;
    std::string process_old_selection;

    int keysRT = -1;
    KeyModel* sel_keys_model = nullptr;
    KeysTableProxy* sel_keys_table = nullptr;
    TCStringArray sel_old_selection;

protected slots:
    void help();
    void CmNext();
    void CmBack();
    void selChangeFilter();
    void selSelectionChanged(int state);
    void processChangeFilter();
    void processSelectionChanged(int state);

public:

    GtDemoWizard( const string& pkey, char flgs[16], int sizes[8], const string& ascript,
                  const string& proc_key, const string& aXname, const string& aYname,
                  TCStringArray& keys, QWidget* parent = nullptr );
    virtual ~GtDemoWizard();

    void   getFlags( char flgs[16] );
    void   getSizes( int size[8], std::string& prKey, TCStringArray& keys );
    string getScript() const
    { return pageScript->getScript(); }
    TCStringArray getNames( string& xName, string& yName ) const
    { return pageScript->getNames(xName, yName); }

protected:

    void  resetNextButton();
    void  resetBackButton();
    int   get_nRT();
    void reset_page_list( int newRT, const char* aXname=nullptr, const char* aYname=nullptr );
    void reset_process_list( int newRT );
    void reset_keys_list(int newRT);
    void alloc_keys_model(int cRT);

    std::string get_process_key();
};

#endif // GtDemoWizard_included
