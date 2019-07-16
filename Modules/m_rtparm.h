//-------------------------------------------------------------------
// $Id: m_rtparm.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of TRTParm class, config and calculation functions
//
// Rewritten from C to C++ by S.Dmytriyeva  
// Copyright (C) 1995-2001 S.Dmytriyeva, D.Kulik
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
//
#ifndef _m_rtparm_h_
#define _m_rtparm_h_

#include "v_mod.h"
#include "v_module.h"
#include "v_ipnc.h"
#include "graph_window.h"

const int RP_RKLEN = 48,
          MAXRTNAME = 12;

typedef struct
{// Description  RTPARM
    char
 //   pstate[MAXSYMB],     // Valid record key of REACDC or DCOMP
 //   psymb[MAXDRGROUP],
 //   dcn[MAXDCNAME],
 //   atyp[MAXSYMB],
 //  NV_[MAXNV], nvch,    // Variant number of RTPARM-task

    What,  //Source of input data for RTPARM-task { r d }, r: REACDC, d: DCOMP
    PunE,  //  Units of energy { j;  J c C N reserved }
    PunV,  //  Units of volume { j;  c L a reserved }
    PunP,  //  Units of pressure P  { b;  B p P A reserved }
    PunT,  // Units of temperature T  { C; K F reserved }
    Pplot, // Flag of plotting empirical data { + - }_
    Pabs,  // P or T for abscissa { P T } default T_
    Ptun,  // Units of T in abscissa { K C } default K_
    Ppun,  // Units of P in abscissa { b;  B p P A reserved }
    PunR1, //  reserved ( + - )

    name[MAXGSNAME+1],    // Full name  sampler definition (title)
    comment[MAXGSNAME+1], // comment
    xNames[MAXAXISNAME], // Abscissa name
    yNames[MAXAXISNAME]; // Ordinate name
 char (*lNam)[MAXGRNAME];    // List of ID of lines on Graph
 char (*lNamE)[MAXGRNAME];   // List of ID of lines of empirical data

 short NP,NT,  // N of points along P and  N of points along TC
    NV,       // Total N of points
    Mode,  /* Mode of indexation of T,P vectors: 0- input of P and ?values
     1- increments in cycle on P nested into cycle on T
     2- increments in cycle on T nested into cycle on P
     3- increment of T and P in one cycle*/
    Nsd,              // N of data source references
    res,              // reserved
    dimEF[2],    // Dimensions of array of empirical data
    dimXY[2],    // Dimensions of data sampler tables: col.1 - N of records;
    axisType[6];   // axis graph type, background(3), graph type, reserved
 float
    Pi[3], // Pressure, bar: P start,  P end,  increment of P
    Ti[3], // Temperature, deg.C: TC start, TC end, increment of TC
    size[2][4];          // Graph axis scale for region and fragment
 double
    // Resalts of calculations [0:NP*NT]
    *T,    // TC vector (temperatures)
    *P,    // P vector (pressures)
    *F,    // Sampled data array
    *TE,   // Grid of input empirical temperatures TC
    *PE,   // Grid of input empirical pressures P, bar
    *FE;    //Empirical input data array

 char
     *trpn,
     *expr,     // Text with IPN-expressions for data sampler
     *exprE;    // Text with IPN-expressions for empirical data (optional)
 char (*sdref)[V_SD_RKLEN]; // List of SDref keys to data sources
 char (*sdval)[V_SD_VALEN]; // Comments on data sources

 short // work data
    jTP,     // current index
    iE, jE;
 TPlotLine *Plot;

}
RTPARM;

// Current RTParm
class TRTParm : public TCModule
{
    RTPARM rp[1];

    GraphWindow *gd_gr;
    TPlotLine *plot;

    void expr_analyze();
    void exprE_calc();

protected:
    IPNCalc rpn[2];       // IPN of equats  -- Expr

public:

    static TRTParm* pm;

    RTPARM *rpp;

    TRTParm( uint nrt );

    const char* GetName() const
    {
        return "RTParm";
    }

    void ods_link( int i=0);
    void dyn_set( int i=0);
    void dyn_kill( int i=0);
    void dyn_new( int i=0);
    void set_def( int i=0);
    bool check_input( const char *key, int level=1 );

    void RecInput( const char *key );
    void MakeQuery();
    int RecBuild( const char *key, int mode = VF_UNDEF );
    void RecCalc( const char *key );
    void RecordPlot( const char *key );
    virtual gstring  GetKeyofRecord( const char *oldKey,
       const char *strTitle, int keyType );

    //void CmHelp();
    const char* GetHtml();
    bool SaveGraphData( GraphData* graph );
#ifndef USE_QWT
    //bool SaveChartData( jsonui::ChartData* grdata );
#endif
};

#endif  // _m_rtparm_h
