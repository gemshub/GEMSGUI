//-------------------------------------------------------------------
// $Id: ms_sysbc.cpp 1379 2009-07-29 14:53:22Z gems $
//
// Implementation of TSyst class, calck  bulk chemical composition functions
//
// Rewritten from C to C++ by S.Dmytriyeva
// Copyright (C) 1995-2012 S.Dmytriyeva, D.Kulik
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
#include "m_compos.h"
#include "m_syseq.h"
#include "s_formula.h"

//mark COMPOS from new mark in ICOMP
void TSyst::mark_ic_to_bc( )
{
    short j, i, ii, ij, Nn;
    TCompos* aCMP=dynamic_cast<TCompos *>( aMod[RT_COMPOS].get() );
    RMULTS* mup = TRMults::sm->GetMU();
    time_t crt;

    if( !mup->La )
        return;
    aCMP->ods_link(0);
    Nn = 0;
    for( j=0; j<mup->La; j++ )
    { /* read record */
        aCMP->TryRecInp( mup->SA[j], crt, 0 );
        for( i=0; i< aCMP->bcp->N; i++ )
        {
            for( ii=0, ij=0; ii<mup->N; ii++ )
                if( sy.Icl[ii] != S_OFF )
                    if( !memcmp( mup->SB[ii], aCMP->bcp->SB[i], MAXICNAME+MAXSYMB ))
                        ij++;
            if( !ij )/*it is IC in this compos dont mark in RMULTS  */
            {
                if( sy.Acl[j] != S_OFF )
                {
                    sy.Acl[j] = S_REM;
                    goto NEXT_BC_R;
                }
            }
        } /* i */
        sy.Acl[j] = S_ON;
        Nn++;
NEXT_BC_R:
        ;
    } /* j */
    sy.La = Nn;
}

//mark DCOMP from new mark in PHASE
void TSyst::mark_ph_to_dc( )
{
    short j=0, k, ii=0, Nn;
    RMULTS* mup = TRMults::sm->GetMU();

    sy.Fi = 0;
    sy.L = 0;
    for( k=0; k<mup->Fi; k++ )
    {
        Nn = mup->Ll[k];
        ii+=Nn;
        sy.Ll[k] = 0;
        for(; j<ii; j++ )
            if( sy.Pcl[k] == S_OFF )
            { /* phase switch off */
                sy.Dcl[j] = S_OFF;
            }
            else
            {
                if( sy.Dcl[j] == S_REM )
                    sy.Dcl[j] = S_ON;
                if( sy.Dcl[j] == S_ON )
                {
                    sy.Ll[k]++;
                    sy.L++;
                }
            }
        if( sy.Ll[k] )
            sy.Fi++;
    }
    //  if( sy.PbPH == S_OFF )   Debugging 22.11.99
    ErrorIf( !sy.Fi || sy.L < 2, GetName(), "No phase selected..." );
}

//mark PHASE from new mark in DCOMP
void TSyst::mark_dc_to_ph()
{
    short j=0, k, ii=0, Nn, PP;
    RMULTS* mup = TRMults::sm->GetMU();

    sy.Fi = 0;
    sy.L = 0;
    for( k=0; k<mup->Fi; k++ )
    {
        Nn = mup->Ll[k];
        ii+=Nn;
        sy.Ll[k] = 0;
        for(PP=0; j<ii; j++ )
            if( sy.Dcl[j] != S_OFF )
                PP++;
        if( !PP )
            sy.Pcl[k] = S_OFF;
        else
        {
            sy.Pcl[k] = S_ON;
            sy.Ll[k] = PP;
            sy.Fi++;
            sy.L += PP;
        }
    }
    //  if( sy.PbPH != S_OFF )   Debugging 22.11.99
    ErrorIf( !sy.Fi || sy.L < 2, GetName(), "No dcomp selected..." );
}

//mark ICOMP from new mark in DCOMP
void TSyst::mark_dc_to_ic()
{
    int  j, ij;
    char ICs[MAXRKEYLEN];
    TFormula aFo;
    std::string form;
    RMULTS* mup = TRMults::sm->GetMU();

    sy.N = 0;
    memset( sy.Icl, S_OFF, mup->N );
    for( j=0; j<mup->L; j++ ) // list od DC
    {
        if( sy.Dcl[j] == S_OFF )
            continue;
        /* analyse DC formule */
        form = aFo.form_extr( j, mup->L, mup->DCF );
        aFo.SetFormula( form.c_str() );   // set formula to analyse
        for(int ii=0; ii<aFo.GetIn(); ii++ )
        { /* cycle on formula terms  */
            ICs[IC_RKLEN-1]=0;
            memset( ICs, ' ', IC_RKLEN-1 );
            memcpy( ICs, aFo.GetCn(ii), MAXICNAME+MAXSYMB );
            aFo.fixup_ics( ICs );
            for( ij=0; ij<mup->N; ij++ )
                if( !memcmp( ICs, mup->SB[ij], MAXICNAME+MAXSYMB ))
                {
                    if( sy.Icl[ij] == S_OFF )
                        sy.N++;
                    sy.Icl[ij] = S_ON;
                }
        } /* ii */
        aFo.Reset();
    } /* j */
}

