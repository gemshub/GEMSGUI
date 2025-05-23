//------------------------------------------------------------------
// $Id: m_reacdc.cpp 1329 2009-07-01 14:20:16Z wagner $
//
// Implementation of TReacDC class, config and calculation functions
//
// Rewritten from C to C++ by S.Dmytriyeva
// Copyright (C) 1995-2009 S.Dmytriyeva, D.Kulik, T.Wagner
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

#include "m_reacdc.h"
#include "m_param.h"
#include "s_supcrt.h"
#include "s_formula.h"
#include "service.h"
#include "visor.h"
#include "filters_data.h"

TReacDC* TReacDC::pm;

TReacDC::TReacDC( uint nrt ):
        TCModule( nrt )
{
    nQ =8;

    aFldKeysHelp.push_back("Phase state code of new Dependent Component { a g f p s l m c i z y h }");
    aFldKeysHelp.push_back("ID of a group to which this new Dependent Component belongs");
    aFldKeysHelp.push_back("Name of this new reaction-defined Dependent Component (chemical species)");
    aFldKeysHelp.push_back("Thermodynamic data subset (TDS) code (e.g. database ID)");

    for(int i=1; i<nQ; i++)
    {
        rcp=&rc[i];
        set_def( i);
    }
    rcp=&rc[0];
    set_def();
    start_title = " Reaction-defined data format for Dependent Components (species)";
}

// link values to objects
void TReacDC::ods_link( int q)
{
    ErrorIf( q >= nQ, GetName(), "E00RErem: Invalid link (q>=8)!" );

    // static
    //aObj[ o_repst]->SetPtr( rc[q].pstate );
    //aObj[ o_reps]->SetPtr( rc[q].psymb );
    //aObj[ o_redcn]->SetPtr( rc[q].dcn );
    //aObj[ o_reatyp]->SetPtr( rc[q].atyp );
    aObj[ o_repct]->SetPtr(   rc[q].pct );
    aObj[ o_reprun]->SetPtr( &rc[q].PreC );    /*6*/
    aObj[ o_repra]->SetPtr(  &rc[q].PreKT );    /*8*/
    aObj[ o_rermtm]->SetPtr(  rc[q].rmtm );
    aObj[ o_rezz]->SetPtr(   &rc[q].Zz );
    aObj[ o_remwt]->SetPtr(  &rc[q].mwt );
    aObj[ o_reks]->SetPtr(    rc[q].Ks );   /*d 3*/
    aObj[ o_regs]->SetPtr(    rc[q].Gs );   /*d 3*/
    aObj[ o_rehs]->SetPtr(    rc[q].Hs );   /*d 3*/
    aObj[ o_ress]->SetPtr(    rc[q].Ss );   /*f 3*/
    aObj[ o_recps]->SetPtr(   rc[q].Cps );  /*f 3*/
    aObj[ o_remvs]->SetPtr(   rc[q].Vs );   /*f 3*/
    aObj[ o_renix]->SetPtr(   rc[q].Nix );  /*f 3*/
    aObj[ o_reptst]->SetPtr( &rc[q].Pst );  /*f 2*/
    aObj[ o_realbet]->SetPtr(&rc[q].Comp ); /*f 2*/
    aObj[ o_reder]->SetPtr(  &rc[q].Der ); /*f 2*/
    aObj[ o_redim]->SetPtr(  &rc[q].nDC );   /*8*/
    aObj[ o_refloat1]->SetPtr( &rc[q].Zz );  /*2*/
    aObj[ o_redbl1]->SetPtr(   rc[q].Ks );  /*9*/
    aObj[ o_refloat2]->SetPtr( rc[q].Ss );  /*18*/
    aObj[ o_restr]->SetPtr(   rc[q].pct );
    aObj[ o_rename]->SetPtr(  rc[q].name );
    aObj[ o_reform]->SetPtr(  rc[q].form );

    // dynamic
    // if( rc[q].nDC > 0 ) {
    aObj[ o_redck]->SetPtr( rc[q].DCk );
    aObj[ o_redck]->SetDim( rc[q].nDC, 1 );
    aObj[ o_rerdc]->SetPtr(  rc[q].rDC );
    aObj[ o_rerdc]->SetDim( rc[q].nDC, 1 );
    aObj[ o_rescdc]->SetPtr(  rc[q].scDC );
    aObj[ o_rescdc]->SetDim( rc[q].nDC, 1 );
    aObj[ o_repardc]->SetPtr(rc[q].ParDC);
    aObj[ o_repardc]->SetDim( rc[q].nDC, 6 );
    //}
    aObj[ o_redcp]->SetPtr( rc[q].DCp );
    aObj[o_redcp]->SetDim( MAXCPCOEF, 1 );
    aObj[ o_rehkfc]->SetPtr(rc[q].HKFc );
    aObj[o_rehkfc]->SetDim( MAXHKFCOEF, 1 );
    aObj[ o_redvt]->SetPtr( rc[q].DVt );
    aObj[ o_redvt]->SetDim( MAXVTCOEF, 1 );
    aObj[ o_redst]->SetPtr( rc[q].DSt );
    aObj[ o_redst]->SetDim( MAXCPCOEF, 1 );
    aObj[ o_repkt]->SetPtr( rc[q].pKt );
    aObj[ o_repkt]->SetDim( MAXCPCOEF, 1 );

    aObj[ o_retcint]->SetPtr( rc[q].TCint );
    aObj[ o_retcint]->SetDim(  max((short)2,rc[q].nTp), 1 );
    aObj[ o_repint]->SetPtr(  rc[q].Pint );
    aObj[ o_repint]->SetDim(  max((short)2,rc[q].nPp), 1 );
    //if( rc[q].PreKP == S_ON && rc[q].nTp && rc[q].nPp ){
    aObj[ o_relgK]->SetPtr(  rc[q].logK );
//    aObj[ o_relgK]->SetDim(  rc[q].nTp, rc[q].nPp ); // bug fix AY, DM 24.10.2016
    aObj[ o_relgK]->SetDim(   rc[q].nPp, rc[q].nTp );
    //}
    //aObj[ o_retprn]->SetPtr( rc[q].tprn  ); aObj[o_retprn]->SetDim(1, SPPTPRNBUFSIZE);
    //if( rc[q].Nsd > 0 ) {
    aObj[ o_resdref ]->SetPtr( rc[q].sdref );
    aObj[o_resdref]->SetDim(rc[q].Nsd, 1);
    aObj[ o_resdval ]->SetPtr( rc[q].sdval );
    aObj[o_resdval]->SetDim(rc[q].Nsd, 1);
    // }
    rcp=&rc[q];
}

// set dynamic Objects ptr to values
void TReacDC::dyn_set(int q)
{
    ErrorIf( rcp!=&rc[q], GetName(), "E01RErem: Invalid access to rc in dyn_set()");
    memcpy( rcp->pstate, rt[nRT]->UnpackKey(), RE_RKLEN );
    rc[q].DCk =   (char (*)[DC_RKLEN])aObj[ o_redck ]->GetPtr();
    rc[q].rDC =   (char *)aObj[ o_rerdc ]->GetPtr();
    rc[q].scDC =  (double *)aObj[ o_rescdc ]->GetPtr();
    rc[q].ParDC = (double (*)[6])aObj[ o_repardc ]->GetPtr();
    rc[q].TCint = (float *)aObj[ o_retcint ]->GetPtr();
    rc[q].Pint =  (float *)aObj[ o_repint ]->GetPtr();
    rc[q].DCp =   (float *)aObj[ o_redcp ]->GetPtr();
    rc[q].HKFc =  (float *)aObj[ o_rehkfc ]->GetPtr();
    rc[q].DVt =   (float *)aObj[ o_redvt ]->GetPtr();
    rc[q].DSt =   (float *)aObj[ o_redst ]->GetPtr();
    rc[q].pKt =   (float *)aObj[ o_repkt ]->GetPtr();
    rc[q].logK =  (float *)aObj[ o_relgK ]->GetPtr();
    rc[q].sdref = (char (*)[V_SD_RKLEN])aObj[ o_resdref ]->GetPtr();
    rc[q].sdval = (char (*)[V_SD_VALEN])aObj[ o_resdval ]->GetPtr();
    rc[q].tprn =  (char *)aObj[ o_retprn ]->GetPtr();
}

