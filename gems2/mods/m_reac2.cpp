//-------------------------------------------------------------------
// $Id$
//
// Implementation of TReacDC class,  calculation functions
//
// Rewritten from C to C++ by S.Dmytriyeva
// Copyright (C) 1995-2001 S.Dmytriyeva, D.Kulik
//
// This file is part of a GEM-Selektor library for thermodynamic
// modelling by Gibbs energy minimization
// Uses: GEM-Vizor GUI DBMS library, gems/lib/gemvizor.lib
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail: gems2.support@psi.ch
//-------------------------------------------------------------------
//
#include <math.h>
#include <stdio.h>

#include "m_param.h"
#include "m_reacdc.h"
#include "m_dcomp.h"
#include "s_supcrt.h"
#include "s_formula.h"
#include "service.h"
#include "visor.h"
#include "num_methods.h"

/*-----------------------------------------------------------------*/
// Calculates coefficients A0 to A6 of logK=f(T)
// from coefficients a0 to a4 of Haas-Fischer Cp=f(T)
// Coeffs a5 to a9 are ignored

void TReacDC::Convert_Cp_to_KT( int /*CE*/ )
{
    double Rln10, T, lnT, T_2, T_05, T_3, Sr, Hr, lgK, Cpr;
    float *Cp, *A;

    Rln10 = R_CONSTANT * lg_to_ln;
    T = rcp->TCst + C_to_K;
    lnT = log( T );
    T_2 = T*T;
    T_3 = T_2*T;
    T_05 = sqrt( T );
    Sr = rcp->Ss[0];
    Hr = rcp->Hs[0];
    Cp = rcp->DCp;
    A = rcp->pKt;

// calculation of logK=f(T) coeffs
    A[0] = ( Sr - Cp[0] - Cp[0]*lnT - Cp[1]*T + Cp[2]/(2.0*T_2)
                  + 2.0*Cp[3]/T_05 - Cp[4]*T_2/2.0 ) / Rln10;
    A[1] = Cp[1]/(2.0*Rln10);
    A[2] = -( Hr - Cp[0]*T - Cp[1]*T_2/2.0 + Cp[2]/T
               - 2.0*Cp[3]*T_05 - Cp[4]*T_3/3.0 ) / Rln10;
    A[3] = Cp[0]/Rln10;
    A[4] = Cp[2]/(2.0*Rln10);
    A[5] = Cp[4]/(6.0*Rln10);
    A[6] = -4.0*Cp[3]/Rln10;

// Old version - comment out after debugging !
    A[0] = Sr/Rln10 - ( Cp[1]*T + Cp[0]*(1.0+lnT) - Cp[2]/(2.0*T_2) +
                        Cp[4]*T_2/2.0 - 2.0*Cp[3]/T_05 ) / Rln10;
    A[2] = ( Cp[1]*T_2/2.0 + Cp[0]*T -Cp[2]/T + Cp[4]*T_3/3.0 +
             2.0*Cp[3]*T_05 - Hr ) / Rln10;
/* commented out by KD on 15.07.03
    switch(CE) // zeroing off unnecessary coeffs for specific cases
    {
    case CTM_LGK:
    case CTM_LGX:
        break;
    case CTM_EK1:
        K[0]=0.0;
    case CTM_EK2:
        K[3]=0.0;
    case CTM_EK3:
        K[1]=0.0;
        K[4]=0.0;
        K[5]=0.0;
        K[6]=0.0;
        break;
    default:
        return;
    }
*/   // Calculation of Cpr and lgK at 25 C
    Cpr = Cp[0] + Cp[1]*T + Cp[2]/T_2 + Cp[4]*T_2 + Cp[3]/T_05;
    lgK = A[0] + A[1]*T + A[2]/T + A[3]*lnT + A[4]/T_2 +
          A[5]*T_2 + A[6]/T_05;
    rcp->Cps[0] = Cpr;
    rcp->Ks[1] = lgK;
}

/*-----------------------------------------------------------------*/
void TReacDC::Convert_KT_to_Cp( int CE )
{
    double Rln10, T, lnT, T_2, T_05, /*T_15,*/ T_3,
           Gr, Sr, Hr, Cpr, lgK;
    float *Cp, *A;

    Rln10 = R_CONSTANT * lg_to_ln;
    T = rcp->TCst + C_to_K;
    lnT = log( T );
    T_2 = T*T;
    T_3 = T_2*T;
    T_05 = sqrt( T );
//    T_15 = pow( T, 1.5 );
    Cp = rcp->DCp;
    A = rcp->pKt;
    Sr = rcp->Ss[0];
//    Hr = rcp->Hs[0];
    Gr = rcp->Gs[0];
    lgK = rcp->Ks[1];
    //AGAIN_CP:
    switch( CE )
    { // calculation 2- and 3-term�param approximation
    case CTM_DKR: // 3-term extrap. from Franck-Marshall density model
    case CTM_MRB: // 3-term extrap. from MRB at 25 C (provisional DK 06.08.07)
    case CTM_LGK:  // Here, all logK-f(T) coeffs are given
//    case CTM_LGX:
        break;
    case CTM_IKZ:  // Isotopic forms
        return;
    case CTM_EK0: // Generating 1-term extrapolation at logK = const
        A[0]= rcp->Ss[0]/Rln10;
        A[1]=0.0;
        A[2]=0.0;
        A[3]=0.0;
        A[4]=0.0;
        A[5]=0.0;
        A[6]=0.0;
        break;
    case CTM_EK1: // Generating 1-term extrapolation at dGr = const
        A[0]=0.0;
        A[1]=0.0;
        A[2] = -rcp->Hs[0]/Rln10;
        A[3]=0.0;
        A[4]=0.0;
        A[5]=0.0;
        A[6]=0.0;
        break;
    case CTM_EK2: // Generating 2-term (Vant Hoff) extrapolation
        A[0] = rcp->Ss[0]/Rln10;
        A[1]=0.0;
        A[2] = -rcp->Hs[0]/Rln10;
        A[3]=0.0;
        A[4]=0.0;
        A[5]=0.0;
        A[6]=0.0;
        break;
    case CTM_PPE:
    case CTM_EK3: // Generating 3-term extrapolation at constant dCpr
        if( Cp )
        {
          Cp[1] = Cp[2] = Cp[3] = Cp[4] = 0.0;
          rcp->Cps[0] = Cp[0];
        }
        Cpr = rcp->Cps[0];
        A[0] = ( rcp->Ss[0] - Cpr*(1.+lnT) ) / Rln10;
        A[1]=0.0;
        A[2] = ( Cpr*T - rcp->Hs[0] ) / Rln10;
        A[3] = Cpr / Rln10;
        A[4]=0.0;
        A[5]=0.0;
        A[6]=0.0;
        break;
    default:
        Error( GetName(),"E23RErun: Invalid method code in Convert_KT_to_Cp()");
    }

    switch( CE )
    {
    case CTM_LGK:
       /* Calc lgK, Hr and Sr */
       lgK = A[0] + A[1]*T + A[2]/T + A[3]*lnT + A[4]/T_2 +
             A[5]*T_2 + A[6]/T_05;
       Hr = Rln10 * ( A[1]*T_2 - A[2] + A[3]*T -
                         2.0*A[4]/T + 2.0*A[5]*T_3 - 0.5*A[6]*T_05 );
       Sr = Rln10 * ( A[0] + 2.0*A[1]*T + A[3]*(1.0 + lnT)
                   - A[4]/T_2 + 3.0*A[5]*T_2 + 0.5*A[6]/T_05 );
// Old variant - commented out after debugging !!
//    Hr = Rln10 * T_2 * ( A[1] - A[2]/T_2 + A[3]/T -
//                         2.0*A[4]/T_3 + 2.0*A[5]*T - 0.5*A[6]/T_15 );
//    Sr = Rln10 * ( A[0] + 2.0*A[1]*T + A[3]*(1.0+lnT) -
//                   A[4]/T_2 + 3.0*A[5]*T_2 + 0.5*A[6]/T_05 );
       if( Cp )
       { // calc coef  Cp
          Cp[0] = Rln10 * A[3];
          Cp[1] = Rln10 * 2.0 * A[1];
          Cp[2] = Rln10 * 2.0 * A[4];
          Cp[3] = -Rln10 * 0.25 * A[6];
          Cp[4] = Rln10 * 6.0 * A[5];
          Cpr = Cp[0] + Cp[1]*T + Cp[2]/T_2 + Cp[4]*T_2 + Cp[3]/T_05;
       }
       else { // no Cp coef array!
          Cpr = Rln10*A[3];
          // Error message ?
       }
       rcp->Ss[0] = Sr;  // In this case, everything will be inserted
       rcp->Hs[0] = Hr;
       rcp->Cps[0] = Cpr;
       rcp->Ks[1] = lgK;
       if( fabs( lgK ) < 100. )
           rcp->Ks[0] = pow(10., lgK );
       rcp->Gs[0] = -Rln10*T*lgK;
    }
    /* Checking consistency of delta-reaction data */
    Hr = Gr + T*Sr;
    if( fabs( rcp->Ss[0] - Sr ) >= 0.19 ||
            fabs( rcp->Hs[0] - Hr ) >= 57.08 ||
            fabs( rcp->Gs[0] - Gr ) >= 57.08 ||
            fabs( rcp->Ks[1] - lgK ) >= 0.01 )
    {
        gstring msgbuf = "W24RErun: Warning: Some of the calculated values\n logK = ";
        vstr doublbuf(100);
        sprintf( doublbuf, "%g", lgK );
        msgbuf += doublbuf;
        msgbuf += ", dGr = ";
        sprintf( doublbuf, "%g", Gr );
        msgbuf += doublbuf;
        msgbuf += ", dSr = ";
        sprintf( doublbuf, "%g", Sr );
        msgbuf += doublbuf;
        msgbuf += " or dHr = ";
        sprintf( doublbuf, "%g", Hr );
        msgbuf += doublbuf;
        msgbuf += "\n values are inconsistent\n";
        msgbuf += " with previous values in >0.01 logK units.\n\n";
        msgbuf += "Replace with calculated values (Y)\n";
        msgbuf += " or keep the previous values (N)?";
        if( vfQuestion(window(), GetName(), msgbuf.c_str() ))
        {
            rcp->Ss[0] = Sr;
            rcp->Hs[0] = Hr;
            rcp->Gs[0] = Gr;
            rcp->Ks[1] = lgK;
        }
        else {
                Error( GetName(),
        "E25RErun: Input logK, dSr or dHr remain inconsistent - bailing out!");
        }
    }
    return;
}

