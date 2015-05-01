//-------------------------------------------------------------------
// $Id: SetFiltersDialog.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of SetFiltersDialog class
//
// Copyright (C) 2001-2008  S.Dmytriyeva
// Uses  string class (C) A.Rysin 1999
//
// This file is part of the GEM-Selektor GUI library which uses the
// Qt v.4 cross-platform App & UI framework (http://qt.nokia.com)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the terms of GEMS4 Development
// Quality Assurance Licence (GEMS4.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#ifndef SetFiltersDialog_included
#define SetFiltersDialog_included

#include <QDialog>

#include "ui_SetFiltersDialog4.h"
#include "filters_data.h"

class SetFiltersDialog : public QDialog, public Ui::SetFiltersDialogData
{
    elmFilesConfData *el_data;
    setFiltersData *data;
    QTreeWidgetItem* pkern;
    
    Q_OBJECT

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
    virtual void languageChange();
    
public:

    SetFiltersDialog(QWidget* win,
         elmFilesConfData *elm_data,
         setFiltersData *set_data,
         const char * prfName,
         const char* caption = 0 );
    virtual ~SetFiltersDialog();

};

#endif // SetFiltersDialog_included


