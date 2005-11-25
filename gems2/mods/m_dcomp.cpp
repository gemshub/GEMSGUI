//-------------------------------------------------------------------
// $Id$
//
// Implementation of TDComp class, config and calculation functions
//
// Rewritten from C to C++ by S.Dmytriyeva
// Copyright (C) 1995-2001 S.Dmytriyeva, D.Kulik, V.Sinitsyn
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
const char *GEMS_DC_HTML = "gm_dcomp";
 
#include <stdio.h>
#include <math.h>

#include "m_dcomp.h"
#include "m_param.h"
#include "v_object.h"
#include "visor.h"
#include "service.h"
#include "s_tpwork.h"
#include "s_formula.h"
#include "s_supcrt.h"
#include "s_fgl.h"
#include "filters_data.h"

TDComp* TDComp::pm;

const double ZBALANCE_PREC =  1e-6,
               STANDARD_TC    = 25.,
                 STANDARD_P     = 1.,
                   DEF_REL_DEV    = 0.05,
                     GAS_MV_STND    = 2478.92,
                       TK_DELTA        = 273.15,
                         TEMPER_PREC     = 0.5,
                           PRESSURE_PREC   = 0.1;

TDComp::TDComp( int nrt ):
        TCModule( nrt )
{
    nQ = 2;
    aFldKeysHelp.Add(
        "l<4  Phase state of Dependent Component (DC) { a g s l h x c }");
    aFldKeysHelp.Add(
        "l<8  Identifier of DC group (capital letters, digits, _ )");
    aFldKeysHelp.Add(
        "l<16 Name of Dependent Component (species, compound)");
    aFldKeysHelp.Add(
        "l<4  Code of data subset { sa xs ... }");
    dcp=&dc[1];
    set_def(1);
    dcp=&dc[0];
    set_def();
    start_title = " Thermochemical/EOS format data for DC (species) ";
}

// link values to objects
void TDComp::ods_link( int q)
{
    // aObj[ o_dcpst ].SetPtr( dc[q].pstate );
    // aObj[ o_dcps ].SetPtr( dc[q].psymb );
    // aObj[ o_dcn  ].SetPtr( dc[q].dcn );
    // aObj[ o_dcatc ].SetPtr( dc[q].atyp );
    // if( dcp == &dc[q] )   return;
    aObj[ o_dcpct ].SetPtr( dc[q].pct );
    aObj[ o_dcpdc ].SetPtr( &dc[q].PdcC );
    aObj[ o_dcrmtm ].SetPtr( dc[q].rmtm );
    aObj[ o_dczz ].SetPtr( &dc[q].Zz );
    aObj[ o_dcmwt ].SetPtr( &dc[q].mwt );
    aObj[ o_dcmvs ].SetPtr( dc[q].mVs );
    aObj[ o_dcgs ].SetPtr( dc[q].Gs );
    aObj[ o_dchs ].SetPtr( dc[q].Hs );
    aObj[ o_dcss ].SetPtr( dc[q].Ss );
    aObj[ o_dccps ].SetPtr( dc[q].Cps );
    aObj[ o_dcptst ].SetPtr( &dc[q].Pst );
    aObj[ o_dcstlam ].SetPtr(&dc[q].Smax );
    aObj[ o_dcbeal ].SetPtr( &dc[q].Comp );
    aObj[ o_dcder ].SetPtr(  &dc[q].Der );
    // Basic
    aObj[ o_dcstr ].SetPtr(   dc[q].pct );
    aObj[ o_dcname ].SetPtr(  dc[q].name );
    aObj[ o_dcform ].SetPtr(  dc[q].form );
    aObj[ o_dcdim ].SetPtr(  &dc[q].NeCp );
    aObj[ o_dcfloat1 ].SetPtr( &dc[q].Zz );
    aObj[ o_dcdbl1 ].SetPtr(  dc[q].Gs );
    aObj[ o_dcfloat2 ].SetPtr(dc[q].Ss );
    // Dynamic
    // if( dc[q].NeCp > 0 ){
    aObj[ o_dccpint ].SetPtr( dc[q].TCint );
    aObj[ o_dccpint ].SetDim( 2, dc[q].NeCp );
    aObj[ o_dccp ].SetPtr( dc[q].Cp );
    aObj[ o_dccp ].SetDim( MAXCPCOEF, dc[q].NeCp );
    //}
    aObj[ o_dccpfs ].SetPtr( dc[q].CpFS );
    aObj[ o_dccpfs ].SetDim( MAXCPFSCOEF, 1 );
    aObj[ o_dchkf ].SetPtr( dc[q].HKFc );
    aObj[ o_dchkf ].SetDim( MAXHKFCOEF, 1 );
    aObj[ o_dcvt ].SetPtr( dc[q].Vt );
    aObj[ o_dcvt ].SetDim( MAXVTCOEF, 1 );
    aObj[ o_dccritpg ].SetPtr( dc[q].CPg );
    aObj[ o_dccritpg ].SetDim( MAXCRITPARAM, 1 );
    aObj[ o_dcodc ].SetPtr( dc[q].ODc );
    aObj[ o_dcodc ].SetDim( MAXODCOEF, 1 );
    //if( dc[q].Nft > 0 ) {
    aObj[ o_dcftp ].SetPtr( dc[q].FtP );
    aObj[ o_dcftp ].SetDim( 5, dc[q].Nft );
    aObj[ o_dcftpb ].SetPtr( dc[q].FtBer );
    aObj[ o_dcftpb ].SetDim( 3, dc[q].Nft );
    aObj[ o_dcfttyp ].SetPtr( dc[q].FtTyp );
    aObj[ o_dcfttyp ].SetDim( 1, dc[q].Nft );
    //}
    //if( dc[q].Nemp > 0 ) {
    aObj[ o_dccemp ].SetPtr( dc[q].Cemp );
    if( dc[q].Nemp > 0 )
      aObj[ o_dccemp ].SetDim( dc[q].Nemp, 1 );
//    aObj[ o_dccemp ].SetDim( dc[q].Nemp, MAXEOSPARAM );
    //}
    //if( dc[q].Nsd > 0 ) {
    aObj[ o_dcsdref ].SetPtr( dc[q].sdref );
    aObj[ o_dcsdref ].SetDim( dc[q].Nsd, 1 );
    aObj[ o_dcsdval ].SetPtr( dc[q].sdval );
    aObj[ o_dcsdval ].SetDim( dc[q].Nsd, 1 );
    //}
    dcp=&dc[q];
}

