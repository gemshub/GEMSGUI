//-------------------------------------------------------------------
// $Id: v_user.cpp 1373 2009-07-22 12:25:22Z gems $
//
// Declaration of miscellaneous utility functions and classes
//
// Copyright (C) 1996-2001 A.Rysin, S.Dmytriyeva
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

#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <cmath>
#include "v_user.h"

double NormDoubleRound(double aVal, int digits)
{
    double val;
    char vbuf[100];	// double is ~15 digit   PATTERN_GET()
    sprintf(vbuf, "%.*le" , digits , aVal);
    sscanf(vbuf, "%le", &val );
	return val;
}

void NormDoubleRound(double *aArr, int size, int digits)
{
    char vbuf[100];	// double is ~15 digit   PATTERN_GET()
    
	for(int ii=0; ii<size; ii++ )
    { sprintf(vbuf, "%.*le" , digits, aArr[ii]);
      sscanf(vbuf, "%le", &aArr[ii] );
    }  
}

void NormFloatRound(float *aArr, int size, int digits)
{
    char vbuf[100];	// double is ~15 digit   PATTERN_GET()
    
	for(int ii=0; ii<size; ii++ )
    { sprintf(vbuf, "%.*e" , digits, aArr[ii]);
      sscanf(vbuf, "%e", &aArr[ii] );
    }  
}

string u_makepath(const string& dir,
           const string& name, const string& ext)
{
    string Path(dir);
    if( dir != "")
      Path += "/";
    Path += name;
    Path += ".";
    Path += ext;

    return Path;
}


void
u_splitpath(const string& Path, string& dir,
            string& name, string& ext)
{
    size_t pos = Path.rfind("/");
    if( pos != string::npos )
        dir = Path.substr(0, pos), pos++;
    else
        dir = "",
              pos = 0;

    size_t pose = Path.rfind(".");
    if( pose != string::npos )
    {
        ext = Path.substr( pose+1, string::npos );
        name = Path.substr(pos, pose-pos);
    }
    else
    {
        ext = "";
        name = Path.substr(pos, string::npos);
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

// read string as: "<characters>",
istream&
f_getline(istream& is, string& str, char delim)
{
    char ch;
    is.get(ch);
    str="";

    while( is.good() && ( ch==' ' || ch=='\n' || ch== '\t') )
        is.get(ch);
    if(ch == '\"')
        is.get(ch);
    while( is.good() &&  ch!=delim && ch!= '\"' )
    {
        str += ch;
        is.get(ch);
    }
    while( !is.eof() &&  ch!=delim )
            is.get(ch);

   return is;
}

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


const int bGRAN = 20;

// Get Path of file and Reading list of file names from it, return number of files
char  (* f_getfiles(const char *f_name, char *Path, 
		long int& nElem, char delim ))[fileNameLength]
{
  int ii, bSize = bGRAN;
  char  (*filesList)[fileNameLength];
  char  (*filesListNew)[fileNameLength];
  filesList = new char[bSize][fileNameLength];
  string name;

// Get path
   string path_;
   string flst_name = f_name;
   size_t pos = flst_name.rfind("/");
   path_ = "";
   if( pos < string::npos )
      path_ = flst_name.substr(0, pos+1);
   strncpy( Path, path_.c_str(), 256-fileNameLength);
   Path[255] = '\0';
     
//  open file stream for the file names list file
   fstream f_lst( f_name/*flst_name.c_str()*/, ios::in );
   ErrorIf( !f_lst.good(), f_name, "Fileopen error");

// Reading list of names from file	
  nElem = 0;
  while( !f_lst.eof() )
  {
	f_getline( f_lst, name, delim);
    if( nElem >= bSize )
    {    bSize = bSize+bGRAN;
         filesListNew = new char[bSize][fileNameLength];
         for( ii=0; ii<nElem-1; ii++ )
		   strncpy( filesListNew[ii], filesList[ii], fileNameLength);
	     delete[] filesList;
		 filesList =  filesListNew;
	}
    strncpy( filesList[nElem], name.c_str(), fileNameLength);
	filesList[nElem][fileNameLength-1] = '\0';
    nElem++; 
  }
  
  // Realloc memory for reading size
  if( nElem != bSize )
  {    
    filesListNew = new char[nElem][fileNameLength];
    for(  ii=0; ii<nElem; ii++ )
	  strncpy( filesListNew[ii], filesList[ii], fileNameLength);
	delete[] filesList;
	filesList =  filesListNew;
  }

  return filesList;	
}

// Shows information on current record
string timeToStr( time_t time )
{
    struct tm *time_s;
    tzset();
    time_s = localtime(&time);
    char str[100];
    strftime(str, 20, "%d/%m/%Y, %H:%M", time_s);
    return string(str);
}

string curDate()
{
    struct tm *time_now;
    time_t secs_now;

    tzset();
    time(&secs_now);
    time_now = localtime(&secs_now);

    char tstr[100];

    strftime(tstr, 11,
             "%d/%m/%Y",
             time_now);

    return tstr;
}

string curDateSmol(char ch )
{
    struct tm *time_now;
    time_t secs_now;

    tzset();
    time(&secs_now);
    time_now = localtime(&secs_now);

    char tstr[100];

    string frm = "%d" + string(1,ch)+ "%m" + string(1,ch) + "%y";
    strftime(tstr, 9, frm.c_str(),
             // "%d/%m/%y",
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

    char tstr[100];
    strftime(tstr, 6,
             "%H:%M",
             time_now);

    return tstr;
}


void StripLine(string& line)
{
   strip( line );
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
    strip(data_str);
    string old_str = data_str;

    if( from_templ== "*" ) // all key
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

      data_str.substr(0, len_);
      if( data_str == old_str )
      {
       size_t ii = data_str.length()-1;
       if( data_str[ii] == '9' )
        data_str[ii] = '_';
       else
        data_str[ii] = '9';
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
       Error( from_templ, "Error (1): Invalid character in template ");
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
    jj = from_templ.length()-1;
    ik = data_str.length()-1;
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
       Error( from_templ, "Error (2): Invalid character in template ");
     ik--;
     jj--;
   }
   data_str = string( data_str, 0, k );
   data_str += to_templ;
   data_str.substr(0, len_);
   if( data_str == old_str )
   {
       int ii = data_str.length()-1;
       if( data_str[ii] == '9' )
        data_str[ii] = '_';
       else
        data_str[ii] = '9';
    }
}


//--------------------- End of v_user.cpp ---------------------------


