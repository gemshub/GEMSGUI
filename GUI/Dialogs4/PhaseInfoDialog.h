//-------------------------------------------------------------------
// $Id: PhaseInfoDialog.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of PhaseInfoDialog class
//
// Copyright (C) 2013  S.Dmytriyeva
// Uses  gstring class (C) A.Rysin 1999
//
// This file is part of the GEM-Selektor GUI library which uses the
// Qt v.4 cross-platform App & UI framework (http://qt.nokia.com)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the terms of GEMS3 Development
// Quality Assurance Licence (GEMS3.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#ifndef PhaseInfoDialog_included
#define PhaseInfoDialog_included

#include <vector>
#include <QDialog>
#include <QButtonGroup>

#include "ui_PhaseInfoDialog4.h"
#include <string>
#include <vector>
using namespace std;

class PhaseInfoDialog : public QDialog, public Ui::PhaseInfoDialogData
{
    Q_OBJECT
    
protected slots:
    virtual void languageChange();
    void slotPopupContextMenuDC(const QPoint &pos);
    void slotPopupContextMenuPh(const QPoint &pos);
    void CopyDataDC();
    void CopyDataPh();
    void help();
    
public:
    PhaseInfoDialog(QWidget* wpar, bool system,  int xph, string phname,
                    vector<int>& xdclist, vector<string>& dcnames, int xdc);
    virtual ~PhaseInfoDialog();


 };

#endif // PhaseInfoDialog_included