// set dynamic Object ptrs to variables in memory
void TDComp::dyn_set(int q)
{
    ErrorIf( dcp!=&dc[q], GetName(),
             "E00DCrem: Illegal access to dc in dyn_set()");
    memcpy( dcp->pstate, rt[nRT].UnpackKey(), DC_RKLEN );
    dc[q].TCint= (float *)aObj[ o_dccpint ].GetPtr();
    dc[q].Cp =   (float *)aObj[ o_dccp ].GetPtr();
    dc[q].CpFS = (float *)aObj[ o_dccpfs ].GetPtr();
    dc[q].HKFc = (float *)aObj[ o_dchkf ].GetPtr();
    dc[q].Vt =   (float *)aObj[ o_dcvt ].GetPtr();
    dc[q].CPg =  (float *)aObj[ o_dccritpg ].GetPtr();
    dc[q].ODc =  (float *)aObj[ o_dcodc ].GetPtr();
    dc[q].FtP =  (float *)aObj[ o_dcftp ].GetPtr();
    dc[q].FtBer =(float *)aObj[ o_dcftpb ].GetPtr();
    dc[q].Cemp = (float *)aObj[ o_dccemp ].GetPtr();
    dc[q].FtTyp = (char (*)[MAXSYMB])aObj[ o_dcfttyp ].GetPtr();
    dc[q].sdref = (char (*)[V_SD_RKLEN])aObj[ o_dcsdref ].GetPtr();
    dc[q].sdval = (char (*)[V_SD_VALEN])aObj[ o_dcsdval ].GetPtr();

    if( dc[q].Cp && aObj[ o_dccp ].GetN() != MAXCPCOEF )
        vfMessage( 0, dcp->pstate, "W01DCrem: Invalid size ai_Cp (Remake needed)" );

    if( dc[q].CpFS && aObj[ o_dccpfs ].GetN() != MAXCPFSCOEF )
        vfMessage( 0, dcp->pstate, "W01DCrem: Invalid size aiCpFS (Remake needed)" );

    if( dc[q].HKFc && aObj[ o_dchkf ].GetN() != MAXHKFCOEF )
        vfMessage( 0, dcp->pstate, "W01DCrem: Invalid size ai_HKF (Remake needed)" );

    if( dc[q].Vt && aObj[ o_dcvt ].GetN() != MAXVTCOEF )
        vfMessage( 0, dcp->pstate, "W01DCrem: Invalid size ai_Vtp (Remake needed)" );

    if( dc[q].CPg && aObj[ o_dccritpg ].GetN() != MAXCRITPARAM )
        vfMessage( 0, dcp->pstate, "W01DCrem: Invalid size CritPg (Remake needed)" );

    if( dc[q].ODc && aObj[ o_dcodc ].GetN() != MAXODCOEF )
        vfMessage( 0, dcp->pstate, "W01DCrem: Invalid size ai_ODc (Remake needed)" );

}

// free dynamic memory for objects and values
void TDComp::dyn_kill(int q)
{
    ErrorIf( dcp!=&dc[q], GetName(),
             "E02DCrem: Illegal access to dc in dyn_kill()");
    dc[q].TCint= (float *)aObj[ o_dccpint ].Free();
    dc[q].Cp =    (float *)aObj[ o_dccp ].Free();
    dc[q].CpFS =  (float *)aObj[ o_dccpfs ].Free();
    dc[q].HKFc =  (float *)aObj[ o_dchkf ].Free();
    dc[q].Vt =    (float *)aObj[ o_dcvt ].Free();
    dc[q].CPg =   (float *)aObj[ o_dccritpg ].Free();
    dc[q].ODc =   (float *)aObj[ o_dcodc ].Free();
    dc[q].FtP =   (float *)aObj[ o_dcftp ].Free();
    dc[q].FtBer = (float *)aObj[ o_dcftpb ].Free();
    dc[q].Cemp =  (float *)aObj[ o_dccemp ].Free();
    dc[q].FtTyp = (char (*)[MAXSYMB])aObj[ o_dcfttyp ].Free();
    dc[q].sdref = (char (*)[V_SD_RKLEN])aObj[ o_dcsdref ].Free();
    dc[q].sdval = (char (*)[V_SD_VALEN])aObj[ o_dcsdval ].Free();
    dc[q].tprn = (char *)aObj[ o_dctprn ].Free();
}

