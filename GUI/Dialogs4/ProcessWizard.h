//-------------------------------------------------------------------
// $Id: ProcessWizard.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of ProcessWizard class
//
// Copyright (C) 2005-2007  S.Dmytriyeva
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

#ifndef ProcessWizard_included
#define ProcessWizard_included

#include <QDialog>
#include "ui_ProcessWizard4.h"
#include "EquatSetupWidget.h"


/*!
  \ class TProcesDelegate
  \ individual items in views are rendered and edited using delegates
*/
class TProcesDelegate: public QItemDelegate
{
        Q_OBJECT

public:

         TProcesDelegate( QObject * parent = 0 );
         QWidget *createEditor(QWidget *parent,
                               const QStyleOptionViewItem &option,
                               const QModelIndex &index) const;
};


class ProcessWizard : public QDialog, public Ui::ProcessWizardData
{
    Q_OBJECT

    char curType;
    string calcScript;
    string outScript;
    EquatSetup *pageScript;
    QList<QListWidget *> pLsts;
    QButtonGroup *subTypeButtons;
    vector<pagesSetupData> pgData;

    bool page1Changed;
    bool tItersChanged;

    void 	resetNextButton();
    void 	resetBackButton();

    // internal functions
    char        getType() const;
    QStringList  getSelected( int nI );
    QStringList getSelected( const char *name );
    int         getNPoints( int col );
    void        setupPages();
    int         isUndefined( int col );
    void        setIterColumn( int col, double from, double until, double step );
    void        setIterColumn( int col, int from, int until, int step );

    // functions for page 1 functionality
    void defineWindow( char type );
    int  getNPV( char type, int subtype);            // get number of points
    void  setCalcScript( char type, int subtype );   // set process script
    void  setOutScript( char type, int subtype );    // set output script
    void  setIter( char type, int ); // set up default iterators

public:

    ProcessWizard( const char* pkey, char flgs[24], int sizes[8],
                   short tabInt[6], double tabDoubl[24],
       const char *acalcScript, const char *aoutScript,
       const char* aXname, const char* aYname,  QWidget* parent = NULL);
    virtual ~ProcessWizard();

    void   getSizes( int size[8] );
    void   getTable( short size[6], double dbl[24] );
    void   getFlags( char flgs[24] );

    string getCalcScript() const
    { string res = textEquat1->toPlainText().toUtf8().data();
      return res;
    }
    string getOutScript() const
    { return pageScript->getScript(); }

    vector<string> getNames( string& xName, string& yName ) const
    { return pageScript->getNames(xName, yName); }

protected slots:
    virtual void languageChange();

protected slots:

    void help();
    void CmNext();
    void CmBack();

    void resetPageList(const char* aXname, const char* aYname);
    void setMode(int);
    void CmItersEdit(int , int );
    void CmSetMode();
    void changePage( int );

};

#endif // ProcessWizard_included
