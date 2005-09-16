//GTDEMO.cpp

#include "v_print.h"
const char * si_tab_3c = "\nTable %d:   %-32.32s  ( Lines %d, Points %d)";
const char * gd_userf  = "User-defined functions";

void
TGtDemo::RecordPrint( const char *key )
{
    if( pVisor->ProfileMode == true && gdp->PsPB != S_OFF  )
        if( vfQuestion(window(), GetName(),"Calculate statistics from sampled data?"))
        {
            probe_stat( key );
            return;
        }
    if( vfQuestion(window(), GetName(),
                   "Collect data to tables (Y)\n or write line by line (N)?"))
        _pr_tab( key );
    else
        _line_tab( key );
}
/*--------------------------------------------------------*/
gstring GdPackKey( char *keybuf, int len) //get pack key
{
    gstring s;
    int i=0;
    bool blank=false;

    while( i<len)
    {
        if( keybuf[i] != ' ' )
        {
            blank = false;
            s += keybuf[i];
        }
        if( keybuf[i] == ' ' )
            if( blank == false )
            {
                s+= ':';
                blank = true;
            }

        i++;
    }
    return s;
}

//Print record GTDEMO to table
void
TGtDemo::_pr_tab( const char *key )
{
    int i, j, N, M, k;
    size_t tblen;
    vstr pbuf(200);
    char (*lNames)[MAXGRNAME];
    double *x, *y;

    SetString("Please, wait until arrays will be sampled");
    pVisor->Update();
    /*Table heard */
    TPrintTable prTab( "Data Sampling, Tabulation and Graphic Demonstration",
                       GetName(), key );
    for( k=0; k<2; k++ )
    { /* cycle by table */
        if( k==0 )
        {
            N = gdp->dimXY[0];
            M = gdp->dimXY[1];
        }
        else
        {
            N = gdp->dimEF[0];
            M = gdp->dimEF[1];
        }
        if( !(M*N) )
            continue;
        if( k==0)
            sprintf( pbuf, si_tab_3c, k, gdp->xNames, M, N  );
        else  sprintf( pbuf, si_tab_3c, k, gd_userf, M,  N  );
        prTab.PutString( pbuf );
        switch( k )
        { /* set pointers to data */
        case 0:
            lNames = gdp->lNam0;
            x = gdp->x0;
            y = gdp->y0;
            break;
        case 1:
            lNames = gdp->lNamE;
            x = gdp->xE;
            y = gdp->yE;
            break;
        }
        /* Head of table */
        if( k ==0 )
        {
            prTab.PutTabHead( 4 );
            prTab.PutTabHead( 5 );
        }
        else prTab.PutTabHead( 1 );
        for( i=0; i<M-1; i++ )
            prTab.PutTabHead( 5 );
        prTab.PutTabHead( 6 );
        /* colum names */
        if( k==0 )
            prTab.PutObjName( "Data Set Key Name", 5 );
        prTab.PutObjName( gdp->xNames, 4 );
        for( i=0; i<M; i++ )
        {
            if( i == M-1)
                prTab.PutObjName( lNames[i], 3 );
            else if( !i )  prTab.PutObjName( lNames[i], 4 );
            else prTab.PutObjName( lNames[i], 2 );
        }
        // delta in table
        if( k == 0 )
        {
            prTab.PutTabDelta( 7 );
            prTab.PutTabDelta( 8 );
        }
        else prTab.PutTabDelta( 1 );
        for( i=0; i<M-1; i++ )
            prTab.PutTabDelta( 8 );
        prTab.PutTabDelta( 9 );

        for( j=0; j<N; j++ )
        { /* put data gstring */
            if( k==0 )
            {
                gstring str = GdPackKey(gdp->rkey+j*gdp->rtLen, gdp->rtLen); //get pack key
                prTab.PutObjName( str.c_str(), 7 );
            }
            prTab.PutFloat( x[j], 6, 4 );
            for( i=0; i<M; i++ )
                if( i== M-1 ) prTab.PutFloat( y[j*M+i], 7, 3 );
                else prTab.PutFloat( y[j*M+i], 7, 2 );
        }
        if( k ==0 )
        {
            prTab.PutTabEnd( 8 );
            prTab.PutTabEnd( 11 );
        }
        else prTab.PutTabEnd( 10 );
        for( i=0; i<M-1; i++ )
            prTab.PutTabEnd( 11 );
        prTab.PutTabEnd( 12 );
        prTab.PutEnd();
    }
    /* Put equations*/
    if( vfQuestion(window(), GetName(), "Print sampling equations?" ) )
    {
        prTab.PutString( "\n Sampling equations:\n--------------------------" );
        prTab.PutString( (char *)aObj[o_gdexpr].GetPtr() );
    }
    prTab.PutEnd();
    // Put resalts to object
    const char *tab= prTab.GetTable();
    tblen = strlen(tab)+1; /* Get size of text buf */
    gdp->prtab = (char *)aObj[ o_gwprtab ].Alloc( 1, tblen, S_ );
    memcpy( gdp->prtab, tab, tblen );

    vfObjToFile(window(), &aObj[o_gwprtab]);
}

// Print resalt arrays to file
void
TGtDemo::_line_tab( const char *key )
{
    int i, j, k, N, M;
    size_t tblen;
    char (*lNames)[MAXGRNAME];
    double *x, *y;
    vstr pbuf(300);

    SetString("Please, wait until arrays will be sampled");
    pVisor->Update();
    /*Table heard */
    TPrintTable prTab( "Data Sampling, Tabulation and Graphic Demonstration",
                       GetName(), key );
    for( k=0; k<2; k++ )
    { /* cycle by table */
        if( k==0 )
        {
            N = gdp->dimXY[0];
            M = gdp->dimXY[1];
        }
        else
        {
            N = gdp->dimEF[0];
            M = gdp->dimEF[1];
        }
        if( !(M*N) )
            continue;
        if( k==0)
            sprintf( pbuf, si_tab_3c, k, gdp->xNames, M, N  );
        else  sprintf( pbuf, si_tab_3c, k, gd_userf, M,  N  );
        prTab.PutString( pbuf );
        switch( k )
        { /* set pointers to data */
        case 0:
            lNames = gdp->lNam0;
            x = gdp->x0;
            y = gdp->y0;
            break;
        case 1:
            lNames = gdp->lNamE;
            x = gdp->xE;
            y = gdp->yE;
            break;
        }
        for( i=0; i<M; i++ )
        {   /* put colum names */
            prTab.PutObjName( gdp->xNames, 6 );
            prTab.PutObjName( lNames[i], 6 );
            prTab.NextLine();
            for( j=0; j<N; j++ )  //put data gstring
            {
                prTab.PutFloat( x[j], 7, 1 );
                prTab.PutFloat( y[j*M+i], 7, 1 );
                prTab.NextLine();
            }
        }
        prTab.PutEnd();
    }
    // Put resalts to object
    const char *tab= prTab.GetTable();
    tblen = strlen(tab)+1; /* Get size of text buf */
    gdp->prtab = (char *)aObj[ o_gwprtab ].Alloc( 1, tblen, S_ );
    memcpy( gdp->prtab, tab, tblen );

    vfObjToFile(window(), &aObj[o_gwprtab]);
}

// out

/*
void  TGtDemo::gd_put_to_txt( const char *buf, int len )
{
    char *text = (char *)aObj[ o_gdexpr ].GetPtr();
    int  Tlen = strlen(text);
    text += Tlen;
    if( Tlen+len >= aObj[ o_gdexpr ].GetM() )
        gdp->expr = (char *)aObj[ o_gdexpr ].Alloc( 1, Tlen+len*2, S_);
    strncpy( text, buf, len );
    *(text+len) = '\0';
}
*/
/*
// default tetxt of equations
void  TGtDemo::gd_def_text_set()
{
    int nOb, nOr, j, nnRT;

    uint i;
    vstr tbuf(100);
    TProfil *aProf=(TProfil *)(&aMod[RT_PARAM]);
    TCStringArray OBlist;

    // set  names
    if( !*gdp->name )
    {
        strncpy( gdp->name, aProf->pa.GDcode[0], MAXGSNAME-1 );
        gdp->name[MAXGSNAME-1]='\0';
    }

    gd_put_to_txt( "\\J\n$default ", 11);

    // select object names list
    for( i=0; i<10; i++ )
        if( *(&gdp->PsIC+i) == '+' )
        {
            nnRT =  i+RT_ICOMP;
            if( nnRT >= RT_GTDEMO ) nnRT++;
            nOb = rt[nnRT].GetObjFirst();
            nOr = rt[nnRT].GetObjCount();
            for( j=nOb; j<nOr+nOb; j++ )
                OBlist.Add( aObj[j].GetKeywd() );
            if( nnRT == RT_SYSEQ )
            {
                nOb = o_muphc;
                nOr = o_spppar-o_muphc;
                for( j=nOb; j<nOr+nOb; j++ )
                    OBlist.Add( aObj[j].GetKeywd() );
            }
        }
    TCIntArray aMrk = vfMultiChoice(window(), OBlist,
                                    "gd_items  Select data objects to be used in GTDEMO" );
    for( i=0; i< aMrk.GetCount(); i++)
    {
        if( !(i%10) )
            gd_put_to_txt( "\n$", 2);
        gd_put_to_txt( OBlist[aMrk[i]].c_str(), OBlist[aMrk[i]].length() );
        gd_put_to_txt( " ", 1);
    }
    OBlist.Clear();
    // Get indexes
    if( gdp->nRT < RT_SYSEQ  || pVisor->ProfileMode != true )
        goto EXPR;
    // Get indexes
    if( aProf->mup->SB )
    {
        for( j=0; j<aProf->mup->N; j++ )
            OBlist.Add( aProf->mup->SB[j] );
        aMrk = vfMultiChoice(window(), OBlist,
                             "gd_ndx    Select components/phases to use in GTDEMO" );
        for( i=0; i< aMrk.GetCount(); i++)
        {
            if( !(i%10) )
                gd_put_to_txt( "\n$", 2);
            gd_put_to_txt( OBlist[aMrk[i]].c_str(), MAXKEYWD );
            //        gd_put_to_txt( " ", 1);
        }

        OBlist.Clear();
    }
    if( aProf->mup->SF )
    {
        for( j=0; j<aProf->mup->Fi; j++ )
            OBlist.Add( aProf->mup->SF[j] );
        aMrk = vfMultiChoice(window(), OBlist,
                             "gd_ndx    Select components/phases to use in GTDEMO" );
        for( i=0; i< aMrk.GetCount(); i++)
        {
            if( !(i%10) )
                gd_put_to_txt( "\n$", 2);
            gd_put_to_txt( OBlist[aMrk[i]].c_str()+MAXSYMB+MAXPHSYMB, MAXPHNAME );
            //        gd_put_to_txt( " ", 1);
        }

        OBlist.Clear();
    }
    if( aProf->mup->SM )
    {
        for( j=0; j<aProf->mup->L; j++ )
            OBlist.Add( aProf->mup->SM[j] );
        aMrk = vfMultiChoice(window(), OBlist,
                             "gd_ndx    Select components/phases to use in GTDEMO" );
        for( i=0; i< aMrk.GetCount(); i++)
        {
            if( !(i%10) )
                gd_put_to_txt( "\n$", 2);
            gd_put_to_txt( OBlist[aMrk[i]].c_str()+MAXSYMB+MAXDRGROUP, MAXDCNAME );
            //        gd_put_to_txt( " ", 1);
        }

        OBlist.Clear();
    }
    if( aProf->mup->SA )
    {
        for( j=0; j<aProf->mup->La; j++ )
            OBlist.Add( aProf->mup->SA[j] );
        aMrk = vfMultiChoice(window(), OBlist,
                             "gd_ndx    Select components/phases to use in GTDEMO" );
        for( i=0; i< aMrk.GetCount(); i++)
        {
            if( !(i%10) )
                gd_put_to_txt( "\n$", 2);
            gd_put_to_txt( OBlist[aMrk[i]].c_str(), MAXCMPNAME );
            //        gd_put_to_txt( " ", 1);
        }

        OBlist.Clear();
    }

EXPR: // load default equations
    gd_put_to_txt( "\n$ ", 3 );
    gd_put_to_txt( aProf->pa.GDcode[1], strlen(aProf->pa.GDcode[1]) );
    gd_put_to_txt( " 1", 2 );
    gd_put_to_txt( "\nx0[jR] =: 1; ", 14);
    for( j=0; j< gdp->dimXY[1]; j++ )
    {
        sprintf( tbuf, "\ny0[jR][%d] =: 1; ", j );
        gd_put_to_txt( tbuf, strlen(tbuf) );
        sprintf( tbuf, "%s%d", aProf->pa.GDpsc, j+1 );
        if( !*gdp->lNam0[j] )
            strncpy( gdp->lNam0[j], tbuf, MAXGRNAME );
    }
    if( !*gdp->xNames )
        strncpy( gdp->xNames, aProf->pa.GDpcc[0], MAXAXISNAME );
    if( !*gdp->yNames )
        strncpy( gdp->yNames, aProf->pa.GDpcc[1], MAXAXISNAME );

}
*/

DCOMP.cpp
/*--------------------------------------------------------*/
//Print record to table  - to be revised 
void
TDComp::_one_rec_prn( TPrintTable& prTab )
{
    int i;
    double delta=1;

    /*Table heard */
    if( dcp->pct[0]=='H' && dcp->pct[1]=='K' )
        delta = cal_to_J;
    prTab.PutString( dcp->form, MAXFORMULA );
    prTab.PutFloat( dcp->Gs[0]/delta, 7, 5 );
    prTab.PutFloat( dcp->Hs[0]/delta, 7, 5 );
    prTab.PutFloat( dcp->Ss[0]/delta, 7, 5 );
    prTab.PutFloat( dcp->Cps[0]/delta, 7, 5 );
    prTab.PutFloat( dcp->mVs[0]*10, 7, 5 );
    if( dcp->HKFc )
        for( i=0; i<MAXHKFCOEF; i++)
            prTab.PutFloat( dcp->HKFc[i], 7, 5 );
    prTab.PutString( dcp->pct, 6 );
    prTab.PutFloat( dcp->Gs[1]/delta, 7, 5 );
    prTab.PutFloat( dcp->Hs[1]/delta, 7, 5 );
    prTab.PutFloat( dcp->Ss[1]/delta, 7, 5 );
    prTab.PutFloat( dcp->Cps[1]/delta, 7, 5 );
    prTab.PutFloat( 0, 7, 5 );
    prTab.NextLine();
}

/*--------------------------------------------------------*/
//Print selected DComp records to table  - under construction
void
TDComp::RecordPrint( const char */*key*/ )
{
    uint i;
    size_t tblen;
    gstring  str;
    TCStringArray aDclist;
    TCIntArray anRDc;
    TCIntArray aMcv;

    // Get Key list
    str = GetKeyofRecord( "*:*:*:*:", "Set key template", KEY_TEMP);
    if(  str== "" )
        return;
    rt[nRT].GetKeyList( str.c_str(), aDclist, anRDc );
    aMcv = vfMultiChoice( window(), aDclist,
                          "c_dcprn  (mark DCOMPs to be printed to table)" );
    if( aMcv.GetCount()<1 )
        return;
    /* Table header */
    TPrintTable prTab( "Dcomp list", GetName(), "" );
    for( i=0; i<aMcv.GetCount(); i++ )
    {
        RecInput( aDclist[aMcv[i]].c_str() );
        _one_rec_prn( prTab );
    }
    // Put results to object
    prTab.PutEnd();
    const char *tab= prTab.GetTable();
    tblen = strlen(tab)+1; /* Get size of text buf */
    dcp->tprn = (char *)aObj[ o_dctprn ].Alloc( 1, tblen, S_ );
    memcpy( dcp->tprn, tab, tblen );

    vfObjToFile( window(), &aObj[o_dctprn] );
}





process.CPP

#include "v_print.h"
const char * pe_int21 ="| %-6.6s | %-11.6g| %-11.6g| %-11.6g|";
const char * pe_int22 ="| %-6.6s | %-11d| %-11d| %-11d|";

/*--------------------------------------------------------*/
//Print record of Process
void
TProcess::RecordPrint( const char *key)
{
    _pr_tab( key );
}

