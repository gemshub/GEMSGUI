//-------------------------------------------------------------------
// $Id: v_ipnc.cpp 1222 2009-02-10 14:27:00Z gems $
//
// Implementation of IPNCalc
//
// Copyright (C) 1996-2001  S.Dmytriyeva
// Uses  gstring class (C) A.Rysin 1999
//
// This file is part of a GEM-Selektor library for thermodynamic
// modelling by Gibbs energy minimization
// Uses: GEM-Vizor GUI DBMS library, gems/lib/gemvizor.lib
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail: gems2.support@psi.ch
//-------------------------------------------------------------------

#include <math.h>
#include <stdio.h>
#include "gstring.h"
#include "v_ipnc.h"
#include "v_object.h"

const char* DIGIT="0123456789.";
const char* OPER="!^*/+-<a>b=c&|()";
const char* RAZD=" +-*/^:[]();=$&|!<>?#,";
const int FuncNumber=29; // number of functions

double derf(double x);
double derfc(double x);


typedef struct
{
    char func [ MAXKEYWD ];
    char nch;
    signed char type;
    int arn;
    int par;
}
FUNCTION;

static FUNCTION fun[] = {
    { "abs", '\0', D_, 1, 1 },
    { "sign", '\0', D_, 1, 1 },
    { "exp", '\0', D_, 1, 1 },
    { "sqrt", '\0', D_, 1, 1 },
    { "ln", '\0', D_, 1, 1 },
    { "lg", '\0', D_, 1, 1 },
    { "sin", '\0', D_, 1, 1 },
    { "sinh", '\0', D_, 1, 1 },
    { "cos", '\0', D_, 1, 1 },
    { "cosh", '\0', D_, 1, 1 },
    { "tan", '\0', D_, 1, 1 },
    { "tanh", '\0', D_, 1, 1 },
    { "ceil", '\0', D_, 1, 1 },
    { "floor", '\0', D_, 1, 1 },
    { "round", '\0', D_, 1, 1 },
    { "asin", '\0', D_, 1, 1 },
    { "acos", '\0', D_, 1, 1 },
    { "atan", '\0', D_, 1, 1 },
    { "erf", '\0', D_, 1, 1 },
    { "erfc", '\0', D_, 1, 1 },
    { "empty", '\0', D_, 0, 1 },
    { "mod", '\0', I_, 2, 1 },
    { "pow", '\0', D_, 2, 1 },
    { "SUM", '\0', D_, 1, 0 },
    { "PROD", '\0', D_, 1, 0 },
    { "MAX", '\0', D_, 1, 0 },
    { "MIN", '\0', D_, 1, 0 },
    { "NULL", '\0', D_, 1, 0 },
    { "ONE", '\0', D_, 1, 0 },
};


IPNCalc::IPNCalc():
        aItm(RPN_BLOCK, 100),  aEq(EQUATION_BLOCK, 5),  aCon(CONST_BLOCK, 5)
{
    input = 0;
    con_add( 0.0 );
    con_add( 1.0 );
}

IPNCalc::~IPNCalc()
{
    aItm.Clear();
    aEq.Clear();
    aCon.Clear();
}

// push element to stack
void IPNCalc::Push( char ch, unsigned n )
{
    aItm.Add( new RpnItem(ch, n) );
}


// add value to array of equations
void IPNCalc::eq_add( int Item, int First )
{
    if( First != -1 )
        aEq.Add( new Equation( Item, First) );
    else
    {
        int Indx = aEq.GetCount()-1;
        aEq[aEq.GetCount()-1].nItems = Item - aEq[Indx].first;
        //aItm.GetCount() - aEq[Indx].first;
    }

}

// add value to array of constant
int IPNCalc::con_add( double val )
{
    for( uint i=0; i<aCon.GetCount(); i++)
    {
        if( aCon[i] == val )
            return i;
    }
    aCon.Add(val);
    return aCon.GetCount()-1;
}

// skip  trailing blanks and commemt statements
char *IPNCalc::xblanc( char *cur )
{
    char *ncur;

    if( !cur || !*cur ) return 0;
    ncur = cur + strspn( cur, " \r\n\t" );
    while( *ncur == '$' )
    {
        ncur += strcspn( ncur, "\r\n\0" );
        if( *ncur )
            ncur += strspn( ncur, " \r\n\t" );
    }
    if( *ncur ) return ncur;
    else return 0;
}

int IPNCalc::INDEX( const char *s, char ch )
{
    if( s == 0 )
        return -1;
    char *f = strchr(s, ch);
    if( f==0 )
        return -1;
    return (int)(f-s);
}

//Get operation priority
int prty[] = { 0,1,2,2,3,3,4,4,4,4,5,5,6,7,8,8 };
int IPNCalc::SPRTY( char c )
{
    int i=INDEX( OPER, c);
    return prty[i];
}

// Get constant and put constant to con
void IPNCalc::InDigit()
{
    int n;
    double d;

    n = strspn( input, DIGIT );
    if( input[n] == 'E' || input[n] == 'e' )
    {
        n++;
        if( input[n] == '-' || input[n] == '+' )
            n++;
        n += strspn( input+n, DIGIT );
    }
    //ErrorIf( n>24 ,"IPNE01", "Too many digits in a numerical constant.");
    gstring str=gstring(input, 0, n);
    //memset( str, 0, 25 );
    //strncpy( str, input, n );
    sscanf( str.c_str(), "%lg", &d );
    input+=n;
    Push( IT_C, con_add(d) );
}

