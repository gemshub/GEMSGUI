//-------------------------------------------------------------------
// $Id: RTparmWizard.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of ProjectWizard class
//
// Copyright (C) 2009  S.Dmytriyeva, D.Kulik
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

#ifndef ProjectWizard_included
#define ProjectWizard_included

#include <QDialog>

namespace Ui {
class ProjectWizardData;
}


class ProjectWizard : public QDialog
{
    Q_OBJECT

    Ui::ProjectWizardData *ui;
    void 	resetNextButton();
    void 	resetBackButton();

public:
    // flgs 0-9 MSpmv, 10-29 TPptv, 30-33 TPun, 34-37  TPsv
    ProjectWizard( const char* pkey, char flgs[38],  QWidget* parent = nullptr);
    virtual ~ProjectWizard();

    int get_Settings();
    void   getFlags( char flgs[38] );

protected slots:

    void help();
    void CmNext();
    void CmBack();
    
};

#endif // ProjectWizard_included
