//-------------------------------------------------------------------
// $Id: v_object.h 1170 2008-12-21 19:41:47Z sveta $
//
// Definition of TObject and TObjList classes
//
// Copyright (C) 1996-2001 S.Dmytriyeva, A.Rysin
// Uses  gstring class (C) A.Rysin 1999
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

#ifndef _v_object_h_
#define _v_object_h_

#include <fstream>
#include "gstring.h"
#include "v_user.h"
#include "v_vals.h"

class GemDataStream;


const char TOKENOLABEL = '^';
const char TOKENOBJBEGIN = '~';

//const MINOBJTYPES = -13;
enum eObjType
{ S_ = 0, I_ = -1, U_ = -2, L_ = -3, X_ = -4, F_ = -5, D_ = -6,
  C_ = -7, N_ = -8, A_ = -9, B_ = -10, H_ = -11, N_TYPE_ = -12
};

typedef signed char ObjType;


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
    gstring Descr;
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

    int GetCellSize() const;
    int GetSize() const;

    //*  void Fill(double val, int n1, int m1, int n2, int m2);
    //*  void Fill(double val);
    //*  void PutAny( uint n=0, uint m=0 );
    //*  void PutEmpty( uint n=0, uint m=0 );

public:
    TObject (const char* name, ObjType type, int n, int m,
             bool dyn, char indexCode, const gstring descr);
    TObject (istream & f);
    ~TObject ();

    int ndx(int n, int m) const
    {
        return n * M + m;
    }

    void ToCFG(ostream & f);

    //--- Selectors
    const char *GetKeywd() const
    {
        return Keywd;
    }
    gstring GetFullName(int aN, int aM);
    gstring GetHelpLink(int aN, int aM);

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
      if(Type == S_ )	
    	  return 1; 
        return M;
    }

    void *GetPtr() const
    {
        return ((!pV) ? nullptr : pV->GetPtr());
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
    const gstring GetDescription(int Ni, int Mi);

    //--- Object parameters manipulation
    void SetN(int newN)
    {
        N = newN;
    }
    void SetM(int newM)
    {
        M = newM;
    }
    void SetDim(int newN, int newM)
    {
        N = newN; M = newM;
    }
    void SetPtr(void *newPtr);

    //--- Value manipulation
    double Get(int n = 0, int m = 0);
    double GetEmpty(int n = 0, int m = 0);
    void Put(double value, int n = 0, int m = 0);
    // Put cell of object to gstring. Return the lengs of gstring.
    // need inline - time critical function
    gstring GetString(int aN = 0, int aM = 0) const
    {
        check_dim(aN, aM);
        return ((GetPtr())? pV->GetString(ndx(aN, aM)) : gstring (S_EMPTY));
    }
    gstring GetStringEmpty(int aN = 0, int aM = 0) const
    {
        return ((GetPtr())? pV->GetString(ndx(aN, aM)) : gstring (S_EMPTY));
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

    int lenDB() const;
    int toDB(GemDataStream& f);
    int ofDB(GemDataStream& f);
    void toTXT(fstream& f);
    void ofTXT(fstream& f);

    bool operator == (const TObject& o) const
    {
        return pV == o.pV;
    }
};


// Array of objects in the system

class TObjList : public TIArrayF<TObject>
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
        return elem(i);
    }
};

extern TObjList aObj;

#endif // v_object.h
