//-------------------------------------------------------------------
// $Id: filters_data.h 968 2007-12-13 13:23:32Z gems $
//
// Internal declarations
//
// Copyright (C) 2002 S.Dmytriyeva
// Uses  gstring class (C) A.Rysin 1999
//
// This file is part of a GEM-Selektor library for thermodynamic
// modelling by Gibbs energy minimization
// Uses: GEM-Selektor GUI GUI DBMS library, gems/lib/gemvizor.lib
//
// This file may be distributed under the terms of GEMS3 Development
// Quality Assurance Licence (GEMS3.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail: gems2.support@psi.ch
//-------------------------------------------------------------------

#ifndef filters_data_included
#define filters_data_included

#include "array.h"
#include "v_user.h"

//-----------------------------------------------------------------------
// Structures for SetFiltersDialog

enum selectType { cbAqueous_ = 0,
                  cbGaseous_,
                  cbSorption_,
                  cbIsotopes_,
                 cbSolids_,
                 cbSolutions_
                 };

struct elmWindowData
{
   TCStringArray ICrds;   // list of selected IComp
   TCStringArray oldIComps; // list from parent project
//   TCStringArray flNames; // kernel, uncertain, specific

   bool flags[6];         // selectType

   elmWindowData()
   {
     flags[0] = true;
     flags[1] = true;
     flags[2] = false;
     flags[3] = false;
     flags[4] = false;
     flags[5] = false;
   }

  elmWindowData( elmWindowData& d )
  {
    uint ii;
    for( ii=0; ii<6; ii++ )
     flags[ii] = d.flags[ii];
    for( ii=0; ii<d.ICrds.GetCount(); ii++ )
     ICrds.Add(d.ICrds[ii]);
    for( ii=0; ii<d.oldIComps.GetCount(); ii++ )
     oldIComps.Add(d.oldIComps[ii]);
//    for( ii=0; ii<d.flNames.GetCount(); ii++ )
//     flNames.Add(d.flNames[ii]);
  }

  const elmWindowData& operator=( const elmWindowData& d)
  {
    uint ii;
    for( ii=0; ii<6; ii++ )
     flags[ii] = d.flags[ii];
    for( ii=0; ii<d.ICrds.GetCount(); ii++ )
     ICrds.Add(d.ICrds[ii]);
    for( ii=0; ii<d.oldIComps.GetCount(); ii++ )
     oldIComps.Add(d.oldIComps[ii]);
//    for( ii=0; ii<d.flNames.GetCount(); ii++ )
//     flNames.Add(d.flNames[ii]);

    return *this;
  }

};

struct elmFilesConfData
{
   // only for modules  RT_SDATA -- RT_PHASE
   //( RT_SDATA, RT_CONST, X, X, RT_ICOMP, RT_DCOMP, RT_COMPOS,
   //   RT_REACDC, X, RT_PHASE )
   bool changed;
   TCStringArray flKeywds;   // list of files in DefaultDB (keywds)
   TCStringArray flNames;    // list of files in DefaultDB (names)
   TCIntArray    flCnt;      // count of files  for type

   TCStringArray selKeywds;   // list of selected files
   TCIntArray    selCnt;      // count of selected files  for type

   elmFilesConfData():
   changed(false)
   {

   }
};


struct icSetupData
{
   gstring from_templ;
   gstring to_templ;

   TCStringArray oldIComps; // list from parent project
   TCStringArray newIComps; // list from default database
//   TCStringArray flKeywds;   // list files to open

   icSetupData():
   from_templ("*"), to_templ("*_")
   {}

   icSetupData( icSetupData& d ):
    from_templ( d.from_templ), to_templ(d.to_templ)
  {
    uint ii;
    for( ii=0; ii<d.oldIComps.GetCount(); ii++ )
     oldIComps.Add(d.oldIComps[ii]);
    for( ii=0; ii<d.newIComps.GetCount(); ii++ )
     newIComps.Add(d.newIComps[ii]);
//    for( ii=0; ii<d.flKeywds.GetCount(); ii++ )
//     flKeywds.Add(d.flKeywds[ii]);
  }

  const icSetupData& operator=( const icSetupData& d)
  {
    uint ii;
    from_templ =  d.from_templ;
    to_templ =    d.to_templ;

    for( ii=0; ii<d.oldIComps.GetCount(); ii++ )
     oldIComps.Add(d.oldIComps[ii]);
    for( ii=0; ii<d.newIComps.GetCount(); ii++ )
     newIComps.Add(d.newIComps[ii]);
//    for( ii=0; ii<d.flKeywds.GetCount(); ii++ )
//     flKeywds.Add(d.flKeywds[ii]);

    return *this;
  }

};

struct dcSetupData
{
   gstring from_templ;
   gstring to_templ;
   gstring f_script;

//   TCStringArray flKeywds;   // list files to open

   dcSetupData():
   from_templ("*?"), to_templ("*_"), f_script("")
   {}

   dcSetupData( dcSetupData& d ):
    from_templ( d.from_templ), to_templ(d.to_templ), f_script(d.f_script)
  {
//    uint ii;
//    for( ii=0; ii<d.flKeywds.GetCount(); ii++ )
//     flKeywds.Add(d.flKeywds[ii]);
  }

  const dcSetupData& operator=( const dcSetupData& d)
  {
 //   uint ii;
    from_templ =  d.from_templ;
    to_templ   =  d.to_templ;
    f_script   =  d.f_script;

//    for( ii=0; ii<d.flKeywds.GetCount(); ii++ )
//     flKeywds.Add(d.flKeywds[ii]);

    return *this;
  }

};


struct rdSetupData
{
   gstring from_templ;
   gstring to_templ;
   gstring f_script;

//   TCStringArray flKeywds;   // list files to open

