//-------------------------------------------------------------------
// $Id: m_dcomp.cpp 1375 2009-07-24 11:41:49Z gems $
//
// Implementation of TDComp class, config and calculation functions
//
// Rewritten from C to C++ by S.Dmytriyeva
// Copyright (C) 1995-2001 S.Dmytriyeva, D.Kulik, V.Sinitsyn
//
// This file is part of a GEM-Selektor library for thermodynamic
// modelling by Gibbs energy minimization
// Uses: GEM-Selektor GUI GUI DBMS library, gems/lib/gemvizor.lib
//
// This file may be distributed under the GPL v.3 license

//
// See http://gems.web.psi.ch/ for more information
// E-mail: gems2.support@psi.ch
//-------------------------------------------------------------------
//


#include <cstdio>
#include <cmath>

#include "m_dcomp.h"
#include "m_param.h"
#include "visor.h"
#include "service.h"
#include "s_formula.h"
#include "s_supcrt.h"
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


TDComp::TDComp( uint nrt ):
        TCModule( nrt )
{
    nQ = 2;
    aFldKeysHelp.push_back("Phase state code of Dependent Component { a g f p s l m c i z y h }");
    aFldKeysHelp.push_back("ID of a group to which this Dependent Component belongs");
    aFldKeysHelp.push_back("Name of this Dependent Component (chemical species)");
    aFldKeysHelp.push_back("Thermodynamic data subset (TDS) code (e.g. database ID)");
    dcp=&dc[1];
    memcpy( dcp->pstate, "a", MAXSYMB );
    set_def(1);
    dcp=&dc[0];
    memcpy( dcp->pstate, "a", MAXSYMB );
    set_def();
    start_title = " Thermochemical/EoS data format for Dependent Components ";
}


// link values to objects
void TDComp::ods_link( int q)
{
    // aObj[ o_dcpst ]->SetPtr( dc[q].pstate );
    // aObj[ o_dcps ]->SetPtr( dc[q].psymb );
    // aObj[ o_dcn  ]->SetPtr( dc[q].dcn );
    // aObj[ o_dcatc ]->SetPtr( dc[q].atyp );
    // if( dcp == &dc[q] )   return;
    aObj[ o_dcpct ]->SetPtr( dc[q].pct );
    aObj[ o_dcpdc ]->SetPtr( &dc[q].PdcC );
    aObj[ o_dcrmtm ]->SetPtr( dc[q].rmtm );
    aObj[ o_dczz ]->SetPtr( &dc[q].Zz );
    aObj[ o_dcmwt ]->SetPtr( &dc[q].mwt );
    aObj[ o_dcmvs ]->SetPtr( dc[q].mVs );
    aObj[ o_dcgs ]->SetPtr( dc[q].Gs );
    aObj[ o_dchs ]->SetPtr( dc[q].Hs );
    aObj[ o_dcss ]->SetPtr( dc[q].Ss );
    aObj[ o_dccps ]->SetPtr( dc[q].Cps );
    aObj[ o_dcptst ]->SetPtr( &dc[q].Pst );
    aObj[ o_dcstlam ]->SetPtr(&dc[q].Smax );
    aObj[ o_dcbeal ]->SetPtr( &dc[q].Comp );
    aObj[ o_dcder ]->SetPtr( &dc[q].Der );
    // Basic
    aObj[ o_dcstr ]->SetPtr( dc[q].pct );
    aObj[ o_dcname ]->SetPtr( dc[q].name );
    aObj[ o_dcform ]->SetPtr( dc[q].form );
    aObj[ o_dcdim ]->SetPtr( &dc[q].NeCp );
    aObj[ o_dcfloat1 ]->SetPtr( &dc[q].Zz );
    aObj[ o_dcdbl1 ]->SetPtr( dc[q].Gs );
    aObj[ o_dcfloat2 ]->SetPtr(dc[q].Ss );
    // Dynamic
    // if( dc[q].NeCp > 0 ){
    aObj[ o_dccpint ]->SetPtr( dc[q].TCint );
    aObj[ o_dccpint ]->SetDim( 2, dc[q].NeCp );
    aObj[ o_dccp ]->SetPtr( dc[q].Cp );
        aObj[ o_dccp ]->SetDim( MAXCPCOEF, dc[q].NeCp );
    // }
    aObj[ o_dccpfs ]->SetPtr( dc[q].CpFS );
        aObj[ o_dccpfs ]->SetDim( MAXCPFSCOEF, 1 );
    aObj[ o_dchkf ]->SetPtr( dc[q].HKFc );
        aObj[ o_dchkf ]->SetDim( MAXHKFCOEF, 1 );
    aObj[ o_dcvt ]->SetPtr( dc[q].Vt );
        aObj[ o_dcvt ]->SetDim( MAXVTCOEF, 1 );
    aObj[ o_dccritpg ]->SetPtr( dc[q].CPg );
        aObj[ o_dccritpg ]->SetDim( MAXCRITPARAM, 1 );
    aObj[ o_dcodc ]->SetPtr( dc[q].ODc );
        aObj[ o_dcodc ]->SetDim( MAXODCOEF, 1 );
    //if( dc[q].Nft > 0 ) {
    aObj[ o_dcftp ]->SetPtr( dc[q].FtP );
        aObj[ o_dcftp ]->SetDim( 5, dc[q].Nft );
    aObj[ o_dcftpb ]->SetPtr( dc[q].FtBer );
        // aObj[ o_dcftpb ]->SetDim( 3, dc[q].Nft );  // only 1 Landau transition
        aObj[ o_dcftpb ]->SetDim( 3, 1 );
    aObj[ o_dcfttyp ]->SetPtr( dc[q].FtTyp );
        aObj[ o_dcfttyp ]->SetDim( 1, dc[q].Nft );
    //}
    //if( dc[q].Nemp > 0 ) {
    aObj[ o_dccemp ]->SetPtr( dc[q].Cemp );
    if( dc[q].Nemp > 0 )
      aObj[ o_dccemp ]->SetDim( dc[q].Nemp, 1 );
      // aObj[ o_dccemp ]->SetDim( dc[q].Nemp, MAXEOSPARAM );
    //}
    //if( dc[q].Nsd > 0 ) {
    aObj[ o_dcsdref ]->SetPtr( dc[q].sdref );
        aObj[ o_dcsdref ]->SetDim( dc[q].Nsd, 1 );
    aObj[ o_dcsdval ]->SetPtr( dc[q].sdval );
        aObj[ o_dcsdval ]->SetDim( dc[q].Nsd, 1 );
    //}
    dcp=&dc[q];
}