//mark DCOMP from new mark in ICOMP
void TSyst::mark_ic_to_dc()
{
    int  j, ij;
    char ICs[MAXRKEYLEN];
    TFormula aFo;
    std::string form;
    RMULTS* mup = TRMults::sm->GetMU();

    sy.L = 0;
    for( j=0; j<mup->L; j++ )
    { /* cycle to DC. */
        if( sy.Dcl[j] == S_OFF )
            continue;
        /* analyse DC formule */
        form = aFo.form_extr( j, mup->L, mup->DCF );
        aFo.SetFormula( form.c_str() );   // set formula to analyse
        for(int ii=0; ii<aFo.GetIn(); ii++ )
        { /* cycle on formula terms  */
            ICs[IC_RKLEN-1]=0;
            memset( ICs, ' ', IC_RKLEN-1 );
            memcpy( ICs, aFo.GetCn(ii), MAXICNAME+MAXSYMB );
            aFo.fixup_ics( ICs );
            // switch off DC if in it formula is switch off IC
            for( ij=0; ij<mup->N; ij++ )
                if( sy.Icl[ij] == S_OFF &&
                        !memcmp( ICs, mup->SB[ij], MAXICNAME+MAXSYMB ))
                {
                    sy.Dcl[j] = S_OFF;
                    break;
                }
            /* IC? */
        } /* ii */
        aFo.Reset();
        if( sy.Dcl[j] != S_OFF )
            sy.L++;
    } /* j */
}