// free dynamic memory in objects and values
void TReacDC::dyn_kill(int q)
{
    ErrorIf( rcp!=&rc[q], GetName(), "E02RErem: Invalid access to rc in dyn_kill()");
    rc[q].DCk =   (char (*)[DC_RKLEN])aObj[ o_redck ]->Free();
    rc[q].rDC =   (char *)aObj[ o_rerdc ]->Free();
    rc[q].scDC =  (double *)aObj[ o_rescdc ]->Free();
    rc[q].ParDC = (double (*)[6])aObj[ o_repardc ]->Free();
    rc[q].TCint = (float *)aObj[ o_retcint ]->Free();
    rc[q].Pint =  (float *)aObj[ o_repint ]->Free();
    rc[q].DCp =   (float *)aObj[ o_redcp ]->Free();
    rc[q].HKFc =  (float *)aObj[ o_rehkfc ]->Free();
    rc[q].DVt =   (float *)aObj[ o_redvt ]->Free();
    rc[q].DSt =   (float *)aObj[ o_redst ]->Free();
    rc[q].pKt =   (float *)aObj[ o_repkt ]->Free();
    rc[q].logK =  (float *)aObj[ o_relgK ]->Free();
    rc[q].sdref = (char (*)[V_SD_RKLEN])aObj[ o_resdref ]->Free();
    rc[q].sdval = (char (*)[V_SD_VALEN])aObj[ o_resdval ]->Free();
    rc[q].tprn =  (char *)aObj[ o_retprn ]->Free();
}


// free dynamic memory in objects and values
void TReacDC::w_dyn_kill()
{
    rcp->ParDC = (double (*)[6])aObj[ o_repardc ]->Free();
}


// realloc dynamic memory
void TReacDC::dyn_new(int q)
{
    ErrorIf( rcp!=&rc[q], GetName(), "E03RErem: Invalid access to rc in dyn_new()");
    short nTp, nPp;
    ErrorIf( rc[q].nDC < 1, GetName(), "E04RErem: Number of DC in reaction is < 1" );

    rc[q].DCk =   (char (*)[DC_RKLEN])aObj[ o_redck ]->Alloc(rc[q].nDC, 1,DC_RKLEN);
    rc[q].rDC =   (char *)aObj[ o_rerdc ]->Alloc(rc[q].nDC, 1, A_);
    rc[q].scDC =  (double *)aObj[ o_rescdc ]->Alloc(rc[q].nDC, 1, D_);

    nTp = max( (short)2, rc[q].nTp );
    rc[q].TCint = (float *)aObj[ o_retcint ]->Alloc( nTp, 1, F_);
    nPp = max( (short)2, rc[q].nPp );
    rc[q].Pint =  (float *)aObj[ o_repint  ]->Alloc(nPp, 1, F_);

    if( rc[q].PreKP != S_OFF && rc[q].nTp >0 && rc[q].nPp>0 )
//        rc[q].logK =  (float *)aObj[ o_relgK ]->Alloc( rc[q].nTp, rc[q].nPp, F_); // bug fix AY, DM 24.10.2016
        rc[q].logK =  (float *)aObj[ o_relgK ]->Alloc( rc[q].nPp, rc[q].nTp, F_);
    else
        rc[q].logK = (float *)aObj[ o_relgK ]->Free();


    if( rc[q].PreDC == S_OFF )
        rc[q].DCp =   (float *)aObj[ o_redcp ]->Free();
    else
        rc[q].DCp =   (float *)aObj[ o_redcp ]->Alloc( MAXCPCOEF, 1, F_);

    if( rc[q].PrAki == S_OFF )  /* HKF - �model */
        rc[q].HKFc =  (float *)aObj[ o_rehkfc ]->Free();
    else
        rc[q].HKFc =  (float *)aObj[ o_rehkfc ]->Alloc( MAXHKFCOEF, 1, F_);

    if( rc[q].PreDV == S_OFF )  /* equations dV(T,P) */
        rc[q].DVt =   (float *)aObj[ o_redvt ]->Free();
    else
        rc[q].DVt =   (float *)aObj[ o_redvt ]->Alloc( MAXVTCOEF, 1, F_);

    if( rc[q].PreDS == S_OFF ) /* array of coeff. S(T) */
        rc[q].DSt =   (float *)aObj[ o_redst ]->Free();
    else
        rc[q].DSt =   (float *)aObj[ o_redst ]->Alloc( MAXCPCOEF, 1, F_);

    if( rc[q].PreKT == S_OFF )  /* array of coef. regres. lgK(T) */
        rc[q].pKt =   (float *)aObj[ o_repkt ]->Free();
    else
        rc[q].pKt =   (float *)aObj[ o_repkt ]->Alloc( MAXCPCOEF, 1, F_);

    if( rc[q].Nsd > 0 )
    {
        rc[q].sdref = (char (*)[V_SD_RKLEN])aObj[ o_resdref ]->Alloc(rc[q].Nsd,
                      1, V_SD_RKLEN);
        rc[q].sdval = (char (*)[V_SD_VALEN])aObj[ o_resdval ]->Alloc(rc[q].Nsd,
                      1, V_SD_VALEN);
    }
    else
    {
        rc[q].sdref = (char (*)[V_SD_RKLEN])aObj[ o_resdref ]->Free();
        rc[q].sdval = (char (*)[V_SD_VALEN])aObj[ o_resdval ]->Free();
    }
    // if( rc[q].tprn == 0 )
    //   rc[q].tprn = (char *)aObj[o_retprn]->Alloc( 1, 256, S_ );
}


void TReacDC::w_dyn_new()
{
    rcp->ParDC = (double (*)[6])aObj[ o_repardc ]->Alloc( rcp->nDC, 6, D_);
}

//set default information
void TReacDC::set_def( int q)
{
    ErrorIf( rcp!=&rc[q], GetName(), "E05RErem: Invalid access to rc in set_def()");
    TProfil *aPa= dynamic_cast<TProfil *>( aMod[RT_PARAM].get());

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
    time_t tre, tc=0L;
    int q, i;
    bool iRet = false;
    if( Level != 1 )
        return true;
    tre = rt[nRT]->Rtime();
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
//            else tc = 0L;
            if( tre <= tc )
                iRet = true;
        }
    }
    catch( TError& xcpt )
    {
        ods_link(q);
        rt[nRT]->SetKey(key);
        Error( key/*xcpt.title.c_str()*/, xcpt.mess );
    }
    ods_link(q);
    rt[nRT]->SetKey(key); //   RecInput( key );
    return iRet;
}

// Input nesessary data and link data objects
void TReacDC::RecInput( const char *key )
{
    TCModule::RecInput( key );
    //  memcpy( rcp->pstate, rt[nRT].UnpackKey(), RE_RKLEN );

}

static TCStringArray aDclist;

