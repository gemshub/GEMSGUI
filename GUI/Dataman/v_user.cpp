//-------------------------------------------------------------------
// $Id: v_user.cpp 1373 2009-07-22 12:25:22Z gems $
//
// Declaration of miscellaneous utility functions and classes
//
// Copyright (C) 1996-2001 A.Rysin, S.Dmytriyeva
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

#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <cmath>
#include "v_user.h"

char chLowUp(char ch);
double NormDoubleRound(double aVal, int digits)
{
    double val;
    char vbuf[30];	// double is ~15 digit   PATTERN_GET()
    sprintf(vbuf, "%.*le" , digits , aVal);
    sscanf(vbuf, "%le", &val );
	return val;
}

void NormDoubleRound(double *aArr, int size, int digits)
{
    char vbuf[30];	// double is ~15 digit   PATTERN_GET()
    
	for(int ii=0; ii<size; ii++ )
    { sprintf(vbuf, "%.*le" , digits, aArr[ii]);
      sscanf(vbuf, "%le", &aArr[ii] );
    }  
}

void NormFloatRound(float *aArr, int size, int digits)
{
    char vbuf[30];	// double is ~15 digit   PATTERN_GET()
    
	for(int ii=0; ii<size; ii++ )
    { sprintf(vbuf, "%.*e" , digits, aArr[ii]);
      sscanf(vbuf, "%e", &aArr[ii] );
    }  
}



istream&
u_getline(istream& is, string& str, char delim)
{
    char ch;
    is.get(ch);
    str="";
    while( is.good() && ch!=delim )
    {
        str += ch;
        is.get(ch);
    }
    return is;
}

//// read string as: "<characters>",
//istream&
//f_getline(istream& is, std::string& str, char delim)
//{
//    char ch;
//    is.get(ch);
//    str="";

//    while( is.good() && ( ch==' ' || ch=='\n' || ch== '\t') )
//        is.get(ch);
//    if(ch == '\"')
//        is.get(ch);
//    while( is.good() &&  ch!=delim && ch!= '\"' )
//    {
//        str += ch;
//        is.get(ch);
//    }
//    while( !is.eof() &&  ch!=delim )
//            is.get(ch);

//   return is;
//}

// The gcvt() function converts number to a minimal length NULL terminated
// ASCII string and stores the result in buf.  It produces ndigit signifi‐
// cant digits in either printf(3) F format or E format.
void Gcvt(double number, size_t ndigit, char *buf)
{
    char internalbuf[40];
    int dg = 6;
     do{
          sprintf(internalbuf, "%.*g", dg, number);
          dg--;
      } while ( strlen(internalbuf)>ndigit && dg>0 );  
    memset( buf, 0, ndigit );
    strncpy(buf, internalbuf, ndigit );
}


string curDate()
{
    struct tm *time_now;
    time_t secs_now;

    tzset();
    time(&secs_now);
    time_now = localtime(&secs_now);

    char tstr[40];

    strftime(tstr, 11,
             "%d/%m/%Y",
             time_now);

    return tstr;
}

std::string curDateSmol(char ch )
{
    struct tm *time_now;
    time_t secs_now;

    tzset();
    time(&secs_now);
    time_now = localtime(&secs_now);

    char tstr[40];

    std::string frm = "%d" + std::string(1,ch)+ "%m" + std::string(1,ch) + "%y";
    strftime(tstr, 9, frm.c_str(),  // "%d/%m/%y",
             time_now);

    return tstr;
}

string curTime()
{
    struct tm *time_now;
    time_t secs_now;

    tzset();
    time(&secs_now);
    time_now = localtime(&secs_now);

    char tstr[40];

    strftime(tstr, 6,
             "%H:%M",
             time_now);

    return tstr;
}



void StripLine(string& line)
{
   strip( line );
}

//void strip(string& str)
//{
//  string::size_type pos1 = str.find_first_not_of(' ');
//  string::size_type pos2 = str.find_last_not_of(' ');
//  str = str.substr(pos1 == string::npos ? 0 : pos1,
//    pos2 == string::npos ? str.length() - 1 : pos2 - pos1 + 1);
//}

//void replace( string& str, const char* old_part, const char* new_part)
//{
//    size_t pos = str.find( old_part ); //rfind( old_part );
//    if( pos != string::npos )
//    {
//        string res(str.substr(0, pos));
//        res += new_part;
//        res += str.substr( pos+strlen(old_part));
//        str = res;
//    }
//}

void replaceall( string& str, const char* old_part, const char* new_part)
{
    size_t pos = str.find( old_part ); //rfind( old_part );
    while( pos != string::npos )
    {
        string res(str.substr(0, pos));
        res += new_part;
        res += str.substr( pos+strlen(old_part));
        str = res;
        pos = str.find( old_part );
    }
}

void KeyToName(std::string& line)
{
   strip( line );
   if( line.empty())
      return;

   if( line[line.length()-1] == ':')
       line.erase(line.length()-1, 1);
   for(size_t ii=0; ii< line.length(); ++ii )
   {
     if( line[ii] == ':' || line[ii] == ' ' )
       line[ii] = '_';
   }
}

//Added Sveta 24/12/2001