// reallocate dynamic memory
void TDComp::dyn_new(int q)
{
    int CM,CE,CV;
    ErrorIf( dcp!=&dc[q], GetName(),
             "E03DCrem: Illegal access to dc in dyn_new()");

    CM = toupper( dc[q].pct[0] );
    CE = toupper( dc[q].pct[1] );
    CV = toupper( dc[q].pct[2] );

    if( dc[q].PdcMK == S_OFF )
    {
        dc[q].TCint = (float *)aObj[ o_dccpint ].Free();
        dc[q].Cp = (float *)aObj[ o_dccp ].Free();
    }
    else
    {
        dc[q].TCint = (float *)aObj[o_dccpint].Alloc( 2, dc[q].NeCp, F_ );
        dc[q].Cp = (float *)aObj[o_dccp].Alloc( MAXCPCOEF, dc[q].NeCp, F_ );
    }

    if( dc[q].PdcFT == S_OFF )
    {
        dc[q].FtP = (float *)aObj[ o_dcftp ].Free();
        dc[q].FtTyp = (char (*)[MAXSYMB])aObj[ o_dcfttyp ].Free();
        if( CE == CTM_BER )
            dc[q].FtBer = (float *)aObj[ o_dcftpb ].Free();
    }
    else
    {
        dc[q].FtP = (float *)aObj[ o_dcftp].Alloc( 5, dc[q].Nft, F_ );
        dc[q].FtTyp=(char (*)[MAXSYMB])aObj[o_dcfttyp].Alloc(1,dc[q].Nft,MAXSYMB);
        if( CE == CTM_BER )
            dc[q].FtBer = (float *)aObj[o_dcftpb].Alloc( 3, dc[q].Nft, F_ );
    }

    if( dc[q].Nemp <= 0 )
        dc[q].Cemp = (float *)aObj[ o_dccemp ].Free();
    else
//        dc[q].Cemp = (float *)aObj[ o_dccemp].Alloc( MAXEOSPARAM, 1, F_ );
      dc[q].Cemp = (float *)aObj[ o_dccemp].Alloc(dc[q].Nemp, 1, F_ );

    if( dc[q].Nsd == 0 )
    {
        dc[q].sdref = (char (*)[V_SD_RKLEN])aObj[ o_dcsdref ].Free();
        dc[q].sdval = (char (*)[V_SD_VALEN])aObj[ o_dcsdval ].Free();
    }
    else
    {
        dc[q].sdref = (char (*)[V_SD_RKLEN])aObj[ o_dcsdref].Alloc(
                          dc[q].Nsd, 1,V_SD_RKLEN );
        dc[q].sdval=(char (*)[V_SD_VALEN])aObj[o_dcsdval].Alloc(
                        dc[q].Nsd, 1, V_SD_VALEN );
    }

    if( CM == CTPM_CPT && CE == CTM_FEI )
        dc[q].CpFS = (float *)aObj[ o_dccpfs].Alloc(MAXCPFSCOEF, 1, F_ );
    else
        dc[q].CpFS = (float *)aObj[ o_dccpfs ].Free();

    if( dc[q].PdcHKF == S_OFF )
        dc[q].HKFc = (float *)aObj[ o_dchkf ].Free();
    else
        dc[q].HKFc = (float *)aObj[ o_dchkf].Alloc(MAXHKFCOEF, 1, F_ );

    if( dc[q].PdcVT == S_OFF  )
        dc[q].Vt = (float *)aObj[ o_dcvt ].Free();
    else
        dc[q].Vt = (float *)aObj[ o_dcvt].Alloc(MAXVTCOEF, 1, F_ );

    if( CV == CPM_GAS /* || CV == CPM_EMP */  )
        dc[q].CPg = (float *)aObj[ o_dccritpg].Alloc(MAXCRITPARAM, 1, F_ );
    else
        dc[q].CPg = (float *)aObj[ o_dccritpg ].Free();

    if( CV == CPM_VBM )     // 04.04.2003  Birch-Murnaghan coeffs
        dc[q].ODc = (float *)aObj[ o_dcodc].Alloc(MAXODCOEF, 1, F_ );
    else
        dc[q].ODc = (float *)aObj[ o_dcodc ].Free();

    // if( dc[q].tprn == 0 )
    //   dc[q].tprn = (char *)aObj[o_dctprn].Alloc( 1, 256, S_ );
}

//set default information

void TDComp::set_def( int q)
{
    ErrorIf( dcp!=&dc[q], GetName(),
             "E04DCrem: Illegal access to dc in set_def()");
    TProfil *aPa=(TProfil *)(&aMod[RT_PARAM]);
    memcpy( dc[q].pct, aPa->pa.DCpct, 6 );
    memcpy( &dc[q].PdcC, aPa->pa.DCpdc, 9 );
    dc[q].Pst = aPa->pa.DRpst;
    dc[q].TCst =aPa->pa.DRtcst;
    dc[q].NeCp = 1;
    dc[q].Nft = dc[q].Nemp = 0;
    dc[q].Nsd = 1;
    strncpy( dc[q].name, rt[rtNum()].FldKey(2), MAXDCNAME );
    dc[q].name[MAXDCNAME] = '\0';
    if( dc[q].pstate[0] == CP_AQU )
    {
       memcpy( dc[q].pct, "HKF   ", 6 );
       memcpy( &dc[q].PdcC, "SjjbC--+-", 9 );
       dc[q].NeCp = 0;
    }
    else
    {
       memcpy( dc[q].pct, aPa->pa.DCpct, 6 );
       memcpy( &dc[q].PdcC, aPa->pa.DCpdc, 9 );
       dc[q].NeCp = 1;
       if( dc[q].pstate[0] == CP_FLUID || dc[q].pstate[0] == CP_LIQID )
          dc[q].Nemp = MAXEOSPARAM;
    }
    dc[q].Pst = aPa->pa.DRpst;
    dc[q].TCst =aPa->pa.DRtcst;
//    dc[q].Nft = dc[q].Nemp = 0;
//    dc[q].Nsd = 1;
    strcpy( dc[q].form, S_EMPTY );
    dc[q].form[1] ='\0';
    dc[q].Gs[0] = DOUBLE_EMPTY;
    dc[q].Gs[1] = DOUBLE_EMPTY;
    dc[q].Hs[0] = DOUBLE_EMPTY;
    dc[q].Hs[1] = DOUBLE_EMPTY;
    dc[q].Zz = FLOAT_EMPTY;
    dc[q].mwt = FLOAT_EMPTY;
    dc[q].mVs[0] = FLOAT_EMPTY;
    dc[q].mVs[1] = FLOAT_EMPTY;
    dc[q].Ss[0] = FLOAT_EMPTY;
    dc[q].Ss[1] = FLOAT_EMPTY;
    dc[q].Cps[0] = FLOAT_EMPTY;
    dc[q].Cps[1] = FLOAT_EMPTY;
    dc[q].Smax = FLOAT_EMPTY;
    dc[q].TCr = FLOAT_EMPTY;
    dc[q].Comp = FLOAT_EMPTY;
    dc[q].Expa = FLOAT_EMPTY;
    dc[q].Der = FLOAT_EMPTY;
    dc[q].DerB = FLOAT_EMPTY;
    dc[q].TCint= 0;
    dc[q].Cp =   0;
    dc[q].CpFS = 0;
    dc[q].HKFc = 0;
    dc[q].Vt =  0;
    dc[q].CPg = 0;
    dc[q].ODc = 0;
    dc[q].FtP = 0;
    dc[q].FtBer = 0;
    dc[q].Cemp =  0;
    dc[q].FtTyp = 0;
    dc[q].sdref = 0;
    dc[q].sdval =0;
    dc[q].tprn = 0;
}