// opens window with 'Remake record' parameters
void
TReacDC::MakeQuery()
{
//    pImp->MakeQuery();
    const char * p_key;
    char flgs[12];
    int size[4];

    p_key  = db->PackKey();
    memcpy( flgs, rcp->pct, 12);
    size[0] = rcp->nDC;
    size[1] = rcp->nTp;
    size[2] = rcp->nPp;
    size[3] = rcp->Nsd;

    if( !vfReacDCSet( window(), p_key, flgs, size, aDclist ))
         Error( p_key, "ReacDC record configuration cancelled by the user!" );
     //  return;   // cancel

    memcpy( rcp->pct, flgs, 12);
    rcp->nDC = (short)size[0];
    rcp->nTp = (short)size[1];
    rcp->nPp = (short)size[2];
    rcp->Nsd = (short)size[3];

}

//Rebuild record structure before calculation

int TReacDC::RecBuild( const char *key, int mode  )
{
    int  Nc1=0, Nn1=0, Nf1=0, Nr1=0;
    char pkey[81];
    int CM,CE,CV;

    //TCStringArray aDclist;
    aDclist.clear();
    //REACDC&DCOMP  keypart
    rt[RT_REACDC]->MakeKey( RT_REACDC, pkey, K_ANY, K_ANY, K_ANY, K_ANY, K_END);

    if( mode != VF3_3 ) { // not clear all mode
        if( rcp->rDC )
        { // calc count DC and RC
            for(int i=0; i<rcp->nDC; i++ )
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
                    Error( GetName(), "E08RErem: Invalid reaction species code!");
                }
            }
        }

        // made vectors selections DCOMP and REACDC
        if( Nc1>0 || Nr1>0 )
        {
            /* Build old selections DCOMP and REACDC */
            aDclist.clear();
            std::string key_dr;

            for( int i=0; i<rcp->nDC; i++ )
            {
                if( rcp->rDC[i] == SRC_DCOMP || rcp->rDC[i] == SRC_REACDC )
                {
                    key_dr  = std::string(1, rcp->rDC[i]);
                    key_dr += ' ';
                    key_dr += char_array_to_string( rcp->DCk[i], DC_RKLEN/*-MAXSYMB*/ );
                    aDclist.push_back( key_dr.c_str() );
                }
            }
        }
    }
AGAIN_MOD:
    int ret = TCModule::RecBuild( key, mode );
    memcpy( rcp->pstate, key/*rt[nRT].UnpackKey()*/, RE_RKLEN );
    if( ret == VF3_3 && !( !rcp->PreC || rcp->PreC == ' ' ) )
        return ret;
    if( ret == VF3_1 )
    {
        strncpy( rcp->rmtm, curDateSmol().c_str(), 9);
        strncpy( rcp->name, db->FldKey(2), db->FldLen(2));
        rcp->name[db->FldLen(2)] = '\0';
        Nn1++;
    }
    if( rcp->nTp < 0 || rcp->nPp < 0 || rcp->Nsd < 0 || rcp->Nsd > 4 )
    {    if( vfQuestion( window(), GetName(),
           "W06RErem: Some counters (nTp, nCp, Nsd) are invalid! Remake again?" ))
            goto AGAIN_MOD;
        else
            Error( GetName(), "E07RErem: Invalid counters (nTp, nCp, Nsd) - bailing out!");
    }
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
        break;
    case CTPM_REA:
        switch(CE)
        {
        case CTM_LGK:
        case CTM_LGX:
            rcp->PreDC = S_ON;
             [[fallthrough]];
        case CTM_EK0:
        case CTM_EK1:
        case CTM_EK3:
        case CTM_EK2:
            rcp->PreKT = S_ON;
            break;
        case CTM_IKZ:
            rcp->PreKP = S_ON;
            break; /* calc lgK(TP) */
        case CTM_DKR:   // Franck-Marshall density model
        case CTM_MRB:   // Modified Ryzhenko-Bryzgalin model
            rcp->PreDS = S_ON;
             break;
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

    /**
AGAINRC:

   aDclist = vfRDMultiKeysSet( window(),
       "Please, mark ReacDC/DComp keys to be included",
        pkey, aDclist_old );


    if(  aDclist.size() < 1 )
    {
       switch ( vfQuestion3(window(), GetName(),
            "W09RErem: Number of selected ReacDC/DComp keys < 1.\n"
            " Mark again, proceed without ReacDC/DComp or Cancel?",
            "&Repeat", "&Proceed"))
       {
         case VF3_1:
                goto AGAINRC;
                ;
         case VF3_2:
                break;
         case VF3_3:  Error( GetName(),
                "E10RErem: No ReacDC/DComp records selected - bailing out!");
       }
    }
**/
    /*================================*/
    rcp->nDC = ( aDclist.size()+Nn1+Nf1);
    dyn_new();

    /*================================*/
    // get aMcv+aMrv  (Remake code! Must be by component groups )
    for( int i=0; i<rcp->nDC; i++ )
    {
        if( approximatelyZero(rcp->scDC[i]) )
            rcp->scDC[i] = 1;
        if( i < static_cast<int>(aDclist.size()) )
        {
            memcpy( rcp->DCk[i], aDclist[i].c_str()+2, DC_RKLEN );
            rcp->rDC[i] = aDclist[i].c_str()[0];
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
    rt[nRT]->SetKey(key); //   RecInput( key );
    return ret;
}

//Calculate record
void
TReacDC::RecCalc( const char* key )
{
    int i,q;
//    RecStatus oldStatus;

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
            db->SetKey(key);
        }
        w_dyn_kill();
        Error(  key , xcpt.mess );
    }
    TCModule::RecCalc(key);
    db->SetStatus(ONEF_);   //  Experimental - trying to bugfix! 13.01.05
}