//Test classified codes! Do more!!!!!!!!!! */
void TSyst::make_syst_sizes()
{
    short i, j, k, N;
    TProfil *aPa= dynamic_cast<TProfil *>( aMod[RT_PARAM].get());
    RMULTS* mup = TRMults::sm->GetMU();

    /* Test classified codes! Do more!!!!!!!!!! */
//    sy.Ls = 0;
    sy.Lw = 0;
    sy.Lg = 0;
    sy.Lsor = 0;
    sy.L = 0;
    sy.Lk = 0;

    for( j=0; j<mup->L; j++ )
    {
        if( sy.Dcl[j] == S_OFF )
            continue;
        sy.L++;
        switch( mup->DCC[j] )
        {      /* water electrolit */
        case DC_AQ_PROTON:
        case DC_AQ_ELECTRON:
        case DC_AQ_SPECIES:
case DC_AQ_SURCOMP:
            sy.Lw++;
            // sy.Ls++;
            break;
        case DC_AQ_SOLVENT:
            sy.LO = j;
        case DC_AQ_SOLVCOM:
            // sy.Ls++;
            break;
            /* Gas mixture */
        case DC_GAS_COMP:
        case DC_GAS_H2O: /* index to switch off ? */
        case DC_GAS_CO2:
        case DC_GAS_N2:
        case DC_GAS_H2:
//            sy.Lg++;     workaround 15.12.2006  SD KD
//            sy.Ls++;
            break;
            /* solution no electrolite */
        case DC_SOL_IDEAL:
//            sy.Ls++;
            break;
        case DC_SOL_MINOR: case DC_SOL_MINDEP:
            if( aPa->pa.p.GAH > 0 )
                sy.lnGmf[j] = log( aPa->pa.p.GAH );
//            sy.Ls++;
            break;
        case DC_SOL_MAJOR: case DC_SOL_MAJDEP:
            if( aPa->pa.p.GAR > 0 )
                sy.lnGmf[j] = log( aPa->pa.p.GAR );
//            sy.Ls++;
            break;
        case DC_SUR_MINAL:
//            sy.Ls++;
            sy.Lsor++;
            break;
        case DC_PEL_CARRIER:
        case DC_SUR_CARRIER:
            if( aPa->pa.p.GAR > 0 ) // DAK testing
                sy.lnGmf[j] = log( aPa->pa.p.GAR );      // 27.10.99
//            sy.Ls++;
            sy.Lsor++;
            break;
            /* one component phase */
        case DC_SCP_CONDEN:
            break;
            /* sorption phase components */
        case DC_SSC_A0:
        case DC_SSC_A1:
        case DC_SSC_A2:
        case DC_SSC_A3:
        case DC_SSC_A4:
        case DC_WSC_A0:
        case DC_WSC_A1:
        case DC_WSC_A2:
        case DC_WSC_A3:
        case DC_WSC_A4:
        case DC_SUR_GROUP:
        case DC_SUR_COMPLEX:
        case DC_SUR_IPAIR:
        case DC_IESC_A:
        case DC_IEWC_B:
            if( aPa->pa.p.GAH > 0 )   // DAK Testing 27.10.99
                sy.lnGmf[j] = log( aPa->pa.p.GAH );     // Adsorption FIA
            sy.Lsor++;
//            sy.Ls++;
            break;
        default:
            break; /* Error - wrong code ! */
        }
    }

// Temporary workaround for Lg counter   15.12.2006  SD KD
    sy.Ls = 0;
    sy.Lg = 0;
    sy.Fi = 0;
    sy.Lhc = 0;
    sy.Fis = 0;
    sy.Fik = 0;
    for( k=0; k<mup->Fi; k++ )
    {
        if( sy.Pcl[k] == S_OFF )
            continue;
        sy.Fi++;
        if( k < mup->Fis )
        {    sy.Fis++;
             sy.Ls += sy.Ll[k];
        }
        switch( mup->PHC[k] )
        {
        case PH_AQUEL:
            if( sy.Ll[k] == sy.Lw+1 && sy.LO >= 0 )
                break;
            break; /* Error code of aqueous species */
        case PH_PLASMA:
        case PH_GASMIX:
        case PH_FLUID:
//            if( sy.Ll[k] == sy.Lg )   workaround  15.12.2006  SD KD
//                break;
               sy.Lg += sy.Ll[k];
               break;
        case PH_HCARBL:
            sy.Lhc = sy.Ll[k];
            break;
        case PH_SIMELT:
        case PH_LIQUID:
            break;
        case PH_POLYEL:
        case PH_SORPTION:
            if(sy.Lsor > 0 )
                break;
            break; /* error code of sorbed species */
        case PH_SINCOND:
        case PH_SINDIS:
        case PH_IONEX:
            break;
        case PH_ADSORPT:
        default:
            ;        /* error code of phase */
        }
    }
    sy.N = 0;
    for( i=0; i<mup->N; i++ )
    {
        if( sy.Icl[i] == S_OFF )
            continue;
        sy.N++;
    }
    /* Calc quantity of selected components and phases */
    for( i=0,N=0; i<mup->N; i++ )
        if( sy.Icl[i] == S_ON ) N++;
    ErrorIf( sy.N != N, GetName(), "Error in selection of IComp" );
    for( i=0,N=0; i<mup->Fi; i++ )
        if( sy.Pcl[i] == S_ON ) N++;
    ErrorIf( sy.Fi != N, GetName(), "Error in selection of Phases" );
    for( i=0,N=0; i<mup->L; i++ )
        if( sy.Dcl[i] == S_ON ) N++;
    ErrorIf( sy.L != N, GetName(), "Error in selection of DComp" );
    if( mup->La && sy.PbAC != S_OFF )  // Fixed 19.02.01  KD
    {
        for( i=0,N=0; i<mup->La; i++ )
            if( sy.Acl[i] == S_ON ) N++;
        sy.La = N;
        if( sy.PbPH == S_OFF ) // Debugging 22.11.99
        if( !N )   // Doubtful
           ErrorIf( !N, GetName(), "Error in quantity of Compos." );
    }

}

// Make System variant
void TSyst::make_syst()
{
    short i, k;
    RMULTS* mup = TRMults::sm->GetMU();

    make_syst_sizes();
    // Calculation of free energy increment for phase-aqueous interfaces
    // due to specific surface area and energy
    if( sy.PYOF != S_OFF )
    {
        double SSA = 0.0, STENW = 0.0, STENG = 0.0;  // Fixed 28.02.2012 by DK
        for( k=0; k<mup->Fi; k++ )
        {
            if( sy.Pcl[k] == S_OFF )
                continue;
            switch( mup->PHC[k] )
            {
            case PH_FLUID:    // cases extended by DK on 25.07.2006
            case PH_GASMIX:
            case PH_LIQUID:
            case PH_SIMELT:
            case PH_HCARBL:
            case PH_SINCOND:
            case PH_SINDIS:
            case PH_SORPTION:
                if( sy.PAalp !=S_OFF )
                {
                    SSA = sy.Aalp[k];
                    sy.PAalp = S_ON;
                }
                if( sy.PSigm !=S_OFF )
                {
                    STENW = sy.Sigm[k][0];  // solid-aqueous spec. interface free energy
                    STENG = sy.Sigm[k][1];  // solid-gaseous spec. interface free energy
                    sy.PSigm = S_ON;
                }
                if( noZero(SSA) && noZero(STENW) && approximatelyZero(sy.YOF[k]) )  // specific surface area is provided
                { // Calculating stability correction in J/g from Freundlich-Ostwald equation
                   sy.YOF[k] = 2./3.*STENW*SSA;
                }
                else if( noZero(SSA) && noZero(STENG) && approximatelyZero(sy.YOF[k]) )
                {    // changed DK 28.02.2012
                   sy.YOF[k] = 2./3.*STENG*SSA;
                }
                break;
            case PH_IONEX:
            case PH_ADSORPT:
            default:
                continue;
            }
        }
        sy.PYOF = S_ON;
    }
    if( sy.PGEX != S_OFF)
        sy.PGEX = S_ON;

// sy.PPHk = S_ON; // experimental - should be set from process or SysEq wizard
// sy.PPHk = S_OFF;
    /* calculate bulk chemical composition of the system */
    systbc_calc(0);
    /* change S_REM to S_ON */
    char *Psw = &sy.PbIC;
    for( i=0; i<28; i++ )
        if( Psw[i] == S_REM )
            Psw[i] = S_ON;
}

