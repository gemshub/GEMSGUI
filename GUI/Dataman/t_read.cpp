//-------------------------------------------------------------------
// $Id: t_read.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of TReadData class
//
// Copyright (C) 2001  S.Dmytriyeva
// Uses  string class (C) A.Rysin 1999
//
// This file is part of the GEM-Selektor GUI library which uses the
// Qt v.4 cross-platform App & UI framework (http://qt.nokia.com)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the terms of GEMS3 Development
// Quality Assurance Licence (GEMS3.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#include <cstdio>
#include "t_read.h"
#include "v_object.h"
#include "v_dbm.h"
#include "v_user.h"
#include "service.h"


TReadData::TReadData(const char *sd_key,
    int nrt, const char *fmt_text ):
  key_format( sd_key ), nRT(nrt)
{
  input = (char*)fmt_text;
  skipSpace();
  do{
      if( *input == ',')
      input++;
      skipSpace();
     if( strncmp( input, "list", 4 ) )
         aList.push_back(false);
     else {
         aList.push_back(true);
         input += 4;
         skipSpace();
       }
     getFormat();
     getData( aList.back() );
     skipSpace();
    } while( *input == ',' );
  // Math scripts
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
            mScript = string( input, 0, pose-input );
            input = pose+2;
            rpn.GetEquat( (char *)mScript.c_str() );
   }
}

TReadData::~TReadData()
{
     aList.clear();
     aFmts.clear();  // list of formats
     aDts.clear();     // list of datas
}

// Format:
//        %s	        Read characters until a " "
//             using only SetString() => no use any formats
//        EMPTY          Set empty value
//        IREC           Set reading record number
//        "characters"   Set string value
//        #obj_name
//        #obj_name[i]
//        #obj_name[i,j] Set values from object (GetString())
void
TReadData::getFormat()
{
  switch( *input )
  {
   case '%': {
       int i = 1;
       int size = 400;
       while( isdigit(input[i]))
               i++;
       if( input[i] != 's' )
       {  string str_err = "Invalid format (must be '%nns'): \n";
                str_err += input;
         Error( key_format, str_err );
       }
       if( i>1 ) //digits
       {
        sscanf(input+1, "%d", &size );
       }
       aFmts.push_back( RFormat( read_r, size,
               string( input, 0, i+1) ) );
       input+=i+1;
       break;
       }
   case 'I':
       if( !strncmp( input, "IREC", 4 ) )
       {
         aFmts.push_back( RFormat( irec_r, 0, "" ));
         input+=4;
       } else
       {string str_err = "Invalid format (must be IREC): \n";
                str_err += input;
         Error( key_format, str_err );
       }
       break;
   case 'E':
       if( !strncmp( input, "EMPTY", 5 ) )
       {
         aFmts.push_back( RFormat( empty_r, 0, S_EMPTY ));
         input+=5;
       } else
         { string str_err = "Invalid format (must be EMPTY): \n";
                str_err += input;
           Error( key_format, str_err );
          }
       break;
   case '#': {
        input++;
        int  ii, jj, i=0;
        while( input[i] != ',' && input[i] != ' ' &&
               input[i] != '$'  && input[i] != '\t'&&
               input[i] != '[' && input[i] != '\0'&&
               input[i] != '\n')
           i++;
        string str = string( input, 0, i );
        int data = aObj.Find( str.c_str() );
        if( data == -1 )
        {  string str_err = "Invalid object name: \n";
            str_err += str;
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
          if( i > 0 )
            sscanf( input, "%d", &ii);
          input += i;
          skipSpace();
          if( *input == ',' )
          {
            input++;
            skipSpace();
            i=0;
            while( isdigit(input[i]) && input[i] != '\0')
                i++;
            if( i > 0 )
              sscanf( input, "%d", &jj);
            input += i;
          }
          skipSpace();
          if( *input != ']')
           {  string str_err = "Invalid format (left ']'): \n";
               str_err += input;
              Error( key_format, str_err );
            }
          input++;
        }
        aFmts.push_back(  RFormat( object_r, data, ii, jj ));
       }
       break;
   case '\"': {
       input++;
       char* pose = strchr( input, '\"');
       if( !pose )
       {  string str_err = "Invalid string ( left simbol '\"'): \n";
              str_err += input;
          Error( key_format, str_err );
       }
       aFmts.push_back(  RFormat( string_r, 0,
                   string( input, 0, pose-input ) ));
       input = pose+1;
       }
       break;
   default:
      {  string str_err = "Invalid format: \n";
         str_err += input;
         Error( key_format, str_err );
      }
   }
  skipSpace();
}


// skip  ' ',  '\n', '\t' and comments (from '$' to end of line)
void
TReadData::skipSpace( )
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


// #RKEY[i] or #obj_name,  #obj_name[i], #obj_name[i,j], #SKIP
void
TReadData::getData( bool isList )
{
 int i, data;
 string str;
 int ii=0, jj=0;

 if( isList )
 ii = jj = 1;

 if( *input != '#') // special world
 {
   string str_err = "Must be #: \n";
   str_err += input;
   Error( key_format, str_err );
 }
 input++;
 i=0;
 while( input[i] != ',' && input[i] != ' ' && input[i] != '$'
           && input[i] != '\t'&&
           input[i] != '[' && input[i] != '\0'&& input[i] != '\n')
 i++;
 str = string( input, 0, i );
 if( str == "RKEY" )
   data = rkey_s;
 else if( str == "SKIP" )
        data = skip_s;
      else data = aObj.Find( str.c_str() );
 if( data == -1 )
 {  string str_err = "Invalid object name: \n";
           str_err += str;
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

  aDts.push_back(  RData( data, ii, jj ) );
  skipSpace();
}


void
TReadData::readData( fstream& fin, RFormat& fmt, RData& dt )
{
  char strbuf[500];
  string dat_str;

  
  switch( fmt.type )
  {
      case read_r:
                 if( fmt.size < 400  )
                  fin.read( strbuf, fmt.size );
                 else
                  fin >> strbuf;
                 dat_str = string(strbuf);
                 break;
      case empty_r:
      case string_r:
                 dat_str = fmt.fmt;
                 break;
      case irec_r:
                 sprintf( strbuf, "%d", fmt.size );
                 dat_str = string(strbuf);
                 break;
      case object_r:
                 dat_str = aObj[fmt.size].GetStringEmpty( fmt.i, fmt.j );
                 break;

  }
  if(  dt.objNum== -2 )
  { if( dt.i < rt[nRT].KeyNumFlds() )
    {
      rt[nRT].SetFldKey( dt.i, dat_str.c_str());
    }
  }
  else if(  dt.objNum > 0  )
    aObj[dt.objNum].SetString( dat_str.c_str(), dt.i, dt.j );

}

void  TReadData::readRecord( int n_itr, fstream& fread )
{

 for(int ii=0; ii<aList.size(); ii++)
 {
   if( aFmts[ii].type == irec_r)
       aFmts[ii].size = n_itr;
   if( aList[ii] == false )
    readData( fread, aFmts[ii], aDts[ii] );
   else
   {
    int iN, iM;
    iN = aDts[ii].i;
    iM = aDts[ii].j;
    for( int i=0; i<iN; i++)
     for( int j=0; j<iM; j++)
     {
      aDts[ii].i = i;
      aDts[ii].j = j;
      readData( fread, aFmts[ii], aDts[ii] );
     }
    aDts[ii].i = iN;
    aDts[ii].j = iM;
   }
 }
 rpn.CalcEquat();
}


//--------------------- End of t_print.cpp ---------------------------

