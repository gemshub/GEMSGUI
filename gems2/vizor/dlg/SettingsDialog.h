//-------------------------------------------------------------------
// Id: gems/vizor/dlg/SettingsDialog.h  version 2.0.0    2001
//
// Declaration of SettingsDialog class
//
// Created : 970207    Modified: 010908
//
// Copyright (C) 1996-2001  A.Rysin, S.Dmytriyeva
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

#ifndef SettingsDialog_included
#define SettingsDialog_included

#include <qfont.h>

#include "SettingsDialogData.h"

class SettingsDialog : public SettingsDialogData
{
    Q_OBJECT

    QFont cellFont;
public:

    SettingsDialog( QWidget* parent = NULL);
    virtual ~SettingsDialog();

protected slots:
    void CmApply();
    void CmHelp();
//    void CmUserDBDirSelect();
//    void CmSysDBDirSelect();
    void CmChangeFont();
    void accept();
};

#endif // SettingsDialog_included