// Input necessary data and link DOD to objects
void TDComp::RecInput( const char *key )
{
    //  memcpy( dcp->pstate, key, MAXSYMB );
    //  dcp->pstate[MAXSYMB-1] = 0;
    TCModule::RecInput( key );
}

/* opens window with 'Remake record' parameters
*/
void
TDComp::MakeQuery()
{
//    pImp->MakeQuery();
    const char * p_key;
    char flgs[15];
    int size[4];

    p_key  = db->PackKey();
    memcpy( flgs, dcp->pct, 15);
    size[0] = dcp->NeCp;
    size[1] = dcp->Nft;
    size[2] = dcp->Nsd;
    size[3] = dcp->Nemp;

    if( !vfDCompSet( window(), p_key, flgs, size ))
         Error( p_key, "DComp record configuration cancelled by the user!" );
     //  return;   // cancel

    memcpy( dcp->pct, flgs, 15);
    dcp->NeCp = (short)size[0];
    dcp->Nft = (short)size[1];
    dcp->Nsd = (short)size[2];
    dcp->Nemp = (short)size[3];

}

// Remake or create DCOMP record structure
//
int
TDComp::RecBuild( const char *key, int mode  )
{
    int CM, CE, CV;
    //  int q=0;

    switch( dcp->pstate[0] )
    {
    case  CP_LIQID: // EoS liquid component
    case  CP_FLUID: // EoS fluid component
                    dcp->Nemp = MAXEOSPARAM;
                    break;
    case  CP_AQU:
    case  CP_GAS:
    case  CP_SOLID:
    case  CP_GASI:
    case  CP_HCARB:
    case  CP_MELT:
    case  CP_SORB:
    case  CP_SSPC:
    case  CP_MACR:
    case  CP_UNIV:
        break;
    default:
        Error( GetName(), "E05DCrem: Invalid code in DComp record key!");
    }
AGAIN:
    int ret = TCModule::RecBuild( key, mode );
    if( ret == VF_CANCEL )
        return ret;
    if( ret == VF3_1 )
    {
        strncpy( dcp->rmtm, curDateSmol().c_str(), 9);
        strncpy( dcp->name, db->FldKey(2), db->FldLen(2));
        dcp->name[db->FldLen(2)] = '\0';
    }


    CM = toupper( dcp->pct[0] );
    CE = toupper( dcp->pct[1] );
    CV = toupper( dcp->pct[2] );
    if( dcp->Nft < 0 || dcp->NeCp > 7 ||
            dcp->Nft < 0 || dcp->Nft > 7  ||
            dcp->Nemp < 0 || dcp->Nemp > MAXEOSPARAM )
        goto AGAIN;

    if( CM == CTPM_HKF && CE == CTM_WAT && CV == CPM_EMP )
    {// This record is one for H2O-gas with code HWS
        CM = CTPM_CPT;
        CE = CTM_CST;
        CV = CPM_GAS;
    }
    if( CM == CTPM_CPT )
    {
        switch( CE)
        {
        case CTM_CST:
            switch( CV)
            {
            case CPM_GAS:
                dcp->NeCp = 1;
                dcp->Nft = 0;
                break;
            }
        case CTM_BER:
        case CTM_CHP:
            break;
        default:
            vfMessage(window(),
            "E06DCrem: Invalid method code", "RecBuild", vfErr);
            goto AGAIN;
        }
    }
    else if( CM != CTPM_HKF && CM != CTPM_EOS )
    {
        vfMessage(window(), "E06DCrem: Invalid method code", "RecBuild", vfErr);
        goto AGAIN;
    }
    if( dcp->NeCp > 0 )  dcp->PdcMK = S_ON;
    if( dcp->Nft > 0 )   dcp->PdcFT = S_ON;
    switch( CV)
    {
      default:
//        dcp->Nemp = 0;
        break;
      case CPM_GAS:
        dcp->Nemp = 4;
        dcp->NeCp = 1;
        break;
      case CPM_EMP:
        dcp->NeCp = 1;
        if( dcp->Nemp < 1 || dcp->Nemp > MAXEOSPARAM )
           dcp->Nemp = MAXEOSPARAM; // added 09.05.2003 CSCS KD
        break;
    }
//  if( dcp->Nemp > 0 ) dcp->PdcVT = S_ON;
    if( CM == CTPM_HKF ) dcp->PdcHKF = S_ON;
    dyn_new();
    pVisor->Update();
    return ret;
}


//Recalculation of DComp record
#define LROUND(x)      ((long)((x)+.5))