// calck  bulk chemical composition of the system
// mode == 0 - set zero to B, mode != 0 add data to B
void TSyst::systbc_calc( int mode )
{
    int i,j,k, N; // wps, Incomplete=0  //  uint ii;
    bool ProvidedBI=false, ProvidedXeD=false, ProvidedXeA=false, ProvidedPhm=false;
    double MsysC = 0., MaqC = 0., VaqC = 0., VsysC = 0., R1C = 0.;
    double Xincr, DCmw, ACmw, Term, MBX;
    double *A=0;
    //char ICs[MAXRKEYLEN];
    TFormula aFo;
    std::string form;
    time_t crt;
    TProfil *aPa= dynamic_cast<TProfil *>(aMod[RT_PARAM].get());
    TCompos* aCMP= dynamic_cast<TCompos *>( aMod[RT_COMPOS].get());
    RMULTS* mup = TRMults::sm->GetMU();

    if( !mode )
      for( i=0; i<mup->N; i++ )
      {
         sy.B[i] =0.;
      }
    
    if( sy.PbIC != S_OFF )
    { //  calc bulk chemical composition from IComp
      for( i=0; i<mup->N; i++ )
         if( noZero(sy.BI[i]) )
             ProvidedBI = true;
      if( ProvidedBI )
      {
        for( i=0; i<mup->N; i++ )
        {
            if( sy.Icl[i] == S_OFF || approximatelyZero(sy.BI[i]) || IsDoubleEmpty(sy.BI[i] ))
                continue;
            double ICmv = mup->BC[i];
            Xincr = aCMP->Reduce_Conc( sy.BIun[i], sy.BI[i], ICmv, 1.0, sy.R1,
                                       sy.Msys, sy.Mwat, sy.Vaq, sy.Maq, sy.Vsys );
            sy.B[i] += Xincr;
            MsysC += Xincr*(double)(mup->BC[i]);
            R1C += Xincr;
        } // cycle i *  
        sy.PbIC = S_ON;
      }
    }
    // add compositions from DCOMP/REACDC formulae amounts/concentrations
    if( sy.PbDC != S_OFF )
    {
      sy.Lb = 0;
      for( j=0; j<mup->L; j++ )
        if(  noZero(sy.XeD[j]) )
           ProvidedXeD = true;
      if( ProvidedXeD )
      {
        A = new double[mup->N];
        for( j=0; j<mup->L; j++ )
        {
            if( sy.Dcl[j] == S_OFF || approximatelyZero(sy.XeD[j]) || IsDoubleEmpty( sy.XeD[j] ))
                continue;
            //Xincr = 0.;  // DCmw = 0.;  // memset( A, 0, sizeof(double)*mup->N );
            sy.Lb++;            
            // analyse DC formula
            form = aFo.form_extr( j, mup->L, mup->DCF );
            aFo.SetFormula( form.c_str() );   // set formula to analyse
            aFo.Stm_line( mup->N, A, (char *)mup->SB, mup->Val );
            aFo.Reset();
            DCmw = MolWeight( mup->N, mup->BC, A );

            Xincr = aCMP->Reduce_Conc( sy.XDun[j], sy.XeD[j], DCmw, 1.0, sy.R1,
                                       sy.Msys, sy.Mwat, sy.Vaq, sy.Maq, sy.Vsys );
            // recalculate using stoichiometry line
            for( i=0; i<mup->N; i++ )
                if( noZero(A[i]) )
                {
                    sy.B[i] += Xincr*(A[i]);
                    R1C += Xincr*(A[i]);
                }
            MsysC += Xincr*DCmw;
        } //  j
        delete[] A;
        A=0;  
        sy.PbDC = S_ON;
      }
    }
    /* calc bulk chemical composition from COMPOS */
    if( sy.PbAC != S_OFF && mup->La )
    {

      for( j=0; j<mup->La; j++ )
          if( noZero(sy.XeA[j]) )
              ProvidedXeA = true;
      if( ProvidedXeA )
      {
        A = new double[mup->N];
        aCMP->ods_link(0);
        for( j=0; j<mup->La; j++ )
        {
            if( sy.Acl[j] == S_OFF || approximatelyZero(sy.XeA[j]) || IsDoubleEmpty( sy.XeA[j] ))
                continue;
            //Xincr = 0.;
            ACmw = 0.;
            memset( A, 0, sizeof(double)*mup->N );
            aCMP->TryRecInp( mup->SA[j], crt, 0 );
            for(int ii=0; ii<aCMP->bcp->N; ii++ )
            { // cycle on COMPOS elements
                for( i=0; i<mup->N; i++ )
                    if( !memcmp( aCMP->bcp->SB[ii], mup->SB[i], MAXICNAME+MAXSYMB ))
                    {
                        if( sy.Icl[i] == S_OFF )
                        { // take sostav not all
                            //Incomplete++;
                            break;
                        }
                        A[i] = aCMP->bcp->C[ii];
                        ACmw += A[i]*(double)(mup->BC[i]);
                        break;
                    }
            } // ii
            Xincr = aCMP->Reduce_Conc( sy.XAun[j], sy.XeA[j], ACmw, 1.0, sy.R1,
                                       sy.Msys, sy.Mwat, sy.Vaq, sy.Maq, sy.Vsys );
            // recalc stehiometric COMPOS
            for( i=0; i<mup->N; i++ )
                if( noZero(A[i]) )
                {   Term = Xincr*(A[i]);
                   // Term = NormDoubleRound(Term, 15 ); // SD 22/07/2009
                   sy.B[i] += Term;
                   R1C += Term;
                }
            MsysC += Xincr*ACmw;
        } //  j
        sy.PbAC = S_ON;
        delete[] A;
        A=0;
      }
    }
    /* calc bulk chemical composition from phases - whole block fixed by DAK 19.05.2010 */
    if( sy.PbPH != S_OFF )
    {        
       if( sy.Msolids > FLOAT_EMPTY )
           ProvidedPhm = true;
       for( k = 0; k<mup->Fi; k++ )
          if( sy.Phm[k] > DOUBLE_EMPTY )
              ProvidedPhm = true;
      if( ProvidedPhm )
      {
        bool NewRkey = false; // , syPbPH, syPPHk;
        std::string pkey;
        std::string skey;
        TSysEq* aSE= dynamic_cast<TSysEq *>( aMod[RT_SYSEQ].get());
        skey = std::string(aSE->ssp->PhmKey, 0, EQ_RKLEN);

        // Added Sveta 14/03/02 check if another SysEq record is available
        if( !skey.empty() && skey[0] != '\0' &&
            skey[0] != ' ' && skey[0] != '-' && skey != S_EMPTY)
        {
//           syPbPH = sy.PbPH;
//           syPPHk = sy.PPHk;
           pkey = rt[RT_SYSEQ]->UnpackKey();  // Save the initial SysEq rkey
           aSE->ods_link(1);
           try
           {
                  int Rnum = rt[RT_SYSEQ]->Find( skey.c_str() );

                  if( Rnum<0 )    // Here call dialog for selection of the SysEq key
                  {
                    skey = vfKeyEdit( window(),
                        "Please, select one SysEq record",  RT_SYSEQ,  skey.c_str() );
                    if( skey.empty() )
                    {
                      std::string msg = "Record ";
                         msg += skey;
                         msg += " not found!" ;
                         Error( GetName(), msg.c_str());
                    }
                    else {  // record selected
                        Rnum = rt[RT_SYSEQ]->Find( skey.c_str() );
                        NewRkey = true;
                    }
                  }
//                  sy.PbPH = syPbPH;
//                  sy.PPHk = syPPHk;
                  rt[RT_SYSEQ]->Get( Rnum );  // Read record
                  aSE->dyn_set(1);
                  PHbcalcSysEq( &MsysC, &MaqC, &R1C, &VaqC, &VsysC );  // Calculation
            }
            catch( TError& xcpt )
            {
               // return to first record
                aSE->ods_link(0);
                rt[RT_SYSEQ]->SetKey( pkey.c_str() ); // Set back the initial record key
                Error( pkey.c_str() , xcpt.mess );
            }

            //    aSE->ssp -= 1; aSE->stp -= 1;// Back to first copy of TSysEq
            // restoring the original SysEq record
             aSE->ods_link(0);
             if( NewRkey )
                 memcpy( aSE->ssp->PhmKey, skey.c_str(), EQ_RKLEN );
             sy.PbPH = S_ON;
              	// Set back the initial record key
             rt[RT_SYSEQ]->SetKey( pkey.c_str() );
        }
        else  // another SysEq record not provided or inaccessible
            sy.PbPH = S_REM;
        if( sy.PbPH != S_OFF && sy.N == TMulti::sm->GetPM()->N )   //   TSysEq::pm->ifCalcFlag() )aPa
        { // Case when no record key is provided -
           PHbcalcMulti( &MsysC, &MaqC, &R1C, &VaqC, &VsysC );  // Calculation
           sy.PbPH = S_ON;

        }
      }
    }
    // calculating total mass of the system
    MBX = 0.0;  // Summation corrected to double on 03.11.2009 DK
    sy.R1 = R1C;
    for( i=0; i<mup->N; i++ )
    {
        if( sy.Icl[i] == S_OFF )
            continue;
        MBX += sy.B[i] * (double)mup->BC[i];
    }
    MBX *= 1e-3;
    sy.MBX = (float)MBX;

    // test calculated composition
    if( !memcmp( mup->SB[mup->N-1], "Zz", 2 ))
    {
        N = mup->N - 1;
        if( fabs( sy.B[N] ) > TMulti::sm->GetPM()->DHBM/10. )  // Fixed by DAK 30.12.02
        {
            if( /*( pe && (pe[0].Istat == P_EXECUTE ||
                pe[0].Istat == P_MT_EXECUTE ))  ||*/
                vfQuestion(window(), rt[RT_SYSEQ]->PackKey(),
                           "Warning: Charge mismatch in calculated \n"
                           "bulk composition!  Compensate(Y) or leave untouched (N)?"  ))
                sy.BI[N] = -sy.B[N];    // sy.B[N]= 0.0;  fixed on Dec.3,2009 by DK
        }			        //  fixed
        else sy.B[N]= 0.0;   		//  fixed
    }

    else N = mup->N;
// SD 14/03/02
    /* test for zero in composition */
    TCStringArray aICkeys;
    TCIntArray aICnums;
    std::vector<bool> sel;
    double fill_data = aPa->pa.p.DHB*1e4; // fixed DK 29.02.2012

    for( i=0; i<N; i++ )
    {
        if( sy.Icl[i] == S_OFF )
            continue;
        if( sy.B[i] < aPa->pa.p.DB || sy.B[i] > 1e6 )
        {
            aICkeys.push_back(std::string(mup->SB[i],0, MAXICNAME));
            aICnums.push_back(i);
            sy.Icl[i] = S_REM;
        }
        else
        {
        	sy.Icl[i] = S_ON;
        }
    }

    if( aICkeys.size() > 0 )
    {
      if( vfExcludeFillEdit( window(), rt[RT_SYSEQ]->PackKey(),
            aICkeys, sel,  fill_data ))
         for( size_t ii=0; ii<aICnums.size(); ii++ )
         {
           i = aICnums[ii];
           if( sel[ii]== true ) //fill
           {
             if( sy.B[i] < aPa->pa.p.DB )
             {
                sy.B[i] = fill_data;
                sy.BI[i] = fill_data;
                sy.BIun[i] = QUAN_MOL; // 'M';  // -- " --
             }
             else if( sy.B[i] > 1e6 )  // Why do we need it?
                        sy.B[i] = 1e5;
           }
           else   //exclude
           {
              sy.Icl[i] = S_OFF;
              sy.B[i] = 0.0;
            }
        }
    }
 	// NormDoubleRound(sy.B, N, 15 ); // SD 22/07/2009
    if( A )
        delete[] A;
   //  pVisor->Update();  //Sveta
}

