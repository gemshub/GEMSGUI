//-------------------------------------------------------------------
// Id: gems/src/core/v_print.h  version 2.0.0  edited 2000-07-10
//
// Declaration of TPrintTable class
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

#ifndef _v_print_h_
#define _v_print_h_

#include "v_user.h"

class TPrintTable  // print resalts of module calc
{
    gstring tbuf;  // bufer of resalts

protected:

public:

    TPrintTable(const char *title,
                const char* modName, const char * key);
    TPrintTable();
    ~TPrintTable();

    //--- Selectors
    const char *GetTable() const
    {
        return tbuf.c_str();
    }

    //--- Value manipulation
    void NextLine();
    void PutString( const char * str, int size=0, bool nextLine=true  );
    void PutObjName( const char *name, int type, int index=-1 );
    void PutFloat( float ww, int presis, int type  );
    void PutTabHead( int type );
    void PutTabDelta( int type );
    void PutTabEnd( int type );
    void PutEnd();

};

extern const char *endLine;

#endif