// set dynamic Object ptrs to variables in memory
void TDComp::dyn_set(int q)
{
    ErrorIf( dcp!=&dc[q], GetName(),
             "E00DCrem: Invalid access to dc in dyn_set()");
    memcpy( dcp->pstate, rt[nRT]->UnpackKey(), DC_RKLEN );
    dc[q].TCint= (float *)aObj[ o_dccpint ]->GetPtr();
    dc[q].Cp =   (float *)aObj[ o_dccp ]->GetPtr();
    dc[q].CpFS = (float *)aObj[ o_dccpfs ]->GetPtr();
    dc[q].HKFc = (float *)aObj[ o_dchkf ]->GetPtr();
    dc[q].Vt =   (float *)aObj[ o_dcvt ]->GetPtr();
    dc[q].CPg =  (float *)aObj[ o_dccritpg ]->GetPtr();
    dc[q].ODc =  (float *)aObj[ o_dcodc ]->GetPtr();
    dc[q].FtP =  (float *)aObj[ o_dcftp ]->GetPtr();
    dc[q].FtBer =(float *)aObj[ o_dcftpb ]->GetPtr();
    dc[q].Cemp = (float *)aObj[ o_dccemp ]->GetPtr();
    dc[q].FtTyp = (char (*)[MAXSYMB])aObj[ o_dcfttyp ]->GetPtr();
    dc[q].sdref = (char (*)[V_SD_RKLEN])aObj[ o_dcsdref ]->GetPtr();
    dc[q].sdval = (char (*)[V_SD_VALEN])aObj[ o_dcsdval ]->GetPtr();

    if( dc[q].Cp && aObj[ o_dccp ]->GetN() != MAXCPCOEF )
        vfMessage( 0, dcp->pstate, "W01DCrem: Invalid size of ai_Cp (remake needed)" );

    if( dc[q].CpFS && aObj[ o_dccpfs ]->GetN() != MAXCPFSCOEF )
        vfMessage( 0, dcp->pstate, "W01DCrem: Invalid size of aiCpFS (remake needed)" );

    if( dc[q].HKFc && aObj[ o_dchkf ]->GetN() != MAXHKFCOEF )
        vfMessage( 0, dcp->pstate, "W01DCrem: Invalid size of ai_HKF (remake needed)" );

    if( dc[q].Vt && aObj[ o_dcvt ]->GetN() != MAXVTCOEF )
        vfMessage( 0, dcp->pstate, "W01DCrem: Invalid size of ai_Vtp (remake needed)" );

    if( dc[q].CPg && aObj[ o_dccritpg ]->GetN() != MAXCRITPARAM )
        vfMessage( 0, dcp->pstate, "W01DCrem: Invalid size of CritPg (remake needed)" );

    if( dc[q].ODc && aObj[ o_dcodc ]->GetN() != MAXODCOEF )
        vfMessage( 0, dcp->pstate, "W01DCrem: Invalid size of ai_ODc (remake needed)" );

}


// free dynamic memory for objects and values
void TDComp::dyn_kill(int q)
{
    ErrorIf( dcp!=&dc[q], GetName(),
             "E02DCrem: Invalid access to dc in dyn_kill()");
    dc[q].TCint= (float *)aObj[ o_dccpint ]->Free();
    dc[q].Cp =    (float *)aObj[ o_dccp ]->Free();
    dc[q].CpFS =  (float *)aObj[ o_dccpfs ]->Free();
    dc[q].HKFc =  (float *)aObj[ o_dchkf ]->Free();
    dc[q].Vt =    (float *)aObj[ o_dcvt ]->Free();
    dc[q].CPg =   (float *)aObj[ o_dccritpg ]->Free();
    dc[q].ODc =   (float *)aObj[ o_dcodc ]->Free();
    dc[q].FtP =   (float *)aObj[ o_dcftp ]->Free();
    dc[q].FtBer = (float *)aObj[ o_dcftpb ]->Free();
    dc[q].Cemp =  (float *)aObj[ o_dccemp ]->Free();
    dc[q].FtTyp = (char (*)[MAXSYMB])aObj[ o_dcfttyp ]->Free();
    dc[q].sdref = (char (*)[V_SD_RKLEN])aObj[ o_dcsdref ]->Free();
    dc[q].sdval = (char (*)[V_SD_VALEN])aObj[ o_dcsdval ]->Free();
    dc[q].tprn = (char *)aObj[ o_dctprn ]->Free();
}


