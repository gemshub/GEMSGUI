//------------------------------------------------------------------
// Id: gems/mods/m_reacdc.cpp  version 2.0.0   2001
//
// Implementation of TReacDC class, config and calculation functions
//
// Rewritten from C to C++ by S.Dmytriyeva  970207 modified 010904
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
const char *GEMS_RE_HTML = "gm_reacdc";

#include <math.h>

#include "v_object.h"
#include "m_reacdc.h"
#include "m_dcomp.h"
#include "m_param.h"
#include "s_supcrt.h"
#include "s_formula.h"
#include "service.h"
#include "visor.h"
#include "filters_data.h"

TReacDC* TReacDC::pm;

TReacDC::TReacDC( int nrt ):
        TCModule( nrt )
{
    nQ =8;
    aFldKeysHelp.Add(
        "l<4  Phase state of (reaction-defined) DC <-REACDC (<-DCOMP)");
    aFldKeysHelp.Add(
        "l<8  Identifier of (REAC)DC group  <-REACDC (<-DCOMP)");
    aFldKeysHelp.Add(
        "l<16 Name of reaction-defined DC (compound) <-REACDC (<-DCOMP)");
    aFldKeysHelp.Add(
        "l<4  Code of thermodynamic dataset (file) <-REACDC (<-DCOMP)");

    for(int i=1; i<nQ; i++)
    {
        rcp=&rc[i];
        set_def( i);
    }
    rcp=&rc[0];
    set_def();
    start_title = " Reaction format thermodynamic data for species";
}

// link values to objects
void TReacDC::ods_link( int q)
{
    ErrorIf( q >= nQ, GetName(), "Illegal link (q>=8)!" );

    // static
    //aObj[ o_repst].SetPtr( rc[q].pstate );
    //aObj[ o_reps].SetPtr( rc[q].psymb );
    //aObj[ o_redcn].SetPtr( rc[q].dcn );
    //aObj[ o_reatyp].SetPtr( rc[q].atyp );
    aObj[ o_repct].SetPtr(   rc[q].pct );
    aObj[ o_reprun].SetPtr( &rc[q].PreC );    /*6*/
    aObj[ o_repra].SetPtr(  &rc[q].PreKT );    /*8*/
    aObj[ o_rermtm].SetPtr(  rc[q].rmtm );
    aObj[ o_rezz].SetPtr(   &rc[q].Zz );
    aObj[ o_remwt].SetPtr(  &rc[q].mwt );
    aObj[ o_reks].SetPtr(    rc[q].Ks );   /*d 3*/
    aObj[ o_regs].SetPtr(    rc[q].Gs );   /*d 3*/
    aObj[ o_rehs].SetPtr(    rc[q].Hs );   /*d 3*/
    aObj[ o_ress].SetPtr(    rc[q].Ss );   /*f 3*/
    aObj[ o_recps].SetPtr(   rc[q].Cps );  /*f 3*/
    aObj[ o_remvs].SetPtr(   rc[q].Vs );   /*f 3*/
    aObj[ o_renix].SetPtr(   rc[q].Nix );  /*f 3*/
    aObj[ o_reptst].SetPtr( &rc[q].Pst );  /*f 2*/
    aObj[ o_realbet].SetPtr(&rc[q].Comp ); /*f 2*/
    aObj[ o_reder].SetPtr(  &rc[q].Der ); /*f 2*/
    aObj[ o_redim].SetPtr(  &rc[q].nDC );   /*8*/
    aObj[ o_refloat1].SetPtr( &rc[q].Zz );  /*2*/
    aObj[ o_redbl1].SetPtr(   rc[q].Ks );  /*9*/
    aObj[ o_refloat2].SetPtr( rc[q].Ss );  /*18*/
    aObj[ o_restr].SetPtr(   rc[q].pct );
    aObj[ o_rename].SetPtr(  rc[q].name );
    aObj[ o_reform].SetPtr(  rc[q].form );

    // dynamic
    // if( rc[q].nDC > 0 ) {
    aObj[ o_redck].SetPtr( rc[q].DCk );
    aObj[ o_redck].SetDim( rc[q].nDC, 1 );
    aObj[ o_rerdc].SetPtr(  rc[q].rDC );
    aObj[ o_rerdc].SetDim( rc[q].nDC, 1 );
    aObj[ o_rescdc].SetPtr(  rc[q].scDC );
    aObj[ o_rescdc].SetDim( rc[q].nDC, 1 );
    aObj[ o_repardc].SetPtr(rc[q].ParDC);
    aObj[ o_repardc].SetDim( rc[q].nDC, 6 );
    //}
    aObj[ o_redcp].SetPtr( rc[q].DCp );
    aObj[o_redcp].SetDim( MAXCPCOEF, 1 );
    aObj[ o_rehkfc].SetPtr(rc[q].HKFc );
    aObj[o_rehkfc].SetDim( MAXHKFCOEF, 1 );
    aObj[ o_redvt].SetPtr( rc[q].DVt );
    aObj[ o_redvt].SetDim( MAXVTCOEF, 1 );
    aObj[ o_redst].SetPtr( rc[q].DSt );
    aObj[ o_redst].SetDim( MAXCPCOEF, 1 );
    aObj[ o_repkt].SetPtr( rc[q].pKt );
    aObj[ o_repkt].SetDim( MAXCPCOEF, 1 );

    aObj[ o_retcint].SetPtr( rc[q].TCint );
    aObj[ o_retcint].SetDim(  max((short)2,rc[q].nTp), 1 );
    aObj[ o_repint].SetPtr(  rc[q].Pint );
    aObj[ o_repint].SetDim(  max((short)2,rc[q].nPp), 1 );
    //if( rc[q].PreKP == S_ON && rc[q].nTp && rc[q].nPp ){
    aObj[ o_relgK].SetPtr(  rc[q].logK );
    aObj[ o_relgK].SetDim(  rc[q].nTp, rc[q].nPp );
    //}
    //aObj[ o_retprn].SetPtr( rc[q].tprn  ); aObj[o_retprn].SetDim(1, SPPTPRNBUFSIZE);
    //if( rc[q].Nsd > 0 ) {
    aObj[ o_resdref ].SetPtr( rc[q].sdref );
    aObj[o_resdref].SetDim(rc[q].Nsd, 1);
    aObj[ o_resdval ].SetPtr( rc[q].sdval );
    aObj[o_resdval].SetDim(rc[q].Nsd, 1);
    // }
    rcp=&rc[q];
}