// skip gstring const "<ᨬ����>", in struct put
// ascii code of the first character in gstring
void IPNCalc::IsAscii()
{
    double d;

    input++;
    d = (double)(*input);
    Push( IT_C, con_add(d) );
    input = strchr( input, '"' );
    ErrorIf( !input ,"E01MSTran: ",
             "Missing \" in string constant.");
    input++;
}

//get identifier
void IPNCalc::Ident( char *s )
{
    int n,m;
    n=strcspn( input, RAZD );
    m = min( MAXKEYWD, n);
    strncpy( s, input, m);
    input+=n;
}

//analyse IPN function
void IPNCalc::Ffun( char *str)
{
    int i,j;
    vstr st(MAXKEYWD);
    gstring err;

    input++;
    if( (input=xblanc( input ))==0 ) goto OSH;
    for( i=0; i<FuncNumber; i++)
        if( !( strncmp( str, fun[i].func, MAXKEYWD) ) )
            break;
    if( i==FuncNumber )
    {
        err = "Function ";
        err += str;
        err+= " is not defined.";
        Error( "E02MSTran: ", err.c_str() );
    }
    if( fun[i].par ) // argument is expression
    {
       if( fun[i].arn == 0 )
       {
         if( *input != ')' )
         {
           err = "Function ";
           err += str;
           err+= " has not parameters.";
           Error( "E26MSTran: ", err.c_str() );
         }
         input++;
       }
       else
       {  for( j=0; j<( fun[i].arn )-1; j++ )
            RPN_expr( ',');
          RPN_expr( ')');
       }
       Push( IT_F, i);
    }
    else
    { // argument is interval
        if( !IsLetter( *input ) )
        {
            err = "Missing identifier of interval:\n";
            err += input;
            Error( "E03MSTran: ", err.c_str() );
        }
        memset( st, '\0', MAXKEYWD );
        //st[MAXKEYWD-1] = '\0';
        Ident( st );
        I_Variab( st );
        if( (input=xblanc( input ))==0 ) goto OSH;
        if( *input!=')' )
        {
            err = "Missing bracket ):\n";
            err += input;
            Error( "E04MSTran: ", err.c_str() );
        }
        input++;
        Push( IT_F, i);
    }
    return;
OSH:
    Error( "E05MSTran: ", "Unexpected end of Math Script text");
}

//analyse IPN variable
void IPNCalc::Variab( const char *str)
{
    int j = aObj.Find(str);

    if( j<0 )
    {
        gstring err = "Variable ";
        err += str;
        err+= " is not known.";
        Error( "E06MSTran: ", err.c_str() );
    }
    if( aObj[j].GetN() > 1 )
    {
        if( *input != '[' )
            Push( IT_C, 0);
        else
        {
            input++;
            RPN_expr( ']');
        }
    }
    if( ( input=xblanc( input ) )==0 ) goto OSH;
    if( aObj[j].GetM() > 1 )
    {
        if( *input != '[' )
            Push( IT_C, 0);
        else
        {
            input++;
            RPN_expr( ']');
        }
    }
    Push( IT_V, j);
    return;
OSH:
    Error( "E15MSTran: ", "Unexpected end of Math Script text");
}

//analyse IPN interval variable
void IPNCalc::I_Variab( char * str)
{
    int j = aObj.Find(str);

    if( j<0 )
    {
        gstring err = "Variable ";
        err += str;
        err+= " is not declared.";
        Error( "E16MSTran: ", err.c_str() );
    }
    if( aObj[j].GetN() > 1 )
    {
        if( (input=xblanc( input ))==0 ) goto  OSH;
        if( *input=='[' )
        {
            input++;
            RPN_expr( '#');
            RPN_expr( ']');
        }
        else  // [0:dimN]
        {
            Push( IT_C, 0);
            Push( IT_C, con_add((double)aObj[j].GetN()) );
        }
    }
    if( aObj[j].GetM() > 1 )
    {
        if( (input=xblanc( input ))==0 ) goto  OSH;
        if( *input=='[' )
        {
            input++;
            RPN_expr( '#');
            RPN_expr( ']');
        }
        else  // [0:dimM]
        {
            Push( IT_C, 0);
            Push( IT_C, con_add((double)aObj[j].GetM()) );
        }
    }
    Push( IT_I, 0);
    Push( IT_V, j);
    return;
OSH:
    Error( "E25MSTran: ", "Unexpected end of Math Script text");
}