// reallocate dynamic memory
void TDComp::dyn_new(int q)
{
    int CM,CE,CV;
    ErrorIf( dcp!=&dc[q], GetName(),
             "E03DCrem: Invalid access to dc in dyn_new()");

    CM = toupper( dc[q].pct[0] );
    CE = toupper( dc[q].pct[1] );
    CV = toupper( dc[q].pct[2] );

    if( dc[q].PdcMK == S_OFF )
    {
        dc[q].TCint = (float *)aObj[ o_dccpint ]->Free();
        dc[q].Cp = (float *)aObj[ o_dccp ]->Free();
    }
    else
    {
        dc[q].TCint = (float *)aObj[ o_dccpint ]->Alloc( 2, dc[q].NeCp, F_ );
        dc[q].Cp = (float *)aObj[ o_dccp ]->Alloc( MAXCPCOEF, dc[q].NeCp, F_ );
    }

    if( dc[q].PdcFT == S_OFF )
    {
        dc[q].FtP = (float *)aObj[ o_dcftp ]->Free();
        dc[q].FtTyp = (char (*)[MAXSYMB])aObj[ o_dcfttyp ]->Free();
        // if( CE == CTM_BER )
        //    dc[q].FtBer = (float *)aObj[ o_dcftpb ]->Free();
    }
    else
    {
        dc[q].FtP = (float *)aObj[ o_dcftp ]->Alloc( 5, dc[q].Nft, F_ );
        dc[q].FtTyp=(char (*)[MAXSYMB])aObj[ o_dcfttyp ]->Alloc( 1, dc[q].Nft, MAXSYMB );
        // if( CE == CTM_BER )
        //    dc[q].FtBer = (float *)aObj[ o_dcftpb ]->Alloc( 3, dc[q].Nft, F_ );
    }

    if ( CM == CTPM_CPT && (CE == CTM_CHP || CE == CTM_BER) )  // added 26.02.2011 (TW)
        dc[q].FtBer = (float *)aObj[ o_dcftpb ]->Alloc( 3, 1, F_ );
    else
        dc[q].FtBer = (float *)aObj[ o_dcftpb ]->Free();

    if( dc[q].Nemp <= 0 )
        dc[q].Cemp = (float *)aObj[ o_dccemp ]->Free();
    else
        // dc[q].Cemp = (float *)aObj[ o_dccemp ]->Alloc( MAXEOSPARAM, 1, F_ );
      dc[q].Cemp = (float *)aObj[ o_dccemp ]->Alloc( dc[q].Nemp, 1, F_ );

    if( dc[q].Nsd == 0 )
    {
        dc[q].sdref = (char (*)[V_SD_RKLEN])aObj[ o_dcsdref ]->Free();
        dc[q].sdval = (char (*)[V_SD_VALEN])aObj[ o_dcsdval ]->Free();
    }
    else
    {
        dc[q].sdref = (char (*)[V_SD_RKLEN])aObj[ o_dcsdref ]->Alloc( dc[q].Nsd, 1, V_SD_RKLEN );
        dc[q].sdval=(char (*)[V_SD_VALEN])aObj[ o_dcsdval ]->Alloc( dc[q].Nsd, 1, V_SD_VALEN );
    }

    if( CM == CTPM_CPT && CV == CPM_AKI )
        dc[q].CpFS = (float *)aObj[ o_dccpfs ]->Alloc( MAXCPFSCOEF, 1, F_ );
    else
        dc[q].CpFS = (float *)aObj[ o_dccpfs ]->Free();

    if( dc[q].PdcHKF == S_OFF )
        dc[q].HKFc = (float *)aObj[ o_dchkf ]->Free();
    else
        dc[q].HKFc = (float *)aObj[ o_dchkf ]->Alloc( MAXHKFCOEF, 1, F_ );

    if( dc[q].PdcVT == S_OFF  )
        dc[q].Vt = (float *)aObj[ o_dcvt ]->Free();
    else
        dc[q].Vt = (float *)aObj[ o_dcvt]->Alloc( MAXVTCOEF, 1, F_ );

    if( CV == CPM_GAS || CV == CPM_PRSV || CV == CPM_SRK || CV == CPM_PR78
            || CV == CPM_CORK || CV == CPM_STP )  // PRSV, SRK, PR78, CORK and STP fluid models
        dc[q].CPg = (float *)aObj[ o_dccritpg ]->Alloc( MAXCRITPARAM, 1, F_ );
    else
        dc[q].CPg = (float *)aObj[ o_dccritpg ]->Free();

    if( CV == CPM_VBM )     // Birch-Murnaghan coeffs, 04.04.2003
        dc[q].ODc = (float *)aObj[ o_dcodc ]->Alloc( MAXODCOEF, 1, F_ );
    else
        dc[q].ODc = (float *)aObj[ o_dcodc ]->Free();

    // if( dc[q].tprn == 0 )
    //   dc[q].tprn = (char *)aObj[ o_dctprn ]->Alloc( 1, 256, S_ );
}


