//-------------------------------------------------------------------
// Id: gems/vizor/dlg/SetFiltersDialog.h  version 2.0.0    2001
//
// Declaration of SetFiltersDialog class
//
// Created : 180102
//
// Copyright (C) 2001  S.Dmytriyeva
// Uses  gstring class (C) A.Rysin 1999
//
// This file is part of the GEM-Vizor library which uses the
// Qt v.2.x GUI Toolkit (Troll Tech AS, http://www.trolltech.com)
// according to the Qt Duo Commercial license #1435515
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL)
//
// See http://les.web.psi.ch/Software/GEMS-PSI/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#ifndef SetFiltersDialog_included
#define SetFiltersDialog_included

#include "SetFiltersDialogData.h"
#include "filters_data.h"

class SetFiltersDialog : public SetFiltersDialogData
{
    elmFilesConfData *el_data;
    setFiltersData *data;
    QListViewItem* pkern;
    
    Q_OBJECT


    void setData();
    void getData();
    void getFiles();
    void setFiles();


protected slots:
    void CmHelp();
//    void CmCancel();
    void CmReset();
    void CmApply();
    void CmOk();
    void CmChangeFilesCfg();
    
public:

    SetFiltersDialog(QWidget* win,
         elmFilesConfData *elm_data,
         setFiltersData *set_data,
         const char * prfName,
         const char* caption = 0 );
    virtual ~SetFiltersDialog();

};

#endif // SetFiltersDialog_included