//get IPN code of one math script operator
void IPNCalc::bildEquat()
{
    vstr str(MAXKEYWD);
    char *s;
    gstring err;
    int i,j;

    input = s = xblanc( input );
    ErrorIf( s==0, "E07MSTran: ", "Math script contains no operators.");
    while( (i = INDEX( s, CK_ )) >= 0 )
    {
        if( (input=xblanc( s ))==0 ) goto OSH;
        if( *input == 'd' && *(input+1) == 'o' )
        {
            bildWhile();
            s=input;
            if( s )
            {
                if( (input=xblanc( s ))==0 ) goto OSH;
                if(  !strncmp(input, "end", 3)) //Sveta 15/09/99
                    return;
            }
            continue;
        }
        if( *input == 'i' && *(input+1) == 'f' )
        {
            bildIf();
            s = input;
            if( s )
            {
                if( (input=xblanc( s ))==0 ) goto OSH;
                if(  !strncmp(input, "end", 3)) //Sveta 15/09/99
                    return;
            }
            continue;
        }
        eq_add( 0, aItm.GetCount() );
        // eq[Neqs].first = Nitems;
        j =INDEX( s, '=' );
        if( j<0 || j>i || *(s+(j+1)) != ':' )
        {
            err = "Assignment needs =: \n";
            err += s;
            Error(  "E08MSTran: ", err.c_str() );
        }
        input = strchr( s, '=' ) + 2;
        if( ( input=xblanc( input )) ==0 ) goto OSH;
        if( *input==';' )
        {
            err = "Expression is needed here:\n";
            err += input;
            Error(  "E09MSTran: ", err.c_str() );
        }
        RPN_expr( CK_ );
        if( ( input=xblanc( s ))==0 ) goto OSH;
        if( !(IsLetter( *input )) )
        {
            err = "Identifier (variable) is needed here:\n";
            err += input;
            Error(  "E10MSTran: ", err.c_str() );
        }
        memset( str, '\0', MAXKEYWD );
        //str[MAXKEYWD-1] = '\0';
        Ident( str );
        Variab( str );
        Push( IT_A, 0 );
        eq_add( aItm.GetCount() );
        s += i + 1;
        s = xblanc( s );
        if( !s || !strncmp(s, "end", 3))
        {
            input = s;
            return;
        }
    }
    input = s;
    if( s )
        input = xblanc( s );
    if( input!=0 )
    {
        err = "Invalid characters at the end of expression:\n";
        err += input;
        Error(  "E11MSTran: ", err.c_str() );
    }
    return;
OSH :
    Error( "E35MSTran: ", "Unexpected end of Math Script text");
}

//get IPN cod operator while
// syntax:  do( <condition> ) begin <equations>  end
void IPNCalc::bildWhile()
{
    int neqbg, it;
    gstring err;

    input += 2; // skip do
    if( ( input=xblanc( input ))==0 ) goto OSH;
    if( *input!='(' )
    {
        err = "Missing (:\n";
        err += input;
        Error(  "E14MSTran: ", err.c_str() );
    }
    input++;
    neqbg = aEq.GetCount();
    eq_add( 0, aItm.GetCount() ); //  eq[Neqs].first = Nitems; // conditions
    if( *input==')')
    {
        err = "Expression is needed here:\n";
        err += input;
        Error(  "E19MSTran: ", err.c_str() );
    }
    RPN_expr( ')' );
    Variab( "k_");
    Push( IT_A, 0 );
    eq_add(  aItm.GetCount() );  // conditional jump to equation
    eq_add(  0, aItm.GetCount() );  // eq[Neqs].first = Nitems;
    it = aItm.GetCount();
    Push( IT_W, 0 );
    eq_add( aItm.GetCount());
    // analyse before  'end'
    if( ( input=xblanc( input ))==0 ) goto OSH;
    if( strncmp( input, "begin", 5 ))
    {
        err = "Missing keyword begin :\n";
        err += input;
        Error(  "E12MSTran: ", err.c_str() );
    }
    input+=5;
    bildEquat();
    if( strncmp( input, "end", 3 ))
    {
        err = "Missing keyword end :\n";
        err += input;
        Error(  "E13MSTran: ", err.c_str() );
    }
    input+=3;
    // unconditional jump to equation
    eq_add(  0, aItm.GetCount() );  // eq[Neqs].first = Nitems;
    Push( IT_B, neqbg );
    eq_add( aItm.GetCount() );
    aItm[it].num = aEq.GetCount();
    input=xblanc( input );
    return;
OSH :
    Error( "E35MSTran: ", "Unexpected end of Math Script text");
}

//get IPN cod operator if
// syntax:  if( <condition> ) begin <equations1>  end
//   else begin <equations2>  end
void IPNCalc::bildIf()
{
    int it2,it;
    gstring err;

    input += 2; // skip if
    if( ( input=xblanc( input ))==0 ) goto OSH;
    if( *input!='(' )
    {
        err = "Missing bracket ( :\n";
        err += input;
        Error(  "E24MSTran: ", err.c_str() );
    }
    input++;
    eq_add(  0, aItm.GetCount() );  // eq[Neqs].first = Nitems; // conditions
    if( *input==')' )
    {
        err = "Expression is needed here:\n";
        err += input;
        Error(  "E29MSTran: ", err.c_str() );
    }
    RPN_expr( ')' );
    Variab( "k_");
    Push( IT_A, 0 );
    eq_add( aItm.GetCount() );
    // conditional jump to equation
    eq_add(  0, aItm.GetCount() );  // 	eq[Neqs].first = Nitems;
    it = aItm.GetCount();
    Push( IT_W, 0 );
    eq_add( aItm.GetCount() );
    // analyse before  'end'
    if( ( input=xblanc( input ))==0 ) goto OSH;
    if( strncmp( input,"begin", 5 ))
    {
        err = "Missing keyword begin :\n";
        err += input;
        Error(  "E22MSTran: ", err.c_str() );
    }
    input+=5;
    bildEquat();
    if( strncmp( input,"end", 3 ))
    {
        err = "Missing keyword end :\n";
        err += input;
        Error(  "E23MSTran: ", err.c_str() );
    }
    input+=3;
    // unconditional jump to equation
    eq_add(  0, aItm.GetCount() );  // 	eq[Neqs].first = Nitems;
    it2 = aItm.GetCount();
    Push( IT_B, aEq.GetCount()/*+1*/ ); // 27/11/2006 SD
    eq_add( aItm.GetCount() );
    aItm[it].num = aEq.GetCount();
    if( ((input=xblanc( input ))!=0) && !strncmp( input,"else", 4 ))
    { // condition else
        input+=4;
/* Sveta 27/11/2006
// conditional jump to equation
        eq_add(  0, aItm.GetCount() );  // 	eq[Neqs].first = Nitems;
        Push( IT_W, aEq.GetCount()+1 );
        eq_add( aItm.GetCount());
*/
        // analyse before  'end'
        if( ( input=xblanc( input ))==0 ) goto OSH;
        if( strncmp( input,"begin", 5 ))
        {
            err = "Missing keyword begin :\n";
            err += input;
            Error(  "E32MSTran: ", err.c_str() );
        }
        input+=5;
        bildEquat();
        if( strncmp( input,"end", 3 ))
        {
            err = "Missing keyword end :\n";
            err += input;
            Error(  "E32MSTran: ", err.c_str() );
        }
        input+=3;
/*  Sveta 27/11/2006
  // unconditional jump to equation
        eq_add(  0, aItm.GetCount() );  // 	eq[Neqs].first = Nitems;
        Push( IT_W, aEq.GetCount()+1 ); // 27/11/2006 SD
        eq_add( aItm.GetCount());
*/
        /*eq[Neqs].first = Nitems;
          Push( IT_B, Neqs+1 );
          eq_add();*/
        aItm[it2].num = aEq.GetCount();
    }
    input=xblanc( input );
    return;
OSH :
    Error( "E45MSTran: ", "Unexpected end of Math Script text." );
}