void
TProcess::_pr_tab( const char *key )
{
    int i, j, k, N, M;
    size_t tblen;
    vstr pbuf(121);
    double ww;

    SetString("Please, wait until arrays will be sampled");
    pVisor->Update();
    /*Table heard */
    TPrintTable prTab( pep->name, GetName(), key );
    M = N = 0;
    for( k=o_petmv; k<=o_penuv; k++ )
    {
        if( !aObj[k].GetPtr()) continue;
        M++;
        N = max( N, aObj[k].GetN() );
    }
    // Process types
    prTab.PutString( endLine, strlen(endLine), false );
    if( pep->PsTP != S_OFF  )
        prTab.PutString( "Process changing TPV of the system" );
    if( pep->PsBC != S_OFF  )
        prTab.PutString( "Process changing composition of the system" );
    if( pep->PsRS != S_OFF  )
        prTab.PutString( "Process changing system CSD parameters" );
    if( pep->PsSY != S_OFF  )
        prTab.PutString( "Mode of saving modified SYSTEM records" );
    if( pep->PsGT != S_OFF  )
        prTab.PutString( "Sequential reactor simulation" );
    if( pep->PsUX != S_OFF  )
        prTab.PutString( "Process depends on equilibrium speciation" );
    if( pep->PsIN != S_OFF  )
        prTab.PutString( "This is an inverse titration problem" );
    if( pep->PvR1 != S_OFF  )
        prTab.PutString( "Inverse titration (built-in Gold Section algorithm)" );
    prTab.PutString( endLine );

    /* intervals */
    prTab.PutString( "\n                             Intervals\n"
                     "|========|============|============|============|\n"
                     "| Object |  Begin     |  End       | Delta      |\n"
                     "|        |  value     |  value     |            |\n"
                     "|========|============|============|============|"  );
    sprintf( pbuf, pe_int22, aObj[o_petmi].GetKeywd(),
             pep->tmi[0], pep->tmi[1], pep->tmi[2] );
    prTab.PutString( pbuf );
    sprintf( pbuf, pe_int22, aObj[o_penvi].GetKeywd(),
             pep->NVi[0], pep->NVi[1], pep->NVi[2] );
    prTab.PutString( pbuf );
    sprintf( pbuf, pe_int21, aObj[o_pepi].GetKeywd(),
             pep->Pi[0], pep->Pi[1], pep->Pi[2] );
    prTab.PutString( pbuf );
    sprintf( pbuf, pe_int21, aObj[o_peti].GetKeywd(),
             pep->Ti[0], pep->Ti[1], pep->Ti[2] );
    prTab.PutString( pbuf );
    sprintf( pbuf, pe_int21, aObj[o_pevi].GetKeywd(),
             pep->Vi[0], pep->Vi[1], pep->Vi[2] );
    prTab.PutString( pbuf );
    sprintf( pbuf, pe_int21, aObj[o_petaui].GetKeywd(),
             pep->Taui[0], pep->Taui[1], pep->Taui[2] );
    prTab.PutString( pbuf );
    sprintf( pbuf, pe_int21, aObj[o_pepxii].GetKeywd(),
             pep->pXii[0], pep->pXii[1], pep->pXii[2] );
    prTab.PutString( pbuf );
    sprintf( pbuf, pe_int21, aObj[o_penui].GetKeywd(),
             pep->Nui[0], pep->Nui[1], pep->Nui[2] );
    prTab.PutString( pbuf );
    sprintf( pbuf, pe_int21, aObj[o_pephi].GetKeywd(),
             pep->pHi[0], pep->pHi[1], pep->pHi[2] );
    prTab.PutString( pbuf );
    sprintf( pbuf, pe_int21, aObj[o_pepei].GetKeywd(),
             pep->pei[0], pep->pei[1], pep->pei[2] );
    prTab.PutString( pbuf );
    prTab.PutString("|========|============|============|============|\n" );

    /* Equations of Process */
    if( aObj[o_peexpr].GetPtr()!= 0 )
    {
        prTab.PutString( "\n Sampling equations:\n----------------------------\n" );
        prTab.PutString( (char *)aObj[o_peexpr].GetPtr() );
        prTab.PutString( endLine );
    }
    /* array modC */
    if( pep->PvModc != S_OFF )
    {
        prTab.PutString( "Array of empirical parameters" );
        if(pep->Nmc==1)
            prTab.PutTabHead( 2 );
        else
        {
            prTab.PutTabHead( 1 );
            for( i=1; i<pep->Nmc-1; i++ )
                prTab.PutTabHead( 5 );
            prTab.PutTabHead( 6 );
        }
        for( j=0; j<pep->Ntm; j++ )
            for( i=0; i<pep->Nmc; i++ )
                if( i== pep->Nmc-1 )
                    prTab.PutFloat( pep->Modc[j*pep->Nmc+i], 7, 3 );
                else  prTab.PutFloat( pep->Modc[j*pep->Nmc+i], 7, 2 );
        if(pep->Nmc==1)
            prTab.PutTabEnd( 9 );
        else
        {
            prTab.PutTabEnd( 10 );
            for( i=1; i<pep->Nmc-1; i++ )
                prTab.PutTabEnd( 11 );
            prTab.PutTabEnd( 12 );
        }
        prTab.NextLine();
    }
    /* ARRAY Kin */
    if( pep->PvKin != S_OFF )
    {
        prTab.PutString( "Array of (reaction) kinetic parameters" );
        if(pep->Nrp==1)
            prTab.PutTabHead( 2 );
        else
        {
            prTab.PutTabHead( 1 );
            for( i=1; i<pep->Nrp-1; i++ )
                prTab.PutTabHead( 5 );
            prTab.PutTabHead( 6 );
        }
        for( j=0; j<pep->Nrea; j++ )
            for( i=0; i<pep->Nrp; i++ )
                if( i== pep->Nrp-1 )
                    prTab.PutFloat( pep->Kin[j*pep->Nrp+i], 7, 3 );
                else  prTab.PutFloat( pep->Kin[j*pep->Nrp+i], 7, 2 );
        if(pep->Nrp==1)
            prTab.PutTabEnd( 9 );
        else
        {
            prTab.PutTabEnd( 10 );
            for( i=1; i<pep->Nrp-1; i++ )
                prTab.PutTabEnd( 11 );
            prTab.PutTabEnd( 12 );
        }
        prTab.NextLine();
    }
    /* work arrays */
    if( M*N )
    {
        prTab.PutString( " Arrays controled parameters of process" );
        if( M==1 )
            prTab.PutTabHead( 2 );
        else
        {
            prTab.PutTabHead( 1 );
            for( i=1; i<M-1; i++ )
                prTab.PutTabHead( 5 );
            prTab.PutTabHead( 6 );
        }
        for( i =0, k=o_petmv; k<=o_penuv; k++ )
        {
            if( !aObj[k].GetPtr()) continue;
            if( i == M-1)
                prTab.PutObjName( aObj[k].GetKeywd(), 3 );
            else prTab.PutObjName( aObj[k].GetKeywd(), 2 );
            i++;
        }
        if( M==1 )
            prTab.PutTabDelta( 2 );
        else
        {
            prTab.PutTabDelta( 1 );
            for( i=1; i<M-1; i++ )
                prTab.PutTabDelta( 8 );
            prTab.PutTabDelta( 9 );
        }
        for( j=0; j<N; j++ )
            for( i=0, k=o_petmv; k<=o_penuv; k++ )
            {
                if( !aObj[k].GetPtr()) continue;
                if( j < aObj[k].GetN() )
                    ww = aObj[k].Get( j, 0 );
                else ww= DOUBLE_EMPTY;
                if( i== M-1 )
                    prTab.PutFloat( ww, 7, 3 );
                else
                    prTab.PutFloat( ww, 7, 2 );
                i++;
            }
        if( M==1 )
            prTab.PutTabEnd( 9 );
        else
        {
            prTab.PutTabEnd( 10 );
            for( i=1; i<M-1; i++ )
                prTab.PutTabEnd( 11 );
            prTab.PutTabEnd( 12 );
        }
    }
    /* list keys of  SYSEQ */
    prTab.PutString( "\n List of tasks:\n----------------------------\n" );
    for( i=0; i<pep->NR1; i++ )
        prTab.PutString( pep->stl[i], EQ_RKLEN );
    prTab.PutEnd( );

    // Put resalts to object
    const char *tab= prTab.GetTable();
    tblen = strlen(tab)+1; /* Get size of text buf */
    pep->tprn = (char *)aObj[ o_petprn ].Alloc( 1, tblen, S_ );
    memcpy( pep->tprn, tab, tblen );
    vfObjToFile(window(), &aObj[o_petprn]);
}


REACDC.cpp

/*--------------------------------------------------------*/
//Print record to table  - to be revised
void
TReacDC::_one_rec_prn( TPrintTable& prTab )
{
    int i;

    prTab.PutString( rcp->form, MAXFORMULA );
    for( i=0; i<rcp->nDC; i++)
    {
        prTab.PutString( rcp->DCk[i], DC_RKLEN, false );
        prTab.PutFloat( rcp->scDC[i], 7, 5 );
        prTab.NextLine();
    }
    prTab.PutFloat( rcp->Ks[1], 7, 5 );
    prTab.PutFloat( rcp->Ss[0], 7, 5 );
    prTab.PutFloat( rcp->Hs[0], 7, 5 );
    prTab.PutFloat( rcp->Cps[0], 7, 5 );
    if( rcp->pKt )
        for( i=0; i<MAXCPCOEF; i++)
            prTab.PutFloat( rcp->pKt[i], 7, 5 );
    prTab.PutString( rcp->pct, 6 );
}


{
    uint i;
    size_t tblen;
    gstring  str;
    TCStringArray aDclist;
    TCIntArray anRDc;
    TCIntArray aMcv;

    // Get Key list
    str = GetKeyofRecord( "*:*:*:*:", "Set template", KEY_TEMP );
    if(  str== "" )
        return;
    rt[nRT].GetKeyList( str.c_str(), aDclist, anRDc );
    aMcv = vfMultiChoice( window(), aDclist,
                          "c_dcprn  (mark REACDCs to be printed to table)" );
    if( aMcv.GetCount() <1 )
        return;
    /*Table heard */
    TPrintTable prTab( "ReactDC list", GetName(), "" );
    for( i=0; i<aMcv.GetCount(); i++ )
    {
        RecInput( aDclist[aMcv[i]].c_str() );
        _one_rec_prn( prTab );
    }
    // Put resalts to object
    prTab.PutEnd();
    const char *tab= prTab.GetTable();
    tblen = strlen(tab)+1; /* Get size of text buffer */
    rcp->tprn = (char *)aObj[ o_retprn ].Alloc( 1, tblen, S_ );
    memcpy( rcp->tprn, tab, tblen );

    vfObjToFile( window(), &aObj[ o_retprn] );

}


RTPARM.cpp

#include "v_print.h"
/*--------------------------------------------------------*/
//Print record RTPARM to table
void
TRTParm::_pr_tab()
{
    int i, j, N, M, k, m1=0, m2;
    size_t tblen;

    N = rpp->NV;
    for( i=0, M=0 ; i<35; i++)
        if( rpp->Pvd[i] != S_OFF )  M++;
    SetString("Please, wait until arrays will be sampled");
    pVisor->Update();
    /*Table heard */
    TPrintTable prTab( "Comparison of RTPARM record", GetName(), rpp->pstate );

    while( m1 < M )
    { /* Head of table */
        m2 = min( m1+4, M);
        if( (M-m2) == 1) m2++;

        prTab.PutTabHead( 1 );
        for( i=m1+1; i<m2-1; i++ )
            prTab.PutTabHead( 5 );
        prTab.PutTabHead( 6 );

        /* Names of colums */
        for( i=0, k=0 ; i<35; i++)
        {
            if( rpp->Pvd[i] == S_OFF )
                continue;
            if( k >= m1 && k < m2 )
            {
                if( k == m2-1 ) prTab.PutObjName( aObj[o_rprt+i].GetKeywd(), 3 );
                else prTab.PutObjName( aObj[o_rprt+i].GetKeywd(), 2 );
            }
            k++;
        }
        prTab.PutTabDelta( 1 );
        for( i=m1+1; i<m2-1; i++ )
            prTab.PutTabDelta( 8 );
        prTab.PutTabDelta( 9 );

        for( j=0; j<N; j++ )
        { /* Put table row */
            for( i=0, k=0; i<35; i++)
            {
                if( rpp->Pvd[i] == S_OFF )
                    continue;
                if( k >= m1 && k < m2 )
                {
                    if( k== m2-1 )
                        prTab.PutFloat( aObj[ o_rprt+i ].Get( j, 0 ), 7, 3 );
                    else
                        prTab.PutFloat( aObj[ o_rprt+i ].Get( j, 0 ), 7, 2 );
                }
                k++;
            }
        }
        prTab.PutTabEnd( 10 );
        for( i=m1+1; i<m2-1; i++ )
            prTab.PutTabEnd( 11 );
        prTab.PutTabEnd( 12 );
        m1 = m2;
    }
    prTab.PutEnd();
    // Put resalts to object
    const char *tab= prTab.GetTable();
    tblen = strlen(tab)+1; /* Get size of text buf */
    rpp->tprn = (char *)aObj[ o_rptprn ].Alloc( 1, tblen, S_ );
    memcpy( rpp->tprn, tab, tblen );

    vfObjToFile( window(), &aObj[o_rptprn] );
}

/*--------------------------------------------------------*/
//Print RTParm record (also compared) to table
void
TRTParm::_pr_line()
{
    int i, j, N, dimM, k;
    size_t tblen;

    N = rpp->NV;
    SetString("Please, wait until arrays will be sampled");
    pVisor->Update();
    /*Table header */
    TPrintTable prTab( "Comparison of RTParm record", GetName(), rpp->pstate );
    prTab.PutString( rpp->pstate, RP_RKLEN );
    if( rpp->RPk )
        for( k=0; k<3; k++)
            prTab.PutString( rpp->RPk[k], RP_RKLEN );
    prTab.NextLine();

    /* print one object to compare */
    for( i=0 ; i<35; i++)
    {
        if( rpp->Pvd[i] == S_OFF )
            continue;
        /* Colums name*/
        dimM = aObj[ o_rprt+i ].GetM();
        for( k=0; k<dimM; k++)
            prTab.PutObjName( aObj[o_rprt+i].GetKeywd(), 1, k );
        prTab.NextLine();

        for( j=0; j<N; j++ )
        { /* print row of table */
            for( k=0; k<dimM; k++)
                prTab.PutFloat( aObj[ o_rprt+i ].Get( j, k ), 9, 1 );
            prTab.NextLine();
        }
        prTab.NextLine();
    }
    prTab.PutEnd();
    // Put results to object
    const char *tab= prTab.GetTable();
    tblen = strlen(tab)+1; /* Get size of text buf */
    rpp->tprn = (char *)aObj[ o_rptprn ].Alloc( 1, tblen, S_ );
    memcpy( rpp->tprn, tab, tblen );

    vfObjToFile(window(), &aObj[o_rptprn]);
}

/*--------------------------------------------------------*/
//Print records from RTPARM to table
void
TRTParm::RecordPrint( const char *key )
{
    if( vfQuestion(window(), GetName(),
                   "Print tables or export to plain text?"))
        _pr_tab();
    else
    {
        Demo( key );
        _pr_line();
        RecInput(key);
    }
}


//-----------------------------------------------------------------

void
TRTParm::Demo( const char *key )
{
    char *rpkey;
    int i, j, k, q, nRPk=0;
    time_t crt;
    float *array[36], *vector;
    bool demoMode = false;

    // demo mode set before
    for( i=0; i<36; i++)
        if(  rpp->Pvd[i] != S_OFF &&  aObj[  o_rprt+i ].GetM() > 1 )
        {
            demoMode = true;
            break;
        }
    if( demoMode == true )
        return;

    for( i=0; i<nQ; i++)
        if( rpp == &rp[i])
            break;
    q =i;

    if( rp[q].PunR1 != S_OFF )
    { /* Read anather RTPARM to compare arrays */
        for( nRPk=0,j=0; j<3; j++)
        {
            rpkey = rp[q].RPk[j];
            if( rpkey[0] == '`' )
                continue;
            nRPk++;
            ods_link(q+j+1);
            TryRecInp( rpkey, crt, q+j+1 );
            if( rp[q].NV > rp[q+j+1].NV ) nRPk--;
        } /* j */
        ods_link( q );
    }

    for(i=0; i<36; i++)
    {
        if( nRPk > 0 && i>3 && rp[q].Pvd[i] != S_OFF )
        { /*Realloc memory and set data */
            array[i] =  new float[(nRPk+1)*rp[q].NV];
            ErrorIf( !array[i], "Alloc", "New operator error");
            for( j=0; j <= nRPk; j++ )
            {
                ods_link( q+j );
                vector = (float *)aObj[ o_rprt + i ].GetPtr();
                if( vector )
                    for( k=0; k < rp[q].NV; k++ )
                        array[i][ (nRPk+1)*k + j ] = vector[k];
            }
            ods_link( q );
        }
    }
    float *ptr;
    for(i=0; i<36; i++)
    {
        if( nRPk > 0 && i>3 && rp[q].Pvd[i] != S_OFF )
        { /* Set data to object*/

            aObj[  o_rprt+i ].Alloc( rp[q].NV, nRPk+1, F_ );
            ptr =  (float *) aObj[o_rprt+i ].GetPtr();
            memcpy( ptr, array[i], sizeof(float)*((nRPk+1)*rp[q].NV));
            delete[] array[i];
        }
    }
    rt[nRT].SetKey(key);
    dyn_set( q );
}

//Demo of RTParm record
void
TRTParm::CmDemo()
{
    try
    {
        MessageToSave();
        gstring str=rt[nRT].PackKey();
        if( str.find_first_of("*?" ) != gstring::npos )
            Error( GetName(), "Undefined current record!");
        int  Rnum = rt[nRT].Find( str.c_str() );
        ErrorIf( Rnum<0, GetName(), "Record to calculate not found!");
        Demo( str.c_str() );
        SetString("Demo record");
        pVisor->Update();
    }
    catch( TError& xcpt )
    {
        vfMessage(window(), xcpt.title, xcpt.mess);
    }
}

void
TRTParm::RecordPlot( const char *key )
{
/*    TIArray<TPlot> pls1;
    TIArray<TPlot> pls2;
    TIArray<TPlot> pls3;
    TIArray<TPlot> pls4;
    int i, nX;
    TPlotLine defpl("");

    int nLn1=0, nLn2=0, nLn3=0, nLn4=0;

    if( rpp->Pvd[1] == '+' ) nX = o_rpt;
    else nX = o_rpp;

    // set sizes
    Demo( key );
    for( i=0; i<35; i++)
        if( rpp->Pvd[i] != S_OFF )
            switch( rpp->Pvd[i] )
            {
            case '1':
                if( pls1.GetCount() > 5 ) break;
                pls1.Add( new TPlot( nX, o_rprt+i));
                nLn1 += pls1[ pls1.GetCount()-1].getLinesNumber();
                break;
            case '2':
                if( pls2.GetCount() > 5 ) break;
                pls2.Add( new TPlot( nX, o_rprt+i));
                nLn2 += pls1[ pls2.GetCount()-1].getLinesNumber();
                break;
            case '3':
                if( pls3.GetCount() > 5 ) break;
                pls3.Add( new TPlot( nX, o_rprt+i));
                nLn3 += pls3[ pls1.GetCount()-1].getLinesNumber();
                break;
            case '4':
                if( pls4.GetCount() > 5 ) break;
                pls4.Add( new TPlot( nX, o_rprt+i));
                nLn4 += pls4[ pls1.GetCount()-1].getLinesNumber();
                break;
            }
    if( pls1.GetCount()>0 )
    {
        if( rpp->Plot1 )
        {
            int oldN = aObj[ o_rpdata1].GetN();
            rpp->nLs[0] = nLn1;
            rpp->Plot1 = (TPlotLine *)aObj[ o_rpdata1].Alloc( nLn1, sizeof(TPlotLine));
            for( int ii=oldN; ii<nLn1; ii++ )
                rpp->Plot1[ii] = defpl;
            graph1 = new GraphWindow( this, pls1, "RTParm graph window 1",
                                          rpp->size[0], rpp->size[1], rpp->Plot1, rpp->axisType[0],
                                          0, rpp->xName[0], rpp->yName[0] );
        }
        else
            graph1 = new GraphWindow( this, pls1, "RTParm graph window 1", 1, "x", "y");

    }

    if( pls2.GetCount()>0 )
    {
        if( rpp->Plot2 )
        {
            int oldN = aObj[ o_rpdata2].GetN();
            rpp->nLs[1] = nLn2;
            rpp->Plot2 = (TPlotLine *)aObj[ o_rpdata2].Alloc( nLn2, sizeof(TPlotLine));
            for( int ii=oldN; ii<nLn2; ii++ )
                rpp->Plot2[ii] = defpl;
            graph2 = new GraphWindow( this, pls2, "RTParm graph window 2",
                                          rpp->size[2], rpp->size[3], rpp->Plot2, rpp->axisType[1],
                                          0, rpp->xName[1], rpp->yName[1] );
        }
        else
            graph2 = new GraphWindow( this, pls2, "RTParm graph window 2", 2, "x", "y");

    }

    if( pls3.GetCount()>0 )
    {
        if( rpp->Plot3 )
        {
            int oldN = aObj[ o_rpdata3].GetN();
            rpp->nLs[2] = nLn3;
            rpp->Plot3 = (TPlotLine *)aObj[ o_rpdata3].Alloc( nLn3, sizeof(TPlotLine));
            for( int ii=oldN; ii<nLn3; ii++ )
                rpp->Plot3[ii] = defpl;
            graph3 = new GraphWindow( this, pls3, "RTParm graph window 3",
                                          rpp->size[4], rpp->size[5], rpp->Plot3, rpp->axisType[2],
                                          0, rpp->xName[2], rpp->yName[2] );
        }
        else
            graph3 = new GraphWindow( this, pls3, "RTParm graph window 3", 3, "x", "y");

    }

    if( pls4.GetCount()>0 )
    {
        if( rpp->Plot4 )
        {
            int oldN = aObj[ o_rpdata4].GetN();
            rpp->nLs[3] = nLn4;
            rpp->Plot4 = (TPlotLine *)aObj[ o_rpdata4].Alloc( nLn4, sizeof(TPlotLine));
            for( int ii=oldN; ii<nLn4; ii++ )
                rpp->Plot4[ii] = defpl;
            graph4 = new GraphWindow( this, pls4, "RTParm graph window 4",
                                          rpp->size[6], rpp->size[7], rpp->Plot4, rpp->axisType[3],
                                          0, rpp->xName[3], rpp->yName[3] );
        }
        else
            graph4 = new GraphWindow( this, pls4, "RTParm graph window 4", 4, "x", "y");

    }
    graphKey = key;
    //exit from demo mode (read old data)
    //  RecInput(key);
*/
}


