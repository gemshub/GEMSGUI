//-------------------------------------------------------------------
// $Id: filters_data.h 968 2007-12-13 13:23:32Z gems $
//
// Internal declarations
//
// Copyright (C) 2002 S.Dmytriyeva
// Uses  string class (C) A.Rysin 1999
//
// This file is part of a GEM-Selektor library for thermodynamic
// modelling by Gibbs energy minimization
// Uses: GEM-Selektor GUI GUI DBMS library, gems/lib/gemvizor.lib
//
// This file may be distributed under the terms of GEMS4 Development
// Quality Assurance Licence (GEMS4.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail: gems2.support@psi.ch
//-------------------------------------------------------------------

#ifndef filters_data_included
#define filters_data_included

#include <vector>
#include "verror.h"
#include "v_user.h"

//-----------------------------------------------------------------------
// Structures for SetFiltersDialog


enum selectType{
    cbAqueous_ = 0,   //  PH_AQUEL = 'a', aqueous electrolyte
    cbGaseous_,       //  PH_GASMIX = 'g', mixture of gases
    cbFluid_,         //  PH_FLUID = 'f', fluid phase
    cbPlasma_,        //  PH_PLASMA = 'p', plasma
    cbSolids_,        //  PH_SINCOND = 's', condenced solid phase, also multicomponent
    cbSindis_,        //  PH_SINDIS = 'd',  dispersed solid phase, also multicomponent
    cbLiquid_,        //  PH_LIQUID = 'l', non-electrolyte liquid (melt)
    cbSimelt_,        //  PH_SIMELT = 'm', silicate (magmatic) melt or non-aqueous electrolyte
    cbSorption_,      //  PH_SORPTION = 'x', dilspersed solid with adsorption (ion exchange) in aqueous
    cbPolyel_,        //  PH_POLYEL = 'y',  colloidal poly- (oligo)electrolyte
    cbHcarbl_,        //  PH_HCARBL = 'h'   mixture of condensed hydrocarbons
    cbSolutions_,     // Check to skip non-aqueous condensed multi-component (solution) Phases
    cbIsotopes_,      // Isotopes x isotopic compounds
    cbRes_
};


struct elmWindowData
{
   vector<string> ICrds;   // list of selected IComp
   vector<string> oldIComps; // list from parent project
   string aSelNames;  // names of selected grups of files
   //   vector<string> flNames; // kernel, uncertain, specific

   bool flags[14];         // selectType

   elmWindowData()
   {
     flags[cbAqueous_] = true;
     flags[cbGaseous_] = true;
     flags[cbFluid_] = true;
     flags[cbPlasma_] = false;
     flags[cbSolids_] = true;
     flags[cbSindis_] = true;
     flags[cbLiquid_] = true;
     flags[cbSimelt_] = false;
     flags[cbSorption_] = false;
     flags[cbPolyel_] = false;
     flags[cbHcarbl_] = false;
     flags[cbSolutions_] = false;
     flags[cbIsotopes_] = false;
     flags[cbRes_] = false;
     aSelNames = "";
   }

  elmWindowData( const elmWindowData& d )
  {
    int ii;
    for( ii=0; ii<14; ii++ )
     flags[ii] = d.flags[ii];
    for( ii=0; ii<d.ICrds.size(); ii++ )
     ICrds.push_back(d.ICrds[ii]);
    for( ii=0; ii<d.oldIComps.size(); ii++ )
     oldIComps.push_back(d.oldIComps[ii]);
    aSelNames = d.aSelNames;
//    for( ii=0; ii<d.flNames.GetCount(); ii++ )
//     flNames.Add(d.flNames[ii]);
  }

  const elmWindowData& operator=( const elmWindowData& d)
  {
    int ii;
    for( ii=0; ii<14; ii++ )
     flags[ii] = d.flags[ii];
    for( ii=0; ii<d.ICrds.size(); ii++ )
     ICrds.push_back(d.ICrds[ii]);
    for( ii=0; ii<d.oldIComps.size(); ii++ )
     oldIComps.push_back(d.oldIComps[ii]);
    aSelNames = d.aSelNames;
//    for( ii=0; ii<d.flNames.GetCount(); ii++ )
//     flNames.Add(d.flNames[ii]);

    return *this;
  }