//analyse IPN expression. ck -end of analyse text
#define SPUSH() (itec<0?'#':stack[itec--])
void IPNCalc::RPN_expr( char ck )
{
    int itec=-1;
    vstr stack(MAXSTACK);
    char op,op1;
    int l;
    vstr str(MAXKEYWD);
    gstring err;

    memset( stack, 0, MAXSTACK );
    if( (input=xblanc(input))==0 ) goto OSH;
    while( (*input!=ck) || (ck==')') )
    {
        if( IsDigit( *input ) ) //constant
        {
            InDigit();
            if( (input=xblanc(input))==0 ) goto OSH;
            continue;
        }
        if( *input == '"') // gstring const
        {
            IsAscii();
            if( (input=xblanc(input))==0 ) goto OSH;
            continue;
        }
        if( IsLetter( *input ) ) // variable or function
        {
            memset( str, '\0', MAXKEYWD );
            //str[MAXKEYWD-1] = '\0';
            Ident( str );
            if( (input=xblanc( input ))==0 ) goto OSH;
            if( *input=='(' )  // function
                Ffun( str );
            else  // variable
                Variab( str );
            if( (input=xblanc(input))==0 ) goto OSH;
            continue;
        }
        switch( *input ) // operation  +,-,*,/,^,(,)
        {
        case '(' :
            input++;
            if( (input=xblanc(input))==0 ) goto OSH;
            if( *input == '-' /*|| *input == '!'*/ )
            { // unary '-' /*or logical not*/
                input++;
                if( (input=xblanc(input))==0 ) goto OSH;
                if( *input == '(' ) // expression
                {
                    input++;
                    RPN_expr( ')');
                }
                else
                {
                    if( IsLetter( *input ) )
                    {
                        memset( str, '\0', MAXKEYWD );
                        //str[MAXKEYWD-1] = '\0';
                        Ident( str );
                        if( (input=xblanc( input ))==0 ) goto OSH;
                        if( *input=='(' ) //function
                            Ffun(str);
                        else // variable
                            Variab( str);
                    }
                    else
                        if( IsDigit( *input ) ) // constant
                            InDigit();
                        else goto OSH;
                }
                if( (input=xblanc( input ))==0 ) goto OSH;
                if( *input!=')')
                {
                    err = "Missing bracket ) :\n";
                    err += input;
                    Error(  "E34MSTran: ", err.c_str() );
                }
                input++;
                Push( IT_U, 0 );
            }
            else
            {
                ErrorIf( itec>=MAXSTACK-1,"E19MSTran: ",
                         "Operation stack overflow.");
                itec++;
                stack[ itec ]='(';
            }
            break;
        case ')' :
            while( ( op=SPUSH() ) != '#' )
            {
                if( op != '(' )
                    Push( IT_O, INDEX( OPER, op ));
                else break;
            }
            if( op == '#' )
            {
                if( ck == ')' )
                {
                    input++;
                    return;
                }
                else
                {
                    err = "No match of brackets ( and ) :\n";
                    err += input;
                    Error("E20MSTran: ", err.c_str() );
                }
            }
            input++;
            break;
        case '+':
        case '-':   // operation + * - / ^
        case '*':   // and logical operations
        case '/':
        case '^':
        case '!': //???????
        case '&':
        case '|':
        case '>':
        case '=':
        case '<':
            op1 = *input;
            if( op1 == '<' && *(input+1) =='>' )
            {
                input++;
                op1 = 'c';
            }
            if( op1 == '<' && *(input+1) =='=' )
            {
                input++;
                op1 = 'a';
            }
            if( op1 == '>' && *(input+1) =='=' )
            {
                input++;
                op1 = 'b';
            }
            while( itec >= 0 )
            {
                op = stack[ itec ];
                if( SPRTY( op ) <= SPRTY( op1 ) )
                {
                    Push( IT_O, INDEX( OPER, op ));
                    itec--;
                }
                else break;
            }
            ErrorIf( itec>=MAXSTACK-1, "E29MSTran: ",
                     "Operation stack overflow.");
            itec++;
            stack[itec]=op1;
            input++;
            break;
        case '?':
            while( (op = SPUSH()) != '(' )
                Push( IT_O, INDEX( OPER, op ));
            input++;
            l = aItm.GetCount();
            Push( IT_T, 0 );
            RPN_expr( ':');
            aItm[l].num = aItm.GetCount()-l;
            l = aItm.GetCount();
            Push( IT_G, 0);
            RPN_expr( ')');
            aItm[l].num = aItm.GetCount()-l;
            break;
        default :
            err = "Invalid symbol of operation:\n";
            err += input;
            Error(  "E21MSTran: ", err.c_str() );
        }
        if( (input=xblanc(input))==0 ) goto OSH;
    }
    while( itec>=0 )
    {
        op = stack[ itec-- ];
        if( op=='(' )
        {
            err = "No match of brackets ( and ):\n";
            err += input;
            Error(  "E30MSTran: ", err.c_str() );
        }
        Push( IT_O, INDEX( OPER, op ));
    }
    input++;
    return;
OSH:
    Error( "E45MSTran: ", "Unexpected end of Math Script text.");
}