// 19/10/1999: ATTENTION! variable _S was changed to S_1
void
TDComp::RecCalc( const char *key )      // dcomp_test
{
    int st, st1, stG, stH, stS, stdG, stdH, stdS;
    double Z, MW, foS, T, G, H, S, S_1, dG, dH, dS, Ro;

    TFormula aFo;
    //double Z, MW, foS;

    aFo.SetFormula( dcp->form );   // set formula to analyse
    aFo.Fmwtz( Z, MW, foS );       // calculate molar mass
    if( fabs( Z ) < ZBALANCE_PREC )
        dcp->Zz = 0.;
    else
        dcp->Zz = (float)Z;
    dcp->mwt = (float)MW;
    switch( dcp->pstate[0] )
    { //  analysing consistency of chemical formula
    case CP_SOLID:
    case CP_GAS:
    case CP_FLUID:
    case CP_UNIV:
    case CP_HCARB:
    case CP_LIQID: // no charge
        if( fabs( Z ) < ZBALANCE_PREC &&
                memcmp( CHARGE_NAME, aFo.GetCn( aFo.GetIn()-1 ), 2 ))
        {
            dcp->Zz = 0.;
            goto NEXT;
        }
        break;
    case CP_AQU:
    case CP_GASI:
    case CP_SSPC:
    case CP_SORB:
    case CP_MACR:
    case CP_MELT: // need charge
        if( !memcmp( CHARGE_NAME, aFo.GetCn( aFo.GetIn()-1 ), 2 ))
            goto NEXT;
    }
    Error( GetName(),
  "W07DCrun: Please, check stoichiometry, charge or valences in the formula");
NEXT:
    if( ( dcp->pstate[0] == CP_GAS || dcp->pstate[0] == CP_GASI ||
          dcp->pstate[0] == CP_FLUID )
        &&  dcp->mVs[0] < 1. )// test molar volume
        dcp->mVs[0] = GAS_MV_STND;
    // test values T, P, Gst, Hst, Sst
    st = IsFloatEmpty( dcp->TCst );
    st1 = IsFloatEmpty( dcp->Pst );
    if( st )
        dcp->TCst = (float)STANDARD_TC;
    if( st1 )
        dcp->Pst = (float)STANDARD_P;
    stG = IsDoubleEmpty( dcp->Gs[0] );
    stH = IsDoubleEmpty( dcp->Hs[0] );
    stS = IsFloatEmpty( dcp->Ss[0] );
    stdG = IsDoubleEmpty( dcp->Gs[1] );
    stdH = IsDoubleEmpty( dcp->Hs[1] );
    stdS = IsFloatEmpty( dcp->Ss[1] );

    T = (double)dcp->TCst + C_to_K;
    G = (double)dcp->Gs[0];
    H = (double)dcp->Hs[0];
    S = (double)dcp->Ss[0];
    dG = (double)dcp->Gs[1];
    dH = (double)dcp->Hs[1];
    dS = (double)dcp->Ss[1];
    Ro = 0.0;
    // test dc type
    if( !stG && stH && !stS )
    {
        H = G + T * ( S - foS );
        dcp->Hs[0] = H;
    }
    else if( stG && !stH && !stS )
    {
        G = H - T * ( S - foS );
        dcp->Gs[0] = G;
    }
    else if( !stG && !stH && stS )
    {
        S = (H - G)/ T + foS;
        dcp->Ss[0] = S;
    }
    else if( !stG && !stH && !stS )
    {
        S_1 = (H - G)/ T + foS;
        if( fabs(S) > 1e-10 )
            if( fabs( (S_1 - S)/S ) >= DEF_REL_DEV )
            {
                G = H - T * ( S - foS );
                vstr Msgb(20);
                gstring s="W08DCrun: Inconsistent values of H0, S0 or G0 -> ";
                sprintf( Msgb, "%g", G );
                s += gstring(Msgb);
                if( vfQuestion( window(), GetName(), s.c_str() ))
                    dcp->Gs[0] = G;
                else
                    Error( GetName(), "E09DCrun: Inconsistent DC standard properties - bailing out...");
            }
    }
    else
        Error( GetName(), "W10DCrun: One of values G0, H0, or S0 is missing!");
    // test pogreshnostey
    if( !stdG && stdH && !stdS )
    {
        dH = sqrt( dG*dG  + dS*dS*T*T);
        dcp->Hs[1] = dH;
    }
    else if( stdG && !stdH && !stdS )
    {
        dG=sqrt( dH*dH + dS*dS*T*T + 2*Ro*dH*dS );
        dcp->Gs[1] = dG;
    }
    else if( !stdG && !stdH && stdS )
    {
        dS = sqrt( dG*dG - dH*dH )/T;
        dcp->Ss[1] = dS;
    }
    else if( stdG || stdH || stdS )
    {
        if( !vfQuestion( window(), GetName(),
     "W11DCrun: Insufficient data to recalculate uncertainties. Continue?" ))
         Error( GetName(), "E12DCrun: Insufficient data for uncertainties - bailing out...");
    }
    // insert parcor
    if( dcp->PdcHKF != S_OFF && toupper( dcp->pct[2] ) == CPM_PCR )
    {   /*  Call PARCOR  added 19.05.98 */
        if( vfQuestion( window(), GetName(),
                 "Estimate parameters of HKF EoS with PARCOR algorithm?" ))
            ParCor();
        goto RESULT;
    }
    if( !vfQuestion( window(), GetName(),
                 "Calculate standard values of Cp, Vm, (Alpha, Beta)?" ))
        goto RESULT;
    // load TPWORK to calc Cp Vm
    aW.ods_link( 0 );
    memset( aW.twp, 0, sizeof( TPWORK ));
    aW.twp->P = dcp->Pst;
    aW.twp->TC = dcp->TCst;
    aW.twp->Pst = dcp->Pst;
    aW.twp->TCst = dcp->TCst;
    aW.twp->T = aW.twp->TC + C_to_K;
    memcpy( aW.twp->DRkey, dcp->pstate, DC_RKLEN );
    DCthermo( 0/*q*/, 0 ); // calc thermodynamic data
    dcp->Cps[0] = aW.twp->Cp;
    dcp->Cps[1] = aW.twp->devCp;
    dcp->mVs[0] = aW.twp->V;
    dcp->mVs[1] = aW.twp->devV;
RESULT:
    aWp.init = false;
    TCModule::RecCalc(key);
}

