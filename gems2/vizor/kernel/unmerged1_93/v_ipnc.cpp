//-------------------------------------------------------------------
// Id: gems/vizor/core/v_ipnc.cpp  version 2.0.0  edited 2000-07-10
//
// Implementation of IPNCalc
//
// Created : 970207    Modified: 010908
//
// Copyright (C) 1996-2001  S.Dmytriyeva
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

#include <math.h>
#include <stdio.h>
#include "gstring.h"
#include "v_ipnc.h"
#include "v_object.h"

char* DIGIT="0123456789.";
char* OPER="!^*/+-<a>b=c&|()";
char* RAZD=" +-*/^:[]();=$&|!<>?#";
const int FuncNumber=25; // number of functions

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
    { "mod", '\0', I_, 2, 1 },
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

int IPNCalc::INDEX( char *s, char ch )
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

// skip gstring const "<символы>", in struct put
// ascii code of the first character in gstring
void IPNCalc::IsAscii()
{
    double d;

    input++;
    d = (double)(*input);
    Push( IT_C, con_add(d) );
    input = strchr( input, '"' );
    ErrorIf( !input ,"IPNTranslate",
             "Error in gstring constant (missing \").");
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
        err+= "is not defined.";
        Error( "IPNTranslate", err.c_str() );
    }
    if( fun[i].par ) // argument is expression
    {
        for( j=0; j<( fun[i].arn )-1; j++ )
            RPN_expr( ',');
        RPN_expr( ')');
        Push( IT_F, i);
    }
    else
    { // argument is interval
        if( !IsLetter( *input ) )
        {
            err = "Missing identifier of interval:\n";
            err += input;
            Error( "IPNTranslate", err.c_str() );
        }
        memset( st, '\0', MAXKEYWD );
        //st[MAXKEYWD-1] = '\0';
        Ident( st );
        I_Variab( st );
        if( (input=xblanc( input ))==0 ) goto OSH;
        if( *input!=')' )
        {
            err = "Missing ):\n";
            err += input;
            Error( "IPNTranslate", err.c_str() );
        }
        input++;
        Push( IT_F, i);
    }
    return;
OSH:
    Error( "IPNTranslate", "Unexpected end of Math Script text");
}