// Calculation of thermodynamic properties of reaction and reaction-defined
//    DC for T,P of interest
//  Input data are loaded into tpwork structure, results extracted from
//    that structure. RCthermo() is recursive (up to 6 levels).
//
void
TReacDC::RCthermo( int q, int p )
{
	int CM,CE,CV, j, isotop = 0;
    double rho, eps, alp, dal, bet, xborn, yborn, /*zborn,*/ qborn;
    time_t tim;
    char  /*buf[121],*/ dckey[DC_RKLEN + 10];

    aW.twp->Tst = aW.twp->TCst + C_to_K;
    aW.twp->RT = R_CONSTANT * aW.twp->T; /* !!!! */
	/* aW.twp->ln10RT ??? */
	// test method codes for thermodynamic calculations
    CM = toupper( rc[q].pct[0] );
    CE = toupper( rc[q].pct[1] );
    CV = toupper( rc[q].pct[2] );

    // if( CE != CTM_MRB )  // Provisional for MRB model - DK on 06.08.07
    if (CE != CTM_IKZ)  // Provisional for Lagrange logK interpolation; to calculate logK if T, P are close to std - DM 17-02-2025
    if( fabs( aW.twp->T - 298.15 ) < 0.01 && fabs(aW.twp->P-1.) < 0.01 )
    {   // standard conditions (Tr,Pr)
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
    TDComp* aDC= dynamic_cast<TDComp *>( aMod[RT_DCOMP].get());
    aDC->ods_link(0);

    /*  memcpy( dckey, rc[q].pstate, DC_RKLEN ); */
    //  dckey[DC_RKLEN] = 0;

    if( CM == CTPM_HKF || CE == CTM_MRB || CE == CTM_DKR )
    {
        // calculate water properties from SUPCRT subroutines, if necessary
        if( fabs(aW.twp->TC - aSta.Temp) > 0.01 ||
               /* ( aW.twp->P > 1e-4 && */ fabs( aW.twp->P - aSta.Pres ) > 0.001 ) // )
        {  // calculate water properties from HGK EoS
			aSta.Temp = aW.twp->TC;
			aSta.Pres = aW.twp->P;
			TSupcrt supCrt;
			supCrt.Supcrt_H2O( aSta.Temp, &aSta.Pres);
			aW.twp->P = aSta.Pres;
        }

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
        aW.twp->P = aSta.Pres;  // pressure
        aW.twp->wRo  = aSta.Dens[aSpc.isat];  // density (g cm-3)
        aW.twp->wEps = aWp.Dielw[aSpc.isat];  // dielectric constant
        aW.twp->wAlp  = aWp.Alphaw[aSpc.isat];  // expansibility
        aW.twp->wdAlpdT = aWp.dAldT[aSpc.isat];  // T derivative
        aW.twp->wBet  = aWp.Betaw[aSpc.isat];  // compressibility
        // aW.twp->wVis = aWp.Viscw[aSpc.isat];  // dynamic viscosity
        aW.twp->wdRdT = - alp * rho;
        aW.twp->wd2RdT2 = rho * ( pow(alp,2.) - dal );
        aW.twp->wdRdP = bet * rho;
        aW.twp->wdEdT = yborn * pow(eps,2.);
        aW.twp->wd2EdT2 = (xborn + 2.*eps*pow(yborn,2.)) * pow(eps,2.);
        aW.twp->wdEdP = qborn * pow(eps,2.);
    }

    w_dyn_new();

    //aSpc.on_sat_curve = false; // 01.06.2016 - needs testing

    // test the component of reaction and calculate its t/d properties
    for( j=0; j<rc[q].nDC; j++ )
    {
        strncpy( dckey, rc[q].DCk[j], DC_RKLEN );  // Override off !!!
        // !!!!!!!! except "any"=* field in data base record
        aW.ods_link( p+1 );
        /* clear new TPwork structure */
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
        aW.twp->wAlp  = aW.WW(p).wAlp;
        aW.twp->wdAlpdT = aW.WW(p).wdAlpdT;
        aW.twp->wBet  = aW.WW(p).wBet;
        // aW.twp->wVis = aW.WW(p).wVis;
        aW.twp->wdRdT = aW.WW(p).wdRdT;
        aW.twp->wd2RdT2 = aW.WW(p).wd2RdT2;
        aW.twp->wdRdP = aW.WW(p).wdRdP;
        aW.twp->wdEdT = aW.WW(p).wdEdT;
        aW.twp->wd2EdT2 = aW.WW(p).wd2EdT2;
        aW.twp->wdEdP = aW.WW(p).wdEdP;
        aW.twp->unE = aW.WW(p).unE;  /* test units of measurement */
        aW.twp->unV = aW.WW(p).unV;
        memcpy( aW.twp->DRkey, dckey, DC_RKLEN );

        switch( rc[q].rDC[j] )
        {
        case SRC_REACDC: /* recursively load another ReacDC record */
            ods_link( q+1 );
            TryRecInp( dckey, tim, q+1 );
            aW.twp->pSD = SRC_REACDC;
            aW.twp->pTM = S_OFF;
            aW.twp->Pst = rc[q+1].Pst;
            aW.twp->TCst = rc[q+1].TCst;
            aW.twp->T = aW.twp->TC + C_to_K;
            // Recursive call of RCthermo()!
            RCthermo( q+1, p+1 );
            break;
        case SRC_FICT:   /* fictive component */
            break;
        case SRC_DCOMP: /* thermochemical component */
            aDC->ods_link( 0 );   /* (q+1) ??? */
            aDC->TryRecInp( dckey, tim, 0 );
            aW.twp->pSD = SRC_DCOMP;
            aW.twp->pTM = S_ON;
            aW.twp->Pst = aDC->dcp->Pst;
            aW.twp->TCst = aDC->dcp->TCst;
            aW.twp->T = aW.twp->TC + C_to_K;
            aDC->DCthermo( 0, p+1 ); // calculate properties at T,P
            aW.WW(p).P = aW.twp->P;  // !!!!!!return P on Psat curve
            break;
        case SRC_NEWDC: /* new component */
            aW.twp->pSD = SRC_NEWDC;
            aW.twp->pTM = S_REM;
            isotop = 0;
            goto CALCULATE_DELTA_R;
        case SRC_NEWISO: /* new isotopic form?*/
            aW.twp->pSD = SRC_NEWISO;
            aW.twp->pTM = S_REM;
            isotop = 1;
            goto CALCULATE_DELTA_R;
        default:
            Error( GetName(),"E01RErun: Invalid DC code in RCthermo()");
        }

        /* Unload work structure to REACDC */
        rc[q].ParDC[j][_Gs_] = aW.twp->G;
        rc[q].ParDC[j][_Hs_] = aW.twp->H;
        rc[q].ParDC[j][_Ss_] = aW.twp->S;
        rc[q].ParDC[j][_Cps_] = aW.twp->Cp;
        rc[q].ParDC[j][_Vs_] = aW.twp->V;

    } /* j */
    /* Error? */  aW.WW(p).pTM = S_REM;

CALCULATE_DELTA_R:
    aW.ods_link( p );
    if( rcp != rc+q )
        ods_link( q );
    if( rc[q].rDC[rc[q].nDC-1] == SRC_DCOMP ||
            rc[q].rDC[rc[q].nDC-1] == SRC_REACDC )
    { /*Calc parameters of reaction without a new component */
        Calc_rDCD( q, p );
        return;
    }

    /* calc delta for the reaction at TP */
    aW.WW(p).pTM = S_REM;
    switch( CM )
    {
    case CTPM_HKF:   // Not used in reacDC presently !
        switch( CE )
        {
        default:
            Error(dckey, "E12RErun: Invalid CE method flag!");
            break;
        case CTM_HKF:
            calc_tphkf_r( q, p );
            break; /* calc aqueous species */
        }
        break;
    case CTPM_CPT:
    case CTPM_REA: /* calculate reaction properties at T,P */
        switch( CE )
        {
        case CTM_LGX:
        case CTM_LGK:
        case CTM_EK0:
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
        case CTM_DKR: // Marshall-Franck density model
        	calc_r_FMD( q, p, CE, CV );
            break;
        case CTM_MRB: // Calling modified Ryzhenko-Bryzgalin model TW KD 08.2007
             calc_r_MRB( q, p, CE, CV );
            break;
        default:
            Error(dckey,"E13RErem: Invalid CE method flag!");
        }
        break;
    case CTPM_ISO:
        calc_iso_a( q, p ); // isotopic forms
        break;
    case CTPM_SOR: /* sorption - not used in this version */
        calc_exion_r( q, p );
        break;
    default:
        {  /* Invalid code method of calculation?*/
            std::string msg = "W14RErun: Invalid CM method flag!";
            msg += dckey;
            msg += "'.\n Change the record?";
            //if( !vfQuestion( GetName(), buf );
            Error( dckey, "E14RErun: Invalid CM method flag in ReacDC!");
            //  else  RecBuild( dckey );  // !!!!!! Recalc new record?
        }
     }
    /* Volume */
    switch ( CV )
    {
    case CPM_CON:
    case CPM_VKE:
    case CPM_VBE:
    case CPM_NUL:   // Added by KD on 15.07.03
    // case CPM_VBM:
    // case CPM_CEH: // constant volume only in this version!
         if( (CE != CTM_MRB) && (CE != CTM_DKR) )  // if not Ryzhenko-Bryzgalin model (provisional)
        	 calc_tpcv_r( q, p, CM, CV );
    default:
        break;
    }

    /* Calc t/d properties of new component */
    if( !isotop )
    { /* this is a usual reaction  */
        double G=0.0, S=0.0, H=0.0, Cp=0.0, V=0.0;
        for( j=0; j<rc[q].nDC-1; j++ )
        {
            G  += rc[q].scDC[j]*rc[q].ParDC[j][_Gs_];
            S  += rc[q].scDC[j]*rc[q].ParDC[j][_Ss_];
            H  += rc[q].scDC[j]*rc[q].ParDC[j][_Hs_];
            Cp += rc[q].scDC[j]*rc[q].ParDC[j][_Cps_];
            V  += rc[q].scDC[j]*rc[q].ParDC[j][_Vs_];
        }
        aW.WW(p).G =  (aW.WW(p).dG - G)/rc[q].scDC[rc[q].nDC-1];
        aW.WW(p).S =  (aW.WW(p).dS - S)/rc[q].scDC[rc[q].nDC-1] /* -foS */ ;
        aW.WW(p).H =  (aW.WW(p).dH - H)/rc[q].scDC[rc[q].nDC-1];
        aW.WW(p).Cp = (aW.WW(p).dCp - Cp)/rc[q].scDC[rc[q].nDC-1];
        aW.WW(p).V =  (aW.WW(p).dV - V)/rc[q].scDC[rc[q].nDC-1];
    }
    else
    { /*This is an isotopic form pseudo-reaction */
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
    double BETA1,BETA2,BETA3,BETA4,ZC,/*GC,*/HC,SC,CPC,VC,ZL,/*GL,*/HL,SL,CPL,VL,
    Z,/*G,*/H=0.,S=0.,CP=0.,V=0., /*Z1,G1,*/H1,S1,CP1,V1, /*Z2,G2,*/H2,S2,CP2,V2,
    /*Z3,G3,*/H3,S3,CP3,V3, /*Z4,G4,*/H4,S4,CP4,V4, DELGR1, DELGR2,
    DELGR3, DELGR4, AZ, AZP, BZ, BZP, ALPHA, BETA, DELS1=0.,
    DELS2, DELS3, DELS4, DELSR1, DELSR2, DELSR3, DELSR4,
    DELHR1, DELHR2, DELHR3, DELHR4, DELCP1, DELCP2, DELCP3, DELCP4,
    DELVR1, DELVR2, DELVR3, DELVR4, GZ, CSC, LSC, DZ, scC;
    int i, iL=0, iC=0, scL,  b1, b2, b3, b4;
    char dcn[MAXRKEYLEN+5];
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
//    GC = rcp->ParDC[iC][_Gs_]/cal_to_J;
    HC = rcp->ParDC[iC][_Hs_]/cal_to_J;
    SC = rcp->ParDC[iC][_Ss_]/cal_to_J;
    CPC = rcp->ParDC[iC][_Cps_]/cal_to_J;
    VC = rcp->ParDC[iC][_Vs_]*10.0;
    CSC = rcp->scDC[iC];
    scC = fabs(CSC);
    if ( scC > 1. )
        Error( GetName(), "E15RErun: |Cps| > 1 in PronsPrep for this reaction!");

    /* Loading data for ligand */
    ZL = rcp->ParDC[iL][_Zs_];
//    GL = rcp->ParDC[iL][_Gs_]/cal_to_J;
    HL = rcp->ParDC[iL][_Hs_]/cal_to_J;
    SL = rcp->ParDC[iL][_Ss_]/cal_to_J;
    CPL = rcp->ParDC[iL][_Cps_]/cal_to_J;
    VL = rcp->ParDC[iL][_Vs_]*10.0;
    LSC = rcp->scDC[iL];

    /* Loading Log_beta using st.coeff.at ligand */
    scL = static_cast<int>(fabs(LSC));
    BETA1 = 0;
    BETA2 = 0;
    BETA3 = 0;
    BETA4 = 0; //BETA = 0;
    b1 = b2 = b3 = b4 = 0;
    BETA = rcp->Ks[1];
    if( approximatelyZero(BETA) )
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
        Error( GetName(), "E16RErun: Invalid stoich.coefficients of DC in reaction!");
    }

    /*
         Calculations for the first complex
    */
    Z= ZC + ZL;
    DELGR1 = lg_to_ln * 1.98719 * 298.15 * BETA1;
//    G1 = DELGR1 + GC + GL;
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
//    Z1 = Z;

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
//    Z1 = Z;
    /*
     END OF PREDICTORS FOR THE FIRST COMPLEX
    */

    /* second complex */

    Z = Z + ZL;

    DELGR2 = lg_to_ln * 1.98719 * 298.15 * BETA2;
//    G2 = DELGR2 + GC + (2.0*GL);

    AZ= 0.016241*Z - 0.000479;
    AZP= -0.360972*Z + 0.3209;
    BZ= 0.32102*Z  - 0.05996;
    BZP= 8.2198*Z - 1.557;

    ALPHA = AZ*(SL+(-5.0*ZL)) + AZP;
    BETA = BZ*(SL+(-5.0*ZL)) + BZP;

    DELS2 = ALPHA*(S1+(-5.0*(ZC+ZL))) + BETA;
    DELSR2= DELS1 + DELS2;

    S2 = DELSR2 + SC + (2.0*SL);
//    Z2 = Z;

    DELHR2 = DELGR2 + (298.15*DELSR2);
    H2 = DELHR2 + HC + (2.0*HL);
    /*
     CHANGED 17 MARCH 1992!!!
    */
    GZ = 0.89*CPC + 0.72*CPL + 16.3;
    DELCP2 = DELCP1 + GZ;
    CP2 = DELCP2 + CP1 + CPL;
//    Z2 = Z;

    DELVR2  = 0.11419*V1 + 8.9432;
    V2 = DELVR2 + DELVR1 + VC + (2.0*VL);
//    Z2 = Z;
    /*
     END OF PREDICTORS FOR THE SECOND COMPLEX
    */
    /* third complex */

    Z = Z + ZL;

    DELGR3 = lg_to_ln * 1.98719 * 298.15 * BETA3;
//    G3 = DELGR3 + GC + (3.0*GL);

    AZ= 0.016241*Z - 0.000479;
    AZP= -0.360972*Z + 0.3209;
    BZ= 0.32102*Z  - 0.05996;
    BZP= 8.2198*Z - 1.557;

    ALPHA = AZ*(SL+(-5.0*ZL)) + AZP;
    BETA = BZ*(SL+(-5.0*ZL)) + BZP;

    DELS3 = ALPHA*(S2+(-5.0*(ZC+(2*ZL)))) + BETA;
    DELSR3= DELS1 + DELS2 + DELS3;

    S3 = DELSR3 + SC + (3.0*SL);
//    Z3 = Z;

    DELHR3 = DELGR3 + (298.15*DELSR3);
    H3 = DELHR3 + HC + (3.0*HL);

    /* CHANGED 17 MARCH 1992!!! */

    GZ = 0.89*CPC + 0.72*CPL + 16.3;
    DELCP3 = DELCP2 + GZ;
    CP3 = DELCP3 + CP2 + CPL;
//    Z3 = Z;

    DELVR3  = 0.11419*V2 + 8.9432;
    V3 = DELVR3 + DELVR2 + DELVR1 + VC + (3.0*VL);
//    Z3 = Z;

    /*
     END OF PREDICTORS FOR THE THIRD COMPLEX
    */

    /* fourth complex */

    Z = Z + ZL;
    DELGR4 = lg_to_ln * 1.98719 * 298.15 * BETA4;
//    G4 = DELGR4 + GC + (4.0*GL);

    AZ= 0.016241*Z - 0.000479;
    AZP= -0.360972*Z + 0.3209;
    BZ= 0.32102*Z  - 0.05996;
    BZP= 8.2198*Z - 1.557;

    ALPHA = AZ*(SL+(-5.0*ZL)) + AZP;
    BETA = BZ*(SL+(-5.0*ZL)) + BZP;

    DELS4 = ALPHA*(S3+(-5.0*(ZC+(3*ZL)))) + BETA;
    DELSR4= DELS1 + DELS2 + DELS3 + DELS4;

    S4 = DELSR4 + SC + (4.0*SL);
//    Z4 = Z;

    DELHR4 = DELGR4 + (298.15*DELSR4);
    H4 = DELHR4 + HC + (4.0*HL);

    /* CHANGED 17 MARCH 1992!!! */

    GZ = 0.89*CPC + 0.72*CPL + 16.3;
    DELCP4 = DELCP3 + GZ;
    CP4 = DELCP4 + CP3 + CPL;
//    Z4 = Z;

    DELVR4  = 0.11419*V3 + 8.9432;
    V4 = DELVR4 + DELVR3 + DELVR2 + DELVR1 + VC + (4.0*VL);
//    Z4 = Z;
    /*
    	The following section has been altered so that
    	there will be output only from where BETA != 0
    */
    if( b1 )
    {
//        Z = Z1;
//        G = G1;
        H = H1;
        S = S1;
        CP = CP1;
        V = V1;
    }
    if( b2 )
    {
//        Z = Z2;
//        G = G2;
        H = H2;
        S = S2;
        CP = CP2;
        V = V2;
    }
    if( b3 )
    {
//        Z = Z3;
//        G = G3;
        H = H3;
        S = S3;
        CP = CP3;
        V = V3;
    }
    if( b4 )
    {
//       Z = Z4;
//        G = G4;
        H = H4;
        S = S4;
        CP = CP4;
        V = V4;
    }

    /* Putting the results into REACDC fields */
    rcp->Hs[0] += rcp->Hs[1] - H * cal_to_J;
    rcp->Ss[0] += rcp->Ss[1] - (float)(S * cal_to_J);
    rcp->Cps[0] += rcp->Cps[1] - (float)(CP * cal_to_J);
    rcp->Vs[0]  += rcp->Vs[1] - (float)(V / 10.0);
    rcp->Hs[1] = H * cal_to_J;
    rcp->Ss[1] = (float)(S * cal_to_J);
    rcp->Cps[1] = (float)(CP * cal_to_J);
    rcp->Vs[1] = (float)(V/10.0);

    aMod[RT_REACDC]->ModUpdate("PRONSPREP correlations (Step 1) done Ok!");

    if( !vfQuestion( window(), "DComp",
               "Would you like to create/modify a DComp record?" ))
        return;
    /* Trying to read resulting DCOMP */

    strncpy( dcn, key, MAXRKEYLEN );
    TDComp* aDC= dynamic_cast<TDComp *>( aMod[RT_DCOMP].get());
    aDC->ods_link(0);
    Rfind = rt[RT_DCOMP]->Find( dcn );
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
        rt[RT_DCOMP]->Get(Rfind);
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
        if( !vfQuestion( window(), "DComp",
             "The DComp record already exists! Modify it (Y) or skip (N)?" ))
            return;
        //Rnum = rt[RT_DCOMP].Find(dcn);
        rt[RT_DCOMP]->Rep(Rfind/*Rnum*/);
    }
    else TDComp::pm->AddRecord(dcn);
    /* A reminder */
    vfQuestion( window(), "DComp", "PRONSPREP Step 2: the DComp record is ready!"
                "Don't forget to re-calculate it! ");
}