//Calculate thermodynamic data
void
TDComp::DCthermo( int q, int p )
{
    int idx, CM,CE,CV;

    aW.ods_link( p );
    if( dcp != dc+q )
        ods_link( q );
    aW.twp->Tst = aW.twp->TCst + C_to_K;
    aW.twp->RT = R_CONSTANT * aW.twp->T;
    aW.twp->Fug = aW.twp->P;
    // method of calculation
    CM = toupper( dcp->pct[0] );
    CE = toupper( dcp->pct[1] );
    CV = toupper( dcp->pct[2] );
    if( CM != CTPM_HKF && aW.twp->P < 1e-9 )
         aW.twp->P = 1e-9;
    if( CM == CTPM_HKF || aW.twp->P < 1e-9 )  // fixed by KD 03.07.03
    {// HKF calculations or determination of P_sat if P=0

        if( fabs(aW.twp->TC - aSta.Temp) > 0.01 ||
                ( fabs( aW.twp->P - aSta.Pres ) > 0.001 ))    // corrected by KD 25.11.05
        { // re-calculation of properties of H2O using HGF/HGK
            aSta.Temp = aW.twp->TC;
            if( aSta.Temp < 0.01 && aSta.Temp >= 0.0 ) // Deg. C!
                aSta.Temp = 0.01;
            aSta.Pres =  aW.twp->P;

            TSupcrt supCrt;
            supCrt.Supcrt_H2O( aSta.Temp, &aSta.Pres);
            aW.twp->P = aSta.Pres;
            aW.twp->wRo  = aSta.Dens[aSpc.isat]; // density of water g/cm3
            aW.twp->wEps = aWp.Dielw[aSpc.isat]; // dielectric constant of water
//            aW.twp->wVis = aWp.Viscw[aSpc.isat]; // dynamic viscosity of water
        }
        else
        { // calculated before
            aW.twp->P = aSta.Pres;
        }
    }
    switch( CM )
    {
    case CTPM_CPT:
//        if( CE == CTM_CHP && CV == CPM_CHE )
//        {  // Added for passing
//        }
        calc_tpcv( q, p, CE, CV );
        if( CV == CPM_GAS && ( aW.twp->P > 10. && aW.twp->TC > 100. ) )
        {
            TFGLcalc aFGL;
            aW.twp->CPg = dcp->CPg;
            aW.twp->mwt = dcp->mwt;
            aW.twp->PdcC = dcp->PdcC;
            aFGL.calc_FGL( );
            aW.twp->CPg = NULL;
        }
        else if( CV == CPM_EMP )  // Calculation of fugacity at X=1 using GC EoS
        {                         // Churakov & Gottschalk 2003 GCA
            TCGFcalc aCGF;
            aW.twp->Cemp = dcp->Cemp;
            aW.twp->PdcC = dcp->PdcC;
            aW.twp->TClow = dcp->TCint[0];
            aCGF.CGcalcFug( );
            aW.twp->Cemp = NULL;
        }
        break;
    case CTPM_HKF:
        {
            switch( CE )
            {
            case CTM_WAT:
                if( CV == CPM_GAS )
                    idx = 0; /* H2O_gas */
                else
                    idx = 1; /* H2O_liquid */
                switch( aSpc.isat )
                {
                case 0: /* 1-phase region */
                    if(idx != 0)
                    {
                        idx = 0; /* get water-liquid */
                        calc_tpH2O( idx );
                    }
                    else //Calc water-gas on from Cp=f(T)
                        //(on isat = 0)
                        calc_tpcv( q, p, CE, CV );
                    break;
                case 1: /* KHP 2-phase region */
                    calc_tpH2O( idx );
                    break;

                default:
                    Error( GetName(),
                           "E13DCrun: Invalid method code!");
                }
                break;
            default:
                calc_tphkf( q, p );
                break; /* calc aqueous species */
            }
        }
        break;
    default:
        gstring msg = "E14DCrun: Invalid method code or data! ";
        msg += dcp->pstate;
        msg += "'.\nChange record?";
        if( !vfQuestion(window(),  GetName(), msg ))
            Error( GetName(), msg.c_str() );
        //  else  RecBuild( key );  // !!!!!! Recalc new record?
    }

}


/* ---------------------------------------------------------------*/
/* PARCOR algorithm (Shock 1988) translated from FORTAN code
   PRONSPREP97 (Sverjensky et al., 1997)  added by DAK 19.05.98  */
/*   Input  variables:
      Z = formal charge of the aqueous species.
      G = standard partial molal free energy of formation of the species.
      H = standard partial molal enthalpy of formation of the species.
      S = standard partial molal third law entropy of the species.
      V = standard partial molal volume of the aqueous species.
      Cp = standard partial molal heat capacity of the aqueous species.
 
      Other parameters used in the program include:
      rx = crystallographic radius of the ion, or
              0.0 if no crystallographic radius is
              available or realistic.
      ak= standard patial molal compressibility
          of the aqueous species.  If = 0.0,
          alternate correlations kick in.
      sigma= equation of state parameter from regression
              of temperature dependent volumetric data.
      polar= switch set to 2.0 for the estimation of omega
              of any neutral species. See:
      (all are set to -0.038; Sverjensky et al., 1997)
 
      The Born coefficient w (omega) is calculated
      from the crystallographic radius where available
      or reasonable.  Otherwise, it is calculated from
      the correlation of s with wcon for charged complexes
      and set to -0.038 for neutral complexes
      (Sverjensky, et al., 1997)
 
      Values of the Born Functions Q, X, and Y are taken
      from the Haar-Gallager-Kell equation of state for
      water at 25 C, and may be different from those given
      by Helgeson et al. (1981).
*/

// #define LROUND(x)      ((long)((x)+.5))