/*-----------------------------------------------------------------*/
void TReacDC::Recalc( int q, const char *key  )
{
    int i, st0, st1, Count, rShift=0, CM, CE;
    double Z, MW, foS;
    vstr dcn(MAXRKEYLEN+5);
    time_t crt;

    TFormula aFo;
    TDComp* aDC=(TDComp *)(&aMod[RT_DCOMP]);
    aDC->ods_link(0);

    memset( dcn, 0, MAXRKEYLEN );
    Count = rcp->nDC;
    CM = toupper( rcp->pct[0] );
    CE = toupper( rcp->pct[1] );

    for( i=0; i<Count; i++ )
    {  /* Get key */
        if( rc[q].rDC[i] != SRC_DCOMP && rc[q].rDC[i] != SRC_REACDC )
            continue;
        memcpy( dcn, rc[q].DCk[i], DC_RKLEN );
        /* Read Record */
        if( rc[q].rDC[i] == SRC_DCOMP )
            aDC->TryRecInp( dcn, crt, 0 );
        else if( rc[q].rDC[i] == SRC_REACDC )
        {
            if( !rShift )
            {
                ods_link(q+1);
                rShift=1;
            }
            TryRecInp( dcn, crt, q+1 );
        }
        if( rc[q].rDC[i] == SRC_DCOMP )
        {
            rc[q].ParDC[i][_Gs_] = aDC->dcp->Gs[0];
            rc[q].ParDC[i][_Hs_] = aDC->dcp->Hs[0];
            rc[q].ParDC[i][_Ss_] = aDC->dcp->Ss[0];
            rc[q].ParDC[i][_Cps_] = aDC->dcp->Cps[0];
            rc[q].ParDC[i][_Vs_] = aDC->dcp->mVs[0];
            if( rc[q].PrAki != S_OFF )
                rc[q].ParDC[i][_Zs_] = aDC->dcp->Zz;
            if( i == rc[q].nDC-1 )
                memcpy( rc[q].form, aDC->dcp->form, MAXFORMULA );
        }
        else
        {
            rc[q].ParDC[i][_Gs_] = rcp->Gs[1];
            rc[q].ParDC[i][_Hs_] = rcp->Hs[1];
            rc[q].ParDC[i][_Ss_] = rcp->Ss[1];
            rc[q].ParDC[i][_Cps_] = rcp->Cps[1];
            rc[q].ParDC[i][_Vs_] = rcp->Vs[1];
            if( rc[q].PrAki != S_OFF )
                rc[q].ParDC[i][_Zs_] = rcp->Zz;
            if( i == rc[q].nDC-1 )
                memcpy( rc[q].form, rcp->form, MAXFORMULA );
        }
    } /* i */
    if( rShift )
    {
        ods_link( q );
        db->Find(key);/*rShift = 0;*/
    }

    //AGAINF:  Edit fields of record ?? vRet = Vaccess( W_REACDC, msg(cp_rcomp) );
    //FIRSTTIME:  Set formula
    /* Get formula and test it */
    aFo.SetFormula( rc[q].form );   // set formula and analyse it
    /* Get number of elements in the formula */
    int nICs;
    short listAN[100];
    /* Calc molar mass and formula charge */
    nICs = aFo.Fmwtz( Z, MW, foS, listAN );
    if( fabs( Z ) < ZBALANCE_PREC )
        rc[q].Zz = 0.;
    else rc[q].Zz = (float)Z;
    rc[q].mwt = (float)MW;

    SetString("RC_test   Checking this ReacDC definition");
    Update();

    switch( rc[q].pstate[0] )
    { /* check phase state */
    case CP_SOLID:
    case CP_GAS:
    case CP_UNIV:
    case CP_HCARB:
    case CP_LIQID: /* no charge  */
        if( fabs( Z ) < ZBALANCE_PREC &&
                memcmp( CHARGE_NAME, aFo.GetCn( aFo.GetIn()-1 ), 2 ))
        {
            rc[q].Zz = 0.;
            goto NEXT;
        }
        break;
    case CP_AQU:
    case CP_GASI:
    case CP_SSPC:
    case CP_SORB:
    case CP_MELT: /* must be charge */
        if( !memcmp( CHARGE_NAME, aFo.GetCn( aFo.GetIn()-1 ), 2 ))
            goto NEXT;
    }
    Error( GetName(),
    "W26RErun: Please, check stoichiometry, charge or valences in the formula.");
NEXT:
    /* test value of st.mol.volume */
    if( ( rc[q].pstate[0] == CP_GAS || rc[q].pstate[0] == CP_GASI )
     &&  rc[q].Vs[1] < 1. )
        rc[q].Vs[1] = GAS_MV_STND;
    /* Test standart T, P, Gst, Hst, Sst */
    st0 = IsFloatEmpty( rc[q].TCst );
    st1 = IsFloatEmpty( rc[q].Pst );
    if( st0 )
        rc[q].TCst = (float)STANDARD_TC;
    if( st1 )
        rc[q].Pst = (float)STANDARD_P;

    if( CE == CTM_DKR )  // Franck-Marshall density model added by TW and DK 29.01.2008
    {
        if( vfQuestion(window(), GetName(),
            "Estimate dGr,dSr,dHr,dCpr,dVr using the Franck-Marshall density model?" ))
         {
            double TK;
            double H2Oprop[4];
            double MFDcoef[7];
            double ReactProp[6];
            double rhoW, alphaW, betaW, dAldTW;

            TK = 298.15;
            // water properties (25 deg C, 1 bar) from SUPCRT92
            rhoW = 0.99706136;
            alphaW = 2.59426542e-4;
            dAldTW = 9.56485765e-6;
            betaW = 4.52187717e-5;
            H2Oprop[0] = rhoW;
            H2Oprop[1] = alphaW;
            H2Oprop[2] = dAldTW;
            H2Oprop[3] = betaW;

        // get species parameters
            MFDcoef[0] = rc[q].DSt[0];
            MFDcoef[1] = rc[q].DSt[1];
            MFDcoef[2] = rc[q].DSt[2];
            MFDcoef[3] = rc[q].DSt[3];
            MFDcoef[4] = rc[q].DSt[4];
            MFDcoef[5] = rc[q].DSt[5];
            MFDcoef[6] = rc[q].DSt[6];

           // calculate MFD function
            MFDcalc(TK, H2Oprop, MFDcoef, ReactProp);

           // get results
           rc[q].Ks[0] = FLOAT_EMPTY;
           rc[q].Ks[1] = ReactProp[0];
           rc[q].Gs[0] = ReactProp[1];
           rc[q].Hs[0] = ReactProp[3];
           rc[q].Ss[0] = ReactProp[2];
           rc[q].Vs[0] = ReactProp[5];
           rc[q].Cps[0] = ReactProp[4];
         }
    }
    if( CE == CTM_MRB )  // Inserted provisionally by DK on 06.08.07
    {
        if( vfQuestion(window(), GetName(),
            "Estimate dGr,dSr,dHr,dCpr,dVr using modified Ryzhenko-Bryzgalin model?" ))
         { /* Check if this is a dissociation reaction */
            if( rc[q].scDC[rc[q].nDC-1] >= 0 )
               Error( GetName(),"W27RErun: MRB model requires a dissociation reaction!");

            double TK;
            double H2Oprop[4];
            double MRBcoef[3];
            double ReactProp[6];
            double rhoW, alphaW, betaW, dAldTW;

            TK = 298.15;
            // test water properties (25 deg C, 1 bar) from SUPCRT92
            rhoW = 0.99706136;
            alphaW = 2.59426542e-4;
            dAldTW = 9.56485765e-6;
            betaW = 4.52187717e-5;
            H2Oprop[0] = rhoW;
            H2Oprop[1] = alphaW;
            H2Oprop[2] = dAldTW;
            H2Oprop[3] = betaW;

        // get species parameters
            MRBcoef[0] = rc[q].DSt[0];
            MRBcoef[1] = rc[q].DSt[1];
            MRBcoef[2] = rc[q].DSt[2];

           // calculate MRB function
            MRBcalc (TK, H2Oprop, MRBcoef, ReactProp);

           // get results
           rc[q].Ks[0] = FLOAT_EMPTY;
           rc[q].Ks[1] = ReactProp[0];
           rc[q].Gs[0] = ReactProp[1];
           rc[q].Hs[0] = ReactProp[3];
           rc[q].Ss[0] = ReactProp[2];
           rc[q].Vs[0] = ReactProp[5];
           rc[q].Cps[0] = ReactProp[4];
         }
    }
    if( CM == CTPM_REA )
    {
        if( CE == CTM_EK0 )
        {
            rc[q].Ss[0] = FLOAT_EMPTY;
            rc[q].Hs[0] = 0.0;
            rc[q].Cps[0] = 0.0;
            if( !rc[q].pKt || rc[q].PreKT == S_OFF ) /* Temporarily !!!!!!! */
                rc[q].pKt =   (float *)aObj[ o_repkt ].Alloc( MAXCPCOEF, 1, F_);
        }
        else if( CE == CTM_EK1 )
        {
            rc[q].Ss[0] = 0.0;
            rc[q].Hs[0] = DOUBLE_EMPTY;
            rc[q].Cps[0] = 0.0;
            if( !rc[q].pKt || rc[q].PreKT == S_OFF ) /* Temporarily !!!!!!! */
                rc[q].pKt =   (float *)aObj[ o_repkt ].Alloc( MAXCPCOEF, 1, F_);
        }
        else if( CE == CTM_EK2 )
        {
            rc[q].Cps[0] = 0.0;
            if( !rc[q].pKt || rc[q].PreKT == S_OFF ) /* Temporarily !!!!!!! */
                rc[q].pKt =   (float *)aObj[ o_repkt ].Alloc( MAXCPCOEF, 1, F_);
        }
        else if( CE == CTM_EK3 )
        {
            if( !rc[q].pKt || rc[q].PreKT == S_OFF ) /* Temporarily !!!!!!! */
                rc[q].pKt =   (float *)aObj[ o_repkt ].Alloc( MAXCPCOEF, 1, F_);
        }
        else if( CE == CTM_LGK )
        {
            for( i=0; i<7; i++ )
              if( IsFloatEmpty( rc[q].Cps[i] ) )
                  rc[q].Cps[i] = 0.0;
        }
    }
    /*----------------------------------------------------*/
    /*if( rc[q].pct[0]==CTP_CP && rc[q].pct[1]==CEV_ST && rc[q].pct[2]==CVV_NO )*/
    if( rc[q].rDC[rc[q].nDC-1] == SRC_NEWDC )
        Recalc_rDCN( foS  );
    else if( rc[q].rDC[rc[q].nDC-1] == SRC_NEWISO )
    {
        Recalc_ISO1( foS);
        Recalc_ISO2( foS );
    }
    else // calc delta reactions
        Recalc_rDCD();
    if( CM == CTPM_CPT && CE == CTM_LGX )
        Convert_Cp_to_KT( CE );
    if( CM == CTPM_REA )
    {
       Convert_KT_to_Cp( CE );
       if( rc[q].rDC[rc[q].nDC-1] == SRC_NEWDC )  // added by KD on 16.07.03
           Recalc_rDCN( foS  );
    }
    if( CM == CTPM_REA && rc[q].pct[2] == CPM_PCR && rc[q].PrAki != S_OFF )
    {   /*  Call PRONSPREP  added 19.05.98 */
        if( vfQuestion(window(), GetName(),
                       "Estimate S, H, Cp, V using PRONSPREP97 algorithm?" ))
        { /* Check if this is a dissociation reaction */
            if( rc[q].scDC[rc[q].nDC-1] < 0 )
            {              
                if( rc[q].pct[1] != CTM_PPE )  // if not 'E' then old PRONSPREP
            	   PronsPrep( key );
                else 
                   PronsPrepOH( key, nICs, listAN );
            }
            else Error( GetName(),"W27RErun: PP97 requires a dissociation reaction!");
        }
    }
}

