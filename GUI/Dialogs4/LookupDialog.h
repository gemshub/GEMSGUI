//-------------------------------------------------------------------
// $Id: SettingsDialog.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of LookupDialog class
//
// Copyright (C) 2009  S.Dmytriyeva
// Uses  string class (C) A.Rysin 1999
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
#ifndef LookupDialog_included
#define LookupDialog_included


#include "ui_LookupDialog4.h"
#include "model_w.h"

class LookupDialog : public QDialog, public Ui::LookupDialogData
{
    Q_OBJECT

     TObjectTable* PTable;
     TObjectTable* TTable;

    void defineTArray();
    void definePArray();
    void initPTable();
    void initTTable();
    void setupPTArrays();


public:

	LookupDialog( QWidget* parent = NULL);
    virtual ~LookupDialog();

    void   setTdata( double Tai[4] );
    void   setPdata( double Pai[4] );

    void   getTdata( double Tai[4] );
    void   getPdata( double Pai[4] );
    void   getFlags( char flgs[6] );

protected slots:
    void CmHelp();
    void CmNext();
    void CmBack();
    void arrayChecked(bool check );
    virtual void languageChange();
    void objectChanged();

};

#endif // LookupDialog_included
