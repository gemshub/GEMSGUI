//-------------------------------------------------------------------
// $Id: KeyFilter.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of KeyFilter class
//
// Copyright (C) 1996-2008  A.Rysin, S.Dmytriyeva
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

#ifndef KeyFilter_h_
#define KeyFilter_h_
#include <memory>
#include <QDialog>
#include <QLineEdit>
#include "v_user.h"

//=============================================
// KeyEdit dialog
//=============================================

class KeyFilter:   public QDialog
{
    Q_OBJECT

    std::vector<std::shared_ptr<QLineEdit>> aEdit;
    QLineEdit *fullKey;

    size_t iRt;
    //   int iEd;
    bool allowTemplates;

protected slots:
    void EvSetAll();
    void EvGetList();
    void CmOk();
    void CmHelp();
    void setKeyLine();

protected:
    std::string SetKeyString();

public:
    KeyFilter(QWidget* win, size_t iRt, const char* keyFilter,
              const char* caption, bool allowTemplates=true );

    std::string getFilter();
};

#endif   // _KeyFilter_h_
