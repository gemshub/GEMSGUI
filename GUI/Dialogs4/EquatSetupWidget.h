//-------------------------------------------------------------------
// $Id: EquatSetupWidget.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of EquatSetup class
//
// Copyright (C) 2010  S.Dmytriyeva
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

#ifndef EquatSetupWidget_included
#define EquatSetupWidget_included

#include <QWidget>
#include <QButtonGroup>
#include "ui_EquatSetupWidget4.h"
#include "array.h"
#include "model_w.h"
#include "service.h"

//-----------------------------------------------------------------------
// Structures for EquatSetupWidget

struct equatSetupData
{
   string xName; // Abscissa name
   string yName; // Ordinate name
   string indexName; // Index name
   string abscissaEquat; // Abscissa first object name

   bool useSeveral; // More then one Abscissa is permitted
   TCStringArray abscissaLines;  // Abscissa object names (if more then one Abscissa)

   equatSetupData( const char * axName, const char * ayName,
                   const char * aiName, const char * aitName, bool useMore = false):
       xName(axName), yName(ayName), indexName(aiName), abscissaEquat(aitName), useSeveral(useMore)
   { }

   equatSetupData( equatSetupData& d ):
     xName(d.xName), yName(d.yName), indexName(d.indexName), abscissaEquat(d.abscissaEquat), useSeveral(d.useSeveral)
   {
       for( size_t ii=0; ii<abscissaLines.size(); ii++)
           abscissaLines.push_back( d.abscissaLines[ii] );
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
   { strip( ndxName ); }

};

void GetListsnRT( int nRT, std::vector<pagesSetupData>& wnData, std::vector<pagesSetupData>& scalarsList );


class EquatSetup : public QWidget, public Ui_EquatWidgetForm
{
   Q_OBJECT

   int cPage;
   int cnRT;
   equatSetupData eqData;
   std::vector<pagesSetupData> stData;
   std::vector<pagesSetupData> pgData;
   std::vector<scriptSetupData>  scriptData;

   QList<QListWidget *> pLists;
   bool useCalc;

   TCStringArray namLines;
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
       int nRT, std::vector<pagesSetupData>& wnData, std::vector<pagesSetupData>& scalarsList,
       const char* script = nullptr, const char* aXname = nullptr, const char* aYname = nullptr  );
    virtual ~EquatSetup();

   string getScript() const;
   TCStringArray getNames( string& xName, string& yName ) const;
   void setNames(TCStringArray lst );
   void setXname( const char* name )
   {
      xNam =name;
   }
   void setYname( const char* name )
   {
      yNam =name;
   }

   size_t getScriptLinesNum() const
   {  return   scriptData.size(); }

   size_t getAbscissaNum() const
   {  return   eqData.abscissaLines.size()+1; }

public slots:
    // slot or function ????
    void resetPageList( int newRT,
          std::vector<pagesSetupData>& wnData, std::vector<pagesSetupData>& scalarsList );
    void slotPopupContextMenu(const QPoint &pos);
    void CmCalc();
    void CmAbscissa();
    void CmAbscissaAdd();
};

#endif // EquatSetup_included


