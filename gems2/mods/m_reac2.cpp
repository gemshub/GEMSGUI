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
// See http://les.web.psi.ch/Software/GEMS-PSI for more information
// E-mail: gems2.support@psi.ch
//-------------------------------------------------------------------
//
#include <math.h>
#include <stdio.h>

#include "m_param.h"
#include "m_reacdc.h"
#include "m_dcomp.h"
#include "s_formula.h"
#include "service.h"
#include "visor.h"

/*-----------------------------------------------------------------*/
void TReacDC::Convert_Cp_to_KT( int CE )
{
    double Rln10, T, lnT, T_2, T_05, T_3, Sr, Hr;
    float *Cp, *K;

    Rln10 = R_CONSTANT * lg_to_ln;
    T = rcp->TCst + C_to_K;
    lnT = log( T );
    T_2 = T*T;
    T_3 = T_2*T;
    T_05 = sqrt( T );
    Sr = rcp->Ss[0];
    Hr = rcp->Hs[0];
    Cp = rcp->DCp;
    K = rcp->pKt;

    // calc coef. logK=f(T)
    K[1] = Cp[1]/(2.0*Rln10);
    K[3] = Cp[0]/Rln10;
    K[4] = Cp[2]/(2.0*Rln10);
    K[5] = Cp[4]/(6.0*Rln10);
    K[6] = -4.0*Cp[3]/Rln10;

    K[0] = Sr/Rln10 - ( Cp[1]*T + Cp[0]*(1.0+lnT) - Cp[2]/(2.0*T_2) +
                        Cp[4]*T_2/2.0 - 2.0*Cp[3]/T_05 ) / Rln10;
    K[2] = ( Cp[1]*T_2/2.0 + Cp[0]*T -Cp[2]/T + Cp[4]*T_3/3.0 +
             2.0*Cp[3]*T_05 - Hr ) / Rln10;
    switch(CE)
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
    return;
}