// set dynamic Objects ptr to values
void TReacDC::dyn_set(int q)
{
    ErrorIf( rcp!=&rc[q], GetName(), "Illegal access to rc in dyn_set.");
    memcpy( rcp->pstate, rt[nRT].UnpackKey(), RE_RKLEN );
    rc[q].DCk =   (char (*)[DC_RKLEN])aObj[ o_redck ].GetPtr();
    rc[q].rDC =   (char *)aObj[ o_rerdc ].GetPtr();
    rc[q].scDC =  (double *)aObj[ o_rescdc ].GetPtr();
    rc[q].ParDC = (double (*)[6])aObj[ o_repardc ].GetPtr();
    rc[q].TCint = (float *)aObj[ o_retcint ].GetPtr();
    rc[q].Pint =  (float *)aObj[ o_repint ].GetPtr();
    rc[q].DCp =   (float *)aObj[ o_redcp ].GetPtr();
    rc[q].HKFc =  (float *)aObj[ o_rehkfc ].GetPtr();
    rc[q].DVt =   (float *)aObj[ o_redvt ].GetPtr();
    rc[q].DSt =   (float *)aObj[ o_redst ].GetPtr();
    rc[q].pKt =   (float *)aObj[ o_repkt ].GetPtr();
    rc[q].logK =  (float *)aObj[ o_relgK ].GetPtr();
    rc[q].sdref = (char (*)[V_SD_RKLEN])aObj[ o_resdref ].GetPtr();
    rc[q].sdval = (char (*)[V_SD_VALEN])aObj[ o_resdval ].GetPtr();
    rc[q].tprn =  (char *)aObj[ o_retprn ].GetPtr();
}

// free dynamic memory in objects and values
void TReacDC::dyn_kill(int q)
{
    ErrorIf( rcp!=&rc[q], GetName(), "Illegal access to bc in dyn_kill.");
    rc[q].DCk =   (char (*)[DC_RKLEN])aObj[ o_redck ].Free();
    rc[q].rDC =   (char *)aObj[ o_rerdc ].Free();
    rc[q].scDC =  (double *)aObj[ o_rescdc ].Free();
    rc[q].ParDC = (double (*)[6])aObj[ o_repardc ].Free();
    rc[q].TCint = (float *)aObj[ o_retcint ].Free();
    rc[q].Pint =  (float *)aObj[ o_repint ].Free();
    rc[q].DCp =   (float *)aObj[ o_redcp ].Free();
    rc[q].HKFc =  (float *)aObj[ o_rehkfc ].Free();
    rc[q].DVt =   (float *)aObj[ o_redvt ].Free();
    rc[q].DSt =   (float *)aObj[ o_redst ].Free();
    rc[q].pKt =   (float *)aObj[ o_repkt ].Free();
    rc[q].logK =  (float *)aObj[ o_relgK ].Free();
    rc[q].sdref = (char (*)[V_SD_RKLEN])aObj[ o_resdref ].Free();
    rc[q].sdval = (char (*)[V_SD_VALEN])aObj[ o_resdval ].Free();
    rc[q].tprn =  (char *)aObj[ o_retprn ].Free();
}


// free dynamic memory in objects and values
void TReacDC::w_dyn_kill()
{
    rcp->ParDC = (double (*)[6])aObj[ o_repardc ].Free();
}


// realloc dynamic memory
void TReacDC::dyn_new(int q)
{
    ErrorIf( rcp!=&rc[q], GetName(), "Illegal access to bc in dyn_new.");
    short nTp, nPp;
    ErrorIf( rc[q].nDC < 1, GetName(), "Count of DC is < 1." );

    rc[q].DCk =   (char (*)[DC_RKLEN])aObj[ o_redck ].Alloc(rc[q].nDC, 1,DC_RKLEN);
    rc[q].rDC =   (char *)aObj[ o_rerdc ].Alloc(rc[q].nDC, 1, A_);
    rc[q].scDC =  (double *)aObj[ o_rescdc ].Alloc(rc[q].nDC, 1, D_);

    nTp = max( (short)2, rc[q].nTp );
    rc[q].TCint = (float *)aObj[ o_retcint ].Alloc( nTp, 1, F_);
    nPp = max( (short)2, rc[q].nPp );
    rc[q].Pint =  (float *)aObj[ o_repint  ].Alloc(nPp, 1, F_);

    if( rc[q].PreKP != S_OFF && rc[q].nTp >0 && rc[q].nPp>0 )
        rc[q].logK =  (float *)aObj[ o_relgK ].Alloc( rc[q].nTp, rc[q].nPp, F_);
    else
        rc[q].logK = (float *)aObj[ o_relgK ].Free();


    if( rc[q].PreDC == S_OFF )
        rc[q].DCp =   (float *)aObj[ o_redcp ].Free();
    else
        rc[q].DCp =   (float *)aObj[ o_redcp ].Alloc( MAXCPCOEF, 1, F_);

    if( rc[q].PrAki == S_OFF )  /* HKF - ¬model */
        rc[q].HKFc =  (float *)aObj[ o_rehkfc ].Free();
    else
        rc[q].HKFc =  (float *)aObj[ o_rehkfc ].Alloc( MAXHKFCOEF, 1, F_);

    if( rc[q].PreDV == S_OFF )  /* equations dV(T,P) */
        rc[q].DVt =   (float *)aObj[ o_redvt ].Free();
    else
        rc[q].DVt =   (float *)aObj[ o_redvt ].Alloc( MAXVTCOEF, 1, F_);

    if( rc[q].PreDS == S_OFF ) /* array of coeff. S(T) */
        rc[q].DSt =   (float *)aObj[ o_redst ].Free();
    else
        rc[q].DSt =   (float *)aObj[ o_redst ].Alloc( MAXCPCOEF, 1, F_);

    if( rc[q].PreKT == S_OFF )  /* array of coef. regres. lgK(T) */
        rc[q].pKt =   (float *)aObj[ o_repkt ].Free();
    else
        rc[q].pKt =   (float *)aObj[ o_repkt ].Alloc( MAXCPCOEF, 1, F_);

    if( rc[q].Nsd > 0 )
    {
        rc[q].sdref = (char (*)[V_SD_RKLEN])aObj[ o_resdref ].Alloc(rc[q].Nsd,
                      1, V_SD_RKLEN);
        rc[q].sdval = (char (*)[V_SD_VALEN])aObj[ o_resdval ].Alloc(rc[q].Nsd,
                      1, V_SD_VALEN);
    }
    else
    {
        rc[q].sdref = (char (*)[V_SD_RKLEN])aObj[ o_resdref ].Free();
        rc[q].sdval = (char (*)[V_SD_VALEN])aObj[ o_resdval ].Free();
    }
    // if( rc[q].tprn == 0 )
    //   rc[q].tprn = (char *)aObj[o_retprn].Alloc( 1, 256, S_ );
}


void TReacDC::w_dyn_new()
{
    rcp->ParDC = (double (*)[6])aObj[ o_repardc ].Alloc( rcp->nDC, 6, D_);
}