  void setFlags( string strBuf)  //"<TDBflags> = "
  {
      size_t  pos1 =  strBuf.find( "<TDBflags> = " ); //13
      if( pos1 != string::npos )
      {
         pos1 += 13;
         size_t pos2 = strBuf.find( ";", pos1 );
         if( pos2-pos1 >= 14 )
           for(int ii=0; ii<14; ii++ )
             flags[ii] = (strBuf[ii+pos1] =='+');
      }
  }

  string getFlags()
  {
      string strBuf =  "<TDBflags> = "; //+13
      for(int ii=0; ii<14; ii++ )
          if( flags[ii] )
              strBuf +='+';
          else
              strBuf +='-';
      strBuf += ";\n";
      return strBuf;
   }

};

//class FileTree;
struct FileTree
{
  string keywd;   ///< Directori name
  string version; ///< Version name
  int fnum;       ///< index file in files list ( only for EJDB mode)
  vector<FileTree *> lst;

  FileTree( string& key, string& vers, int afnum = -1 ):
      keywd(key), version(vers), fnum(afnum)
  {}

  void deleteTag( string& aTag )
  {
     if( lst.size() == 1 &&  aTag == lst[0]->keywd )
           lst.pop_back();
      else
      {
          for(int jj=0; jj<lst.size(); jj++ )
              lst[jj]->deleteTag( aTag );
      }
  }

  void insertTag( string& fname, const string& vers, char skipSmb, int afnum = -1 )
  {
      FileTree* childRow;
      string aVer, tag;
      int jj;

      size_t pos3, pos2 = fname.find(skipSmb, 1);
      if( pos2 != string::npos )
      {
        tag = fname.substr(1, pos2-1);
        childRow = 0;
        pos3 = fname.find(skipSmb, pos2+1);
        if( pos3 != string::npos)
            aVer = "";
        else aVer = vers.c_str();

        // test used tag before
        for( jj=0; jj < lst.size(); jj++ )
        {
            if( tag == lst[jj]->keywd )
            {
                childRow = lst[jj];
                break;
            }
        }
        if( !childRow )
        {
          childRow = new FileTree( tag, aVer, afnum );
          lst.push_back( childRow );
        }
        fname = fname.substr( pos2);
        childRow->insertTag( fname, vers,skipSmb, afnum );
     }
     else
     {
        tag = ".";
        if( lst.size()<1 ||  tag != lst[0]->keywd )
           {  childRow = new FileTree( tag, aVer, afnum );
              lst.insert( lst.begin(), childRow );
           }
     }

   }
};

struct elmFilesConfData
{
   // only for modules  RT_SDATA -- RT_PHASE
   //( RT_SDATA, RT_CONST, X, X, RT_ICOMP, RT_DCOMP, RT_COMPOS,
   //   RT_REACDC, X, RT_PHASE )
   bool changed;
   vector<string> flKeywds;   // list of files in DefaultDB (keywds)
   vector<string> flNames;    // list of files in DefaultDB (names)
   vector<int>    flCnt;      // count of files  for type

   vector<string> selKeywds;   // list of selected files
   vector<int>    selCnt;      // count of selected files  for type

   FileTree * ftree;

   elmFilesConfData():
       changed(false), ftree(0)
   {

   }
};


struct icSetupData
{
   string from_templ;
   string to_templ;

   vector<string> oldIComps; // list from parent project
   vector<string> newIComps; // list from default database
//   vector<string> flKeywds;   // list files to open

   icSetupData():
   from_templ("*"), to_templ("*_")
   {}

