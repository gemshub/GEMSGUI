
#ifdef __unix__
#include <unistd.h>
#endif

#include "m_param.h"
#include "gdatastream.h"

TProfil* TProfil::pm;

const double R_CONSTANT = 8.31451,
              NA_CONSTANT = 6.0221367e23,
                F_CONSTANT = 96485.309,
                  e_CONSTANT = 1.60217733e-19,
                    k_CONSTANT = 1.380658e-23,
// Conversion factors
                      cal_to_J = 4.184,
                        C_to_K = 273.15,
                          lg_to_ln = 2.302585093,
                            ln_to_lg = 0.434294481;

SPP_SETTING pa_ = {
    "GEM-Selektor v2-PSI: Controls and defaults for numeric modules",
    {
        1,  /* PC */  3,     /* PD */   3,   /* PRD */
        1,  /* PSM  */ 144,  /* DP */   15,   /* DW */
        -2, /* DT */  0,     /* PLLG */   1,   /* PE */
        500,   /* IIM */
        1e-30, /* DG */   1e-8,  /* DHB */  1e-12,  /* DS */
        1e-5,  /* DK */  0.01,  /* DF */  1e-12,  /* DFM */
        1e-6,  /* DFYw */  1e-6,  /* DFYaq */    1e-6,  /* DFYid */
        1e-6,  /* DFYr,*/  1e-6,  /* DFYh,*/   1e-6,  /* DFYc,*/
        1e-12, /* DFYs, */  1e-17,  /* DB */   0.7,   /* AG */
        0.07,   /* DGC */   1.0,   /* GAR */  1000., /* GAH */
        0.001, /* GAS */  12.05,  /* DNS */  1e-5,  /* XwMin, */
        1e-7,  /* ScMin, */    1e-20, /* DcMin, */   1e-10, /* PhMin, */
        3e-5,  /* ICmin */   1e-7,  /* EPS */   1e-15,  /* IEPS */
        1e-4,  /* DKIN  */ 0,  /* tprn */
    },
}; /* SPP_SETTING */


void
BASE_PARAM::write(ostream& oss)
{
    oss.write( (char*)&PC, 10*sizeof(short) );
    oss.write( (char*)&DG, 28*sizeof(double) );
    oss.write( (char*)&tprn, sizeof(char*) );
}


void
SPP_SETTING::write(ostream& oss)
{
    oss.write( ver, TDBVERSION );
    p.write( oss );
}


TProfil::TProfil( int nNd )
{
    pa= pa_;
    multi = new TMulti( nNd );
    pmp = multi->GetPM();
}

void
TProfil::calcMulti()
{
    // MultiCalcInit( keyp.c_str() );
    //    // realloc memory for  R and R1
    pmp->R = new double[pmp->N*(pmp->N+1)];
    pmp->R1 = new double[pmp->N*(pmp->N+1)];
    memset( pmp->R, 0, pmp->N*(pmp->N+1)*sizeof(double));
    memset( pmp->R1, 0, pmp->N*(pmp->N+1)*sizeof(double));

    if( AutoInitialApprox() == false )
        MultiCalcIterations();
}

void TProfil::outMulti( GemDataStream& ff, gstring& path  )
{
    ff.writeArray( &pa.p.PC, 10 );
    ff.writeArray( &pa.p.DG, 28 );
    multi->to_file( ff, path );
}


void TProfil::readMulti( GemDataStream& ff )
{
      ff.readArray( &pa.p.PC, 10 );
      ff.readArray( &pa.p.DG, 28 );
      multi->from_file( ff );
}


// ------------------ End of m_param.cpp -----------------------