bool
TRTParm::SaveGraphData( GraphData *gr )
{

/*    if( gr == graph1 )
    {
        //exit from demo mode (read old data)
        RecInput(graphKey.c_str());
        rpp->axisType[0] = gr->axisType;
        strncpy( rpp->xName[0], gr->xName.c_str(), 9);
        strncpy( rpp->yName[0], gr->yName.c_str(), 9);
        memcpy( &rpp->size[0][0], gr->region, 4*sizeof(float) );
        memcpy( &rpp->size[1][0], gr->part,  4*sizeof(float) );

        rpp->Plot1 = (TPlotLine *)
                     aObj[ o_rpdata1].Alloc( gr->lines.GetCount(), sizeof(TPlotLine));
        rpp->nLs[0] = gr->lines.GetCount();
        for(uint ii=0; ii<gr->lines.GetCount(); ii++ )
            rpp->Plot1[ii] = gr->lines[ii];

        RecSave( graphKey.c_str() );

    }
    if( gr == graph2 )
    {
        //exit from demo mode (read old data)
        RecInput(graphKey.c_str());
        rpp->axisType[1] = gr->axisType;
        strncpy( rpp->xName[1], gr->xName.c_str(), 9);
        strncpy( rpp->yName[1], gr->yName.c_str(), 9);
        memcpy( &rpp->size[2][0], gr->region, 4*sizeof(float) );
        memcpy( &rpp->size[3][0], gr->part,  4*sizeof(float) );

        rpp->Plot2 = (TPlotLine *)
                     aObj[ o_rpdata2].Alloc( gr->lines.GetCount(), sizeof(TPlotLine));
        rpp->nLs[1] = gr->lines.GetCount();
        for(uint ii=0; ii<gr->lines.GetCount(); ii++ )
            rpp->Plot2[ii] = gr->lines[ii];

        RecSave( graphKey.c_str() );

    }
    if( gr == graph3 )
    {
        //exit from demo mode (read old data)
        RecInput(graphKey.c_str());
        rpp->axisType[2] = gr->axisType;
        strncpy( rpp->xName[2], gr->xName.c_str(), 9);
        strncpy( rpp->yName[2], gr->yName.c_str(), 9);
        memcpy( &rpp->size[4][0], gr->region, 4*sizeof(float) );
        memcpy( &rpp->size[5][0], gr->part,  4*sizeof(float) );

        rpp->Plot3 = (TPlotLine *)
                     aObj[ o_rpdata3].Alloc( gr->lines.GetCount(), sizeof(TPlotLine));
        rpp->nLs[2] = gr->lines.GetCount();
        for(uint ii=0; ii<gr->lines.GetCount(); ii++ )
            rpp->Plot3[ii] = gr->lines[ii];

        RecSave( graphKey.c_str() );
    }
    if( gr == graph4 )
    {
        //exit from demo mode (read old data)
        RecInput(graphKey.c_str());
        rpp->axisType[3] = gr->axisType;
        strncpy( rpp->xName[3], gr->xName.c_str(), 9);
        strncpy( rpp->yName[3], gr->yName.c_str(), 9);
        memcpy( &rpp->size[6][0], gr->region, 4*sizeof(float) );
        memcpy( &rpp->size[7][0], gr->part,  4*sizeof(float) );

        rpp->Plot4 = (TPlotLine *)
                     aObj[ o_rpdata4].Alloc( gr->lines.GetCount(), sizeof(TPlotLine));
        rpp->nLs[3] = gr->lines.GetCount();
        for(uint ii=0; ii<gr->lines.GetCount(); ii++ )
            rpp->Plot4[ii] = gr->lines[ii];

        RecSave( graphKey.c_str() );
    }

    fEdit = false;
    // set sizes
    Demo( graphKey.c_str() );
*/    return true;
}
//    ms_mtparm--------------------------

#include "v_print.h"
/*--------------------------------------------------------*/
//Print record MTPARM to table
void
TMTparm::_pr_tab( const char *key )
{
    int i, j, N, M, k;
    unsigned long tblen;
    vstr pbuf(300);

    N = tp.L;
    for( i=0, M=0; i<15; i++)
        if( *(&tp.PtvG+i) != S_OFF ) M++;
    ErrorIf( !(M*N), GetName(), "No data in MTparm!" );
    SetString("Please, wait until arrays will be sampled");
    pVisor->Update();
    TPrintTable prTab( "  Contents of MTparm ", GetName(), key );

    /*Table heard */
    prTab.PutTabHead( 3 );
    for( i=0; i<M-1; i++ )
        prTab.PutTabHead( 5 );
    prTab.PutTabHead( 6 );
    /*put colum names */
    prTab.PutString( "|  Record Key                    | | | ", 39, false );
    for( i=0, k=0; i<15; i++)
    {
        if( *(&tp.PtvG+i) == S_OFF )
            continue;
        if( k == M-1 ) prTab.PutObjName( aObj[o_tpg+i].GetKeywd(), 3 );
        else prTab.PutObjName( aObj[o_tpg+i].GetKeywd(), 2 );
        k++;
    }
    prTab.PutTabDelta( 3 );
    for( i=0; i<M-1; i++ )
        prTab.PutTabDelta( 8 );
    prTab.PutTabDelta( 9 );
    for( j=0; j<N; j++ )  // gstring of data
    {
        sprintf( pbuf,   " %-32.32s %c %c %c", mup->SM[j], tp.mark[j],
                 mup->DCC[j], mup->DCS[j] );
        prTab.PutString( pbuf, strlen(pbuf), false );
        for( i=0, k=0; i<15; i++)
        {
            if( *(&tp.PtvG+i) == S_OFF )
                continue;
            if( k== M-1 )
                prTab.PutFloat( aObj[ o_tpg+i ].Get( j, 0 ), 7, 3 );
            else
                prTab.PutFloat( aObj[ o_tpg+i ].Get( j, 0 ), 7, 2 );
            k++;
        }
    }
    prTab.PutTabEnd( 7 );
    for( i=0; i<M-1; i++ )
        prTab.PutTabEnd( 11 );
    prTab.PutTabEnd( 12 );
    prTab.PutEnd();
    // Put resalts to object
    const char *tab= prTab.GetTable();
    tblen = strlen(tab)+1; /* Get size of text buf */
    TProfil *aPa=(TProfil *)(&aMod[RT_PARAM]);
    aPa->pa.p.tprn= (char *)aObj[o_patprn].Alloc( 1, tblen, S_ );
    memcpy( aPa->pa.p.tprn, tab, tblen );

    vfObjToFile(window(), &aObj[o_patprn]);
}

//Print record MTPARM to text
void
TMTparm::_pr_line( const char *key )
{
    int i, j, N, M;
    unsigned long tblen;
    vstr pbuf(300);

    N = tp.L;
    for( i=0, M=0; i<15; i++)
        if( *(&tp.PtvG+i) != S_OFF ) M++;
    ErrorIf( !(M*N), GetName(), "No data in MTparm!" );
    SetString("Please, wait until arrays will be sampled");
    pVisor->Update();
    TPrintTable prTab( "  Contents of MTparm ", GetName(), key );

    for( i=0; i<15; i++) // print lines head
    {
        if( *(&tp.PtvG+i) == S_OFF )
            continue;
        prTab.PutObjName( aObj[o_tpg+i].GetKeywd(), 1, 0 );
    }
    prTab.NextLine();
    for( j=0; j<N; j++ ) // print lines data
    {
        for( i=0; i<15; i++)
        {
            if( *(&tp.PtvG+i) == S_OFF )
                continue;
            prTab.PutFloat( aObj[ o_tpg+i ].Get( j, 0 ), 9, 1 );
        }
        prTab.NextLine();
    }
    prTab.PutEnd();
    // Put resalts to object
    const char *tab= prTab.GetTable();
    tblen = strlen(tab)+1; /* Get size of text buf */
    TProfil *aPa=(TProfil *)(&aMod[RT_PARAM]);
    aPa->pa.p.tprn= (char *)aObj[o_patprn].Alloc( 1, tblen, S_ );
    memcpy( aPa->pa.p.tprn, tab, tblen );

    vfObjToFile(window(), &aObj[o_patprn]);
}

void
TMTparm::RecordPrint( const char *key )
{
    if( vfQuestion(window(), GetName(),
                   "Print tables or export to text?"))
        _pr_tab( key );
    else
        _pr_line( key);
}

//------------------------------------------------------------------------
ms_muleq



/*--------------------------------------------------------*/
//Print record SYSEQ to table
// IComp elements
void TMulti::print_IC( TPrintTable &prTab )
{
    int i;
    vstr pbuf(200);
    TProfil *aPa=(TProfil *)(&aMod[RT_PARAM]);

    prTab.PutString(
        "                                                         Table 1\n"
        " PARAMETERS OF STOICHIOMETRIC UNITS (IC, independent components)\n"
        "|=========|=============|============|===========================|\n"
        "|  IC     |b (input bulk| Deviation  | u (IC chemical potential) |\n"
        "| Name    | composition,| of mass    |-------------|-------------|\n"
        "|         |   moles)    | balance    | Normalized  |   J/mole    |\n"
        "|=========|=============|============|=============|=============|" );

    for( i=0; i<pm.N; i++ ) // print one gstring
    {
        sprintf( pbuf, " %-8.8s  %12.6e  %11.4e  %12.7g  %12.3f  ",
                 pm.SB[i], pm.B[i], pm.C[i], pm.U[i], pm.U_r[i] );
        prTab.PutString( pbuf );
    }
    prTab.PutString(
        "|=========|=============|============|===========================|\n");
    if( pm.PHC[0] == PH_AQUEL )
    {
        if( pm.XF[0] < aPa->pa.p.DS )
            goto AQ_KILLED;
        sprintf( pbuf, "Aqueous phase: I(molal)= %-.7g pH= %-.4g pe=%-.4g Eh(V)=%-.5g",
                 pm.IC, pm.pH, pm.pe, pm.Eh );
        prTab.PutString( pbuf );
        /* Print table of molyaliti */
        sprintf( pbuf, "\n%-62s Table 1a", pm.stkey );
        prTab.PutString( pbuf );
        prTab.PutString(
            "       TOTAL DISSOLVED INDEPENDENT COMPONENTS\n"
            "|=========|==============|=============|==============|\n"
            "|   IC    |m_i,t Molality|    Log      | Mass Conc.   |\n"
            "|  Name   | total (moles |  molality   | (mg per 1 kg |\n"
            "|         |per 1 kg H2O) |(log(m_i,t)) | of solution) |\n"
            "|=========|==============|=============|==============|");
        for( i=0; i<pm.N; i++ ) //print table gstring
        {
            sprintf( pbuf, " %-8.8s  %13.6e  %12.5f  %13.7g  ",
                     pm.SB[i], pm.IC_m[i], pm.IC_lm[i], pm.IC_wm[i]*1e3 );
            prTab.PutString( pbuf );
        }
        prTab.PutString(
            "|=========|==============|=============|==============|\n");
    }
AQ_KILLED:
    ;
}

const char *tc_tab_14a =" %-14.13s %c %12.5e %10.5g %8.5g %8.4f %9.1lf ";
const char *tc_tab_14x =" %-13.12s %c %11.5e %11.6g %8.4g %8.4f %9.1lf ";
const char *tc_tab_14m =" %-13.12s %c %12.5le %10.4e %8.4g %8.4g %9.1lf ";
const char *tc_tab_15x =
    "|============|==|============|==========|========|========|==========|";
const char *tc_tab_16x =
    "COMMENTS: for surface species, surface activity coefficients are listed\n"
    "Excess Gibss energy related to Galvani potential is included into values\n"
    "of electrochemical potential.";

// DComp elements
void TMulti::print_DC( TPrintTable &prTab, int &jb )
{
    int i, je, k=0, Header=1;
    double XFC, AFC/*,XFA*/;
    vstr pbuf(300);
    TProfil *aPa=(TProfil *)(&aMod[RT_PARAM]);

    if( pm.PHC[0] == PH_AQUEL )
    {
        if( pm.XF[0] < aPa->pa.p.DS )
            goto AQ_KILLED;
        /* print table 2a */  //next page ?
        sprintf( pbuf, "\n%62s: Table 2a", pm.stkey );
        prTab.PutString( pbuf );
        prTab.PutString(
            "   PARAMETERS OF (VALID) AQUEOUS SPECIES (DC IN AQUEOS ELECTROLYTE)\n"
            "|==============|==|===========|==========|========|========|===========|\n"
            "| DC (aqueous):   | Molality  | Mass con-|Activity|  log   |Dual chem. |\n"
            "|  Name       Type|m_j (moles |centration| coeffi-| acti-  |potential  |\n"
            "|                 |per 1kgH2O)|  (ppm)   | cient  | vity   |(J/mole)   |\n"
            "|==============|==|===========|==========|========|========|===========|");
        for( i=0; i<pm.L1[0]; i++ )  // put gstring
        {
            if( pm.X[i] < pm.lowPosNum )
                continue;
            switch( pm.DCC[i] )
            {
            default:
                sprintf( pbuf, tc_tab_14a,
                         pm.SM[i], pm.DCC[i], pm.Y_m[i], pm.Y_w[i],
                         pm.Gamma[i], pm.Y_la[i], pm.Fx[i] /* *pm.RT */ );
                break;
            case DC_AQ_SOLVENT:
            case DC_AQ_SOLVCOM:
                sprintf( pbuf, tc_tab_14a,
                         pm.SM[i], pm.DCC[i], pm.X[i], pm.X[i]*pm.MM[i]/1e3,
                         pm.Gamma[i], pm.Y_la[i], pm.Fx[i] /* *pm.RT */ );
            }
            prTab.PutString( pbuf );
            jb = i;
        }
        prTab.PutString(
            "|==============|==|===========|==========|========|========|===========|\n");
        prTab.PutString(
            "COMMENTS: for H2O-solvent, instead of molality, a total number of moles\n"
            "  in the system is printed, instead of mass concentration - mass in kg." );
        sprintf( pbuf,
                 "For liquid H2O at PVT: density = %-.7g, molar volume = %-.4g,\n"
                 "  dielectric constant = %-.4g.\n"
                 "Parameters of aqueous phase: I=%-.7g pH=%-.3g pe=%-.3g Eh=%-.5g\n",
                 aPa->tpp->RoW, (float)(18.016/aPa->tpp->RoW), aPa->tpp->EpsW,
                 pm.IC, pm.pH, pm.pe, pm.Eh );
        prTab.PutString( pbuf );
AQ_KILLED:
        jb = pm.L1[0];
        k++;
    }
    if( pm.PHC[k] == PH_GASMIX || pm.PHC[k] == PH_PLASMA )
    {
        je = jb + pm.L1[k];
        if( pm.XF[k] < aPa->pa.p.DS )
            goto GAS_KILLED;
        /* Print table 2g */  // next page?
        sprintf( pbuf, "\n%62s: Table 2g", pm.stkey );
        prTab.PutString( pbuf );
        prTab.PutString(
            "         PARAMETERS OF DEPENDENT COMPONENTS IN GASEOUS PHASE\n"
            "|============|==|============|==========|========|========|==========|\n"
            "| DC (gaseous): |x_j (number | Partial  |Fugacity|log(p_j)|Dual chem.|\n"
            "|  Name     Type|of moles in | fugacity |coeffi- |partial |potential |\n"
            "|               |the system) | f_j, bar |cient   |pressure| (J/mole) |\n"
            "|============|==|============|==========|========|========|==========|");
        for( i= jb; i<je; i++ )
        {  /* Print line table 2g */
            /*      if( pm.X[i] < pm.lowPosNum )
                    continue;  */
            sprintf( pbuf, " %-13.12s %c %11.5e %10.5g %8.4g %9.5g %9.1f ",
                     pm.SM[i], pm.DCC[i], pm.X[i], pm.Fug[i- jb], pm.Gamma[i],
                     pm.Ppg_l[i- jb], pm.Fx[i] /* *pm.RT */ );
            prTab.PutString( pbuf );
        }
        prTab.PutString(
            "|============|==|============|==========|========|========|==========|\n");
GAS_KILLED:
        k++;
        jb = je;
    }
    if( k < pm.FIs ) /* other phases multicomponent */
    {
        for( ; k<pm.FIs; k++ )
        {
            je = jb + pm.L1[k];
            if( pm.XF[k] < aPa->pa.p.DS )
                goto SOL_KILLED;

            if( pm.PHC[k] == PH_SORPTION || pm.PHC[k] == PH_POLYEL )
            {
                /* Print table 2x */
                if( Header > 0 ) // print header for adsorbtion phases next line?
                {
                    sprintf( pbuf, "\n%62s: Table 2x", pm.stkey );
                    prTab.PutString( pbuf );
                    prTab.PutString(
                        "PARAMETERS OF DEPENDENT COMPONENTS IN ASYMMETRIC PHASES WITH SORPTION\n"
                        "|============|==|============|==========|========|========|==========|\n"
                        "| DC (sorption):|x_j (number |Wx_j (sur-| Surface|  log   |(Electro)-|\n"
                        "|  Name     Type|of moles in |face mole |activity| mola-  |chem.pot. |\n"
                        "|               |the system) |fraction) | coeff. | lity   |(J/mole)  |\n"
                        "|============|==|============|==========|========|========|==========|");
                    Header = 0;
                }
                /* print names and phase parametres */
                XFC = pm.XF[k] - pm.XFA[k];
                AFC = pm.XFA[k] * pm.Aalp[k];
                //XFA = pm.XFA[k];
                sprintf( pbuf,
                         "Phase:%-12.10s A(m2)=%-.5g Xs(mole)=%-.6g Gmax(meq/m2)=%-.6g\n",
                         pm.SF[k]+MAXSYMB, AFC, XFC, XFC/AFC*1000. );
                prTab.PutString( pbuf );
                i=0;
                sprintf( pbuf, "-EDL: Charge(C/m2): qA=%g qB=%g qD=%g\n"
                         "      Potential(V): PsiA=%g PsiB=%g PsiD=%g\n",
                         pm.XetaA[k][i]/AFC, pm.XetaB[k][i]/AFC,
                         (-pm.XetaA[k][i]-pm.XetaB[k][i])/AFC, pm.XpsiA[k][i],
                         pm.XpsiB[k][i], pm.XpsiD[k][i] /* , pm.XcapA[k],
                                    pm.XcapB[k], pm.XcapD[k], pm.XdlD[k], pm.XdlB[k] */ );
                prTab.PutString( pbuf );
                for( i= jb; i<je; i++ )
                {  /* PRINT gstring of table 2x */
                    if( pm.X[i] < pm.lowPosNum )
                        continue;
                    if( pm.DCC[i] == DC_SUR_MINAL || pm.DCC[i] == DC_SUR_CARRIER
                            || pm.DCC[i] == DC_PEL_CARRIER )
                        sprintf( pbuf, tc_tab_14x,
                                 pm.SM[i], pm.DCC[i], pm.X[i], pm.Wx[i],
                                 pm.Gamma[i], pm.Y_la[i], pm.Fx[i] /* *pm.RT */ );
                    else sprintf( pbuf, tc_tab_14x,
                                      pm.SM[i], pm.DCC[i], pm.X[i], pm.Wx[i],
                                      pm.Gamma[i], pm.Y_la[i], pm.Fx[i] /* *pm.RT */ );
                    prTab.PutString( pbuf );
                }
                goto SOL_KILLED;
            }
            else
            { /* other multicomponent phases */
                if( Header == 0 )
                {
                    prTab.PutString( tc_tab_15x );
                    prTab.PutString( tc_tab_16x );
                }
                if( Header == 1 )
                { /* Print table 2m */   // next gstring?
                    sprintf( pbuf, "\n%62s: Table 2m", pm.stkey );
                    prTab.PutString( pbuf );
                    prTab.PutString(
                        "PARAMETERS OF (VALID) DEPENDENT COMPONENTS OF OTHER PHASES-SOLUTIONS\n"
                        "|============|==|============|==========|========|========|===========|\n"
                        "| DC (solutes): |x_j (number |Wx_j (mole|Activity|  log   |Dual chem. |\n"
                        "|  Name     Type|of moles in |fraction  | coeffi-| acti-  |potential  |\n"
                        "|               |the system) |in phase) | cient  | vity   |(J/mole)   |\n"
                        "|============|==|============|==========|========|========|===========|");
                    Header = 2;
                }
                sprintf( pbuf, "Phase: %-12.11s  Xa(moles)= %g Ma(kg)= %g Va(L)=%g ",
                         pm.SF[k]+MAXSYMB, pm.XF[k], pm.FWGT[k]/1e3, pm.FVOL[k]/1e3 );
                prTab.PutString( pbuf );
                for( i= jb; i<je; i++ )
                {  /* print gstring table 2m */
                    if( pm.X[i] < pm.lowPosNum )
                        continue;
                    sprintf( pbuf, tc_tab_14m,
                             pm.SM[i], pm.DCC[i], pm.X[i], pm.Wx[i], pm.Gamma[i],
                             pm.Y_la[i], pm.Fx[i] /* *pm.RT */ );
                    prTab.PutString( pbuf );
                }
            }
SOL_KILLED:
            jb =je;
        } /* end multicomponent cycle */
        if( !Header )
        {
            prTab.PutString( tc_tab_15x );
            prTab.PutString( tc_tab_16x );
        }
        if( Header > 1 )
        {
            prTab.PutString(
                "|============|==|============|==========|========|========|===========|" );
            prTab.PutString( "COMMENTS:  \n  \n" );
        }
        Header = 1;
    }
    if( pm.FI > pm.FIs && k >= pm.FIs )
    { /* one component phases */
        for( ; k<pm.FI; k++ )
        {
            je = jb + pm.L1[k];
            if( pm.XF[k] < aPa->pa.p.DS )
                goto SINGL_KILLED;
            if( Header )
            { /* Print Table 2s */ // next page?
                sprintf( pbuf, "\n%62s: Table 2s", pm.stkey );
                prTab.PutString( pbuf );
                prTab.PutString(
                    "PARAMETERS OF (VALID) DEPENDENT COMPONENTS IN SINGLE-COMPONENT PHASES\n"
                    "|============|==|============|==========|========|==========|==========|\n"
                    "| DC (single):  | x_j (number| Mass con-| Meta-  |gEx_j ex- |Dual chem.|\n"
                    "|  Name     Type| of moles in| tent in  |stabili-|cess ener-|potential |\n"
                    "|               | the system)|the system|ty coef.|gy metast.| (J/mole) |\n"
                    "|============|==|============|==========|========|==========|==========|");
                Header = 0;
            }
            i= jb;
            /* print gstring table 2s */
            if( pm.X[i] < pm.lowPosNum )
                continue;
            sprintf( pbuf, tc_tab_14m,
                     pm.SM[i], pm.DCC[i], pm.X[i], pm.Y_w[i], pm.Gamma[i],
                     pm.GEX[i]*pm.RT, pm.Fx[i] /* *pm.RT */ );
            prTab.PutString( pbuf );
SINGL_KILLED:
            jb =je;
        } /* end cycle one component phases */
        if( !Header )
        {
            prTab.PutString(
                "|============|==|============|==========|========|========|============|" );
            prTab.PutString( "COMMENTS:  \n  \n" );
        }
    }
}



