//-------------------------------------------------------------------
// $Id: SystemWizard.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of SystemWizard class
//
// Copyright (C) 2010  S.Dmytriyeva, D.Kulik
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

#ifndef SystemWizard_included
#define SystemWizard_included

#include <QDialog>
#include <QComboBox>
#include "ui_SystemWizard4.h"


class SystemWizard : public QDialog, public Ui::SystemWizardData
{
    Q_OBJECT

    void 	resetNextButton();
    void 	resetBackButton();

public:

    SystemWizard( const char* pkey, char flgs[40],
         std::string name, std::string comment, std::string EQkey,  QWidget* parent = nullptr);
    virtual ~SystemWizard();

    std::string getName();
    std::string getComment();
    std::string getEQkey();

    void   getFlags( char flgs[40] );

protected slots:
    virtual void languageChange();

protected slots:

    void help();
    void CmNext();
    void CmBack();
    
};

#endif // SystemWizard_included
