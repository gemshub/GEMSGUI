//-------------------------------------------------------------------
// Id: gems/src/core/v_print.cpp  version 2.0.0  edited 2000-07-10
//
// Implementation of TPrintTable class
//
// Created : 970207
//
// Copyright (C) 1996-2000 A.Rysin,S.Dmytriyeva,D.Kulik
//
// Uses  gstring class (C) A.Rysin 1999
//
// This file is part of a GEMS-Visor meta-toolkit layer which uses
// the Qt v.2.1 GUI Toolkit (Troll Tech AG Norway, www.trolltech.com)
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL) which includes terms of the Q Public
// License as defined by Troll Tech AS of Norway and appearing
// in the file LICENSE.QPL
//
// See http://www.psi.ch/~gems-psi for further information
// E-mail gems2.support@psi.ch; arysin@yahoo.com
//
//-------------------------------------------------------------------

#include <stdio.h>

#include "v_print.h"
#include "v_object.h"
#include "v_mod.h"


const char *tww_eny = "     `     ";
const char * startLine =
    "\n---------------------------------------------------------------------\n"
    "Geochemical Equilibrium Modelling by Gibbs Energy Minimization\n"
    "GEM-Selektor v1.93b-PSI (C) GEMS Development Team, 1996-2001\n\n";
const char *endLine =
    "-----------------------------------------------------------------\n";

TPrintTable::TPrintTable(const char *title,
                         const char* modName, const char * key)
{
    tbuf = startLine;
    tbuf += title;
    tbuf += "\nDate: ";
    tbuf +=  curDate();
    tbuf += "  Time: ";
    tbuf +=  curTime();
    tbuf += "\n";
    tbuf += modName;
    tbuf += "   Keywd: ";
    tbuf += key;
    tbuf += "\n\n";
}

TPrintTable::TPrintTable()
{
    tbuf = "";
}

TPrintTable::~TPrintTable()
{}


void TPrintTable::NextLine()
{
    tbuf += "\n";
}

// Put gstring line to table
void TPrintTable::PutString( const char * str,  int size, bool nextLine  )
{
    if( size==0 )
        size = strlen(str);
    tbuf += gstring( str, 0, size );
    if( nextLine )
        NextLine();
}

// Put object name with index
void TPrintTable::PutObjName( const char* name, int type, int index  )
{
    vstr pbuf(100);

    switch(type)
    {
    case 1: //rp_tab_14  "%11.7s[%d]  "
        sprintf( pbuf, "%11.7s[%d]  ", name/*aObj[nObj].GetKeywd()*/, index);
        break;
    case 2: //si_tab_14a "|%12.7s "
        sprintf( pbuf, "|%12.7s ", name );
        break;
    case 3: //si_tab_14b "|%12.7s |\n"
        sprintf( pbuf, "|%12.7s |\n", name );
        break;
    case 4: //si_tab_14 "|%11.7s  "
        sprintf( pbuf, "|%11.7s  ", name );
        break;
    case 5: //si_tab_13 "| %-23.23s"
        sprintf( pbuf, "| %-23.23s",  name );
        break;
    case 7: //si_tab_13 "  %-23.23s"
        sprintf( pbuf, "  %-23.23s",  name );
        break;
    case 6: //si_tab_14c "%12.7s  "
        sprintf( pbuf, "  %12.7s ",  name );
        break;
    }
    tbuf += pbuf;
}

// Put float value  (rp_tab_15)
void TPrintTable::PutFloat( float ww, int presis, int type  )
{
    vstr pbuf(100), tww(32);

    if( IsFloatEmpty( ww ) )
        strcpy( tww, tww_eny);
    else    gcvt( ww, presis, tww);
    switch( type )
    {
    case 1: //rp_tab_15
        sprintf( pbuf,  "%15.15s ", tww.p );
        break;
    case 2: //si_tab_15
        sprintf( pbuf,  " %13.13s", tww.p );
        break;
    case 3: //si_tab_15b
        sprintf( pbuf,  " %13.13s \n", tww.p );
        break;
    case 4: //si_tab_15y
        sprintf( pbuf,  " %12.12s ", tww.p );
        break;
    case 5: //si_tab_15y
        sprintf( pbuf,  "%12.12s ", tww.p );
        break;
    }

    tbuf += pbuf;
}

//last line in tabl print (si_tab_16)
void TPrintTable::PutEnd()
{
    tbuf += endLine;
    tbuf+= char(12);
}

//last line in tabl print
void TPrintTable::PutTabHead( int type )
{
    switch( type )
    {
    case 1:
        tbuf += "\n|============="; // si_tab_4z
        break;
    case 2:
        tbuf +=   "\n|=============|\n"; // si_tab_4x
        break;
    case 3:
        tbuf +=   "\n|================================|=|=|="; // tp_tab_4
        break;
    case 4:
        tbuf +=   "\n|========================"; // si_tab_4   24
        break;
    case 5:
        tbuf +=   "|============="; // si_tab_5
        break;
    case 6:
        tbuf +=   "|=============|\n"; // si_tab_6
        break;
    }
}

//delta line in tabl print
void TPrintTable::PutTabDelta( int type )
{
    switch( type )
    {
    case 1:
        tbuf += "|============="; // si_tab_7z
        break;
    case 2:
        tbuf += "|=============|\n"; // si_tab_7x
        break;
    case 3:
        tbuf += "|================================|=|=|="; // tp_tab_7
        break;
    case 7:
        tbuf += "|========================"; // si_tab_7
        break;
    case 8:
        tbuf += "|============="; // si_tab_8
        break;
    case 9:
        tbuf += "|=============|\n"; // si_tab_9
        break;
    }
}

//last line in tabl print
void TPrintTable::PutTabEnd( int type )
{
    switch( type )
    {
    case 10:
        tbuf += "|============="; // si_tab_10z
        break;
    case 9:
        tbuf += "|=============|\n"; // si_tab_10x
        break;
    case 8:
        tbuf += "|========================"; // si_tab_10
        break;
    case 7:
        tbuf += "|================================|=|=|=";// tp_tab_11
        break;
    case 11:
        tbuf +=  "|============="; // si_tab_11
        break;
    case 12:
        tbuf +=  "|=============|\n"; // si_tab_12
        break;
    }
}