//set default information
void TDComp::set_def( int q)
{
    ErrorIf( dcp!=&dc[q], GetName(),
             "E04DCrem: Invalid access to dc in set_def()");
    TProfil *aPa= dynamic_cast<TProfil *>( aMod[RT_PARAM].get());
    memcpy( dc[q].pct, aPa->pa.DCpct, 6 );
    memcpy( &dc[q].PdcC, aPa->pa.DCpdc, 9 );
    dc[q].Pst = aPa->pa.DRpst;
    dc[q].TCst =aPa->pa.DRtcst;
    dc[q].NeCp = 1;
    dc[q].Nft = dc[q].Nemp = 0;
    dc[q].Nsd = 1;
    strncpy( dc[q].name, rt[rtNum()]->FldKey(2), MAXDCNAME );
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
// dc[q].Nft = dc[q].Nemp = 0;
// dc[q].Nsd = 1;
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


// opens window with 'Remake record' parameters
void TDComp::MakeQuery()
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

// Remake or create DComp record structure
int TDComp::RecBuild( const char *key, int mode  )
{
    int CM, CE, CV;
    //  int q=0;

    memcpy( dcp->pstate, rt[nRT]->UnpackKey(), DC_RKLEN );
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
    case  CP_EXION:
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
    { // This record is one for H2O-gas with code HWS
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
// dcp->Nemp = 0;
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
// if( dcp->Nemp > 0 ) dcp->PdcVT = S_ON;
    if( CM == CTPM_HKF ) dcp->PdcHKF = S_ON;
    dyn_new();
    pVisor->Update();
    return ret;
}


// LROUND macro for Parcor algorithm
#define LROUND(x)      ( static_cast<float>(static_cast<long>((x)+.5)))


//Recalculation of DComp record
// 19/10/1999: variable _S was changed to S_1
// DM 29.10.2024 if TCst and Pst other than standard values 25 C 1 bar, possible to be entered by the user in ReacDC

void TDComp::RecCalc(const char *key )      // dcomp_test
{
    RecCalc(key , dcp->TCst, dcp->Pst);
}


void TDComp::RecCalc(const char *key , double TCst, double Pst)      // dcomp_test
{
    int st, st1, stG, stH, stS, stdG, stdH, stdS;
    double Z, MW, foS, T, G, H, S, S_1, dG, dH, dS, Ro, nj;

    TFormula aFo;
    //double Z, MW, foS;
    //int nICs;
    short listAN[100];  // atomic numbers of ICs involved in the formula
    aFo.SetFormula( dcp->form );   // set formula to analyse
    /*nICs =*/ aFo.Fmwtz( Z, MW, foS, listAN, nj );  // calculate molar mass, return number of ICs
    // need to store nj in DComp data structure (DerB?)
    // make sure that this is only done for minerals

    if( fabs( Z ) < ZBALANCE_PREC )
        dcp->Zz = 0.;
    else
        dcp->Zz = (float)Z;
    dcp->mwt = (float)MW;
    switch( dcp->pstate[0] )
    { //  analysing consistency of chemical formula
    case CP_SOLID:
    case CP_EXION:
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
//    case CP_EXION:
    case CP_MACR:
    case CP_MELT: // need charge
        if( !memcmp( CHARGE_NAME, aFo.GetCn( aFo.GetIn()-1 ), 2 ))
            goto NEXT;
    }
    Error( GetName(), "W07DCrun: Please, check stoichiometry, valences or charge in the formula (depending on DC phase class)");
NEXT:
    if( ( dcp->pstate[0] == CP_GAS || dcp->pstate[0] == CP_GASI ||
          dcp->pstate[0] == CP_FLUID )
        &&  dcp->mVs[0] < 1. )// test molar volume
        dcp->mVs[0] = (float)GAS_MV_STND;

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
                std::string s="W08DCrun: Inconsistent values of H0, S0 or G0 -> ";
                s += std::to_string(G);
                if( vfQuestion( window(), GetName(), s.c_str() ))
                    dcp->Gs[0] = G;
                else
                    Error( GetName(), "E09DCrun: Inconsistent DC standard properties - bailing out...");
            }
    }
    else
        Error( GetName(), "W10DCrun: One of values G0, H0, or S0 is missing!");

    // test pogreshnostey
    if ((25.0 == TCst) && (1.0 == Pst) ) // DM 29.10.2024 check if different std T and P are used for record calc.
    {
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
    }
    // insert parcor
    if( dcp->PdcHKF != S_OFF && toupper( dcp->pct[2] ) == CPM_PCR )
    {   // call PARCOR added 19.05.98
        if( vfQuestion( window(), GetName(),
                 "Estimate parameters of HKF EoS with PARCOR algorithm?" ))
            ParCor();
        goto RESULT;
    }
    if ((25.0 == TCst) && (1.0 == Pst) ) // DM 29.10.2024 check if different std T and P are used for record calc.
    {
        if( !vfQuestion( window(), GetName(),
                     "Calculate standard values of Cp, Vm, (Alpha, Beta)?" ))
            goto RESULT;
    }
    // load TPWORK to calc Cp Vm
    aW.ods_link(0);
    aW.set_zero(0);
    if ((dcp->TCst != TCst) || (dcp->Pst != Pst) ) // DM 29.10.2024 check if different std T and P are used for record calc.
    {
        if ((TCst != 298.15) || (Pst != 1.0) )
        {
            aW.twp->P = Pst;
            aW.twp->TC = TCst;
            aW.twp->Pst = Pst;
            aW.twp->TCst = TCst;
        }
        else
        {
            aW.twp->P = dcp->Pst;
            aW.twp->TC = dcp->TCst;
            aW.twp->Pst = dcp->Pst;
            aW.twp->TCst = dcp->TCst;
        }
    }
    else
    {
        aW.twp->P = dcp->Pst;
        aW.twp->TC = dcp->TCst;
        aW.twp->Pst = dcp->Pst;
        aW.twp->TCst = dcp->TCst;
    }
    aW.twp->T = aW.twp->TC + C_to_K;
    memcpy( aW.twp->DRkey, dcp->pstate, DC_RKLEN );
    DCthermo( 0/*q*/, 0 ); // calc thermodynamic data
    dcp->Cps[0] = aW.twp->Cp;
    dcp->Cps[1] = aW.twp->devCp;
    if ((dcp->TCst != TCst) || (dcp->Pst != Pst) ) // DM 29.10.2024 check if different std T and P are used for record calc.
    {
        dcp->Gs[0] = aW.twp->G;
        dcp->Gs[1] = aW.twp->devG;
        dcp->Hs[0] = aW.twp->H;
        dcp->Hs[1] = aW.twp->devH;
        dcp->Ss[0] = aW.twp->S;
        dcp->Ss[1] = aW.twp->devS;
    }

    dcp->mVs[0] = aW.twp->V;
    dcp->mVs[1] = aW.twp->devV;
RESULT:
    aWp.init = false;
    TCModule::RecCalc(key);
}


