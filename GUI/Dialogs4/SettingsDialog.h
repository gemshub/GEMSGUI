//-------------------------------------------------------------------
// $Id: SettingsDialog.h 275 2004-07-06 08:35:54Z gems $
//
// Declaration of SettingsDialog class
//
// Copyright (C) 1996-2007  A.Rysin, S.Dmytriyeva
//
// This file is part of the GEM-Selektor GUI library which uses the
// Qt v.4 cross-platform App & UI framework (https://qt.io/download-open-source)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the GPL v.3 license

//
// See http://les.web.psi.ch/Software/GEMS-PSI/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#ifndef SettingsDialog_included
#define SettingsDialog_included

#include <QFont>
#include <QDialog>


namespace Ui {
class SettingsDialogData;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

    Ui::SettingsDialogData *ui;
    QFont cellFont;

public:

    SettingsDialog( QWidget* parent = nullptr);
    virtual ~SettingsDialog();

protected slots:

    virtual void CmChangeFont();
    virtual void CmSysDBDirSelect();
    virtual void CmUserDBDirSelect();
    virtual void CmApply();
    virtual void CmHelp();
    virtual void CmDefaultFont();
    void accept();
};

#endif // SettingsDialog_included