//get IPN notation
void IPNCalc::GetEquat( char *txt )
{
    input = txt;
    if( !input || *input=='\0' || *input=='`' )
         return;
//    ErrorIf( !input || *input=='\0', "IPNTranslate",
//             "No Math Script text to analyse!");
    aItm.Clear( false );
    aEq.Clear( false );
    aCon.Clear( false );
    con_add( 0.0 );
    con_add( 1.0 );

    bildEquat();
    ErrorIf( input!=0, "E00MSTran: ", "Math Script operator syntax error");
}

#define StackEnd( i ) aStack[aStack.GetCount()-1+i]
#define StackDel() aStack.Remove(aStack.GetCount()-1)

// calc IPN notation
void IPNCalc::CalcEquat()
{
    int o_k_,i, k1,k2,k3,k4,j1,j2;
    double z;
    int ci;
    uint /*nstack,*/ ni, ieq;
    TOArray<double> aStack(MAXSTACK, 10);
    int k = 1;

    o_k_=aObj.Find("k_");
    aObj[o_k_].SetPtr(&k);
    ieq = 0;
    while( /*ieq >= 0 &&*/ ieq < aEq.GetCount() )
    {
        i = aEq[ieq].first;
        ci = aItm[i].code;
        ni = aItm[i].num;
        if( ci == IT_W )  // conditional jump to equation
        {
            ErrorIf( ni<ieq, "E01MSExec", "Illegal conditional goto command");
            if( fabs( aObj[o_k_].Get() ) < 1e-34 )
                ieq = ni;
            else ieq++;
            continue;
        }
        if( ci == IT_B ) // unconditional jump to equation
        {
            ErrorIf( ni > aEq.GetCount()+1, "E02MSExec",
                     "Illegal unconditional goto command");
            ieq =ni;
            continue;
        }
        ieq++;
        while( ci != IT_A )
        {
            switch( ci )
            {
            case IT_O :
                if( aStack.GetCount()<2)
                    Error( "E03MSExec", "No operands left in execution stack");
                switch( ni )
                {
                case 0 :
                    if( fabs(StackEnd(0)) >1e-34 )
                        StackEnd(0) = 0.;
                    else StackEnd(0) = 1.;
                    break;
                case 1 :
                    StackEnd(-1) =
                        pow (  StackEnd(-1),  StackEnd(0) );
                    StackDel();
                    break;
                case 2 :
                    StackEnd(-1) *= StackEnd(0);
                    StackDel();
                    break;
                case 3 :
                    ErrorIf( fabs(StackEnd(0)) < 1e-34,
                             "E04MSExec","Attempt of zerodivide!");
                    StackEnd(-1) /= StackEnd(0);
                    StackDel();
                    break;
                case 4 :
                    StackEnd(-1) += StackEnd(0);
                    StackDel();
                    break;
                case 5 :
                    StackEnd(-1) -= StackEnd(0);
                    StackDel();
                    break;
                case 6 :
                    if( StackEnd(-1) < StackEnd(0))
                        StackEnd(-1) = 1.;
                    else StackEnd(-1) = 0.;
                    StackDel();
                    break;
                case 7 :
                    if( StackEnd(-1) <= StackEnd(0))
                        StackEnd(-1) =1.;
                    else StackEnd(-1) = 0.;
                    StackDel();
                    break;
                case 8 :
                    if( StackEnd(-1) > StackEnd(0))
                        StackEnd(-1) = 1.;
                    else StackEnd(-1) =0.;
                    StackDel();
                    break;
                case 9 :
                    if( StackEnd(-1) >= StackEnd(0))
                        StackEnd(-1) = 1.;
                    else StackEnd(-1) = 0.;
                    StackDel();
                    break;
                case 10:
                    if( fabs(StackEnd(-1)-StackEnd(0))
                            < 1e-34) StackEnd(-1) = 1.;
                    else StackEnd(-1) = 0.;
                    StackDel();
                    break;
                case 11 :
                    if( fabs(StackEnd(-1)-StackEnd(0))
                            >= 1e-34 ) StackEnd(-1) = 1.;
                    else StackEnd(-1) = 0.;
                    StackDel();
                    break;
                case 12 :
                    if( fabs(StackEnd(0)) <1e-34 )
                        StackEnd(-1) = 0.;
                    StackDel();
                    break;
                case 13 :
                    if( fabs(StackEnd(0)) >= 1e-34 )
                        StackEnd(-1) = 1.;
                    StackDel();
                    break;
                default:
                    Error("E05MSExec","Illegal binary operator code.");
                }
                break;
            case IT_U :
                ErrorIf( aStack.GetCount()<1, "E13MSExec",
                         "No operands left in execution stack.");
                switch( ni )
                {
                case 0 :
                    StackEnd(0) = - StackEnd(0);
                    break;
                default:
                    Error("E06MSExec","Illegal unary operator code.");
                }
                break;
            case IT_V :   // variable
                if( aObj[ni].GetM()>1 )
                {
                    k2 = ROUND( StackEnd(0) );
                    StackDel();
                }
                else k2 = 0;
                if( aObj[ni].GetN()>1 )
                {
                    k1 = ROUND( StackEnd(0) );
                    StackDel();
                }
                else k1 = 0;
                if( aItm[i+1].code == IT_A ) //save result
                {
                    aObj[ni].Put( StackEnd(0), k1, k2 );
                    StackDel();
                }
                else  //set variable to stack
                {
                    aStack.Add( aObj[ni].Get( k1, k2 ) );
                }
                break;
            case IT_C :  // constant
                aStack.Add( aCon[ni] );
                break;
            case IT_F :
                if( ni != empty_f )
                  ErrorIf( aStack.GetCount()<1, "E23MSExec",
                         "No operands left in execution stack.");
                switch( ni )
                {
                case abs_f:
                    StackEnd(0) = fabs( StackEnd(0) );
                    break;
                case sign_f:
                    if( fabs(StackEnd(0)) > 1e-33 )
                        StackEnd(0) = StackEnd(0) /
                                      fabs( StackEnd(0) );
                    else StackEnd(0) = 0;
                    break;
                case exp_f:
                    StackEnd(0) = exp( StackEnd(0) );
                    break;
                case sqrt_f:
                    ErrorIf( StackEnd(0)<=1e-33, "E07MSExec",
                             "Attempt of sqrt() argument <= 0");
                    StackEnd(0) = sqrt( StackEnd(0) );
                    break;
                case ln_f:
                    ErrorIf( StackEnd(0)<=1e-33, "E08MSExec",
                             "Attempt of ln() argument <= 0");
                    StackEnd(0) = log( StackEnd(0) );
                    break;
                case lg_f:
                    ErrorIf( StackEnd(0)<=1e-33, "E09MSExec",
                             "Attempt of lg() argument <= 0");
                    StackEnd(0) = 0.434294481* log( StackEnd(0) );
                    break;
                case sin_f  :
                    StackEnd(0) = sin( StackEnd(0) );
                    break;
                case sinh_f :
                    StackEnd(0) = sinh( StackEnd(0) );
                    break;
                case cos_f  :
                    StackEnd(0) = cos( StackEnd(0) );
                    break;
                case cosh_f :
                    StackEnd(0) = cosh( StackEnd(0) );
                    break;
                case tan_f  :
                    StackEnd(0) = tan( StackEnd(0) );
                    break;
                case tanh_f :
                    StackEnd(0) = tanh( StackEnd(0) );
                    break;
                case ceil_f :
                    StackEnd(0) = ceil( StackEnd(0) );
                    break;
                case floor_f:
                    StackEnd(0) = floor( StackEnd(0) );
                    break;
                case round_f:
                    StackEnd(0) =(double)ROUND(StackEnd(0));
                    break;
                case asin_f :
                    ErrorIf( fabs(StackEnd(0))>1, "E10MSExec",
                             "Attempt of asin() |argument| < 0");
                    StackEnd(0) = asin( StackEnd(0) );
                    break;
                case acos_f :
                    ErrorIf( fabs(StackEnd(0))>1, "E11MSExec",
                             "Attempt of acos() |argument| < 0");
                    StackEnd(0) = acos( StackEnd(0) );
                    break;
                case atan_f :
                    StackEnd(0) = atan( StackEnd(0) );
                    break;
                case erf_f :
                    StackEnd(0) = derf( StackEnd(0) );
                    break;
                case erfc_f :
                    StackEnd(0) = derfc( StackEnd(0) );
                    break;
                case empty_f :
                       aStack.Add( DOUBLE_EMPTY );
                     break;
                case mod_f  :
                    ErrorIf( StackEnd(0)==0||aStack.GetCount()<2,
                             "E12MSExec","Missing mod() argument(s).");
                    StackEnd(-1) =
                        (double)( ROUND (StackEnd(-1))%
                                  ROUND (StackEnd(0)) );

                    StackDel();
                    break;
                case pow_f  :
                    StackEnd(-1) =
                        pow (  StackEnd(-1),  StackEnd(0) );
                    StackDel();
                    break;
                default:
                    Error("E00MSExec","Illegal function code");
                }
                break;
            case IT_I :    // interval
                i++;
                ErrorIf(aItm[i].code != IT_V, "E14MSExec",
                        "Missing interval function argument(s).");
                ni = aItm[i].num;
                i++;
                ErrorIf(aItm[i].code != IT_F, "E15MSExec",
                        "Missing interval function code.");
                if( aObj[ni].GetM()>1 )
                {
                    k4 = ROUND( StackEnd(0) );
                    StackDel();
                    k3 = ROUND( StackEnd(0) );
                    StackDel();
                }
                else k3 = k4 = 0;
                if( aObj[ni].GetN()>1 )
                {
                    k2 = ROUND( StackEnd(0) );
                    StackDel();
                    k1 = ROUND( StackEnd(0) );
                    StackDel();
                }
                else k1 = k2 = 0;
                switch( aItm[i].num )
                {
                case sum_f :
                    aStack.Add( 0. );
                    for( j1 = k1; j1 <= k2; j1++ )
                        for( j2 = k3; j2 <= k4; j2++ )
                        {
                            z = aObj[ni].Get( j1, j2 );
                            StackEnd(0) += z;
                        }
                    break;
                case prod_f :
                    aStack.Add( 1. );
                    for( j1 = k1; j1 <= k2; j1++ )
                        for( j2 = k3; j2 <= k4; j2++ )
                        {
                            z = aObj[ni].Get( j1, j2 );
                            StackEnd(0) *= z;
                            if( fabs(StackEnd(0))<1.e-34) break;
                        }
                    break;
                case max_f :
                    aStack.Add( aObj[ni].Get( k1, k3 ) );
                    for( j1 = k1; j1 <= k2; j1++ )
                        for( j2 = k3; j2 <= k4; j2++ )
                        {
                            z = aObj[ni].Get( j1, j2 );
                            if( (z-StackEnd(0))>1.e-34)
                                StackEnd(0) = z;
                        }
                    break;
                case min_f :
                    aStack.Add( aObj[ni].Get( k1, k3 ) );
                    for( j1 = k1; j1 <= k2; j1++ )
                        for( j2 = k3; j2 <= k4; j2++ )
                        {
                            z = aObj[ni].Get( k1, k2 );
                            if( (z-StackEnd(0)) < 0 )
                                StackEnd(0) = z;
                        }
                    break;
                case null_f:
                    for( j1 = k1; j1 <= k2; j1++ )
                        for( j2 = k3; j2 <= k4; j2++ )
                            aObj[ni].Put( 0., j1, j2 );
                    break;
                case one_f:
                    for( j1 = k1; j1 <= k2; j1++ )
                        for( j2 = k3; j2 <= k4; j2++ )
                            aObj[ni].Put( 1., j1, j2 );
                    break;
                default  :
                    Error("E20MSExec","Illegal function code");
                }
                break;
            case IT_T :
                if( fabs(StackEnd(0)) < 1e-33 )
                    i += ni;
                StackDel();
                break;
            case IT_G :
                i += ni-1;
                break;
            default  :
                Error("E16MSExec","Invalid element code in execution stack.");
            }
            /*if( TraceStatus )
                TraceStatus = TraceYNprint( rpn, ieq, i, nstack, stack, 0 );*/
            i++;
            ci = aItm[i].code;
            ni = aItm[i].num;
        }
        if( aStack.GetCount()>0 )
            Error( "E17MSExec","Stack is not empty after execution.");
    }
    return;
}