// Calculate thermodynamic data at T and P
// uses calc_tpcv() and calc_voldp()
void TDComp::DCthermo( int q, int p )
{
    int idx, CM, CE, CV;
    double rho, eps, alp, dal, bet, xborn, yborn, /*zborn,*/ qborn;
//    char wPdcC = "87 'W'";

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

    if( CM != CTPM_HKF && aW.twp->P < 1e-5 )
    	aW.twp->P = 1e-5;                   // lowest pressure set to 1 Pa
    if( CM == CTPM_HKF || CV == CPM_AKI /*&& aW.twp->P < 1.00001e-5 */ )  // fixed by KD 03.07.03, 05.12.06, 30.01.08
    { // HKF calculations and/or or determination of P_sat if P=0
        if( ((fabs(aW.twp->TC - aSta.Temp) > 0.01) ||
                ( fabs( aW.twp->P - aSta.Pres ) > 0.001 ) || (aSpc.PdcC != dcp->PdcC)) )    // corrected by KD 25.11.05 // added || (aSpc.PdcC != dcp->PdcC)) 01.06.2016
        { // re-calculation of properties of H2O using HGF/HGK
    		aSta.Temp = aW.twp->TC;
    		if( aSta.Temp < 0.01 && aSta.Temp >= 0.0 ) // Deg. C!
    			aSta.Temp = 0.01;
            if( aW.twp->P < 6.11732e-3 ) // 6.11732e-3 is P_sat at triple point of H2O
                // At lower pressures HKF/HGK runs unstable or crashes
    			aSta.Pres = 0.0;  // 06.12.2006  DK
    		else
    			aSta.Pres =  aW.twp->P;

            // 01.06.2016
            aSpc.CV = CV;
            aSpc.PdcC = dcp->PdcC;
            //

    		TSupcrt supCrt;
            supCrt.Supcrt_H2O( aSta.Temp, &aSta.Pres );
            if( aW.twp->P < 6.11732e-3 )   // 06.12.2006  DK
                aW.twp->P = aSta.Pres;

            // pull water properties from WATERPARAM
            rho = aSta.Dens[aSpc.isat];
            alp = aWp.Alphaw[aSpc.isat];
            dal = aWp.dAldT[aSpc.isat];
            bet = aWp.Betaw[aSpc.isat];
            eps = aWp.Dielw[aSpc.isat];
            xborn = aWp.XBorn[aSpc.isat];
            yborn = aWp.YBorn[aSpc.isat];
            //zborn = aWp.ZBorn[aSpc.isat];
            qborn = aWp.QBorn[aSpc.isat];

            // recalculate and assign water properties
            aW.twp->wRo  = aSta.Dens[aSpc.isat];  // density (g cm-3)
            aW.twp->wAlp  = aWp.Alphaw[aSpc.isat];  // expansibility
            aW.twp->wEps = aWp.Dielw[aSpc.isat];  // dielectric constant
            aW.twp->wdAlpdT = aWp.dAldT[aSpc.isat];  // T derivative
            aW.twp->wBet  = aWp.Betaw[aSpc.isat];  // compressibility
            // aW.twp->wVis = aWp.Viscw[aSpc.isat]; // dynamic viscosity
            aW.twp->wdRdT = - alp * rho;
            aW.twp->wd2RdT2 = rho * ( pow(alp,2.) - dal );
            aW.twp->wdRdP = bet * rho;
            aW.twp->wdEdT = yborn * pow(eps,2.);
            aW.twp->wd2EdT2 = (xborn + 2.*eps*pow(yborn,2.)) * pow(eps,2.);
            aW.twp->wdEdP = qborn * pow(eps,2.);
        }

        else
        { // calculated before
            aW.twp->P = aSta.Pres;
            aW.twp->wRo  = aSta.Dens[aSpc.isat]; // 01.06.2016
        }
    }

    switch( CM )
    {
    case CTPM_CPT:
    	// if( CE == CTM_CHP && CV == CPM_CHE )
    	// {  // Added for passing
    	// }
        calc_tpcv( q, p, CE, CV );
        if( CV == CPM_GAS && ( aW.twp->P > 10. && aW.twp->TC > 100. ) )
        {
            aW.twp->CPg = dcp->CPg;
            aW.twp->mwt = dcp->mwt;
            aW.twp->PdcC = dcp->PdcC;
            aW.twp->CPg = NULL;
        }

        else if( CV == CPM_EMP )  // calculation of fugacity at (X=1) using CG EoS
        {
            double FugProps[6];
            TCGFcalc myCGF( 1, (aW.twp->P), (aW.twp->TC+273.15) );
            aW.twp->Cemp = dcp->Cemp;
            aW.twp->PdcC = dcp->PdcC;
            aW.twp->TClow = dcp->TCint[0];
            myCGF.CGcalcFugPure( (aW.twp->TClow+273.15), (aW.twp->Cemp), FugProps );

            // increment thermodynamic properties
            aW.twp->G += R_CONSTANT * (aW.twp->TC+273.15) * log( FugProps[0] );
            aW.twp->H += FugProps[2];
            aW.twp->S += FugProps[3];
            aW.twp->V = FugProps[4];
            aW.twp->Fug = FugProps[0] * (aW.twp->P);
            aW.twp->Cemp = NULL;
        }

        else if( CV == CPM_PRSV )  // calculation of fugacity at (X=1) using PRSV EoS
        {
            double FugProps[6];
            TPRSVcalc myPRSV( 1, (aW.twp->P), (aW.twp->TC+273.15) );
            aW.twp->CPg = dcp->CPg;
            aW.twp->TClow = dcp->TCint[0];
            myPRSV.PRSVCalcFugPure( (aW.twp->TClow+273.15), (aW.twp->CPg), FugProps );
            // myPRSV.~TPRSVcalc();

            // increment thermodynamic properties
            aW.twp->G += R_CONSTANT * (aW.twp->TC+273.15) * log( FugProps[0] );
            aW.twp->H += FugProps[2];
            aW.twp->S += FugProps[3];
            aW.twp->V = FugProps[4];
            aW.twp->Fug = FugProps[0] * (aW.twp->P);
            aW.twp->CPg = NULL;
        }

        else if( CV == CPM_SRK )  // calculation of fugacity at (X=1) using SRK EoS
        {
            double FugProps[6];
            TSRKcalc mySRK( 1, (aW.twp->P), (aW.twp->TC+273.15) );
            aW.twp->CPg = dcp->CPg;
            aW.twp->TClow = dcp->TCint[0];
            mySRK.SRKCalcFugPure( (aW.twp->TClow+273.15), (aW.twp->CPg), FugProps );
            // mySRK.~TSRKcalc();

            // increment thermodynamic properties
            aW.twp->G += R_CONSTANT * (aW.twp->TC+273.15) * log( FugProps[0] );
            aW.twp->H += FugProps[2];
            aW.twp->S += FugProps[3];
            aW.twp->V = FugProps[4];
            aW.twp->Fug = FugProps[0] * (aW.twp->P);
            aW.twp->CPg = NULL;
        }

        else if( CV == CPM_PR78 )  // calculation of fugacity at (X=1) using PR78 EoS
        {
            double FugProps[6];
            TPR78calc myPR78( 1, (aW.twp->P), (aW.twp->TC+273.15) );
            aW.twp->CPg = dcp->CPg;
            aW.twp->TClow = dcp->TCint[0];
            myPR78.PR78CalcFugPure( (aW.twp->TClow+273.15), (aW.twp->CPg), FugProps );
            // myPR78.~TPR78calc();

            // increment thermodynamic properties
            aW.twp->G += R_CONSTANT * (aW.twp->TC+273.15) * log( FugProps[0] );
            aW.twp->H += FugProps[2];
            aW.twp->S += FugProps[3];
            aW.twp->V = FugProps[4];
            aW.twp->Fug = FugProps[0] * (aW.twp->P);
            aW.twp->CPg = NULL;
        }

        else if( CV == CPM_CORK )  // calculation of fugacity at (X=1) using CORK EoS
        {
            double FugProps[6];
            // TCORKcalc myCORK( 1, (aW.twp->P), (aW.twp->TC+273.15), dcp->PdcC /*dcp->pct[3]*/ );
            TCORKcalc myCORK( 1, (aW.twp->P), (aW.twp->TC+273.15), (dcp->pct[3]) );  // modified 05.11.2010 (TW)
            aW.twp->CPg = dcp->CPg;
            aW.twp->TClow = dcp->TCint[0];
            myCORK.CORKCalcFugPure( (aW.twp->TClow+273.15), (aW.twp->CPg), FugProps );
            // myCORK.~TCORKcalc();

            // increment thermodynamic properties
            aW.twp->G += R_CONSTANT * (aW.twp->TC+273.15) * log( FugProps[0] );
            aW.twp->H += FugProps[2];
            aW.twp->S += FugProps[3];
            aW.twp->V = FugProps[4];
            aW.twp->Fug = FugProps[0] * (aW.twp->P);
            aW.twp->CPg = NULL;
        }

        else if ( CV == CPM_STP )  // calculation of fugacity at (X=1) using STP EoS
        {
            double FugProps[6];
            TSTPcalc mySTP( 1, (aW.twp->P), (aW.twp->TC+273.15), (dcp->pct[3]) );
            aW.twp->CPg = dcp->CPg;
            aW.twp->TClow = dcp->TCint[0];
            mySTP.STPCalcFugPure( (aW.twp->TClow+273.15), (aW.twp->CPg), FugProps );
            // mySTP.~STPcalc();

            // increment thermodynamic properties
            aW.twp->G += R_CONSTANT * (aW.twp->TC+273.15) * log( FugProps[0] );
            aW.twp->H += FugProps[2];
            aW.twp->S += FugProps[3];
            aW.twp->V = FugProps[4];
            aW.twp->Fug = FugProps[0] * (aW.twp->P);
            aW.twp->CPg = NULL;
        }


        else if( CV == CPM_AKI )
        {
        	// calculation of partial molal volumes for aqueous non-polar species
                // using EOS (Akinfiev,Diamond 2003) added by DK and TW 30.01.2008
        	calc_akinf( q, p );
        }
        break;

        case CTPM_HKF:   //  'H'  HKF (HGK) EoS
        {  // Calculations based on SUPCRT92
            switch( CE )
            {
              case CTM_WAT:             //  'W'     H2O
                if( CV == CPM_GAS )
                    idx = 0;            //  'G'     H2O_gas (vapor, steam)
                else
                    idx = 1;            //  'F'     H2O_liquid

                calc_tpH2O(aSpc.isat);

                // 01.06.2016
                if (idx == 0 && aSpc.on_sat_curve)
                    calc_tpH2O(0);

                if (idx == 0 )  // H2O vapor
                {   double fd = aW.twp->RT * log(aW.twp->P);
                    // Provisional - HGK seems to return a value corrected with ln(P)!
                    aW.twp->G -= fd;   // This is really needed
                    //aW.twp->S -= 30.2;  // Not clear why this needs to be done
//                  aW.twp->H -= fd;   // provisional - needs checking against IAPS tables DK 4.06.2016
//                  aW.twp->S += fd/aW.twp->T;   // lnP seems more reasonable to correct H0 than S0
                }

  // 01.06.2016

//                switch( aSpc.isat )
//                {
//                  case 2: // metastable systems (water at P<Psat or vapor at P>Psat)?
//                    calc_tpH2O( idx );
//                    break;
//                  case 0: // 1-phase region
//                    if(idx != 0)
//                    {
//                        //idx = 0;   // H2O -liquid
//                    calc_tpH2O( idx );
//                    }
//                    // Fixed on 02.Nov.2009 by DK after discussion with TW
//                    // else //Calc water-gas on from Cp=f(T)
//                        // (on isat = 0)
//                    // calc_tpcv( q, p, CE, CV );
//                    break;
//                  case 1: // P_sat  2-phase region
//                        //calc_tpH2O( idx );
//                        calc_tpH2O( aSpc.isat );
//                    break;
//                  default:
//                    Error( GetName(),
//                           "E13DCrun: Invalid method code!");
//                }
//                if( CV == CPM_GAS )
//                {   double fd = aW.twp->RT * log(aW.twp->P);
//                    // Provisional - HGK seems to return a value corrected with ln(P)!
//                    aW.twp->G -= fd;
//                    aW.twp->S += fd/aW.twp->T;   // even more provisional for entropy! seems to br
//                }
                break;
              default:    // 'K' or any other code
                calc_tphkf( q, p );  // HKF calculations for aqueous species
                break;
            }
        }
        break;
    default:
        std::string msg = "E14DCrun: Invalid method code or data! ";
        msg += dcp->pstate;
        msg += "'.\nChange record?";
        if( !vfQuestion(window(),  GetName(), msg ))
            Error( GetName(), msg );
        //else  RecBuild( key );  // Recalc new record?
    }
}


