//-------------------------------------------------------------------
// Id: gems/vizor/kernel/v_user.cpp  version 2.0.0    2001
//
// Declaration of miscellaneous utility functions and classes
//
// Created : 970207    Modified: 010908
//
// Copyright (C) 1996-2001 A.Rysin, S.Dmytriyeva
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

#include <malloc.h>
#include <time.h>

#include "v_user.h"

//____________________________________________________________________________
//
// Memory allocation
//____________________________________________________________________________


void
u_makepath(gstring& Path, const gstring& dir,
           const gstring& name, const gstring& ext)
{
    Path = dir;
    Path += "/";
    Path += name;
    Path += ".";
    Path += ext;
}


void
u_splitpath(const gstring& Path, gstring& dir,
            gstring& name, gstring& ext)
{
    size_t pos = Path.rfind("/");
    if( pos != gstring::npos )
        dir = Path.substr(0, pos);
    else
        dir = "",
              pos = 0;

    size_t pose = Path.rfind(".");
    if( pose != gstring::npos )
    {
        ext = Path.substr( pose+1, gstring::npos );
        name = Path.substr(pos+1, pose-pos-1);
    }
    else
    {
        ext = "";
        name = Path.substr(pos+1, gstring::npos);
    }
}


istream&
u_getline(istream& is, gstring& str, char delim)
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


gstring curDate()
{
    struct tm *time_now;
    time_t secs_now;

    tzset();
    time(&secs_now);
    time_now = localtime(&secs_now);

    vstr tstr(11);

    strftime(tstr, 11,
             "%d/%m/%Y",
             time_now);

    return tstr.p;
}


gstring curTime()
{
    struct tm *time_now;
    time_t secs_now;

    tzset();
    time(&secs_now);
    time_now = localtime(&secs_now);

    vstr tstr(6);

    strftime(tstr, 6,
             "%H:%M",
             time_now);

    return tstr.p;
}


void
StripLine(gstring& line)
{
    /*
    // stripping spaces
        size_t ii;
        for( ii=0; ii<line.length() && IsSpace(line[ii]); ii++ );
    //    line.erase(0,ii);
        if( ii>=line.length() )//line.empty() )
          return;
        int jj = line.length()-1;  
        for( ; jj>=0 && IsSpace(line[jj]); jj--);
        line = line.substr(ii, jj-ii+1);
    */
    line.strip();
}

//Added Sveta 24/12/2001

char chLowUp(char ch)
{
   char ch1;

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
  ChangeforTempl( gstring& data_str,  const gstring& from_templ1,
                  const gstring& to_templ1, uint len_ )
{
    if( data_str.empty() )
        return;

    gstring  from_templ = from_templ1;
    gstring  to_templ = to_templ1;
    bool inv_case = false;
    uint ii;
    data_str.strip();

    if( from_templ.equals("*") )
    {
      if( to_templ[0] == '*' )
      {
       if( data_str.length() >= len_ )
        data_str = gstring( data_str, 0, len_-1 );
       data_str += gstring(to_templ, 1);
      }
      else
       if( to_templ.equals("invcase") )
        for( ii=0; ii<data_str.length(); ii++)
         data_str[ii] = chLowUp( data_str[ii]);
       else
         data_str = to_templ;

      data_str.substr(0, len_);
      return;
    }

    // Changed first n symbols
    if( gstring( to_templ, 0, 7).equals("invcase") )
    {  inv_case = true;
       to_templ = gstring( to_templ, 7 );
    }

    ii=0;
    int k=0, pos_to;
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
       Error( from_templ.c_str(), "Illegal simbol in template ");
     ii++;
   }
   data_str = gstring( data_str, k );
   pos_to = to_templ.find( "*" );
   data_str = gstring( to_templ, 0, pos_to) + data_str;
   if( pos_to  == gstring::npos )
    to_templ = "";
   else
    to_templ = gstring( to_templ, pos_to+1);
   pos_to = from_templ.find( "*" );
   if( pos_to  == gstring::npos )
    from_templ = "";
   else
    from_templ = gstring( from_templ, pos_to+1);

   // Changed last n symbols
   inv_case = false;
   if( to_templ.equals("invcase") )
   {  inv_case = true;
      to_templ = "";
   }

   int jj, ik;
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
       Error( from_templ.c_str(), "Illegal simbol in template ");
     ik--;
     jj--;
   }
   data_str = gstring( data_str, 0, k );
   data_str += to_templ;
   data_str.substr(0, len_);
}


//--------------------- End of v_user.cpp ---------------------------