void TDComp::ParCor(  )
{
    double z, /*g, h,*/ s, v, cp, ak, rx, sigma, polar, Q, x, y, xN,
    conv, tr, theta, pfunk, eta, wabsh, gamma, alphaz,  re,
    wcon, wabs, vs, cps, /*ss,*/ vn, cpn, /*sn,*/ aks, akn, a1, a2,
    a4, a3, c1, c2;
    float  ra, ia1, ia2, ia3, ia4, ic1, ic2, ire, iwcon, isigma;

    /* Input parameters - convert to cal/mol and cm3 */
    //  g = dcp->Gs[0] / cal_to_J;
    //  h = dcp->Hs[0] / cal_to_J;
    s = dcp->Ss[0] / cal_to_J;
    cp = dcp->Cps[0] / cal_to_J;
    v = dcp->mVs[0] * 10;
    z = dcp->Zz;
    //  ak = dcp->Comp;  /* optional, to be entered in DCOMP screen */
    //  rx = dcp->Smax;  /* optional, to be entered in DCOMP screen */

    /*
        Now the partial molal properties are used in PARCOR
        to estimate equation of state coefficients for each complex.
        The following are the Born functions at 25 C,
        and various other constants.
    */
    ak = 0.0;
    rx = 0.0;
    sigma = 0.0;
    polar = 2.0;

    Q = 5.903e-07;
    x = -3.090e-07;
    y = -5.802e-05;
    xN = -1.820e-10;
    conv = 41.8393;
    tr = 298.15;
    theta = 228.0;
    pfunk=2601.0;
    eta = 1.66027e+05;
    wabsh = 0.5387e+05;
    /*
    	Calculation of GAMMA term
    	(Helgeson & Kirkham 1976, page 153)
    */
    if( z > 0.0 )
        gamma=0.94;
    else	gamma=0.0;

    if(z != 0.0)
    {
        if(abs((int)z) == 1.0) alphaz = 72.;
        if(abs((int)z) == 2.0) alphaz = 141.;
        if(abs((int)z) == 3.0) alphaz = 211.;
        if(abs((int)z) == 4.0) alphaz = 286.;
        if(s != 0.0)
        {
            if(rx != 0.0)
                re = rx+z*gamma;
            else
            {
                ra = z*z *(eta * y - 100.)/(s - alphaz);
                /*ire = 100.*ra +.5;*/
                ire = LROUND(100.*ra);
                re = ire/100.;
            }
        }
        else
        {
            if(rx != 0.0)
            {
                re = rx+z*gamma;
                //		     s = z*z* (eta* y - 100.) / re + alphaz;
            }

            else
                Error("E15DCrun: PARCOR error",  "If Z!=0, either S or Rx must be given!" );
        }
        wabs = eta*z*z/re;
        wcon = wabs-z*wabsh;
    }
    else
    {
        if( polar == 2.)
            wcon = -0.038*100000.;
        /* rounding *
          	  iwcon = wcon + .5;
          	  wcon = iwcon;  * ??? */
    }
    /* rounding */
    /*iwcon = wcon + .5; */
    iwcon = LROUND(wcon);
    wcon = iwcon;
    /*
    	Calculation of solvation contributions to the
    	standard partial molal properties
    */
    vs=-wcon*Q*conv;
    cps=wcon*tr*x;
    //	ss=wcon*y;
    /*
    	Calculation of the non-solvation contributions
    	to the standard partial molal properties
    */
    vn=v-vs;
    cpn=cp-cps;
    //	sn=s-ss;
    /*
    	THE CORRELATIONS FOR E.O.S. PARAMETERS BEGIN !
    */
    if(ak != 0.0)
    {
        aks=wcon*xN*conv;
        akn=ak-aks;
        if(sigma != 0.0)
        {
            a2 = 17.19e4 * akn+421.1;
            /* rounding */
            /* ia2 = a2* 100.0 + 0.5;*/
            ia2 = LROUND(a2* 100.0);
            a2 = ia2/100.0;

            a1 = sigma/conv-(a2/pfunk);
            /* rounding */
            /*ia1 = a1*100000. + .5;*/
            ia1 = LROUND(a1*100000.);
            a1 = ia1/100000.;
        }
        else
        {
            a2=17.19e4 * akn+421.1;
            /* rounding */
            /*ia2 = a2*100. + .5;*/
            ia2 = LROUND(a2*100.);
            a2 = ia2/100.;

            sigma=1.11*vn+1.8;
            /* rounding */
            /*isigma = sigma*100. + .5;*/
            isigma = LROUND(sigma*100.);
            sigma = isigma/100.;

            a1=sigma/conv-(a2/pfunk);
            /* rounding */
            /*ia1 = a1*100000. + .5;*/
            ia1 = LROUND(a1*100000.);
            a1 = ia1/100000.;
        }
    }
    else
    {
        if(sigma != 0.0)
        {
            a1=1.3684e-2 * vn + 0.1765;
            /* rounding */
            /*ia1 = a1*100000. + .5;*/
            ia1 = LROUND(a1*100000.);
            a1 = ia1/100000.;

            a2=((sigma/conv)-a1)*pfunk;
            /* rounding */
            /*ia2 = a2*100. + .5;*/
            ia2 = LROUND(a2*100.);
            a2 = ia2/100.;
        }
        else
        {
            sigma=1.11*vn+1.8;
            /* rounding */
            /*isigma = sigma*100. + .5;*/
            isigma = LROUND(sigma*100.);
            sigma = isigma/100.;

            a1=1.3684e-2 * vn+0.1765;
            /* rounding */
            /*ia1 = a1*100000. + .5;*/
            ia1 = LROUND(a1*100000.);
            a1 = ia1/100000.;

            a2=((sigma/conv)-a1)*pfunk;
            /* rounding */
            /*ia2 = a2*100. + .5;*/
            ia2 = LROUND(a2*100.);
            a2 = ia2/100.;
        }
    }
    a4=-4.134*a2-27790.0;
    /* rounding */
    /*ia4 = a4 +.5;*/
    ia4 = LROUND(a4);
    a4 = ia4;

    a3=((vn/conv)-a1-a2/pfunk)*(tr-theta)-(a4/pfunk);
    /* rounding */
    /*ia3 = a3*10000. + .5;*/
    ia3 = LROUND(a3*10000.);
    a3 = ia3/10000.;

    c2= .2037*cp-3.0346;
    /* rounding */
    /*ic2=c2*10000. + .5;*/
    ic2=LROUND(c2*10000.);
    c2 = ic2;

    c1=cpn-c2/(tr-theta)/(tr-theta);
    /* rounding */
    /*ic1 = c1*10000. + .5;*/
    ic1 = LROUND(c1*10000.);
    c1= ic1/10000.;
    akn=(a2+a4/(tr-theta))*(1/pfunk/pfunk);
    aks=wcon*conv*xN;
    ak=akn+aks;
    /*
    	THAT'S ALL FOLKS !
    */
    /* Returning results to HKF EOS params vector */
    dcp->HKFc[0] = a1; /* *10 */
    dcp->HKFc[1] = a2; /* /100 */
    dcp->HKFc[2] = a3;
    dcp->HKFc[3] = a4; /* /10000 */
    dcp->HKFc[4] = c1;
    dcp->HKFc[5] = c2; /* /10000 */
    dcp->HKFc[6] = wcon; /* /100000 */
    if( fabs( ak ) >= 1e-3 )
        dcp->HKFc[7] = ak; /* to check! */
    else dcp->HKFc[7] = 0e0;
    /* Fixing the PP97 switch to HKF calculation */
    dcp->pct[2] = CPM_HKF;
    /* Finished */
}

// Help on DComp module ( ? button )
void
TDComp::CmHelp()
{
//    pVisor->OpenHelp("GEMS-Linux-HOWTO", "P33D3");
    pVisor->OpenHelp( GEMS_DC_HTML );  //  05.01.01
}