// ----------------------------------------------------------------------
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

void TDComp::ParCor()
{
    double z, /*g, h,*/ s, v, cp, ak, rx, sigma, polar, Q, x, y, xN,
            conv, tr, theta, pfunk, eta, wabsh, gamma, alphaz=0.,  re,
            wcon=1., wabs, vs, cps, /*ss,*/ vn, cpn, /*sn,*/ aks, akn, a1, a2,
            a4, a3, c1, c2;
    float  ra, ia1, ia2, ia3, ia4, ic1, ic2, ire, iwcon, isigma;

    // Input parameters, convert to cal/mol and cm3
    // g = dcp->Gs[0] / cal_to_J;
    // h = dcp->Hs[0] / cal_to_J;
    s = dcp->Ss[0] / cal_to_J;
    cp = dcp->Cps[0] / cal_to_J;
    v = dcp->mVs[0] * 10;
    z = dcp->Zz;
    //  ak = dcp->Comp;  /* optional, to be entered in DCOMP screen */
    //  rx = dcp->Smax;  /* optional, to be entered in DCOMP screen */

    ak = 0.0;
    rx = 0.0;
    re = 0.0;
    sigma = 0.0;
    polar = 2.0;
    Q = 5.903e-07;
    x = -3.090e-07;
    y = -5.802e-05;
    xN = -1.820e-10;
    conv = 41.8393;
    tr = 298.15;
    theta = 228.0;
    pfunk = 2601.0;
    eta = 1.66027e+05;
    wabsh = 0.5387e+05;

    // calculation of gamma term (Helgeson and Kirkham, 1976)
    if( z > 0.0 )
        gamma = 0.94;
    else
        gamma = 0.0;

    if( noZero(z) )
    {

/*        if(abs((int)z) == 1.0) alphaz = 72.;    // ???? 1/12/2006
        if(abs((int)z) == 2.0) alphaz = 141.;
        if(abs((int)z) == 3.0) alphaz = 211.;
        if(abs((int)z) == 4.0) alphaz = 286.;
*/
        if( approximatelyEqual( fabs(z), 1.0) ) alphaz = 72.;    // ???? 1/12/2006
        if( approximatelyEqual( fabs(z), 2.0) ) alphaz = 141.;
        if( approximatelyEqual( fabs(z), 3.0) ) alphaz = 211.;
        if( approximatelyEqual( fabs(z), 4.0) ) alphaz = 286.;
        if(  noZero(s) )
        {
            if( noZero(rx) )
                re = rx+z*gamma;
            else
            {
                ra = z*z *(eta * y - 100.)/(s - alphaz);
                // ire = 100.*ra +.5;
                ire = LROUND(100.*ra);
                re = ire/100.;
            }
        }
        else
        {
            if( noZero( rx ))
            {
                re = rx+z*gamma;
                // s = z*z* (eta* y - 100.) / re + alphaz;
            }
            else
                Error("E15DCrun: PARCOR error",  "If Z!=0, either S or Rx must be given!" );
        }
        wabs = eta*z*z/re;
        wcon = wabs-z*wabsh;
    }
    else
    {
        if( approximatelyEqual( polar, 2.) )
            wcon = -0.038*100000.;
    }

    iwcon = LROUND(wcon);
    wcon = iwcon;

    // calculation of solvation contributions
    vs=-wcon*Q*conv;
    cps=wcon*tr*x;
    // ss = wcon*y;

    // Calculation of non-solvation contributions
    vn = v - vs;
    cpn = cp - cps;
    // sn = s-ss;

    // the correlations for EOS parameters begin
    if( noZero( ak ) )
    {
        aks=wcon*xN*conv;
        akn=ak-aks;
        if( noZero( sigma ) )
        {
            a2 = 17.19e4 * akn+421.1;


            ia2 = LROUND(a2* 100.0);
            a2 = ia2/100.0;
            a1 = sigma/conv-(a2/pfunk);
            ia1 = LROUND(a1*100000.);
            a1 = ia1/100000.;
        }
        else
        {
            a2=17.19e4 * akn+421.1;
            ia2 = LROUND(a2*100.);
            a2 = ia2/100.;
            sigma = 1.11*vn+1.8;
            isigma = LROUND(sigma*100.);
            sigma = isigma/100.;
            a1=sigma/conv-(a2/pfunk);
            ia1 = LROUND(a1*100000.);
            a1 = ia1/100000.;
        }
    }
    else
    {
        if( noZero(sigma) )
        {
            a1 = (1.3684e-2)*vn + 0.1765;
            ia1 = LROUND(a1*100000.);
            a1 = ia1/100000.;
            a2=((sigma/conv)-a1)*pfunk;
            ia2 = LROUND(a2*100.);
            a2 = ia2/100.;
        }
        else
        {
            sigma = (1.11)*vn + 1.8;
            isigma = LROUND(sigma*100.);
            sigma = isigma/100.;
            a1 = (1.3684e-2)*vn + 0.1765;
            ia1 = LROUND(a1*100000.);
            a1 = ia1/100000.;
            a2 =((sigma/conv)-a1)*pfunk;
            ia2 = LROUND(a2*100.);
            a2 = ia2/100.;
        }
    }

    a4 = - (4.134)*a2 - 27790.0;
    ia4 = LROUND(a4);
    a4 = ia4;
    a3 = ((vn/conv)-a1-a2/pfunk)*(tr-theta) - (a4/pfunk);
    ia3 = LROUND(a3*10000.);
    a3 = ia3/10000.;
    c2 = (0.2037)*cp - 3.0346;
    ic2 = LROUND(c2*10000.);
    c2 = ic2;

    c1 = cpn-c2/(tr-theta)/(tr-theta);
    ic1 = LROUND(c1*10000.);
    c1 = ic1/10000.;
    akn = (a2+a4/(tr-theta))*(1/pfunk/pfunk);
    aks = wcon*conv*xN;
    ak = akn+aks;

    // returning results to HKF EOS params vector
    dcp->HKFc[0] = a1; // *10
    dcp->HKFc[1] = a2; // *10-2
    dcp->HKFc[2] = a3;
    dcp->HKFc[3] = a4; // *10-4
    dcp->HKFc[4] = c1;
    dcp->HKFc[5] = c2; // *10-4/
    dcp->HKFc[6] = wcon; // *10-5
    if( fabs( ak ) >= 1e-3 )
        dcp->HKFc[7] = ak; // to check
    else dcp->HKFc[7] = 0e0;
    // Fixing the PP97 switch to HKF calculation
    dcp->pct[2] = CPM_HKF;
}


