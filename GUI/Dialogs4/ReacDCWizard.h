//-------------------------------------------------------------------
// $Id: ReacDCWizard.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of ReacDCWizard class
//
// Copyright (C) 2005-2008  S.Dmytriyeva, D.Kulik
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

#ifndef ReacDCWizard_included
#define ReacDCWizard_included

#include <QDialog>
#include "table_model.h"

namespace Ui {
class ReacDCWizardData;
}

class ReacDCWizard : public QDialog
{
    Q_OBJECT

    Ui::ReacDCWizardData *ui;
    RDKeyModel* keys_model = nullptr;
    KeysTableProxy* tableKeys = nullptr;
    TCStringArray old_selection;

    void 	resetNextButton();
    void 	resetBackButton();

public:

    ReacDCWizard( const char* pkey, char flgs[12], int size[4],
    const TCStringArray& sel, QWidget* parent = nullptr);
    virtual ~ReacDCWizard();

    void   getSizes( int size[4] );
    void   getFlags( char flgs[12] );
    TCStringArray  allSelectedKeys() const;
    //    double   getR2();

protected slots:

    void help();
    void CmNext();
    void CmBack();

    void ChangeFilter();
    void selectionChanged(int state);
};


#endif // ReacDCWizard_included