//analyse IPN variable
void IPNCalc::Variab( char *str)
{
    int j = aObj.Find(str);

    if( j<0 )
    {
        gstring err = "Variable ";
        err += str;
        err+= "is not declared.";
        Error( "IPNTranslate", err.c_str() );
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
    Error( "IPNTranslate", "Unexpected end of Math Script text");
}

//analyse IPN interval variable
void IPNCalc::I_Variab( char * str)
{
    int j = aObj.Find(str);

    if( j<0 )
    {
        gstring err = "Variable ";
        err += str;
        err+= "is not declared.";
        Error( "IPNTranslate", err.c_str() );
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
    Error( "IPNTranslate", "Unexpected end of Math Script text");
}

//get IPN cod of one equat
void IPNCalc::bildEquat()
{
    vstr str(MAXKEYWD);
    char *s;
    gstring err;
    int i,j;

    input = s = xblanc( input );
    ErrorIf( s==0, "IPNTranslate", "No equations given");
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
            err = "=: needed for the assignment:\n";
            err += s;
            Error(  "IPNTranslate", err.c_str() );
        }
        input = strchr( s, '=' ) + 2;
        if( ( input=xblanc( input )) ==0 ) goto OSH;
        if( *input==';' )
        {
            err = "Expression is needed here:\n";
            err += input;
            Error(  "IPNTranslate", err.c_str() );
        }
        RPN_expr( CK_ );
        if( ( input=xblanc( s ))==0 ) goto OSH;
        if( !(IsLetter( *input )) )
        {
            err = "Variable identifier is needed here:\n";
            err += input;
            Error(  "IPNTranslate", err.c_str() );
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
        err = "Illegal characters at the end of expression text:\n";
        err += input;
        Error(  "IPNTranslate", err.c_str() );
    }
    return;
OSH :
    Error( "IPNTranslate", "Unexpected end of Math Script text");
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
        Error(  "IPNTranslate", err.c_str() );
    }
    input++;
    neqbg = aEq.GetCount();
    eq_add( 0, aItm.GetCount() ); //  eq[Neqs].first = Nitems; // conditions
    if( *input==')')
    {
        err = "Expression is needed here:\n";
        err += input;
        Error(  "IPNTranslate", err.c_str() );
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
        err = "Missing begin:\n";
        err += input;
        Error(  "IPNTranslate", err.c_str() );
    }
    input+=5;
    bildEquat();
    if( strncmp( input, "end", 3 ))
    {
        err = "Missing end:\n";
        err += input;
        Error(  "IPNTranslate", err.c_str() );
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
    Error( "IPNTranslate", "Unexpected end of Math Script text");
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
        err = "Missing (:\n";
        err += input;
        Error(  "IPNTranslate", err.c_str() );
    }
    input++;
    eq_add(  0, aItm.GetCount() );  // eq[Neqs].first = Nitems; // conditions
    if( *input==')' )
    {
        err = "Expression is needed here:\n";
        err += input;
        Error(  "IPNTranslate", err.c_str() );
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
        err = "Missing begin:\n";
        err += input;
        Error(  "IPNTranslate", err.c_str() );
    }
    input+=5;
    bildEquat();
    if( strncmp( input,"end", 3 ))
    {
        err = "Missing end:\n";
        err += input;
        Error(  "IPNTranslate", err.c_str() );
    }
    input+=3;
    // unconditional jump to equation
    eq_add(  0, aItm.GetCount() );  // 	eq[Neqs].first = Nitems;
    it2 = aItm.GetCount();
    Push( IT_B, aEq.GetCount()+1 );
    eq_add( aItm.GetCount() );
    aItm[it].num = aEq.GetCount();
    if( ((input=xblanc( input ))!=0) && !strncmp( input,"else", 4 ))
    { // condition else
        input+=4;
        // conditional jump to equation
        eq_add(  0, aItm.GetCount() );  // 	eq[Neqs].first = Nitems;
        Push( IT_W, aEq.GetCount()+1 );
        eq_add( aItm.GetCount());
        // analyse before  'end'
        if( ( input=xblanc( input ))==0 ) goto OSH;
        if( strncmp( input,"begin", 5 ))
        {
            err = "Missing begin:\n";
            err += input;
            Error(  "IPNTranslate", err.c_str() );
        }
        input+=5;
        bildEquat();
        if( strncmp( input,"end", 3 ))
        {
            err = "Missing end:\n";
            err += input;
            Error(  "IPNTranslate", err.c_str() );
        }
        input+=3;
        // unconditional jump to equation
        eq_add(  0, aItm.GetCount() );  // 	eq[Neqs].first = Nitems;
        Push( IT_W, aEq.GetCount()+1 );
        eq_add( aItm.GetCount());
        /*eq[Neqs].first = Nitems;
          Push( IT_B, Neqs+1 );
          eq_add();*/
        aItm[it2].num = aEq.GetCount();
    }
    input=xblanc( input );
    return;
OSH :
    Error( "IPNTranslate", "Unexpected end of Math Script text");
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
                    err = "Missing ):\n";
                    err += input;
                    Error(  "IPNTranslate", err.c_str() );
                }
                input++;
                Push( IT_U, 0 );
            }
            else
            {
                ErrorIf( itec>=MAXSTACK-1,"IPNTranslate",
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
                    err = "No match of ( and ):\n";
                    err += input;
                    Error("IPNTranslate", err.c_str() );
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
            ErrorIf( itec>=MAXSTACK-1, "IPNTranslate",
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
            err = "Illegal character:\n";
            err += input;
            Error(  "IPNTranslate", err.c_str() );
        }
        if( (input=xblanc(input))==0 ) goto OSH;
    }
    while( itec>=0 )
    {
        op = stack[ itec-- ];
        if( op=='(' )
        {
            err = "No match of ( and ):\n";
            err += input;
            Error(  "IPNTranslate", err.c_str() );
        }
        Push( IT_O, INDEX( OPER, op ));
    }
    input++;
    return;
OSH:
    Error( "IPNTranslate", "Unexpected end of Math Script text");
}

