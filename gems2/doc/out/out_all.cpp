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
    tblen = strlen(tab)+1; /* Get size of text buf  */
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
    tblen = strlen(tab)+1; /* Get size of text buf  */
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
    tblen = strlen(tab)+1; /* Get size of text buf  */
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

    /* Equations of Process  */
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
    /* ARRAY  Kin */
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
    tblen = strlen(tab)+1; /* Get size of text buf  */
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
    tblen = strlen(tab)+1; /* Get size of text buf  */
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
    tblen = strlen(tab)+1; /* Get size of text buf  */
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
    tblen = strlen(tab)+1; /* Get size of text buf  */
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
    tblen = strlen(tab)+1; /* Get size of text buf  */
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
        else  /* one component phase  */
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
    tblen = strlen(tab)+1; /* Get size of text buf  */
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
    tblen = strlen(tab)+1; /* Get size of text buf  */
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
    tblen = strlen(tab)+1; /* Get size of text buf  */
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
        "                                                         Table  4\n\n"
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
    tblen = strlen(tab)+1; /* Get size of text buf  */
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