/*-----------------------------------------------------------------*/
// Recalc svoystv of vedushego DC po deltam reakcii i svoystvam
// other components
void TReacDC::Recalc_rDCN( double /*foS*/ )
{
    double  LK, RR, R_T, T, G=0.0, H=0.0, S=0.0, Cp=0.0, V=0.0;
    int i;
    int stG, stH, stS, stK, stLK;

    stG =  !IsDoubleEmpty(rcp->Gs[0]);
    stH =  !IsDoubleEmpty(rcp->Hs[0]);
    stS =  !IsFloatEmpty(rcp->Ss[0]);
    stK =  !IsDoubleEmpty( rcp->Ks[0] );
    stLK=  !IsDoubleEmpty( rcp->Ks[1] );

    if( !( stG || stH || stS || stK || stLK ) )
    { // no data for the reaction were supplied
        if( !IsDoubleEmpty(rcp->Gs[1]) )
        {
            stG = stK = stLK = 1;
            rcp->ParDC[rcp->nDC-1][_Gs_] = rcp->Gs[1];
        }
        if( !IsFloatEmpty(rcp->Ss[1]) )
        {
            stS = 1;
            rcp->ParDC[rcp->nDC-1][_Ss_] = rcp->Ss[1];
        }
        if( !IsDoubleEmpty(rcp->Hs[1]) )
        {
            stH = 1;
            rcp->ParDC[rcp->nDC-1][_Hs_] = rcp->Hs[1];
        }
        if( !IsFloatEmpty(rcp->Cps[1]) )
            rcp->ParDC[rcp->nDC-1][_Cps_] = rcp->Cps[1];
        if( !IsFloatEmpty(rcp->Vs[1]) )
            rcp->ParDC[rcp->nDC-1][_Vs_] = rcp->Vs[1];

        if( stG || stS || stH )
        { // trying to calculate data for the reaction
            Recalc_rDCD();
            rcp->Vs[0] = 0.;
            rcp->Cps[0] = 0.;
            if( !stG )
            {
                rcp->Gs[0] = DOUBLE_EMPTY;
                rcp->Gs[1] = DOUBLE_EMPTY;
            }
            if( !stS )
            {
                rcp->Ss[0] = FLOAT_EMPTY;
                rcp->Ss[1] = FLOAT_EMPTY;
            }
            if( !stH )
            {
                rcp->Hs[0] = DOUBLE_EMPTY;
                rcp->Hs[1] = DOUBLE_EMPTY;
            }
            if( stLK )
            {
                stG = 0;
                stK = 0;
                rcp->Gs[0] = DOUBLE_EMPTY;
                rcp->Gs[1] = DOUBLE_EMPTY;
                rcp->Ks[0] = DOUBLE_EMPTY;
            }
        }
        else return;
    }

    T = (double)rcp->TCst + C_to_K;
    RR = R_CONSTANT; /* !!!!! */
    R_T = RR*T;
    /* etap 1 */
STAGE1:
    if( stG && !stK && !stLK )
    {
        rcp->Ks[1] = -rcp->Gs[0] / lg_to_ln / R_T;
        rcp->Ks[0] = pow( 10., rcp->Ks[1] );
        stK = 1;
        stLK = 1;
        goto STAGE2;
    }
    if( !stG && !stK && stLK )
    {
        rcp->Gs[0] = -rcp->Ks[1] * lg_to_ln * R_T;
        rcp->Ks[0] = pow( 10., rcp->Ks[1] );
        stK = 1;
        stG = 1;
        goto STAGE2;
    }
    if( !stG && stK && !stLK )
    {
        rcp->Ks[1] = ln_to_lg * log( rcp->Ks[0] );
        rcp->Gs[0] = -rcp->Ks[1] * lg_to_ln * R_T;
        stLK = 1;
        stG = 1;
        goto STAGE2;
    }
    if( stG && stLK )
    {
        //G = -rcp->Ks[1] * lg_to_ln * R_T;
        LK = -rcp->Gs[0] / lg_to_ln / R_T;
        if( fabs( LK-rcp->Ks[1] ) >= 0.01 )
            if( !vfQuestion(window(), GetName(),
          "W28RErun: Inconsistent dGr and logK: take logK (yes) or dGr (no)?" ))
                rcp->Ks[1] = LK;
        rcp->Gs[0] = -rcp->Ks[1] * lg_to_ln * R_T;
        ;
        rcp->Ks[0] = pow( 10., LK );
        stK = 1;
        G=0.0;
    }

STAGE2:
    /* 2: foS excluded and rearranged 5 Dec 96 DAK (see remnants at end of file)*/
    for( i=0; i<rcp->nDC-1; i++ )
    {
        G  += rcp->scDC[i]*rcp->ParDC[i][_Gs_];
        H  += rcp->scDC[i]*rcp->ParDC[i][_Hs_];
        S  += rcp->scDC[i]*rcp->ParDC[i][_Ss_];
        Cp += rcp->scDC[i]*rcp->ParDC[i][_Cps_];
        V  += rcp->scDC[i]*rcp->ParDC[i][_Vs_];
    }
    if( stG && stH && !stS )
        rcp->Ss[0] = ( rcp->Hs[0] - rcp->Gs[0] ) / T;
    else if( stG && !stH && stS )
        rcp->Hs[0] = rcp->Gs[0] + T * rcp->Ss[0];
    else if( !stG && stH && stS )
    {
        rcp->Gs[0] = rcp->Hs[0] - T * rcp->Ss[0];
        stG = 1;
        goto STAGE1;
    }
    else if( stG && stH && stS ) ;
    else
        Error( GetName(),"W29RErun: One of values dGr, dHr, or dSr is missing.");

    /* calc Cp and V for vedushiy component?*/
    if( IsFloatEmpty( rcp->Cps[0] ))
        rcp->Cps[0] = 0.0;
    if( IsFloatEmpty( rcp->Vs[0] ))
        rcp->Vs[0] = 0.0;

    rcp->Gs[1] = rcp->ParDC[rcp->nDC-1][_Gs_] =
                     (rcp->Gs[0] - G)/rcp->scDC[rcp->nDC-1];
    rcp->Hs[1] = rcp->ParDC[rcp->nDC-1][_Hs_] =
                     (rcp->Hs[0] - H)/rcp->scDC[rcp->nDC-1];
    rcp->Ss[1] = rcp->ParDC[rcp->nDC-1][_Ss_] =
                     (rcp->Ss[0] - S)/rcp->scDC[rcp->nDC-1];
    rcp->Cps[1] = rcp->ParDC[rcp->nDC-1][_Cps_] =
                      (rcp->Cps[0] - Cp)/rcp->scDC[rcp->nDC-1];
    rcp->Vs[1]  = rcp->ParDC[rcp->nDC-1][_Vs_] =
                      (rcp->Vs[0] - V)/rcp->scDC[rcp->nDC-1];

}