//set default information
void TReacDC::set_def( int q)
{
    ErrorIf( rcp!=&rc[q], GetName(), "Illegal access to rc in set_def.");
    TProfil *aPa=(TProfil *)(&aMod[RT_PARAM]);

    //memset( rc[q].pct, 0, sizeof( REACDC )-DC_RKLEN );
    memcpy( rc[q].pct, aPa->pa.REpct, 6 );
    memcpy( &rc[q].PreC, aPa->pa.REpdc, 6 );
    memcpy( &rc[q].PreKT, aPa->pa.REpvc, 8 );
    strcpy( rc[q].name, "reacdc");
    strcpy( rc[q].form, "`" );
    strcpy( rc[q].rmtm, "`" );
    memset( &rc[q].nDC, 0, 8*sizeof(int));
    rc[q].Nsd =1;
    rc[q].Zz = rc[q].mwt = 0.;
    //    values
    rc[q].Comp = rc[q].Expa = rc[q].Der = rc[q].DerB = FLOAT_EMPTY;
    rc[q].Vs[0] = 0;
    rc[q].Vs[1] = rc[q].Vs[2] = FLOAT_EMPTY;
    rc[q].Cps[0] = 0;
    rc[q].Cps[1] = rc[q].Cps[2] = FLOAT_EMPTY;
    rc[q].Ks[0] = rc[q].Ks[1] = rc[q].Ks[2] = DOUBLE_EMPTY;
    rc[q].Gs[0] = rc[q].Gs[1] = rc[q].Gs[2] = DOUBLE_EMPTY;
    rc[q].Hs[0] = rc[q].Hs[1] = rc[q].Hs[2] = DOUBLE_EMPTY;
    rc[q].Ss[0] = rc[q].Ss[1] = rc[q].Ss[2] = FLOAT_EMPTY;
    rc[q].Nix[0] = rc[q].Nix[1] = rc[q].Nix[2] = FLOAT_EMPTY;
    rc[q].TCst = (float)STANDARD_TC;
    rc[q].Pst  = (float)STANDARD_P;
    // pointers
    rc[q].DCk =   0;
    rc[q].rDC =   0;
    rc[q].scDC =  0;
    rc[q].ParDC = 0;
    rc[q].TCint = 0;
    rc[q].Pint =  0;
    rc[q].DCp =   0;
    rc[q].HKFc =  0;
    rc[q].DVt =   0;
    rc[q].DSt =   0;
    rc[q].pKt =   0;
    rc[q].logK =  0;
    rc[q].sdref = 0;
    rc[q].sdval = 0;
    rc[q].tprn =  0;
}

// return true if nessasary recalc
bool TReacDC::check_input( const char *key, int Level )
{
    time_t tre, tc;
    int q, i;
    bool iRet = false;
    if( Level != 1 )
        return true;
    tre = rt[nRT].Rtime();
    for( i=0; i<nQ; i++)
        if( rcp == &rc[i])
            break;
    q =i;
    ods_link( q+1);
    try
    {
        for( i=0; i<rc[q].nDC; i++ )
        {
            if( rc[q].rDC[i] == SRC_DCOMP )
            {
                TDComp::pm->TryRecInp( rc[q].DCk[i], tc, 0 );
                // TDComp::pm->RecInput( rc[q].DCk[i] );
                // tc = rt[RT_DCOMP].Rtime();
            }

            else if( rc[q].rDC[i] == SRC_REACDC )
            {
                TryRecInp( rc[q].DCk[i], tc, q+1 );
                //RecInput( rc[q].DCk[i] );
                //tc = rt[nRT].Rtime();
            }

            else tc = 0L;
            if( tre <= tc )
                iRet = true;
        }
    }
    catch( TError& xcpt )
    {
        ods_link(q);
        rt[nRT].Find(key);
        Error( key/*xcpt.title.c_str()*/, xcpt.mess.c_str());
    }
    ods_link(q);
    rt[nRT].Find(key); //   RecInput( key );
    return iRet;
}

// Input nessasery data and links objects
void TReacDC::RecInput( const char *key )
{
    TCModule::RecInput( key );
    //  memcpy( rcp->pstate, rt[nRT].UnpackKey(), RE_RKLEN );

}

//Rebild record structure before calc

int TReacDC::RecBuild( const char *key, int mode  )
{
    int i, iir, Ndc, Nrc, Nc1, Nn1 = 0, Nf1, Nr1;
    vstr pkey(81);
    int CM,CE,CV;
    gstring str;
    short oldnDC = rcp->nDC/*, newnDC*/;
    uint j;

    TCStringArray aDclist;
    TCStringArray aRclist;
    TCStringArray aDclist_old;
    TCStringArray aRclist_old;

AGAIN_MOD:
    int ret = TCModule::RecBuild( key, mode );
    memcpy( rcp->pstate, key/*rt[nRT].UnpackKey()*/, RE_RKLEN );
    /*newnDC = rcp->nDC;
    rcp->nDC = oldnDC; */
    if( ret == VF3_3 && !( !rcp->PreC || rcp->PreC == ' ' ) )
        return ret;
    if( ret == VF3_1 )
        Nn1 = 1;
    if( rcp->nTp < 0 || rcp->nPp < 0 || rcp->Nsd < 0 || rcp->Nsd > 4 )
        if( vfQuestion( window(), GetName(),"Invalid values! Repeat?" ))
            goto AGAIN_MOD;
        else
            Error( GetName(), "Invalid values!");
    CM = toupper( rcp->pct[0] );
    CE = toupper( rcp->pct[1] );
    CV = toupper( rcp->pct[2] );
    rcp->PreKT = S_OFF;
    rcp->PreKP = S_OFF;
    rcp->PreDS = S_OFF;
    rcp->PreDV = S_OFF;
    rcp->PreDC = S_OFF;
    rcp->PrSVC = S_OFF;
    rcp->PrAki = S_OFF;
    rcp->PrIso = S_OFF;
    switch( CM )
    {
    case CTPM_CPT:
        rcp->PreDC = S_ON;
        rcp->PreKT = S_ON;
        break;
    case CTPM_HKF:
        rcp->PrAki = S_ON;
        break;
    case CTPM_ISO:
        rcp->PreKT = S_ON;
        rcp->PrIso = S_ON;
        break;
    case CTPM_SOR:
        rcp->PreDS = S_ON;
        break;
    case CTPM_EOS:
        if( CE == CTM_DKR /*|| CE == CTM_PPE*/ )
            rcp->PreDS = S_ON;
        break;
    case CTPM_REA:
        switch(CE)
        {
        case CTM_LGK:
        case CTM_LGX:
            rcp->PreDC = S_ON;
        case CTM_EK1:
        case CTM_EK3:
        case CTM_EK2:
            rcp->PreKT = S_ON;
            break;
        case CTM_IKZ:
            rcp->PreKP = S_ON;
            break; /* calc lgK(TP) */
        case CTM_DKR:
            /* 19.05.98 */
        case CTM_PPE:
            rcp->PreKT = S_ON;
            break;
        }
    }
    switch(CV)
    {
    case CPM_VKE:
    case CPM_VBE: /* rcp->PreKP = S_ON; */
        rcp->PreDV = S_ON;
        break;
    case CPM_PCR:
        rcp->PrAki = S_ON;
    }
    /************/
    Nc1 = 0;
    Nr1 = 0;
    Nf1 = 0;

    //REACDC&DCOMP  keypart
    rt[RT_REACDC].MakeKey( RT_REACDC, pkey, K_ANY, K_ANY, K_ANY, K_ANY, K_END);

    if( rcp->rDC )
    { // calc count DC and RC
        for( i=0; i<rcp->nDC; i++ )
        {
            switch( rcp->rDC[i] )
            {
            case SRC_DCOMP:
                Nc1++;
                continue;
            case SRC_REACDC:
                Nr1++;
                continue;
            case SRC_NEWISO:
            case SRC_NEWDC:
                Nn1++;
                continue;
            case SRC_FICT:
                Nf1++;
                continue;
            default:
                Error( GetName(), "Invalid rcp->rDC[i]");
            }
        }
     }
    // made vectors selections DCOMP and REACDC
    if( Nc1>0 || Nr1>0 )
    {
        /* Build old selections DCOMP and REACDC */
        aDclist_old.Clear();
        aRclist_old.Clear();

        for( i=0; i<rcp->nDC; i++ )
        {
          gstring key_dr = gstring( rcp->DCk[i], 0, DC_RKLEN );
          if( rcp->rDC[i] == SRC_DCOMP )
              aDclist_old.Add( key_dr.c_str() );
          else
             if( rcp->rDC[i] == SRC_REACDC )
                aRclist_old.Add( key_dr.c_str() );
        }
    }

AGAINRC:
    aRclist = vfMultiKeysSet( window(),
       "Please, mark ReacDC keys to be included",
       RT_REACDC, pkey, aRclist_old );
    aDclist = vfMultiKeysSet( window(),
       "Please, mark DComp keys to be included",
       RT_DCOMP, pkey, aDclist_old );


    if( aRclist.GetCount() < 1 && aDclist.GetCount() < 1 )
    {
       switch ( vfQuestion3(window(), GetName(),
            "Number of selected ReacDC&DComp keys < 1.\n"
            " Mark again, proceed without ReacDC&DComp or Cancel?",
            "&Repeat", "&Proceed"))
       {
         case VF3_1:
                goto AGAINRC;
                ;
         case VF3_2:
                break;
         case VF3_3:  Error( GetName(),
                      "No ReacDC&DComp records selected...");
       }
    }



    /*================================*/
    rcp->nDC = aDclist.GetCount()+aRclist.GetCount()+Nn1+Nf1;
    // ???? 28/02/02 Sveta
    // if( (oldnDC != newnDC) && (newnDC != rcp->nDC) )
    //    rcp->nDC = newnDC;
    dyn_new();

    /*================================*/
    // get aMcv+aMrv  (Remake cod! Must be by component groups )
    for( i=0,iir=0; i<rcp->nDC; i++ )
    {
        if( !rcp->scDC[i] )
            rcp->scDC[i] = 1;
        if( i < aRclist.GetCount() )
        {
            memcpy( rcp->DCk[i], aRclist[i].c_str(), DC_RKLEN );
            rcp->rDC[i] = SRC_REACDC;
            iir++;
        }
        else if( i< aDclist.GetCount()+aRclist.GetCount() )
        {
            memcpy( rcp->DCk[i], aDclist[i-iir].c_str(), DC_RKLEN );
            rcp->rDC[i] = SRC_DCOMP;
        }
        else
        {
            if( rcp->PrIso != S_OFF )
                rcp->rDC[i] = SRC_NEWISO;
            else
                rcp->rDC[i] = SRC_NEWDC;
            memcpy( rcp->DCk[i], rcp->pstate, DC_RKLEN );
        }
    } //i

    /**************/
    rt[nRT].SetKey(key); //   RecInput( key );
    return ret;
}

