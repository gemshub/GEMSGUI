//-------------------------------------------------------------------
// $Id$
//
// Implementation of TPrintData class
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

#include <stdio.h>

#include "t_print.h"
#include "v_object.h"
#include "v_dbm.h"
#include "v_user.h"
#include "service.h"


TPrintData::TPrintData(const char *sd_key,
    int nrt, fstream& fout, const char *fmt_text ):
  key_format( sd_key ), nRT(nrt)
{
  ErrorIf( !fout.good() , rt[nRT].GetKeywd(), "Fileopen error");
  input = (char*)fmt_text;

  prr = 1;
  iir = 0;
  int o_prn_= aObj.Find("prn");
  int o_ii_ = aObj.Find("ii");

  aObj[o_prn_].SetPtr(&prr);
  aObj[o_ii_].SetPtr(&iir);

  while( *input )
  {
    int i=0, j=-1, count;
    int code = getToken( i, j );
    ifcond = false;

    if( code == line_d )
        count = 1;
    else
       if(  code == list_d )
       {
            code = getToken( i, j );
            if( code >=0 )
            {
              count = aObj[code].GetN();
              if( aObj[code].IsNull() )
               count = 0;
            }
            else
              count = 1;
       }
       else
           {  gstring str_err = "Illegal command: \n";
              str_err += input;
              Error( key_format.c_str(), str_err.c_str() );
            }
    // insert condition  ## <math script>  ##
    skipSpace();
    if( *input == '#' && *(input+1) == '#' )
    {
            input+=2;
            char* pose = strchr( input, '#');
            while( pose )
            {
               if( *(pose+1) == '#' )
                 break;
               pose = strchr( pose+1, '#');
            }
            if( !pose )
            {  gstring str_err = "Illegal condition: \n";
               str_err += input;
               Error( key_format.c_str(), str_err.c_str() );
            }
            cond = gstring( input, 0, pose-input );
            input = pose+2;
            ifcond = true;
            rpn.GetEquat( (char *)cond.c_str() );
    }
    // end insert

     aFmts.Clear();  // list of formats
     aDts.Clear();     // list of datas
     //Read formats
     do{
          if( *input == ',')
            input++;
          skipSpace();
          /*while( *input == ' ' || *input == '\n' || *input == '\t')
                  input++; */
          getFormat( input );
          getData();
          skipSpace();
          /*while( *input == ' ' || *input == '\n' || *input == '\t')
                  input++;*/
    }while( *input == ',' /**input != '\0'*/ );
    //Print formats
    for( int ii=0; ii<count; ii++ )
    {
      if( ii == 239)
       ii = ii;
       if( ifcond )
       {
         iir = ii;
         rpn.CalcEquat();
         if( prr == 0 )
          continue;
       }
       for( uint jj=0; jj<aDts.GetCount(); jj++ )
       {
          if( code == line_d && aDts[jj].index_i != 0)
            prnData( fout, aDts[jj].index_i,
                     aFmts[jj], aDts[jj] );
          else
           prnData( fout, ii, aFmts[jj], aDts[jj] );
       }
        fout << "\n";
    }
    skipSpace();
    //
    /*while( *input == ' ' || *input == '\n' || *input == '\t')
           input++; */
  }
}

TPrintData::~TPrintData()
{
     aFmts.Clear();  // list of formats
     aDts.Clear();     // list of datas
}


bool
TPrintData::getFormat( const char * fmt )
{
 int i = 0;

 // test format % [flags] [width] [.prec] type_char
 // flags: '+' or '-';  width = nn; [.prec]= .nn
 // type_char
 // s	Prints characters until a null-terminator
 // f	Floating point	signed value of the form [-]dddd.dddd.
 // e	Floating point	signed value of the form [-]d.dddde[+/-]ddd
 // g	Floating point	signed value in either e or f form,
 //     based on given value and precision.

 if( fmt[i++] != '%')
    return false;
 if( fmt[i] == '-')
    i++;
 if( fmt[i] == '+')
    i++;
 while( isdigit(fmt[i]))
    i++;
 if( fmt[i] == '.')
    i++;
 while( isdigit(fmt[i]))
    i++;
 if( fmt[i] == 's' || fmt[i] == 'f' ||
     fmt[i] == 'e' || fmt[i] == 'g' )
 {
    aFmts.Add( new PFormat( fmt[i], gstring( fmt, 0, i) ) );
    input += i+1;
    return true;
 }
 return false;
}

