//-------------------------------------------------------------------
// $Id: KeyDialog.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of KeyDialog class
//
// Copyright (C) 1996-2008 A.Rysin, S.Dmytriyeva
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

#ifndef KeyDialog_included
#define KeyDialog_included

#include <QDialog>
#include "v_user.h"

namespace Ui {
class KeyDialogData;
}

class KeyDialog : public QDialog
{
    Q_OBJECT

    Ui::KeyDialogData *ui;
    //  TStringArray keyList;
    bool multi;
    std::string keyFilter;
    size_t iRt;
    TCStringArray old_sel;

    void SetList();

protected slots:
    virtual void CmSelectAll();
    virtual void CmClearAll();
    virtual void CmFilter();
    void CmHelp();


public:

    KeyDialog(QWidget* win, size_t irt, const char* key = "*",
              const char* caption = nullptr, bool filter=true);
    KeyDialog(QWidget* win, size_t irt, TCStringArray& sel,
              const char* key = "*", const char* caption = nullptr );

    virtual ~KeyDialog();


    TCStringArray allSelectedKeys();
    std::string getKey();
};

// work with ReacDC&DComp keys lists
class RDKeyDialog : public QDialog
{
    Q_OBJECT

    Ui::KeyDialogData *ui;
    std::string keyFilter;
    QStringList old_sel;
    short NsuT;     // N of surface site types (to set up on remake)
    void SetList();
    QString makeKey( char type, const char *key );

protected slots:
    virtual void CmSelectAll();
    virtual void CmClearAll();
    virtual void CmFilter();
    void CmHelp();

public:

    RDKeyDialog(QWidget* win, TCStringArray& sel,
                const char* key = "*", const char* caption = nullptr, short NsuT=0 );
    virtual ~RDKeyDialog();
    TCStringArray allSelectedKeys();
    //    string getKey();
};


#endif // KeyDialog_included
