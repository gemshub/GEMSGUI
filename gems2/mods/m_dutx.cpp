//-------------------------------------------------------------------
// Id: gems/mods/m_dutx.cpp  version 2.0.0   2001
// To be finalized in Version 4.0 (2005)
// Implementation of TDuterm class, calculation functions
//
// Rewritten from C to C++ by S.Dmytriyeva  970207 modified 010904
// Copyright (C) 1995-2001 S.Dmytriyeva, D.Kulik
//
// This file is part of a GEM-Selektor library for thermodynamic
// modelling by Gibbs energy minimization
// Uses: GEM-Vizor GUI DBMS library, gems/lib/gemvizor.lib
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL)
//
// See http://les.web.psi.ch/Software/GEMS-PSI for more information
// E-mail: gems2.support@psi.ch
//-------------------------------------------------------------------
//
#include <math.h>

#include "m_duterm.h"
#include "m_syseq.h"
#include "v_object.h"
#include "s_formula.h"
#include "service.h"
#include "visor.h"

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/*'lnAx' Расчет логарифма активности для данного компонента
* Данные должны быть загружены в структуру DUTERMSET dt
* DCC - класс зависимого компонента
* Возвращает lnAx или F_EMPTY при ошибке расчета
*/
double TDuterm::dut_lnAx_calc( char DCC )
{
    double logA=DOUBLE_EMPTY;

    dt.lnCx = log( dt.Cx );
    dt.lnWx = log( dt.Wx );
    dt.Gsas = 1. - dt.Wx;  /* Для Wx умалч. = 1 */
    dt.Gcas = 2. - dt.Wx - 1./dt.Wx;
    dt.Gpsi = dt.Ze * dt.Psi * dt.F/dt.RT;
    if( IsDoubleEmpty( dt.Gex ))
        dt.Gex = dt.lnGam;
    if( IsDoubleEmpty( dt.Agmx ))
        dt.Agmx = dt.Agx * 6.02e5;
    if( IsDoubleEmpty( dt.Nx ))
        dt.Nx = dt.Asur * dt.Agmx * dt.Amw;
    if( IsDoubleEmpty( dt.Gsd ))
        dt.Gsd = -log( dt.Tetp * dt.Nx / (1.-dt.Nx) ); /* dt.Tetp умалч. 1 */

    switch( DCC )
    { /* Водный электролит */
    case DC_AQ_PROTON:
    case DC_AQ_ELECTRON:
    case DC_AQ_SPECIES:
        dt.lnMx = dt.lnCx - dt.lnWx;
        logA = dt.Gsm + dt.lnGam + dt.lnCx - dt.lnWx
               + dt.Gsas;
        break;
        /* Рассчитать мольный вес растворителя !!!!!!!!!!!!!!!!!!!!!!!!!!! */
    case DC_AQ_SOLVCOM:
    case DC_AQ_SOLVENT:
        dt.lnMx = dt.Gsm;
        logA = dt.lnGam + dt.lnCx + dt.Gcas;
        break;
    case DC_GAS_COMP:   /* Газы, кроме H2O и CO2 */
    case DC_GAS_H2O:
    case DC_GAS_CO2:
    case DC_GAS_H2:
    case DC_GAS_N2:
        logA = dt.lnGam + dt.lnCx + dt.lnPg;
        break;
    case DC_SCP_CONDEN: /* Однокомпонентные фазы */
        logA = 0;
        break;
    case DC_SOL_MINOR:
    case DC_SOL_MAJOR:
    case DC_SOL_IDEAL:  /* Растворы неэлектролитов */
        logA = dt.lnGam + dt.lnCx;
        break;
    case DC_SUR_MINAL:
    case DC_SUR_CARRIER:
    case DC_PEL_CARRIER: /* Фазы с сорбцией - сорбенты */
        logA = dt.lnGam + dt.lnCx + dt.Gcas;
        break;
    case DC_SUR_SITE:
    case DC_SUR_COMPLEX:
    case DC_SUR_IPAIR:    /* Фазы с сорбцией - сорбаты */
        logA = dt.lnGam + dt.lnMx + dt.Gsas + dt.Gsm + dt.Gpsi + dt.Gsd;
        break;
        /*
            case DC_SUR_DL_ION:
            case DC_SUR_EX_ION:  * Обменные ионы *
                logA = dt.lnGam + dt.lnMx + dt.Gsas + dt.Gsm + dt.Gpsi + dt.Gsd;
                                 break;
         
            case DC_PEL_SITE:
            case DC_PEL_COMPLEX: * Комплексы на поли- и олигоэлектролите *
                logA = dt.lnGam + dt.lnMx + dt.Gsas + dt.Gsm + dt.Gpsi + dt.Gsd;
                                 break;
        */
    default:
        if( isdigit( DCC ))
        {  /* Это пов. комплекс типа p */
            logA = dt.lnGam + dt.lnMx + dt.Gsas + dt.Gsm + dt.Gpsi + dt.Gsd;
            break;
        } /* иначе неверный код */
        Error( GetName(), "Error DCC code.");
    }
    /* Окончательный пересчет */
    dt.lnAx = logA;
    dt.Ax = exp( dt.lnAx );
    return logA;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/*'Nu-Mu' Расчет прямого хим.потенциала Mu и разности Nu-Mu
* Данные должны быть загружены в структуру DUTERMSET dt
* DCC - класс зависимого компонента
* Возвращает Mu-Nu или F_EMPTY при ошибке расчета
*/
double TDuterm::dut_Mu_Nu_calc( char DCC )
{
    double logA, Mu_Nu;

    if( IsDoubleEmpty( dt.Gdis ))
        dt.Gdis = 0.666666 * dt.Asig * dt.Asur * dt.Amw;
    logA = dut_lnAx_calc( DCC );
    dt.lnAx = logA;
    dt.Ax = exp( logA );
    dt.Mu = dt.G0 + dt.dGex + dt.Gdis + dt.lnAx;
    Mu_Nu = dt.Nu - dt.Mu;

    return Mu_Nu;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/*'g0' Расчет стандартного хим.потенциала ЗК G0 (нормированного)
* Данные должны быть загружены в структуру DUTERMSET dt
* DCC - класс зависимого компонента
* Возвращает G0 или F_EMPTY при ошибке расчета
*/
double TDuterm::dut_G0TP_calc( char DCC )
{
    double logA, G0;

    if( IsDoubleEmpty( dt.Gdis ))
        dt.Gdis = 0.666666 * dt.Asig * dt.Asur * dt.Amw;
    logA = dut_lnAx_calc( DCC );
    dt.lnAx = logA;
    dt.Ax = exp( dt.lnAx );
    G0 = dt.Nu - dt.dGex - dt.Gdis - dt.lnAx;
    dt.G0 = G0;
    /*  dt.Mu = dt.G0 + dt.dGex + dt.Gdis + dt.lnAx; */
    return G0;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/*'dGex' Расчет потенциала метастабильности ЗК dGex (нормированного)
* Данные должны быть загружены в структуру DUTERMSET dt
* DCC - класс зависимого компонента
* Возвращает dGex или F_EMPTY при ошибке расчета
*/
double TDuterm::dut_Gexk_calc( char DCC )
{
    double logA, dGex;

    if( IsDoubleEmpty( dt.Gdis ))
        dt.Gdis = 0.666666 * dt.Asig * dt.Asur * dt.Amw;
    logA = dut_lnAx_calc( DCC );
    dt.lnAx = logA;
    dt.Ax = exp( dt.lnAx );
    dGex = dt.Nu - dt.G0 - dt.Gdis - dt.lnAx;
    dt.dGex = dGex;
    /*  dt.Mu = dt.G0 + dt.dGex + dt.Gdis + dt.lnAx; */
    return dGex;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/*'Gdis' Расчет свободной энергии дисперсности ЗК Gdis (нормированной)
* Данные должны быть загружены в структуру DUTERMSET dt
* DCC - класс зависимого компонента
* Возвращает Gdis или F_EMPTY при ошибке расчета
*/
double TDuterm::dut_Gdis_calc( char DCC )
{
    double logA, Gdis;

    logA = dut_lnAx_calc( DCC );
    dt.lnAx = logA;
    dt.Ax = exp( dt.lnAx );
    Gdis = dt.Nu - dt.G0 - dt.dGex - dt.lnAx;
    dt.Gdis = Gdis;
    /*  dt.Mu = dt.G0 + dt.dGex + dt.Gdis + dt.lnAx; */
    return Gdis;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/*'Asur' Расчет удельной площади поверхности дисп. фазы Asur (м2/г)
* Данные должны быть загружены в структуру DUTERMSET dt
* DCC - класс зависимого компонента
* Возвращает Asur или F_EMPTY при ошибке расчета
*/
double TDuterm::dut_Asur_calc( char DCC )
{
    double logA, Asur, Gdis;

    logA = dut_lnAx_calc( DCC );

    dt.lnAx = logA;
    dt.Ax = exp( dt.lnAx );
    Gdis = dt.Mu - dt.G0 - dt.dGex - dt.lnAx;
    Asur = Gdis / ( 0.666666 * dt.Asig * dt.Amw );
    ErrorIf( fabs(dt.Gdis - Gdis) > 1e3, GetName(),
             "Gdis calculated and inserted do not equal." );
    dt.Asur = Asur;
    dt.Gdis = Gdis;
    return Asur;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/*'Asig' Расчет удельной своб.энергии поверхности дисп. фазы Asig (J/m2)
* Данные должны быть загружены в структуру DUTERMSET dt
* DCC - класс зависимого компонента
* Возвращает Asig или F_EMPTY при ошибке расчета
*/
double TDuterm::dut_Asig_calc( char DCC )
{
    double logA, Asig, Gdis;

    logA = dut_lnAx_calc( DCC );
    dt.lnAx = logA;
    dt.Ax = exp( dt.lnAx );
    Gdis = dt.Mu - dt.G0 - dt.dGex - dt.lnAx;
    Asig = Gdis / ( 0.666666 * dt.Asur * dt.Amw );
    ErrorIf( fabs(dt.Gdis - Gdis) > 1e3, GetName(),
             "Gdis calculated and inserted do not equal." );
    dt.Asig = Asig;
    dt.Gdis = Gdis;
    return Asig;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/*'Gex' Расчет избыточн.своб.энерг. неидеальности ЗК Gex,
* а также коэфф. активности
* Данные должны быть загружены в структуру DUTERMSET dt
* DCC - класс зависимого компонента
* Возвращает Gex или F_EMPTY при ошибке расчета
*/
double TDuterm::dut_Gex_calc( char DCC )
{
    double logA, Gex;

    if( IsDoubleEmpty( dt.Gdis ))
        dt.Gdis = 0.666666 * dt.Asig * dt.Asur * dt.Amw;

    dt.lnCx = log( dt.Cx );
    dt.lnWx = log( dt.Wx );
    dt.Gsas = 1. - dt.Wx;  /* Для Wx умалч. = 1 */
    dt.Gcas = 2. - dt.Wx - 1./dt.Wx;
    dt.Gpsi = dt.Ze * dt.Psi * dt.F/dt.RT;
    if( IsDoubleEmpty( dt.Agmx ))
        dt.Agmx = dt.Agx * 6.02e5;
    if( IsDoubleEmpty( dt.Nx ))
        dt.Nx = dt.Asur * dt.Agmx * dt.Amw;
    if( IsDoubleEmpty( dt.Gsd ))
        dt.Gsd = -log( dt.Tetp * dt.Nx / (1.-dt.Nx) ); /* dt.Tetp умалч. 1 */
    /* вычисление логарифма активности */
    logA = dt.Nu - dt.G0 - dt.dGex - dt.Gdis;

    switch( DCC )
    { /* Водный электролит */
    case DC_AQ_PROTON:
    case DC_AQ_ELECTRON:
    case DC_AQ_SPECIES:
        dt.lnMx = dt.lnCx - dt.lnWx;
        Gex = logA - dt.Gsm - dt.lnCx + dt.lnWx - dt.Gsas;
        break;
        /* Рассчитать мольный вес растворителя !!!!!!!!!!!!!!!!!!!!!!!!!!! */
    case DC_AQ_SOLVCOM:
    case DC_AQ_SOLVENT:
        Gex = logA - dt.lnCx - dt.Gcas;
        break;
    case DC_GAS_COMP:   /* Газы, кроме H2O и CO2 */
    case DC_GAS_H2O:
    case DC_GAS_CO2:
    case DC_GAS_H2:
    case DC_GAS_N2:
        Gex = logA - dt.lnCx - dt.lnPg;
        break;
    case DC_SCP_CONDEN: /* Однокомпонентные фазы */
        /*Gex = 0.;*/
        dt.Gamma = 1;
        dt.lnGam = 0;
        return 0.0;
    case DC_SOL_MINOR:
    case DC_SOL_MAJOR:
    case DC_SOL_IDEAL:  /* Растворы неэлектролитов */
        Gex = logA - dt.lnCx;
        break;
    case DC_SUR_MINAL:
    case DC_SUR_CARRIER:
    case DC_PEL_CARRIER: /* Фазы с сорбцией - сорбенты */
        Gex = logA - dt.lnCx - dt.Gcas;
        break;
    default:  /* иначе неверный код */
        Error( GetName(), "Error DCC code.");
    }
    /* Окончательный пересчет */
    dt.Gex = Gex;
    dt.lnGam = Gex;
    dt.Gamma = exp( Gex );
    return Gex;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/*'surGam' Расчет коэффициента поверхностной активности,
* а также Gex для сорбатов.
* Данные должны быть загружены в структуру DUTERMSET dt
* DCC - класс зависимого компонента
* Возвращает Gex или F_EMPTY при ошибке расчета
*/
double TDuterm::dut_sGam_calc( char DCC )
{
    double logX, logA, Gex;

    if( IsDoubleEmpty( dt.Gdis ))
        dt.Gdis = 0.666666 * dt.Asig * dt.Asur * dt.Amw;

    logX = log( dt.Cx );
    dt.lnMx = logX - log( dt.Wx );
    dt.Gsas = 1. - dt.Wx;  /* Для Wx умалч. = 1 */
    dt.Gcas = 2. - dt.Wx - 1./dt.Wx;
    dt.Gpsi = dt.Ze * dt.Psi * dt.F/dt.RT;
    if( IsDoubleEmpty( dt.Agmx ))
        dt.Agmx = dt.Agx * 6.02e5;
    if( IsDoubleEmpty( dt.Nx ))
        dt.Nx = dt.Asur * dt.Agmx * dt.Amw;
    if( IsDoubleEmpty( dt.Gsd ))
        dt.Gsd = -log( dt.Tetp * dt.Nx / (1.-dt.Nx) ); /* dt.Tetp умалч. 1 */
    /* вычисление логарифма активности */
    logA = dt.Mu - dt.G0 - dt.dGex - dt.Gdis;

    switch( DCC )
    {
    case DC_SUR_SITE:
    case DC_SUR_COMPLEX:
    case DC_SUR_IPAIR:    /* Фазы с сорбцией - сорбаты */
        Gex = logA - dt.lnMx - dt.Gsas - dt.Gsm - dt.Gpsi - dt.Gsd;
        break;
        /*
            case DC_SUR_DL_ION:
            case DC_SUR_EX_ION:  * Обменные ионы *
                Gex = logA - dt.lnMx - dt.Gsas - dt.Gsm - dt.Gpsi - dt.Gsd;
                                 break;
            case DC_PEL_SITE:
            case DC_PEL_COMPLEX: * Комплексы на поли- и олигоэлектролите *
                Gex = logA - dt.lnMx - dt.Gsas - dt.Gsm - dt.Gpsi - dt.Gsd;
                                 break;
        */
    default:
        if( isdigit( DCC ))
        {  /* Это пов. комплекс типа p */
            Gex = logA - dt.lnMx - dt.Gsas - dt.Gsm - dt.Gpsi - dt.Gsd;
            break;
        } /* иначе неверный код */
        Error( GetName(), "Error DCC code.");
    }
    /* Окончательный пересчет */
    dt.Gex = Gex;
    dt.lnGam = Gex;
    dt.Gamma = exp( Gex );
    return Gex;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/*'Cx' Расчет (кажущейся) концентрации зависимого компонента Cx
* Данные должны быть загружены в структуру DUTERMSET dt
* DCC - класс зависимого компонента
* Возвращает Cx или F_EMPTY при ошибке расчета
*/
double TDuterm::dut_Cx_calc( char DCC )
{
    double logX, logM, logA, Cx;

    if( IsDoubleEmpty( dt.Gdis ))
        dt.Gdis = 0.666666 * dt.Asig * dt.Asur * dt.Amw;

    dt.lnCx = log( dt.Cx );
    dt.lnWx = log( dt.Wx );
    dt.Gsas = 1. - dt.Wx;  /* Для Wx умалч. = 1 */
    dt.Gcas = 2. - dt.Wx - 1./dt.Wx;
    dt.Gpsi = dt.Ze * dt.Psi * dt.F/dt.RT;
    if( IsDoubleEmpty( dt.Agmx ))
        dt.Agmx = dt.Agx * 6.02e5;
    if( IsDoubleEmpty( dt.Nx ))
        dt.Nx = dt.Asur * dt.Agmx * dt.Amw;
    if( IsDoubleEmpty( dt.Gsd ))
        dt.Gsd = -log( dt.Tetp * dt.Nx / (1.-dt.Nx) ); /* dt.Tetp умалч. 1 */
    /* вычисление логарифма активности */
    logA = dt.Nu - dt.G0 - dt.dGex - dt.Gdis;

    switch( DCC )
    { /* Водный электролит */
    case DC_AQ_PROTON:
    case DC_AQ_ELECTRON:
    case DC_AQ_SPECIES:
        logX = logA - dt.lnGam - dt.Gsas - dt.Gsm + dt.lnWx;
        dt.lnMx = logX - dt.lnWx;
        break;
        /* Рассчитать мольный вес растворителя !!!!!!!!!!!!!!!!!!!!!!!!!!! */
    case DC_AQ_SOLVCOM:
    case DC_AQ_SOLVENT:
        logX = logA - dt.Gex - dt.Gcas;
        goto FINISH;
    case DC_GAS_COMP:   /* Газы, кроме H2O и CO2 */
    case DC_GAS_H2O:
    case DC_GAS_CO2:
    case DC_GAS_H2:
    case DC_GAS_N2:
        logX = logA - dt.Gex - dt.lnPg;
        goto FINISH;
    case DC_SCP_CONDEN: /* Однокомпонентные фазы */
        /*logX = 0.;*/
        dt.Cx = 1.;
        dt.lnAx = 0.;
        dt.Ax = 1.;
        return 1.0;
    case DC_SOL_MINOR:
    case DC_SOL_MAJOR:
    case DC_SOL_IDEAL:  /* Растворы неэлектролитов */
        logX = logA - dt.Gex;
        goto FINISH;
    case DC_SUR_MINAL:
    case DC_SUR_CARRIER:
    case DC_PEL_CARRIER: /* Фазы с сорбцией - сорбенты */
        logX = logA - dt.Gex - dt.Gcas;
        goto FINISH;
    case DC_SUR_SITE:
    case DC_SUR_COMPLEX:
    case DC_SUR_IPAIR:    /* Фазы с сорбцией - сорбаты */
        logM = logA - dt.Gex - dt.Gsas - dt.Gsm - dt.Gpsi - dt.Gsd;
        break;
        /*
            case DC_SUR_DL_ION:
            case DC_SUR_EX_ION:  * Обменные ионы *
                logM = logA - dt.Gex - dt.Gsas - dt.Gsm - dt.Gpsi - dt.Gsd;
                                 break;
            case DC_PEL_SITE:
            case DC_PEL_COMPLEX: * Комплексы на поли- и олигоэлектролите *
                logM = logA - dt.Gex - dt.Gsas - dt.Gsm - dt.Gpsi - dt.Gsd;
                                 break;
        */
    default:
        if( isdigit( DCC ))
        {  /* Это пов. комплекс типа p */
            logM = logA - dt.Gex - dt.Gsas - dt.Gsm - dt.Gpsi - dt.Gsd;
            break;
        } /* иначе неверный код */
        Error( GetName(), "Error DCC code.");
    }
    logX = logM - dt.lnWx;
    dt.lnMx = logM;
FINISH: /* Окончательный пересчет */
    dt.lnAx = logA;
    /*  if( fabs( logX ) < 34. )  */
    Cx = exp( logX );
    /*  else dt.Cx = exp( sign( logX )*34 ); */
    dt.Cx = Cx;
    dt.lnCx = logX;
    return Cx;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/*'Nsur' Расчет мольной доли и плотности позиций р-го типа
* Данные должны быть загружены в структуру DUTERMSET dt
* DCC - класс зависимого компонента
* Возвращает Gsd или F_EMPTY при ошибке расчета
*/
double TDuterm::dut_Nsur_calc( char DCC )
{
    double logX, logA, Gsd;

    if( IsDoubleEmpty( dt.Gdis ))
        dt.Gdis = 0.666666 * dt.Asig * dt.Asur * dt.Amw;

    logX = log( dt.Cx );
    dt.lnMx = logX - log( dt.Wx );
    dt.Gsas = 1. - dt.Wx;  /* Для Wx умалч. = 1 */
    dt.Gcas = 2. - dt.Wx - 1./dt.Wx;
    dt.Gpsi = dt.Ze * dt.Psi * dt.F/dt.RT;
    /*
      if( IsDoubleEmpty( dt.Agmx ))
        dt.Agmx = dt.Agx * 6.0221367e5;
      if( IsDoubleEmpty( dt.Nx ))
        dt.Nx = dt.Asur * dt.Agmx * dt.Amw;
      if( IsDoubleEmpty( dt.Gsd ))
        dt.Gsd = -log( dt.Tetp * dt.Nx / (1.-dt.Nx) );
    */
    /* вычисление логарифма активности */
    logA = dt.Mu - dt.G0 - dt.dGex - dt.Gdis;

    switch( DCC )
    {
    case DC_SUR_SITE:
    case DC_SUR_COMPLEX:
    case DC_SUR_IPAIR:    /* Фазы с сорбцией - сорбаты */
        Gsd = logA - dt.lnMx - dt.Gsas - dt.Gsm - dt.Gpsi - dt.Gex;
        break;
        /*
            case DC_SUR_DL_ION:
            case DC_SUR_EX_ION:  * Обменные ионы *
                Gsd = logA - dt.lnMx - dt.Gsas - dt.Gsm - dt.Gpsi - dt.Gex;
                                 break;
            case DC_PEL_SITE:
            case DC_PEL_COMPLEX: * Комплексы на поли- и олигоэлектролите *
                Gsd = logA - dt.lnMx - dt.Gsas - dt.Gsm - dt.Gpsi - dt.Gex;
                                 break;
        */
    default:
        if( isdigit( DCC ))
        {  /* Это пов. комплекс типа p */
            Gsd = logA - dt.lnMx - dt.Gsas - dt.Gsm - dt.Gpsi - dt.Gex;
            break;
        } /* иначе неверный код */
        Error( GetName(), "Error DCC code.");
    }
    /* Окончательный пересчет */
    dt.Gsd = Gsd;

    return Gsd;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/*'Gpsi' Расчет необходимой величины электростат. вклада и потенциала
* Данные должны быть загружены в структуру DUTERMSET dt
* DCC - класс зависимого компонента
* Возвращает Gpsi или F_EMPTY при ошибке расчета
*/
double TDuterm::dut_Psi_calc( char DCC )
{
    double logX, logA, Gpsi;

    if( IsDoubleEmpty( dt.Gdis ))
        dt.Gdis = 0.666666 * dt.Asig * dt.Asur * dt.Amw;

    logX = log( dt.Cx );
    dt.lnMx = logX - log( dt.Wx );
    dt.Gsas = 1. - dt.Wx;  /* Для Wx умалч. = 1 */
    dt.Gcas = 2. - dt.Wx - 1./dt.Wx;

    if( IsDoubleEmpty( dt.Agmx ))
        dt.Agmx = dt.Agx * 6.02e5;
    if( IsDoubleEmpty( dt.Nx ))
        dt.Nx = dt.Asur * dt.Agmx * dt.Amw;
    if( IsDoubleEmpty( dt.Gsd ))
        dt.Gsd = -log( dt.Tetp * dt.Nx / (1.-dt.Nx) );

    /* вычисление логарифма активности */
    logA = dt.Mu - dt.G0 - dt.dGex - dt.Gdis;

    switch( DCC )
    {
    case DC_SUR_SITE:
    case DC_SUR_COMPLEX:
    case DC_SUR_IPAIR:    /* Фазы с сорбцией - сорбаты */
        Gpsi = logA - dt.lnMx - dt.Gsas - dt.Gsm - dt.Gsd - dt.Gex;
        break;
        /*
            case DC_SUR_DL_ION:
            case DC_SUR_EX_ION:  * Обменные ионы *
                Gpsi = logA - dt.lnMx - dt.Gsas - dt.Gsm - dt.Gsd - dt.Gex;
                                 break;
            case DC_PEL_SITE:
            case DC_PEL_COMPLEX: * Комплексы на поли- и олигоэлектролите *
                Gpsi = logA - dt.lnMx - dt.Gsas - dt.Gsm - dt.Gsd - dt.Gex;
                                 break;
        */
    default:
        if( isdigit( DCC ))
        {  /* Это пов. комплекс типа p */
            Gpsi = logA - dt.lnMx - dt.Gsas - dt.Gsm - dt.Gsd - dt.Gex;
            break;
        } /* иначе неверный код */
        Error( GetName(), "Error DCC code.");
    }
    /* Окончательный пересчет */
    dt.Gpsi = Gpsi;
    dt.Psi = Gpsi / dt.Ze / dt.F * dt.RT;

    return Gpsi;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* Проверка кода единиц и вычисление приведенной концентрации Xincr
*  по введенному количеству Xe для каждого из типов ЗК (DCC).
*  Возвращает значение Xincr
*/
double TDuterm::DU_Reduce_Conc( char DCC, char UNITP, double Xe, double DCmw,
                                double /*Vm*/, double R1, double Msys, double Mwat, double Vaq, double Maq,
                                double /*Vsys*/ )
{
    double Xincr = 0., AQF=1.;
    TProfil* PRof = (TProfil*)(&aMod[RT_PARAM]);

    if( PRof->pmp->LO && PRof->pmp->XF[0] > 1e-9 )
        AQF = PRof->pmp->XF[0];
    /* количества */
    switch( UNITP )
    {
    case QUAN_MKMOL: /*'Y'*/
        Xincr = Xe / 1e6;
        goto FINISH;
    case QUAN_MMOL:  /*'h'*/
        Xincr = Xe / 1e3;
        goto FINISH;
    case QUAN_MOL:   /*'M'*/
        Xincr = Xe;
        goto FINISH;
    }
    if( DCmw > 1e-12 )
        switch( UNITP )
        {
        case QUAN_MGRAM: /*'y'*/
            Xincr = Xe / DCmw / 1e3;
            goto FINISH;
        case QUAN_GRAM:  /*'g'*/
            Xincr = Xe / DCmw;
            goto FINISH;
        case QUAN_KILO:  /*'G'*/
            Xincr = Xe * 1e3 / DCmw;
            goto FINISH;
        }
    /* концентрации */
    if( fabs( R1 ) > 1e-12 )
        switch( UNITP )   /* Мольн.доли от сумм. к-ва молей в системе */
        {
        case CON_MOLFR:  /*'n'*/
            Xincr = Xe * R1;
            goto FINISH;
        case CON_MOLPROC:/*'N'*/
            Xincr = Xe / 100.*R1;
            goto FINISH;
        case CON_pMOLFR: /*'f'*/
            if( Xe > -1. && Xe < 15 )
                Xincr = pow(10, -Xe ) *R1;
            goto FINISH;
        }
    if( fabs( Msys ) > 1e-12 && DCmw > 1e-12 )
        switch( UNITP ) /* Масс.доли - от сумм. массы системы */
        {
        case CON_WTFR:   /*'w'*/
            Xincr = Xe * Msys * 1e3 / DCmw;
            goto FINISH;
        case CON_WTPROC: /*'%'*/
            Xincr = Xe * Msys *10. / DCmw;
            goto FINISH;
        case CON_PPM:    /*'P'*/
            Xincr = Xe * Msys / 1e3 / DCmw;
            goto FINISH;
        }
    if( fabs( Mwat ) > 1e-12 )
        switch( UNITP ) /*Моляльности - отнесены к массе воды-растворителя */
        {
        case CON_MOLAL:  /*'m'*/
            Xincr = Xe*Mwat / AQF;
            goto FINISH;
        case CON_MMOLAL: /*'i'*/
            Xincr = Xe / 1e3 * Mwat / AQF;
            goto FINISH;
        case CON_pMOLAL: /*'p'*/
            if( Xe > -1. && Xe < 15 )
                Xincr = pow( 10, -Xe ) * Mwat / AQF;
            goto FINISH;
        }
    if( fabs( Vaq ) > 1e-12 )
        switch( UNITP )  /*Молярности - отнесены к обьему водного раствора */
        {
        case CON_MOLAR:  /*'L'*/
            Xincr = Xe * Vaq / AQF;
            goto FINISH;
        case CON_MMOLAR: /*'j'*/
            Xincr = Xe * Vaq / 1e3 / AQF;
            goto FINISH;
        case CON_pMOLAR: /*'q'*/
            if( Xe > -1. && Xe < 15 )
                Xincr = pow( 10, -Xe ) * Vaq / AQF;
            goto FINISH;
            /* г/л, мг/л, мкг/л */
        case CON_AQGPL: /*'d'*/
            Xincr = Xe * Vaq / DCmw / AQF;
            goto FINISH;
        case CON_AQMGPL: /*'e'*/
            Xincr = Xe * Vaq / DCmw / 1e3 / AQF;
            goto FINISH;
        case CON_AQMKGPL: /*'b'*/
            Xincr = Xe * Vaq / DCmw / 1e6 / AQF;
            goto FINISH;
        }
    if( fabs( Maq ) > 1e-12 && DCmw > 1e-12 )
        switch( UNITP )     /*Концентрации - отнесены к массе водн р-ра */
        {
        case CON_AQWFR:  /*'C'*/
            Xincr = Xe * Maq * 1e3 / DCmw / AQF;
            goto FINISH;
        case CON_AQWPROC:/*'c'*/
            Xincr = Xe * 10. * Maq / DCmw / AQF;
            goto FINISH;
        case CON_AQPPM:  /*'a'*/
            Xincr = Xe * Maq / 1e3 / DCmw / AQF;
            goto FINISH;
        }
    /* Неверный код единиц концентрации или ошибка входных переменных */
    Error( GetName(), "Error in units of consentration or other data.");

FINISH: /* Пересчет в нормализованную концентрацию */
    switch( DCC )
    { /* Водный электролит */
    case DC_AQ_PROTON:
    case DC_AQ_ELECTRON:
    case DC_AQ_SPECIES:
        Xincr /= R1;
        break;
    case DC_AQ_SOLVCOM:
    case DC_AQ_SOLVENT:
        Xincr /= R1;
        break;

    case DC_GAS_COMP: /* Газы, кроме H2O и CO2 */
    case DC_GAS_H2O:
    case DC_GAS_CO2:
    case DC_GAS_H2:
    case DC_GAS_N2:
        Xincr /= R1;
        break;
        /* Растворы неэлектролитов */
    case DC_SCP_CONDEN:
    case DC_SOL_IDEAL:
    case DC_SOL_MINOR:
    case DC_SOL_MAJOR:
    case DC_SUR_MINAL:
    case DC_SUR_CARRIER:
    case DC_PEL_CARRIER:
        Xincr /= R1;
        break;
        /* Фазы с сорбцией */
    case DC_SUR_SITE:
    case DC_SUR_COMPLEX:
    case DC_SUR_IPAIR:
        /*    case DC_SUR_DL_ION: case DC_SUR_EX_ION:  case DC_PEL_SITE:
            case DC_PEL_COMPLEX: */ 
        Xincr /= R1;
        break;
    default:
        if( isdigit( DCC ))
        {
            Xincr /= R1;
            break;
        }
        else /* иначе неверный код */
            return 7777777.;
    }

    return Xincr;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* Создание нового варианта TPARAM со значениями gT, рассчитанными
*  по DUTERM. Возвращает 0 или -1 в случае ошибки
*/
void TDuterm::ut_tp_insert()
{
    TProfil* PRof = (TProfil*)(&aMod[RT_PARAM]);
    short jm, i;
    for( i=0; i<utp->Ld; i++ )
    {
        jm = utp->llf[i];
        PRof->tpp->G[jm] = utp->gT0[i];
    }
}
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* Расчет неизвестных значений энергии Гиббса зависимых
*  компонентов по их стехиометрии и химическим потенциалам
*  независимых компонентов в равновесном состоянии системы
*/
void TDuterm::duterm_calc()
{
    int jj, i, j, k, /*km,*/ im, ip, DL;
    double RTJ, /*RTK,*/ /*T,*/ P, Cx, Xw, Gmu, DCmw, Res, aji;
    TProfil* PRof = (TProfil*)(&aMod[RT_PARAM]);
    TIArray<TFormula> aFo;
    gstring form;

    if( PRof->pmp->LO && PRof->pmp->XF[0] > 1e-9 ) /* Мольная доля воды */
        Xw = PRof->pmp->X[PRof->pmp->LO]/PRof->pmp->XF[0];
    else Xw = 1.0;
    dt.Wx = Xw;
    utp->Mwat = PRof->pmp->X[PRof->pmp->LO] * 18.0573/1000.;

    utp->A = (float *)aObj[ o_uta].Alloc( utp->L, PRof->mup->N, F_ );

    DL = utp->L; /*T = PRof->pmp->Tc;*/
    P = PRof->pmp->Pc;
    RTJ = PRof->pmp->RT;  //RTK = RTJ / cal_to_J;

    /* Загрузка формул  */
    for( j=0; j<DL; j++ )
    {
        aFo.Add( new TFormula() );
        form = aFo[j].form_extr( j, DL, utp->DDF );
        aFo[j].SetFormula( form.c_str() ); // and ce_fscan
    }

    for( i=0; i<DL; i++ )
        aFo[i].Stm_line( PRof->mup->N, utp->A+i*PRof->mup->N,
                         (char *)PRof->mup->SB, PRof->mup->Val );
    aFo.Clear();
    /* Расчет нормализованных концентраций -доделать!!!!!!!!!!!!!! */

    if( utp->PvYmS != S_OFF )
    { /* Предобработка невязок */
        ;
    }

    if( utp->PvEqn != S_OFF )
    { /* Трансляция и пересчет IIPN уравнений */
        ut_text_analyze();
        rpn.CalcEquat();
    }
    /* Главный цикл пересчета DUTERM */
    for( j = 0; j < DL; j++ )
    {
        /* Находим индекс фазы */
        jj = 0;
        for( k=0; k<PRof->mup->Fi; k++ )
        {
            if( j >= jj && j < jj+PRof->mup->Ll[k] )
            { /*km = k;*/ goto PHI_GET;
            }
            jj += PRof->mup->Ll[k];
        }
        /* Ошибка - индекс фазы не найден! */
        ErrorIf( j < utp->Ld, GetName(), "Error phase index." );
        //km = -1;

        /* Расчет величин mu(T,P) */
PHI_GET:
        Gmu = 0.;
        DCmw = 0.;
        for( im = 0; im < PRof->mup->N; im++ )
        { /* Находим индекс НК по MULTI */
            aji = utp->A[j*PRof->mup->N+im];
            if( !aji )
                continue;
            for( i=0; i<PRof->pmp->N; i++ )
                if( im == PRof->pmp->mui[i] )
                {
                    ip = i;
                    goto ICI_GET;
                }
            /* Ошибка - нет того НК !! Formula of add DC*/
            Error( GetName(), "Error IC name in formula of add DC." );
ICI_GET:
            Gmu += aji * PRof->pmp->U[ip];
            DCmw += aji * PRof->mup->BC[im];
        }
        /*  utp->MU[j] = Gmu;  */
        if( utp->R1 < 1e-9 )
            utp->R1 = 1.;
        /* Расчет нормализованных концентраций */
        Cx = DU_Reduce_Conc( utp->DCC[j], utp->UnWx[j], utp->CX[j], DCmw,
                             1.0, utp->R1, utp->Msys, utp->Mwat, utp->Vaq, utp->Maq,
                             utp->Vsys );
        /*
         pm[q].ln1N[j] = sy[q].Nfsp[pm[q].muj[j]];
          G += pm[q].ln5551 - log( pm[q].ln1Na[k] )
            -log(pm[q].ln1N[j])+ log( 1.-pm[q].ln1Na[k] );
        */
        /* Загрузка параметров DUTERM - уравнений */
        memset( &dt, 0, sizeof(DUTERMSET) );
        utp->MU[j] = Gmu*RTJ;
        dt.RT =    RTJ; /* T*R_CONSTANT */
        dt.F  =    F_CONSTANT;  /* Faraday constant */
        dt.Gsm =   4.01653;
        dt.lnPg =  log( P );
        dt.G0 =    utp->gT0[j]/RTJ; /* единицы измер. доделать ! utp->UnE */
        dt.Nu =    Gmu;
        dt.Cx =    Cx;   /* Мольн. доля солюта (сорбата) в фазе */
        dt.lnMx =  utp->WX[j]; /* molality if aq solution */
        dt.Wx =    Xw;   /* Мольн.доля воды в водном р-ре */
        dt.Gex =   utp->gEx[j]/RTJ;
        if( utp->gExK )
            dt.dGex =  utp->gExK[j]/RTJ;
        if( fabs( utp->Gam[j] ) > 1e-30 )
            dt.lnGam = log( utp->Gam[j] );
        dt.Gamma = utp->Gam[j];
        if( utp->Area )
            dt.Asur =  utp->Area[j];
        if( utp->Sigm )
            dt.Asig =  utp->Sigm[j];
        if( utp->Psi )
            dt.Psi =   utp->Psi[j];
        dt.Ze =    utp->A[j*PRof->mup->N+PRof->mup->N-1];
        if( utp->Nsig )
            dt.Tetp =  utp->Nsig[j];
        if( utp->Nsph )
            dt.Agx  =  utp->Nsph[j];
        dt.Amw  =  utp->Res;  /* Это мольная масса носителя для сорбции */
        dt.Agmx =  dt.Agx * 6.0221367e5;
        dt.Nx =    dt.Asur * dt.Agmx * dt.Amw;
        dt.Gdis =  DOUBLE_EMPTY;
        dt.Gid  =  DOUBLE_EMPTY;
        dt.lnAx =  DOUBLE_EMPTY;
        dt.Gsd  =  DOUBLE_EMPTY;
        dt.Gpsi =  DOUBLE_EMPTY;
        dt.Gsd  =  DOUBLE_EMPTY;
        /*
          if( IsDoubleEmpty( dt.Agmx ))
            dt.Agmx = dt.Agx * 6.0221367e5;
          if( IsDoubleEmpty( dt.Nx ))
            dt.Nx = dt.Asur * dt.Agmx * dt.Amw;
          if( IsDoubleEmpty( dt.Gsd ))
            dt.Gsd = -log( dt.Tetp * dt.Nx / (1.-dt.Nx) );
        */
        /* Анализ кода метода расчета */
        switch( utp->UtRes[j] )
        {
        case DUT_MU_NU: /* Расчет разности прямого и двойств. хим. пот. */
            Res = dut_Mu_Nu_calc( utp->DCC[j] );
            utp->gTP[j] = Res*RTJ;
            break;
        case DUT_EUSER: /*'F' Расчет любых парам. по пользов. формуле IIPN
                                       gamma по Cx можно считать в IIPN всегда */
            rpn.CalcEquat();
            break;
        case DUT_G0TP:  /*'G' Расчет g0(T,P) по u, Cx, gamma, dGex */
            Res = dut_G0TP_calc( utp->DCC[j] );
            utp->gTP[j] = Res*RTJ;
            utp->gEx[j] = dt.Gex*RTJ;
            break;
        case DUT_GETPX: /*'Z' Расчет RT*ln(gamma) по g0(T,P), u, Cx, dGex */
            Res = dut_Gex_calc( utp->DCC[j] );
            utp->gEx[j] = Res*RTJ;
            utp->Gam[j] = dt.Gamma;
            utp->gTP[j] = utp->MU[j] - utp->gT0[j] - Res*RTJ;
            break;
        case DUT_GEKIN: /*'K' Расчет dGex(metast) по g0, u, Cx(x), gamma */
            Res = dut_Gexk_calc( utp->DCC[j] );
            utp->gExK[j] = Res*RTJ;
            break;
        case DUT_CONDC: /*'C' Расчет Wx по g0(T,P), dGex, u, gamma       */
            Res = dut_Cx_calc( utp->DCC[j] );
            utp->WX[j] = Res;
            break;
        case DUT_DISAREA: /*'A' Расчет Asur по g0,u,Cx,Sigma,gamma,Nsur,Psi*/
            Res = dut_Asur_calc( utp->DCC[j] );
            utp->Area[j] = Res;
            break;
        case DUT_DISSIG: /*'S' Расчет Sigma по g0,u,Cx,Asur,gamma,Nsur,Psi*/
            Res = dut_Asig_calc( utp->DCC[j] );
            utp->Sigm[j] = Res;
            break;
        case DUT_SURNSG: /*'N' Расчет Nsur по g0,u,Cx,Sigma,Asur,gamma,Psi*/
            Res = dut_Nsur_calc( utp->DCC[j] );
            utp->Nsig[j] = Res;
            break;
        case DUT_SURPSI: /*'P' Расчет Psi по g0,u,Cx,Sigma,Asur,gamma,Nsur*/
            Res = dut_Psi_calc( utp->DCC[j] );
            utp->Psi[j] = Res;
            break;
        case DUT_SURAC: /*'Y' Расчет Sur gamma по g0,u,Cx,Sigma,Asur,Nsur,Psi*/
            Res = dut_sGam_calc( utp->DCC[j] );
            utp->Gam[j] = Res;
            break;
        case DUT_DISG: /*'D' Расчет Wx как для CONDC и ^2 невязок Wx-Cx */
            Res = dut_Gdis_calc( utp->DCC[j] );
            utp->Sigm[j] = Res;
            utp->Area[j] = 1.;
            break;
        case DUT_DEVDC: /*'C' Расчет Wx по g0(T,P), dGex, u, gamma       */
            Res = dut_Cx_calc( utp->DCC[j] );
            utp->WX[j] = Res;
            /* Расчет невязок */
            break;
        default:    /* Неправильный код метода расчета */
            break;
        }
    }   /* loop j */

    /* Расчет невязок по НК и cумм невязок *

        case DUT_DEVIC: *'U' Расчет ^2 невязок mE-mC для подмн. НК *
                        break;
        case DUT_DEVAC: *'E' Расчет степени комплексации для НК в aq *
                        break;
    */
    utp->A = (float *)aObj[ o_uta].Free();

    /* Запись скорректированных gT в набор данных TPARAM */
    if( utp->Ld )
        if( vfQuestion(window(), GetName(),
                       "Insert estimated gT(T,P) values for DC into next MTPARM?" ))
            ut_tp_insert();
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* Расчет "прямых" химических потенциалов ЗК F[] через их количества
* молей Y[], количества молей фаз YF[] и приведенные нормированные
* функции gT (в векторе pm[q].G[]) по всей рабочей мультисистеме
*/
double TDuterm::PrimeChemPot( /* Оптимизированная функция */
    double G,      /* Парц. мольн. норм. величина gT0+gEx для ЗК */
    double logY,   /* величина ln x - к-во молей ЗК */
    double logYF,  /* величина ln Xa - к-во молей фазы */
    double asTail, /* Фактор коррекции асимметрии (0 для симм. фаз) */
    double logYw,  /* величина ln Xv - к-во молей носителя асимм. фазы */
    char DCCW      /* Коды рабочего классификатора ЗК (SPP_IPMD.H) */
)
{
    double F;  /* Величина прямого (электро)химического потенциала */
    switch( DCCW ) /* Выбор нужных выражений v_j */
    {
    case DC_SINGLE:
        F = G;
        break;
    case DC_ASYM_SPECIES:
        F = G + logY - logYw + asTail;
        break;
    case DC_ASYM_CARRIER:
        F = G + logY - logYF + asTail + 1.0 -
            1.0/(1.0 - asTail);
        break;
    case DC_SYMMETRIC:
        F = G + logY - logYF;
        break;
    default:
        F = 7777777.;
    }
    return F;
}

// -------------------- End of m_dutx.cpp -------------------------