//Calculate record
void
TReacDC::RecCalc( const char* key )
{
    int i,q;
    for( i=0; i<nQ; i++)
        if( rcp == &rc[i])
            break;
    q =i;
    w_dyn_new();
    try
    {
        Recalc( q, key );
        w_dyn_kill();
    }
    catch( TError& xcpt )
    {
        if( rcp != rc+q )
        {
            ods_link( q );
            db->Find(key);
        }
        w_dyn_kill();
        Error(  key , xcpt.mess.c_str() );
    }
    TCModule::RecCalc(key);
}

//Calc thermodynamic values vedushego DC of REACDC for T,P
//that giv from twp. Result set to twp.
//
void
TReacDC::RCthermo( int q, int p )
{
    int CM,CE,CV, j, isotop = 0;
    time_t tim;
    vstr  /*buf[121],*/ dckey(DC_RKLEN + 10);

    aW.twp->Tst = aW.twp->TCst + C_to_K;
    aW.twp->RT = R_CONSTANT * aW.twp->T; /* !!!! */
    /* aW.twp->ln10RT ??? */
    if( fabs( aW.twp->T - 298.15 ) < 0.01 && fabs(aW.twp->P-1.) < 0.01 )
    {
        aW.twp->K = rcp->Ks[0];
        aW.twp->lgK = rcp->Ks[1];
        aW.twp->dG =  rcp->Gs[0];
        aW.twp->dS =  rcp->Ss[0];
        aW.twp->dH =  rcp->Hs[0];
        aW.twp->dCp = rcp->Cps[0];
        aW.twp->dV =  rcp->Vs[0];
        aW.twp->G =  rcp->Gs[1];
        aW.twp->S =  rcp->Ss[1];
        aW.twp->H =  rcp->Hs[1];
        aW.twp->Cp = rcp->Cps[1];
        aW.twp->V =  rcp->Vs[1];
        return;
    }
    if( rcp != rc+q )
        ods_link( q );
    aW.ods_link( p );
    TDComp* aDC=(TDComp *)(&aMod[RT_DCOMP]);
    aDC->ods_link(0);

    /*test method of calculations thermodynamic and set counters*/
    CM = toupper( rc[q].pct[0] );
    CE = toupper( rc[q].pct[1] );
    CV = toupper( rc[q].pct[2] );
    /*  memcpy( dckey, rc[q].pstate, DC_RKLEN ); */
    //  dckey[DC_RKLEN] = 0;

    if( CM == CTPM_HKF )
    {
        /* calc water fitches */
        if( fabs(aW.twp->TC - aSta.Temp) > 0.01 ||
                ( aW.twp->P > 1e-4 && fabs( aW.twp->P - aSta.Pres ) > 0.001 ))
        { /* calc fitches water for HGK */
            aSta.Temp = aW.twp->TC;
            aSta.Pres = aW.twp->P;

            TSupcrt sapCrt;
            sapCrt.Supcrt_H2O( aSta.Temp, &aSta.Pres);
            aW.twp->P = aSta.Pres;
            aW.twp->wRo  = aSta.Dens[aSpc.isat];
            aW.twp->wEps = aWp.Dielw[aSpc.isat];
//            aW.twp->wVis = aWp.Viscw[aSpc.isat];
        }
        else
        { /* calculated before */
            aW.twp->P = aSta.Pres;
        }
    }
    w_dyn_new();
    /*test component of reactions and calc that t/d fitches*/
    for( j=0; j<rc[q].nDC; j++ )
    {
        strncpy( dckey, rc[q].DCk[j], DC_RKLEN );
        /* !!!!!!!! exept field t/d base any? (*) */
        aW.ods_link( p+1 );
        /* clear new work structure */
        aW.set_zero( p+1 );
        aW.twp->P = aW.WW(p).P;
        aW.twp->Pst = aW.WW(p).Pst;
        aW.twp->TC = aW.WW(p).TC;
        aW.twp->T = aW.WW(p).T;
        aW.twp->TCst = aW.WW(p).TCst;
        aW.twp->Tst = aW.WW(p).Tst;
        aW.twp->RT = aW.WW(p).RT;
        aW.twp->wRo = aW.WW(p).wRo;
        aW.twp->wEps = aW.WW(p).wEps;
//        aW.twp->wVis = aW.WW(p).wVis;

        aW.twp->unE = aW.WW(p).unE;  /* test edinic izmereniya */
        aW.twp->unV = aW.WW(p).unV;
        memcpy( aW.twp->DRkey, dckey, DC_RKLEN );

        switch( rc[q].rDC[j] )
        {
        case SRC_REACDC: /* recursion load new record */
            ods_link( q+1 );
            TryRecInp( dckey, tim, q+1 );
            aW.twp->pSD = SRC_REACDC;
            aW.twp->pTM = S_OFF;
            aW.twp->Pst = rc[q+1].Pst;
            aW.twp->TCst = rc[q+1].TCst;
            aW.twp->T = aW.twp->TC + C_to_K;
            /* Call RCthermo! */
            RCthermo( q+1, p+1 );
            break;
        case SRC_FICT:   /* fictiv component */
            break;
        case SRC_DCOMP: /* thermochemical component */
            aDC->ods_link( 0 );   /* (q+1) ??? */
            aDC->TryRecInp( dckey, tim, 0 );
            aW.twp->pSD = SRC_DCOMP;
            aW.twp->pTM = S_ON;
            aW.twp->Pst = aDC->dcp->Pst;
            aW.twp->TCst = aDC->dcp->TCst;
            aW.twp->T = aW.twp->TC + C_to_K;
            aDC->DCthermo( 0, p+1 );
            aW.WW(p).P = aW.twp->P;  /* !!!!!!return P on KNP */
            break;
        case SRC_NEWDC: /* vedushiy component */
            aW.twp->pSD = SRC_NEWDC;
            aW.twp->pTM = S_REM;
            isotop = 0;
            goto CALCULATE_DELTA_R;
        case SRC_NEWISO: /* isotop form  */
            aW.twp->pSD = SRC_NEWISO;
            aW.twp->pTM = S_REM;
            isotop = 1;
            goto CALCULATE_DELTA_R;
        default:
            Error( GetName(),"Invalid DC code in RCthermo.");
        }

        /* Unload work structure to REACDC */
        rc[q].ParDC[j][_Gs_] = aW.twp->G;
        rc[q].ParDC[j][_Hs_] = aW.twp->H;
        rc[q].ParDC[j][_Ss_] = aW.twp->S;
        rc[q].ParDC[j][_Cps_] = aW.twp->Cp;
        rc[q].ParDC[j][_Vs_] = aW.twp->V;

    } /* j */
    /* Is it error? */  aW.WW(p).pTM = S_REM;

CALCULATE_DELTA_R:
    aW.ods_link( p );
    if( rcp != rc+q )
        ods_link( q );
    if( rc[q].rDC[rc[q].nDC-1] == SRC_DCOMP ||
            rc[q].rDC[rc[q].nDC-1] == SRC_REACDC )
    { /*Calc parametres of reaction if vedushiy d or r */
        Calc_rDCD( q, p );
        return;
    }

    /* calc delta reactions if data TP */
    aW.WW(p).pTM = S_REM;
    switch( CM )
    {
    case CTPM_HKF:
        switch( CE )
        {
        default:
            Error(dckey.p,"Invalid code in CE.");
        case CTM_HKF:
            calc_tphkf_r( q, p );
            break; /* calc aqueous species */
        case CTM_AKI:
            calc_akinf_r( q, p, CE, CV );
            break; /* calc on Akinfiev */
        }
        break;
    case CTPM_CPT:
    case CTPM_REA: /* reactions if vedushiy component not new */
        switch( CE )
        {
        case CTM_CST:
        case CTM_LGX:
        case CTM_LGK:
        case CTM_EK1:
        case CTM_EK3:
        case CTM_EK2:
            calc_lgk_r( q, p, CE, CV  );
            break; /* On lgK(T) */
        case CTM_IKZ:
            calc_r_interp( q, p, CE, CV );
            break;
            /*  case CTM_EK1:  dbc = calc_isocoul_r( q, p, CE, CV );
                               break;  */
        case CTM_DKR:
            /*                    case CTM_DKE:  calc_dissoc_r( q, p, CE, CV ); */
            break;
        default:
            Error(dckey.p,"Invalid code in CE.");
        }
        break;
    case CTPM_ISO:
        calc_iso_a( q, p );
        break;
    case CTPM_SOR: /* sorbcion  */
        calc_exion_r( q, p );
        break;
    default:
        {  /* Illegal cod method of calculation  */
            gstring msg = "Error in PDB record '";
            msg += dckey;
            msg += "'.\nChange record?";
            //if( !vfQuestion( GetName(), buf );
            Error( dckey.p, "Invalid code of calculation method for REACDC!");
            //  else  RecBuild( dckey );  // !!!!!! Recalc new record?
        }

    }
    /* Volium */
    switch ( CV )
    {
    case CPM_CON:
    case CPM_VKE:
    case CPM_VBE:
    case CPM_VBL:
    case CPM_CEH:
        calc_tpcv_r( q, p, CM, CV );
    default:
        break;
    }
    /* Calc t/d fitches of vedushego componenta */
    if( !isotop )
    { /* this is usual reactions  */
        double G=0.0, S=0.0, H=0.0, Cp=0.0, V=0.0;
        for( j=0; j<rc[q].nDC-1; j++ )
        {
            G  += rc[q].scDC[j]*rc[q].ParDC[j][_Gs_];
            S  += rc[q].scDC[j]*rc[q].ParDC[j][_Ss_];
            H  += rc[q].scDC[j]*rc[q].ParDC[j][_Hs_];
            Cp += rc[q].scDC[j]*rc[q].ParDC[j][_Cps_];
            V  += rc[q].scDC[j]*rc[q].ParDC[j][_Vs_];
        }
        /*  No calc steh coeff */
        aW.WW(p).G =  (aW.WW(p).dG - G)/rc[q].scDC[rc[q].nDC-1];
        aW.WW(p).S =  (aW.WW(p).dS - S)/rc[q].scDC[rc[q].nDC-1] /* -foS */ ;
        aW.WW(p).H =  (aW.WW(p).dH - H)/rc[q].scDC[rc[q].nDC-1];
        aW.WW(p).Cp = (aW.WW(p).dCp - Cp)/rc[q].scDC[rc[q].nDC-1];
        aW.WW(p).V =  (aW.WW(p).dV - V)/rc[q].scDC[rc[q].nDC-1];
    }
    else
    { /*This is isotop form  */
        double R_T, LNK;
        double G=0.0, S=0.0, H=0.0, Cp=0.0, V=0.0;

        R_T = aW.WW(p).T * R_CONSTANT;
        LNK = aW.WW(p).lgK / 1000;

        for( j=0; j < rc[q].nDC-1; j++ )
        {
            G  += rc[q].scDC[j]*(rc[q].ParDC[j][_Gs_]+rc[q].Nix[j]*R_T*LNK);
            S  += rc[q].scDC[j]*(rc[q].ParDC[j][_Ss_]+rc[q].Nix[j]*aW.WW(p).dS);
            H  += rc[q].scDC[j]*rc[q].ParDC[j][_Hs_];
            Cp += rc[q].scDC[j]*rc[q].ParDC[j][_Cps_];
            V  += rc[q].scDC[j]*rc[q].ParDC[j][_Vs_];
        }
        aW.WW(p).G =  -G/rc[q].scDC[rc[q].nDC-1];
        aW.WW(p).S =  -S/rc[q].scDC[rc[q].nDC-1];
        aW.WW(p).H =  (aW.WW(p).dH - H)/rc[q].scDC[rc[q].nDC-1];
        aW.WW(p).Cp = (aW.WW(p).dCp - Cp)/rc[q].scDC[rc[q].nDC-1];
        aW.WW(p).V =  (aW.WW(p).dV - V)/rc[q].scDC[rc[q].nDC-1];
    }
    //Sveta 10/09/99
    w_dyn_kill();

}