/*-----------------------------------------------------------------*/
// Calculate delta_reaction from properties of components
void TReacDC::Recalc_rDCD( )
{
    double  RR, R_T, T, lgK;
    int i;

    /* copy svoystv poslednego vedusfego DK */
    rcp->Gs[1] = rcp->ParDC[rcp->nDC-1][_Gs_];
    rcp->Ss[1] = rcp->ParDC[rcp->nDC-1][_Ss_];
    rcp->Hs[1] = rcp->ParDC[rcp->nDC-1][_Hs_];
    rcp->Cps[1] = rcp->ParDC[rcp->nDC-1][_Cps_];
    rcp->Vs[1]  = rcp->ParDC[rcp->nDC-1][_Vs_];

    /*Calc delta reactions */
    rcp->Gs[0] = 0.0;
    rcp->Hs[0] = 0.0;
    rcp->Ss[0] = 0.0;
    rcp->Cps[0] = 0.0;
    rcp->Vs[0] = 0.0;
    for( i=0; i<rcp->nDC; i++ )
    {
        rcp->Gs[0]  += rcp->scDC[i]*rcp->ParDC[i][_Gs_];
        rcp->Hs[0]  += rcp->scDC[i]*rcp->ParDC[i][_Hs_];
        rcp->Ss[0]  += rcp->scDC[i]*rcp->ParDC[i][_Ss_];
        rcp->Cps[0] += rcp->scDC[i]*rcp->ParDC[i][_Cps_];
        rcp->Vs[0]  += rcp->scDC[i]*rcp->ParDC[i][_Vs_];
    }
    /* calc equilibrium constant K and lgK */
    T = (double)rcp->TCst + C_to_K;
    RR = R_CONSTANT; /*  !!!!! */
    R_T = RR*T;
    lgK = -rcp->Gs[0] / lg_to_ln / R_T;
    if( IsDoubleEmpty( rcp->Ks[1] ))
        rcp->Ks[1] = lgK;
    rcp->Ks[2] = lgK - rcp->Ks[1]; /* !0 if insert lgK */
    rcp->Ks[1] = lgK;
    rcp->Ks[0] = pow( 10., lgK );
}

/*-----------------------------------------------------------------*/
void TReacDC::Recalc_ISO1( double /*foS*/ )
{
    double  LK, RR, R_T;
    int stG, stK, stLK;

    RR = R_CONSTANT; /* !!!!! */

    stG =  !IsDoubleEmpty( rcp->Gs[0] );
    stK =  !IsDoubleEmpty( rcp->Ks[0] );
    stLK=  !IsDoubleEmpty( rcp->Ks[1] );
    R_T = ( rcp->TCst + C_to_K )* RR;
    //  STAGE1:
    if( stG && !stK && !stLK )
    {  /* in Ks[1] set 1000 ln K !!!!!! */
        rcp->Ks[1] = -rcp->Gs[0]*1000./R_T;
        rcp->Ks[0] = exp( rcp->Ks[1]/1000. );
        goto STAGE2;
    }
    if(stK && !stLK && !stG )
    {
        rcp->Ks[1] = log( rcp->Ks[0] ) * 1000.;
        rcp->Gs[0] = -rcp->Ks[1] * R_T/1000.;
        goto STAGE2;
    }
    if(!stK && stLK && !stG )
    {
        rcp->Ks[0] = exp( rcp->Ks[1]/1000. );
        rcp->Gs[0] = -rcp->Ks[1] * R_T/1000.;
        goto STAGE2;
    }
    if( stK && stG || stLK && stG || stK && stLK && stG )
    {
        LK = -rcp->Gs[0]*1000./R_T;
        if( !stK )
            rcp->Ks[0] = exp( rcp->Ks[1]/1000. );
        if( !stLK )
            rcp->Ks[1] = log( rcp->Ks[0] ) * 1000.;
        if( fabs( LK-rcp->Ks[1] ) >= 0.01 )
            if( !vfQuestion(window(), GetName(),
                "W30RErun: Inconsistent dGr and logK: take logK (Y) or dGr (N)?" ))
                rcp->Ks[1] = LK;
        rcp->Gs[0] = -rcp->Ks[1] * R_T/1000.;
        rcp->Ks[0] = exp( rcp->Ks[1]/1000. );
        goto STAGE2;
    }
    if(!stK && !stLK && !stG )
        Error( GetName(),
               "W31RErun: One of values K, 1000lnK, dGr was not specified");
STAGE2:
    return;
}

/*-----------------------------------------------------------------*/
// etap 2
void TReacDC::Recalc_ISO2( double foS )
{
    double  T, LNK, G=0.0, H=0.0, S=0.0, Cp, V, RR, R_T;
    int i;
    int stG, stH, stS;

    RR = R_CONSTANT; /* !!!!! */

    stG =  !IsDoubleEmpty( rcp->Gs[0] );
    stH =  !IsDoubleEmpty( rcp->Hs[0] );
    stS =  !IsFloatEmpty( rcp->Ss[0] );
    T = (double)rcp->TCst + C_to_K;
    R_T = T * RR;
    LNK = log( rcp->Ks[0] );
    if( stG && stH && !stS )
    {
        for( i=0; i<rcp->nDC-1; i++ )
        {
            G  += rcp->scDC[i]*(rcp->ParDC[i][_Gs_]+rcp->Nix[i]*R_T*LNK);
            H  += rcp->scDC[i]*rcp->ParDC[i][_Hs_];
        }
        rcp->Gs[1] = rcp->ParDC[rcp->nDC-1][_Gs_] =
                         -G / rcp->scDC[rcp->nDC-1];
        rcp->Hs[1] = rcp->ParDC[rcp->nDC-1][_Hs_] =
                         (rcp->Hs[0]-H) / rcp->scDC[rcp->nDC-1];
        rcp->Ss[1] = rcp->ParDC[rcp->nDC-1][_Ss_] =
                         (rcp->ParDC[rcp->nDC-1][_Hs_] -
                          rcp->ParDC[rcp->nDC-1][_Hs_]) / T + foS;
        for( i=0; i<rcp->nDC; i++ )
            rcp->Ss[0]  += rcp->scDC[i]*rcp->ParDC[i][_Ss_];
        goto FINITA;
    }
    if( stG && !stH && stS )
    {
        for( i=0; i<rcp->nDC-1; i++ )
        {
            G  += rcp->scDC[i]*(rcp->ParDC[i][_Gs_]+rcp->Nix[i]*R_T* LNK);
            S  += rcp->scDC[i]*(rcp->ParDC[i][_Ss_]+rcp->Nix[i]*rcp->Ss[0]);
        }
        rcp->Gs[1] = rcp->ParDC[rcp->nDC-1][_Gs_] =
                         -G/rcp->scDC[rcp->nDC-1];
        rcp->Ss[1] = rcp->ParDC[rcp->nDC-1][_Ss_] =
                         -S/rcp->scDC[rcp->nDC-1] /* - foS */ ;
        rcp->Hs[1] = rcp->ParDC[rcp->nDC-1][_Hs_] =
                         rcp->ParDC[rcp->nDC-1][_Gs_] +
                         ( rcp->ParDC[rcp->nDC-1][_Ss_] - foS ) * T;
        for( i=0; i<rcp->nDC; i++ )
            rcp->Hs[0]  += rcp->scDC[i]*rcp->ParDC[i][_Hs_];
        goto FINITA;
    }
    if( !stG && stH && stS )
    {
        for( i=0; i<rcp->nDC-1; i++ )
        {
            /* S  += rcp->scDC[i]*rcp->SsDC[i]; */
            S  += rcp->scDC[i]*(rcp->ParDC[i][_Ss_]+rcp->Nix[i]*rcp->Ss[0]);
            H  += rcp->scDC[i]*rcp->ParDC[i][_Hs_];
        }
        rcp->Ss[1] = rcp->ParDC[rcp->nDC-1][_Ss_] =
                         -S/rcp->scDC[rcp->nDC-1] /* - foS */;
        rcp->Hs[1] = rcp->ParDC[rcp->nDC-1][_Hs_] =
                         (rcp->Hs[0] -H)/rcp->scDC[rcp->nDC-1];
        rcp->Gs[1] = rcp->ParDC[rcp->nDC-1][_Gs_] =
                         rcp->ParDC[rcp->nDC-1][_Hs_] -
                         (rcp->ParDC[rcp->nDC-1][_Ss_] - foS ) *T;
        for( i=0; i<rcp->nDC; i++ )
        {
            if(i< rcp->nDC-1)
                rcp->Gs[0] += rcp->scDC[i]*( rcp->ParDC[i][_Gs_] +
                                             rcp->Nix[i]*R_T* LNK );
            else rcp->Gs[0]  += rcp->scDC[i]*rcp->ParDC[i][_Gs_];
        }
        goto FINITA;
    }
    if( stG && stH && stS )
    {
        for( i=0; i<rcp->nDC-1; i++ )
        {
            G  += rcp->scDC[i]*(rcp->ParDC[i][_Gs_]+rcp->Nix[i]* R_T* LNK);
            S  += rcp->scDC[i]*(rcp->ParDC[i][_Ss_]+rcp->Nix[i]*rcp->Ss[0]);
            H  += rcp->scDC[i]*rcp->ParDC[i][_Hs_];
        }
        rcp->Gs[1] = rcp->ParDC[rcp->nDC-1][_Gs_] =
                         -G/rcp->scDC[rcp->nDC-1];
        rcp->Ss[1] = rcp->ParDC[rcp->nDC-1][_Ss_] =
                         -S/rcp->scDC[rcp->nDC-1] /* -foS */;
        rcp->Hs[1] = rcp->ParDC[rcp->nDC-1][_Hs_] =
                         (rcp->Hs[0] - H)/rcp->scDC[rcp->nDC-1];
        goto FINITA;
    }
    Error( GetName(),"W32RErun: One of values dGr, dHr, or dSr is missing (Iso2)");
FINITA:
    /*calc Cp and  V for vedushiy component */
    Cp = 0.0;
    V = 0.0;
    if( IsFloatEmpty( rcp->Cps[0] ))
        rcp->Cps[0] = 0.0;
    if( IsFloatEmpty( rcp->Vs[0] ))
        rcp->Vs[0] = 0.0;
    for( i=0; i<rcp->nDC-1; i++ )
    {
        Cp += rcp->scDC[i]*rcp->ParDC[i][_Cps_];
        V  += rcp->scDC[i]*rcp->ParDC[i][_Vs_];
    }
    rcp->Cps[1] = rcp->ParDC[rcp->nDC-1][_Cps_] =
                      (rcp->Cps[0] - Cp)/rcp->scDC[rcp->nDC-1];
    rcp->Vs[1]  = rcp->ParDC[rcp->nDC-1][_Vs_] =
                      (rcp->Vs[0] - V)/rcp->scDC[rcp->nDC-1];
}