//Calc bulk chemical composition from phases in EQStat
// Modified 19.05.2010 DK
void TSyst::PHbcalcSysEq( double *MsysC, double *MaqC, double *R1C,
                     double */*VaqC*/,  double */*VsysC*/ )
{
    TSysEq* aSE= dynamic_cast<TSysEq *>( aMod[RT_SYSEQ].get());
    TCompos* aCMP= dynamic_cast<TCompos *>( aMod[RT_COMPOS].get());
    RMULTS* mup = TRMults::sm->GetMU();
    std::vector<TFormula> aFo;
    std::string form;
    int i, j, jf=0, jsf=0, k, Lf;
    double *A, *X;
    double *B, *BS,  Xf, Mass, Xincr;
    bool NotSolid;

    BS = new double[mup->N];    // To keep total stoichiometry of all solids
    fillValue( BS, 0., (mup->N) );

    for( k = 0; k<mup->Fi; k++ ) /*phases */
    {
        Lf=0;  /* count of selected DC */
        Xf = 0.0;
        for( j=jsf; j<aSE->stp->L; j++ )
        {
            if( aSE->stp->llf[j] >= jf+mup->Ll[k] )
                break;
            Xf += aSE->stp->Y[j];
            Lf++;
        }
        NotSolid =  mup->PHC[k] == PH_AQUEL || mup->PHC[k] == PH_GASMIX ||
                    mup->PHC[k] == PH_FLUID || mup->PHC[k] == PH_PLASMA ||
                    mup->PHC[k] == PH_SIMELT || mup->PHC[k] == PH_LIQUID;
        if( Lf && noZero(Xf) && ( noZero(sy.Phm[k]) || ( noZero(sy.Msolids) && !NotSolid ) ) )
            goto NEXT;
        jf+=mup->Ll[k];
        jsf += Lf;
        continue;
NEXT:
        X = new double[Lf];
        fillValue( X, 0., Lf );

        /* load formulae */
        for( i=0; i<Lf; i++ )
        {
            aFo.push_back(  TFormula() );
            j=aSE->stp->llf[jsf+i];
            form = aFo[i].form_extr( j, mup->L, mup->DCF );
            aFo[i].SetFormula( form.c_str() ); // and ce_fscan
            X[i] = aSE->stp->Y[jsf+i];   // load mole quantities
        }
        A = new double[Lf*mup->N];
        fillValue( A, 0., (Lf*mup->N) );
        B = new double[mup->N];
        fillValue( B, 0., (mup->N) );

        for( i=0; i<Lf; i++ )
            aFo[i].Stm_line( mup->N, A+i*mup->N, (char *)mup->SB, mup->Val );
        aFo.clear();
        Mass = 0.0; /* calc mass of phase */
        for( i=0; i<Lf; i++ )
            Mass += X[i]*MolWeight( mup->N, mup->BC, A+i*mup->N );
        /* calc stoichiometry of phase */
        stbal( mup->N, Lf, A, X, B );
        delete[] A;
        delete[] X;
        if( noZero(sy.Phm[k]) && sy.PPHk != S_ON )
        {
           Xincr = aCMP->Reduce_Conc( sy.XPun[k], sy.Phm[k], Mass, 1.0, sy.R1,
                                   sy.Msys, sy.Mwat, sy.Vaq, sy.Maq, sy.Vsys );
           /* recalculating the recipe increments */
           for( i=0; i<mup->N; i++ )
              if( noZero(B[i]) )
              {
                 sy.B[i] += Xincr*B[i];
              }
           if( sy.Msys > 1e-7 )  // Why 1e-7?
               *MsysC += Xincr*Mass;
           if( sy.Maq > 1e-7 )
               *MaqC += Xincr*Mass;
           for( i=0; i<mup->N; i++ ) // added Sveta 13/04/2006
           if( sy.R1 > 1e-7 )
               *R1C += Xincr*B[i];
        }
        if( noZero(sy.Msolids) && !NotSolid && sy.PPHk != S_OFF )
        {  //  Adding phase to total bulk composition of solids in the equilibrium state
           for( i=0; i<mup->N; i++ )
              BS[i] += B[i];
        }
        delete[] B;
        jf+=mup->Ll[k];
        jsf+=Lf;
    } /* k */
    // Added the use of bXs (bulk composition of all solid phases in the referenced equilibrium state)
    if( noZero(sy.Msolids) && sy.PPHk != S_OFF ) // 19.05.2010  DK
    {  // adding total solids, for now in grams only!
        Mass = MolWeight( mup->N, mup->BC, BS );
        if( noZero(Mass) )
        {
           Xincr = aCMP->Reduce_Conc( 'g', sy.Msolids, Mass, 1.0, sy.R1,
                                   sy.Msys, sy.Mwat, sy.Vaq, sy.Maq, sy.Vsys );
           // recalculating the recipe increments
           for( i=0; i<mup->N; i++ )
             if( noZero(BS[i]) )
             {
                sy.B[i] += Xincr*BS[i];
             }
          if( sy.Msys > 1e-7 )  // Why 1e-7?
             *MsysC += Xincr*Mass;
//          if( sy.Maq > 1e-7 )
//             *MaqC += Xincr*Mass;
          for( i=0; i<mup->N; i++ )
            if( sy.R1 > 1e-7 )
               *R1C += Xincr*BS[i];
       }
    }
    delete[] BS;
}