// Help on DComp module ( ? button )
const char* TDComp::GetHtml()
{
   return GM_DCOMP_HTML;

}


// Test record with key
void TDComp::TryRecInp( const char *key_, time_t& time_s, int q, bool save )
{

    TCStringArray aDclist;
    TCIntArray anRDc;
    if (save)
    if( ! MessageToSave() )
	return;

    TDBKey dbKey(db->GetDBKey());
    dbKey.SetKey(key_);
    dbKey.SetFldKey(3,"*");
    std::string str_key( dbKey.UnpackKey(), 0, db->KeyLen());
    RecStatus iRet = db->Rtest( str_key.c_str(), 1 );
    std::string msg;

    switch( iRet )
    {
    case MANY_:    // Get Key list
        db->GetKeyList(  str_key.c_str(), aDclist, anRDc );
        db->Get(anRDc[0]);
        [[fallthrough]];
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
            msg += char_array_to_string( key_, db->KeyLen() );
            msg += "'.\n Maybe, a database file is not linked.\n";
            if( pVisor->ProfileMode )
                Error( GetName(), msg );
            msg +=  "Create a new record?";
            if( !vfQuestion(0, GetName(), msg ))
                Error( GetName(), "E17DCrun: New record creation dismissed...");
            std::string str = key_;

            if( str.find_first_of("*?" ) != std::string::npos)  // pattern
                str = GetKeyofRecord( str.c_str(),
                                      "Enter a new record key, please ", KEY_NEW);
            if(  str.empty() )
                Error( GetName(), "E18DCrun: Record creation dismissed...");
            int  Rnum = db->Find( str.c_str() );
            ErrorIf( Rnum>=0, GetName(), " E19DCrun: This record alredy exists!");
            pVisor->OpenModule(window(), nRT,0,true);
            auto str1 = char_array_to_string( db->UnpackKey(), db->KeyLen());
            check_input( str1.c_str() );
            RecBuild( str.c_str() );
            SetString(" W20DCrun: Remake of the new record finished OK. "
                      " It is recommended to re-calculate the data.");
            pVisor->Update();
            Error("W21DCrun: Calculation failed! ",
                  "Check data fields and try calculation again!");
        }
        break;
    case FAIL_:
        msg = "E22DCrun: Failure!!! Database chain ";
        msg += GetName();
        msg += " is corrupt,\n"
               "Data record key '";
        msg += char_array_to_string( key_, db->KeyLen() );
        msg += "'\n Try to backup/restore or compress files in this database chain!";
        Error( GetName(),  msg );
    }
}