   rdSetupData():
   from_templ("*?"), to_templ("*_"), f_script("")
   {}

   rdSetupData( rdSetupData& d ):
    from_templ( d.from_templ), to_templ(d.to_templ), f_script(d.f_script)
  {
//    uint ii;
//    for( ii=0; ii<d.flKeywds.GetCount(); ii++ )
//     flKeywds.Add(d.flKeywds[ii]);
  }

  const rdSetupData& operator=( const rdSetupData& d)
  {
//    uint ii;
    from_templ =  d.from_templ;
    to_templ   =  d.to_templ;
    f_script   =  d.f_script;

//    for( ii=0; ii<d.flKeywds.GetCount(); ii++ )
//     flKeywds.Add(d.flKeywds[ii]);

    return *this;
  }

};


struct cmSetupData
{
   gstring from_templ;
   gstring to_templ;
   gstring f_script;

//   TCStringArray flKeywds;   // list files to open

   cmSetupData():
   from_templ("*"), to_templ("*_"), f_script("")
   {}

   cmSetupData( cmSetupData& d ):
    from_templ( d.from_templ), to_templ(d.to_templ), f_script(d.f_script)
  {
//    uint ii;
//    for( ii=0; ii<d.flKeywds.GetCount(); ii++ )
//     flKeywds.Add(d.flKeywds[ii]);
  }

  const cmSetupData& operator=( const cmSetupData& d)
  {
//    uint ii;
    from_templ =  d.from_templ;
    to_templ   =  d.to_templ;
    f_script   =  d.f_script;

//    for( ii=0; ii<d.flKeywds.GetCount(); ii++ )
//     flKeywds.Add(d.flKeywds[ii]);

    return *this;
  }

};

// Checkboxes in Phases page of SetFiltersDialog
enum copyFlagX { PHcopyL_ = 0,
                 PHcopyF_,
                 PHcopyD_,
                 PHcopyN_,
                 PHcopyA_,
                 PHcopyY_
                 };

struct phSetupData
{
   gstring from_templ;
   gstring to_templ;

   bool flags[6];

//   TCStringArray flKeywds;   // list files to open

   phSetupData():
   from_templ("*"), to_templ("*_")
   {
     flags[0] =true;
     flags[1] =false;// SD 20/01/2010 true;
     flags[2] =true;
     flags[3] =true;
     flags[4] =true; // SD 20/01/2010 false;
     flags[5] =false;
   }

   phSetupData( phSetupData& d ):
    from_templ( d.from_templ), to_templ(d.to_templ)
  {
    uint ii;
    for( ii=0; ii<6; ii++ )
     flags[ii] = d.flags[ii];
//    for( ii=0; ii<d.flKeywds.GetCount(); ii++ )
//     flKeywds.Add(d.flKeywds[ii]);
  }

  const phSetupData& operator=( const phSetupData& d)
  {
    uint ii;
    from_templ =  d.from_templ;
    to_templ   =  d.to_templ;

    for( ii=0; ii<6; ii++ )
     flags[ii] = d.flags[ii];
//    for( ii=0; ii<d.flKeywds.GetCount(); ii++ )
//     flKeywds.Add(d.flKeywds[ii]);

    return *this;
  }


};

struct setFiltersData
{
 icSetupData ic_d;
 dcSetupData dc_d;
 rdSetupData rd_d;
 phSetupData ph_d;
 cmSetupData cm_d;

  setFiltersData()
  {
  }

 setFiltersData( setFiltersData& d ):
    ic_d( d.ic_d),  dc_d( d.dc_d), rd_d( d.rd_d),
    ph_d( d.ph_d),  cm_d( d.cm_d)
  {
  }

  const setFiltersData& operator=( const setFiltersData& d)
  {
    ic_d = d.ic_d;
    dc_d = d.dc_d;
    rd_d = d.rd_d;
    ph_d = d.ph_d;
    cm_d = d.cm_d;

    return *this;
  }

};


//-----------------------------------------------------------------------
// Structures for InputSystemDialog

struct tableSetupData
{
   int nWin;     // page number
   int nObj;     // number of object
   int nIdx;     // line into object
   double val;
   char unit;

   gstring objName;
   gstring ndxName;

   tableSetupData( int aWin, int aobj, const char * oName,
                   int aIdx, const char * ndName,
                   double aVal, char aUnit ):
           nWin(aWin), nObj(aobj), nIdx(aIdx), val(aVal), unit (aUnit),
           objName(oName), ndxName (ndName)
   { }

   tableSetupData( tableSetupData& d ):
           nWin(d.nWin), nObj(d.nObj), nIdx(d.nIdx), val(d.val), unit(d.unit),
           objName(d.objName), ndxName(d.ndxName)
   {
       // cout << "nWin = " << nWin << " nObj = " << nObj << " nIdx = " << nIdx << endl;
   }

};


struct windowSetupData
{
   gstring pageName;
   int nObj;       // index of object
   int nOunit;     // index of object with units
   int nSwitch;    // index of object with switches
   int unitLine;   // index of units line in units.ini
   double defVal;  // default value
   char defUnit;   // default units

   windowSetupData( const char * pName, int aobj, int aUnt,
            int uObj, int sObj, double adefVal, char adefUnit ):
        pageName(pName),  nObj(aobj), nOunit(uObj), nSwitch(sObj),
        unitLine(aUnt), defVal( adefVal), defUnit(adefUnit)
   { }

   windowSetupData( windowSetupData& d ):
           pageName(d.pageName),  nObj(d.nObj), nOunit(d.nOunit),  nSwitch(d.nSwitch),
           unitLine(d.unitLine), defVal( d.defVal), defUnit(d.defUnit)
   {}

};


#endif // calc_filters_included