// skip  ' ',  '\n', '\t' and comments (from '$' to end of line)
void
TPrintData::skipSpace( )
{
 while( *input == '$' || *input == ' ' ||
        *input == '\n' || *input == '\t')
 {
   if( *input == '$' )
    do{
       input++;
      }while( *input != '\n' && *input != '\0');
   if( *input == '\0' )
     return;
   input++;
  }
}


// special words or #obj_name,  #obj_name[i], #obj_name[i,j]
int
TPrintData::getToken( int& ii, int& jj )
{
 int i, data;
 gstring str;

 skipSpace();
/* while( *input == ' ' || *input == '\n' || *input == '\t')
  input++;
*/
 if( *input == '"') // line
 {
    input++;
    i=0;
    while( input[i] != '\0')
       if( input[i] == '"' /*&&  input[i-1] != '\\'*/)
         break;
       else
          i++;
    ii = i;
/*    str = gstring( input, 0, i );
    if( *input == '\'')
      input++;
*/
    data = text_d;
    return data;
 }
 if( *input != '#') // special world
 {
   i=0;
   while( isalpha( input[i] ))
          i++;

/*   while( input[i] != ' ' && input[i] != ',' && input[i] != '\n'
   && input[i] != '\t' && input[i] != '$' && input[i] != '\0')
          i++;
*/
    str = gstring( input, 0, i );

    if( str.equals("substr") )
    {  data = substr_d;
       input += i;

       skipSpace();
       if( *input == '[' )  // sizes
       {
        input++;
        skipSpace();
        i=0;
        while( isdigit(input[i]) && input[i] != '\0')
               i++;
        if( i > 0 ) sscanf( input, "%d", &ii);
        input += i;
        skipSpace();
        if( *input == ',' )
        {
          input++;
          skipSpace();
          i=0;
          while( isdigit(input[i]) && input[i] != '\0')
                i++;
          if( i > 0 ) sscanf( input, "%d", &jj);
          input += i;
        }
        skipSpace();
        if( *input != ']')
        {  gstring str_err = "Illegal format: \n";
               str_err += input;
            Error( key_format, str_err.c_str() );
        }
        input++;
      }
      return data;
    }
    if( str.equals("list") ) data = list_d;
    else if( str.equals("line") ) data = line_d;
      else if( str.equals("all") ) data = all_d;
         else if( str.equals("label") ) data = label_d;
            else if( str.equals("rkey") ) data = rkey_d;
                else if( str.equals("index") ) data = index_d;
                   else  if( str.equals("space") ) data = space_d;
                      else  if( str.equals("date") ) data = date_d;
                          else  if( str.equals("time") ) data = time_d;
    else
    {  gstring str_err = "Illegal token: \n";
           str_err += str;
       if( i==0 )
           str_err += input;
       Error( key_format, str_err.c_str() );
    }
    input += i;
 }
 else  // obj_name
 {
    input++;
    i=0;
    while( input[i] != ',' && input[i] != ' ' && input[i] != '$'
           && input[i] != '\t'&&
           input[i] != '[' && input[i] != '\0'&& input[i] != '\n')
       i++;
    str = gstring( input, 0, i );
    data = aObj.Find( str.c_str() );
    if( data < 0 )
    {  gstring str_err = "Illegal object name: \n";
           str_err += str;
       if( i==0 )
           str_err += input;
       Error( key_format.c_str(), str_err.c_str() );
    }
    input += i;
    skipSpace();
    if( *input == '[' )  // sizes
    {
       input++;
       skipSpace();
       i=0;
       while( isdigit(input[i]) && input[i] != '\0')
              i++;
       if( i > 0 ) sscanf( input, "%d", &ii);
       input += i;
       skipSpace();
       if( *input == ',' )
       {
          input++;
          skipSpace();
          i=0;
          while( isdigit(input[i]) && input[i] != '\0')
                i++;
          if( i > 0 ) sscanf( input, "%d", &jj);
          input += i;
       }
     skipSpace();
     if( *input != ']')
     {  gstring str_err = "Illegal format: \n";
           str_err += input;
       Error( key_format, str_err.c_str() );
     }
    input++;
   }
  }

  return data;
}



