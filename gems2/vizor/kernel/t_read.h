//-------------------------------------------------------------------
// $Id$
//
// Declaration of TReadData class
//
// Copyright (C) 2001  S.Dmytriyeva
// Uses  gstring class (C) A.Rysin 1999
//
// This file is part of the GEM-Vizor library which uses the
// Qt v.2.x GUI Toolkit (Troll Tech AS, http://www.trolltech.com)
// according to the Qt Duo Commercial license
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
#include "v_ipnc.h"


  enum data_rftype { 
     read_r = 1, /*%nns*/
     empty_r = 2, /*EMPTY*/
     object_r = 3,/*#obj_name[i,j]*/
     string_r =4, /*"string"*/
     irec_r =5, /*IREC*/
    };


// Format:
//        %nns	         Read characters until a " " or nn
//             using only SetString() => no use any formats
//        EMPTY          Set empty value
//        IREC           Set reading record number
//        "characters"   Set string value
//        #obj_name
//        #obj_name[i]
//        #obj_name[i,j] Set values from object (GetString())

struct RFormat
{
  int     type;
  int     size; // number of characters to read
                // or object index for object_r type
  int  i;        // to object_r i(N) index
  int  j;        // to object_r j(M) index
  gstring fmt;

  RFormat( const char aType, int aSize, gstring aFmt ):
    type( aType), size(aSize), i(0), j(0), fmt(aFmt)
  {

  }

  RFormat( const char aType, int obj_ndx,int i_ndx, int j_ndx ):
    type( aType), size(obj_ndx),i(i_ndx), j(j_ndx), fmt("")
  {

  }

  RFormat( RFormat& d ):
    type( d.type), size(d.size), i(d.i), j(d.j), fmt( d.fmt)
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

  RData( RData& d ):
   objNum(d.objNum), i(d.i), j(d.j)
   { }

};


// Format: RFormat RData, or
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
   void readData( fstream& fin, RFormat& fmt, RData& dt );

public:

    TReadData(const char *sd_key,
    int nrt, const char *fmt_text );
    ~TReadData();

    void  readRecord( int n_itr, fstream& fread );
};

#endif  // _t_read_h