const char *tc_tab_18 ="  %-13.11s %c %10.6lg %10.4lg %9.5lg %10.6lg %10.6lg ";
// Phase elements
void TMulti::print_PH( TPrintTable &prTab )
{
    int  jb, je=0, k;
    vstr pbuf(200);
    TProfil *aPa=(TProfil *)(&aMod[RT_PARAM]);

    /* Print table 3 - phases */ // next page?
    sprintf( pbuf, "\n%62s: Table 3", pm.stkey );
    prTab.PutString( pbuf );
    prTab.PutString(
        "                TOTAL PARAMETERS OF PHASES PRESENT AT EQUILIBRIUM STATE\n"
        "|============|===|==========|==========|=========|==========|==========|\n"
        "| Phase          |XFa number|F(a) phase|w(a) mass|g(a) meta-|XAa number|\n"
        "| Name      Type |of moles  |stability |content  |stability |of moles  |\n"
        "|                |in system |criterion |in syst.%|parameter |of carrier|\n"
        "|============|===|==========|==========|=========|==========|==========|");
    for( k=0;  k < pm.FI; k++ ) // print one gstring
    {
        jb = je;
        je += pm.L1[k];
        if( pm.XF[k] < aPa->pa.p.DS )
            continue;
        if( k < pm.FIs )   /* multicomponent phase */
            sprintf( pbuf, tc_tab_18, pm.SF[k]+MAXSYMB, pm.PHC[k], pm.XF[k],
                     pm.Falp[k], pm.FWGT[k]/pm.MBX/10.,
                     pm.YOF[k]*pm.RT, pm.XFA[k] );
        else  /* one component phase */
            sprintf( pbuf, tc_tab_18, pm.SF[k]+MAXSYMB, pm.PHC[k],
                     pm.XF[k], pm.Falp[k], pm.FWGT[k]/pm.MBX/10.,
                     pm.GEX[jb]*pm.RT, 0.0 );
        prTab.PutString( pbuf );
    }
    prTab.PutString(
        "|============|===|==========|==========|=========|==========|==========|");
    prTab.PutString(
        "COMMENTS: g(a) parameter for dispersed and sorbing phases displays an \n"
        "excess free energy that causes elevated solubility of phase carrier \n"
        "due to influence of free energy of solid-aqueous interface" );
}

void
TMulti::RecordPrint( const char *key )
{
    int jb=0;
    long int tblen;
    vstr pbuf(200);

    SetString("Please, wait until arrays will be sampled");
    pVisor->Update();
    /*Table heard */
    TPrintTable prTab(
        " CALCULATION OF EQUILIBRIUM STATE IN GEOCHEMICAL SYSTEM", GetName(), key );
    // key = pm.stkey
    sprintf( pbuf,
             "State variables: V(L)=%-.7g  P(bar)=%-.7g  T=%-.6g(K) =%-.6g(C)",
             pm.VXc/1e3, pm.Pc, pm.TCc, pm.Tc );
    prTab.PutString( pbuf );
    sprintf( pbuf,
             " Mass(kg)=%.10g Min.potential: G(x)=%-.8g(moles) =%-.7g(kJ)\n",
             pm.MBX, pm.FX, pm.FX*pm.RT/1e3 );
    prTab.PutString( pbuf );
    print_IC( prTab );
    print_DC( prTab, jb );
    print_PH( prTab );

    sprintf( pbuf, "\nTask %-62s", pm.stkey );
    prTab.PutString( pbuf );
    prTab.PutEnd();
    // Put resalts to object
    const char *tab= prTab.GetTable();
    tblen = strlen(tab)+1; /* Get size of text buf */
    pm.pbuf = (char *)aObj[ o_wd_txtb ].Alloc( 1, tblen, S_ );
    memcpy( pm.pbuf, tab, tblen );

    vfObjToFile(window(), &aObj[o_wd_txtb]);
    pm.pbuf  = (char *)aObj[ o_wd_txtb ].Free(); //
}

//----------------------------------------------------------------------------
ms_sysbc


#include "v_print.h"
/*--------------------------------------------------------*/
//Print table of IComp
void
TSyst::syIC_pr_tab( const char *key  )
{
    int i;
    size_t tblen;
    vstr pl(600), datb(32), timeb(32);

    SetString("Please, wait until arrays will be sampled");
    pVisor->Update();
    /*Table heard */
    TPrintTable prTab( "Comparison of System record", GetName(), key );
    prTab.PutString(
        "                                                         Table 1\n\n"
        "          Parameters of Independent Components (IC) \n"
        "|================================|==|==|=============|=============|\n"
        "| Independent Component          |  |  | Bulk        | Quantity/   |\n"
        "|                                |  |  | Composition | Concentrat. |\n"
        "|================================|==|==|=============|=============|");

    for( i=0; i<mup->N; i++ ) // print strin of table 1
    {
        gcvt( sy.B[i], 7, datb);
        gcvt( sy.BI[i], 6, timeb);
        sprintf( pl, " %-32.32s  %c  %c %-13.13s  %-12.12s ",
                 mup->SB[i], sy.Icl[i], sy.BIun[i], datb.p, timeb.p );
        prTab.PutString( pl );
    }
    prTab.PutString(
        "|================================|==|==|=============|=============|\n");

    if( mup->La > 0 && sy.PbAC != S_OFF )
    {
        prTab.PutString(
            "                                                         Table 2\n\n"
            "          Parameters of GCO (COMPOS)               \n"
            "|===============================================|==|==|=============|\n"
            "| Geochemical Composition Object (GCO) Rkey     |  |  | Quan./Conc. |\n"
            "|===============================================|==|==|=============|");
        for( i=0; i<mup->La; i++ ) // print gstring
        {
            gcvt( sy.XeA[i], 6, datb);
            sprintf( pl, " %-47.47s  %c  %c  %-12.12s ",
                     mup->SA[i], sy.Acl[i], sy.XAun[i], datb.p );
            prTab.PutString( pl );
        }
        prTab.PutString(
            "|===============================================|==|==|=============|\n");
    }
    prTab.PutEnd();
    // Put resalts to object
    const char *tab= prTab.GetTable();
    tblen = strlen(tab)+1; /* Get size of text buf */
    TProfil *aPa=(TProfil *)(&aMod[RT_PARAM]);
    aPa->pa.p.tprn= (char *)aObj[o_patprn].Alloc( 1, tblen, S_ );
    memcpy( aPa->pa.p.tprn, tab, tblen );

    vfObjToFile(window(), &aObj[o_patprn]);
}

int nOp[6] = {o_syxed, -o_sydul, -o_sydll, o_sygex,
              o_sylngmf, -o_symasdj, };
//Print table of DComp
void
TSyst::syDC_pr_tab( const char *key  )
{
    int i,j, nOend;
    unsigned long tblen;
    vstr pl(200);
    double xx;
    //, datb[10], timeb[10], xx[20], *wptr, *key;

    if( mup->L <= 0 ) return;
    for( i=0;i<6;i++)
        if( aObj[abs(nOp[i])].GetPtr() !=0 )
            nOend = i;

    SetString("Please, wait until arrays will be sampled");
    pVisor->Update();
    /*Table heard */
    TPrintTable prTab( "Comparison of System record", GetName(), key );
    prTab.PutString(
        "                                                         Table 3\n\n"
        "          PARAMETERS OF DEPENDENT COMPONENTS          ");
    prTab.PutString( "|=|================================|=|=|=", 41, false);
    if( sy.RLC ) prTab.PutString( "|==", 3, false );
    if( sy.RSC ) prTab.PutString( "|==", 3, false );
    for( i=0;i<6;i++)
        if( aObj[abs(nOp[i])].GetPtr() !=0 )
        {
            if( i != nOend )   prTab.PutTabHead( 5 );
            else   prTab.PutTabHead( 6 );
        }
    // head of table
    prTab.PutString( "| | Dependent Component            | | | ", 41, false);
    if( sy.RLC ) prTab.PutString( "|  ", 3, false );
    if( sy.RSC ) prTab.PutString( "|  ", 3, false );
    for( i=0;i<6;i++)
        if( aObj[abs(nOp[i])].GetPtr() !=0 )
        {
            if( i == nOend ) prTab.PutObjName( aObj[abs(nOp[i])].GetKeywd(), 3 );
            else prTab.PutObjName( aObj[abs(nOp[i])].GetKeywd(), 2 );
        }
    prTab.PutString( "|=|================================|=|=|=", 41, false);
    if( sy.RLC ) prTab.PutString( "|==", 3, false );
    if( sy.RSC ) prTab.PutString( "|==", 3, false );
    for( i=0;i<6;i++)
        if( aObj[abs(nOp[i])].GetPtr() !=0 )
        {
            if( i != nOend ) prTab.PutTabDelta( 8 );
            else   prTab.PutTabDelta( 9 );
        }
    for( j=0; j<mup->L; j++ )   // table 3 line
    {
        sprintf( pl, " %c %-32.32s %c %c %c", sy.Dcl[j], mup->SM[j] ,
                 mup->DCS[j], mup->DCC[j], sy.XDun[j] );
        prTab.PutString( pl, strlen(pl), false );
        if( sy.RLC )
        {
            sprintf( pl, " %c ", sy.RLC[j]);
            prTab.PutString( pl, 3, false );
        }
        if( sy.RSC )
        {
            sprintf( pl, " %c ", sy.RSC[j]);
            prTab.PutString( pl, 3, false );
        }
        for( i=0;i<6;i++)
            if( aObj[abs(nOp[i])].GetPtr() !=0 )
            {
                if( nOp[i]<0 && j >= mup->Ls ) xx= DOUBLE_EMPTY;
                else  xx = aObj[ abs(nOp[i]) ].Get( j, 0 );
                if( i == nOend )  prTab.PutFloat( xx, 7, 3 );
                else    prTab.PutFloat( xx, 7, 2 );
            }
    }
    prTab.PutString( "|=|================================|=|=|=", 41, false);
    if( sy.RLC ) prTab.PutString( "|==", 3, false );
    if( sy.RSC ) prTab.PutString( "|==", 3, false );
    for( i=0;i<6;i++)
        if( aObj[abs(nOp[i])].GetPtr() !=0 )
        {
            if( i != nOend ) prTab.PutTabEnd( 11 );
            else   prTab.PutTabEnd( 12 );
        }
    prTab.PutEnd();
    // Put resalts to object
    const char *tab= prTab.GetTable();
    tblen = strlen(tab)+1; /* Get size of text buf */
    TProfil *aPa=(TProfil *)(&aMod[RT_PARAM]);
    aPa->pa.p.tprn= (char *)aObj[o_patprn].Alloc( 1, tblen, S_ );
    memcpy( aPa->pa.p.tprn, tab, tblen );

    vfObjToFile(window(), &aObj[o_patprn]);
}

int nOf[8] = {o_syphm, o_sypul, o_sypll, o_syyof, o_syaalp,
              o_sysigm, o_syxr0h0, o_syxeps };
//Print table of DComp
void
TSyst::syPh_pr_tab( const char *key  )
{
    int i, j, nOend;
    unsigned long tblen;
    vstr pbuf(120);

    for( i=0;i<8;i++)
        if( aObj[nOf[i]].GetPtr()  != 0 )
            nOend = i;
    SetString("Please, wait until arrays will be sampled");
    pVisor->Update();
    /*Table heard */
    TPrintTable prTab( "Comparison of System record", GetName(), key );
    prTab.PutString(
        "                                                         Table 4\n\n"
        "          PARAMETERS OF PHASES        " );
    // Table head
    prTab.PutString( "|=|================================|===|=|===", 45, false);
    if( sy.RFLC ) prTab.PutString( "|==", 3, false );
    if( sy.RFSC ) prTab.PutString( "|==", 3, false );
    if( sy.XPun ) prTab.PutString( "|==", 3, false );
    for( i=0;i<8;i++)
        if( aObj[nOf[i]].GetPtr() !=0 )
        {
            if( i != nOend )   prTab.PutTabHead( 5 );
            else   prTab.PutTabHead( 6 );
        }
    // head of table
    prTab.PutString( "| | Phase                          |   | |   ", 45, false);
    if( sy.RFLC ) prTab.PutString( "|  ", 3, false );
    if( sy.RFSC ) prTab.PutString( "|  ", 3, false );
    if( sy.XPun ) prTab.PutString( "|  ", 3, false );
    for( i=0;i<8;i++)
        if( aObj[nOf[i]].GetPtr() !=0 )
        {
            if( i == nOend ) prTab.PutObjName( aObj[abs(nOp[i])].GetKeywd(), 3 );
            else prTab.PutObjName( aObj[abs(nOp[i])].GetKeywd(), 2 );
        }
    prTab.PutString( "|=|================================|===|=|===", 45, false);
    if( sy.RFLC ) prTab.PutString( "|==", 3, false );
    if( sy.RFSC ) prTab.PutString( "|==", 3, false );
    if( sy.XPun ) prTab.PutString( "|==", 3, false );
    for( i=0;i<8;i++)
        if( aObj[nOf[i]].GetPtr() !=0 )
        {
            if( i != nOend ) prTab.PutTabDelta( 8 );
            else   prTab.PutTabDelta( 9 );
        }
    for( j=0; j<mup->Fi; j++ ) // table gstring
    {
        sprintf( pbuf, " %c %-32.32s %3d %c %3d", sy.Pcl[j], mup->SF[j], sy.Ll[j],
                 mup->PHC[j], mup->Ll[j] );
        prTab.PutString( pbuf, strlen(pbuf), false );
        if( sy.RFLC )
        {
            sprintf( pbuf, " %c ", sy.RFLC[j]);
            prTab.PutString( pbuf, 3, false );
        }
        if( sy.RFSC )
        {
            sprintf( pbuf, " %c ", sy.RFSC[j]);
            prTab.PutString( pbuf, 3, false );
        }
        if( sy.XPun )
        {
            sprintf( pbuf, " %c ", sy.XPun[j]);
            prTab.PutString( pbuf, 3, false );
        }
        for( i=0;i<8;i++)
            if( aObj[nOf[i]].GetPtr() !=0 )
            {
                if( i==nOend ) prTab.PutFloat( aObj[ abs(nOf[i]) ].Get( j, 0 ), 7, 3 );
                else    prTab.PutFloat( aObj[ abs(nOf[i]) ].Get( j, 0 ), 7, 2 );
            }
    }
    prTab.PutString( "|=|================================|===|=|===", 45, false);
    if( sy.RFLC ) prTab.PutString( "|==", 3, false );
    if( sy.RFSC ) prTab.PutString( "|==", 3, false );
    if( sy.XPun ) prTab.PutString( "|==", 3, false );
    for( i=0;i<8;i++)
        if( aObj[nOf[i]].GetPtr() !=0 )
        {
            if( i != nOend ) prTab.PutTabEnd( 11 );
            else   prTab.PutTabEnd( 12 );
        }
    prTab.PutEnd();
    // Put resalts to object
    const char *tab= prTab.GetTable();
    tblen = strlen(tab)+1; /* Get size of text buf */
    TProfil *aPa=(TProfil *)(&aMod[RT_PARAM]);
    aPa->pa.p.tprn= (char *)aObj[o_patprn].Alloc( 1, tblen, S_ );
    memcpy( aPa->pa.p.tprn, tab, tblen );

    vfObjToFile(window(), &aObj[o_patprn]);
}

