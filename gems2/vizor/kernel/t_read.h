//-------------------------------------------------------------------
// Id: gems/src/kernel/t_read.h  version 2.0.0  edited 2000-07-10
//
// Declaration of TReadData class
//
//
// Created : 020206  
//
// Copyright (C) 2001  S.Dmytriyeva
// Uses  gstring class (C) A.Rysin 1999
//
// This file is part of the GEM-Vizor library which uses the
// Qt v.2.x GUI Toolkit (Troll Tech AS, http://www.trolltech.com)
// according to the Qt Duo Commercial license #1435515
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL)
//
// See http://les.web.psi.ch/Software/GEMS-PSI/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------
#ifndef _t_read_h_
#define _t_read_h_

#include "v_object.h"

  enum data_rftype { 
     read_r = 1 /*%nns*/, empty_r = 2 /*EMPTY*/,
     skip_r = 3 /*SKIP*/, string_r =4 /*"string"*/
    };


// Format:
//        %nns	        Read characters until a " " or nn
//             using only SetString() => no use any formats
//        EMPTY         Set empty value
//        "characters"  Set values for object (SetString())

struct RFormat
{
  int     type;
  gstring fmt;

  RFormat( const char aType, gstring aFmt ):
    type( aType), fmt(aFmt)
  {}

  RFormat( PFormat& d ):
    type( d.type), fmt( d.fmt)
  {}

  int FmtType()
  {
    return type;
  }

  const char* FmtString()
  {
    return fmt.c_str();
  }

};

enum data_rdtype { // >=0 - object number
                   undef_s = -1, rkey_s = -2, skip_s = -3
                 };

// Format:  #obj_name[i,j]
//          #RKEY[i]
//          #SKIP

struct RData
{
  int  objNum;   // object number  or  rkey or   skip command
  int  i;        // object i (N)  (sizeN for list reading)
  int  j;        // object j (M)  (sizeM for list reading)

  RData( int _index, int i_index, int j_index):
   objNum(_index), i(i_index), j(j_index)
   { }

  RData( PData& d ):
   objNum(d.objNum), i(d.i), j(d.j)
   { }

};


// Format: RFormat RData, or
//         RFormat RData ... RData, or (more than 1 , not reliased)
//         list RFormat  #obj_name[sizeN,sizeM],
//             ( reading sizeN*sizeM values )

class TReadData  // reading data by format
{
    gstring key_format;        // key of SDref record
    int nRT;                   // module number

    char *input;               // current position
    TOArray<bool> aList;     // true if reading list of values
    TIArray<RFormat> aFmts;  // list of formats
    TIArray<RData> aDts;     // list of datas

   gstring mScript;     // Text with IPN-expression ## equations ##
   IPNCalc rpn;       // IPN of equats of print condition

protected:

   void skipSpace();
   void getFormat();
   void getData( bool isList );
   void prnData( fstream& fin, RFormat& fmt, RData& dt );

public:

    TReadData(const char *sd_key,
    int nrt, const char *fmt_text );
    ~TReadData();

    void  readRecord( fstream& fread );
};

#endif  // _t_read_h
