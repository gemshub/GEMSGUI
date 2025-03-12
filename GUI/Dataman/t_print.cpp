//-------------------------------------------------------------------
// $Id: t_print.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of TPrintData class
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

#include <cstdio>
#include "t_print.h"
#include "v_object.h"
#include "v_dbm.h"
#include "v_user.h"
#include "service.h"
#include "GEMS3K/v_detail.h"

TPrintData::TPrintData(const char *sd_key,
    unsigned int nrt, fstream& fout, const char *fmt_text ):
  key_format( sd_key ), nRT(nrt)
{
  ErrorIf( !fout.good() , rt[nRT]->GetKeywd(), "Fileopen error");
  input = const_cast<char*>(fmt_text);

  prr = 1;
  iir = 0;
  uint o_prn_= aObj.Find("prn");
  uint o_ii_ = aObj.Find("ii");

  aObj[o_prn_]->SetPtr(&prr);
  aObj[o_ii_]->SetPtr(&iir);

  while( *input )
  {
    int i=0, j=-1, count=0;
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
              count = aObj[code]->GetN();
              if( aObj[code]->IsNull() )
               count = 0;
            }
            else
              count = 1;
       }
       else
           {  string str_err = "Invalid command: \n";
              str_err += input;
              Error( key_format, str_err );
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
            {  string str_err = "Invalid condition: \n";
               str_err += input;
               Error( key_format, str_err );
            }
            cond = char_array_to_string( input, pose-input );
            input = pose+2;
            ifcond = true;
            try{
                rpn.GetEquat( const_cast<char *>(cond.c_str()) );
               }
            catch( TError& xcpt )
            {
               if( vfQuestion(nullptr, xcpt.title.c_str(), (xcpt.mess+" \nContinue?").c_str() ))
                {
                   ifcond = false;
                }
               else
                     Error(xcpt.title, xcpt.mess);
             }
    }
    // end insert

     aFmts.clear();  // list of formats
     aDts.clear();     // list of datas
     bool showMss = true;
     //Read formats
     do{
          if( *input == ',')
            input++;
          skipSpace();
          /*while( *input == ' ' || *input == '\n' || *input == '\t')
                  input++; */
          if( !getFormat( input ) )
          {  string str_err = "Wrong or missing format: \n";
               str_err += input;
            Error( key_format, str_err );
          }   // added by KD 17.06.2002 
          getData();
          skipSpace();
          /*while( *input == ' ' || *input == '\n' || *input == '\t')
                  input++;*/
    }while( *input == ',' /**input != '\0'*/ );
    //Print formats
    for( int ii=0; ii<count; ii++ )
    {
       if( ifcond )
       {
         prr = 1;
         iir = ii;
         try{
              rpn.CalcEquat();
             }
         catch( TError& xcpt )
         {
            if( showMss )
             {  //xcpt.mess += " \nContinue?";
                switch( vfQuestion3(nullptr, xcpt.title, xcpt.mess+" \nContinue?",
                           "&Yes", "&No", "&Yes to All" ))
                {
                  case VF3_3:  showMss=false;
                               break;
                  case VF3_1:  break;
                  case VF3_2:  Error(xcpt.title, xcpt.mess);
                }
            }
            prr = 0;
        }
        if( prr == 0 )
          continue;
       }
       for( size_t jj=0; jj<aDts.size(); jj++ )
       {
          if( code == line_d && aDts[jj].index_i != 0)
            prnData( fout, aDts[jj].index_i, aFmts[jj], aDts[jj] );
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
     aFmts.clear();  // list of formats
     aDts.clear();     // list of datas
}


bool
TPrintData::getFormat( const char * fmt )
{
 size_t i = 0;

 // test format % [flags] [width] [.prec] type_char
 // flags: '+' or '-';  width = nn; [.prec]= .nn
 // type_char
 // s	Prints characters until a null-terminator
 // c	Prints character in "'" == 'c'
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
 if( fmt[i] == 's' || fmt[i] == 'c' || fmt[i] == 'f' ||
     fmt[i] == 'e' || fmt[i] == 'g' )
 {
    aFmts.push_back( PFormat( fmt[i], char_array_to_string( fmt, i) ) );
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
 int i;
 int data=0;
 string str;

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
/*    str = char_array_to_string( input, i );
    if( *input == '\'')
      input++;
*/
    data = text_d;
    return data;
 }
 if( *input != '#') // special word
 {
   i=0;
   while( isalpha( input[i] ))
          i++;

/*   while( input[i] != ' ' && input[i] != ',' && input[i] != '\n'
   && input[i] != '\t' && input[i] != '$' && input[i] != '\0')
          i++;
*/
    str = char_array_to_string( input, i );

    if( str == "substr" )
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
        {  string str_err = "Invalid format: \n";
               str_err += input;
            Error( key_format, str_err );
        }
        input++;
      }
      return data;
    }
    if( str == "list" ) data = list_d;
    else if( str =="line" ) data = line_d;
      else if( str =="all" ) data = all_d;
         else if( str == "label" ) data = label_d;
            else if( str == "rkey" ) data = rkey_d;
                else if( str == "index" ) data = index_d;
                   else  if( str == "space" ) data = space_d;
                      else  if( str == "date" ) data = date_d;
                          else  if( str == "time" ) data = time_d;
    else
    {  string str_err = "Invalid token: \n";
           str_err += str;
       if( i==0 )
           str_err += input;
       Error( key_format, str_err );
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
    str = char_array_to_string( input, i );
    data = aObj.Find( str.c_str() );
    if( data < 0 )
    {  string str_err = "Invalid object name: \n";
           str_err += str;
       if( i==0 )
           str_err += input;
       Error( key_format, str_err );
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
     {  string str_err = "Invalid format: \n";
           str_err += input;
       Error( key_format, str_err );
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
   aDts.push_back( PData( _data, char_array_to_string( input, i ).c_str() ) );
   input += i;
   if( *input == '"')
      input++;
   return;
  }
 if( _data < time_d )
 {
   string str = "Error in format:\n";
           str+= input;
   Error( key_format, str);
 }
 if( j < 0  )
 { j=i; i=0; }
 aDts.push_back( PData(_data, i, j, bg_s, end_s, _all, _label));

}

const string emptiness("---");

void
TPrintData::prnData( fstream& fout, int ind, PFormat& fmt, PData& dt )
{
  char strbuf[8192];
  string format = fmt.FmtOut();
  switch( dt.data )
  {
    case space_d: fmt.type = 's';
                 sprintf( strbuf, format.c_str(), " ");
                 fout << strbuf << " ";
                 break;
    case index_d: fmt.type = 'd';
                 sprintf( strbuf, format.c_str(), ind );
                 fout << strbuf << " ";
                 break;
    case rkey_d: fmt.type = 's';
                 sprintf( strbuf, format.c_str(), rt[nRT]->PackKey() );
                 fout << strbuf << " ";
                 break;
    case date_d: fmt.type = 's';
                 sprintf( strbuf, format.c_str(), curDate().c_str() );
                 fout << strbuf << " ";
                 break;
    case time_d: fmt.type = 's';
                 sprintf( strbuf, format.c_str(), curTime().c_str() );
                 fout << strbuf << " ";
                 break;
    case text_d: fmt.type = 's';
                 sprintf( strbuf, format.c_str(), dt.line.c_str() );
                 fout << strbuf << " ";
                 break;
    default:     // objects
           {  if( dt.is_label )    // set format to output
                 fmt.type = 's';
              else
               if( fmt.type != 's' && fmt.type != 'c' )
               {
                   switch ( aObj[dt.data]->GetType() )
                   {
                    case F_: break;
                    case D_: fmt.long_ = 'l'; break;
                    default: fmt.type = 's';
                   }
               }
              int scalar = 0, Ndim, Mdim;
              int count = 1;
              Mdim = aObj[dt.data]->GetM();
              Ndim = aObj[dt.data]->GetN();
              if( dt.is_all )
                  count = Mdim;
              if( Mdim * Ndim == 1 )
                  scalar = 1;
              for(int ii=0; ii<count; ii++)
              {
               if( dt.is_all )
                  dt.index_j = ii;
               if( dt.is_label )
               { // workaround - skip printing DOD labels and data
                 // if dynamic object is not allocated
                 if( !aObj[dt.data]->IsNull() )
                    sprintf( strbuf, format.c_str(),
                        aObj[dt.data]->GetKeywd() );
               }
               else
               {
                 if( fmt.type == 's' || fmt.type == 'c' )
                 {
                   string str_data;
                   if( dt.end_sub > 0 && ( aObj[dt.data]->GetType() >= 0 ) )
                   { // substring only for string type
                    str_data = aObj[dt.data]->GetString( ind, dt.index_j );
                    str_data = str_data.substr( dt.bg_sub, dt.end_sub );
                    }
                   else
                     str_data = aObj[dt.data]->GetStringEmpty( ind, dt.index_j );
                   if( fmt.type == 's' )
                    sprintf( strbuf, format.c_str(), str_data.c_str() );
                   else //c
                    sprintf( strbuf, "'%c'", str_data.c_str()[0] );

                  }
                  else // float or double data
                  {
                     if( !aObj[dt.data]->IsNull() ) // workaround - print
                     {  // only static or allocated objects !
                        int indx;
                        if( scalar )
                           indx = 0;
                        else indx = ind;
                        if( aObj[dt.data]->IsEmpty( indx, dt.index_j) )
                        {
                          char oldtype = fmt.type;
                          fmt.type = 's';
                          sprintf( strbuf, format.c_str(), emptiness.c_str() ); // S_EMPTY );
                          fmt.type = oldtype;
                        }
                         else sprintf( strbuf, format.c_str(), aObj[dt.data]->Get( indx, dt.index_j) );
                     }
                     else
                     {
                         char oldtype = fmt.type;
                         fmt.type = 's';
                         sprintf( strbuf, format.c_str(),  "NULL" ); // S_EMPTY );
                         fmt.type = oldtype;
                      }
                 }
                }
                fout << strbuf << " ";
               } // ii
          }
    } // switch
}
//--------------------- End of t_print.cpp ---------------------------

