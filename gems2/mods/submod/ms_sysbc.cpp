//-------------------------------------------------------------------
// $Id$
//
// Implementation of TSyst class, calck  bulk chemical composition functions
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

#include "v_object.h"
#include "m_param.h"
#include "m_compos.h"
#include "m_syseq.h"
#include "s_formula.h"
#include "service.h"
#include "visor.h"


//mark COMPOS from new mark in ICOMP
void TSyst::mark_ic_to_bc( )
{
    short j, i, ii, ij, Nn;
    TCompos* aCMP=(TCompos *)(&aMod[RT_COMPOS]);
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
    vstr ICs(MAXRKEYLEN);
    TFormula aFo;
    gstring form;

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
    vstr ICs(MAXRKEYLEN);
    TFormula aFo;
    gstring form;

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
            /* ���� �� ���������� D�, ���� ����� ������� IC? */
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
    TProfil *aPa=(TProfil *)(&aMod[RT_PARAM]);

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
        case DC_SOL_MINOR:
            if( aPa->pa.p.PSM && aPa->pa.p.GAH > 0 )
                sy.lnGmf[j] = log( aPa->pa.p.GAH );
//            sy.Ls++;
            break;
        case DC_SOL_MAJOR:
            if( aPa->pa.p.PSM && aPa->pa.p.GAR > 0 )
                sy.lnGmf[j] = log( aPa->pa.p.GAR );
//            sy.Ls++;
            break;
        case DC_SUR_MINAL:
//            sy.Ls++;
            sy.Lsor++;
            break;
        case DC_PEL_CARRIER:
        case DC_SUR_CARRIER:
            if( aPa->pa.p.PSM && aPa->pa.p.GAR > 0 ) // DAK testing
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
            if( aPa->pa.p.PSM && aPa->pa.p.GAH > 0 )   // DAK Testing 27.10.99
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
            break;
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
     /*   switch( mup->ICC[i] )
        {
        case IC_ELEMENT:
        case IC_ISOTOPE:
        case IC_FORMULA:
        case IC_METALION:
        case IC_LIGAND:
        case IC_ADDIT:
            break;

        case IC_OXYGEN16:
        case IC_OXYGEN18: * index and quantity of O? *
        case IC_OXYGEN:
            break;

        case IC_PROTIUM:
        case IC_DEYTERIUM:
        case IC_TRITIUM:  * index and quantity of  H? *
        case IC_HYDROGEN:
            break;

        case IC_CHARGE:   * charch - index *
            if( sy.PE != S_ON )
                break;   * may be error� *
        case IC_VOLUME:   * volume IC - index *
            if( sy.PV != S_ON )
                break;  * may be error�� *
        default:
            ; * error in code IComp *
        } */
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

    make_syst_sizes();
    // Calculation of free energy increment for phase-aqueous interfaces
    // due to specific surface area and energy
    if( sy.PYOF != S_OFF )
    {
        double SSA = 0.0, STEN = 0.1;  // Fixed 22.01.2008 by DK 
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
                    STEN = sy.Sigm[k][0];
                    sy.PSigm = S_ON;
                }
                // Calculating J/g in Thomson-Kelvin equation
                sy.YOF[k] = 2./3.*STEN*SSA;
                /*  */
                break;
            default:
                continue;
            }
        }
        sy.PYOF = S_ON;
    }
    if( sy.PGEX != S_OFF)
        sy.PGEX = S_ON;

    /* calck  bulk chemical composition of the system */
    systbc_calc(0);
    /* change S_REM �� S_ON */
    char *Psw = &sy.PbIC;
    for( i=0; i<28; i++ )
        if( Psw[i] == S_REM )
            Psw[i] = S_ON;
}