/*--------------------------------------------------------*/
//Print record
void
TSyst::RecordPrint( const char *key )
{
    if( vfQuestion(window(), GetName(),
                   "Print tables for IC and compositions?"))
        syIC_pr_tab( key );
    if( vfQuestion(window(), GetName(),
                   "Print tables for phases?"))
        syPh_pr_tab( key );
    if( vfQuestion(window(), GetName(),
                   "Print tables for DC?"))
        syDC_pr_tab( key );
}
//===========================================================================
// Sveta 16/11/2004


    // test
    void dyn_kill_test(MULTI& tes);
    void dyn_new_test(MULTI& tes);
    void Test_Eq( int size, double *p1, double *p2, const char *key );
    void Test_Eq( int size, float *p1, float *p2, const char *key );
    void Test_Eq( int size, short *p1, short *p2, const char *key );
    void dyn__test(MULTI& tes);
    MULTI copy1;
    bool flCopy;
    MULTI& GetPMcopy1()
    {
        return copy1;
    }


//====================================================================

// set old data dynamic memory
void TMulti::dyn_new_test(MULTI& tes)
{
  // Part 1
 if( flCopy == true )
   dyn_kill_test(tes);
 memset(&tes, 0, sizeof(MULTI));
 memcpy( &tes.N, &pm.N, 36*sizeof(short) );
 memcpy( &tes.TC, &pm.TC, 54*sizeof(double) );

 // tes.A = (float *)aObj[ o_wi_a].Alloc( tes.L, tes.N, F_ );
// tes.SB = (char (*)[MAXICNAME+MAXSYMB])aObj[ o_wd_sb].Alloc(
//    tes.N, 1, MAXICNAME+MAXSYMB );
// Error in free memory
//aObj[ o_w_sbh].SetPtr( tes.SB ); aObj[ o_w_sbh ].SetDim( tes.N, 1 );
// tes.SFs = (char (*)[MAXPHNAME+MAXSYMB])aObj[ o_wd_sfs].Alloc(
//    tes.FI, 1, MAXPHNAME+MAXSYMB );
// tes.SM = (char (*)[MAXDCNAME])aObj[ o_wd_sm].Alloc( tes.L, 1, MAXDCNAME );
// tes.SF = (char (*)[MAXPHNAME+MAXSYMB])aObj[ o_wd_sf].Alloc(
//     tes.FI, 1, MAXPHNAME+MAXSYMB );
// tes.SM2 = (char (*)[MAXDCNAME])aObj[ o_wd_sm2].Alloc( tes.Ls, 1, MAXDCNAME );
// tes.SF2 = (char (*)[MAXPHNAME+MAXSYMB])aObj[ o_wd_sf2].Alloc(
//     tes.FIs, 1, MAXPHNAME+MAXSYMB );

// tes.RLC = (char *)aObj[ o_wi_rlc].Alloc( tes.L, 1, A_ );
// tes.RSC = (char *)aObj[ o_wi_rsc].Alloc( tes.L, 1, A_ );
// tes.ICC = (char *)aObj[ o_wi_icc].Alloc( tes.N, 1, A_ );
// tes.DCC = (char *)aObj[ o_wi_dcc].Alloc( tes.L, 1, A_ );
// tes.PHC = (char *)aObj[ o_wi_phc].Alloc( tes.FI, 1, A_ );
// tes.DCCW = (char *)aObj[ o_wi_dccw].Alloc( tes.L, 1, A_ );

 // need  always to alloc vectors
 tes.L1 = new short[pm.FI];
 memcpy( tes.L1, pm.L1, pm.FI*sizeof(short) );
 tes.muk = new short[pm.FI];
 memcpy( tes.muk, pm.muk, pm.FI*sizeof(short) );
 tes.mui = new short[pm.N];
 memcpy( tes.mui, pm.mui, pm.N*sizeof(short) );
 tes.muj = new short[pm.L];
 memcpy( tes.muj, pm.muj, pm.L*sizeof(short) );

 tes.DUL = new double[pm.L];
 memcpy( tes.DUL, pm.DUL, pm.L*sizeof(double) );
 tes.DLL = new double[pm.L];
 memcpy( tes.DLL, pm.DLL, pm.L*sizeof(double) );
 tes.Vol = new double[pm.L];
 memcpy( tes.Vol, pm.Vol, pm.L*sizeof(double) );
 tes.Pparc = new double[pm.L];
 memcpy( tes.Pparc, pm.Pparc, pm.L*sizeof(double) );
 tes.MM = new double[pm.L];
 memcpy( tes.MM, pm.MM, pm.L*sizeof(double) );
 tes.Awt = new float[pm.N];
 memcpy( tes.Awt, pm.Awt, pm.N*sizeof(float) );
 tes.XFs = new float[pm.FI];
 memcpy( tes.XFs, pm.XFs, pm.FI*sizeof(float) );
 tes.Falps = new float[pm.FI];
 memcpy( tes.Falps, pm.Falps, pm.FI*sizeof(float) );
 tes.G = new double[pm.L];
 memcpy( tes.G, pm.G, pm.L*sizeof(double) );
 tes.G0 = new double[pm.L];
 memcpy( tes.G0, pm.G0, pm.L*sizeof(double) );
 tes.lnGam = new double[pm.L];
 memcpy( tes.lnGam, pm.lnGam, pm.L*sizeof(double) );
 tes.lnGmo = new double[pm.L];
 memcpy( tes.lnGmo, pm.lnGmo, pm.L*sizeof(double) );
 tes.B = new double[pm.N];
 memcpy( tes.B, pm.B, pm.N*sizeof(double) );
 tes.U = new double[pm.N];
 memcpy( tes.U, pm.U, pm.N*sizeof(double) );
 tes.U_r = new double[pm.N];
 memcpy( tes.U_r, pm.U_r, pm.N*sizeof(double) );
 tes.C = new double[pm.N];
 memcpy( tes.C, pm.C, pm.N*sizeof(double) );
 tes.XF = new double[pm.FI];
 memcpy( tes.XF, pm.XF, pm.FI*sizeof(double) );
 tes.YF = new double[pm.FI];
 memcpy( tes.YF, pm.YF, pm.FI*sizeof(double) );
 tes.Falp = new double[pm.FI];
 memcpy( tes.Falp, pm.Falp, pm.FI*sizeof(double) );
 tes.X = new double[pm.L];
 memcpy( tes.X, pm.X, pm.L*sizeof(double) );
 tes.Y = new double[pm.L];
 memcpy( tes.Y, pm.Y, pm.L*sizeof(double) );
 tes.XY = new double[pm.L];
 memcpy( tes.XY, pm.XY, pm.L*sizeof(double) );
 tes.MU = new double[pm.L];
 memcpy( tes.MU, pm.MU, pm.L*sizeof(double) );
 tes.EMU = new double[pm.L];
 memcpy( tes.EMU, pm.EMU, pm.L*sizeof(double) );
 tes.NMU = new double[pm.L];
 memcpy( tes.NMU, pm.NMU, pm.L*sizeof(double) );
 tes.W = new double[pm.L];
 memcpy( tes.W, pm.W, pm.L*sizeof(double) );
 tes.F = new double[pm.L];
 memcpy( tes.F, pm.F, pm.L*sizeof(double) );
 tes.F0 = new double[pm.L];
 memcpy( tes.F0, pm.F0, pm.L*sizeof(double) );
 tes.YOF = new double[pm.FI];
 memcpy( tes.YOF, pm.YOF, pm.FI*sizeof(double) );
 tes.lnGmM = new double[pm.L];
 memcpy( tes.lnGmM, pm.lnGmM, pm.L*sizeof(double) );
 tes.GEX = new double[pm.L];
 memcpy( tes.GEX, pm.GEX, pm.L*sizeof(double) );
 tes.FVOL = new double[pm.FI];
 memcpy( tes.FVOL, pm.FVOL, pm.FI*sizeof(double) );
 tes.FWGT = new double[pm.FI];
 memcpy( tes.FWGT, pm.FWGT, pm.FI*sizeof(double) );

 if( tes.L > 0 )
 {
   tes.Y_la = new double[pm.L];
   memcpy( tes.Y_la, pm.Y_la, pm.L*sizeof(double) );
   tes.Y_w = new double[pm.L];
   memcpy( tes.Y_w, pm.Y_w, pm.L*sizeof(double) );
   tes.Fx = new double[pm.L];
   memcpy( tes.Fx, pm.Fx, pm.L*sizeof(double) );
   tes.Wx = new double[pm.L];
   memcpy( tes.Wx, pm.Wx, pm.L*sizeof(double) );
   tes.VL = new float[pm.L];
   memcpy( tes.VL, pm.VL, pm.L*sizeof(float) );
   tes.Gamma = new double[pm.L];
   memcpy( tes.Gamma, pm.Gamma, pm.L*sizeof(double) );
   tes.lnGmf = new double[pm.L];
   memcpy( tes.lnGmf, pm.lnGmf, pm.L*sizeof(double) );
   tes.D = new double[MST][MST];
   memcpy( tes.D, pm.D, MST*MST );
 }

   // Part 2  not requited arrays

 if( tes.FIs > 0 && tes.Ls > 0 )
 {
   tes.BF = new double[pm.FIs*pm.N];
   memcpy( tes.BF, pm.BF, pm.FIs*pm.N*sizeof(double) );
   tes.XFA = new double[pm.FIs];
   memcpy( tes.XFA, pm.XFA, pm.FIs*sizeof(double) );
   tes.YFA = new double[pm.FIs];
   memcpy( tes.YFA, pm.YFA, pm.FIs*sizeof(double) );
//     tes.LsMod = (short *)aObj[ o_wi_lsmod].Alloc( tes.FIs, 1, I_ );
//     tes.LsMdc = (short *)aObj[ o_wi_lsmdc].Alloc( tes.FIs, 1, I_ );
//    PMs and DMS may be realloced after load arrays LsMod and LsMdc
//   tes.PMc = new float[pm.FIs];
//   memcpy( tes.PMc, pm.PMc, pm.FIs*sizeof(float) );
//   tes.DMc = new float[pm.Ls];
//   memcpy( tes.DMc, pm.DMc, pm.Ls*sizeof(float) );
   tes.PUL = new double[pm.FIs];
   memcpy( tes.PUL, pm.PUL, pm.FIs*sizeof(double) );
   tes.PLL = new double[pm.FIs];
   memcpy( tes.PLL, pm.PLL, pm.FIs*sizeof(double) );
//     tes.YOF = (float *)aObj[ o_wi_yof].Alloc( tes.FI, 1, F_ );
//     tes.sMod = (char (*)[6])aObj[ o_wi_smod].Alloc( tes.FIs, 1, 6 );
//     tes.RFLC = (char *)aObj[ o_wi_rflc].Alloc( tes.FIs, 1, A_ );
//     tes.RFSC = (char *)aObj[ o_wi_rfsc].Alloc( tes.FIs, 1, A_ );
 }

 if( tes.LO > 1 )
 {
   tes.Y_m = new double[pm.L];
   memcpy( tes.Y_m, pm.Y_m, pm.L*sizeof(double) );
   tes.IC_m = new double[pm.N];
   memcpy( tes.IC_m, pm.IC_m, pm.N*sizeof(double) );
   tes.IC_lm = new double[pm.N];
   memcpy( tes.IC_lm, pm.IC_lm, pm.N*sizeof(double) );
   tes.IC_wm = new double[pm.N];
   memcpy( tes.IC_wm, pm.IC_wm, pm.N*sizeof(double) );
 }
 // dispersion and sorbtion phases
 if( PAalp_ != S_OFF )
 {
   tes.Aalp = new float[pm.FI];
   memcpy( tes.Aalp, pm.Aalp, pm.FI*sizeof(float) );
   tes.Xr0h0 = new float[pm.FI][2];
   memcpy( tes.Xr0h0, pm.Xr0h0, pm.FI*2*sizeof(float) );
 }
 if( PSigm_ != S_OFF )
 {    tes.Sigw = new float[pm.FI];
     memcpy( tes.Sigw, pm.Sigw, pm.FI*sizeof(float) );
 }
 if( PSigm_ != S_OFF )
 {    tes.Sigg = new float[pm.FI];
     memcpy( tes.Sigg, pm.Sigg, pm.FI*sizeof(float) );
 }
 if( tes.E )
 {
    tes.EZ = new double[pm.L];
     memcpy( tes.EZ, pm.EZ, pm.L*sizeof(double) );
    tes.Xcond = new float[pm.FI];
     memcpy( tes.Xcond, pm.Xcond, pm.FI*sizeof(float) );
    tes.Xeps = new float[pm.FI];
     memcpy( tes.Xeps, pm.Xeps, pm.FI*sizeof(float) );
 }

 if( tes.FIat > 0 && tes.Lads > 0 && tes.FIs > 0 )
 { // ADSORBTION AND ION EXCHANGE
    tes.SATX = new short[pm.Lads][4];
    memcpy( tes.SATX, pm.SATX, pm.Lads*4*sizeof(short) );
//   tes.SCM  = (char (*)[MST])aObj[ o_wi_scm].Alloc( tes.FIs, tes.FIat, A_ );
//   memset( tes.SCM, SC_NOT_USED, tes.FIs*tes.FIat );
    tes.Nfsp = new float[pm.FIs][MST];
     memcpy( tes.Nfsp, pm.Nfsp, pm.FIs*MST*sizeof(float) );
    tes.MASDT = new float[pm.FIs][MST];
     memcpy( tes.MASDT, pm.MASDT, pm.FIs*MST*sizeof(float) );
    tes.XcapA = new float[pm.FIs][MST];
     memcpy( tes.XcapA, pm.XcapA, pm.FIs*MST*sizeof(float) );
    tes.XcapB = new float[pm.FIs][MST];
     memcpy( tes.XcapB, pm.XcapB, pm.FIs*MST*sizeof(float) );
    tes.XcapD = new float[pm.FIs][MST];
     memcpy( tes.XcapD, pm.XcapD, pm.FIs*MST*sizeof(float) );
    tes.XcapF = new float[pm.FIs][MST];
     memcpy( tes.XcapF, pm.XcapF, pm.FIs*MST*sizeof(float) );
    tes.XdlA = new float[pm.FIs][MST];
     memcpy( tes.XdlA, pm.XdlA, pm.FIs*MST*sizeof(float) );
    tes.XdlB = new float[pm.FIs][MST];
     memcpy( tes.XdlB, pm.XdlB, pm.FIs*MST*sizeof(float) );
    tes.XdlD = new float[pm.FIs][MST];
     memcpy( tes.XdlD, pm.XdlD, pm.FIs*MST*sizeof(float) );
    tes.XpsiA = new double[pm.FIs][MST];
     memcpy( tes.XpsiA, pm.XpsiA, pm.FIs*MST*sizeof(double) );
    tes.XpsiB = new double[pm.FIs][MST];
     memcpy( tes.XpsiB, pm.XpsiB, pm.FIs*MST*sizeof(double) );
    tes.XpsiD = new double[pm.FIs][MST];
     memcpy( tes.XpsiD, pm.XpsiD, pm.FIs*MST*sizeof(double) );
    tes.XlamA = new float[pm.FIs][MST];
     memcpy( tes.XlamA, pm.XlamA, pm.FIs*MST*sizeof(float) );
    tes.Xetaf = new float[pm.FIs][MST];
     memcpy( tes.Xetaf, pm.Xetaf, pm.FIs*MST*sizeof(float) );
    tes.MASDJ = new float[pm.Lads][DFCN];
     memcpy( tes.MASDJ, pm.MASDJ, pm.Lads*DFCN*sizeof(float) );
//    tes.MASDJ = new float[pm.Ls];
//     memcpy( tes.MASDJ, pm.MASDJ, pm.Ls*sizeof(float) );
    tes.XetaA = new double[pm.FIs][MST];
     memcpy( tes.XetaA, pm.XetaA, pm.FIs*MST*sizeof(double) );
    tes.XetaB = new double[pm.FIs][MST];
     memcpy( tes.XetaB, pm.XetaB, pm.FIs*MST*sizeof(double) );
    tes.XFTS = new double[pm.FIs][MST];
     memcpy( tes.XFTS, pm.XFTS, pm.FIs*MST*sizeof(double) );
    tes.lnSAC = new double[pm.Lads][4];
     memcpy( tes.lnSAC, pm.lnSAC, pm.Lads*4*sizeof(double) );
//   tes.SATT = (char *)aObj[ o_wi_satt].Alloc( tes.Ls, 1, A_ );
 }

 if( tes.PG > 0 )
 {
//   tes.Fug = (float *)aObj[ o_wd_fug].Alloc( tes.PG, 1, F_ );
//   tes.Fug_l = (float *)aObj[ o_wd_fugl].Alloc( tes.PG, 1, F_ );
//   tes.Ppg_l = (float *)aObj[ o_wd_ppgl].Alloc( tes.PG, 1, F_ );
 }

   // Part 3

 if( tes.Ls > 1 && tes.FIs > 0 )
 {
    tes.Wb = new float[pm.Ls];
     memcpy( tes.Wb, pm.Wb, pm.Ls*sizeof(float) );
    tes.Wabs = new float[pm.Ls];
     memcpy( tes.Wabs, pm.Wabs, pm.Ls*sizeof(float) );
    tes.Rion = new float[pm.Ls];
     memcpy( tes.Rion, pm.Rion, pm.Ls*sizeof(float) );
//   tes.Qp = (double *)aObj[ o_wi_qp].Alloc( tes.FIs, 20, D_ );
//     tes.Qd = (double *)aObj[ o_wi_qd].Alloc( 1, 20, D_ );

 }
  flCopy = true;
// tes.R = (float *)aObj[ o_w_r].Alloc( tes.N, tes.N+1, D_ );

}
// free dynamic memory in objects and values
void TMulti::dyn_kill_test(MULTI& tes)
{
// memcpy( &pm.PunE, "jjbC", 4 );
// memset( &pm.N, 0, 36*sizeof(short));
// memset( &pm.TC, 0, 54*sizeof(double));
 delete[] tes.L1; /* delete[] tes.LsMod;   delete[] tes.LsMdc; */
 delete[] tes.mui;    delete[] tes.muk  ;    delete[] tes.muj  ;
 delete[] tes.SATX;    delete[] tes.DUL  ;    delete[] tes.DLL  ;
 delete[] tes.GEX;    delete[] tes.PUL  ;    delete[] tes.PLL  ;
 delete[] tes.YOF; /*  delete[] tes.PMc  ;    delete[] tes.DMc  ; */
 delete[] tes.Vol;    delete[] tes.HYM  ;    delete[] tes.VL   ;
 delete[] tes.MM ;    delete[] tes.H0   ;    delete[] tes.A0   ;
 delete[] tes.U0;    delete[] tes.S0   ;    delete[] tes.Cp0  ;
 delete[] tes.Cv0;    delete[] tes.Pparc;    delete[] tes.Y_m  ;
 delete[] tes.Y_la;    delete[] tes.Y_w  ;    delete[] tes.Gamma;
 delete[] tes.lnGmf;    delete[] tes.lnGmM;    delete[] tes.EZ   ;
 delete[] tes.Wb;    delete[] tes.Wabs ;    delete[] tes.Rion ;
 delete[] tes.Aalp;    delete[] tes.Sigw ;    delete[] tes.Sigg ;
 delete[] tes.Nfsp;    delete[] tes.MASDT;    delete[] tes.FVOL ;
 delete[] tes.FWGT;    delete[] tes.XcapA;    delete[] tes.XcapB;
 delete[] tes.XcapD;    delete[] tes.XdlA ;    delete[] tes.XdlB ;
 delete[] tes.XdlD;    delete[] tes.XpsiA;    delete[] tes.XpsiB;
 delete[] tes.XpsiD;    delete[] tes.Xr0h0;    delete[] tes.XlamA;
 delete[] tes.Xetaf;    delete[] tes.Xcond;    delete[] tes.Xeps ;
 delete[] tes.Awt;    delete[] tes.A    ;    delete[] tes.XFs  ;
 delete[] tes.Falps;    delete[] tes.Fug  ;    delete[] tes.Fug_l;
 delete[] tes.Ppg_l;    delete[] tes.XFTS ;    delete[] tes.MASDJ;
 delete[] tes.G;    delete[] tes.G0   ;    delete[] tes.lnGam;
 delete[] tes.lnGmo;    delete[] tes.lnSAC;    delete[] tes.B    ;
 delete[] tes.U;    delete[] tes.U_r  ;    delete[] tes.C    ;
 delete[] tes.IC_m;    delete[] tes.IC_lm;    delete[] tes.IC_wm;
 delete[] tes.BF;    delete[] tes.XF   ;    delete[] tes.YF   ;
 delete[] tes.XFA;    delete[] tes.YFA  ;    delete[] tes.Falp ;
 delete[] tes.XetaA;    delete[] tes.XetaB;    delete[] tes.X    ;
 delete[] tes.Y;    delete[] tes.XY   ;    delete[] tes.Qp   ;
 delete[] tes.Qd;    delete[] tes.MU   ;    delete[] tes.EMU  ;
 delete[] tes.NMU;    delete[] tes.W    ;    delete[] tes.Fx   ;
 delete[] tes.Wx;    delete[] tes.F    ;    delete[] tes.F0   ;
 delete[] tes.D;    delete[] tes.R    ;    delete[] tes.R1   ;
 /* delete[] tes.sMod; */   delete[] tes.SB   ;    delete[] tes.SM   ;
 delete[] tes.SF;    delete[] tes.SFs  ;    delete[] tes.pbuf ; //
 delete[] tes.RLC;    delete[] tes.RSC  ;   /* delete[] tes.RFLC ;
 delete[] tes.RFSC;  */  delete[] tes.ICC  ;    delete[] tes.DCC  ;
 delete[] tes.PHC;    delete[] tes.SCM  ;  /*  delete[] tes.SATT ; */
 delete[] tes.DCCW;    delete[] tes.XcapF;    delete[] tes.SM2   ;
 delete[] tes.SF2;

}

