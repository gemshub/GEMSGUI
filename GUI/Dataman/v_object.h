//-------------------------------------------------------------------
// $Id: v_object.h 1170 2008-12-21 19:41:47Z sveta $
//
// Definition of TObject and TObjList classes
//
// Copyright (C) 1996-2001 S.Dmytriyeva, A.Rysin
// Uses  string class (C) A.Rysin 1999
//
// This file is part of the GEM-Selektor GUI library which uses the
// Qt v.4 cross-platform App & UI framework (http://qt.nokia.com)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the terms of GEMS4 Development
// Quality Assurance Licence (GEMS4.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#ifndef _v_object_h_
#define _v_object_h_

#include <fstream>
#include <vector>
#include <stdarg.h>
//#include <QJsonObject>

#include "v_user.h"
#include "v_vals.h"
#include "bson.h"

class GemDataStream;


const char TOKENOLABEL = '^';
const char TOKENOBJBEGIN = '~';

//const MINOBJTYPES = -13;
enum eObjType
{ S_ = 0, I_ = -1, U_ = -2, L_ = -3, X_ = -4, F_ = -5, D_ = -6,
  C_ = -7, N_ = -8, A_ = -9, B_ = -10, H_ = -11, J_ = -12, N_TYPE_ = -13
};

typedef signed char ObjType;


struct TValBson:
            TValBase
{
    bson bcobj;

    TValBson(): TValBase(false)
    { ptr = &bcobj;
      bcobj.data = 0;
      bcobj.errstr = 0;
    }

    ~TValBson()
    {
       bson_destroy(&bcobj);
    }

    void SetPtr(void* p)
    {
        Error( "Bson object", "Invalid operation");
        ptr = p;
    }

    size_t cSize() const
    {
      Error( "Bson object", "Invalid object type for old DataBase");
      return bson_size(&bcobj);
    }

    // see Alloc() description for TVal<T>
    void* Alloc(size_t sz)
    {
        bson_destroy(&bcobj);
        if( sz )
         bcobj.data = new char[ sz ];
        return ptr;
    }

    double Get(size_t /*ndx*/) const
    {
        return 0.;
    }
    void Put(double /*val*/, size_t /*ndx*/)
    {}

    bool IsAny(size_t /*ndx*/) const
    {
        return false;
    }

    bool IsEmpty(size_t /*ndx*/) const
    {
        return (bcobj.data==0);
    }

    string GetString(size_t /*ndx*/) const;

    //  bool VerifyString(const char* s);
    bool SetString(const char* s, size_t ndx);

    void write(GemDataStream& s, size_t size);

    void read(GemDataStream& s, size_t size );

};


//const int MAXKEYWD = 6+1;

/*
    TObject provides GUI and DBMS access to application data
    it can be any type, and in general is N*M matrix
*/

class TObject
{
    bool Dynamic;
    int N;
    int M;
    string Descr;
    TValBase* pV;		//  void* ptr;
    ObjType Type;
    char IndexationCode;
    char Keywd[MAXKEYWD];

    //void* GetCellPtr(int N = 0, int M = 0);
    TValBase* BuildVal(ObjType t, int M);

    TObject (const TObject& );	// forbidden
    const TObject& operator = (const TObject& );	// ----""---
    //  TObject();  // for TArray

protected:

    void check() const
    {
        ErrorIf(IsNull(), GetKeywd(), "Access to null object");
    }

    void check_dim(int n, int m) const
    {
        check();
        ErrorIf(n >= N
                || m >= M, GetKeywd(), "Cell index beyond object dimension");
    }

    void check_type(ObjType typ) const
    {
        ErrorIf(typ > 126 || typ < N_TYPE_, GetKeywd(), "Invalid object type");
    }

    void write(ostream & os);
    void read(istream & os);

    size_t GetCellSize() const;
    size_t GetSize() const;