//print IPN notation
void IPNCalc::PrintEquat( char *s, fstream& f)
{
    int k,l=0,j;

    f << "Source expressions :\n" << s;
    for(uint i=0; i < aEq.GetCount(); l=0, i++)
    {
        f << "Expression " << i << "\n";
        for( j=aEq[i].first; j<aEq[i].first+aEq[i].nItems; l++, j++)
        {
            if( l > 20 )
            {
                f<<"\n";
                l=0;
            }
            k = aItm[j].num;
            switch( aItm[j].code )
            {
            case IT_C:
                f<< " " << aCon[k];
                break;
            case IT_V :
                f<< " " << aObj[k].GetKeywd();
                break;
            case IT_F :
                f<< " " << fun[k].func;
                break;
            case IT_U :
                f<< " u-";
                break;
            case IT_O :
                f<< " " << OPER[k];
                break;
            case IT_A :
                f<< " =:";
                break;
            case IT_I :
                f<< " #";
                break;
            case IT_T :
                f<< " ?";
                break;
            case IT_G :
                f<< " :";
                break;
            case IT_W :
                f << " {";
                break;
            case IT_B :
                f<< " }";
                break;
            default   :
                f<< " ??";
                break;
            }
        }
        f << "\n";
    }
    f << "------------------------\n\n";
}