//Calc bulk chemical composition from phases in current contents of MULTI
// Added by DK 12.02.2010
void TSyst::PHbcalcMulti( double *MsysC, double *MaqC, double *R1C,
                     double */*VaqC*/,  double */*VsysC*/ )
{
    double *A, *B, Mass, Xincr;
    TCompos* aCMP= dynamic_cast<TCompos *>( aMod[RT_COMPOS].get());
    MULTI* pmp = TMulti::sm->GetPM();
    RMULTS* mup = TRMults::sm->GetMU();
    int j, i, k, k_, i_;

    B = new double[mup->N];
    j=0;
    for( k=0; k< pmp->FI; k++ )
    { // cycle by phases
        i=j+pmp->L1[k];
        k_ = pmp->muk[k];
        if( approximatelyZero(sy.Phm[k_]) || approximatelyZero(pmp->XFs[k]) || (sy.PPHk == S_OFF && sy.PbPH != S_REM ) )
        {
            j=i;       // There is no amount given for this phase in xp_
            continue;  // or amount of this phase in equilibrium is zero
        }
        fillValue( B, 0., (mup->N) );
        if( k < pmp->FIs )
        {   // This is a multicomponent phase
            A = pmp->BF+k*pmp->N;
            for( i=0; i<pmp->N; i++ )
            {
                i_ = pmp->mui[i];
                B[i_] = A[i];
            }
        }
        else {  // This is a single-component phase
            A = pmp->A+j*pmp->N;
            for( i = 0; i < pmp->N; i++ )
            {
                i_ = pmp->mui[i];
                B[i_] = pmp->X[j]*A[i];
            }
        }
        // calculate the mass of phase
        Mass = MolWeight( mup->N, mup->BC, B );
        // calculate the conversion factor
        Xincr = aCMP->Reduce_Conc( sy.XPun[k_], sy.Phm[k_], Mass, 1.0, sy.R1,
                                   sy.Msys, sy.Mwat, sy.Vaq, sy.Maq, sy.Vsys );
        // recalculate phase stoichiometry
        for( i_=0; i_<mup->N; i_++ )
            if( noZero(B[i_]) )
            {
                sy.B[i_] += Xincr*B[i_];
            }

        if( sy.Msys > 1e-7 )
            *MsysC += Xincr*Mass;
        if( sy.Maq > 1e-7 )
            *MaqC += Xincr*Mass;
        for( i_=0; i_<mup->N; i_++ ) // added Sveta 13/04/2006
           if( sy.R1 > 1e-7 )
              *R1C += Xincr*B[i_];
        j=i;
    }
    Mass = pmp->FitVar[0];
    if( noZero(sy.Msolids) && noZero(Mass) && (sy.PPHk != S_ON || sy.PbPH == S_REM ) )
    {  // adding total solids, for now in grams only!
        A = pmp->BFC;
        for( i=0; i<pmp->N; i++ )
        {
            i_ = pmp->mui[i];
            B[i_] = A[i];
        }
//        Mass = MolWeight( mup->N, mup->BC, B );
        Xincr = aCMP->Reduce_Conc( 'g', sy.Msolids, Mass, 1.0, sy.R1,
                                   sy.Msys, sy.Mwat, sy.Vaq, sy.Maq, sy.Vsys );
        for( i_=0; i_<mup->N; i_++ )
            if( noZero(B[i_]) )
            {
                sy.B[i_] += Xincr*B[i_];
            }
        if( sy.Msys > 1e-7 )      // Why 1e-7 ?
            *MsysC += Xincr*Mass;
//        if( sy.Maq > 1e-7 )
//            *MaqC += Xincr*Mass;
        for( i_=0; i_<mup->N; i_++ )
           if( sy.R1 > 1e-7 )
              *R1C += Xincr*B[i_];
    }
    delete[] B;
}

