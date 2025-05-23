//-------------------------------------------------------------------
// $Id: t_print.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of TPrintData class
//
// Copyright (C) 2001  S.Dmytriyeva
//
// This file is part of the GEM-Selektor GUI library which uses the
// Qt v.4 cross-platform App & UI framework (https://qt.io/download-open-source)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html) 
//
// This file may be distributed under the GPL v.3 license

//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------
#ifndef _t_print_h_
#define _t_print_h_

#include <vector>
#include "v_ipnc.h"

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
  char type;
  char long_;
  string fmt;

  PFormat( const char aType, string aFmt ):
    type( aType), long_(' '), fmt(aFmt)
  {}

  string FmtOut()
  {
    string res = fmt;
    if( long_ == 'l' && type != 's')
      res += "l";
    res += string( 1, type);
    return res;
  }

};

  enum data_ttttt { // >=0 - object number
     index_d = -1, space_d = -2,  rkey_d = -3,
     date_d =-4, time_d = -5,  text_d = -6,
     all_d = -10, label_d =-11, substr_d = -12,
     line_d=-13, list_d = -14
    };


// Format: [all] [label] [substr[i_from,i_to]] #obj_name or
//         [label] [substr[i_from,i_to]] #obj_name[col]    or
//         [substr[i_from,i_to]] #obj_name[line,col]          or
//         index  or
//         space  or
//         rkey   or
//         date, time

struct PData
{
  bool is_all;   // for all colums of object
  bool is_label; // for objects type
  int  data;     // object number  or index, space,  rkey ...
  int index_i;   // object line
  int index_j;   // object column
  int bg_sub;    // start substring
  int end_sub;   // end substring  or

   // object line for  line type
  string line;

  PData( int _data, int _i, int _j,
        int bg_, int end_, bool _all, bool _label):
   is_all(_all), is_label( _label),  data(_data),
   index_i(_i), index_j(_j), bg_sub(bg_), end_sub(end_)
   { }

   PData( int _data, const char * _line):
   is_all(false), is_label(false),  data(_data),
   index_i(0), index_j(0), bg_sub(-1), end_sub(-1),
   line( _line )
   { }

};

class TPrintData  // print resalts of module calc
{
    string key_format;        // key of SDref record
    unsigned int nRT;          // module number

    char *input;               // current position
    std::vector<PFormat> aFmts;  // list of formats
    std::vector<PData> aDts;     // list of datas

    bool  ifcond;
    string cond;      // Text with IPN-expression << prn=: cond; >>
                       // ii - index of line in expression
    IPNCalc rpn;       // IPN of equats of print condition
    int prr;           // internal
    int iir;           // internal


protected:
    bool getFormat( const char * aFmt );
    int  getToken( int& ii, int& jj );
    void getData();
    void prnData( fstream& fout, int ind, PFormat& fmt, PData& dt );
    void skipSpace();


public:

    TPrintData(const char *sd_key, unsigned int nrt, fstream& fout, const char *fmt_text );
    ~TPrintData();

};

#endif  // _t_print_h
