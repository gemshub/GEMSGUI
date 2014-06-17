//-------------------------------------------------------------------
// $Id: KeyDialog.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of KeyDialog class
//
// Copyright (C) 1996-2008 A.Rysin, S.Dmytriyeva
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

#ifndef KeyDialog_included
#define KeyDialog_included

#include <QDialog>

#include "ui_KeyDialog4.h"
#include "gstring.h"


class KeyDialog : public QDialog, public Ui::KeyDialogData
{
    Q_OBJECT

    //  TStringArray keyList;
    bool multi;
    gstring keyFilter;
    int iRt;
    TCStringArray old_sel;

    void SetList();

protected slots:
    virtual void CmSelectAll();
    virtual void CmClearAll();
    virtual void languageChange();
    virtual void CmFilter();
    void CmHelp();


public:

    KeyDialog(QWidget* win, int irt, const char* key = "*",
              const char* caption = 0, bool filter=true);
    KeyDialog(QWidget* win, int irt, TCStringArray& sel,
              const char* key = "*", const char* caption = 0 );

    virtual ~KeyDialog();


    TCStringArray allSelectedKeys();
    gstring getKey();
};

// work with ReacDC&DComp keys lists
class RDKeyDialog : public QDialog, public Ui::KeyDialogData
{
    Q_OBJECT

    gstring keyFilter;
    QStringList old_sel;
    short NsiT;     // N of surface site types (to set up on remake)
    void SetList();
    QString makeKey( char type, const char *key );

protected slots:
    virtual void CmSelectAll();
    virtual void CmClearAll();
    virtual void languageChange();
    virtual void CmFilter();
    void CmHelp();

public:

    RDKeyDialog(QWidget* win, TCStringArray& sel,
              const char* key = "*", const char* caption = 0, short NsiT=0 );
    virtual ~RDKeyDialog();
    TCStringArray allSelectedKeys();
//    gstring getKey();
};


#endif // KeyDialog_included