char chLowUp(char ch)
{
   char ch1=ch;

   if( isalpha(ch) )
   { if( islower(ch))
       ch1 = toupper(ch);
    else
       ch1 = tolower(ch);
   }
   else
   {  if( ch == '-' )
       ch1 = '_';
      else
       if( ch == '_' )
         ch1 = '-';
   }

   return ch1;
}


//      *    *<characters> - dobavit` simvoly v konec
//
//      *    <characters> - zamenit` vse simvoly na posledomatel`nost`
//
//      *    invcase      - perevesti vse simvoly v drugoj registr
//

//      ?...?*    <characters>* - zamenit` pervye n simvolov na posledomatel`nost`
//        n          m           iz m simvolov
//
//      ?...?*  invcase* - perevesti pervye n simvolov v drugoj registr
//        n

//
//      *?...?    *<characters> - zamenit` poslednie n simvolov na posledomatel`nost`
//        n          m           iz m simvolov
//
//      *?...?  *invcase - perevesti poslednie n simvolov v drugoj registr
//        n

void
  ChangeforTempl( string& data_str,  const string& from_templ1,
                  const string& to_templ1, uint len_ )
{
    if( data_str.empty() )
        return;


    string  from_templ = from_templ1;
    string  to_templ = to_templ1;
    bool inv_case = false;
    size_t ii;
    strip( data_str );
    string old_str = data_str;

    if( from_templ =="*" ) // all key
    {
      if( to_templ.find("invcase") != string::npos )
      {
          if( to_templ == "invcase" )
            for( ii=0; ii<data_str.length(); ii++)
              data_str[ii] = chLowUp( data_str[ii]);
          else goto PART;
      }
      else
        if( to_templ[0] == '*' )
        {
         if( data_str.length() >= len_ )
          data_str = string( data_str, 0, len_-1 );
         data_str += string(to_templ, 1);
        }
        else
          if(to_templ.find("*") != string::npos) // no all case
            goto PART;
          else
            data_str = to_templ;

      data_str = data_str.substr(0, len_);
      if( data_str == old_str )
      {
          size_t ii1 = data_str.length()-1;
          if( data_str[ii1] == '9' )
              data_str[ii1] = '_';
          else
              data_str[ii1] = '9';
      }
      return;
    }

    PART:
    // Changed first n symbols
    if( string( to_templ, 0, 7) == "invcase" )
    {  inv_case = true;
       to_templ = string( to_templ, 7 );
    }

    ii=0;
    size_t k=0;
    size_t pos_to;
    while( ii<data_str.length() && ii<from_templ.length() )
    {
      if( from_templ[ii] == '*'  )
        break;
      if( from_templ[ii] == '?' )
      {
        if( inv_case )
          data_str[ii] = chLowUp( data_str[ii]);
        else
          k++;
      }
      else
       Error( from_templ.c_str(), "Error (1): Invalid character in template ");
     ii++;
   }
   data_str = string( data_str, k );
   pos_to = to_templ.find( "*" );
   data_str = string( to_templ, 0, pos_to) + data_str;
   if( pos_to  == string::npos )
    to_templ = "";
   else
    to_templ = string( to_templ, pos_to+1);
   pos_to = from_templ.find( "*" );
   if( pos_to  == string::npos )
    from_templ = "";
   else
    from_templ = string( from_templ, pos_to+1);

   // Changed last n symbols
   inv_case = false;
   if( to_templ == "invcase" )
   {  inv_case = true;
      to_templ = "";
   }
//   size_t jj, ik;
   int jj, ik;      // Bugfix 11.04.2006
    jj = static_cast<int>(from_templ.length())-1;
    ik = static_cast<int>(data_str.length())-1;
    k =  data_str.length();
    while( jj >= 0 && ik >= 0  )
    {
      if( from_templ[jj] == '*'  )
        break;
      if( from_templ[jj] == '?' )
      {
        if( inv_case )
          data_str[ik] = chLowUp( data_str[ik]);
        else
          k--;
      }
      else
       Error( from_templ.c_str(), "Error (2): Invalid character in template ");
     ik--;
     jj--;
   }
   data_str = string( data_str, 0, k );
   data_str += to_templ;
   data_str = data_str.substr(0, len_);
   if( data_str == old_str )
   {
       size_t ii1 = data_str.length()-1;
       if( data_str[ii1] == '9' )
        data_str[ii1] = '_';
       else
        data_str[ii1] = '9';
    }
}

// "a;b;c" to array { "a", "b", "c" }
TCStringArray split(const string& str, const string& delimiters)
{
    TCStringArray v;
    string vv;

    if( str.empty() )
        return v;

    string::size_type start = 0;
    auto pos = str.find_first_of(delimiters.c_str(), start);
    while(pos != string::npos)
    {
        vv = string(str, start, pos - start);
        strip( vv );
        v.push_back( vv );
        start = pos + 1;
        pos = str.find_first_of(delimiters.c_str(), start);
    }

    vv = string (str, start, str.length() - start);
    strip( vv );
    if( !vv.empty() )
        v.push_back( vv );
    return v;
}

//--------------------- End of v_user.cpp ---------------------------