   icSetupData( const icSetupData& d ):
    from_templ( d.from_templ), to_templ(d.to_templ)
  {
    int ii;
    for( ii=0; ii<d.oldIComps.size(); ii++ )
     oldIComps.push_back(d.oldIComps[ii]);
    for( ii=0; ii<d.newIComps.size(); ii++ )
     newIComps.push_back(d.newIComps[ii]);
//    for( ii=0; ii<d.flKeywds.GetCount(); ii++ )
//     flKeywds.Add(d.flKeywds[ii]);
  }

  const icSetupData& operator=( const icSetupData& d)
  {
    int ii;
    from_templ =  d.from_templ;
    to_templ =    d.to_templ;

    for( ii=0; ii<d.oldIComps.size(); ii++ )
     oldIComps.push_back(d.oldIComps[ii]);
    for( ii=0; ii<d.newIComps.size(); ii++ )
     newIComps.push_back(d.newIComps[ii]);
//    for( ii=0; ii<d.flKeywds.GetCount(); ii++ )
//     flKeywds.Add(d.flKeywds[ii]);

    return *this;
  }

};

struct dcSetupData
{
   string from_templ;
   string to_templ;
   string f_script;

//   vector<string> flKeywds;   // list files to open

   dcSetupData():
   from_templ("*?"), to_templ("*_"), f_script("")
   {}

   dcSetupData( const dcSetupData& d ):
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
   string from_templ;
   string to_templ;
   string f_script;

//   vector<string> flKeywds;   // list files to open

   rdSetupData():
   from_templ("*?"), to_templ("*_"), f_script("")
   {}

   rdSetupData( const rdSetupData& d ):
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
   string from_templ;
   string to_templ;
   string f_script;

//   vector<string> flKeywds;   // list files to open

   cmSetupData():
   from_templ("*"), to_templ("*_"), f_script("")
   {}

   cmSetupData( const cmSetupData& d ):
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
enum copyFlagX { PHcopyL__ = 0, // (deleted!) Copy Phase records for &liquid (glass, hydrocarbon, melt) phases
                 PHcopyF_,     // Copy only Phase records &that retain all species after filtering
                 PHcopyD_,     // Create a list of discarded, but partially &usable Phase records
                 PHcopyN__,     // (deleted!) Copy also Phase records for non-&ideal solution phases
                 PHcopyA__,     // (deleted!) Copy Phase records for aqueous &electrolyte and/or gas phase
                 PHcopyY__      // (deleted!) Copy Phase records for &non-aqueous adsorption phases
                 };

struct phSetupData
{
   string from_templ;
   string to_templ;

   bool flags[6];

//   vector<string> flKeywds;   // list files to open

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

   phSetupData( const phSetupData& d ):
    from_templ( d.from_templ), to_templ(d.to_templ)
  {
    int ii;
    for( ii=0; ii<6; ii++ )
     flags[ii] = d.flags[ii];
//    for( ii=0; ii<d.flKeywds.GetCount(); ii++ )
//     flKeywds.Add(d.flKeywds[ii]);
  }

  const phSetupData& operator=( const phSetupData& d)
  {
    int ii;
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

 setFiltersData( const setFiltersData& d ):
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

   string objName;
   string ndxName;

   tableSetupData( int aWin, int aobj, const char * oName,
                   int aIdx, const char * ndName,
                   double aVal, char aUnit ):
           nWin(aWin), nObj(aobj), nIdx(aIdx), val(aVal), unit (aUnit),
           objName(oName), ndxName (ndName)
   { }

   tableSetupData( const tableSetupData& d ):
           nWin(d.nWin), nObj(d.nObj), nIdx(d.nIdx), val(d.val), unit(d.unit),
           objName(d.objName), ndxName(d.ndxName)
   {
       // cout << "nWin = " << nWin << " nObj = " << nObj << " nIdx = " << nIdx << endl;
   }

};


struct windowSetupData
{
   string pageName;
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

   windowSetupData( const windowSetupData& d ):
           pageName(d.pageName),  nObj(d.nObj), nOunit(d.nOunit),  nSwitch(d.nSwitch),
           unitLine(d.unitLine), defVal( d.defVal), defUnit(d.defUnit)
   {}

};


#endif // calc_filters_included