// Test record with key
void
TDComp::TryRecInp( const char *key_, time_t& time_s, int q )
{

    TCStringArray aDclist;
    TCIntArray anRDc;

    if( ! MessageToSave() )
	return;

    vstr key( db->KeyLen(), key_);
    TDBKey dbKey(db->GetDBKey());
    dbKey.SetKey(key);
    dbKey.SetFldKey(3,"*");
    gstring str_key( dbKey.UnpackKey(), 0, db->KeyLen());
    RecStatus iRet = db->Rtest( str_key.c_str(), 1 );
    gstring msg;

    switch( iRet )
    {
    case MANY_:    // Get Key list
        db->GetKeyList(  str_key.c_str(), aDclist, anRDc );
        db->Get(anRDc[0]);
    case ONEF_:
        dyn_set(q);
        time_s = db->Rtime();
        return;
    case UNDF_:
    case NONE_:
    case EMPC_:
        {
            msg = "E16DCrun: In database chain  ";
            msg +=  GetName();
            msg += ": Data record not found, \n"
                   " key  '";
            msg += gstring( key, 0, db->KeyLen() );
            msg += "'.\n Maybe, a database file is not linked.\n";
            if(pVisor->ProfileMode == true)
                Error( GetName(), msg.c_str() );
            msg +=  "Create a new record?";
            if( !vfQuestion(0, GetName(), msg ))
                Error( GetName(), "E17DCrun: New record creation dismissed...");
            gstring str = key.p;

            if( str.find_first_of("*?" ) != gstring::npos)  // pattern
                str = GetKeyofRecord( str.c_str(),
                                      "Enter a new record key, please ", KEY_NEW);
            if(  str.empty() )
                Error( GetName(), "E18DCrun: Record creation dismissed...");
            int  Rnum = db->Find( str.c_str() );
            ErrorIf( Rnum>=0, GetName(), " E19DCrun: This record alredy exists!");
            pVisor->OpenModule(window(), nRT);
            vstr str1( db->KeyLen(), db->UnpackKey());
            check_input( str1 );
            RecBuild( str.c_str() );
            SetString(" W20DCrun: Remake of the new record finished OK. "
                      " It is recommended to re-calculate the data.");
            pVisor->Update();
            Error("W21DCrun: Calculation failed! ",
                  "Check data fields and try calculation again!");
        } // break;
    case FAIL_:
        msg = "E22DCrun: Failure!!! Database chain ";
        msg += GetName();
        msg += " is corrupt,\n"
               "Data record key '";
        msg += gstring( key, 0, db->KeyLen() );
        msg += "'\n Try to backup/restore or compress files in this database chain!";
        Error( GetName(),  msg.c_str() );
    }
}

void TDComp::CopyRecords( const char * prfName, TCIntArray& cnt,
                 elmWindowData el_data, dcSetupData st_data)
{
    TCIntArray anR;
    TCStringArray aDCkey;

    // open selected kernel files
    // db->OpenOnlyFromList(el_data.flNames);
    int fnum_ = db->GetOpenFileNum( prfName );

    // get list of records
    db->GetKeyList( "*:*:*:*:", aDCkey, anR );

    //  test&copy  selected records
    // ( add to last key field first symbol from prfname )
    int i;
    int itmpl;
    uint j;
    TFormula aFo;

    for(uint ii=0; ii<aDCkey.GetCount(); ii++ )
    {
     if( !el_data.flags[cbAqueous_] &&
         ( aDCkey[ii][0] == 'a' || aDCkey[ii][0] == 'x' ))
       continue;
     if( !el_data.flags[cbGaseous_] && aDCkey[ii][0] == 'g' )
       continue;
     if( !el_data.flags[cbSorption_] && aDCkey[ii][0] == 'c' )
       continue;

     RecInput( aDCkey[ii].c_str() );
     //test record
     aFo.SetFormula( dcp->form ); // and ce_fscan
     itmpl=0;
     for( i=0; i<aFo.GetIn(); i++ )
     {
       for( j=0; j<el_data.ICrds.GetCount(); j++ )
        if( !memcmp( el_data.ICrds[j].c_str(), aFo.GetCn(i), MAXICNAME ) )
          break;
       if( j == el_data.ICrds.GetCount() )
        break;

       //template
       for( j=0; j<el_data.oldIComps.GetCount(); j++ )
        if( !memcmp( el_data.oldIComps[j].c_str(), aFo.GetCn(i), MAXICNAME ) )
          { itmpl++;
            break;
          }
      }
      if( i < aFo.GetIn() )
        continue;

     // add cnt
     for( i=0; i<aFo.GetIn(); i++ )
     {
       for( j=0; j<el_data.ICrds.GetCount(); j++ )
        if( !memcmp( el_data.ICrds[j].c_str(), aFo.GetCn(i), MAXICNAME ) )
          cnt[j]++;
     }
     // test Vol
       for( j=0; j<el_data.ICrds.GetCount(); j++ )
        if( !memcmp( el_data.ICrds[j].c_str(), "Vol", 3 ) )
          cnt[j]++;

      if( itmpl== aFo.GetIn()) //all icomps in template
        continue;

     // !!! changing record key
     gstring str= gstring(db->FldKey( 3 ), 0, db->FldLen( 3 ));
    ChangeforTempl( str,  st_data.from_templ,
                    st_data.to_templ, db->FldLen( 3 ));
        str += ":";
        gstring str1 = gstring(db->FldKey( 2 ), 0, db->FldLen( 2 ));
        str1.strip();
        str = str1 + ":" + str;
        str1 = gstring(db->FldKey( 1 ), 0, db->FldLen( 1 ));
        str1.strip();
        str = str1 + ":" + str;
        str1 = gstring(db->FldKey( 0 ), 0, db->FldLen( 0 ));
        str1.strip();
        str = str1 + ":" + str;
     AddRecord( str.c_str(), fnum_ );
    }

    // close all no project files
    TCStringArray names1;
    names1.Add(prfName);
    db->OpenOnlyFromList(names1);
}



// ------------------ End of m_dcomp.cpp ----------------------------