void TDComp::CopyRecords( const char * prfName, TCIntArray& cnt,
 elmWindowData el_data, dcSetupData st_data, std::set<std::string>& SDlist)
{
    TCIntArray anR;
    TCStringArray aDCkey;

    // open selected kernel files
    // db->OpenOnlyFromList(el_data.flNames);
    int fnum_ = db->GetOpenFileNum( prfName );

     // delete the equvalent keys
     TCStringArray aICkey_new;         // 30/11/2006
     aICkey_new.clear();

   // get list of records
    db->GetKeyList( "*:*:*:*:", aDCkey, anR );

    //  test&copy  selected records
    // ( add to last key field first symbol from prfname )
    size_t i, j, itmpl;
    TFormula aFo;

    for(size_t ii=0; ii<aDCkey.size(); ii++ )
    {

     // Phase Filters
      if( !el_data.flags[cbAqueous_] && aDCkey[ii][0] == 'a' )
        continue;
      if( !el_data.flags[cbGaseous_] && ( aDCkey[ii][0] == 'g') )
       continue;
      if( !el_data.flags[cbFluid_] && ( aDCkey[ii][0] == 'f') )
        continue;
      if( !el_data.flags[cbPlasma_] && ( aDCkey[ii][0] == 'p') )
      continue;
      if( !el_data.flags[cbSolids_] && ( aDCkey[ii][0] == 's') )
      continue;
      //if( !el_data.flags[cbSindis_] && ( aDCkey[ii][0] == 'd') )
      //   continue;
      if( !el_data.flags[cbLiquid_] && ( aDCkey[ii][0] == 'l') )
      continue;
      if( !el_data.flags[cbSimelt_] && ( aDCkey[ii][0] == 'm') )
      continue;
      if( !el_data.flags[cbSorption_] && ( aDCkey[ii][0] == 'x' || aDCkey[ii][0] == 'c'
              || aDCkey[ii][0] == 'z' || aDCkey[ii][0] == 'i' ) )
      continue;
      if( !el_data.flags[cbPolyel_] && (aDCkey[ii][0] == 'y') )
      continue;
      if( !el_data.flags[cbHcarbl_] && ( aDCkey[ii][0] == 'h') )
      continue;

      // test the same component (overload) 30/11/2006
      std::string stt = aDCkey[ii].substr(0,MAXSYMB+MAXDRGROUP+MAXDCNAME);
      for( j=0; j<aICkey_new.size(); j++ )
         if( stt ==  aICkey_new[j])
            break;
      if( j<aICkey_new.size() )
            continue;

     RecInput( aDCkey[ii].c_str() );
     //test record
     aFo.SetFormula( dcp->form ); // and ce_fscan
     itmpl=0;
     for( i=0; i<aFo.GetIn(); i++ )
     {
       for( j=0; j<el_data.ICrds.size(); j++ )
        if( !memcmp( el_data.ICrds[j].c_str(), aFo.GetCn(i), MAXICNAME ) )
          break;
       if( j == el_data.ICrds.size() )
        break;

       //template
       for( j=0; j<el_data.oldIComps.size(); j++ )
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
       for( j=0; j<el_data.ICrds.size(); j++ )
        if( !memcmp( el_data.ICrds[j].c_str(), aFo.GetCn(i), MAXICNAME ) )
          cnt[j]++;
     }
     // test Vol
       for( j=0; j<el_data.ICrds.size(); j++ )
        if( !memcmp( el_data.ICrds[j].c_str(), "Vol", 3 ) )
          cnt[j]++;

      if( itmpl== aFo.GetIn()) //all icomps in template
        continue;

     // changing record key
     std::string str= std::string(db->FldKey( 3 ), 0, db->FldLen( 3 ));
    ChangeforTempl( str,  st_data.from_templ,
                    st_data.to_templ, db->FldLen( 3 ));
        str += ":";
        std::string str1 = std::string(db->FldKey( 2 ), 0, db->FldLen( 2 ));
        strip( str1 );
        str = str1 + ":" + str;
        str1 = std::string(db->FldKey( 1 ), 0, db->FldLen( 1 ));
        strip( str1 );
        str = str1 + ":" + str;
        str1 = std::string(db->FldKey( 0 ), 0, db->FldLen( 0 ));
        strip( str1 );
        str = str1 + ":" + str;
     // Point SaveRecord
     if( AddRecordTest( str.c_str(), fnum_ ))
     {  aICkey_new.push_back( stt );  // 30/11/2006
        for(int isd=0; isd<dcp->Nsd; isd++)
        { std::string sdkey = char_array_to_string( dcp->sdref[isd],V_SD_RKLEN);
          strip( sdkey );
          SDlist.insert( sdkey );
        }
     }
   }

    // close all no project files
    TCStringArray names1;
    names1.push_back(prfName);
    db->OpenOnlyFromList(names1);
}

// ------------------ End of m_dcomp.cpp ----------------------------
