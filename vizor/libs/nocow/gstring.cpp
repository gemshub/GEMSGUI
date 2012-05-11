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

#include "gstring.h"

//#define CHECK_GSTR

#ifdef CHECK_GSTR

/*	if CHECK_GSTR is defined we'll have StringLenError() exception
	on operations with length > 1M
	this is usually the case when unsigned value parameter
	given negative value (like (size_t)-1 == 2^32-2)
	this is usually error somewhere in code

	this is for internal debugging reasons and
	if you want to use it you have to move
	StringLenError calss in gstring.h to access it in your app
*/

struct StringLenError {};

inline 
void CHECK_LEN(size_t ln) throw (StringLenError)
{
 if( ln >= 1000000 )
    throw  StringLenError();
}

#else

#define CHECK_LEN(ln) ;

#endif

const size_t OVERSIZE = 1;

gstring::str_val*
gstring::str_val::clone(size_t add_size)
{
CHECK_LEN(GetCount()+add_size);
    str_val* pv = new str_val( GetCount() + add_size + OVERSIZE);
    strncpy(pv->p, p, GetCount());
    pv->count = GetCount();
    return pv;
}

bool
gstring::str_val::equals(const str_val& s)
{
    if( &s == this )
	return true;

    size_t ln1 = GetCount();

    if( ln1 != s.GetCount() )
	return false;

    if( ln1 == 0 )
	return true;    // both are empty

    return ( strncmp(p, s.p, ln1) == 0 );
}

gstring::gstring(const gstring& s, size_t pos, size_t len)
{
    if( pos == 0 && len == npos )
    {
	ps = s.ps->clone();
	return;
    }
    size_t s_len = s.length() - pos;
    size_t length;
    if( len == npos )
	length = s_len;
    else
	length = (len<s_len) ? len : s_len;
CHECK_LEN(length);
    ps = new str_val( length + OVERSIZE );
    ps->count = length;
    strncpy(ps->p, s.ps->p + pos, length);
}

gstring::gstring(const char* s, size_t pos, size_t len)
{
    size_t s_len;
    if( len == npos )
	s_len = strlen(s);
    else
	for(s_len = 0; s_len < len && s[s_len+pos]; s_len++ );

CHECK_LEN(s_len);
    ps = new str_val( s_len + OVERSIZE );
    ps->count = s_len;
    strncpy(ps->p, s + pos, s_len);
}

gstring::gstring(int num, char ch)
{
CHECK_LEN(num);
    ps = new str_val(num + OVERSIZE);
    memset(ps->p, ch, num);
    ps->count = num;
}
    
const gstring& 
gstring::operator=(const char* s)
{
    size_t length = strlen(s);
CHECK_LEN(length);
    delete ps;
    ps = new str_val( length + OVERSIZE );
    ps->count = length;
    strncpy(ps->p, s, length);
    
    return *this;
}

const gstring&
gstring::operator+=(const char* s)
{
    size_t length1 = strlen(s);
    size_t cnt = ps->count;
CHECK_LEN(length()+length1);
    ps->resize(length() + length1 + OVERSIZE);
    strncpy(ps->p + cnt, s, length1);
    ps->count = cnt + length1;

    return *this;
}

const gstring&
gstring::operator+=(const gstring& s)
{
    size_t length1 = s.length();
    size_t cnt = ps->count;
CHECK_LEN(length()+length1);
    ps->resize(length() + length1 + OVERSIZE);
    strncpy(ps->p + cnt, s.ps->p, length1);
    ps->count = cnt + length1;
    
    return *this;
}

bool
gstring::equals(const char* sp) const
{
    size_t ln1 = length();

    if( ln1 != strlen(sp) )
	return false;

    if( ln1 == 0 )
	return true;    // both are empty

    return ( strncmp(sp, ps->p, ln1) == 0 );
}

size_t 
gstring::find(const char* s, size_t pos) const
{
    return find(gstring(s), pos);
}

size_t 
gstring::find(const gstring& s, size_t pos) const
{
    if( pos + s.length() > length() )
	return npos;
	
    for( ;pos<length(); pos++)
	    if( substr(pos, s.length()) == s )
		return pos;
    return npos; 
}

size_t
gstring::find(const char ch, size_t pos) const
{
    for( ;pos<length(); pos++)
	if( ps->elem(pos) == ch )
	    return pos;
    return npos;
}

size_t 
gstring::find_first_of(const char* s) const
{
    int s_ln = strlen(s);
    for(size_t kk=0; kk<length(); kk++)
	for(int ii=0; ii<s_ln; ii++)
	    if( ps->elem(kk) == s[ii] )
		return kk;
    return npos;
}

size_t 
gstring::rfind(const char* s) const
{
    size_t s_ln = strlen(s);
    if( s_ln > length() || !s_ln )
	return npos;
	
    for(int ll=length()-s_ln ;ll>=0; ll--)
	    if( strncmp( ps->p + ll, s, s_ln ) == 0 )
		return ll;
    return npos;
}

void 
gstring::erase(size_t p1, size_t npos)
{
CHECK_LEN(npos);
    memmove(ps->p + p1, ps->p + p1 + npos, length()-p1-npos);
    ps->count -= npos;
}

gstring 
gstring::substr(size_t pos, size_t len) const
{
    return gstring(*this, pos, len);
}

inline
bool isSpace(char ch) { return (ch==' '||ch=='\t'); }

void
gstring::strip()
{
    if( empty() )
	return;
    size_t endp = length();  
    for( ; endp>0 && isSpace( ps->elem(endp-1) ); endp--);
    
    ps->count = endp;
    
    size_t ii = 0;
    for( ; ii<length() && isSpace( ps->elem(ii) ); ii++ );
    
    if( ii == 0 || ii >= length() )
      return;
    erase(0, ii);
}
