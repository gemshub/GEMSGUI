//-------------------------------------------------------------------
// $Id: EquatSetupWidget.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of EquatSetup class
//
// Copyright (C) 2010  S.Dmytriyeva
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

#ifndef EquatSetupWidget_included
#define EquatSetupWidget_included

#include <QWidget>
#include <QButtonGroup>
#include "ui_EquatSetupWidget4.h"
#include "verror.h"
#include "model_w.h"

//-----------------------------------------------------------------------
// Structures for EquatSetupWidget

struct equatSetupData
{
   string xName; // Abscissa name
   string yName; // Ordinate name
   string indexName; // Index name
   string abscissaEquat; // Abscissa first object name

   bool useSeveral; // More then one Abscissa is permitted
   vector<string> abscissaLines;  // Abscissa object names (if more then one Abscissa)

   equatSetupData( const char * axName, const char * ayName,
                   const char * aiName, const char * aitName, bool useMore = false):
       xName(axName), yName(ayName), indexName(aiName),
       abscissaEquat(aitName), useSeveral(useMore)
   { }

   equatSetupData( equatSetupData& d ):
     xName(d.xName), yName(d.yName), indexName(d.indexName),
       abscissaEquat(d.abscissaEquat), useSeveral(d.useSeveral)
   {
       for( int ii=0; ii<abscissaLines.size(); ii++)
           abscissaLines.push_back( d.abscissaLines[ii] );
   }
};

struct pagesSetupData
{
   string pageName;
   int nObj;         // index of object
   int ndx;         //  second index in object
   string ndxName; // if ndxName not empty ( ndx is first ndx ), second from list

   pagesSetupData( const char * pName, int aobj, int andx = 0,
                   const char*  andxName = 0):
        pageName(pName),  nObj(aobj), ndx(andx)
   {
     if( andxName )
     {
         ndxName = andxName;
         pageName += "(";
         pageName += ndxName;
         pageName += ")";
     }
     else
       ndxName = "";
   }

   pagesSetupData( const pagesSetupData& d ):
     pageName(d.pageName),  nObj(d.nObj), ndx(d.ndx), ndxName(d.ndxName)
   {

   }
};

struct scriptSetupData
{
   int nWin;     // page number
   int nObj;     // number of object
   int nIdx;     // line into object
   //double val;
   //char unit;

   string objName;
   string ndxName;
   string lineText;

   scriptSetupData( int aWin, int aobj, const char * oName,
                   int aIdx, const char * ndName,
                   const char * alineText ):
           nWin(aWin), nObj(aobj), nIdx(aIdx),
           objName(oName), ndxName(ndName), lineText(alineText)
   { strip(ndxName); }

   scriptSetupData( const scriptSetupData& d ):
           nWin(d.nWin), nObj(d.nObj), nIdx(d.nIdx),
           objName(d.objName), ndxName(d.ndxName), lineText(d.lineText)
   { }
};

void GetListsnRT( int nRT, vector<pagesSetupData>& wnData,
                  vector<pagesSetupData>& scalarsList );


class EquatSetup : public QWidget, public Ui_EquatWidgetForm
{
   Q_OBJECT

   int cPage;
   int cnRT;
   equatSetupData eqData;
   vector<pagesSetupData> stData;
   vector<pagesSetupData> pgData;
   vector<scriptSetupData>  scriptData;

   QList<QListWidget *> pLists;
   bool useCalc;

   vector<string> namLines;
   string xNam;
   string yNam;


   // internal functions
   int  tableFindRow( int nO, int ndx );
   void tableInsertRow( int nO, int ndx, const char * andName );
   void tableDeleteRow( int row );
   void scriptUpdate( );
   void emptyScriptTable();
   string getStringValue( int nO, int ndx, const char * andName );

protected slots:
    virtual void languageChange();
    void changePage( int nPage );
    void changeTable( const QItemSelection & selected, const QItemSelection & deselected );

public:

    EquatSetup( QWidget* parent, equatSetupData aEqData,
       int nRT, const vector<pagesSetupData>& wnData,
       const vector<pagesSetupData>& scalarsList, const char* script = 0,
       const char* aXname = 0, const char* aYname = 0  );
    virtual ~EquatSetup();

   string getScript() const;
   vector<string> getNames( string& xName, string& yName ) const;
   void setNames(const vector<string>& lst );
   void setXname( const char* name )
   {
      xNam =name;
   }
   void setYname( const char* name )
   {
      yNam =name;
   }

   int getScriptLinesNum() const
   {  return   scriptData.size(); }

   int getAbscissaNum() const
   {  return   eqData.abscissaLines.size()+1; }

public slots:
    // slot or function ????
    void resetPageList( int newRT,
          const vector<pagesSetupData>& wnData,
          const vector<pagesSetupData>& scalarsList );
    void slotPopupContextMenu(const QPoint &pos);
    void CmCalc();
    void CmAbscissa();
    void CmAbscissaAdd();
};

#endif // EquatSetup_included