/*-----------------------------------------------------------------*/
// Calc parametres of reaction if vedushiy d or r
void TReacDC::Calc_rDCD( int q, int p )
{
    int i;
    double RR = R_CONSTANT, R_T;

    if( fabs( aW.WW(p).TC - rc[q].TCst ) < 1 )
    {
        aW.WW(p).K =   rc[q].Ks[0];
        aW.WW(p).lgK = rc[q].Ks[1];
        aW.WW(p).dG =  rc[q].Gs[0];
        aW.WW(p).G  =  rc[q].Gs[1];
        aW.WW(p).dH =  rc[q].Hs[0];
        aW.WW(p).H  =  rc[q].Hs[1];
        aW.WW(p).dS =  rc[q].Ss[0];
        aW.WW(p).S  =  rc[q].Ss[1];
        aW.WW(p).dV =  rc[q].Vs[0];
        aW.WW(p).V  =  rc[q].Vs[1];
        aW.WW(p).dCp = rc[q].Cps[0];
        aW.WW(p).Cp =  rc[q].Cps[1];
    }
    else
    {
        /* copy values last "vedushego" DC */
        aW.WW(p).G = rc[q].ParDC[rc[q].nDC-1][_Gs_];
        aW.WW(p).S = rc[q].ParDC[rc[q].nDC-1][_Ss_];
        aW.WW(p).H = rc[q].ParDC[rc[q].nDC-1][_Hs_];
        aW.WW(p).Cp = rc[q].ParDC[rc[q].nDC-1][_Cps_];
        aW.WW(p).V = rc[q].ParDC[rc[q].nDC-1][_Vs_];

        /* calc delta reactions */
        aW.WW(p).dG = 0.0;
        aW.WW(p).dH = 0.0;
        aW.WW(p).dS = 0.0;
        aW.WW(p).dCp = 0.0;
        aW.WW(p).dV = 0.0;
        for( i=0; i<rc[q].nDC; i++ )
        {
            aW.WW(p).dG += rc[q].scDC[i]*rc[q].ParDC[i][_Gs_];
            aW.WW(p).dH += rc[q].scDC[i]*rc[q].ParDC[i][_Hs_];
            aW.WW(p).dS += rc[q].scDC[i]*rc[q].ParDC[i][_Ss_];
            aW.WW(p).dCp += rc[q].scDC[i]*rc[q].ParDC[i][_Cps_];
            aW.WW(p).dV += rc[q].scDC[i]*rc[q].ParDC[i][_Vs_];
        }
        /* calc const ravnovesiya and it lg */
        //      T = (double)rc[q].TCst + C_to_K;
        /* RR = R_CONSTANT;  !!!!! */
        R_T = aW.WW(p).T * RR;
        aW.WW(p).lgK = -aW.WW(p).dG / lg_to_ln / R_T;
        aW.WW(p).K = pow( 10., aW.WW(p).lgK );
    }
    return;
}

/*-----------------------------------------------------------------*/
// Calc aqueous species
void TReacDC::calc_tphkf_r( int q, int p )
{
    aW.WW(p).K =   rc[q].Ks[0];
    aW.WW(p).lgK = rc[q].Ks[1];
    aW.WW(p).dlgK =rc[q].Ks[2];
    aW.WW(p).dG =  rc[q].Gs[0];
    aW.WW(p).G  =  rc[q].Gs[1];
    aW.WW(p).devG =rc[q].Gs[2];
    aW.WW(p).dH =  rc[q].Hs[0];
    aW.WW(p).H  =  rc[q].Hs[1];
    aW.WW(p).devH =rc[q].Hs[2];
    aW.WW(p).dS =  rc[q].Ss[0];
    aW.WW(p).S  =  rc[q].Ss[1];
    aW.WW(p).devS =rc[q].Ss[2];
    aW.WW(p).dV =  rc[q].Vs[0];
    aW.WW(p).V  =  rc[q].Vs[1];
    aW.WW(p).devV =rc[q].Vs[2];
    aW.WW(p).dCp = rc[q].Cps[0];
    aW.WW(p).Cp =  rc[q].Cps[1];
    aW.WW(p).devCp=rc[q].Cps[2];
}


/*-----------------------------------------------------------------*/
// Calculation of d(reaction) from logK = f(T)
void TReacDC::calc_lgk_r( int q, int p, int CE, int /*CV*/ )
{
    double Rln10, T, lnT, T_2, T_05, /*T_15,*/ T_3, /*DH,*/ Tr, //dGr_d,
        dSr, dGr, dHr, dCpr, dVr, lgK, R_T; /* Units of measurement!!!!! */
    float *A, *Cp;

    Rln10 = R_CONSTANT * lg_to_ln;
    R_T = aW.WW(p).T * R_CONSTANT;  /* R_T = aW.WW(p).RT; */
    if( fabs( aW.WW(p).TC - rc[q].TCst ) < 0.1 )
    {
        aW.WW(p).K =   rc[q].Ks[0];
        aW.WW(p).lgK = rc[q].Ks[1];
        aW.WW(p).dG =  rc[q].Gs[0];
        aW.WW(p).G  =  rc[q].Gs[1];
        aW.WW(p).dH =  rc[q].Hs[0];
        aW.WW(p).H  =  rc[q].Hs[1];
        aW.WW(p).dS =  rc[q].Ss[0];
        aW.WW(p).S  =  rc[q].Ss[1];
        aW.WW(p).dV =  rc[q].Vs[0];
        aW.WW(p).V  =  rc[q].Vs[1];
        aW.WW(p).dCp = rc[q].Cps[0];
        aW.WW(p).Cp =  rc[q].Cps[1];
        goto FINITA;
    }
    Tr = rc[q].TCst+273.15;
    T = aW.WW(p).T;
    lnT = log( T );
    T_2 = T*T;
    T_3 = T_2*T;
    T_05 = sqrt( T );
//    T_15 = pow( T, 1.5 );
    A = rc[q].pKt;
    Cp = rc[q].DCp;
    dVr = rc[q].Vs[0];   //Gr = rc[q].Gs[0];
//    dGr = rc[q].Gs[0];
    dHr = rc[q].Hs[0];
    dSr = rc[q].Ss[0];
    dCpr = rc[q].Cps[0];
    lgK = rc[q].Ks[1];
//    if( ( CE == CTM_LGK || CE == CTM_LGX || CE == CTM_EK1 || CE == CTM_EK2
//            || CE == CTM_EK3 ) && rc[q].pKt )
    // Re-written by KD on 15.07.03 to implement partial 1,2 and 3 term
    /*  else if( CE == CTM_EK1 ) Only dG(Tr)=const was specified * 05.12.96
         lgK = -Gr/R_T/lg_to_ln; */
    switch (CE)
    {
       case CTM_EK0: // 1-term lgK = const
// lgK = A[0];
            dCpr = 0.0;
            dHr = 0.0;
//            dGr_d = -dSr * T;
            break;
       case CTM_EK1: // 1-term dGr = const
// lgK = A[2]/T;
            dCpr = 0.0;
            dSr = 0.0;
//            dGr_d = dHr;
            lgK = - dHr / T / Rln10;
          break;
       case CTM_EK2:  // 2-term or 1-term lgK=const at dHr=0
// lgK = A[0] + A[2]/T;
            dCpr = 0.0;
//            dGr_d = dHr - dSr * T;
            lgK = (dSr - dHr/T ) / Rln10;
          break;
       case CTM_EK3:  // 3-term
// lgK = A[0] + A[2]/T + A[3] * lnT;
            lgK = ( dSr - dHr/T - dCpr * ( 1 - Tr/T - log( T/Tr ))) / Rln10;
            dSr += dCpr * log( T / Tr );
            dHr += dCpr * (T - Tr );
//            dGr_d = dHr - dSr * T;
          break;
       case CTM_LGK:  // full 7-term logK approx
       case CTM_LGX:  // (derived from dCp=f(T))
            lgK = A[0] + A[1] * T + A[2]/T + A[3] * lnT + A[4] / T_2 +
                  A[5] * T_2 + A[6] / T_05;
            dHr = Rln10 *( A[1]*T_2 - A[2] + A[3]*T - 2.0*A[4]/T
                     + 2.0*A[5]*T_3 - 0.5*A[6]*T_05 );
            dSr = Rln10 * ( A[0] + 2.0*A[1]*T + A[3]*(1.0+lnT) -
                       A[4]/T_2 + 3.0*A[5]*T_2 + 0.5*A[6]/T_05 );
//            dGr_d = dHr - dSr * T;
//         if( rc[q].DCp )
            dCpr = Cp[0] + Cp[1]*T + Cp[2]/T_2 + Cp[4]*T_2 + Cp[3]/T_05;
//            dHr = Rln10 * T_2 * ( A[1] - A[2]/T_2 + A[3]/T -
//                             2.0*A[4]/T_3 + 2.0*A[5]*T - 0.5*A[6]/T_15 );
//            dSr = Rln10 * ( A[0] + 2.0*A[1]*T + A[3]*(1.0+lnT) -
//                       A[4]/T_2 + 3.0*A[5]*T_2 + 0.5*A[6]/T_05 );
          break;
       default:
       ; // error message ?
    }
    // Calculation of dGr
    dGr = -R_T * lgK * lg_to_ln;
    /* Loading output data */
    aW.WW(p).lgK = lgK;
    if( fabs(lgK) < 34. )
        aW.WW(p).K = exp( lgK*lg_to_ln );
    else aW.WW(p).K = FLOAT_EMPTY;
//
    aW.WW(p).dG = dGr;
    aW.WW(p).dH =  dHr;
    aW.WW(p).dS = dSr;
    aW.WW(p).dCp = dCpr;
  /* Checking consistency of Gr, Hr and Sr,  Gr == Hr - T*Sr; */
//    DH = dGr + T*dSr;
//    if( fabs( dGr - dGr_d ) > 1. || fabs( DH - dHr ) > 57.08 )  // J/mol
//    {
//       cout << "\nlgK_r: DH=" << DH << " | " << dHr << " ;   dGr=" << dGr_d;
 //      cout << " rKey:" /* << aW.WW(p).DRkey */ << rc[q].name ;
//      To add an error message ?
//    }
  // Correction for pressure at constant dVr ?
    aW.WW(p).dV = dVr;

 FINITA: // Copying uncertainties
    aW.WW(p).dlgK =rc[q].Ks[2];
    aW.WW(p).devG = rc[q].Gs[2];
    aW.WW(p).devH =rc[q].Hs[2];
    aW.WW(p).devS =rc[q].Ss[2];
    aW.WW(p).devV =rc[q].Vs[2];
    aW.WW(p).devCp=rc[q].Cps[2];
}

