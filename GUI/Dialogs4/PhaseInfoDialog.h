//-------------------------------------------------------------------
// $Id: PhaseInfoDialog.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of PhaseInfoDialog class
//
// Copyright (C) 2013  S.Dmytriyeva
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

#ifndef PhaseInfoDialog_included
#define PhaseInfoDialog_included

#include <vector>
#include <QDialog>

namespace Ui {
class PhaseInfoDialogData;
}

class PhaseInfoDialog : public QDialog
{
    Q_OBJECT
    
    Ui::PhaseInfoDialogData *ui;

protected slots:
    void slotPopupContextMenuDC(const QPoint &pos);
    void slotPopupContextMenuPh(const QPoint &pos);
    void CopyDataDC();
    void CopyDataPh();
    void help();
    
public:

    PhaseInfoDialog(QWidget* wpar, bool system,  int xph, std::string phname,
                    std::vector<int>& xdclist, std::vector<std::string>& dcnames, int xdc);
    virtual ~PhaseInfoDialog();
};

#endif // PhaseInfoDialog_included