//Gamma / Error Functions
//    Copyright(C) 1996 Takuya OOURA (email: ooura@mmm.t.u-tokyo.ac.jp).
/* error function in double precision */

double derfc(double x)
{
    double t, u, y;

    t = 3.97886080735226 / (fabs(x) + 3.97886080735226);
    u = t - 0.5;
    y = (((((((((0.00127109764952614092 * u + 1.19314022838340944e-4) * u -
        0.003963850973605135) * u - 8.70779635317295828e-4) * u +
        0.00773672528313526668) * u + 0.00383335126264887303) * u -
        0.0127223813782122755) * u - 0.0133823644533460069) * u +
        0.0161315329733252248) * u + 0.0390976845588484035) * u +
        0.00249367200053503304;
    y = ((((((((((((y * u - 0.0838864557023001992) * u -
        0.119463959964325415) * u + 0.0166207924969367356) * u +
        0.357524274449531043) * u + 0.805276408752910567) * u +
        1.18902982909273333) * u + 1.37040217682338167) * u +
        1.31314653831023098) * u + 1.07925515155856677) * u +
        0.774368199119538609) * u + 0.490165080585318424) * u +
        0.275374741597376782) * t * exp(-x * x);
    return x < 0 ? 2 - y : y;
}
/* error function in double precision */