/*-----------------------------------------------------------------*/
// Interpolation on tabulated values of logK for TP pairs
void TReacDC::calc_r_interp( int q, int p, int /*CE*/, int /*CV*/ )
{
    int nP;
    double Pa, lgK, RR = R_CONSTANT, R_T; /* !!!!! */
    R_T = aW.WW(p).T * RR;
    /* R_T = aW.WW(p).RT; */
    if( fabs( aW.WW(p).TC - rc[q].TCst ) < 0.2 )
    {
        aW.WW(p).K =   rc[q].Ks[0];
        aW.WW(p).lgK = rc[q].Ks[1];
        aW.WW(p).dG =  rc[q].Gs[0];
        aW.WW(p).G  =  rc[q].Gs[1];
        aW.WW(p).dH =  rc[q].Hs[0];
        aW.WW(p).H  =  rc[q].Hs[1];
        aW.WW(p).dS =  rc[q].Ss[0];
        aW.WW(p).S  =  rc[q].Ss[1];
        aW.WW(p).dV =  rc[q].Vs[0];
        aW.WW(p).V  =  rc[q].Vs[1];
        aW.WW(p).dCp = rc[q].Cps[0];
        aW.WW(p).Cp =  rc[q].Cps[1];
        goto FINITA;
    }
    Pa = aW.WW(p).P;
    switch( rc[q].nPp )
    {
    case 0:
    case 1:
        if( Pa >= rc[q].Pint[0] && Pa < rc[q].Pint[1] )
            Pa = rc[q].Pint[0];
        else
            Error( GetName(),"E33RErun: Invalid Pa in calc_r_interp()");
        nP = 1;
        break;
    case 2:
        if( Pa >= rc[q].Pint[0] && Pa < rc[q].Pint[1] )
            Pa = rc[q].Pint[0];
        else if( Pa >= rc[q].Pint[1] )
            Pa = rc[q].Pint[1];
        nP = 1;
        break;
    default:
        nP = rc[q].nPp;
        break;
    }
    /* call interpolations */
    lgK = LagranInterp(  rc[q].Pint, rc[q].TCint, rc[q].logK,
                Pa, aW.WW(p).TC, rc[q].nTp, nP, 1 );
    //  if( lgK > 7777776. )
    //    return 1;
    aW.WW(p).lgK = lgK;
    if( fabs(lgK) > 34 )
        aW.WW(p).K = exp( lgK*lg_to_ln );
    else aW.WW(p).K = FLOAT_EMPTY;
    aW.WW(p).dG = -R_T* lgK * lg_to_ln;
    /* Recalc dH and Pr. - to complete ! */
    aW.WW(p).dH =  rc[q].Hs[0];
    aW.WW(p).dS =  rc[q].Ss[0];
    aW.WW(p).dV =  rc[q].Vs[0];
    aW.WW(p).dCp = rc[q].Cps[0];
FINITA:
    aW.WW(p).dlgK =rc[q].Ks[2];
    aW.WW(p).devG = rc[q].Gs[2];
    aW.WW(p).devH =rc[q].Hs[2];
    aW.WW(p).devS =rc[q].Ss[2];
    aW.WW(p).devV =rc[q].Vs[2];
    aW.WW(p).devCp=rc[q].Cps[2];
    return;
}

//-----------------------------------------------------------------
// Calculation of deltaR with modified Ryzhenko-Bryzgalin
// model (added by TW and DK on 3.08.2007)

void TReacDC::calc_r_MRB( int q, int p, int /*CE*/, int /*CV*/ )
{
    double TK;
    double H2Oprop[4];
    double MRBcoef[3];
    double ReactProp[6];
    double rhoW, alphaW, betaW, dAldTW;

    if( fabs( aW.WW(p).TC - rc[q].TCst ) < 0.2 )
    {  // standard temperature - just get data from ReacDC record
       aW.WW(p).K =   rc[q].Ks[0];
       aW.WW(p).lgK = rc[q].Ks[1];
       aW.WW(p).dG =  rc[q].Gs[0];
       aW.WW(p).G  =  rc[q].Gs[1];
       aW.WW(p).dH =  rc[q].Hs[0];
       aW.WW(p).H  =  rc[q].Hs[1];
       aW.WW(p).dS =  rc[q].Ss[0];
       aW.WW(p).S  =  rc[q].Ss[1];
       aW.WW(p).dV =  rc[q].Vs[0];
       aW.WW(p).V  =  rc[q].Vs[1];
       aW.WW(p).dCp = rc[q].Cps[0];
       aW.WW(p).Cp =  rc[q].Cps[1];
        goto FINITA;
    }

     TK = aW.WW(p).T;
//     P = aW.WW(p).P;
       rhoW = aW.WW(p).wRo;
       alphaW = aW.WW(p).wAlp;
       dAldTW = aW.WW(p).wdAlpdT;
       betaW = aW.WW(p).wBet;
    H2Oprop[0] = rhoW;
    H2Oprop[1] = alphaW;
    H2Oprop[2] = dAldTW;
    H2Oprop[3] = betaW;

// get species parameters
    MRBcoef[0] = rc[q].DSt[0];
    MRBcoef[1] = rc[q].DSt[1];
    MRBcoef[2] = rc[q].DSt[2];

// calculate results - call MRB function (see below)
    MRBcalc (TK, H2Oprop, MRBcoef, ReactProp);

//       aW.WW(p).K =   rc[q].Ks[0];
       aW.WW(p).lgK = ReactProp[0];
       if( fabs(aW.WW(p).lgK) < 34. )
         aW.WW(p).K = exp( aW.WW(p).lgK*lg_to_ln );
       else aW.WW(p).K = FLOAT_EMPTY;
       aW.WW(p).dG =  ReactProp[1];
       aW.WW(p).dH =  ReactProp[3];
       aW.WW(p).dS =  ReactProp[2];
       aW.WW(p).dV =  ReactProp[5];
       aW.WW(p).dCp = ReactProp[4];

FINITA:
    aW.WW(p).dlgK =rc[q].Ks[2];
    aW.WW(p).devG = rc[q].Gs[2];
    aW.WW(p).devH =rc[q].Hs[2];
    aW.WW(p).devS =rc[q].Ss[2];
    aW.WW(p).devV =rc[q].Vs[2];
    aW.WW(p).devCp=rc[q].Cps[2];
}

// -------------------------------
// Written 10/07/2007 by T. Wagner
// calculates the pK of aqueous species from the MRB model
// parameters: temperature in K, MRB parameters (pK298, Azz, Bzz),
// H2O properties (density, alpha, temperature derivative of alpha, beta)

