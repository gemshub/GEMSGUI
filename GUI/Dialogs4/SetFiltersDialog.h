//-------------------------------------------------------------------
// $Id: SetFiltersDialog.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of SetFiltersDialog class
//
// Copyright (C) 2001-2008  S.Dmytriyeva
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

#ifndef SetFiltersDialog_included
#define SetFiltersDialog_included

#include <QDialog>
#include <QTreeWidgetItem>
#include "filters_data.h"

namespace Ui {
class SetFiltersDialogData;
}

class SetFiltersDialog : public QDialog
{

    Q_OBJECT

    Ui::SetFiltersDialogData *ui;
    elmFilesConfData *el_data;
    setFiltersData *data;
    QTreeWidgetItem* pkern;

    void setData();
    void getData();
    void getFiles();
    void setFiles();


protected slots:
    void CmHelp();
    void CmReset();
    void CmApply();
    void CmOk();
    void CmChangeFilesCfg();
    
public:

    SetFiltersDialog(QWidget* win,
                     elmFilesConfData *elm_data,
                     setFiltersData *set_data,
                     const char * prfName,
                     const char* caption = nullptr );
    virtual ~SetFiltersDialog();

};

#endif // SetFiltersDialog_included