// Calculation of phase stoichiometry
void TSyst::stbal( int N, int L, double *Smatr, double *DCstc,
                   double *ICm )
{
    int i, j;
    double DCv;

    ErrorIf( !Smatr||!DCstc||!ICm, GetName(), "No memo in Smatr, DCstc orICm" );
    memset( ICm, 0, N*sizeof( double )); // changed Sveta 13/04/2006
    for( j=0; j<L; j++ )
    {
        DCv = DCstc[j];
        if( fabs( DCv ) < 1e-19 )
            continue;
        for( i=0; i<N; i++ )
            ICm[i] += (*(Smatr+i+N*j)) * DCv;
    }
}

//Calculate molar mass of DC from the stoichiometry line and vector of IC atomic masses
double TSyst::MolWeight( int N, float *ICaw, double *Smline )
{
    int i;
    double MW = 0.0;

    for( i=0; i<N; i++ )
        if( noZero(ICaw[i]) && noZero(Smline[i]) )
            MW += static_cast<double>(ICaw[i]) * (Smline[i]);

    return( MW );
}

//Calculate molar mass of DC from the stoichiometry line and vector of IC atomic masses
double TSyst::MolWeight( int N, double *ICaw, double *Smline )
{
    int i;
    double MW = 0.0;

    for( i=0; i<N; i++ )
        if( noZero(ICaw[i]) && noZero(Smline[i]) )
            MW += (ICaw[i]) * (Smline[i]);

    return( MW );
}

//Is bulk composition set up in the system?
bool TSyst::BccCalculated( )
{
    for(int i=0; i<TRMults::sm->GetMU()->N; i++ )
        if( fabs( sy.B[i]) > (TProfil::pm->pa.p.DB*10))
            return true;
    ;

    return false;
}

//--------------------- End of ms_sysbc.cpp ---------------------------