void TMulti::Test_Eq( int size, double *p1, double *p2, const char *key )
{
  vstr buf(150);
  for( int i=0; i<size; i++ )
    if( fabs(p1[i]-p2[i]) > fabs(p1[i])*1e-7 )
    {
       sprintf(buf, "%7s[%3d]:  %12.8g  %12.8g", key, i, p1[i], p2[i]);
//#ifndef IPMGEMPLUGIN
//       vfMessage( window(), "Double ", buf.p );
//#endif
//       std::cout << "Double " << buf.p << endl;
//       break;
    }
}

void TMulti::Test_Eq( int size, float *p1, float *p2, const char *key )
{
  vstr buf(150);
  for( int i=0; i<size; i++ )
    if( p1[i] != p2[i] )
    {
       sprintf(buf, "%7s[%3d]:  %12.8g  %12.8g", key, i, p1[i], p2[i]);
//#ifndef IPMGEMPLUGIN
//       vfMessage(window(), "Float ", buf.p);
//#endif
//       cout << "Float  " << buf.p << endl;
//       break;
    }
}

void TMulti::Test_Eq( int size, short *p1, short *p2, const char *key )
{
  vstr buf(150);
  for( int i=0; i<size; i++ )
    if( p1[i] != p2[i] )
    {
       sprintf(buf, "%7s[%3d]:  %12d  %12d", key, i, p1[i], p2[i]);
//#ifndef IPMGEMPLUGIN
//       vfMessage(window(), "Float ", buf.p);
//#endif
//       cout << "Short  " << buf.p << endl;
//       break;
    }
}

// Test by comparing old and new dynamic memory of the MULTI structure
void TMulti::dyn__test(MULTI& tes)
{
// cout << "Test  begin:     previous     current" << endl;
 Test_Eq( 36, &tes.N, &pm.N, "IFlags" );
 Test_Eq( 54, &tes.TC, &pm.TC, "Scalars" );
 Test_Eq( pm.L, tes.DUL, pm.DUL, "DUL" );
 Test_Eq( pm.L, tes.DLL, pm.DLL, "DLL" );
 Test_Eq( pm.L, tes.Vol, pm.Vol, "Vol" );
 Test_Eq( pm.L, tes.Pparc, pm.Pparc, "Pparc" );
 Test_Eq( pm.L, tes.MM, pm.MM, "MM" );
 Test_Eq( pm.N, tes.Awt, pm.Awt, "Awt" );
// Test_Eq( pm.FI, tes.XFs, pm.XFs, "XFs" );
// Test_Eq( pm.FI, tes.Falps, pm.Falps, "Falps" );
 Test_Eq( pm.L, tes.G, pm.G, "G" );
 Test_Eq( pm.L, tes.G0, pm.G0, "G0" );
 Test_Eq( pm.L, tes.lnGam, pm.lnGam, "lnGam" );
 Test_Eq( pm.L, tes.lnGmo, pm.lnGmo, "lnGmo" );
 Test_Eq( pm.N, tes.B, pm.B, "B" );
 Test_Eq( pm.N, tes.U, pm.U, "U" );
 Test_Eq( pm.N, tes.U_r, pm.U_r, "U_r" );
 Test_Eq( pm.N, tes.C, pm.C, "C" );
 Test_Eq( pm.FI, tes.XF, pm.XF, "XF" );
 Test_Eq( pm.FI, tes.YF, pm.YF, "YF" );
 Test_Eq( pm.FI, tes.Falp, pm.Falp, "Falp" );
 Test_Eq( pm.L, tes.X, pm.X, "X" );
 Test_Eq( pm.L, tes.Y, pm.Y, "Y" );
 Test_Eq( pm.L, tes.XY, pm.XY, "XY" );
 Test_Eq( pm.L, tes.MU, pm.MU, "MU" );
 Test_Eq( pm.L, tes.EMU, pm.EMU, "EMU" );
 Test_Eq( pm.L, tes.NMU, pm.NMU, "NMU" );
 Test_Eq( pm.L, tes.W, pm.W, "W" );
 Test_Eq( pm.L, tes.F, pm.F, "F" );
 Test_Eq( pm.L, tes.F0, pm.F0, "F0" );
 Test_Eq( pm.FI, tes.YOF, pm.YOF, "YOF" );
 Test_Eq( pm.FI, tes.FVOL, pm.FVOL, "FVOL" );
 Test_Eq( pm.FI, tes.FWGT, pm.FWGT, "FWGT" );
 Test_Eq( pm.L, tes.lnGmM, pm.lnGmM, "lnGmM" );
 Test_Eq( pm.L, tes.GEX, pm.GEX, "GEX" );

 if( tes.L > 0 )
 {
 Test_Eq( pm.L, tes.Y_la, pm.Y_la, "Y_la" );
 Test_Eq( pm.L, tes.Y_w, pm.Y_w, "Y_w" );
 Test_Eq( pm.L, tes.Fx, pm.Fx, "Fx" );
 Test_Eq( pm.L, tes.Wx, pm.Wx, "Wx" );
 Test_Eq( pm.L, tes.VL, pm.VL, "VL" );
 Test_Eq( pm.L, tes.Gamma, pm.Gamma, "Gamma" );
 Test_Eq( pm.L, tes.lnGmf, pm.lnGmf, "lnGmf" );
// Test_Eq( pm.MST*pm.MST, &tes.D[0][0], &pm.D[0][0], "D" );
 }

   // Part 2  not required arrays

 if( tes.FIs > 0 && tes.Ls > 0 )
 {
 Test_Eq( pm.FIs*pm.N, tes.BF, pm.BF, "BF" );
 Test_Eq( pm.FIs, tes.XFA, pm.XFA, "XFA" );
 Test_Eq( pm.FIs, tes.YFA, pm.YFA, "YFA" );
// Test_Eq( pm.FIs, tes.PMc, pm.PMc, "PMc" ); /* not allocated ! */
// Test_Eq( pm.FIs, tes.DMc, pm.DMc, "DMc" ); /* not allocated ! */
// Test_Eq( pm.FIs, tes.PUL, pm.PUL, "PUL" );
// Test_Eq( pm.FIs, tes.PLL, pm.PLL, "PLL" );
}

 if( tes.LO > 1 )
 {
 Test_Eq( pm.L, tes.Y_m, pm.Y_m, "Y_m" );
 Test_Eq( pm.N, tes.IC_m, pm.IC_m, "IC_m" );
 Test_Eq( pm.N, tes.IC_lm, pm.IC_lm, "IC_lm" );
 Test_Eq( pm.N, tes.IC_wm, pm.IC_wm, "IC_wm" );
 }
 // sorption phases
 if( PAalp_ != S_OFF )
 {
 Test_Eq( pm.FI, tes.Aalp, pm.Aalp, "Aalp" );
// Test_Eq( pm.FI*2, &tes.Xr0h0[0][0], &pm.Xr0h0[0][0], "Xr0h0" );
 }
 if( PSigm_ != S_OFF )
   Test_Eq( pm.FI, tes.Sigw, pm.Sigw, "Sigw" );
 if( PSigm_ != S_OFF )
   Test_Eq( pm.FI, tes.Sigg, pm.Sigg, "Sigg" );
 if( tes.E )
 {
 Test_Eq( pm.L, tes.EZ, pm.EZ, "EZ" );
 Test_Eq( pm.FI, tes.Xcond, pm.Xcond, "Xcond" );
 Test_Eq( pm.FI, tes.Xeps, pm.Xeps, "Xeps" );
 }

 if( tes.FIat > 0 && tes.Lads > 0 && tes.FIs > 0 )
 {

 Test_Eq( pm.Lads*4, &tes.SATX[0][0], &pm.SATX[0][0], "SATX" );
 Test_Eq( pm.FIs*MST, &tes.Nfsp[0][0], &pm.Nfsp[0][0], "Nfsp" );
 Test_Eq( pm.FIs*MST, &tes.MASDT[0][0], &pm.MASDT[0][0], "MASDT" );
 Test_Eq( pm.FIs*MST, &tes.XcapA[0][0], &pm.XcapA[0][0], "XcapA" );
 Test_Eq( pm.FIs*MST, &tes.XcapB[0][0], &pm.XcapB[0][0], "XcapB" );
 Test_Eq( pm.FIs*MST, &tes.XcapD[0][0], &pm.XcapD[0][0], "XcapD" );
 Test_Eq( pm.FIs*MST, &tes.XcapF[0][0], &pm.XcapF[0][0], "XcapF" );
 Test_Eq( pm.FIs*MST, &tes.XdlA[0][0], &pm.XdlA[0][0], "XdlA" );
 Test_Eq( pm.FIs*MST, &tes.XdlB[0][0], &pm.XdlB[0][0], "XdlB" );
 Test_Eq( pm.FIs*MST, &tes.XdlD[0][0], &pm.XdlD[0][0], "XdlD" );
 Test_Eq( pm.FIs*MST, &tes.XpsiA[0][0], &pm.XpsiA[0][0], "XpsiA" );
 Test_Eq( pm.FIs*MST, &tes.XpsiB[0][0], &pm.XpsiB[0][0], "XpsiB" );
 Test_Eq( pm.FIs*MST, &tes.XpsiD[0][0], &pm.XpsiD[0][0], "XpsiD" );
 Test_Eq( pm.FIs*MST, &tes.XlamA[0][0], &pm.XlamA[0][0], "XlamA" );
 Test_Eq( pm.FIs*MST, &tes.Xetaf[0][0], &pm.Xetaf[0][0], "Xetaf" );
 Test_Eq( pm.FIs*MST, &tes.XpsiA[0][0], &pm.XpsiA[0][0], "XpsiA" );
 Test_Eq( pm.Lads*DFCN, &tes.MASDJ[0][0], &pm.MASDJ[0][0], "MASDJ" );
// Test_Eq( pm.Ls, tes.MASDJ, pm.MASDJ, "MASDJ" );
 Test_Eq( pm.FIs*MST, &tes.XetaA[0][0], &pm.XetaA[0][0], "XetaA" );
 Test_Eq( pm.FIs*MST, &tes.XetaB[0][0], &pm.XetaB[0][0], "XetaB" );
 Test_Eq( pm.FIs*MST, &tes.XFTS[0][0], &pm.XFTS[0][0], "XFTS" );
// Test_Eq( pm.Ls, tes.lnSAT, pm.lnSAT, "lnSAT" );
 }

 if( tes.PG > 0 )
 {
//   tes.Fug = (float *)aObj[ o_wd_fug].Alloc( tes.PG, 1, F_ );
//   tes.Fug_l = (float *)aObj[ o_wd_fugl].Alloc( tes.PG, 1, F_ );
//   tes.Ppg_l = (float *)aObj[ o_wd_ppgl].Alloc( tes.PG, 1, F_ );
 }

   // Part 3

// if( tes.Ls > 1 && tes.FIs > 0 )
// {
// Test_Eq( pm.Ls, tes.Wb, pm.Wb, "Wb" );
// Test_Eq( pm.Ls, tes.Wabs, pm.Wabs, "Wabs" );
// Test_Eq( pm.Ls, tes.Rion, pm.Rion, "Rion" );
// }
//  cout << "Test sequence end: " << endl;
}

//==============================================================================


