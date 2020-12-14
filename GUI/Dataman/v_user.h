//-------------------------------------------------------------------
// $Id$
/// \file v_user.h
/// Declaration of platform-specific utility functions and classes
//
// Copyright (C) 1996,2001,2012 A.Rysin, S.Dmytriyeva
// <GEMS Development Team, mailto:gems2.support@psi.ch>
//
// This file is part of the GEMS3K code for thermodynamic modelling
// by Gibbs energy minimization <http://gems.web.psi.ch/GEMS3K/>
//
// GEMS3K is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation, either version 3 of
// the License, or (at your option) any later version.

// GEMS3K is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with GEMS3K code. If not, see <http://www.gnu.org/licenses/>.
//-------------------------------------------------------------------

#ifndef _v_user_h_
#define _v_user_h_

#include <set>
#include <vector>
#include <algorithm>

#include <iostream>
#include <cstring>
using namespace std;
#include "GEMS3K/v_detail.h"


#ifdef __APPLE__

#ifndef __unix
#define __unix
#endif
#ifndef __FreeBSD
#define __FreeBSD
#endif

typedef unsigned int uint;
#endif

// added for convenience because of frequent use
typedef vector<string> TCStringArray;
// Added for convenience
typedef vector<int> TCIntArray;
const int MAXKEYWD = 6+1;

template <class T>
int findIndex( const std::vector<T>arr, const T& val )
{
    auto pos =  std::find(arr.begin(), arr.end(), val);
    if( pos != arr.end()  )
        return static_cast<int>(pos-arr.begin());
    else
        return -1;
}

#ifndef  __unix

typedef unsigned int uint;

#endif //  __noborl


void Gcvt(double number, size_t ndigit, char *buf);
double NormDoubleRound(double aVal, int digits);
void NormDoubleRound(double *aArr, int size, int digits);
void NormFloatRound(float *aArr, int size, int digits);

inline bool IsSpace(char ch)
{
    return ( (ch == ' ') || (ch == '\t') );
}

void StripLine(string& line);
void KeyToName(std::string& line);
TCStringArray split(const string& str, const string& delimiters);

// Added by SD on 22/12/2001
// Change string on templates
void
ChangeforTempl( string& data_str,  const string& from_templ1,
                const string& to_templ1, uint len_ );

// Returns string representation of current date in dd/mm/yyyy format
string curDate();

// Returns string representation of current date in dd/mm/yy format
std::string curDateSmol(char ch = '/');

// Returns string representation of current time in HH:MM  format
string curTime();

// Returns string representation of current date and time
inline
string curDateTime()
{
    return curDate() + curTime();
}

// reads line to string class from istream with a delimiter
istream& u_getline(istream& instream, string& dst_string, char delimit = '\n');


#ifdef __FreeBSD
// replacement for missing function in FreeBSD
inline char* gcvt(double num, int digit, char* buf)
{
    sprintf(buf, "%*g", digit, num);
    return buf;
}

#endif  // __FreeBSD




#endif // _v_user_h_