double derf(double x)
{
    int k;
    double w, t, y;
    static double a[65] = {
        5.958930743e-11, -1.13739022964e-9,
        1.466005199839e-8, -1.635035446196e-7,
        1.6461004480962e-6, -1.492559551950604e-5,
        1.2055331122299265e-4, -8.548326981129666e-4,
        0.00522397762482322257, -0.0268661706450773342,
        0.11283791670954881569, -0.37612638903183748117,
        1.12837916709551257377,
        2.372510631e-11, -4.5493253732e-10,
        5.90362766598e-9, -6.642090827576e-8,
        6.7595634268133e-7, -6.21188515924e-6,
        5.10388300970969e-5, -3.7015410692956173e-4,
        0.00233307631218880978, -0.0125498847718219221,
        0.05657061146827041994, -0.2137966477645600658,
        0.84270079294971486929,
        9.49905026e-12, -1.8310229805e-10,
        2.39463074e-9, -2.721444369609e-8,
        2.8045522331686e-7, -2.61830022482897e-6,
        2.195455056768781e-5, -1.6358986921372656e-4,
        0.00107052153564110318, -0.00608284718113590151,
        0.02986978465246258244, -0.13055593046562267625,
        0.67493323603965504676,
        3.82722073e-12, -7.421598602e-11,
        9.793057408e-10, -1.126008898854e-8,
        1.1775134830784e-7, -1.1199275838265e-6,
        9.62023443095201e-6, -7.404402135070773e-5,
        5.0689993654144881e-4, -0.00307553051439272889,
        0.01668977892553165586, -0.08548534594781312114,
        0.56909076642393639985,
        1.55296588e-12, -3.032205868e-11,
        4.0424830707e-10, -4.71135111493e-9,
        5.011915876293e-8, -4.8722516178974e-7,
        4.30683284629395e-6, -3.445026145385764e-5,
        2.4879276133931664e-4, -0.00162940941748079288,
        0.00988786373932350462, -0.05962426839442303805,
        0.49766113250947636708
    };
    static double b[65] = {
        -2.9734388465e-10, 2.69776334046e-9,
        -6.40788827665e-9, -1.6678201321e-8,
        -2.1854388148686e-7, 2.66246030457984e-6,
        1.612722157047886e-5, -2.5616361025506629e-4,
        1.5380842432375365e-4, 0.00815533022524927908,
        -0.01402283663896319337, -0.19746892495383021487,
        0.71511720328842845913,
        -1.951073787e-11, -3.2302692214e-10,
        5.22461866919e-9, 3.42940918551e-9,
        -3.5772874310272e-7, 1.9999935792654e-7,
        2.687044575042908e-5, -1.1843240273775776e-4,
        -8.0991728956032271e-4, 0.00661062970502241174,
        0.00909530922354827295, -0.2016007277849101314,
        0.51169696718727644908,
        3.147682272e-11, -4.8465972408e-10,
        6.3675740242e-10, 3.377623323271e-8,
        -1.5451139637086e-7, -2.03340624738438e-6,
        1.947204525295057e-5, 2.854147231653228e-5,
        -0.00101565063152200272, 0.00271187003520095655,
        0.02328095035422810727, -0.16725021123116877197,
        0.32490054966649436974,
        2.31936337e-11, -6.303206648e-11,
        -2.64888267434e-9, 2.050708040581e-8,
        1.1371857327578e-7, -2.11211337219663e-6,
        3.68797328322935e-6, 9.823686253424796e-5,
        -6.5860243990455368e-4, -7.5285814895230877e-4,
        0.02585434424202960464, -0.11637092784486193258,
        0.18267336775296612024,
        -3.67789363e-12, 2.0876046746e-10,
        -1.93319027226e-9, -4.35953392472e-9,
        1.8006992266137e-7, -7.8441223763969e-7,
        -6.75407647949153e-6, 8.428418334440096e-5,
        -1.7604388937031815e-4, -0.0023972961143507161,
        0.0206412902387602297, -0.06905562880005864105,
        0.09084526782065478489
    };

    w = x < 0 ? -x : x;
    if (w < 2.2) {
        t = w * w;
        k = (int) t;
        t -= k;
        k *= 13;
        y = ((((((((((((a[k] * t + a[k + 1]) * t +
            a[k + 2]) * t + a[k + 3]) * t + a[k + 4]) * t +
            a[k + 5]) * t + a[k + 6]) * t + a[k + 7]) * t +
            a[k + 8]) * t + a[k + 9]) * t + a[k + 10]) * t +
            a[k + 11]) * t + a[k + 12]) * w;
    } else if (w < 6.9) {
        k = (int) w;
        t = w - k;
        k = 13 * (k - 2);
        y = (((((((((((b[k] * t + b[k + 1]) * t +
            b[k + 2]) * t + b[k + 3]) * t + b[k + 4]) * t +
            b[k + 5]) * t + b[k + 6]) * t + b[k + 7]) * t +
            b[k + 8]) * t + b[k + 9]) * t + b[k + 10]) * t +
            b[k + 11]) * t + b[k + 12];
        y *= y;
        y *= y;
        y *= y;
        y = 1 - y * y;
    } else {
        y = 1;
    }
    return x < 0 ? -y : y;
}

//--------------------- End of v_ipnc.cpp ---------------------------