// set old data dynamic memory
void TMulti::dyn_new_test(MULTI& tes)
{
  // Part 1
 if( flCopy == true )
   dyn_kill_test(tes);
 memset(&tes, 0, sizeof(MULTI));
 memcpy( &tes.N, &pm.N, 36*sizeof(short) );
 memcpy( &tes.TC, &pm.TC, 54*sizeof(double) );

 // tes.A = (float *)aObj[ o_wi_a].Alloc( tes.L, tes.N, F_ );
// tes.SB = (char (*)[MAXICNAME+MAXSYMB])aObj[ o_wd_sb].Alloc(
//    tes.N, 1, MAXICNAME+MAXSYMB );
// Error in free memory
//aObj[ o_w_sbh].SetPtr( tes.SB ); aObj[ o_w_sbh ].SetDim( tes.N, 1 );
// tes.SFs = (char (*)[MAXPHNAME+MAXSYMB])aObj[ o_wd_sfs].Alloc(
//    tes.FI, 1, MAXPHNAME+MAXSYMB );
// tes.SM = (char (*)[MAXDCNAME])aObj[ o_wd_sm].Alloc( tes.L, 1, MAXDCNAME );
// tes.SF = (char (*)[MAXPHNAME+MAXSYMB])aObj[ o_wd_sf].Alloc(
//     tes.FI, 1, MAXPHNAME+MAXSYMB );
// tes.SM2 = (char (*)[MAXDCNAME])aObj[ o_wd_sm2].Alloc( tes.Ls, 1, MAXDCNAME );
// tes.SF2 = (char (*)[MAXPHNAME+MAXSYMB])aObj[ o_wd_sf2].Alloc(
//     tes.FIs, 1, MAXPHNAME+MAXSYMB );

// tes.RLC = (char *)aObj[ o_wi_rlc].Alloc( tes.L, 1, A_ );
// tes.RSC = (char *)aObj[ o_wi_rsc].Alloc( tes.L, 1, A_ );
// tes.ICC = (char *)aObj[ o_wi_icc].Alloc( tes.N, 1, A_ );
// tes.DCC = (char *)aObj[ o_wi_dcc].Alloc( tes.L, 1, A_ );
// tes.PHC = (char *)aObj[ o_wi_phc].Alloc( tes.FI, 1, A_ );
// tes.DCCW = (char *)aObj[ o_wi_dccw].Alloc( tes.L, 1, A_ );

 // need  always to alloc vectors
 tes.L1 = new short[pm.FI];
 memcpy( tes.L1, pm.L1, pm.FI*sizeof(short) );
 tes.muk = new short[pm.FI];
 memcpy( tes.muk, pm.muk, pm.FI*sizeof(short) );
 tes.mui = new short[pm.N];
 memcpy( tes.mui, pm.mui, pm.N*sizeof(short) );
 tes.muj = new short[pm.L];
 memcpy( tes.muj, pm.muj, pm.L*sizeof(short) );

 tes.DUL = new double[pm.L];
 memcpy( tes.DUL, pm.DUL, pm.L*sizeof(double) );
 tes.DLL = new double[pm.L];
 memcpy( tes.DLL, pm.DLL, pm.L*sizeof(double) );
 tes.Vol = new double[pm.L];
 memcpy( tes.Vol, pm.Vol, pm.L*sizeof(double) );
 tes.Pparc = new double[pm.L];
 memcpy( tes.Pparc, pm.Pparc, pm.L*sizeof(double) );
 tes.MM = new double[pm.L];
 memcpy( tes.MM, pm.MM, pm.L*sizeof(double) );
 tes.Awt = new float[pm.N];
 memcpy( tes.Awt, pm.Awt, pm.N*sizeof(float) );
 tes.XFs = new float[pm.FI];
 memcpy( tes.XFs, pm.XFs, pm.FI*sizeof(float) );
 tes.Falps = new float[pm.FI];
 memcpy( tes.Falps, pm.Falps, pm.FI*sizeof(float) );
 tes.G = new double[pm.L];
 memcpy( tes.G, pm.G, pm.L*sizeof(double) );
 tes.G0 = new double[pm.L];
 memcpy( tes.G0, pm.G0, pm.L*sizeof(double) );
 tes.lnGam = new double[pm.L];
 memcpy( tes.lnGam, pm.lnGam, pm.L*sizeof(double) );
 tes.lnGmo = new double[pm.L];
 memcpy( tes.lnGmo, pm.lnGmo, pm.L*sizeof(double) );
 tes.B = new double[pm.N];
 memcpy( tes.B, pm.B, pm.N*sizeof(double) );
 tes.U = new double[pm.N];
 memcpy( tes.U, pm.U, pm.N*sizeof(double) );
 tes.U_r = new double[pm.N];
 memcpy( tes.U_r, pm.U_r, pm.N*sizeof(double) );
 tes.C = new double[pm.N];
 memcpy( tes.C, pm.C, pm.N*sizeof(double) );
 tes.XF = new double[pm.FI];
 memcpy( tes.XF, pm.XF, pm.FI*sizeof(double) );
 tes.YF = new double[pm.FI];
 memcpy( tes.YF, pm.YF, pm.FI*sizeof(double) );
 tes.Falp = new double[pm.FI];
 memcpy( tes.Falp, pm.Falp, pm.FI*sizeof(double) );
 tes.X = new double[pm.L];
 memcpy( tes.X, pm.X, pm.L*sizeof(double) );
 tes.Y = new double[pm.L];
 memcpy( tes.Y, pm.Y, pm.L*sizeof(double) );
 tes.XY = new double[pm.L];
 memcpy( tes.XY, pm.XY, pm.L*sizeof(double) );
 tes.MU = new double[pm.L];
 memcpy( tes.MU, pm.MU, pm.L*sizeof(double) );
 tes.EMU = new double[pm.L];
 memcpy( tes.EMU, pm.EMU, pm.L*sizeof(double) );
 tes.NMU = new double[pm.L];
 memcpy( tes.NMU, pm.NMU, pm.L*sizeof(double) );
 tes.W = new double[pm.L];
 memcpy( tes.W, pm.W, pm.L*sizeof(double) );
 tes.F = new double[pm.L];
 memcpy( tes.F, pm.F, pm.L*sizeof(double) );
 tes.F0 = new double[pm.L];
 memcpy( tes.F0, pm.F0, pm.L*sizeof(double) );
 tes.YOF = new double[pm.FI];
 memcpy( tes.YOF, pm.YOF, pm.FI*sizeof(double) );
 tes.lnGmM = new double[pm.L];
 memcpy( tes.lnGmM, pm.lnGmM, pm.L*sizeof(double) );
 tes.GEX = new double[pm.L];
 memcpy( tes.GEX, pm.GEX, pm.L*sizeof(double) );
 tes.FVOL = new double[pm.FI];
 memcpy( tes.FVOL, pm.FVOL, pm.FI*sizeof(double) );
 tes.FWGT = new double[pm.FI];
 memcpy( tes.FWGT, pm.FWGT, pm.FI*sizeof(double) );

 if( tes.L > 0 )
 {
   tes.Y_la = new double[pm.L];
   memcpy( tes.Y_la, pm.Y_la, pm.L*sizeof(double) );
   tes.Y_w = new double[pm.L];
   memcpy( tes.Y_w, pm.Y_w, pm.L*sizeof(double) );
   tes.Fx = new double[pm.L];
   memcpy( tes.Fx, pm.Fx, pm.L*sizeof(double) );
   tes.Wx = new double[pm.L];
   memcpy( tes.Wx, pm.Wx, pm.L*sizeof(double) );
   tes.VL = new float[pm.L];
   memcpy( tes.VL, pm.VL, pm.L*sizeof(float) );
   tes.Gamma = new double[pm.L];
   memcpy( tes.Gamma, pm.Gamma, pm.L*sizeof(double) );
   tes.lnGmf = new double[pm.L];
   memcpy( tes.lnGmf, pm.lnGmf, pm.L*sizeof(double) );
   tes.D = new double[MST][MST];
   memcpy( tes.D, pm.D, MST*MST*sizeof(double) );
 }

   // Part 2  not requited arrays

 if( tes.FIs > 0 && tes.Ls > 0 )
 {
   tes.BF = new double[pm.FIs*pm.N];
   memcpy( tes.BF, pm.BF, pm.FIs*pm.N*sizeof(double) );
   tes.XFA = new double[pm.FIs];
   memcpy( tes.XFA, pm.XFA, pm.FIs*sizeof(double) );
   tes.YFA = new double[pm.FIs];
   memcpy( tes.YFA, pm.YFA, pm.FIs*sizeof(double) );
//     tes.LsMod = (short *)aObj[ o_wi_lsmod].Alloc( tes.FIs, 1, I_ );
//     tes.LsMdc = (short *)aObj[ o_wi_lsmdc].Alloc( tes.FIs, 1, I_ );
//    PMs and DMS may be realloced after load arrays LsMod and LsMdc
//   tes.PMc = new float[pm.FIs];
//   memcpy( tes.PMc, pm.PMc, pm.FIs*sizeof(float) );
//   tes.DMc = new float[pm.Ls];
//   memcpy( tes.DMc, pm.DMc, pm.Ls*sizeof(float) );
   tes.PUL = new double[pm.FIs];
   memcpy( tes.PUL, pm.PUL, pm.FIs*sizeof(double) );
   tes.PLL = new double[pm.FIs];
   memcpy( tes.PLL, pm.PLL, pm.FIs*sizeof(double) );
//     tes.YOF = (float *)aObj[ o_wi_yof].Alloc( tes.FI, 1, F_ );
//     tes.sMod = (char (*)[6])aObj[ o_wi_smod].Alloc( tes.FIs, 1, 6 );
//     tes.RFLC = (char *)aObj[ o_wi_rflc].Alloc( tes.FIs, 1, A_ );
//     tes.RFSC = (char *)aObj[ o_wi_rfsc].Alloc( tes.FIs, 1, A_ );
 }

 if( tes.LO > 1 )
 {
   tes.Y_m = new double[pm.L];
   memcpy( tes.Y_m, pm.Y_m, pm.L*sizeof(double) );
   tes.IC_m = new double[pm.N];
   memcpy( tes.IC_m, pm.IC_m, pm.N*sizeof(double) );
   tes.IC_lm = new double[pm.N];
   memcpy( tes.IC_lm, pm.IC_lm, pm.N*sizeof(double) );
   tes.IC_wm = new double[pm.N];
   memcpy( tes.IC_wm, pm.IC_wm, pm.N*sizeof(double) );
 }
 // dispersion and sorbtion phases
 if( syp->PAalp != S_OFF )
 {
   tes.Aalp = new float[pm.FI];
   memcpy( tes.Aalp, pm.Aalp, pm.FI*sizeof(float) );
   tes.Xr0h0 = new float[pm.FI][2];
   memcpy( tes.Xr0h0, pm.Xr0h0, pm.FI*2*sizeof(float) );
 }
 if( syp->PSigm != S_OFF )
 {    tes.Sigw = new float[pm.FI];
     memcpy( tes.Sigw, pm.Sigw, pm.FI*sizeof(float) );
 }
 if( syp->PSigm != S_OFF )
 {    tes.Sigg = new float[pm.FI];
     memcpy( tes.Sigg, pm.Sigg, pm.FI*sizeof(float) );
 }
 if( tes.E )
 {
    tes.EZ = new double[pm.L];
     memcpy( tes.EZ, pm.EZ, pm.L*sizeof(double) );
    tes.Xcond = new float[pm.FI];
     memcpy( tes.Xcond, pm.Xcond, pm.FI*sizeof(float) );
    tes.Xeps = new float[pm.FI];
     memcpy( tes.Xeps, pm.Xeps, pm.FI*sizeof(float) );
 }

 if( tes.FIat > 0 && tes.Lads > 0 && tes.FIs > 0 )
 { // ADSORBTION AND ION EXCHANGE
//   tes.SATNdx = (short (*)[2])aObj[ o_wi_satndx].Alloc( tes.Ls, 2, I_ );
//   tes.SCM  = (char (*)[MST])aObj[ o_wi_scm].Alloc( tes.FIs, tes.FIat, A_ );
//   memset( tes.SCM, SC_NOT_USED, tes.FIs*tes.FIat );
    tes.Nfsp = new float[pm.FIs][MST];
     memcpy( tes.Nfsp, pm.Nfsp, pm.FIs*MST*sizeof(float) );
    tes.MASDT = new float[pm.FIs][MST];
     memcpy( tes.MASDT, pm.MASDT, pm.FIs*MST*sizeof(float) );
    tes.XcapA = new float[pm.FIs][MST];
     memcpy( tes.XcapA, pm.XcapA, pm.FIs*MST*sizeof(float) );
    tes.XcapB = new float[pm.FIs][MST];
     memcpy( tes.XcapB, pm.XcapB, pm.FIs*MST*sizeof(float) );
    tes.XcapD = new float[pm.FIs][MST];
     memcpy( tes.XcapD, pm.XcapD, pm.FIs*MST*sizeof(float) );
    tes.XcapF = new float[pm.FIs][MST];
     memcpy( tes.XcapF, pm.XcapF, pm.FIs*MST*sizeof(float) );
    tes.XdlA = new float[pm.FIs][MST];
     memcpy( tes.XdlA, pm.XdlA, pm.FIs*MST*sizeof(float) );
    tes.XdlB = new float[pm.FIs][MST];
     memcpy( tes.XdlB, pm.XdlB, pm.FIs*MST*sizeof(float) );
    tes.XdlD = new float[pm.FIs][MST];
     memcpy( tes.XdlD, pm.XdlD, pm.FIs*MST*sizeof(float) );
    tes.XpsiA = new double[pm.FIs][MST];
     memcpy( tes.XpsiA, pm.XpsiA, pm.FIs*MST*sizeof(double) );
    tes.XpsiB = new double[pm.FIs][MST];
     memcpy( tes.XpsiB, pm.XpsiB, pm.FIs*MST*sizeof(double) );
    tes.XpsiD = new double[pm.FIs][MST];
     memcpy( tes.XpsiD, pm.XpsiD, pm.FIs*MST*sizeof(double) );
    tes.XlamA = new float[pm.FIs][MST];
     memcpy( tes.XlamA, pm.XlamA, pm.FIs*MST*sizeof(float) );
    tes.Xetaf = new float[pm.FIs][MST];
     memcpy( tes.Xetaf, pm.Xetaf, pm.FIs*MST*sizeof(float) );
    tes.MASDJ = new float[pm.Lads][DFCN];
     memcpy( tes.MASDJ, pm.MASDJ, pm.Lads*DFCN*sizeof(float) );
//    tes.MASDJ = new float[pm.Ls];
//     memcpy( tes.MASDJ, pm.MASDJ, pm.Ls*sizeof(float) );
    tes.XetaA = new double[pm.FIs][MST];
     memcpy( tes.XetaA, pm.XetaA, pm.FIs*MST*sizeof(double) );
    tes.XetaB = new double[pm.FIs][MST];
     memcpy( tes.XetaB, pm.XetaB, pm.FIs*MST*sizeof(double) );
    tes.XFTS = new double[pm.FIs][MST];
     memcpy( tes.XFTS, pm.XFTS, pm.FIs*MST*sizeof(double) );
    tes.lnSAC = new double[pm.Lads][4];
     memcpy( tes.lnSAC, pm.lnSAC, pm.Lads*4*sizeof(double) );
//   tes.SATT = (char *)aObj[ o_wi_satt].Alloc( tes.Ls, 1, A_ );
 }

 if( tes.PG > 0 )
 {
//   tes.Fug = (float *)aObj[ o_wd_fug].Alloc( tes.PG, 1, F_ );
//   tes.Fug_l = (float *)aObj[ o_wd_fugl].Alloc( tes.PG, 1, F_ );
//   tes.Ppg_l = (float *)aObj[ o_wd_ppgl].Alloc( tes.PG, 1, F_ );
 }

   // Part 3

 if( tes.Ls > 1 && tes.FIs > 0 )
 {
    tes.Wb = new float[pm.Ls];
     memcpy( tes.Wb, pm.Wb, pm.Ls*sizeof(float) );
    tes.Wabs = new float[pm.Ls];
     memcpy( tes.Wabs, pm.Wabs, pm.Ls*sizeof(float) );
    tes.Rion = new float[pm.Ls];
     memcpy( tes.Rion, pm.Rion, pm.Ls*sizeof(float) );
//   tes.Qp = (double *)aObj[ o_wi_qp].Alloc( tes.FIs, 20, D_ );
//     tes.Qd = (double *)aObj[ o_wi_qd].Alloc( 1, 20, D_ );

 }
  flCopy = true;
// tes.R = (float *)aObj[ o_w_r].Alloc( tes.N, tes.N+1, D_ );

}
// free dynamic memory in objects and values
void TMulti::dyn_kill_test(MULTI& tes)
{
// memcpy( &pm.PunE, "jjbC", 4 );
// memset( &pm.N, 0, 36*sizeof(short));
// memset( &pm.TC, 0, 54*sizeof(double));
 delete[] tes.L1; /* delete[] tes.LsMod;   delete[] tes.LsMdc; */
 delete[] tes.mui;    delete[] tes.muk  ;    delete[] tes.muj  ;
 delete[] tes.SATX;    delete[] tes.DUL  ;    delete[] tes.DLL  ;
 delete[] tes.GEX;    delete[] tes.PUL  ;    delete[] tes.PLL  ;
 delete[] tes.YOF; /*  delete[] tes.PMc  ;    delete[] tes.DMc  ; */
 delete[] tes.Vol;    delete[] tes.HYM  ;    delete[] tes.VL   ;
 delete[] tes.MM ;    delete[] tes.H0   ;    delete[] tes.A0   ;
 delete[] tes.U0;    delete[] tes.S0   ;    delete[] tes.Cp0  ;
 delete[] tes.Cv0;    delete[] tes.Pparc;    delete[] tes.Y_m  ;
 delete[] tes.Y_la;    delete[] tes.Y_w  ;    delete[] tes.Gamma;
 delete[] tes.lnGmf;    delete[] tes.lnGmM;    delete[] tes.EZ   ;
 delete[] tes.Wb;    delete[] tes.Wabs ;    delete[] tes.Rion ;
 delete[] tes.Aalp;    delete[] tes.Sigw ;    delete[] tes.Sigg ;
 delete[] tes.Nfsp;    delete[] tes.MASDT;    delete[] tes.FVOL ;
 delete[] tes.FWGT;    delete[] tes.XcapA;    delete[] tes.XcapB;
 delete[] tes.XcapD;    delete[] tes.XdlA ;    delete[] tes.XdlB ;
 delete[] tes.XdlD;    delete[] tes.XpsiA;    delete[] tes.XpsiB;
 delete[] tes.XpsiD;    delete[] tes.Xr0h0;    delete[] tes.XlamA;
 delete[] tes.Xetaf;    delete[] tes.Xcond;    delete[] tes.Xeps ;
 delete[] tes.Awt;    delete[] tes.A    ;    delete[] tes.XFs  ;
 delete[] tes.Falps;    delete[] tes.Fug  ;    delete[] tes.Fug_l;
 delete[] tes.Ppg_l;    delete[] tes.XFTS ;    delete[] tes.MASDJ;
 delete[] tes.G;    delete[] tes.G0   ;    delete[] tes.lnGam;
 delete[] tes.lnGmo;    delete[] tes.lnSAC;    delete[] tes.B    ;
 delete[] tes.U;    delete[] tes.U_r  ;    delete[] tes.C    ;
 delete[] tes.IC_m;    delete[] tes.IC_lm;    delete[] tes.IC_wm;
 delete[] tes.BF;    delete[] tes.XF   ;    delete[] tes.YF   ;
 delete[] tes.XFA;    delete[] tes.YFA  ;    delete[] tes.Falp ;
 delete[] tes.XetaA;    delete[] tes.XetaB;    delete[] tes.X    ;
 delete[] tes.Y;    delete[] tes.XY   ;    delete[] tes.Qp   ;
 delete[] tes.Qd;    delete[] tes.MU   ;    delete[] tes.EMU  ;
 delete[] tes.NMU;    delete[] tes.W    ;    delete[] tes.Fx   ;
 delete[] tes.Wx;    delete[] tes.F    ;    delete[] tes.F0   ;
 delete[] tes.D;    delete[] tes.R    ;    delete[] tes.R1   ;
 /* delete[] tes.sMod; */   delete[] tes.SB   ;    delete[] tes.SM   ;
 delete[] tes.SF;    delete[] tes.SFs  ;    delete[] tes.pbuf ; //
 delete[] tes.RLC;    delete[] tes.RSC  ;   /* delete[] tes.RFLC ;
 delete[] tes.RFSC;  */  delete[] tes.ICC  ;    delete[] tes.DCC  ;
 delete[] tes.PHC;    delete[] tes.SCM  ;  /*  delete[] tes.SATT ; */
 delete[] tes.DCCW;    delete[] tes.XcapF;    delete[] tes.SM2   ;
 delete[] tes.SM3;   delete[] tes.DCC3; delete[] tes.SF2;

}

void TMulti::Test_Eq( int size, double *p1, double *p2, const char *key )
{
  vstr buf(150);
  for( int i=0; i<size; i++ )
    if( fabs(p1[i]-p2[i]) > fabs(p1[i])*1e-7 )
    {
       sprintf(buf, "%7s[%3d]:  %12.8g  %12.8g", key, i, p1[i], p2[i]);
//#ifndef IPMGEMPLUGIN
//       vfMessage( window(), "Double ", buf.p );
//#endif
//       cout << "Double " << buf.p << endl;
//       break;
    }
}

void TMulti::Test_Eq( int size, float *p1, float *p2, const char *key )
{
  vstr buf(150);
  for( int i=0; i<size; i++ )
    if( p1[i] != p2[i] )
    {
       sprintf(buf, "%7s[%3d]:  %12.8g  %12.8g", key, i, p1[i], p2[i]);
//#ifndef IPMGEMPLUGIN
//       vfMessage(window(), "Float ", buf.p);
//#endif
//       cout << "Float  " << buf.p << endl;
//       break;
    }
}

void TMulti::Test_Eq( int size, short *p1, short *p2, const char *key )
{
  vstr buf(150);
  for( int i=0; i<size; i++ )
    if( p1[i] != p2[i] )
    {
       sprintf(buf, "%7s[%3d]:  %12d  %12d", key, i, p1[i], p2[i]);
//#ifndef IPMGEMPLUGIN
//       vfMessage(window(), "Float ", buf.p);
//#endif
//       cout << "Short  " << buf.p << endl;
//       break;
    }
}

