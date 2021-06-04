//-------------------------------------------------------------------
// $Id: PhaseWizard.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of PhaseWizard class
//
// Copyright (C) 2005-2021  S.Dmytriyeva, D.Kulik
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

#ifndef PhaseWizard_included
#define PhaseWizard_included

#include <QDialog>
#include "table_model.h"

namespace Ui {
class PhaseWizardData;
}

class PhaseWizard : public QDialog
{
    Q_OBJECT

    Ui::PhaseWizardData *ui;
    RDKeyModel* rd_keys_model = nullptr;
    KeysTableProxy* rd_keys_table = nullptr;
    TCStringArray rd_old_selection;
    size_t old_nDC = 0;

    KeyModel* phase_keys_model = nullptr;
    KeysTableProxy* phase_keys_table = nullptr;
    TCStringArray phase_old_selection;

    RDKeyModel* lDCr_keys_model = nullptr;
    KeysTableProxy* lDCr_keys_table = nullptr;
    TCStringArray lDCr_old_selection;

    int ndxInPhPs_0;
    void 	resetNextButton();
    void 	resetBackButton();

public:

    PhaseWizard( const char* pkey, char flgs[37], int size[30],
                 double r2, const TCStringArray& rd_sel,
                 const TCStringArray& phase_sel, const TCStringArray& lDCr_sel,
                 QWidget* parent = nullptr);
    virtual ~PhaseWizard();

    void   getSizes( int size[30], TCStringArray& rd_sel,
                     TCStringArray& phase_sel, TCStringArray& lDCr_sel );
    void   getFlags( char flgs[37] );
    double   getR2();


protected slots:

    void help();
    void CmNext();
    void CmBack();
    
    void rdChangeFilter();
    void rdSelectionChanged(int state);
    void phaseChangeFilter();
    void phaseSelectionChanged(int state);
    void lDCrChangeFilter();
    void lDCrSelectionChanged(int state);

};

#endif // PhaseWizard_included