/*-----------------------------------------------------------------*/
void TReacDC::Convert_KT_to_Cp( int CE )
{
    double Rln10, T, lnT, T_2, T_05, T_15, T_3,
                                   Gr, Sr, Hr, Cpr=0.0, lgK;
    float *Cp, *K;

    Rln10 = R_CONSTANT * lg_to_ln;
    T = rcp->TCst + C_to_K;
    lnT = log( T );
    T_2 = T*T;
    T_3 = T_2*T;
    T_05 = sqrt( T );
    T_15 = pow( T, 1.5 );
    Cp = rcp->DCp;
    K = rcp->pKt;
    //AGAIN_CP:
    switch( CE )
    { /* calc on 2- and 3-term param approximation */
    case CTM_LGK:
    case CTM_LGX:
        break;
    case CTM_IKZ:
        return;
    case CTM_EK1: /* 1-term approx */
        K[2] = -rcp->Hs[0]/Rln10;
        K[0] =0.0;
        K[3] =0.0;
        K[1]=0.0;
        K[4]=0.0;
        K[5]=0.0;
        K[6]=0.0;
        break;
    case CTM_EK3: /* 3-term approx- it was correct from Vasilii */
        if( !Cp )
        {
            K[3] = rcp->Cps[0] / Rln10;
            K[0] = ( rcp->Ss[0] - rcp->Cps[0]*(1.+lnT) ) / Rln10;
            K[2] = ( rcp->Cps[0]*T - rcp->Hs[0] ) / Rln10;
            K[1]=0.0;
            K[4]=0.0;
            K[5]=0.0;
            K[6]=0.0;
        }
        else
        { /* 5-term approx- Sveta to PM2SEL */
            K[1] = Cp[1] / (2*Rln10);
            K[3] = Cp[0] / Rln10;
            K[4] = Cp[2] / (2*Rln10);
            K[0] = ( rcp->Ss[0] - Cp[0]*(1.+lnT)-Cp[1]*T+Cp[2]/(2*T_2) ) / Rln10;
            K[2] = ( Cp[0]*T+Cp[1]/2*T_2+Cp[2]/T-rcp->Hs[0] ) / Rln10;
            K[5] = 0.0;
            K[6] = 0.0;
        }
        break;
    case CTM_EK2: /* 2-term approx */
        K[0] = rcp->Ss[0]/Rln10;
        K[2] = -rcp->Hs[0]/Rln10;
        K[3] =0.0;
        K[1]=0.0;
        K[4]=0.0;
        K[5]=0.0;
        K[6]=0.0;
        break;
    default:
        Error( GetName(),"E23RErun: Illegal method code in Convert_KT_to_Cp()");
    }

    /* Calc lgK, Hr and Sr */
    lgK = K[0] + K[1]*T + K[2]/T + K[3]*lnT + K[4]/T_2 +
          K[5]*T_2 + K[6]/T_05;
    Hr = Rln10 * T_2 * ( K[1] - K[2]/T_2 + K[3]/T -
                         2.0*K[4]/T_3 + 2.0*K[5]*T - 0.5*K[6]/T_15 );
    Sr = Rln10 * ( K[0] + 2.0*K[1]*T + K[3]*(1.0+lnT) -
                   K[4]/T_2 + 3.0*K[5]*T_2 + 0.5*K[6]/T_05 );

    if( (CE == CTM_LGK || CE == CTM_LGX) && Cp )
    { /*calc coef  Cp */
        Cp[0] = Rln10 * K[3];
        Cp[1] = Rln10 * 2.0 * K[1];
        Cp[2] = Rln10 * 2.0 * K[4];
        Cp[4] = Rln10 * 6.0 * K[5];
        Cp[3] = -Rln10 * 0.25 * K[6];
        Cpr = Cp[0] + Cp[1]*T + Cp[2]/T_2 + Cp[4]*T_2 + Cp[3]/T_05;
    }
    else Cpr = Rln10*K[3];
    /* Checking consistency of delta-reaction data */
    Gr = Hr - T*Sr;
    if( fabs( rcp->Ss[0] - Sr ) >= 0.19 ||
            fabs( rcp->Hs[0] - Hr ) >= 57.08 ||
            fabs( rcp->Gs[0] - Gr ) >= 57.08 ||
            fabs( rcp->Ks[1] - lgK ) >= 0.01 )
    {
        gstring msgbuf = "W24RErun: Warning: calculated pK = ";
        vstr doublbuf(100);

        sprintf( doublbuf, "%g", lgK );
        msgbuf += doublbuf;
        msgbuf += ", G0 = ";
        sprintf( doublbuf, "%g", Gr );
        msgbuf += doublbuf;
        msgbuf += ", \n dSr = ";
        sprintf( doublbuf, "%g", Sr );
        msgbuf += doublbuf;
        msgbuf += " or dHr = ";
        sprintf( doublbuf, "%g", Hr );
        msgbuf += doublbuf;
        msgbuf += " values are inconsistent\n";
        msgbuf += "with input values to >0.01 pK. Insert calculated values?";
        if( vfQuestion(window(), GetName(), msgbuf.c_str() ))
        {
            rcp->Ss[0] = Sr;
            rcp->Hs[0] = Hr;
            rcp->Cps[0] = Cpr;
            rcp->Ks[1] = lgK;
            Error( GetName(),
        "E25RErun: Calculated pK, dSr or dHr are inconsistent - bailing out!");
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
    aFo.SetFormula( rc[q].form );   // set formula and analyse
    /* Calc molal ves and zaryd */
    aFo.Fmwtz( Z, MW, foS );
    if( fabs( Z ) < ZBALANCE_PREC )
        rc[q].Zz = 0.;
    else rc[q].Zz = (float)Z;
    rc[q].mwt = (float)MW;

    SetString("RC_test   Checking this ReacDC definition");
    Update();

    switch( rc[q].pstate[0] )
    { /* test phase state  */
    case CP_SOLID:
    case CP_GAS:
    case CP_UNIV:
    case CP_HCARB:
    case CP_LIQID: /* no zaryd  */
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
    case CP_MELT: /* must be zaryd */
        if( !memcmp( CHARGE_NAME, aFo.GetCn( aFo.GetIn()-1 ), 2 ))
            goto NEXT;
    }
    Error( GetName(),
    "W26RErun: Please, check stoichiometry, charge or valences in the formula.");
NEXT:
    /* test value st.mol.volume */
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

    if( CM == CTPM_REA )
    {
        if( CE == CTM_EK1 )
        {
            rc[q].Ss[0] = 0.0;
            rc[q].Hs[0] = DOUBLE_EMPTY;
            rc[q].Cps[0] = 0.0;
            if( !rc[q].pKt || rc[q].PreKT == S_OFF ) /* Temporarily !!!!!!! */
                rc[q].pKt =   (float *)aObj[ o_repkt ].Alloc( MAXCPCOEF, 1, F_);
        }
        else if( CE == CTM_EK2 )
            rc[q].Cps[0] = 0.0;
        else if( CE == CTM_LGK )
        {
            if( IsFloatEmpty( rc[q].Cps[0] ) )
                rc[q].Cps[0] = 0.0;
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
    if( CM == CTPM_CPT )
        Convert_Cp_to_KT( CE );
    if( CM == CTPM_REA )
        Convert_KT_to_Cp( CE );
    if( CM == CTPM_REA && rc[q].pct[2] == CPM_PCR && rc[q].PrAki != S_OFF )
    {   /*  Call PRONSPREP  added 19.05.98 */
        if( vfQuestion(window(), GetName(),
                       "Estimate S, H, Cp, V using PRONSPREP97 algorithm?" ))
        { /* Check if this is dissociation reaction */
            if( rc[q].scDC[rc[q].nDC-1] < 0 )
                PronsPrep( key );
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
        rcp->Ks[0] = pow( 10, rcp->Ks[1] );
        stK = 1;
        stLK = 1;
        goto STAGE2;
    }
    if( !stG && !stK && stLK )
    {
        rcp->Gs[0] = -rcp->Ks[1] * lg_to_ln * R_T;
        rcp->Ks[0] = pow( 10, rcp->Ks[1] );
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

    /* calc Cp and V for vedushiy component  */
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
// Calc delta reactions from svoystv component
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
    /* calc const ravnovesiya and it lg */
    T = (double)rcp->TCst + C_to_K;
    RR = R_CONSTANT; /*  !!!!! */
    R_T = RR*T;
    lgK = -rcp->Gs[0] / lg_to_ln / R_T;
    if( IsDoubleEmpty( rcp->Ks[1] ))
        rcp->Ks[1] = lgK;
    rcp->Ks[2] = lgK - rcp->Ks[1]; /* !0 if insert lgK */
    rcp->Ks[1] = lgK;
    rcp->Ks[0] = pow( 10, lgK );
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
                "W30Rerun: Inconsistent dGr and logK: take logK (Y) or dGr (N)?" ))
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
        aW.WW(p).K = pow( 10, aW.WW(p).lgK );
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
// Calc on Akinfiev
void TReacDC::calc_akinf_r( int q, int p, int /*CE*/, int /*CV*/ )
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
    double R = R_CONSTANT, Rln10, T, lnT, T_2, T_05, T_15, T_3,
        Sr, Gr, Hr, Cpr, Vr, lgK, R_T; /* Units of measurement!!!!! */
    float *Kt, *Cp;

    Rln10 = R_CONSTANT * lg_to_ln;
    R_T = aW.WW(p).T * R;  /* R_T = aW.WW(p).RT; */
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
    T = aW.WW(p).T;
    lnT = log( T );
    T_2 = T*T;
    T_3 = T_2*T;
    T_05 = sqrt( T );
    T_15 = pow( T, 1.5 );
    Kt = rc[q].pKt;
    Cp = rc[q].DCp;
    Vr = rc[q].Vs[0];   //Gr = rc[q].Gs[0];
    Hr = rc[q].Hs[0];
    Sr = rc[q].Ss[0];
    Cpr = rc[q].Cps[0];
    lgK = 0;

    if( ( CE == CTM_LGK || CE == CTM_LGX || CE == CTM_EK1 || CE == CTM_EK2
            || CE == CTM_EK3 ) && rc[q].pKt )
    { /* Calculation of lg K = f(T) */
        lgK = Kt[0] + Kt[1] * T + Kt[2]/T + Kt[3] * lnT + Kt[4] / T_2 +
              Kt[5] * T_2 + Kt[6] / T_05;
        /* Calculation of Hr and Sr */
        Hr = Rln10 * T_2 * ( Kt[1] - Kt[2]/T_2 + Kt[3]/T -
                             2.0*Kt[4]/T_3 + 2.0*Kt[5]*T - 0.5*Kt[6]/T_15 );
        Sr = Rln10 * ( Kt[0] + 2.0*Kt[1]*T + Kt[3]*(1.0+lnT) -
                       Kt[4]/T_2 + 3.0*Kt[5]*T_2 + 0.5*Kt[6]/T_05 );
        if( (CE == CTM_LGK || CE == CTM_LGX) && rc[q].DCp )
            Cpr = Cp[0] + Cp[1]*T + Cp[2]/T_2 + Cp[4]*T_2 + Cp[3]/T_05;
    }
    /*  else if( CE == CTM_EK1 ) Only dG(Tr)=const was specified * 05.12.96
         lgK = -Gr/R_T/lg_to_ln; */
    Gr = -R_T * lgK * lg_to_ln;
    /* Loading output data */
    aW.WW(p).lgK = lgK;
    if( fabs(lgK) < 34. )
        aW.WW(p).K = exp( lgK*lg_to_ln );
    else aW.WW(p).K = FLOAT_EMPTY;
    aW.WW(p).dG = Gr;
    /* Checking consistency of Gr, Hr and Sr,  Gr == Hr - T*Sr; */
    aW.WW(p).dH =  Hr;
    aW.WW(p).dS = Sr;
    aW.WW(p).dCp = Cpr;
    aW.WW(p).dV = Vr;
FINITA:
    aW.WW(p).dlgK =rc[q].Ks[2];
    aW.WW(p).devG = rc[q].Gs[2];
    aW.WW(p).devH =rc[q].Hs[2];
    aW.WW(p).devS =rc[q].Ss[2];
    aW.WW(p).devV =rc[q].Vs[2];
    aW.WW(p).devCp=rc[q].Cps[2];
}

/*-----------------------------------------------------------------*/
// Interpolyation on tabls values    logK for par TP
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
    lgK = lagr( rc[q].TCint, rc[q].Pint, rc[q].logK,
                Pa, aW.WW(p).TC, nP, rc[q].nTp );
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

/*-----------------------------------------------------------------*/
// Interpolyation on tabls values  (massiv y) by polinom Lagranga
float TReacDC::lagr(float *a, float *x, float *y, float x1,
                    float x2,  int p0,int p1)
{
    float s,z,s1[21];
    int pa, px, i=0, j, j1, k, ja, ja1;

    if (x2 < a[0])
        Error( GetName(), "E34RErun: x2 less than Tmin!");
    if(x1 < x[0])
        Error( GetName(), "E35RErun: x1 less than Pmin!");
    /*s=0.;*/
    pa = p1-1;
    px = p0-1;
    for(j1=0;j1<p1;j1++)   /*p1 - dlina  T? */
        if (x2 >= a[j1] && x2 <= a[j1+1])
            goto m1;
    //z=x2;
    goto m2;
m1:
    for(i=0;i<p0;i++)      /*p0 -dlina  x ? */
        if(x1 >= x[i] && x1 <= x[i+1])
            goto m;
    // z=x1;
    if(x1 <= x[p0-1])
        goto m;
m2:
    if(x2 <= a[p1-1])
        goto m;
m:
    if(i < p0-px)
        j=i;
    else j=p0-px-1;
    if(j1 >= p1-pa)
        j1=p1-pa-1;
    ja1=j1;
    for(ja=0;ja <= pa; ja++)
    {
        s=0.;
        for(i=0;i<=px;i++)
        {
            z=1; //z1=1;
            for(k=0;k<=px;k++)
                if(k!=i)
                    z*=(x1-x[k+j])/(x[i+j]-x[k+j]);
            s+=y[i+j+(j1)*p0]*z;
        }
        s1[ja]=s;
        j1++;
    }
    s=0.;
    for(i=0;i<=pa;i++)
    {
        z=1;
        for(k=0;k<=pa;k++)
            if(k!=i)
                z*=(x2-a[k+ja1])/(a[i+ja1]-a[k+ja1]);
        s+=s1[i]*z;
    }
    return(s);
}

/*-----------------------------------------------------------------*/
void TReacDC::calc_dissoc_r( int q, int p, int /*CE*/, int /*CV*/ )
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

    if( CV == CPM_CEH )
    { /* calc compression-deconpress */
        aC = (double)rc[q].Comp;
        aE = (double)rc[q].Expa;
        if( !IsDoubleEmpty( aC ) && !IsDoubleEmpty( aE ))
        { /* calc coef compression-deconpress */
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
    else if( CV == CPM_VKE && rc[q].DVt )
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
    /*     if( fabs( aC ) > 1e-18 )
             aW.twp->dCv = aW.twp->dCp - T* aW.twp->dV * aE * aE / aC; */
    /*Calc aW.twp->dU ¨ aW.twp->dF */
    /*     aW.twp->dU = aW.twp->dH - aW.twp->P*aW.twp->dV;
         aW.twp->dF = aW.twp->dG - aW.twp->P*aW.twp->dV; */
    aW.twp->lgK -= aW.twp->dV * (aW.twp->P - aW.twp->Pst) / aW.twp->RT / lg_to_ln;
    /* w[p].dV =  rc[q].Vs[0]; w[p].V  =  rc[q].Vs[1]; w[p].devV =rc[q].Vs[2];*/
}

// ------------------ End of m_reac2.cpp --------------------------


