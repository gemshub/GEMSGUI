//-------------------------------------------------------------------
// Id: gems/src/kernel/t_read.cpp  version 2.0.0  edited 2000-07-10
//
// Implementation of TReadData class
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

#include <stdio.h>

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
         aList.Add(false);
     else {
         aList.Add(true);
         input += 4;
         skipSpace();
       }
     getFormat();
     getData( aList[aList.GetCount()-1] );
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
            {  gstring str_err = "Illegal condition: \n";
               str_err += input;
               Error( key_format.c_str(), str_err.c_str() );
            }
            mScript = gstring( input, 0, pose-input );
            input = pose+2;
            rpn.GetEquat( (char *)mScript.c_str() );
   }
}

TReadData::~TReadData()
{
     aList.Clear();
     aFmts.Clear();  // list of formats
     aDts.Clear();     // list of datas
}

// Format:
//        %s	        Read characters until a " "
//             using only SetString() => no use any formats
//        EMPTY         Set empty value
//        SKIP          Skip value
//        "characters"  Set values for object (SetString())
void
TReadData::getFormat()
{
  if( *input == '%s' )
  {
    int i = 1;
    while( isdigit(input[i]))
     i++;
     if( input[i] != 's' )
     {  gstring str_err = "Illegal format: \n";
        str_err += input;
        Error( key_format.c_str(), str_err.c_str() );
      }
     aFmts.Add( new RFormat( read_r, gstring( input, 0, i+1) ) );
     input+=i+1;
  }
  else if( strncmp( input, "EMPTY", 5 ) )
       {
         aFmts.Add( new RFormat( empty_r, S_EMPTY );
         input+=5;
       }
       else if( *input == '\"' )
            {
              input++;
              char* pose = strchr( input, '\"');
              if( !pose )
              {  gstring str_err = "Illegal string: \n";
                 str_err += input;
                 Error( key_format.c_str(), str_err.c_str() );
              }
              aFmts.Add( new RFormat( string_r,
                            gstring( input, 0, pose-input ) );
              input = pose+1;
            }
 else
  {  gstring str_err = "Illegal format: \n";
     str_err += input;
     Error( key_format.c_str(), str_err.c_str() );
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
 gstring str;
 int ii=0, jj=0;

 if( isList )
 ii = jj = 1;

 if( *input != '#') // special world
 {
   gstring str_err = "Must be #: \n";
   str_err += input;
   Error( key_format.c_str(), str_err.c_str() );
 }
 input++;
 i=0;
 while( input[i] != ',' && input[i] != ' ' && input[i] != '$'
           && input[i] != '\t'&&
           input[i] != '[' && input[i] != '\0'&& input[i] != '\n')
 i++;
 str = gstring( input, 0, i );
 if( str == "RKEY" )
   data = rkey_s;
 else if( str == "SKIP" )
        data = skip_s;
      else data = aObj.Find( str.c_str() );
 if( data == -1 )
 {  gstring str_err = "Illegal object name: \n";
           str_err += str;
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

  aDts.Add( new RData( data, ii, jj ) );
  skipSpace();
}


void
TReadData::prnData( fstream& fin, RFormat& fmt, RData& dt )
{
  vstr strbuf(500);
  gstring dat_str;

  switch( fmt.type )
  {
      case read_r:
                 fin >> strbuf;
                 dat_str = gstring(strbuf.p);
                 break;
      case skip_r:
                 fin >> strbuf;
                 return;
      case empty_r:
      case string_r:
                 dat_str = fmt.fmt;
                 break;

  }
  if(  dt.objNum== -2 )
  { if( dt.i < rt[nRT].KeyNumFlds() )
    strnspy( rt[nRT].FldKey( dt.i ), dat_str.c_str(), rt[nRT].FldLen( dt.i ));
  }
  else if(  dt.objNum > 0  )
         aObj[dt.data].SetString( dat_str.c_str(), dt.i, dt.j );

}

void  TReadData::readRecord( fstream& fread )
{

 for(uint ii=0; ii<aList.GetCount(); ii++)
 {
   if( aList[ii] == false )
    prnData( fread, aFmts[ii], aDts[ii] );
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
      prnData( fread, aFmts[ii], aDts[ii] );
     }
  }
 }
 rpn.CalcEquat();
}


//--------------------- End of t_print.cpp ---------------------------