/* --------------------------------------------------------------- */
/* PRONSPREP - Prediction of Partial Molal Properties and Equation of
19.05.98       State parameters for aqueous complexes.
       Modified by D.A.Kulik on Feb.16 1998 at IOW Rostok
       FINAL VERSION OF APRIL 20, 1996 -
       CONSISTENT WITH THE EQUATIONS, TABLES AND FIGURES IN
       SVERJENSKY, SHOCK, AND HELGESON (1997):
       "Prediction of the thermodynamic properties of aqueous metal complexes
        to 1000 C and 5kb" (Geochim. Cosmochim. Acta, 61, 1359-1412).
 
   --- THIS VERSION IS VALID FOR MONOVALENT LIGANDS ONLY EXCEPT OH- ---
   --- Modified by D.A.Kulik to inclide 1st complexes of SO4-2 and CO3-2
         Modified by Sverjensky and Shock, 1992.
       Created by Dimitri A. Sverjensky (August, 1988).
*/
void TReacDC::PronsPrep( const char *key )
{

    double BETA1,BETA2,BETA3,BETA4,ZC,GC,HC,SC,CPC,VC,ZL,GL,HL,SL,CPL,VL,
    Z,/*G,*/H,S,CP,V, Z1,G1,H1,S1,CP1,V1, Z2,G2,H2,S2,CP2,V2,
    Z3,G3,H3,S3,CP3,V3, Z4,G4,H4,S4,CP4,V4, DELGR1, DELGR2,
    DELGR3, DELGR4, AZ, AZP, BZ, BZP, ALPHA, BETA, DELS1,
    DELS2, DELS3, DELS4, DELSR1, DELSR2, DELSR3, DELSR4,
    DELHR1, DELHR2, DELHR3, DELHR4, DELCP1, DELCP2, DELCP3, DELCP4,
    DELVR1, DELVR2, DELVR3, DELVR4, GZ, CSC, LSC, DZ, scC;
    int i, iL, iC, scL,  b1, b2, b3, b4;
    vstr dcn(MAXRKEYLEN+5);
    int Rfind;

    /* Finding index of cation and anion */
    for( i=0; i<2; i++ )
    {
        Z = rcp->ParDC[i][_Zs_];
        if( Z > 0.0 )
            iC = i;
        else if( Z < 0.0 )
            iL = i;
        else Error( GetName(), "Z=0 in PronsPrep");
    }
    /* Loading data for cation */
    ZC = rcp->ParDC[iC][_Zs_];
    GC = rcp->ParDC[iC][_Gs_]/cal_to_J;
    HC = rcp->ParDC[iC][_Hs_]/cal_to_J;
    SC = rcp->ParDC[iC][_Ss_]/cal_to_J;
    CPC = rcp->ParDC[iC][_Cps_]/cal_to_J;
    VC = rcp->ParDC[iC][_Vs_]*10.0;
    CSC = rcp->scDC[iC];
    scC = fabs(CSC);
    if ( scC > 1. )
        Error( GetName(), "fabs of Cps values for DC  > 1 in PronsPrep");

    /* Loading data for ligand */
    ZL = rcp->ParDC[iL][_Zs_];
    GL = rcp->ParDC[iL][_Gs_]/cal_to_J;
    HL = rcp->ParDC[iL][_Hs_]/cal_to_J;
    SL = rcp->ParDC[iL][_Ss_]/cal_to_J;
    CPL = rcp->ParDC[iL][_Cps_]/cal_to_J;
    VL = rcp->ParDC[iL][_Vs_]*10.0;
    LSC = rcp->scDC[iL];

    /* Loading Log_beta using st.coeff.at ligand */
    scL = (int)fabs(LSC);
    BETA1 = 0;
    BETA2 = 0;
    BETA3 = 0;
    BETA4 = 0; //BETA = 0;
    b1 = b2 = b3 = b4 = 0;
    BETA = rcp->Ks[1];
    if(BETA == 0.0 )
        BETA = 1e-9;
    switch( scL )
    {
    case 1:
        BETA1 = BETA;
        b1 = 1;
        break;
    case 2:
        BETA2 = BETA;
        b2 = 1;
        break;
    case 3:
        BETA3 = BETA;
        b3 = 1;
        break;
    case 4:
        BETA4 = BETA;
        b4 = 1;
        break;
    default:
        Error( GetName(),
               "Invalid stoichiometry coefficients of DC in reaction.");
    }

    /*
         Calculations for the first complex
    */
    Z= ZC + ZL;
    DELGR1 = lg_to_ln * 1.98719 * 298.15 * BETA1;
    G1 = DELGR1 + GC + GL;
    /*
      Now the entropy predictor starts for the first complex
    */
    if( ZL > -1.1 )
    {
        AZ   = 0.016241*Z - 0.000479;
        AZP  = -0.360972*Z + 0.3209;
        BZ   = 0.32102*Z  - 0.05996;
        BZP  = 8.2198*Z - 1.557;
        ALPHA = AZ*(SL+(-5.0*ZL)) + AZP;
        BETA = BZ*(SL+(-5.0*ZL)) + BZP;

        DELS1 = ALPHA*(SC+(-5.0*ZC)) + BETA;
        DELSR1= DELS1;
    }
    else
    {
        /* Added by D.A.Kulik for SO4-2 and CO3-2 ligands
           according to Sverjensky et al. 1997 p.1387
          uncertain to about 5 cal/mole/K (cmp. CaCO3, CaSO4) */
        if(VL > 0.0)
            /* Sulfate complex */
            DELSR1 = (0.055 - 0.055*Z)*SC + 13.84*Z + 18.16;
        else
            /*  Carbonate complex */
            DELSR1 = (0.213 - 1.617*Z)*SC + 66.82*Z + 28.67;
    }

    S1 = DELSR1 + SC + SL;
    Z1 = Z;

    DELHR1 = DELGR1 + (298.15*DELSR1);
    H1 = DELHR1 + HC + (1.0*HL);
    /*
      Here the Cp predictor starts for the first complex.
    	modified 8 Aug 1992 after latest revision
    	Modified 17 March 1992 to incorporate changes
    	from last July 1991
    */
    if(ZL > -1.1)
    {
        DZ = 0.856*CPL - 2.1 + 45.3*ZC;
        DELCP1 = 1.25*CPC + DZ;
    }
    else
        /* Added by D.A.Kulik for SO4-2 and CO3-2 ligands of M+2 (for CaSO4aq +1.09)
          according to assumption of Sverjensky et al. 1997 p. 1384
          uncertain to 4 cal/mole/k (cmp. CaCO3, KSO4-, AgCO3-)*/
        DELCP1 = 46.91 +1.09;

    CP1 = DELCP1 + CPC + CPL;

    /*
       Here the Volume predictor starts for the first complex
    */

    if(ZL > -1.1)
        DELVR1  = 0.11419*VC + 8.9432;
    else
        /* Added by D.A.Kulik for SO4-2 and CO3-2 ligands
          according to Sverjensky et al. 1997 p.1382  */
        DELVR1  = (-0.25*Z-0.3)*VC + (3.32-2.88*Z);

    V1 = DELVR1 + VC + VL;
    Z1 = Z;
    /*
     END OF PREDICTORS FOR THE FIRST COMPLEX
    */

    /* second complex */

    Z = Z + ZL;

    DELGR2 = lg_to_ln * 1.98719 * 298.15 * BETA2;
    G2 = DELGR2 + GC + (2.0*GL);

    AZ= 0.016241*Z - 0.000479;
    AZP= -0.360972*Z + 0.3209;
    BZ= 0.32102*Z  - 0.05996;
    BZP= 8.2198*Z - 1.557;

    ALPHA = AZ*(SL+(-5.0*ZL)) + AZP;
    BETA = BZ*(SL+(-5.0*ZL)) + BZP;

    DELS2 = ALPHA*(S1+(-5.0*(ZC+ZL))) + BETA;
    DELSR2= DELS1 + DELS2;

    S2 = DELSR2 + SC + (2.0*SL);
    Z2 = Z;

    DELHR2 = DELGR2 + (298.15*DELSR2);
    H2 = DELHR2 + HC + (2.0*HL);
    /*
     CHANGED 17 MARCH 1992!!!
    */
    GZ = 0.89*CPC + 0.72*CPL + 16.3;
    DELCP2 = DELCP1 + GZ;
    CP2 = DELCP2 + CP1 + CPL;
    Z2 = Z;

    DELVR2  = 0.11419*V1 + 8.9432;
    V2 = DELVR2 + DELVR1 + VC + (2.0*VL);
    Z2 = Z;
    /*
     END OF PREDICTORS FOR THE SECOND COMPLEX
    */
    /* third complex */

    Z = Z + ZL;

    DELGR3 = lg_to_ln * 1.98719 * 298.15 * BETA3;
    G3 = DELGR3 + GC + (3.0*GL);

    AZ= 0.016241*Z - 0.000479;
    AZP= -0.360972*Z + 0.3209;
    BZ= 0.32102*Z  - 0.05996;
    BZP= 8.2198*Z - 1.557;

    ALPHA = AZ*(SL+(-5.0*ZL)) + AZP;
    BETA = BZ*(SL+(-5.0*ZL)) + BZP;

    DELS3 = ALPHA*(S2+(-5.0*(ZC+(2*ZL)))) + BETA;
    DELSR3= DELS1 + DELS2 + DELS3;

    S3 = DELSR3 + SC + (3.0*SL);
    Z3 = Z;

    DELHR3 = DELGR3 + (298.15*DELSR3);
    H3 = DELHR3 + HC + (3.0*HL);

    /* CHANGED 17 MARCH 1992!!! */

    GZ = 0.89*CPC + 0.72*CPL + 16.3;
    DELCP3 = DELCP2 + GZ;
    CP3 = DELCP3 + CP2 + CPL;
    Z3 = Z;

    DELVR3  = 0.11419*V2 + 8.9432;
    V3 = DELVR3 + DELVR2 + DELVR1 + VC + (3.0*VL);
    Z3 = Z;

    /*
     END OF PREDICTORS FOR THE THIRD COMPLEX
    */

    /* fourth complex */

    Z = Z + ZL;
    DELGR4 = lg_to_ln * 1.98719 * 298.15 * BETA4;
    G4 = DELGR4 + GC + (4.0*GL);

    AZ= 0.016241*Z - 0.000479;
    AZP= -0.360972*Z + 0.3209;
    BZ= 0.32102*Z  - 0.05996;
    BZP= 8.2198*Z - 1.557;

    ALPHA = AZ*(SL+(-5.0*ZL)) + AZP;
    BETA = BZ*(SL+(-5.0*ZL)) + BZP;

    DELS4 = ALPHA*(S3+(-5.0*(ZC+(3*ZL)))) + BETA;
    DELSR4= DELS1 + DELS2 + DELS3 + DELS4;

    S4 = DELSR4 + SC + (4.0*SL);
    Z4 = Z;

    DELHR4 = DELGR4 + (298.15*DELSR4);
    H4 = DELHR4 + HC + (4.0*HL);

    /* CHANGED 17 MARCH 1992!!! */

    GZ = 0.89*CPC + 0.72*CPL + 16.3;
    DELCP4 = DELCP3 + GZ;
    CP4 = DELCP4 + CP3 + CPL;
    Z4 = Z;

    DELVR4  = 0.11419*V3 + 8.9432;
    V4 = DELVR4 + DELVR3 + DELVR2 + DELVR1 + VC + (4.0*VL);
    Z4 = Z;
    /*
    	The following section has been altered so that
    	there will be output only from where BETA != 0
    */
    if( b1 )
    {
        Z = Z1;
//        G = G1;
        H = H1;
        S = S1;
        CP = CP1;
        V = V1;
    }
    if( b2 )
    {
        Z = Z2;
//        G = G2;
        H = H2;
        S = S2;
        CP = CP2;
        V = V2;
    }
    if( b3 )
    {
        Z = Z3;
//        G = G3;
        H = H3;
        S = S3;
        CP = CP3;
        V = V3;
    }
    if( b4 )
    {
        Z = Z4;
//        G = G4;
        H = H4;
        S = S4;
        CP = CP4;
        V = V4;
    }

    /* Putting the results into REACDC fields */
    rcp->Hs[0] += rcp->Hs[1] - H * cal_to_J;
    rcp->Ss[0] += rcp->Ss[1] - S * cal_to_J;
    rcp->Cps[0] += rcp->Cps[1] - CP * cal_to_J;
    rcp->Vs[0]  += rcp->Vs[1] - V / 10.0;
    rcp->Hs[1] = H * cal_to_J;
    rcp->Ss[1] = S * cal_to_J;
    rcp->Cps[1] = CP * cal_to_J;
    rcp->Vs[1] = V/10.0;

    aMod[RT_REACDC].ModUpdate("PRONSPREP correlations done");

    if( !vfQuestion( window(), "DCOMP", "Would you like to create/modify DCOMP record?" ))
        return;
    /* Trying to read resulting DCOMP */

    strncpy( dcn, key, MAXRKEYLEN );
    TDComp* aDC=(TDComp *)(&aMod[RT_DCOMP]);
    aDC->ods_link(0);
    Rfind = rt[RT_DCOMP].Find( dcn );
    if(Rfind <0 )
    { /* There is no such record - copying data */
        aDC->dcp->Zz = rcp->Zz;
        aDC->dcp->mwt = rcp->mwt;
        aDC->dcp->Pst = rcp->Pst;
        aDC->dcp->TCst = rcp->TCst;
        aDC->dcp->Comp = rcp->Comp;
        aDC->dcp->Expa = rcp->Expa;
        aDC->dcp->Der = rcp->Der;
        aDC->dcp->DerB = rcp->DerB;
        aDC->dcp->PdcC = rcp->PreC;
        aDC->dcp->pct[2] = rcp->pct[2];
        memcpy( aDC->dcp->form, rcp->form, MAXFORMULA );
        memcpy( aDC->dcp->name, rcp->name, MAXFORMULA );
        memcpy( aDC->dcp->rmtm, rcp->rmtm, MAXRMTM );
    }
    else
    {  /* The record is found */
        rt[RT_DCOMP].Get(Rfind);
        aDC->dyn_set();
    }
    aDC->dcp->Gs[0] = rcp->Gs[1];
    aDC->dcp->Hs[0] = rcp->Hs[1];
    aDC->dcp->Ss[0] = rcp->Ss[1];
    aDC->dcp->Cps[0] = rcp->Cps[1];
    aDC->dcp->mVs[0] = rcp->Vs[1];
    aDC->dcp->Gs[1] = rcp->Gs[2];
    aDC->dcp->Hs[1] = rcp->Hs[2];
    aDC->dcp->Ss[1] = rcp->Ss[2];
    /* Creating/saving DCOMP record */
    if( Rfind>=0 )
    {
        if( !vfQuestion( window(), "DCOMP",
                         "DCOMP record already exist! Modify it (Y) or skip (N)?" ))
            return;
        //Rnum = rt[RT_DCOMP].Find(dcn);
        rt[RT_DCOMP].Rep(Rfind/*Rnum*/);
    }
    else TDComp::pm->AddRecord(dcn);
    /* A reminder */
    vfQuestion( window(), "DCOMP", "PP97: DCOMP record is ready. "
                "Don't forget re-calculate it! ");
}



