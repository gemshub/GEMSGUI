#ifndef _gstring_h_
#define _gstring_h_

/***************************************************************************
	gstring class
	Version 1.02
        23/07/2000

	represents some (vital) part
	of stl::string functionality
	without copy-on-write technology
	(there's a lot of improvements could be done here :-)

	Copyright (c) 2000
	Authors: Andy Rysin, Dima Kulik
	E-mail: arysin@yahoo.com
****************************************************************************/

#include <ctype.h>
#include <string.h>
#include "array.h"


class gstring
{
/*
	inside class for keeping string values
*/
  struct str_val:
    public TOArray<char>
 {
  friend class gstring;
    str_val(size_t sz=30, size_t gran=30)
	:TOArray<char>(sz, gran)
	{ }
    ~str_val() {}
    
    bool equals(const str_val& s);
    str_val* clone(size_t add_size=0);
    const char* c_str() { if( elem(GetCount()) ) { Add('\0'); count--; } return p; }
 };

    str_val* ps;

 public:
    static const size_t npos = static_cast<size_t>(-1);
    
// various constructors
    gstring() { ps = new str_val(); }
    gstring(const gstring& s) { ps = s.ps->clone(); }
    gstring(const gstring& s, size_t pos, size_t len=npos);
    gstring(const char* s, size_t pos=0, size_t len=npos);
    gstring(int num, char ch);
    
    ~gstring() { delete ps; }
    
    size_t length() const { return ps->GetCount(); }
    size_t empty() const { return length() == 0; }

// find something from position 'pos' or beginning if omited
    size_t find(const char* s, size_t pos = 0) const;
    size_t find(const gstring& s, size_t pos = 0) const;
    size_t find(const char ch, size_t pos = 0) const;
    size_t find_first_of(const char* s) const;

// reverse find
    size_t rfind(const char* s) const;

// returns substring
    gstring substr(size_t pos, size_t len = npos) const;
// erases from position 'p1' and size 'sz'
            void erase(size_t p1, size_t p2);

// various assigning
    const gstring& operator=(const gstring& s)
	{ delete ps; ps = s.ps->clone(); return *this; }
    const gstring& operator+=(const gstring& s);

// comparison
    bool operator==(const gstring& s) const
	{ return ps->equals(*s.ps); }
    bool operator!=(const gstring& s) const
	{ return !(*this == s); }
    bool equals(const char* s) const;

// variuos assigning
    const gstring& operator=(const char* s);
    const gstring& operator+=(const char* s);
    const gstring& operator+=(const char ch)
    { ps->Add(ch); return *this; }

// self-explanatory
    char& operator[](size_t ii) { return ps->elem(ii); }

// returns c-style string pointer
    const char* c_str() const { return ps->c_str(); }

// strip spaces from both ends
    void strip();
};

inline
bool operator==(const gstring& str, const char* sp)
{
    return str.equals(sp);
}

inline
bool operator!=(const gstring& str, const char* sp)
{
    return !(str == sp);
}

inline
bool operator==(const char* sp, const gstring& str)
{
    return (str == sp);
}

inline
bool operator!=(const char* sp, const gstring& str)
{
    return !(str == sp);
}

inline
const gstring operator+(const gstring& str1, const gstring& str2)
{
    gstring res(str1);
    return res += str2;
}

typedef TArrayF<gstring> TCStringArray;

#endif //_gstring_h_