// calck  bulk chemical composition of the system
// mode == 0 - set zero to B, mode != 0 add data to B
void TSyst::systbc_calc( int mode )
{
    int i,j, N; // wps, Incomplete=0;
    //  uint ii;
    double MsysC = 0., MaqC = 0., VaqC = 0., VsysC = 0., R1C = 0.;
    double Xincr, DCmw, ACmw;
    float *A=0;
    vstr ICs(MAXRKEYLEN);
    TFormula aFo;
    gstring form;
    time_t crt;
    TProfil *aPa=(TProfil *)(&aMod[RT_PARAM]);
    TCompos* aCMP=(TCompos *)(&aMod[RT_COMPOS]);


    if( !mode )
        memset( sy.B, 0, sizeof(double)*mup->N );
    if( sy.PbIC != S_OFF )
    { //  calc bulk chemical composition from IComp *
        for( i=0; i<mup->N; i++ )
        {
            if( sy.Icl[i] == S_OFF || !sy.BI[i] || IsDoubleEmpty(sy.BI[i] ))
                continue;
            float ICmv = mup->BC[i];
            Xincr = aCMP->Reduce_Conc( sy.BIun[i], sy.BI[i], ICmv, 1.0, sy.R1,
                                       sy.Msys, sy.Mwat, sy.Vaq, sy.Maq, sy.Vsys );
            sy.B[i] += Xincr;
            MsysC += Xincr*(double)(mup->BC[i]);
            R1C += Xincr;
        } // cycle i *
        sy.PbIC = S_ON;
    }
    /* calc composition from DCOMP/REACDC */
    if( sy.PbDC != S_OFF )
    {
        sy.Lb = 0;
        A = new float[mup->N];
        for( j=0; j<mup->L; j++ )
        {
            if( sy.Dcl[j] == S_OFF || !sy.XeD[j] || IsDoubleEmpty( sy.XeD[j] ))
                continue;
            //Xincr = 0.;
            DCmw = 0.;
            sy.Lb++;
            memset( A, 0, sizeof(float)*mup->N );
            // analyse DC formule
            form = aFo.form_extr( j, mup->L, mup->DCF );
            aFo.SetFormula( form.c_str() );   // set formula to analyse
            for(int ii=0; ii<aFo.GetIn(); ii++ )
            { // cycle on formula terms
                ICs[IC_RKLEN-1]=0;
                memset( ICs, ' ', IC_RKLEN-1 );
                memcpy( ICs, aFo.GetCn(ii), MAXICNAME+MAXSYMB );
                aFo.fixup_ics( ICs );
                for( i=0; i<mup->N; i++ )
                    if( !memcmp( ICs, mup->SB[i], MAXICNAME+MAXSYMB ))
                    {
                        if( sy.Icl[i] == S_OFF )  //we have switch off IC in formule DC
                            goto NEXT_DC;
                        A[i] += aFo.GetSC(ii);
                        DCmw += (double)(A[i]*mup->BC[i]);
                        break;
                    }
            } // ii
            aFo.Reset();
            Xincr = aCMP->Reduce_Conc( sy.XDun[j], sy.XeD[j], DCmw, 1.0, sy.R1,
                                       sy.Msys, sy.Mwat, sy.Vaq, sy.Maq, sy.Vsys );
            // recalc stehiometric
            for( i=0; i<mup->N; i++ )
                if( A[i] )
                {
                    sy.B[i] += Xincr*(double)(A[i]);
                    R1C += Xincr*(double)(A[i]);
                }
            MsysC += Xincr*DCmw;
NEXT_DC:
            ;
        } //  j
        delete[] A;
        A=0;
        sy.PbDC = S_ON;
    }
    /* calc bulk chemical composition from COMPOS */
    if( sy.PbAC != S_OFF && mup->La )
    {
        A = new float[mup->N];
        aCMP->ods_link(0);
        for( j=0; j<mup->La; j++ )
        {
            if( sy.Acl[j] == S_OFF || !sy.XeA[j] || IsDoubleEmpty( sy.XeA[j] ))
                continue;
            //Xincr = 0.;
            ACmw = 0.;
            memset( A, 0, sizeof(float)*mup->N );
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
                        A[i] = (float)aCMP->bcp->C[ii];
                        ACmw += (double)(A[i]*mup->BC[i]);
                        break;
                    }
            } // ii
            Xincr = aCMP->Reduce_Conc( sy.XAun[j], sy.XeA[j], ACmw, 1.0, sy.R1,
                                       sy.Msys, sy.Mwat, sy.Vaq, sy.Maq, sy.Vsys );
            // recalc stehiometric COMPOS
            for( i=0; i<mup->N; i++ )
                if( A[i] )
                {
                    sy.B[i] += Xincr*(double)(A[i]);
                    R1C += Xincr*(double)(A[i]);
                }
            MsysC += Xincr*ACmw;
        } //  j
        sy.PbAC = S_ON;
        delete[] A;
        A=0;
    }
    /* calc bulk chemical composition from phases - whole block fixed by DAK 27.10.99 */
    if( sy.PbPH != S_OFF )
    {
        gstring pkey;
        gstring skey;

        // Added Sveta 14/03/02  if no Syseq Rec


        pkey = rt[RT_SYSEQ].UnpackKey();  // Save the initial rkey
        //  strncpy(skey, sy.PhmKey, EQ_RKLEN);

        TSysEq* aSE=(TSysEq *)(&aMod[RT_SYSEQ]);
        skey = gstring(aSE->ssp->PhmKey, 0, EQ_RKLEN);

        // Added Sveta 14/03/02  if no Syseq Rec
        if( !skey.empty() && skey[0] != '\0' &&
            skey[0] != ' ' && skey[0] != '-' && skey != S_EMPTY)
        {

          //    aSE->ssp += 1; aSE->stp += 1;			// Second copy of TSysEq
          //    Rnum = rt[RT_SYSEQ].Find( skey );  // does not work!
          //    ErrorIf( Rnum < 0, GetName(), "SysBCC: SysEQ record missing!" );
           aSE->ods_link(1);
           try
           {
                  int Rnum = rt[RT_SYSEQ].Find( skey.c_str() );
                  if( Rnum<0 )
                  {
                        gstring msg = "Record ";
                         msg += skey;
                         msg += " not found!" ;
                         Error( GetName(), msg.c_str());
                  }
                  rt[RT_SYSEQ].Get( Rnum );
                  aSE->dyn_set(1);
//              aSE->TryRecInp( skey.c_str(), crt, 1 );
              //  aSE->dyn_set(1);
              PHbcalc( &MsysC, &MaqC, &R1C, &VaqC, &VsysC );  // Calculation
            }
            catch( TError& xcpt )
            {
               // return to first record
                aSE->ods_link(0);
                rt[RT_SYSEQ].Find( pkey.c_str() ); // Set back the initial record key
                Error( pkey.c_str() , xcpt.mess );
            }

            //    aSE->ssp -= 1; aSE->stp -= 1;// Back to first copy of TSysEq

            // return to first record
             aSE->ods_link(0);
             sy.PbPH = S_ON;
              	// Set back the initial record key
             rt[RT_SYSEQ].Find( pkey.c_str() );
       }
    }
    /* calc mass of system */
    sy.MBX = 0.0;
    sy.R1 = R1C;
    for( i=0; i<mup->N; i++ )
    {
        if( sy.Icl[i] == S_OFF )
            continue;
        sy.MBX += (float)sy.B[i] * mup->BC[i];
    }
    sy.MBX /= 1000.;
    /*  if( Incomplete *&& !( pe && (pe[0].Istat == P_EXECUTE ||
        pe[0].Istat == P_MT_EXECUTE))* )
         vfMessage( GetName(),
         "Warning: there are COMPOS definitions used incompletely\n"
         "for calculation of 'b' vector (marked with '*')", vfErr);
    */
    /* test calculated of composition */
    if( !memcmp( mup->SB[mup->N-1], "Zz", 2 ))
    {
        N = mup->N - 1;
        if( fabs( sy.B[N] ) > aPa->pmp->DHBM/10. )  // Fixed by DAK 30.12.02
        {
            if( /*( pe && (pe[0].Istat == P_EXECUTE ||
                pe[0].Istat == P_MT_EXECUTE ))  ||*/
                vfQuestion(window(), rt[RT_SYSEQ].PackKey(),
                           "Warning: Charge imbalance in calculated \n"
                           "bulk composition!  Zero it off?"  ))
                sy.B[N]= 0.0;
        }					//  fixed
        else sy.B[N]= 0.0;   		//  fixed
    }

    else N = mup->N;