void
TPrintData::getData( )
{
 bool _all = false;
 bool _label  = false;
 int i=0, j=-1;
 int bg_s=-1, end_s=-1;

 int _data = getToken( i, j );

 if( _data == all_d)
 {
  _all = true;
   i=0; j=-1;
  _data = getToken( i, j );
 }

 if( _data == label_d)
 {
  _label = true;
   i=0; j=-1;
  _data = getToken( i, j );
 }

 if( _data == substr_d)
 {
   bg_s = i;
   end_s = j;
   i=0; j=-1;
   _data = getToken( i, j );
 }

 if( _data == text_d )
 {
   aDts.Add( new PData( _data, gstring( input, 0, i ).c_str() ) );
   input += i;
   if( *input == '"')
      input++;
   return;
  }
 if( _data < time_d )
 {
   gstring str = "Error in format:\n";
           str+= input;
   Error( key_format, str.c_str());
 }
 if( j < 0  )
 { j=i; i=0; }
 aDts.Add( new PData(_data, i, j, bg_s, end_s, _all, _label));

}

void
TPrintData::prnData( fstream& fout, int ind, PFormat& fmt, PData& dt )
{
  vstr strbuf(500);
  switch( dt.data )
  {
    case space_d: fmt.type = 's';
                 sprintf( strbuf, fmt.FmtOut().c_str(), " ");
                 fout << strbuf << " ";
                 break;
    case index_d: fmt.type = 'd';
                 sprintf( strbuf, fmt.FmtOut().c_str(), ind );
                 fout << strbuf << " ";
                 break;
    case rkey_d: fmt.type = 's';
                 sprintf( strbuf, fmt.FmtOut().c_str(), rt[nRT].PackKey() );
                 fout << strbuf << " ";
                 break;
    case date_d: fmt.type = 's';
                 sprintf( strbuf, fmt.FmtOut().c_str(), curDate().c_str() );
                 fout << strbuf << " ";
                 break;
    case time_d: fmt.type = 's';
                 sprintf( strbuf, fmt.FmtOut().c_str(), curTime().c_str() );
                 fout << strbuf << " ";
                 break;
    case text_d: fmt.type = 's';
                 sprintf( strbuf, fmt.FmtOut().c_str(), dt.line.c_str() );
                 fout << strbuf << " ";
                 break;
    default:     // objects
           {  if( dt.is_label )    // set format to output
                 fmt.type = 's';
              else
               if( fmt.type != 's')
               {
                   switch ( aObj[dt.data].GetType() )
                   {
                    case F_: break;
                    case D_: fmt.long_ = 'l'; break;
                    default: fmt.type = 's';
                   }
               }
              int count = 1;
              if( dt.is_all )
                  count = aObj[dt.data].GetM();
              for(int ii=0; ii<count; ii++)
              {
               if( dt.is_all )
                  dt.index_j = ii;
               if( dt.is_label )
                 sprintf( strbuf, fmt.FmtOut().c_str(),
                        aObj[dt.data].GetKeywd() );
               else
               {
                 if( fmt.type == 's' )
                 {
                   gstring str_data;
                   if( dt.end_sub > 0 && ( aObj[dt.data].GetType() >= 0 ) )
                   { // substring only for string type
                    str_data = aObj[dt.data].GetString( ind, dt.index_j );
                    str_data = str_data.substr( dt.bg_sub, dt.end_sub );
                    }
                   else
                     str_data = aObj[dt.data].GetStringEmpty( ind, dt.index_j );
                   sprintf( strbuf, fmt.FmtOut().c_str(), str_data.c_str() );
                  }
                  else // float or double data
                  {
                     if( aObj[dt.data].IsNull()  ||
                       aObj[dt.data].IsEmpty( ind, dt.index_j) )
                     {
                       char oldtype = fmt.type;
                       fmt.type = 's';
                       sprintf( strbuf, fmt.FmtOut().c_str(), S_EMPTY );
                       fmt.type = oldtype;
                     }
                     else sprintf( strbuf, fmt.FmtOut().c_str(),
                           aObj[dt.data].Get( ind, dt.index_j) );
                  }
                }
                fout << strbuf << " ";
               } // ii
          }
    } // switch
}
//--------------------- End of t_print.cpp ---------------------------

