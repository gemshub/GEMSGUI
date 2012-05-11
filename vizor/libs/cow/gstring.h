#ifndef _gstring_h_
#define _gstring_h_

/***************************************************************************
	gstring class 
	Version 1.02
	23/07/2000

	represents some (vital) part 
	of stl::string functionality
	and uses copy-on-write technology 
	(there's a lot of improvements could be done here :-)

	No licence now sorry, probably later will use GPL
	Copyright (c) 2000
	Author: Andy Rysin
	E-mail: arysin@yahoo.com
****************************************************************************/

#include <ctype.h>
#include <string.h>


class gstring
{
/*
	inside class for keeping string values
	several strings can point to the same value
*/
 class str_val
 {
  friend class gstring;
 
    char* p;
    size_t size;
    size_t gran;
    int ref_cnt;
 
  public:
    str_val(size_t sz=30, size_t gr=30):
	size( (sz<gr) ? gr : sz ),
	gran(gr),
	ref_cnt(1)
	{ if( !size ) size = gran;
	    p = new char[size]; }
    ~str_val() { delete[] p; }
    
// one more string points to us
	str_val* grab() { ref_cnt++; return this; }

// one less string points to us
	void release() { if( !--ref_cnt ) delete this; }

// 'p' must be accesed through data() - to remove 'friend class' from this later
	char* data() const { return p; }

// self-explanatory :-)
	char& elem(size_t ndx) { return p[ndx]; }

// if 'ndx' is out of val's size resize storage
	void resize_if_need(size_t ndx);
 };


    str_val* ps;
    size_t len;

    void clone_if_need(size_t add_size=0);
    
 public:
    static const size_t npos = static_cast<size_t>(-1);
    
// various constructors
    gstring(): len(0) { ps = new str_val(); }
    gstring(const gstring& s): len(s.length()) { ps = s.ps->grab(); }
    gstring(const gstring& s, size_t pos, size_t len=npos);
    gstring(const char* s, size_t pos=0, size_t len=npos);
    gstring(int num, char ch);
    
    ~gstring() { ps->release(); }
    
    size_t length() const { return len; }
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
    void erase(size_t p1, size_t sz);
    
// various assigning
    const gstring& operator=(const gstring& s)
	{ ps->release(); ps = s.ps->grab(); len = s.length(); return *this; }
    const gstring& operator+=(const gstring& s);

// comparison
    bool operator==(const gstring& s) const
	{ return ( strncmp(ps->data(), s.ps->data(), (len<s.len)?len:s.len) == 0 ); }
    bool operator!=(const gstring& s) const
	{ return !(*this == s); }

// variuos assigning
    bool equals(const char* s) const;
    const gstring& operator=(const char* s);
    const gstring& operator+=(const char* s);
    const gstring& operator+=(const char ch)
    { clone_if_need(1); ps->resize_if_need(len); ps->data()[len++] = ch; return *this; }

// self-explanatory
    char& operator[](size_t ii) { clone_if_need(); return ps->elem(ii); }
    const char& operator[](size_t ii) const { return ps->elem(ii); }
    
// clones and returns c-style string pointer
// WARNING: this could cause a lot of warnigns form compilers
//		about calling non-const method of const object
//		if you try it with const gstring class
    const char* c_str();

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

#endif //_gstring_h_