//get IPN notation
void IPNCalc::GetEquat( char *txt )
{
    input = txt;
    ErrorIf( !input || *input=='\0', "IPNTranslate",
             "No Math Script text to analyse!");
    aItm.Clear( false );
    aEq.Clear( false );
    aCon.Clear( false );
    con_add( 0.0 );
    con_add( 1.0 );

    bildEquat();
    ErrorIf( input!=0, "IPNTranslate", "Math Script equation syntax error");
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
            ErrorIf( ni<ieq, "IPNCalc", "Illegal goto");
            if( fabs( aObj[o_k_].Get() ) < 1e-10 )
                ieq = ni;
            else ieq++;
            continue;
        }
        if( ci == IT_B ) // unconditional jump to equation
        {
            ErrorIf( ni > aEq.GetCount()+1, "IPNCalc",
                     "Illegal goto");
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
                    Error( "IPNCalc", "No operands left in the stack");
                switch( ni )
                {
                case 0 :
                    if( fabs(StackEnd(0)) >1e-34 )
                        StackEnd(0) = 0.;
                    else StackEnd(0) = 1.;
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
                             "IPNCalc","Attempt of zerodivide!");
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
                    Error("IPNCalc","Illegal binary operator");
                }
                break;
            case IT_U :
                ErrorIf( aStack.GetCount()<1, "IPNCalc",
                         "No operands left in the stack (2)");
                switch( ni )
                {
                case 0 :
                    StackEnd(0) = - StackEnd(0);
                    break;
                default:
                    Error("IPNCalc","Illegal unary operator");
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
                ErrorIf( aStack.GetCount()<1, "IPNCalc",
                         "No operands left in the stack (3)");
                switch( ni )
                {
                case abs_f:
                    StackEnd(0) = fabs( StackEnd(0) );
                    break;
                case sign_f:
                    if( fabs(StackEnd(0)) > 1e-34 )
                        StackEnd(0) = StackEnd(0) /
                                      fabs( StackEnd(0) );
                    else StackEnd(0) = 0;
                    break;
                case exp_f:
                    StackEnd(0) = exp( StackEnd(0) );
                    break;
                case sqrt_f:
                    ErrorIf( StackEnd(0)<=1e-34, "IPNCalc",
                             "sqrt argument < 0");
                    StackEnd(0) = sqrt( StackEnd(0) );
                    break;
                case ln_f:
                    ErrorIf( StackEnd(0)<=1e-34, "IPNCalc",
                             "ln argument < 0");
                    StackEnd(0) = log( StackEnd(0) );
                    break;
                case lg_f:
                    ErrorIf( StackEnd(0)<=1e-34, "IPNCalc",
                             "lg argument < 0");
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
                    ErrorIf( fabs(StackEnd(0))>1, "IPNCalc",
                             "asin |argument| < 0");
                    StackEnd(0) = asin( StackEnd(0) );
                    break;
                case acos_f :
                    ErrorIf( fabs(StackEnd(0))>1, "IPNCalc",
                             "acos |argument| < 0");
                    StackEnd(0) = acos( StackEnd(0) );
                    break;
                case atan_f :
                    StackEnd(0) = atan( StackEnd(0) );
                    break;
                case mod_f  :
                    ErrorIf( StackEnd(0)==0||aStack.GetCount()<2,
                             "IPNCalc","mod() execution error");
                    StackEnd(-1) =
                        (double)( ROUND (StackEnd(-1))%
                                  ROUND (StackEnd(0)) );

                    StackDel();
                    break;
                default:
                    Error("IPNCalc","Undefined function");
                }
                break;
            case IT_I :    // interval
                i++;
                ErrorIf(aItm[i].code != IT_V, "IPNCalc",
                        "Missing variable (interval function)");
                ni = aItm[i].num;
                i++;
                ErrorIf(aItm[i].code != IT_F, "IPNCalc",
                        "Missing function (interval function)");
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
                    Error("IPNCalc","Undefined function!");
                }
                break;
            case IT_T :
                if( fabs(StackEnd(0)) < 1e-34 )
                    i += ni;
                StackDel();
                break;
            case IT_G :
                i += ni-1;
                break;
            default  :
                Error("IPNCalc","Invalid element code!");
            }
            /*if( TraceStatus )
                TraceStatus = TraceYNprint( rpn, ieq, i, nstack, stack, 0 );*/
            i++;
            ci = aItm[i].code;
            ni = aItm[i].num;
        }
        if( aStack.GetCount()>0 )
            Error( "IPNCalc","Stack is not empty after calculation!");
    }
    return;
}

//print IPN notation
void IPNCalc::PrintEquat( char *s, fstream& f)
{
    int k,l,j;

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

//--------------------- End of v_ipnc.cpp ---------------------------