// Help on ReacDC module ( ? button )
void
TReacDC::CmHelp()
{
    pVisor->OpenHelp( GEMS_RE_HTML );  //  05.01.01
}

#include "m_sdata.h"
/*--------------------------------------------------------*/
//Print record to table
void
TReacDC::RecordPrint( const char *key )
{
// select  SDref key
 gstring sd_key;
  if( key )
  sd_key=key;
 else
 {
   if( rcp->sdref )
     for( int ii=0; ii<rcp->Nsd; ii++ )
       if( gstring( rcp->sdref[ii], 0, V_SD_RKLEN ).find( "pscript" ) != gstring::npos)
       { sd_key = gstring( rcp->sdref[ii]);
         break;
       }
   if( sd_key.empty() )
       sd_key = "pscript:0000:reacdc:";
 }
 // read sdref record with format prn
 TSData::pm->RecInput( sd_key.c_str() );
 char * text_fmt = TSData::pm->getAbstr();
 if( !text_fmt )
   Error( sd_key.c_str(), "No format text in this record.");
 PrintSDref( sd_key.c_str(), text_fmt );

}


// Test record with key
void
TReacDC::TryRecInp( const char *key_, time_t& time_s, int q )
{

    TCStringArray aDclist;
    TCIntArray anRDc;

    MessageToSave();

    vstr key(db->KeyLen(), key_);
    TDBKey dbKey(db->GetDBKey());
    dbKey.SetKey(key);
    dbKey.SetFldKey(3,"*");
    gstring str_key( dbKey.UnpackKey(), 0, db->KeyLen() );
    RecStatus iRet = db->Rtest( str_key.c_str(), 1 );
    gstring msg;

    switch( iRet )
    {
    case MANY_:    // Get Key list
        db->GetKeyList( str_key.c_str(), aDclist, anRDc );
        db->Get(anRDc[0]);
    case ONEF_:
        dyn_set(q);
        time_s = db->Rtime();
        return;
    case UNDF_:
    case NONE_:
    case EMPC_:
        {
            msg = "PDB chain  ";
            msg +=  GetName();
            msg += ": Data record not found, \n"
                   " key  '";
            msg += gstring( key, 0, db->KeyLen() );
            msg += "'.\n Maybe, a PDB file is not linked to chain.\n";
            if(pVisor->ProfileMode == true)
                Error( GetName(), msg.c_str() );
            msg +=  "Create new record?";
            if( !vfQuestion(window(), GetName(), msg ))
                Error( GetName(), " New record create action dismissed...");
            gstring str = key.p;

            if( str.find_first_of("*?" ) != gstring::npos)  // pattern
                str = GetKeyofRecord( str.c_str(),
                                      "Insert new key of Data Record", KEY_NEW);
            if(  str.empty() )
                Error( GetName(), " Record create action dismissed...");
            int  Rnum = db->Find( str.c_str() );
            ErrorIf( Rnum>=0, GetName(), " Record alredy exist!");
            pVisor->OpenModule(window(), nRT);
            gstring str1 = db->UnpackKey();
            check_input( str1.c_str() );
            RecBuild( str.c_str() );
            SetString("Remake of new record finished OK. "
                      "It is recommended to re-calculate the data");
            pVisor->Update();
            Error("Calculation failed!", " Check data fields and try again!");
        } // break;
    case FAIL_:
        msg = "Failure!!! PDB chain ";
        msg += GetName();
        msg += " is corrupt,\n"
               "Data record key '";
        msg += gstring( key, 0, db->KeyLen() );
        msg += "'\n Try to unload or re-index this PDB chain.";
        Error( GetName(),  msg.c_str() );
    }
}