// SD 14/03/02
    /* test for zero in composition */
    TCStringArray aICkeys;
    TCIntArray aICnums;
    TOArray<bool> sel;
    double fill_data = aPa->pa.p.DB*10.;

    for( i=0; i<N; i++ )
    {
        if( sy.Icl[i] == S_OFF )
            continue;
        if( sy.B[i] < aPa->pa.p.DB || sy.B[i] > 1e6 )
        {
            aICkeys.Add(gstring(mup->SB[i],0, MAXICNAME));
            aICnums.Add(i);
            sy.Icl[i] = S_REM;
        }
        else sy.Icl[i] = S_ON;
    }

    if( aICkeys.GetCount() > 0 )
    {
      if( vfExcludeFillEdit( window(), rt[RT_SYSEQ].PackKey(),
            aICkeys, sel,  fill_data ))
         for( uint ii=0; ii<aICnums.GetCount(); ii++ )
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
    if( A )
        delete[] A;
    ///  pVisor->Update();  //Sveta
}

//Calc bulk chemical composition from phases in EQStat

void TSyst::PHbcalc( double *MsysC, double *MaqC, double *R1C,
                     double */*VaqC*/,  double */*VsysC*/ )
{
    TSysEq* aSE=(TSysEq *)(&aMod[RT_SYSEQ]);
    TCompos* aCMP=(TCompos *)(&aMod[RT_COMPOS]);
    TIArray<TFormula> aFo;
    gstring form;
    int i, j, jf=0, jsf=0, k, Lf;
    float *A;
    double *B, Xf, Mass, Xincr;
    double *X;

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
        if( Lf && Xf && sy.Phm[k] )
            goto NEXT;
        jf+=mup->Ll[k];
        jsf += Lf;
        continue;
NEXT:
        X = new double[Lf];
        memset( X, 0, sizeof(double)*Lf );

        /* load formulae */
        for( i=0; i<Lf; i++ )
        {
            aFo.Add( new TFormula() );
            j=aSE->stp->llf[jsf+i];
            form = aFo[i].form_extr( j, mup->L, mup->DCF );
            aFo[i].SetFormula( form.c_str() ); // and ce_fscan
            X[i] = aSE->stp->Y[jsf+i];   // load mole quantities
        }
        A = new float[Lf*mup->N];
        fillValue( A, (float)0., (Lf*mup->N) );
        B = new double[mup->N];
        memset( B, 0, sizeof(double)*(mup->N) );

        for( i=0; i<Lf; i++ )
            aFo[i].Stm_line( mup->N, A+i*mup->N, (char *)mup->SB, mup->Val );
        aFo.Clear();
        Mass = 0.0; /* calc mass of phase */
        for( i=0; i<Lf; i++ )
            Mass += X[i]*MolWeight( mup->N, mup->BC, A+i*mup->N );
        /* calc stoichiometry of phase */
        stbal( mup->N, Lf, A, X, B );
        delete[] A;
        delete[] X;
        Xincr = aCMP->Reduce_Conc( sy.XPun[k], sy.Phm[k], Mass, 1.0, sy.R1,
                                   sy.Msys, sy.Mwat, sy.Vaq, sy.Maq, sy.Vsys );
        /* recalc stehiometric of PHASE */
        for( i=0; i<mup->N; i++ )
            if( B[i] )
                sy.B[i] += Xincr*B[i];

        if( sy.Msys > 1e-7 )
            *MsysC += Xincr*Mass;
        if( sy.Maq > 1e-7 )
            *MaqC += Xincr*Mass;
      for( i=0; i<mup->N; i++ ) // added Sveta 13/04/2006
        if( sy.R1 > 1e-7 )
            *R1C += Xincr*B[i];

        delete[] B;
        jf+=mup->Ll[k];
        jsf+=Lf;
    } /* k */
}

void TSyst::stbal( int N, int L, float *Smatr, double *DCstc,
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
            ICm[i] += (double)(*(Smatr+i+N*j)) * DCv;
    }
}

//Calc mol mass ofDC by gstring of stehiometric matr and vector
// atom mass
double TSyst::MolWeight( int N, float *ICaw, float *Smline )
{
    int i;
    double MW = 0.0;

    for( i=0; i<N; i++ )
        if( ICaw[i] && Smline[i] )
            MW += (double)(ICaw[i]) * (double)(Smline[i]);

    return( MW );
}

//Calc mol mass ofDC by gstring of stehiometric matr and vector
// atom mass
double TSyst::MolWeight( int N, double *ICaw, double *Smline )
{
    int i;
    double MW = 0.0;

    for( i=0; i<N; i++ )
        if( ICaw[i] && Smline[i] )
            MW += (ICaw[i]) * (Smline[i]);

    return( MW );
}

//Is bulk composition set up in the system?
bool TSyst::BccCalculated( )
{
    for(int i=0; i<mup->N; i++ )
        if( fabs( sy.B[i]) > (TProfil::pm->pa.p.DB*10))
            return true;
    ;

    return false;
}
//--------------------- End of ms_sysbc.cpp ---------------------------