    //*  void Fill(double val, int n1, int m1, int n2, int m2);
    //*  void Fill(double val);
    //*  void PutAny( uint n=0, uint m=0 );
    //*  void PutEmpty( uint n=0, uint m=0 );

public:

    TObject (const char* name, ObjType type, int n, int m,
             bool dyn, char indexCode, const string descr);
    TObject (istream & f);
    ~TObject ();

    size_t ndx(int n, int m) const
    {
        return n * M + m;
    }

    void ToCFG(ostream & f);

    //--- Selectors
    const char *GetKeywd() const
    {
        return Keywd;
    }

    string GetFullName(int aN, int aM);
    string GetHelpLink(int aN, int aM);

    ObjType GetType() const
    {
        return Type;
    }

    char GetIndexationCode() const
    {
        return IndexationCode;
    }

    bool IsDynamic() const
    {
        return Dynamic;
    }

    void setDynamic( bool dat )
    {
        Dynamic = dat; // Sveta inernal not using this function
    }

    int GetN() const
    {
        return N;
    }

    int GetM() const
    {
        return M;
    }

    int GetNS() const
    {
      if(IsNull())
    	  return 0;

    	return N;
    }

    int GetMS() const
    {
      if(IsNull())
      	  return 0;
      if(Type == S_ || Type == J_)
    	  return 1; 
        return M;
    }

    void *GetPtr() const
    {
        return ((!pV) ? 0 : pV->GetPtr());
    }

    bool IsNull() const
    {
        return !GetPtr();
    }

    bool IsAny(int N, int M) const
    {
      return pV->IsAny( ndx(N, M) );
    }

    bool IsEmpty(int N, int M) const
    {
      return pV->IsEmpty( ndx(N, M) );
    }

    const string GetDescription(int Ni, int Mi);

    //--- Object parameters manipulation
    void SetN(unsigned newN)
    {
        N = newN;
    }

    void SetM(unsigned newM)
    {
        M = newM;
    }

    void SetDim(unsigned newN, unsigned newM)
    {
        N = newN, M = newM;
    }

    void SetPtr(void *newPtr);

    //--- Value manipulation
    double Get(int n = 0, int m = 0);
    double GetEmpty(int n = 0, int m = 0);
    void Put(double value, int n = 0, int m = 0);
    // Put cell of object to string. Return the lengs of string.
    // need inline - time critical function
    string GetString(int aN = 0, int aM = 0) const
    {
        check_dim(aN, aM);
        return ((GetPtr())? pV->GetString(ndx(aN, aM)) : string (S_EMPTY));
    }

    string GetStringEmpty(int aN = 0, int aM = 0) const
    {
        return ((GetPtr())? pV->GetString(ndx(aN, aM)) : string (S_EMPTY));
    }

    bool SetString(const char* vbuf, int aN = 0, int aM = 0);

    //--- Object manipulation
    // Useful for the 1st call
    void* Alloc();
    // To change dimensions
    void* Alloc(int newN, int newM)
    {
        return Alloc(newN, newM, Type);
    }
    // full version
    void* Alloc(int newN, int newM, ObjType newType);
    void* Free();

    size_t lenDB() const;
    size_t toDB(GemDataStream& f);
    size_t ofDB(GemDataStream& f);

//    void toTXT(fstream& f);
//    void ofTXT(fstream& f);
//    QJsonObject toJsonObject();
//    void fromJsonObject( QJsonObject obj );

    void checkUtf8(); // for old records
    void toBsonObject( bson *obj );
    void fromBsonObject( const char *obj );

    bool operator == (const TObject& o) const
    {
        return pV == o.pV;
    }
};


// Array of objects in the system

class TObjList : public vector<TObject*>
{
public:
    TObjList();
    TObjList(istream& f);

    void fromDAT(istream& f);
    void toDAT(ostream& f);
    void load(const char* f_obj, int N = -1);	// from '.ini'

    //--- Selectors
    int Find(const char* keywd);
    TObject& operator[] (int i)
    {
        return *at(i);
    }
};

extern TObjList aObj;

#endif // v_object.h