/* --------------------------------------------------------------- */
/* This is an implementation of the hydroxide algorithm of:
	Shock, E.L., Sassani, D.C., Willis, M., Sverjensky, D.A. (1997):
	Inorganic species in geologic fluids: Correlations among standard
	molal thermodynamic properties of aqueous ions and hydroxide
	complexes. Geochim. Cosmochim. Acta, 61, 907-950.
	note: the calculated properties are those of hydroxide species
	with non-conventional stoichiometry, i.e. MOH, MO, HMO2, MO2
*/

void TReacDC::PronsPrepOH( const char *key, int /*nIC*/, short *lAN )
{
    double ZC,HC,SC,/*CPC,*/VC,/*ZL,*/HL,SL,/*CPL,*/VL,Z,H=0.,S=0.,CP=0.,V=0.,
    H1,S1,CP1,V1,H2,S2,CP2,V2,H3=0.,S3=0.,CP3=0.,V3=0.,H4=0.,S4=0.,CP4=0.,V4=0.,
    DELGR1,DELGR2,DELGR3,DELGR4,DELSR1,DELSR2,DELSR3,DELSR4,
    DELHR1,DELHR2,DELHR3,DELHR4,DELVR1,//DELVR2,DELVR3,DELVR4,
    CSC, LSC, /*DZ,*/ scC, LOGKR, Sw, Hw;//, Gw, Cpw, Vw;
    int i, iL=0, iC=0, scL, NC, ZZ;
    char dcn[MAXRKEYLEN+5];
    int Rfind;

    Sw = 69.923/cal_to_J;
    Hw = (-285881)/cal_to_J;
    //Gw = (-237183)/cal_to_J;
    //Cpw = 75.3605/cal_to_J;
    //Vw = 1.80684*10.;

    // Finding index of cation and anion
    for( i=0; i<2; i++ )
    {
        Z = rcp->ParDC[i][_Zs_];
        if( Z > 0.0 )
            iC = i;
        else if( Z < 0.0 )
            iL = i;
        else Error( GetName(), "Z=0 in PronsPrep");
    }
    // Loading data for cation
    ZC = rcp->ParDC[iC][_Zs_];
//    GC = rcp->ParDC[iC][_Gs_]/cal_to_J;
    HC = rcp->ParDC[iC][_Hs_]/cal_to_J;
    SC = rcp->ParDC[iC][_Ss_]/cal_to_J;
    //CPC = rcp->ParDC[iC][_Cps_]/cal_to_J;
    VC = rcp->ParDC[iC][_Vs_]*10.0;
    CSC = rcp->scDC[iC];
    scC = fabs(CSC);
    if ( scC > 1. )
        Error( GetName(), "E15RErun: |Cps| > 1 in PronsPrep for this reaction!");

    // Loading data for ligand
    //ZL = rcp->ParDC[iL][_Zs_];
//    GL = rcp->ParDC[iL][_Gs_]/cal_to_J;
    HL = rcp->ParDC[iL][_Hs_]/cal_to_J;
    SL = rcp->ParDC[iL][_Ss_]/cal_to_J;
    //CPL = rcp->ParDC[iL][_Cps_]/cal_to_J;
    VL = rcp->ParDC[iL][_Vs_]*10.0;
    LSC = rcp->scDC[iL];

    scL = static_cast<int>(fabs(LSC));
    LOGKR = rcp->Ks[1];
    if( approximatelyZero(LOGKR) )
        LOGKR = 1e-9;
    ZZ = static_cast<int>(fabs(ZC));

//    NC = 0;  // needs to pull atomic number of cation from IComp here
    NC = lAN[iC];

    // calculations for complex number 1
    switch ( ZZ )
    {
	case 1:
		S1 = 1.32*SC - 6.0;
		CP1 = (-1.14)*S1 + 9.0;
		DELVR1 = 0.11419*VC + 8.9432;
		V1 = DELVR1 + VC + (1.0*VL);
		DELSR1 = S1 - SC - (1.0*SL);
		DELGR1 = LOGKR * lg_to_ln * 1.98719 * 298.15;
		DELHR1 = DELGR1 + (298.15*DELSR1);
		H1 = DELHR1 + HC + (1.0*HL);
		break;
	case 2:
		S1 = 1.32*SC + 24.5;
		CP1 = (-1.14)*S1 + 9.0;
		if ( NC == 4 || NC == 12 || NC == 20 || NC == 38 || NC == 56 || NC == 88 )
			V1 = 0.16*S1 + 4.9;
		else
			V1 = 0.45*S1 - 12.0;
		DELSR1 = S1 - SC - (1.0*SL);
		DELGR1 = LOGKR * lg_to_ln * 1.98719 * 298.15;
		DELHR1 = DELGR1 + (298.15*DELSR1);
		H1 = DELHR1 + HC + (1.0*HL);
		break;
	case 3:
		if ( NC == 31 || NC == 49 || NC == 81 || NC == 83 )
			S1 = 1.32*SC + 37.0;
		else
			S1 = 1.32*SC + 62.0;
		CP1 = (-1.14)*S1 - 37.2;
		if ( NC == 21 || NC == 22 || NC == 23 || NC == 24 || NC == 25 || NC == 26
			|| NC == 27 || NC == 28 )
			V1 = 0.45*S1 - 12.0;
		else
			V1 = 0.16*S1 + 4.9;
		DELSR1 = S1 - SC - (1.0*SL);
		DELGR1 = LOGKR * lg_to_ln * 1.98719 * 298.15;
		DELHR1 = DELGR1 + (298.15*DELSR1);
		H1 = DELHR1 + HC + (1.0*HL);
		break;
	case 4:
		S1 = 1.32*SC + 74.0;
		CP1 = (-1.14)*S1 - 37.2;
		V1 = 0.16*S1 + 4.9;
		DELSR1 = S1 - SC - (1.0*SL);
		DELGR1 = LOGKR * lg_to_ln * 1.98719 * 298.15;
		DELHR1 = DELGR1 + (298.15*DELSR1);
		H1 = DELHR1 + HC + (1.0*HL);
		break;
	default:
		Error( GetName(), "E16RErun: Estimation of standard state properties "
				"is not possible for cations with charge > 4!");
		break;
	}


    // calculations for the complex number 2
    switch ( ZZ )
    {
	case 1:
		S2 = 1.42*SC - 11.0;
		CP2 = (-1.14)*S2 - 15.5;
		V2 = 0.45*S2 - 12.0;
		DELSR2 = S2 - SC - (2.0*SL) + Sw;
		DELGR2 = LOGKR * lg_to_ln * 1.98719 * 298.15;
		DELHR2 = DELGR2 + (298.15*DELSR2);
		H2 = DELHR2 + HC + (2.0*HL) - Hw;
		break;
	case 2:
		S2 = 1.42*SC + 20.5;
		CP2 = (-1.14)*S2 - 15.5;
		if ( NC == 4 || NC == 12 || NC == 20 || NC == 38 || NC == 56 || NC == 88 )
			V2 = 0.16*S2 + 4.9;
		else
			V2 = 0.45*S2 - 12.0;
		DELSR2 = S2 - SC - (2.0*SL) + Sw;
		DELGR2 = LOGKR * lg_to_ln * 1.98719 * 298.15;
		DELHR2 = DELGR2 + (298.15*DELSR2);
		H2 = DELHR2 + HC + (2.0*HL) - Hw;
		break;
	case 3:
		S2 = 1.42*SC + 83.0;
		CP2 = (-1.14)*S2 - 60.8;
		if ( NC == 21 || NC == 22 || NC == 23 || NC == 24 || NC == 25 || NC == 26
			|| NC == 27 || NC == 28 )
			V2 = 0.45*S2 - 12.0;
		else
			V2 = 0.16*S2 + 4.9;
		DELSR2 = S2 - SC - (2.0*SL) + Sw;
		DELGR2 = LOGKR * lg_to_ln * 1.98719 * 298.15;
		DELHR2 = DELGR2 + (298.15*DELSR2);
		H2 = DELHR2 + HC + (2.0*HL) - Hw;
		break;
	case 4:
		S2 = 1.42*SC + 108.0;
		CP2 = (-1.14)*S2 - 60.8;
		V2 = 0.16*S2 + 4.9;
		DELSR2 = S2 - SC - (2.0*SL) + Sw;
		DELGR2 = LOGKR * lg_to_ln * 1.98719 * 298.15;
		DELHR2 = DELGR2 + (298.15*DELSR2);
		H2 = DELHR2 + HC + (2.0*HL) - Hw;
		break;
	default:
		Error( GetName(), "E16RErun: Estimation of standard state properties "
				"is not possible for cations with charge > 4!" );
		break;
	}

    // calculations for complex number 3
    switch ( ZZ )
    {
	case 1:
		if( !(scL == 1 || scL == 2) )
		   Error( GetName(), "E16RErun: Estimation of standard state properties "
	   	   "is not possible for complexes of monovalent cations with ligand number > 2!");
		break;
	case 2:
		S3 = 1.52*SC + 15.5;
		CP3 = (-2.28)*S3 - 24.0;
		if ( NC == 4 || NC == 12 || NC == 20 || NC == 38 || NC == 56 || NC == 88 )
			V3 = 0.25*S3 + 11.7;
		else
			V3 = 0.54*S3 - 4.8;
		DELSR3 = S3 - SC - (3.0*SL) + Sw;
		DELGR3 = LOGKR * lg_to_ln * 1.98719 * 298.15;
		DELHR3 = DELGR3 + (298.15*DELSR3);
		H3 = DELHR3 + HC + (3.0*HL) - Hw;
		break;
	case 3:
		S3 = 1.52*SC + 123.0;
		CP3 = (-2.28)*S3 - 24.0;
		if ( NC == 21 || NC == 22 || NC == 23 || NC == 24 || NC == 25 || NC == 26
			|| NC == 27 || NC == 28 )
			V3 = 0.54*S3 - 4.8;
		else
			V3 = 0.25*S3 + 11.7;
		DELSR3 = S3 - SC - (3.0*SL) + Sw;
		DELGR3 = LOGKR * lg_to_ln * 1.98719 * 298.15;
		DELHR3 = DELGR3 + (298.15*DELSR3);
		H3 = DELHR3 + HC + (3.0*HL) - Hw;
		break;
	case 4:
		S3 = 1.52*SC + 140.0;
		CP3 = (-2.28)*S3 - 24.0;
		V3 = 0.25*S3 + 11.7;
		DELSR3 = S3 - SC - (3.0*SL) + Sw;
		DELGR3 = LOGKR * lg_to_ln * 1.98719 * 298.15;
		DELHR3 = DELGR3 + (298.15*DELSR3);
		H3 = DELHR3 + HC + (3.0*HL) - Hw;
		break;
	default:
		Error( GetName(), "E16RErun: Estimation of standard state properties "
				"is not possible for cations with charge > 4!");
		break;
	}


    // calculations for complex number 4
    switch ( ZZ )
    {
	case 1:
		if( !(scL == 1 || scL == 2) )
		   Error( GetName(), "E16RErun: Estimation of standard state properties "
			"is not possible for complexes of monovalent cations with ligand number > 2!");
		break;
	case 2:
		S4 = 1.62*SC + 11.0;
		CP4 = (-2.28)*S4 - 106.2;
		if ( NC == 4 || NC == 12 || NC == 20 || NC == 38 || NC == 56 || NC == 88 )
			V4 = 0.25*S4 + 11.7;
		else
			V4 = 0.54*S4 - 4.8;
		DELSR4 = S4 - SC - (4.0*SL) + (2.0*Sw);
		DELGR4 = LOGKR * lg_to_ln * 1.98719 * 298.15;
		DELHR4 = DELGR4 + (298.15*DELSR4);
		H4 = DELHR4 + HC + (4.0*HL) - (2.0*Hw);
		break;
	case 3:
		S4 = 1.62*SC + 118.0;
		CP4 = (-2.06)*S4 - 34.5;
		if ( NC == 21 || NC == 22 || NC == 23 || NC == 24 || NC == 25 || NC == 26
			|| NC == 27 || NC == 28 )
			V4 = 0.54*S4 - 4.8;
		else
			V4 = 0.25*S4 + 11.7;
		DELSR4 = S4 - SC - (4.0*SL) + (2.0*Sw);
		DELGR4 = LOGKR * lg_to_ln * 1.98719 * 298.15;
		DELHR4 = DELGR4 + (298.15*DELSR4);
		H4 = DELHR4 + HC + (4.0*HL) - (2.0*Hw);
		break;
	case 4:
		S4 = 1.62*SC + 135.0;
		CP4 = (-2.06)*S4 - 34.5;
		V4 = 0.25*S4 + 11.7;
		DELSR4 = S4 - SC - (4.0*SL) + (2.0*Sw);
		DELGR4 = LOGKR * lg_to_ln * 1.98719 * 298.15;
		DELHR4 = DELGR4 + (298.15*DELSR4);
		H4 = DELHR4 + HC + (4.0*HL) - (2.0*Hw);
		break;
	default:
		Error( GetName(), "E16RErun: Pronsprep-OH is not possible for cations "
				"with charge > 4!");
		break;
	}


    if( scL == 1 )
    {
        H = H1;
        S = S1;
        CP = CP1;
        V = V1;
    }
    if( scL == 2 )
    {
        H = H2;
        S = S2;
        CP = CP2;
        V = V2;
    }
    if( scL == 3 )
    {
        H = H3;
        S = S3;
        CP = CP3;
        V = V3;
    }
    if( scL == 4 )
    {
        H = H4;
        S = S4;
        CP = CP4;
        V = V4;
    }

    // Putting the results into REACDC fields
    rcp->Hs[0] += rcp->Hs[1] - H * cal_to_J;
    rcp->Ss[0] += rcp->Ss[1] - (float)(S * cal_to_J);
    rcp->Cps[0] += rcp->Cps[1] - (float)(CP * cal_to_J);
    rcp->Vs[0]  += rcp->Vs[1] - (float)(V / 10.0);
    rcp->Hs[1] = H * cal_to_J;
    rcp->Ss[1] = (float)(S * cal_to_J);
    rcp->Cps[1] = (float)(CP * cal_to_J);
    rcp->Vs[1] = (float)(V/10.0);

    aMod[RT_REACDC]->ModUpdate("PRONSPREP correlations (Step 1) done Ok!");

    if( !vfQuestion( window(), "DComp",
               "Would you like to create/modify a DComp record?" ))
        return;
    // Trying to read resulting DCOMP

    strncpy( dcn, key, MAXRKEYLEN );
    TDComp* aDC= dynamic_cast<TDComp *>( aMod[RT_DCOMP].get());
    aDC->ods_link(0);
    Rfind = rt[RT_DCOMP]->Find( dcn );
    if(Rfind <0 )
    { // There is no such record - copying data
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
    {  // The record is found
        rt[RT_DCOMP]->Get(Rfind);
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
    // Creating/saving DCOMP record
    if( Rfind>=0 )
    {
        if( !vfQuestion( window(), "DComp",
             "The DComp record already exists! Modify it (Y) or skip (N)?" ))
            return;
        //Rnum = rt[RT_DCOMP].Find(dcn);
        rt[RT_DCOMP]->Rep(Rfind/*Rnum*/);
    }
    else TDComp::pm->AddRecord(dcn);
    // A reminder
    vfQuestion( window(), "DComp", "PRONSPREP Step 2: the DComp record is ready!"
                "Don't forget to re-calculate it! ");
}




// Help on ReacDC module ( ? button )
const char* TReacDC::GetHtml()
{
   return GM_REACDC_HTML;
}


// Test record with key
void
TReacDC::TryRecInp( const char *key_, time_t& time_s, int q )
{

    TCStringArray aDclist;
    TCIntArray anRDc;

    if( ! MessageToSave() )
	return;

    TDBKey dbKey(db->GetDBKey());
    dbKey.SetKey(key_);
    dbKey.SetFldKey(3,"*");
    std::string str_key( dbKey.UnpackKey(), 0, db->KeyLen() );
    RecStatus iRet = db->Rtest( str_key.c_str(), 1 );
    std::string msg;

    switch( iRet )
    {
    case MANY_:    // Get Key list
        db->GetKeyList( str_key.c_str(), aDclist, anRDc );
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
            msg = "W17RErun: In database chain  ";
            msg +=  GetName();
            msg += ": Data record not found, \n"
                   " key  '";
            msg += char_array_to_string( key_, db->KeyLen() );
            msg += "'.\n Maybe, a database file is not linked to chain.\n";
            if( pVisor->ProfileMode )
                Error( GetName(), msg );
            msg +=  "Create a new record?";
            if( !vfQuestion(window(), GetName(), msg ))
                Error( GetName(), " E18RErun: New record create action dismissed...");
            std::string str = key_;

            if( str.find_first_of("*?" ) != std::string::npos)  // pattern
                str = GetKeyofRecord( str.c_str(),
                            "Enter a new data record key, please!", KEY_NEW);
            if(  str.empty() )
                Error( GetName(), " E19RErun: Record create action dismissed (no key)...");
            int  Rnum = db->Find( str.c_str() );
            ErrorIf( Rnum>=0, GetName(), " W20RErun: This record alredy exists!");
            pVisor->OpenModule(window(), nRT,0,true);
            std::string str1 = db->UnpackKey();
            check_input( str1.c_str() );
            RecBuild( str.c_str() );
            SetString("Remake of new record finished OK. "
                      " It is recommended to re-calculate the data");
            pVisor->Update();
            Error("E21RErun: Calculation failed!",
                    " Please, check data fields and try again!");
        }
        break;
    case FAIL_:
        msg = "E22RErun: Failure!!! Database chain ";
        msg += GetName();
        msg += " is corrupt,\n"
               "data record key '";
        msg += char_array_to_string( key_, db->KeyLen() );
        msg += "'\n Try to backup/restore or compress files in this database chain!";
        Error( GetName(),  msg );
    }
}

void TReacDC::CopyRecords( const char * prfName, TCIntArray& cnt,
 elmWindowData el_data, rdSetupData st_data, std::set<std::string>& SDlist)
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
     aFo.SetFormula( rcp->form ); // and ce_fscan
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

    // !!! changing record key
     std::string str= std::string(db->FldKey( 3 ), 0, db->FldLen( 3 ));
    ChangeforTempl( str, st_data.from_templ,
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
     //Point SaveRecord
        if( AddRecordTest( str.c_str(), fnum_ ))
        {
            aICkey_new.push_back( stt );  // 30/11/2006
            for(int isd=0; isd<rcp->Nsd; isd++)
            {
                std::string sdkey = char_array_to_string( rcp->sdref[isd],V_SD_RKLEN);
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

// ------------------ End of m_reacdc.cpp --------------------------

