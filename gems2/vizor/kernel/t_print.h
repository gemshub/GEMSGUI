//-------------------------------------------------------------------
// Id: gems/src/kernel/t_print.cpp  version 2.0.0  edited 2000-07-10
//
// Declaration of TPrintData class
//
//
// Created : 010823    Modified: 010908
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
#ifndef _t_print_h_
#define _t_print_h_

#include "v_object.h"

 // Format: % [flags] [width] [.prec] type_char
 // flags: '+' or '-';  width = nn; [.prec]= .nn
 // type_char
 // s	Prints characters until a null-terminator
 // f	Floating point	signed value of the form [-]dddd.dddd.
 // e	Floating point	signed value of the form [-]d.dddde[+/-]ddd
 // g	Floating point	signed value in either e or f form,
 //     based on given value and precision.

struct PFormat
{
  gstring fmt;
  char type;
  char long_;

  PFormat( const char aType, gstring aFmt ):
    fmt(aFmt), type( aType), long_(' ')
  {}

  PFormat( PFormat& d ):
    fmt( d.fmt), type( d.type), long_(d.long_)
  {}

  gstring FmtOut()
  {
    gstring res = fmt;
    if( long_ == 'l')
      res += "l";
    res += gstring( 1, type);
    return res;
  }
};

  enum data_ttttt { // >=0 - object number
     index_d = -1, space_d = -2,  rkey_d = -3,
     date_d =-4, time_d = -5,  text_d = -6,
     all_d = -10, label_d =-11, line_d=-12, list_d = -13
    };


// Format: [all] [label] #obj_name or
//         [label] #obj_name[i]  or
//         #obj_name[i,j]  or   (substring from-to for string data)
//         index  or
//         space  or
//         rkey
//         date, time

struct PData
{
  bool is_all;   // for all colums of object
  bool is_label; // for objects type
  int  data;     // object number  or index, space,  rkey
  int index;     // object column or start substring
  int end_sub;   // end substring
  gstring line;

  PData( int _data, int _index, int s_end,
        bool _all, bool _label):
   is_all(_all), is_label( _label),  data(_data),
   index(_index), end_sub(s_end)
   { }

   PData( int _data, const char * _line):
   is_all(false), is_label(false),  data(_data),
   index(0), end_sub(-1), line( _line )
   { }


  PData( PData& d ):
   is_all(d.is_all), is_label( d.is_label),  data(d.data),
   index(d.index), end_sub(d.end_sub), line(d.line)
   { }

};

class TPrintData  // print resalts of module calc
{
    gstring key_format;        // key of SDref record
    int nRT;                   // module number

    char *input;               // current position
    TIArray<PFormat> aFmts;  // list of formats
    TIArray<PData> aDts;     // list of datas

protected:
    bool getFormat( const char * aFmt );
    int  getToken( int& ii, int& jj );
    void getData();
    void prnData( fstream& fout, int ind, PFormat& fmt, PData& dt );
    void skipSpace();


public:

    TPrintData(const char *sd_key,
    int nrt, fstream& fout, const char *fmt_text );
    ~TPrintData();

};

#endif  // _t_print_h