int
TReacDC::MRBcalc( double TK, double *H2Oprop, double *MRBcoef, double *ReactProp )
{
	// calculates reaction properties from MRB model
	double a, b, c, d, e, f, g, RHO298;
	double R_C = 8.31451;
	double RHO, ALP, dALPdT, BET, dRHOdT, d2RHOdT2, dRHOdP;
	double pK298, A, B;
	double dGr, dSr, dHr, dCPr, dVr, dG298;
	double pKw298, pKwTP, zzH2O, zzA, BB, pKTP, logKTP;
	double I, J, dI, dJ, d2I, d2J;

	RHO = H2Oprop[0];
	ALP = H2Oprop[1];
	dALPdT = H2Oprop[2];
	BET = H2Oprop[3];

	pK298 = MRBcoef[0];
	A = MRBcoef[1];
	B = MRBcoef[2];

	dRHOdT = -ALP*RHO;
	d2RHOdT2 = RHO *(pow(ALP,2.)-dALPdT);
	dRHOdP = BET*RHO;

	a = -4.098;
	b = -3245.2;
	c = 2.2362e+5;
	d = -3.984e+7;
	e = 13.957;
	f = -1262.3;
	g = 8.5641e+5;
	RHO298 = 0.997061;
	zzH2O = 1.0107;

	pKw298 = -(a + b/298.15 + c/pow(298.15,2.) + d/pow(298.15,3.)
		+ ( e + f/298.15 + g/pow(298.15,2.))*log10(RHO298));

	pKwTP = -(a + b/TK + c/pow(TK,2.) + d/pow(TK,3.)
		+ ( e + f/TK + g/pow(TK,2.))*log10(RHO));

	zzA = A + B/TK;
	BB = ( pKwTP - pKw298*(298.15)/TK ) / zzH2O;
	pKTP = pK298*(298.15/TK ) + zzA*BB;
	logKTP = - pKTP;
	dGr = pKTP*log(10.)*R_C*TK;
	dG298 = pK298*log(10.)*R_C*298.15;

	I = a*TK + b + c/TK + d/pow(TK,2.)
		+ ( e*TK + f + g/TK ) * log10(RHO) - (-pKw298)*298.15;
	J = A + B/TK;
	dI = a - c/pow(TK,2.) - 2.*d/pow(TK,3.) + ( e - g/pow(TK,2.) ) * log10(RHO)
		+ ( e*TK + f + g/TK) * 1./(RHO*log(10.)) * dRHOdT;
	dJ = -B/pow(TK,2.);
	d2I = 2.*c/pow(TK,3.) + 6.*d/pow(TK,4.) + 2.*g/pow(TK,3.)*log10(RHO)
		+ 2.*( e - g/pow(TK,2.) ) * 1./(RHO*log(10.)) * dRHOdT
		- ( e*TK + f + g/TK ) * 1./(pow(RHO,2.)*log(10.)) * pow(dRHOdT,2.)
		+ ( e*TK + f + g/TK ) * 1./(RHO*log(10.)) * d2RHOdT2;
	d2J = 2.*B/pow(TK,3.);

	dSr = R_C*log(10.)/zzH2O * ( dI*J + dJ*I );
	dCPr = R_C*log(10.)/zzH2O * TK * ( d2I*J + 2.*dI*dJ + d2J*I );
	dVr = - R_C*log(10.)/zzH2O * ( e*TK + f + g/TK ) * 1./(RHO*log(10.))
		* dRHOdP * ( A + B/TK);
	dHr = dGr + dSr*TK;

	ReactProp[0] = logKTP;
	ReactProp[1] = dGr;
	ReactProp[2] = dSr;
	ReactProp[3] = dHr;
	ReactProp[4] = dCPr;
	ReactProp[5] = dVr;

	return 0;
}


//-----------------------------------------------------------------
// Calculation of deltaR with Franck-Marshall density model
// (added by TW and DK on 29.01.2008

void TReacDC::calc_r_FMD( int q, int p, int /*CE*/, int /*CV*/ )
{
    double TK;
    double H2Oprop[4];
    double MFDcoef[7];
    double ReactProp[6];
    double rhoW, alphaW, betaW, dAldTW;

    if( fabs( aW.WW(p).TC - rc[q].TCst ) < 0.2 )
    {  // standard temperature - just get data from ReacDC record
       aW.WW(p).K =   rc[q].Ks[0];
       aW.WW(p).lgK = rc[q].Ks[1];
       aW.WW(p).dG =  rc[q].Gs[0];
       aW.WW(p).G  =  rc[q].Gs[1];
       aW.WW(p).dH =  rc[q].Hs[0];
       aW.WW(p).H  =  rc[q].Hs[1];
       aW.WW(p).dS =  rc[q].Ss[0];
       aW.WW(p).S  =  rc[q].Ss[1];
       aW.WW(p).dV =  rc[q].Vs[0];
       aW.WW(p).V  =  rc[q].Vs[1];
       aW.WW(p).dCp = rc[q].Cps[0];
       aW.WW(p).Cp =  rc[q].Cps[1];
        goto FINITA;
    }

     TK = aW.WW(p).T;
//     P = aW.WW(p).P;
       rhoW = aW.WW(p).wRo;
       alphaW = aW.WW(p).wAlp;
       dAldTW = aW.WW(p).wdAlpdT;
       betaW = aW.WW(p).wBet;
    H2Oprop[0] = rhoW;
    H2Oprop[1] = alphaW;
    H2Oprop[2] = dAldTW;
    H2Oprop[3] = betaW;

// get species parameters
    MFDcoef[0] = rc[q].DSt[0];
    MFDcoef[1] = rc[q].DSt[1];
    MFDcoef[2] = rc[q].DSt[2];
    MFDcoef[3] = rc[q].DSt[3];
    MFDcoef[4] = rc[q].DSt[4];
    MFDcoef[5] = rc[q].DSt[5];
    MFDcoef[6] = rc[q].DSt[6];

    // calculate results - call MRB function (see below)
    MFDcalc (TK, H2Oprop, MFDcoef, ReactProp);

//       aW.WW(p).K =   rc[q].Ks[0];
       aW.WW(p).lgK = ReactProp[0];
       if( fabs(aW.WW(p).lgK) < 34. )
         aW.WW(p).K = exp( aW.WW(p).lgK*lg_to_ln );
       else aW.WW(p).K = FLOAT_EMPTY;
       aW.WW(p).dG =  ReactProp[1];
       aW.WW(p).dH =  ReactProp[3];
       aW.WW(p).dS =  ReactProp[2];
       aW.WW(p).dV =  ReactProp[5];
       aW.WW(p).dCp = ReactProp[4];

FINITA:
    aW.WW(p).dlgK =rc[q].Ks[2];
    aW.WW(p).devG = rc[q].Gs[2];
    aW.WW(p).devH =rc[q].Hs[2];
    aW.WW(p).devS =rc[q].Ss[2];
    aW.WW(p).devV =rc[q].Vs[2];
    aW.WW(p).devCp=rc[q].Cps[2];
}

int
TReacDC::MFDcalc( double TK, double *H2Oprop, double *MFDcoef, double *ReactProp )
{
	// calculates reaction properties from Marshall-Franck density (MFD) model
	double a, b, c, d, e, f, g;
	double R_C = 8.31451;
	double RHO, ALP, dALPdT, BET, dRHOdT, d2RHOdT2, dRHOdP;
	double logK, dGr, dSr, dHr, dCPr, dVr;
	double J, dJ, d2J;

	RHO = H2Oprop[0];
	ALP = H2Oprop[1];
	dALPdT = H2Oprop[2];
	BET = H2Oprop[3];
	dRHOdT = -ALP*RHO;
	dRHOdP = BET*RHO;
	d2RHOdT2 = RHO*(pow(ALP,2.)-dALPdT);

	a = MFDcoef[0];
	b = MFDcoef[1];
	c = MFDcoef[2];
	d = MFDcoef[3];
	e = MFDcoef[4];
	f = MFDcoef[5];
	g = MFDcoef[6];

	logK = a + b/TK + c/pow(TK,2.) + d/pow(TK,3.) + ( e + f/TK + g/pow(TK,2.) )*log10(RHO);

	J = a + b/TK + c/pow(TK,2.) + d/pow(TK,3.) + ( e + f/TK + g/pow(TK,2.) )*log10(RHO);
	dJ = - b/pow(TK,2.) - 2.*c/pow(TK,3.) - 3.*d/pow(TK,4.)
		+ ( -f/pow(TK,2.) - 2.*g/pow(TK,3.) )*log10(RHO)
		+ ( e + f/TK + g/pow(TK,2.) ) * 1./(RHO*log(10.)) * dRHOdT;
	d2J = 2.*b/pow(TK,3.) + 6.*c/pow(TK,4.) + 12.*d/pow(TK,5.)
		+ ( 2.*f/pow(TK,3.) + 6.*g/pow(TK,4.) )*log10(RHO)
		+ 2.*( -f/pow(TK,2.) -2.*g/pow(TK,3.) ) * 1./(RHO*log(10.)) * dRHOdT
		- ( e + f/TK + g/pow(TK,2.) ) * 1./(pow(RHO,2.)*log(10.)) * pow (dRHOdT,2.)
		+ ( e + f/TK + g/pow(TK,2.) ) * 1./(RHO*log(10.)) * d2RHOdT2;

	dGr = - R_C*TK*log(10.)*logK;
	dSr = R_C*log(10.)*( J + dJ*TK );
	dCPr = R_C*log(10.)*TK*( 2.*dJ + d2J*TK );
	dVr = - R_C*log(10.)*TK*( ( e + f/TK + g/pow(TK,2.) ) * 1./(RHO*log(10.)) * dRHOdP );
	dHr = dGr + dSr*TK;

	ReactProp[0] = logK;
	ReactProp[1] = dGr;
	ReactProp[2] = dSr;
	ReactProp[3] = dHr;
	ReactProp[4] = dCPr;
	ReactProp[5] = dVr;

	return 0;
}