void TReacDC::CopyRecords( const char * prfName, TCIntArray& cnt,
                      elmWindowData el_data, rdSetupData st_data)
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
    uint j;
    TFormula aFo;

    for(uint ii=0; ii<aDCkey.GetCount(); ii++ )
    {
     if( !el_data.flags[cbAqueous_] &&
         ( *db->FldKey( 0 )== 'a' || *db->FldKey( 0 )== 'x' ))
       continue;
     if( !el_data.flags[cbGaseous_] && *db->FldKey( 0 )== 'g' )
       continue;
     if( !el_data.flags[cbSorption_] && *db->FldKey( 0 )== 'c' )
       continue;

     RecInput( aDCkey[ii].c_str() );
     //test record
       aFo.SetFormula( rcp->form ); // and ce_fscan
     for( i=0; i<aFo.GetIn(); i++ )
     {
       for( j=0; j<el_data.ICrds.GetCount(); j++ )
        if( !memcmp( el_data.ICrds[j].c_str(), aFo.GetCn(i), MAXICNAME ) )
          break;
       if( j == el_data.ICrds.GetCount() )
        break;
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
    // !!! changing record key
     gstring str= gstring(db->FldKey( 3 ), 0, db->FldLen( 3 ));
    ChangeforTempl( str, st_data.from_templ,
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

    // close all no profile files
    TCStringArray names1;
    names1.Add(prfName);
    db->OpenOnlyFromList(names1);
}

// ------------------ End of m_reacdc.cpp --------------------------

