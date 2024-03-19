//-------------------------------------------------------------------
// $Id: v_object.h 1170 2008-12-21 19:41:47Z sveta $
//
// Definition of TObject and TObjList classes
//
// Copyright (C) 1996-2001 S.Dmytriyeva, A.Rysin
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

#ifndef _v_object_h_
#define _v_object_h_

#include <fstream>
#include <memory>
#include "v_user.h"
#include "v_vals.h"

class GemDataStream;
class QJsonObject;
class QJsonValue;

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
    string Descr = "";
    TValBase* pV;		//  void* ptr;
    ObjType Type;
    char IndexationCode;
    char Keywd[MAXKEYWD];

    std::string string_empty_value = S_EMPTY;

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
        if( n >= N || m >= M )
            Error(GetKeywd(), "Cell index beyond object dimension");
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

    void set_empty_string( const string& object_empty )
    {
        string_empty_value = object_empty;
    }

    TObject (const char* name, ObjType type, int n, int m,
             bool dyn, char indexCode, const string descr);
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
    bool IsAny(int aN, int aM) const
    {
    return pV->IsAny( ndx(aN, aM) );
    }
    bool IsEmpty(int aN, int aM) const
    {
    return pV->IsEmpty( ndx(aN, aM) );
    }
    const string GetDescription(int Ni, int Mi);

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
    double Get(int n = 0, int m = 0) const;
    double GetEmpty(int n = 0, int m = 0);
    void Put(double value, int n = 0, int m = 0);
    // Put cell of object to string. Return the lengs of string.
    // need inline - time critical function
    string GetString(int aN = 0, int aM = 0) const
    {
        check_dim(aN, aM);
        return ((GetPtr())? pV->GetString(ndx(aN, aM)) : string(S_EMPTY));
    }
    string GetStringEmpty(int aN = 0, int aM = 0) const
    {
        auto str_value = ((GetPtr())? pV->GetString(ndx(aN, aM)) : string(S_EMPTY) );
        if(  Type >= 0 && str_value == string(S_EMPTY))
            str_value = string_empty_value;
        return str_value;
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

    void toJsonObject( QJsonObject& obj ) const;
    void fromJsonObject( const QJsonObject& obj );

    bool operator == (const TObject& o) const
    {
        return pV == o.pV;
    }
    QJsonValue toJsonValue() const;
    void fromJsonValue(const QJsonValue &obj);
};


// Array of objects in the system

class TObjList : public std::vector<std::shared_ptr<TObject>>
{
public:

    TObjList();
    TObjList(istream& f);

    void fromDAT(istream& f);
    void toDAT(ostream& f);
    void load(const char* f_obj, int N = -1);	// from '.ini'

    //--- Selectors
    int Find(const char* keywd);

};

extern TObjList aObj;

#endif // v_object.h