/*-----------------------------------------------------------------*/
// Calc parametres for isotop form if povishen TP
void TReacDC::calc_iso_a( int q, int p )
{
    double lna1000, RR = R_CONSTANT, R_T; /*!!!!! */
    R_T = aW.WW(p).T * RR;
    /* R_T = aW.WW(p).RT; */
    if( fabs( aW.WW(p).TC - rc[q].TCst ) < 0.1 )
    {
        aW.WW(p).K =   rc[q].Ks[0];
        aW.WW(p).lgK = rc[q].Ks[1];
        aW.WW(p).dG =  rc[q].Gs[0];
        aW.WW(p).G  =  rc[q].Gs[1];
        aW.WW(p).H  =  rc[q].Hs[1];
        aW.WW(p).S  =  rc[q].Ss[1];
        aW.WW(p).V  =  rc[q].Vs[1];
        aW.WW(p).Cp =  rc[q].Cps[1];
        goto FINITA;
    }
    if( rc[q].PrIso != S_OFF && rc[q].pKt )
    { /* calc zavisimosti 1000*lnK = f(T) */
        lna1000 = rc[q].pKt[0] +
                  rc[q].pKt[1] / aW.WW(p).T +
                  rc[q].pKt[2] / (aW.WW(p).T * aW.WW(p).T) +
                  rc[q].pKt[3] / pow( aW.WW(p).T, 3.0 ) +
                  rc[q].pKt[4] / pow( aW.WW(p).T, 4.0 );
        /*Calc parametres reaction dH, dS, dC  Do it!!!!!! */
    }
    else
    { /* We have only alfa for standart T */
        lna1000 = rc[q].Ks[1];
    }
    aW.WW(p).lgK = lna1000;
    aW.WW(p).K = exp( lna1000/1000.);
    aW.WW(p).dG = -R_T*lna1000/1000.;
FINITA:
    aW.WW(p).dlgK = rc[q].Ks[2];
    aW.WW(p).devG = rc[q].Gs[2];
    aW.WW(p).dH =   rc[q].Hs[0];
    aW.WW(p).devH = rc[q].Hs[2];
    aW.WW(p).dS =   rc[q].Ss[0];
    aW.WW(p).devS = rc[q].Ss[2];
    aW.WW(p).dV =   rc[q].Vs[0];
    aW.WW(p).devV = rc[q].Vs[2];
    aW.WW(p).dCp =  rc[q].Cps[0];
    aW.WW(p).devCp= rc[q].Cps[2];
}

/*-----------------------------------------------------------------*/
void TReacDC::calc_exion_r( int q, int p )
{
    aW.WW(p).K =   rc[q].Ks[0];
    aW.WW(p).lgK = rc[q].Ks[1];
    aW.WW(p).dlgK =rc[q].Ks[2];
    aW.WW(p).dG =  rc[q].Gs[0];
    aW.WW(p).G  =  rc[q].Gs[1];
    aW.WW(p).devG =rc[q].Gs[2];
    aW.WW(p).dH =  rc[q].Hs[0];
    aW.WW(p).H  =  rc[q].Hs[1];
    aW.WW(p).devH =rc[q].Hs[2];
    aW.WW(p).dS =  rc[q].Ss[0];
    aW.WW(p).S  =  rc[q].Ss[1];
    aW.WW(p).devS =rc[q].Ss[2];
    aW.WW(p).dV =  rc[q].Vs[0];
    aW.WW(p).V  =  rc[q].Vs[1];
    aW.WW(p).devV =rc[q].Vs[2];
    aW.WW(p).dCp = rc[q].Cps[0];
    aW.WW(p).Cp =  rc[q].Cps[1];
    aW.WW(p).devCp=rc[q].Cps[2];
}

/*-----------------------------------------------------------------*/
//  Calc volume if calc params of reaction
void TReacDC::calc_tpcv_r( int q, int /*p*/, int /*CM*/, int CV )
{
    double a, T,  Vst, Tst, Pst, P_Pst, T_Tst, Ts2, dT,
    VP, VT, aC, aE;
    int i;
    /* set standart values */
    Pst = (double)rc[q].Pst;
    Vst = (double)rc[q].Vs[0];
    if( IsDoubleEmpty( Vst ))
        Vst = 0;
    dT = TK_DELTA; //TC = aW.twp->TC;
    aW.twp->T =    T = aW.twp->TC + dT;
    aW.twp->Tst =  Tst = (double)rc[q].TCst + dT;
    //T2 = T * T;  //T3 = T2 * T;   //T4 = T3 * T;   //T05 = sqrt( T );
    //Tst2 = Tst * Tst;  //Tst3 = Tst2 * Tst;
    //Tst4 = Tst3 * Tst; //Tst05 = sqrt( Tst );
    T_Tst = T - Tst;
    Ts2 = T_Tst * T_Tst;   //TT = T / Tst;

    /* set begin values */
    aW.twp->dV = Vst;
    if(( rc[q].pstate[0] == CP_GAS || rc[q].pstate[0] == CP_GASI ) && aW.twp->P > 0.0 )
    { /* molar volume from the ideal gas law */
        aW.twp->dV = T / aW.twp->P * R_CONSTANT;
    }

/*
    if( CV == CPM_CEH )
    { // calc compression-deconpress
        aC = (double)rc[q].Comp;
        aE = (double)rc[q].Expa;
        if( !IsDoubleEmpty( aC ) && !IsDoubleEmpty( aE ))
        { // calc coef compression-deconpress
            if( fabs( aW.twp->P - Pst ) > PRESSURE_PREC ||
                    fabs( T - Tst ) > TEMPER_PREC )
            {
                aE /= Vst;
                aC /= Vst;
                aW.twp->dV  = Vst * ( 1.+ aE * T_Tst ) * ( 1.- aC * aW.twp->P );
                aW.twp->dS  -= Vst * aW.twp->P * aE * (1. - aC * aW.twp->P / 2.);
                aW.twp->dG += Vst * ( 1.+ aE * T_Tst ) * ( aW.twp->P - aC *
                              aW.twp->P*aW.twp->P / 2. );
                aW.twp->dH += aW.twp->P * Vst * ( 1.-aC * aW.twp->P/2. ) *
                              ( 1.+ aE * T_Tst - aE );
            }
        }
    }
    else
*/
    if( (CV == CPM_VKE || CV == CPM_VBE) && rc[q].DVt )
    {  /* calc on equation V(P,T) */
        aC = 0.;
        aE = 0.;
        for( i=0; i<5; i++ )
        { /* see all  coef Vp(T,P) */
            a = (double)rc[q].DVt[i];
            if( IsDoubleEmpty( a ) || !a ) continue;
            switch( i ) /* calc delta */
            {
            case 0:
                aE += a;
                break;
            case 1:
                aE += a * 2. * T_Tst;
                break;
            case 2:
                aE += a * 3. * T_Tst * T_Tst;
                break;
            case 3:
                aC -= a;
                break;
            case 4:
                aC -= a * 2. * aW.twp->P;
                break;
            }
        }
        if( fabs( aW.twp->P - Pst ) > PRESSURE_PREC ||
                fabs( T - Tst ) > TEMPER_PREC )
        { /* Calc mol volume and P */
            P_Pst = aW.twp->P - Pst;
            VP = Vst * P_Pst;
            VT = Vst * T_Tst;
            aW.twp->dG += VP;
            aW.twp->dH += VP;
            for( i=0; i<MAXVTCOEF; i++ )
            {
                a = (double)rc[q].DVt[i];
                if( IsDoubleEmpty( a ) || !a ) continue;
                switch( i ) /*calc delta coef Vp(T) */
                {
                case 0:
                    aW.twp->dV += a * VT;
                    /*    aW.twp->dS  -= a * VP;
                          aW.twp->dG += a * T_Tst * VP;
                          aW.twp->dH -= a * Tst * VP;  */
                    break;
                case 1:
                    aW.twp->dV += a * VT * T_Tst;
                    /*    aW.twp->dS  -= a * VP * 2. * T_Tst;
                          aW.twp->dG += a * VP * Ts2;
                          aW.twp->dH -= a * VP * ( T + Tst ) * T_Tst; */
                    break;
                case 2:
                    aW.twp->dV += a * VT * Ts2;
                    /*    aW.twp->dS  -= a * VP * 3.* Ts2;
                          aW.twp->dG += a * VP * Ts2 * T_Tst;
                          aW.twp->dH += a * VP * Ts2 * ( 2.*T + Tst ); */
                    break;
                case 3:
                    aW.twp->dV += a * VP;
                    /*    aW.twp->dG += a * VP * P_Pst / 2.;
                          aW.twp->dH += a * VP * P_Pst / 2.; */
                    break;
                case 4:
                    aW.twp->dV += a * VP * P_Pst;
                    /*    aW.twp->dG += a * VP * ( aW.twp->P*aW.twp->P - Pst*Pst ) / 3.;
                    aW.twp->dH += a * VP * ( aW.twp->P*aW.twp->P - Pst*Pst ) / 3.; */
                    break;
                }
            }
        }
        aW.twp->Alp = aC;
        aW.twp->Bet = aE;
        /*       rc[q].Comp = (float)aW.twp->Alp;
                 rc[q].Expa = (float)aW.twp->Bet;  */
    }
    if( CV == CPM_CON || CV == CPM_NUL )
    {
            P_Pst = aW.twp->P - Pst;
            VP = Vst * P_Pst;
//            VT = Vst * T_Tst;
            aW.twp->dG += VP;
            aW.twp->dH += VP;
    }
// Calculating pressure correction to logK
    aW.twp->lgK -= aW.twp->dV * (aW.twp->P - aW.twp->Pst) / aW.twp->RT / lg_to_ln;
}

// ------------------ End of m_reac2.cpp --------------------------


