//-------------------------------------------------------------------
// $Id: EquatSetupWidget.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of EquatSetup class
//
// Copyright (C) 2010  S.Dmytriyeva
// Uses  gstring class (C) A.Rysin 1999
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

#ifndef EquatSetupWidget_included
#define EquatSetupWidget_included

#include <QWidget>
#include <QButtonGroup>
#include "ui_EquatSetupWidget4.h"
#include "array.h"
#include "model_w.h"

//-----------------------------------------------------------------------
// Structures for EquatSetupWidget

struct equatSetupData
{

   gstring xName; // Abscissa name
   gstring yName; // Ordinate name
   gstring indexName; // Index name
   gstring abscissaEquat; // Abscissa object name

   equatSetupData( const char * axName, const char * ayName,
                   const char * aiName, const char * aitName):
        xName(axName), yName(ayName), indexName(aiName), abscissaEquat(aitName)
   { }

   equatSetupData( equatSetupData& d ):
     xName(d.xName), yName(d.yName), indexName(d.indexName), abscissaEquat(d.abscissaEquat)
   { }
};

struct pagesSetupData
{
   gstring pageName;
   int nObj;         // index of object
   int ndx;         //  second index in object

   pagesSetupData( const char * pName, int aobj, int andx = 0 ):
        pageName(pName),  nObj(aobj), ndx(andx)
   { }

   pagesSetupData( pagesSetupData& d ):
           pageName(d.pageName),  nObj(d.nObj), ndx(d.ndx)
   { }
};

struct scriptSetupData
{
   int nWin;     // page number
   int nObj;     // number of object
   int nIdx;     // line into object
   //double val;
   //char unit;

   gstring objName;
   gstring ndxName;
   gstring lineText;

   scriptSetupData( int aWin, int aobj, const char * oName,
                   int aIdx, const char * ndName,
                   const char * alineText ):
           nWin(aWin), nObj(aobj), nIdx(aIdx),
           objName(oName), ndxName(ndName), lineText(alineText)
   { }

   scriptSetupData( scriptSetupData& d ):
           nWin(d.nWin), nObj(d.nObj), nIdx(d.nIdx),
           objName(d.objName), ndxName(d.ndxName), lineText(d.lineText)
   { }
};

void GetListsnRT( int nRT, TIArray<pagesSetupData>& wnData, TIArray<pagesSetupData>& scalarsList );


class EquatSetup : public QWidget, public Ui_EquatWidgetForm
{
   Q_OBJECT

   int cPage;
   int cnRT;
   equatSetupData eqData;
   TIArray<pagesSetupData> stData;
   TIArray<pagesSetupData> pgData;
   TIArray<scriptSetupData>  scriptData;

   QList<QListWidget *> pLists;
   bool useCalc;

   TCStringArray namLines;

   // internal functions
   int  tableFindRow( int nO, int ndx );
   void tableInsertRow( int nO, int ndx, const char * andName );
   void tableDeleteRow( int row );
   void scriptUpdate( );
   void emptyScriptTable();
   gstring getStringValue( int nO, int ndx, const char * andName );

protected slots:
    virtual void languageChange();
    void changePage( int nPage );
    void changeTable( const QItemSelection & selected, const QItemSelection & deselected );

public:

    EquatSetup( QWidget* parent, equatSetupData aEqData,
       int nRT, TIArray<pagesSetupData>& wnData, TIArray<pagesSetupData>& scalarsList,
       const char* script = 0  );
    virtual ~EquatSetup();

   gstring getScript() const;
   TCStringArray getNames() const;

   int getScriptLinesNum() const
   {  return   scriptData.GetCount(); }

public slots:
    // slot or function ????
    void resetPageList( int newRT,
          TIArray<pagesSetupData>& wnData, TIArray<pagesSetupData>& scalarsList );
    void slotPopupContextMenu(const QPoint &pos);
    void CmCalc();
    void CmAbscissa();

};

#endif // EquatSetup_included


