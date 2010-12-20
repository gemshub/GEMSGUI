//-------------------------------------------------------------------
// $Id: RTparmWizard.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of RTparmWizard class
//
// Copyright (C) 2005-2008  S.Dmytriyeva, D.Kulik
//
// This file is part of the GEM-Vizor library which uses the
// Qt v.2.x GUI Toolkit (Troll Tech AS, http://www.trolltech.com)
// according to the Qt Duo Commercial license
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#ifndef RTparmWizard_included
#define RTparmWizard_included

#include <QDialog>
#include "ui_RTparmWizard4.h"
#include "EquatSetupWidget.h"



class RTparmWizard : public QDialog, public Ui::RTparmWizardData
{
    Q_OBJECT

    bool isDC;
    gstring calcScript;
    EquatSetup *pageScript;

    void 	resetNextButton();
    void 	resetBackButton();
    bool PChange();
    bool TChange();

public:

    RTparmWizard( const char* pkey, char flgs[10], int sizes[7], float val[6],
                  const char *acalcScript, const char* aXname, const char* aYname,
                  QWidget* parent = NULL);
    virtual ~RTparmWizard();

    void   getSizes( int size[7] );
    void   getFlags( char flgs[10], gstring& xName );
    void   getFloat( float val[6] );

    gstring getScript() const
    { return pageScript->getScript();
    }
    TCStringArray getNames( gstring& xName, gstring& yName ) const
    { return pageScript->getNames(xName, yName); }

protected slots:
    virtual void languageChange();

protected slots:

    void help();
    void CmNext();
    void CmBack();
    void resetPageList(const char* aXname, const char* aYname);

};


#endif // RTparmWizard_included
