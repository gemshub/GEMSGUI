//-------------------------------------------------------------------
// $Id: SettingsDialog.h 275 2004-07-06 08:35:54Z gems $
//
// Declaration of SettingsDialog class
//
// Copyright (C) 1996-2007  A.Rysin, S.Dmytriyeva
// Uses  gstring class (C) A.Rysin 1999
//
// This file is part of the GEM-Vizor library which uses the
// Qt v.2.x GUI Toolkit (Troll Tech AS, http://www.trolltech.com)
// according to the Qt Duo Commercial license
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
#include <QWidget>

#include "ui_SettingsDialog4.h"

class SettingsDialog : public QDialog, public Ui::SettingsDialogData
{
    Q_OBJECT

    QFont cellFont;
public:

    SettingsDialog( QWidget* parent = NULL);
    virtual ~SettingsDialog();

protected slots:
   virtual void languageChange();

   virtual void CmChangeFont();
   virtual void CmSysDBDirSelect();
   virtual void CmUserDBDirSelect();
   virtual void CmApply();
   virtual void CmHelp();
   virtual void CmHelpGenerate();
   virtual void CmDefaultFont();
   void accept();
};

#endif // SettingsDialog_included
