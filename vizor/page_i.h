//-------------------------------------------------------------------
// $Id$
//
// Inernal codes for vizor system
//
// Copyright (C) 1996-2001  A.Rysin
// Uses  gstring class (C) A.Rysin 1999
//
// This file is part of the GEM-Vizor library which uses the
// Qt v.2.x GUI Toolkit (Troll Tech AS, http://www.trolltech.com)
// according to the Qt Duo Commercial license
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------
#ifndef page_i_h
#define page_i_h

#include "page_s.h"
#include "visor_w.h"

//const int wdCELL = 90;
//const int htCELL = 18;
//const int htCHECK = htCELL;
//const int wdCHECK = htCHECK;

const int wdSPACE = 8;
const int htSPACE = 2;
const int wdFSPACE = wdSPACE;
const int htFSPACE = htSPACE;

//const int wdLABEL = 60;
//const int htLABEL = htCELL;

const int szTab = 25;		// height of a tab control
const int szFButton = 20;		// width of a files list button
const int szTime = 120;

const int szSBAR = 16;		// size of TFiled 'Grid' ScrollBar



inline int wdF(eFieldType ft, int npos)
{
    if( ft == ftCheckBox ) {	// we'd like to get square 'checkbox cell'
	int width = pVisorImp->getCharWidth();
	int height = pVisorImp->getCharHeight();
	return max(width, height);
    }
    else {
	return npos * pVisorImp->getCharWidth();
    }
}

inline int htF(eFieldType ft, int ht)
{
    if( ft==ftText ) {
	return pVisorImp->getCharHeight()*ht/2;
    }
    else if( ft == ftCheckBox ) {
	int width = pVisorImp->getCharWidth();
	int height = pVisorImp->getCharHeight();
	return max(width, height);
    }
    else {
	return pVisorImp->getCharHeight();
    }
}


inline
bool IsSign(uint ch)
{
    return (ch=='-');
}

inline
bool IsEorP(uint ch)
{
    return (ch=='e' || ch=='E' || ch=='.');
}

inline
bool IsDigit(uint ch)
{
    return (ch>='0' && ch<='9');
}


#endif  // _page_i_h