// Test by comparing old and new dynamic memory of the MULTI structure
void TMulti::dyn__test(MULTI& tes)
{
// cout << "Test  begin:     previous     current" << endl;
 Test_Eq( 36, &tes.N, &pm.N, "IFlags" );
 Test_Eq( 54, &tes.TC, &pm.TC, "Scalars" );
 Test_Eq( pm.L, tes.DUL, pm.DUL, "DUL" );
 Test_Eq( pm.L, tes.DLL, pm.DLL, "DLL" );
 Test_Eq( pm.L, tes.Vol, pm.Vol, "Vol" );
 Test_Eq( pm.L, tes.Pparc, pm.Pparc, "Pparc" );
 Test_Eq( pm.L, tes.MM, pm.MM, "MM" );
 Test_Eq( pm.N, tes.Awt, pm.Awt, "Awt" );
// Test_Eq( pm.FI, tes.XFs, pm.XFs, "XFs" );
// Test_Eq( pm.FI, tes.Falps, pm.Falps, "Falps" );
 Test_Eq( pm.L, tes.G, pm.G, "G" );
 Test_Eq( pm.L, tes.G0, pm.G0, "G0" );
 Test_Eq( pm.L, tes.lnGam, pm.lnGam, "lnGam" );
 Test_Eq( pm.L, tes.lnGmo, pm.lnGmo, "lnGmo" );
 Test_Eq( pm.N, tes.B, pm.B, "B" );
 Test_Eq( pm.N, tes.U, pm.U, "U" );
 Test_Eq( pm.N, tes.U_r, pm.U_r, "U_r" );
 Test_Eq( pm.N, tes.C, pm.C, "C" );
 Test_Eq( pm.FI, tes.XF, pm.XF, "XF" );
 Test_Eq( pm.FI, tes.YF, pm.YF, "YF" );
 Test_Eq( pm.FI, tes.Falp, pm.Falp, "Falp" );
 Test_Eq( pm.L, tes.X, pm.X, "X" );
 Test_Eq( pm.L, tes.Y, pm.Y, "Y" );
 Test_Eq( pm.L, tes.XY, pm.XY, "XY" );
 Test_Eq( pm.L, tes.MU, pm.MU, "MU" );
 Test_Eq( pm.L, tes.EMU, pm.EMU, "EMU" );
 Test_Eq( pm.L, tes.NMU, pm.NMU, "NMU" );
 Test_Eq( pm.L, tes.W, pm.W, "W" );
 Test_Eq( pm.L, tes.F, pm.F, "F" );
 Test_Eq( pm.L, tes.F0, pm.F0, "F0" );
 Test_Eq( pm.FI, tes.YOF, pm.YOF, "YOF" );
 Test_Eq( pm.FI, tes.FVOL, pm.FVOL, "FVOL" );
 Test_Eq( pm.FI, tes.FWGT, pm.FWGT, "FWGT" );
 Test_Eq( pm.L, tes.lnGmM, pm.lnGmM, "lnGmM" );
 Test_Eq( pm.L, tes.GEX, pm.GEX, "GEX" );

 if( tes.L > 0 )
 {
 Test_Eq( pm.L, tes.Y_la, pm.Y_la, "Y_la" );
 Test_Eq( pm.L, tes.Y_w, pm.Y_w, "Y_w" );
 Test_Eq( pm.L, tes.Fx, pm.Fx, "Fx" );
 Test_Eq( pm.L, tes.Wx, pm.Wx, "Wx" );
 Test_Eq( pm.L, tes.VL, pm.VL, "VL" );
 Test_Eq( pm.L, tes.Gamma, pm.Gamma, "Gamma" );
 Test_Eq( pm.L, tes.lnGmf, pm.lnGmf, "lnGmf" );
// Test_Eq( pm.L, tes.D, pm.D, "D" );
 }

   // Part 2  not required arrays

 if( tes.FIs > 0 && tes.Ls > 0 )
 {
 Test_Eq( pm.FIs*pm.N, tes.BF, pm.BF, "BF" );
 Test_Eq( pm.FIs, tes.XFA, pm.XFA, "XFA" );
 Test_Eq( pm.FIs, tes.YFA, pm.YFA, "YFA" );
// Test_Eq( pm.FIs, tes.PMc, pm.PMc, "PMc" ); /* not allocated ! */
// Test_Eq( pm.FIs, tes.DMc, pm.DMc, "DMc" ); /* not allocated ! */
// Test_Eq( pm.FIs, tes.PUL, pm.PUL, "PUL" );
// Test_Eq( pm.FIs, tes.PLL, pm.PLL, "PLL" );
}

 if( tes.LO > 1 )
 {
 Test_Eq( pm.L, tes.Y_m, pm.Y_m, "Y_m" );
 Test_Eq( pm.N, tes.IC_m, pm.IC_m, "IC_m" );
 Test_Eq( pm.N, tes.IC_lm, pm.IC_lm, "IC_lm" );
 Test_Eq( pm.N, tes.IC_wm, pm.IC_wm, "IC_wm" );
 }
 // sorption phases
 if( syp->PAalp != S_OFF )
 {
 Test_Eq( pm.FI, tes.Aalp, pm.Aalp, "Aalp" );
// Test_Eq( pm.FI*2, &tes.Xr0h0[0][0], &pm.Xr0h0[0][0], "Xr0h0" );
 }
 if( syp->PSigm != S_OFF )
   Test_Eq( pm.FI, tes.Sigw, pm.Sigw, "Sigw" );
 if( syp->PSigm != S_OFF )
   Test_Eq( pm.FI, tes.Sigg, pm.Sigg, "Sigg" );
 if( tes.E )
 {
 Test_Eq( pm.L, tes.EZ, pm.EZ, "EZ" );
 Test_Eq( pm.FI, tes.Xcond, pm.Xcond, "Xcond" );
 Test_Eq( pm.FI, tes.Xeps, pm.Xeps, "Xeps" );
 }

 if( tes.FIat > 0 && tes.Lads > 0 && tes.FIs > 0 )
 {
 Test_Eq( pm.FIs*MST, &tes.Nfsp[0][0], &pm.Nfsp[0][0], "Nfsp" );
 Test_Eq( pm.FIs*MST, &tes.MASDT[0][0], &pm.MASDT[0][0], "MASDT" );
 Test_Eq( pm.FIs*MST, &tes.XcapA[0][0], &pm.XcapA[0][0], "XcapA" );
 Test_Eq( pm.FIs*MST, &tes.XcapB[0][0], &pm.XcapB[0][0], "XcapB" );
 Test_Eq( pm.FIs*MST, &tes.XcapD[0][0], &pm.XcapD[0][0], "XcapD" );
 Test_Eq( pm.FIs*MST, &tes.XcapF[0][0], &pm.XcapF[0][0], "XcapF" );
 Test_Eq( pm.FIs*MST, &tes.XdlA[0][0], &pm.XdlA[0][0], "XdlA" );
 Test_Eq( pm.FIs*MST, &tes.XdlB[0][0], &pm.XdlB[0][0], "XdlB" );
 Test_Eq( pm.FIs*MST, &tes.XdlD[0][0], &pm.XdlD[0][0], "XdlD" );
 Test_Eq( pm.FIs*MST, &tes.XpsiA[0][0], &pm.XpsiA[0][0], "XpsiA" );
 Test_Eq( pm.FIs*MST, &tes.XpsiB[0][0], &pm.XpsiB[0][0], "XpsiB" );
 Test_Eq( pm.FIs*MST, &tes.XpsiD[0][0], &pm.XpsiD[0][0], "XpsiD" );
 Test_Eq( pm.FIs*MST, &tes.XlamA[0][0], &pm.XlamA[0][0], "XlamA" );
 Test_Eq( pm.FIs*MST, &tes.Xetaf[0][0], &pm.Xetaf[0][0], "Xetaf" );
 Test_Eq( pm.FIs*MST, &tes.XpsiA[0][0], &pm.XpsiA[0][0], "XpsiA" );
 Test_Eq( pm.Lads*DFCN, &tes.MASDJ[0][0], &pm.MASDJ[0][0], "MASDJ" );
// Test_Eq( pm.Ls, tes.MASDJ, pm.MASDJ, "MASDJ" );
 Test_Eq( pm.FIs*MST, &tes.XetaA[0][0], &pm.XetaA[0][0], "XetaA" );
 Test_Eq( pm.FIs*MST, &tes.XetaB[0][0], &pm.XetaB[0][0], "XetaB" );
 Test_Eq( pm.FIs*MST, &tes.XFTS[0][0], &pm.XFTS[0][0], "XFTS" );
 Test_Eq( MST*MST, &tes.D[0][0], &pm.D[0][0], "D" );
 Test_Eq( pm.Lads*4, &tes.lnSAC[0][0], &pm.lnSAC[0][0], "lnSAC" );
 }

 if( tes.PG > 0 )
 {
//   tes.Fug = (float *)aObj[ o_wd_fug].Alloc( tes.PG, 1, F_ );
//   tes.Fug_l = (float *)aObj[ o_wd_fugl].Alloc( tes.PG, 1, F_ );
//   tes.Ppg_l = (float *)aObj[ o_wd_ppgl].Alloc( tes.PG, 1, F_ );
 }

   // Part 3

// if( tes.Ls > 1 && tes.FIs > 0 )
// {
// Test_Eq( pm.Ls, tes.Wb, pm.Wb, "Wb" );
// Test_Eq( pm.Ls, tes.Wabs, pm.Wabs, "Wabs" );
// Test_Eq( pm.Ls, tes.Rion, pm.Rion, "Rion" );
// }
//  cout << "Test sequence end: " << endl;
}

//

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* Obsolete calculation of surface activity terms (Kulik, 1998, 2000, 2002)
* Revised by DAK on 03.Jan.2000 in Mainz ( cf. Kulik, 1999a,b )
*  to improve on IPM convergence at high SAT values.
* SAT are now placed into pmp->lnGam[j], as any activity coeffs
*  pmp->lnSAT vector is now used to keep original DUL[j] to restore
*  them after IPM-2 refinements for surface complexes.
*/
void TProfil::SurfaceActivityTerm( int jb, int je, int k )
{
    int i, j, ist, Cj, iSite[6];
    double XS0,  xj0, XVk, XSk, XSkC, xj, Mm, rIEPS, ISAT, SAT,
           / OSAT, * SATst, xjn, q1, q2;

    if( pmp->XFA[k] < pmp->DSM ) // No sorbent retained by the IPM 
        return;
    if( pmp->XF[k]-pmp->XFA[k] < pmp->lowPosNum /* *10. * )
        return;  /* No surface species *

    for(i=0; i<6; i++)
        iSite[i] = -1;

    /* Extraction of site indices for neutral >OH groups *
    for( j=jb; j<je; j++ )
    {
        if( pmp->SATT[j] != SAT_SITE )
//        {
//            if( pmp->DCC[j] == DC_PEL_CARRIER || pmp->DCC[j] == DC_SUR_MINAL ||
//                    pmp->DCC[j] == DC_SUR_CARRIER ) continue;
//          ist = pmp->SATNdx[j][0] / MSPN; // MSPN = 2 - number of EDL planes
            continue;
//        }
        ist = pmp->SATNdx[j][0] / MSPN;
        iSite[ist] = j;
    }

    for( j=jb; j<je; j++ )
    { /* Loop for DC *
        if( pmp->X[j] <= pmp->lowPosNum /* *10. * )
            continue;  /* This surface DC has been killed by IPM *
//        OSAT = pmp->lnGmo[j]; // added 6.07.01 by KDA
        rIEPS = pa.p.IEPS;   // between 1e-8 and 1e-10 default 1e-9
        switch( pmp->DCC[j] )  /* code of species class *
        {
        default: /* pmp->lnGam[j] = 0.0; *
            continue;
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
        case DC_IEWC_B:
        case DC_SUR_COMPLEX:
        case DC_SUR_IPAIR:
        case DC_IESC_A:
            /* Calculate ist - index of surface type *
            ist = pmp->SATNdx[j][0] / MSPN;
            /* Cj - index of carrier DC *
            Cj = pmp->SATNdx[j][1];
            if( Cj < 0 )
            {  /* Assigned to the whole sorbent *
                XVk = pmp->XFA[k];
                Mm = pmp->FWGT[k] / XVk;
            }
            else
            { /* Assigned to one of the sorbent end-members *
                XVk = pmp->X[Cj];
                if( XVk < pmp->DSM/10.0 )
                    continue; /* This end-member is zeroed off by IPM *
                Mm = pmp->MM[Cj] * XVk/pmp->XFA[k];  // mol.mass
            }
            XSk = pmp->XFTS[k][ist]; /* Tot.moles of sorbates on surf.type *
            xj = pmp->X[j];  /* Current moles of this surf.species *
//          a=1.0;    Frumkin factor - reserved for extension to FFG isotherm
            switch( pmp->SATT[j] )
            {
            case SAT_COMP: // Competitive surface species on a surface type
                /* a = fabs(pmp->MASDJ[j]); *
// rIEPS = pa.p.IEPS * 0.8;
                if( iSite[ist] < 0 )
                    xjn = 0.0;
                else xjn = pmp->X[iSite[ist]]; // neutral site does not compete!
                XS0 = pmp->MASDT[k][ist] * XVk * Mm / 1e6
                      * pmp->Nfsp[k][ist]; // expected total in moles
                if( pa.p.PC == 1 )
                    rIEPS = pa.p.IEPS * XS0;  // relative IEPS
                XSkC = XSk - xjn - xj; // occupied by the competing species;
	                             // this sorbate cannot compete to itself
                if( XSkC < 0.0 )
                    XSkC = 0.0;
                if( XSkC >= XS0 )  // Limits
                    XSkC = XS0 - 2.0 * rIEPS;
                xj0 = XS0 - XSkC;    // expected moles of this sorbate
                if(xj >= xj0)
                       xj = xj0 - rIEPS; // limits: 2 * rIEPS to XS0 - rIEPS
                if( xj * 2 <= xj0 )
                    ISAT = 0.0;      // ideal case
                else
                {
                   q1 = xj0 - xj;
                   q2 = rIEPS * XS0;
                   if( pa.p.PC == 2 && !pmp->W1 || pa.p.PC != 2 )
                   {
                      if( q1 > q2 )
                        q2 = q1;
                   }
                   else {
                      q2 = q1;
                      if( q2 <= 1e-33 )
                         q2 = 1e-33;
                   }
                   ISAT = log( xj ) - log( q2 );
                }
                // Test and compare with OSAT
/*                if( pmp->IT > 0 &&
                   ( ISAT < 0 || ISAT > 0-log(rIEPS) || fabs(ISAT-OSAT) > 3 ))
                {
cout << "IT=" << pmp->IT << ": Compt SAT for j= " << j << " old lnSAT= " << OSAT << " new lnSAT= " << ISAT << endl;
cout << "     x[j]= " << pmp->X[j] << " xj= " << xj << " xj0= " << xj0 << " XSkC= " << XSkC << " XSk= " << XSk << endl;
// ISAT = OSAT + 1;           // experiment
                }
*              pmp->lnGam[j] = ISAT;
                break;
            case SAT_NCOMP: /* Non-competitive surface species *
// rIEPS = pa.p.IEPS * 2;
                xj0 = fabs(pmp->MASDJ[j][PI_DEN]) * XVk * Mm / 1e6
                      * pmp->Nfsp[k][ist]; /* in moles *
                if( pa.p.PC == 1 )
                    rIEPS = pa.p.IEPS * xj0;  // relative IEPS
                if(xj >= xj0)
                     xj = xj0 - rIEPS;  // upper limit
                if( xj * 2.0 <= xj0 )  // Linear adsorption - to improve !
                    ISAT = 0.0;
                else
                {
                    q1 = xj0 - xj;  // limits: rIEPS to 0.5*xj0
                    q2 = xj0 * rIEPS;
                    if( pa.p.PC == 2 && pmp->W1 )
                       ISAT = log( xj ) - log( q1 );
                    else {
                       if( q1 > q2 )
                          ISAT = log( xj ) - log( q1 );
                       else
                          ISAT = log( xj ) - log( q2 );
                    }
                 }
//                    if( pa.p.PC == 2 && !pmp->W1 || pa.p.PC != 2 )
/*                  if( pa.p.PC == 2 && pmp->W1 )
                    {
                         q2 = q1;
                    }
                    else {
                       if( q1 > q2 )
                           q2 = q1;
                    }
                    if( q2 <= 1e-33 )
                        q2 = 1e-33;
                    ISAT = log( xj ) - log( q2 );
                }
*/                // Test and compare with OSAT
/*                if( pmp->IT > 0 &&
                  ( ISAT < 0 || ISAT > 0-log(rIEPS) || fabs(ISAT-OSAT) > 3 ))
                {
cout << "IT=" << pmp->IT << ": Ncomp SAT for j= " << j << " old lnSAT= " << OSAT << " new lnSAT= " << ISAT << endl;
cout << "     x[j]= " << pmp->X[j] << " xj= " << xj << " xj0= " << xj0 << " XSk= " << XSk << endl;
// ISAT = OSAT + 1;           // experiment
                }
*              pmp->lnGam[j] = ISAT;
                break;
            case SAT_SITE:  /* Neutral surface site (e.g. >O0.5H@ group) *
// rIEPS = pa.p.IEPS;
                XS0 = pmp->MASDT[k][ist] * XVk * Mm / 1e6
                      * pmp->Nfsp[k][ist]; /* in moles *
                if( pa.p.PC == 1 )
                    rIEPS = pa.p.IEPS * XS0;  // relative IEPS
                XSkC = XSk - xj;
                if( XSkC < 0.0 ) // potentially a serious error !
                    XSkC = 0.0;
//                if( XSkC >= XS0 )  // Limits
//                    XSkC = XS0 - 2.0 * rIEPS;
                if( pmp->MASDJ[j][PI_DEN] <= 0.0 )
                    SATst = pa.p.DNS*1.66054*pmp->Aalp[k]/
                            pmp->MASDT[k][ist];
                else SATst = pa.p.DNS*1.66054*pmp->Aalp[k]/pmp->MASDJ[j][PI_DEN];
//                                 pmp->MASDJ[j];
                if( fabs(XS0-XSkC) > XS0*rIEPS )
                {
                    if( XSkC > XS0 )  // case 1
                        SAT = 1.0;
                    else
                    {
                        SAT = xj/(XS0-XSkC);      // case 2
                        if( XSk > XS0 )
                            SAT *= XSk/(XS0-XSkC);    // case 3
                    }
                }
                else SAT = xj * XS0*rIEPS;     // boost ?????
/*                if( pmp->IT > 0 &&
                  ( SAT < 1e-2 || SAT > 1.0/rIEPS || fabs(log(SAT)+log(SATst)-OSAT) > 3 ) )
                {
cout << "IT=" << pmp->IT << ": NSite SAT for j= " << j << " old lnSAT= " << OSAT << " new lnSAT= " << ISAT << endl;
cout << "     x[jn]= " << pmp->X[j] << " XSk= " << XSk << " XSkC=" << XSkC << " XS0=" << XS0 << " SATst=" << SATst << endl;
//  ISAT = OSAT + 1;           // experiment
                }
*              if( SAT < 1e-2 )  // limits
                    SAT = 1e-2;  // to limit boosting
                if( SAT > 1.0/rIEPS )
                    SAT = 1.0/rIEPS;  // to limit from above
                pmp->lnGam[j] = log( SAT );
                pmp->lnGam[j] += log( SATst );
                break;
            case SAT_INDEF: /* No SAT calculation *
            default:        /* pmp->lnGam[j] = 0.0; *
                break;
            }
        }
    }  /* j *
}  

