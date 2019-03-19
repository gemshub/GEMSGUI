//-------------------------------------------------------------------
// $Id: m_gtdemo.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of TGtDemo class, config and calculation functions
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
#ifndef _m_gtdemo_h_
#define _m_gtdemo_h_

#include "v_mod.h"
#include "v_ipnc.h"
#include "graph.h"

const int
MAXGTDNAME =    16,
                MAXDATATYPE =    8,
                                 MAXGTDCODE =     16,
                                                  MAXGDGROUP =    26;

typedef struct
{ // Description  GTDEMO
  char
    PtAEF,    // Use empirical data (xE, yE ) { + - }

    // Sample data from PDB chain
    PsIC,  // ICOMP     + -
    PsDC,  // DCOMP     + -
    PsBC,  // COMPOS    + -
    PsRE,  // REACDC    + -
    PsRP,  // RTPARM    + -
    PsPH,  // PHASE     + -
    PsST,  // SYSEQ    + -
    PsPE,  // PROCESS   + -
    PsPB,  // PROBE     + -
    PsUT,  // DUTERM    + -
    // reserved
    PsTR,  // MASSTRANSPORT + -  !
    PsRes1,  // reserved    + -  !
    PsRes2,  // reserved    + -  !
    PsRes3,  // reserved    + -  !
    PsRes4,  // reserved    + -

    name[MAXGSNAME+1],    // Full name of GTDEMO sampler definition (title)
    comment[MAXGSNAME+1], // comment
    xNames[MAXAXISNAME], // Abscissa name
    yNames[MAXAXISNAME]; // Ordinate name
  float
    size[2][4];          // Graph axis scale for region and fragment

    /*  data for calc*/
  short Nlrk,           // N of records
    nRT,              // chain index
    Nsd,              // N of data source references
    dimX,              // reserved
    Nwc,              // Dimensions: array of constants gd_wc
    Nqp,              // array of work cells gOp
    axisType[6],     // axis graph type, background(3), graph type, reserved
    dimEF[2],    // Dimensions of array of empirical data yE, xE
    dimXY[2];    // Dimensions of data sampler tables: col.1 - N of records;
    //col.2 - N of lines
  char
    wcrk[MAXRKEYLEN],  // reserved
    prKey[MAXRKEYLEN], // PROCES or PROBE record key
    *rkey,         // List of record keys of sampled PDB records [0:Nlrk-1]
    *expr,     // Text with IPN-expressions for data sampler
    *exprE;    // Text with IPN-expressions for empirical data (optional)
    
  char (*sdref)[V_SD_RKLEN]; // List of SDref keys to data sources
  char (*sdval)[V_SD_VALEN]; // Comments on data sources
  char (*lNam0)[MAXGRNAME];   // List of ID of lines on Graph
  char (*lNamE)[MAXGRNAME];   // List of ID of lines of empirical data

  double
    *x0,   // Vector of abscissa dimXY[][dimX]
    *y0,  // Sampled data array dimXY[][]
    *xE, *yE,         // Input empirical data XE, YE
    *wc,              // Array of constants (optional)
    *qp;              // Array of work cells (optional)
    /* work parameters */
  short
    rtLen,   // len of cuurent record  no object
    Next,   // Flag to continue (1) or finish (0) sampling iterations
    jR,     // Index of currently sampled PDB record jRl[x][0]<jR<jRl[x][1]*/
    i0,iE,    //i0, i1, i2, i3, iE indexes 0<i<dimXY[2][0]
    j0,jE     //j0, j1, j2, j3, jE indexes 0<j<dimXY[2][1]
    ;

  char 
	Wkb[MAXRKEYLEN],   // Current key of sampled PDB record
    SYS_key[EQ_RKLEN],    // key of last read record SYSTEM
    *etext,        // internal
    *prtab,        // internal
    *tprn;         // internal
}

GTDEMO;

typedef struct
{         /* GTDEMO statistic by Probe?*/
    short nI,       /* current index in iopt*/
    *iopt ;   /* optimal balls for Systat calc dimXY[][0]*/
    double
    *opt,  /* matr of criterial?*/
    *G,
    *b;
}
GDSTAT;

// Current GtDemo
class TGtDemo : public TCModule
{
    GTDEMO gd[1];
    GDSTAT gst;

    GraphWindow *gd_gr;
    TPlotLine *plot;
    gstring titler;

protected:
    IPNCalc rpn[2];       // IPN of equats of process  -- Expr

    bool check_RT( int nrt );

    void gd_ps_set();
    void bld_rec_list();
    void gd_text_analyze();
    void gd_EF_calc();
    void gd_rec_read( int nI );
//    void elst(int N,double *U,double *par);
//    void probe_stat( const char *key );
    void keyTest( const char *key );

public:

    static TGtDemo* pm;

    GTDEMO *gdp;
    TGtDemo( uint nrt );

    const char* GetName() const
    {
        return "GtDemo";
    }

    void ods_link( int i=0);
    void dyn_set( int i=0);
    void dyn_kill( int i=0);
    void dyn_new( int i=0);
    void set_def( int i=0);
    gstring   GetKeyofRecord( const char *oldKey, const char *strTitle,
                              int keyType );

    void RecInput( const char *key );
    void MakeQuery();
    int RecBuild( const char *key, int mode = VF_UNDEF );
    void RecCalc( const char *key );
    void RecordPlot( const char *key );

    //Get key from list
    const char * GetRkey( int j) const
    {
        return gdp->rkey+j*gdp->rtLen;
    }

    bool SaveGraphData( GraphData* graph );
    //void CmHelp();
    const char* GetHtml();

    const gstring& GetString();

};

#endif  // _m_gtdemo_h_
