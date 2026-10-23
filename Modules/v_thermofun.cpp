#include "v_thermofun.h"
#include "m_icomp.h"
#include "m_dcomp.h"
#include "m_reacdc.h"
#include "Submods/ms_rmults.h"
#include "Submods/ms_system.h"
#include <map>
#include <set>
#include <cmath>
#include <algorithm>

static QJsonObject thf_convert_dcomp_aggregate_state(char type);
static QJsonObject thf_convert_dcomp_TPMethods0(char type);
static QJsonObject thf_convert_dcomp_TPMethods1(char type);
static QJsonObject thf_convert_dcomp_TPMethods2(char type);
static QJsonObject thf_convert_dcomp_class(char type);
static QJsonObject thf_convert_reacdc_TPMethods0(char type);
static QJsonObject thf_convert_reacdc_TPMethods1(char type);
static QJsonObject thf_convert_reacdc_TPMethods2(char type);

std::string thf_string(const char *value, size_t size)
{
    auto val = std::string( value, 0, size );
    strip(val);
    return val;
}

QJsonObject thf_object(const char *key, const char *value)
{
    return thf_object( key, std::string(value) );
}

QJsonObject thf_object(const char *key, const std::string &value)
{
    QJsonObject converted;
    auto val = value;
    strip(val);
    converted[key] = val.c_str();
    return converted;
}

QJsonObject thf_convert_icomp_class( char type )
{
    QJsonObject converted;
    switch(type)
    {
    default:
    case 'h':
    case 'o':
    case 'e':   converted["0"] ="ELEMENT";
        break;
    case 'z':   converted["4"] ="CHARGE";
        break;
    case 'a':   converted["2"] ="LIGAND";
        break;
    case 'v':   converted["5"] ="OTHER_EC";
        break;
    }
    return converted;
}

QJsonObject thf_convert_dcomp_aggregate_state( char type )
{
    QJsonObject converted;
    switch(type)
    {
    case 'a':   converted["4"] ="AS_AQUEOUS";
        break;
    case 'f':
    case 'g':   converted["0"] ="AS_GAS";
        break;
    case 'c':
    case 's':   converted["3"] ="AS_CRYSTAL";
        break;
    case 'l':   converted["1"] ="AS_LIQUID";
        break;
    case 'p':   converted["8"] ="AS_PLASMA";
        break;
    case 'z':   converted["5"] ="AS_SURFACE";   // sorbed species
        break;
    case 'i':   converted["6"] ="AS_IONEX";     // ion exchange species
        break;
    default:    converted["101"] ="AS_OTHER";   // not a state of ThermoFun
        break;
    }
    return converted;
}


QJsonObject thf_convert_dcomp_TPMethods0( char type )
{
    QJsonObject converted;
    switch(type)
    {
    case 'C':   converted["0"] ="cp_ft_equation";
        break;
    case 'H':   converted["2"] ="solute_hkf88_gems";
        break;
//    case 'H':   converted["3"] ="solute_hkf88_reaktoro";
//        break;
//    case 'J':   converted["25"] ="water_diel_jnort91_reaktoro";
//        break;
    case 'J':   converted["26"] ="water_diel_jnort91_gems";
        break;
    }
    return converted;
}

QJsonObject thf_convert_dcomp_TPMethods1( char type )
{
    QJsonObject converted;
    switch(type)
    {
    case 'H':   converted["5"] ="landau_holland_powell98";
        break;
    case 'B':   converted["6"] ="landau_berman88";
        break;
    case 'F':   converted["1"] ="cp_ft_equation_saxena86";
        break;
//    case 'W':   converted["31"] ="water_eos_hgk84_reaktoro";
//        break;
    case 'W':   converted["29"] ="water_eos_hgk84_lvs83_gems";
        break;
//    case 'V':   converted["32"] ="water_eos_iapws95_reaktoro";
//        break;
    case 'V':   converted["30"] ="water_eos_iapws95_gems";
        break;
    case 'S':   converted["43"] ="standard_entropy_cp_integration";
        break;
    }
    return converted;
}

QJsonObject thf_convert_dcomp_TPMethods2( char type )
{
    QJsonObject converted;
    switch(type)
    {
    case 'N':   converted["40"] ="mv_pvnrt";
        break;
    case '1':
    case 'C':   converted["34"] ="mv_constant";
        break;
    case 'K':   converted["35"] ="mv_equation_dorogokupets88";
        break;
    case 'V':   converted["36"] ="mv_equation_berman88";
        break;
    case 'B':   converted["37"] ="mv_eos_birch_murnaghan_gott97";
        break;
    case 'E':   converted["38"] ="mv_eos_murnaghan_hp98";
        break;
    case 'H':   converted["39"] ="mv_eos_tait_hp11";
        break;
    case 'G':   converted["8"] ="fug_critical_param";
        break;
    case 'R':   converted["9"] ="fluid_prsv";
        break;
    case 'S':   converted["10"] ="fluid_churakov_gottschalk";
        break;
    case 'T':   converted["11"] ="fluid_soave_redlich_kwong";
        break;
    case '6':   converted["12"] ="fluid_sterner_pitzer";
        break;
    case '7':   converted["13"] ="fluid_peng_robinson78";
        break;
    case '8':   converted["14"] ="fluid_comp_redlich_kwong_hp91";
        break;
    case 'A':   converted["4"] ="solute_aknifiev_diamond03";
        break;
    }
    return converted;
}


QJsonObject thf_convert_dcomp_class( char type )
{
    QJsonObject converted;
    switch(type)
    {
    case 'd':
    case 'r':
    case 'n':
    case 'i':
    case 'f':
    case '0':
    case '2':
    case '4':
    case '6':
    case '8':
    case '1':
    case '3':
    case '5':
    case '7':
    case '9':
    case 'A':
    case 'B':
    case 'H':
    case 'N':   converted["101"] ="AS_OTHER";
        break;
    case 'T':
    case 'E':
    case 'S':   converted["2"] ="SC_AQSOLUTE";
        break;
    case 'K':
    case 'X':
    case 'Y':
    case 'Z':   converted["4"] ="SC_SURFSPECIES";
        break;
    case 'W':
    case 'L':   converted["3"] ="SC_AQSOLVENT";
        break;
    case 'G':
    case 'V':
    case 'C':   converted["1"] ="SC_GASFLUID";
        break;
    case 'I':
    case 'J':
    case 'M':
    case 'F':
    case 'D':
    case 'Q':
    case 'P':
    case 'R':
    case 'O':   converted["0"] ="SC_COMPONENT";
        break;
    }
    return converted;
}

QJsonObject thf_convert_reacdc_TPMethods0( char type )
{
    QJsonObject converted;
    switch(type)
    {
    case 'C':   converted["11"] ="dr_heat_capacity_ft";
        break;
    case 'I':   converted["2"] ="iso_compounds_grichuk88";
        break;
    case 'X':   converted["1"] ="adsor_ion_exchange";
        break;
    case 'K':   converted["0"] ="logk_fpt_function";
        break;
    }
    return converted;
}

QJsonObject thf_convert_reacdc_TPMethods1( char type )
{
    QJsonObject converted;
    switch(type)
    {
    case 'L':   converted["3"] ="logk_nordstrom_munoz88";
        break;
    case '0':   converted["4"] ="logk_1_term_extrap0";
        break;
    case '1':   converted["5"] ="logk_1_term_extrap1";
        break;
    case '2':   converted["6"] ="logk_2_term_extrap";
        break;
    case '3':   converted["7"] ="logk_3_term_extrap";
        break;
    case 'Z':   converted["8"] ="logk_lagrange_interp";
        break;
    case 'R':   converted["9"] ="logk_marshall_frank78";
        break;
    case 'Y':   converted["10"] ="solute_eos_ryzhenko_gems";
        break;
    }
    return converted;
}

QJsonObject thf_convert_reacdc_TPMethods2( char type )
{
    QJsonObject converted;
    switch(type)
    {
    case 'C':   converted["13"] ="dr_volume_constant";
        break;
    case 'K':   converted["12"] ="dr_volume_fpt";
        break;
    }
    return converted;
}


static std::string thf_rtrim(std::string str)
{
    while( !str.empty() && (str.back()==' ' || str.back()==':' || str.back()=='\0') )
        str.pop_back();
    return str;
}

/// Records written to the file by the last export: "DC" or "RC", the key and the symbol in the file, separated by tabs
/// (the checks of the export read it)
std::vector<std::string> thf_exported_keys;

/// The key of a record as a text without the blanks at the end
static std::string thf_key_symbol( const std::string& raw )
{
    std::string r = raw;
    if( r.size() < size_t(DC_RKLEN) )
        r.resize( DC_RKLEN, ' ' );
    return thf_string( r.c_str()+MAXSYMB+MAXDRGROUP, MAXDCNAME );
}

/// The part of the key after the symbol (the codes of the record)
static std::string thf_key_variant( const std::string& raw )
{
    const size_t pos = MAXSYMB+MAXDRGROUP+MAXDCNAME;
    return raw.size() > pos ? thf_rtrim( raw.substr(pos) ) : std::string();
}

/// The records that the project, or the system that is loaded, uses and their symbols.
/// In a system the records with the switch '+' are the records of the system.
struct ThfProjectRecords
{
    std::set<std::string> dcomp_keys, reacdc_keys, symbols;
    bool system = false;
    bool known() const { return !symbols.empty(); }

    /// The key of the project may have '_' (any code) where the key of the record has a code, or the other way round
    static bool same_key( const std::string& a, const std::string& b )
    {
        for( size_t i=0; i<std::max(a.size(), b.size()); i++ )
        {
            const char ca = i<a.size() ? a[i] : ' ', cb = i<b.size() ? b[i] : ' ';
            if( ca != cb && ca != '_' && cb != '_' )
                return false;
        }
        return true;
    }
    /// The record keys of the database that are the records of the project: the same key, or else the first
    /// key that differs only where one of them has '_'
    static std::set<std::string> resolve( const std::set<std::string>& project_keys, const std::set<std::string>& db )
    {
        std::set<std::string> found;
        for( const auto& pk: project_keys )
        {
            if( db.count(pk) )
            {
                found.insert(pk);
                continue;
            }
            for( const auto& k: db )
                if( same_key( pk, k ) )
                {
                    found.insert(k);
                    break;
                }
        }
        return found;
    }
};

static ThfProjectRecords thf_project_records()
{
    ThfProjectRecords used;
    RMULTS* mup = TRMults::sm->GetMU();
    if( !mup || !mup->SM || !mup->DCS )
        return used;
    // a system is loaded: only its records with the switch '+' (or '*', which becomes '+') are exported
    const SYSTEM* syp = TSyst::sm->GetSY();
    used.system = syp && syp->Dcl && syp->L > 0;
    for( long j=0; j<mup->L; j++ )
    {
        if( used.system && syp->Dcl[j] == S_OFF )
            continue;
        std::string key = thf_rtrim( std::string( mup->SM[j], DC_RKLEN ) );
        if( mup->DCS[j] == 'd' )
            used.dcomp_keys.insert(key);
        else if( mup->DCS[j] == 'r' )
            used.reacdc_keys.insert(key);
        else
            continue;
        used.symbols.insert( thf_string( mup->SM[j]+MAXSYMB+MAXDRGROUP, MAXDCNAME ) );
        gui_logger->debug("ThermoFun export: project record '{}' ({})", key, mup->DCS[j]);
    }
    return used;
}

/// What is written to the file: the records of the project or system, the other records of the database whose symbol is
/// free, and the records that a reaction needs as reactant and that are not written under their symbol. These
/// records are written with another symbol: symbol#codes.
struct ThfPlan
{
    std::vector<std::string> dc_order, rc_order;                // keys of the database (without blanks at the end)
    std::map<std::string, std::string> dc_symbol, rc_symbol;    // key -> symbol in the file
    std::map<std::string, std::string> dc_raw, rc_raw;          // key -> key as the database gives it
    std::set<std::string> dc_db, rc_db;
    std::map<std::string, std::string> aliased;                 // key -> symbol, for the records written with another symbol

    /// The record of the database for the key of a reactant: the same key, or the key that differs where one has '_'
    bool find( const std::string& raw, bool& reacdc, std::string& key ) const
    {
        const std::string k = thf_rtrim(raw);
        if( dc_db.count(k) ) { reacdc = false; key = k; return true; }
        if( rc_db.count(k) ) { reacdc = true; key = k; return true; }
        for( const auto& d: dc_db )
            if( ThfProjectRecords::same_key( k, d ) ) { reacdc = false; key = d; return true; }
        for( const auto& r: rc_db )
            if( ThfProjectRecords::same_key( k, r ) ) { reacdc = true; key = r; return true; }
        return false;
    }
    /// The symbol in the file of the record that a reactant key names
    std::string reactant_symbol( const std::string& raw ) const
    {
        bool reacdc; std::string key;
        if( find( raw, reacdc, key ) )
        {
            const auto& m = reacdc ? rc_symbol : dc_symbol;
            const auto it = m.find(key);
            if( it != m.end() )
                return it->second;
        }
        return thf_key_symbol(raw);
    }
};
static ThfPlan thf_plan;

/// Reads the keys of the records and of the reactions and makes the plan of the export
static void thf_make_plan( const TCStringArray& dcKeys, const TCStringArray& rcKeys )
{
    thf_plan = ThfPlan();
    for( const auto& k: dcKeys ) { thf_plan.dc_db.insert( thf_rtrim(k) ); thf_plan.dc_raw[thf_rtrim(k)] = k; }
    for( const auto& k: rcKeys ) { thf_plan.rc_db.insert( thf_rtrim(k) ); thf_plan.rc_raw[thf_rtrim(k)] = k; }

    // the symbol of a reaction-defined species is the last reactant of its reaction; its reactants
    std::map<std::string, std::string> rc_product;
    std::map<std::string, std::vector<std::string>> rc_reactants;
    for( const auto& k: rcKeys )
    {
        TReacDC::pm->RecInput( k.c_str() );
        const auto* rc = TReacDC::pm->rcp;
        std::vector<std::string> reactants;
        for( int j=0; j<rc->nDC; j++ )
            reactants.push_back( std::string( rc->DCk[j], DC_RKLEN ) );
        rc_reactants[thf_rtrim(k)] = reactants;
        rc_product[thf_rtrim(k)] = reactants.empty() ? thf_key_symbol(k) : thf_key_symbol(reactants.back());
        if( !reactants.empty() && rc_product[thf_rtrim(k)] != thf_key_symbol(k) )
            gui_logger->warn("ThermoFun export: ReacDC {} is written with the symbol {} of its last reactant", k, rc_product[thf_rtrim(k)]);
    }

    const auto used = thf_project_records();
    const auto usedDc = ThfProjectRecords::resolve( used.dcomp_keys, thf_plan.dc_db );
    const auto usedRc = ThfProjectRecords::resolve( used.reacdc_keys, thf_plan.rc_db );

    std::set<std::string> taken;                 // symbols in the file
    std::set<std::string> in_dc, in_rc;
    for( const auto& k: usedDc ) { in_dc.insert(k); taken.insert( thf_key_symbol(thf_plan.dc_raw[k]) ); }
    for( const auto& k: usedRc ) { in_rc.insert(k); taken.insert( rc_product[k] ); }
    if( !used.system )
    {   // the other records of the database, if their symbol is free
        for( const auto& k: dcKeys )
            if( !in_dc.count(thf_rtrim(k)) && taken.insert( thf_key_symbol(k) ).second )
                in_dc.insert( thf_rtrim(k) );
        for( const auto& k: rcKeys )
            if( !in_rc.count(thf_rtrim(k)) && taken.insert( rc_product[thf_rtrim(k)] ).second )
                in_rc.insert( thf_rtrim(k) );
    }
    for( const auto& k: dcKeys ) if( in_dc.count(thf_rtrim(k)) ) thf_plan.dc_symbol[thf_rtrim(k)] = thf_key_symbol(k);
    for( const auto& k: rcKeys ) if( in_rc.count(thf_rtrim(k)) ) thf_plan.rc_symbol[thf_rtrim(k)] = rc_product[thf_rtrim(k)];

    // records that the reactions need and that are not in the file
    std::vector<std::string> queue;
    for( const auto& k: in_rc ) queue.push_back(k);
    while( !queue.empty() )
    {
        const std::string rk = queue.back();
        queue.pop_back();
        for( const auto& raw: rc_reactants[rk] )
        {
            bool reacdc; std::string key;
            if( !thf_plan.find( raw, reacdc, key ) )
            {
                gui_logger->warn("ThermoFun export: the reactant {} of the reaction {} is not a record of the database", thf_rtrim(raw), rk);
                continue;
            }
            auto& syms = reacdc ? thf_plan.rc_symbol : thf_plan.dc_symbol;
            if( syms.count(key) )
                continue;
            const std::string base = reacdc ? rc_product[key] : thf_key_symbol( thf_plan.dc_raw[key] );
            std::string alias = base + "#" + thf_key_variant( reacdc ? thf_plan.rc_raw[key] : thf_plan.dc_raw[key] );
            for( int n=2; taken.count(alias); n++ )
                alias = base + "#" + thf_key_variant( reacdc ? thf_plan.rc_raw[key] : thf_plan.dc_raw[key] ) + std::to_string(n);
            taken.insert(alias);
            syms[key] = alias;
            thf_plan.aliased[key] = alias;
            gui_logger->info("ThermoFun export: {} {} is a reactant of a reaction and is not the record of the project: written as {}",
                             reacdc ? "ReacDC" : "DComp", key, alias);
            if( reacdc )
                queue.push_back(key);
        }
    }
    for( const auto& k: dcKeys ) if( thf_plan.dc_symbol.count(thf_rtrim(k)) ) thf_plan.dc_order.push_back( thf_rtrim(k) );
    for( const auto& k: rcKeys ) if( thf_plan.rc_symbol.count(thf_rtrim(k)) ) thf_plan.rc_order.push_back( thf_rtrim(k) );
}

/// Comparison of the stored values of a reaction-defined species with the values that follow from its reaction
/// (the stored values of the reactants): G, H, S, Cp, V
static void thf_check_reaction( const std::string& key, const std::string& symbol )
{
    const auto* rc = TReacDC::pm->rcp;
    const int n = rc->nDC;
    if( n < 2 || rc->scDC[n-1] == 0.0 )
        return;
    double reac[5] = { rc->Gs[0], rc->Hs[0], rc->Ss[0], rc->Cps[0], rc->Vs[0] };
    double stored[5] = { rc->Gs[1], rc->Hs[1], rc->Ss[1], rc->Cps[1], rc->Vs[1] };
    double nu = rc->scDC[n-1];
    std::vector<double> coef;
    std::vector<std::string> raws;
    for( int j=0; j<n-1; j++ ) { coef.push_back(rc->scDC[j]); raws.push_back( std::string(rc->DCk[j], DC_RKLEN) ); }
    double sum[5] = { 0, 0, 0, 0, 0 };
    for( size_t j=0; j<raws.size(); j++ )
    {
        bool reacdc; std::string rk;
        if( !thf_plan.find( raws[j], reacdc, rk ) )
        {
            gui_logger->warn("ThermoFun export: ReacDC {} ({}) is not checked: its reactant {} is not a record of the database", key, symbol, thf_rtrim(raws[j]));
            return;
        }
        double v[5];
        if( reacdc )
        {
            TReacDC::pm->RecInput( thf_plan.rc_raw[rk].c_str() );
            const auto* r2 = TReacDC::pm->rcp;
            v[0] = r2->Gs[1]; v[1] = r2->Hs[1]; v[2] = r2->Ss[1]; v[3] = r2->Cps[1]; v[4] = r2->Vs[1];
        }
        else
        {
            TDComp::pm->RecInput( thf_plan.dc_raw[rk].c_str() );
            const auto* d2 = TDComp::pm->dcp;
            v[0] = d2->Gs[0]; v[1] = d2->Hs[0]; v[2] = d2->Ss[0]; v[3] = d2->Cps[0]; v[4] = d2->mVs[0];
        }
        for( int q=0; q<5; q++ )
        {
            if( IsDoubleEmpty(v[q]) || IsFloatEmpty(float(v[q])) ) sum[q] = std::nan("");
            else sum[q] += coef[j]*v[q];
        }
    }
    TReacDC::pm->RecInput( thf_plan.rc_raw[key].c_str() );   // the reaction again
    static const char* names[5] = { "G", "H", "S", "Cp", "V" };
    static const double tol[5] = { 10.0, 100.0, 1.0, 1.0, 0.01 };   // J/mol, J/mol, J/(mol K), J/(mol K), J/bar
    for( int q=0; q<5; q++ )
    {
        if( IsDoubleEmpty(reac[q]) || IsFloatEmpty(float(reac[q])) || IsDoubleEmpty(stored[q]) || IsFloatEmpty(float(stored[q])) || std::isnan(sum[q]) )
            continue;
        const double derived = (reac[q] - sum[q]) / nu;
        if( std::fabs(derived - stored[q]) > tol[q] )
            gui_logger->warn("ThermoFun export: ReacDC {} ({}): the stored {} is {}, the reaction gives {} (difference {})",
                             key, symbol, names[q], stored[q], derived, stored[q]-derived);
    }
}

void TDComp::thf_keys( TCStringArray& keys )
{
    TCIntArray anR;
    db->GetKeyList( "*:*:*:*:", keys, anR );
}

void TReacDC::thf_keys( TCStringArray& keys )
{
    TCIntArray anR;
    db->GetKeyList( "*:*:*:*:", keys, anR );
}

QJsonArray TIComp::all_to_thermofun()
{
    QJsonArray allArray;
    TCIntArray anR;
    TCStringArray aIC1;
    std::string fld_key;
    db->GetKeyList( "*:*:*:", aIC1, anR );
    QJsonArray datasources( {"GEMS3gui"} );

    for( size_t ii=0; ii<aIC1.size(); ii++ )
    {
       RecInput( aIC1[ii].c_str() );
       QJsonObject thermo_data;
       thermo_data["datasources"] = datasources;
       thermo_data["symbol"] = thf_string(db->FldKey(0), db->FldLen(0)).c_str();
       thermo_data["class_"] = thf_convert_icomp_class(*db->FldKey(1));
       thermo_data["atomic_mass"] = QJsonObject( {{"name","M0i"},
                                                  {"values",QJsonArray({icp->awt})}});
       thermo_data["entropy"] = QJsonObject( {{"name","S0i"},
                                              {"values",QJsonArray({icp->aSs})}});
       allArray.append(thermo_data);
    }
    return allArray;
}


QJsonArray TDComp::all_to_thermofun()
{
    QJsonArray allArray;
    TCStringArray aDC1, aRC1;
    QJsonObject temp_data;
    thf_keys( aDC1 );
    TReacDC::pm->thf_keys( aRC1 );

    thf_exported_keys.clear();
    thf_make_plan( aDC1, aRC1 );
    for( const auto& plan_key: thf_plan.dc_order )
    {
        RecInput( thf_plan.dc_raw[plan_key].c_str() );
        const std::string symbol = thf_plan.dc_symbol[plan_key];
        QJsonObject thermo_data;

        thermo_data["name"] = thf_string(dcp->name, MAXFORMULA).c_str();
        thermo_data["symbol"] = symbol.c_str();
        thermo_data["formula"] = thf_string(dcp->form, MAXFORMULA).c_str();
        thermo_data["formula_charge"] = dcp->Zz;
        thermo_data["mass_per_mole"] = QJsonObject({{"values", QJsonArray({dcp->mwt})}, {"units",QJsonArray({"g/mol"})}});
        thermo_data["aggregate_state"] = thf_convert_dcomp_aggregate_state(*db->FldKey(0));
        thermo_data["class_"] = thf_convert_dcomp_class(dcp->PdcC);
        thermo_data["Tst"] = dcp->TCst+273.15;
        thermo_data["Pst"] = dcp->Pst*1e05;
        thermo_data["sm_heat_capacity_p"] = QJsonObject( {{"units",QJsonArray({"J/(mol*K)"})},
                                                          {"values",QJsonArray({dcp->Cps[0]})},
                                                          {"errors",QJsonArray({thf_value(dcp->Cps[1])})}});
        thermo_data["sm_gibbs_energy"] = QJsonObject( {{"units",QJsonArray({"J/mol"})},
                                                       {"values",QJsonArray({dcp->Gs[0]})},
                                                       {"errors",QJsonArray({thf_value(dcp->Gs[1])})}});
        thermo_data["sm_enthalpy"] = QJsonObject( {{"units",QJsonArray({"J/mol"})},
                                                   {"values",QJsonArray({dcp->Hs[0]})},
                                                   {"errors",QJsonArray({thf_value(dcp->Hs[1])})}});
        thermo_data["sm_entropy_abs"] = QJsonObject( {{"units",QJsonArray({"J/(mol*K)"})},
                                                      {"values",QJsonArray({dcp->Ss[0]})},
                                                      {"errors",QJsonArray({thf_value(dcp->Ss[1])})}});
        thermo_data["sm_volume"] = QJsonObject( {{"units",QJsonArray({"J/bar"})},
                                                 {"values",QJsonArray({dcp->mVs[0]})},
                                                 {"errors",QJsonArray({thf_value(dcp->mVs[1])})}});
        if( !IsDoubleEmpty(dcp->Comp))
            thermo_data["m_compressibility"] = QJsonObject( {{"units",QJsonArray({"1e-05/K"})},
                                                             {"values",QJsonArray({dcp->Comp})}});
        if( !IsDoubleEmpty(dcp->Expa))
            thermo_data["m_expansivity"] = QJsonObject( {{"units",QJsonArray({"kbar"})},
                                                         {"values",QJsonArray({dcp->Expa})}});
        QJsonArray datasources;
        for(int isd=0; isd<dcp->Nsd; isd++)
            datasources.append( thf_string(dcp->sdref[isd],V_SD_RKLEN).c_str() );
        thermo_data["datasources"] = datasources;

        // Undefined
        // thermo_data["reaction"] = QJsonObject(); // reaction_symbol[0]  not in DComp ?
        // thermo_data["limitsTP"] = QJsonObject(); //   s.properties.limitsTP only into TPMethods ?

        QJsonArray arrTPMethods;
        auto method = thf_convert_dcomp_TPMethods0(dcp->pct[0]);
        // special case for water solvent
        if ( dcp->PdcC == 'W')
            method = thf_convert_dcomp_TPMethods0('J');
        auto method0 = method;
        if( !method.empty() )
        {
            if( method.contains("0")  && dcp->Cp && dcp->TCint ) // cp_ft_equation
            {
                for( int ii=0; ii<dcp->NeCp; ii++ )
                {
                    QJsonObject oTPMethods0;
                    oTPMethods0["method"] = method;

                    oTPMethods0["limitsTP"] = QJsonObject( {{"lowerT", dcp->TCint[ii]+273.15 },
                                                            {"upperT", dcp->TCint[ii+dcp->NeCp]+273.15 },
                                                            {"range", true} });
                    QJsonObject oCp = thf_object_column( "values", dcp->Cp, MAXCPCOEF, dcp->NeCp, ii );
                    oCp.insert("names", QJsonArray{"a0","a1","a2","a3","a4","a5","a6","a7","a8","a9","a10"} );
                    oCp.insert("units", QJsonArray{"J/(mol*K)","J/(mol*K^2)","(J*K)/mol","J/(mol*K^0.5)","J/(mol*K^3)",
                                                   "J/(mol*K^4)","J/(mol*K^5)","(J*K^2)/mol","J/mol","J/(mol*K^1.5)","J/(mol*K)"} );
                    oTPMethods0["m_heat_capacity_ft_coeffs"] = oCp;

                    if(dcp->FtP && ii<dcp->Nft)
                    {
                        QJsonObject oFtP= thf_object_column( "values", dcp->FtP, 5, dcp->Nft, ii );
                        oFtP.insert("names", QJsonArray{"Temperature", "dS", "dH", "dV", "dT/dP"} );
                        oFtP.insert("units", QJsonArray{"K", "J/(mol*K)", "J/mol", "J/bar","K/bar"} );
                        oTPMethods0["m_phase_trans_props"] = oFtP;
                    }
                    arrTPMethods.append(oTPMethods0);
                }
            }
            else
            {
                QJsonObject oTPMethods0;
                oTPMethods0["method"] = method;
                if( dcp->TCint ) // only first column
                    oTPMethods0["limitsTP"] =  QJsonObject( {{"lowerT", dcp->TCint[0]+273.15 },
                                                             {"upperT", dcp->TCint[dcp->NeCp]+273.15 },
                                                             {"range", true} });
                if( dcp->Cp ) // only first column
                {
                    QJsonObject oCp = thf_object_column( "values", dcp->Cp, MAXCPCOEF, dcp->NeCp, 0 );
                    oCp.insert("names", QJsonArray{"a0","a1","a2","a3","a4","a5","a6","a7","a8","a9","a10"} );
                    oCp.insert("units", QJsonArray{"J/(mol*K)","J/(mol*K^2)","(J*K)/mol","J/(mol*K^0.5)","J/(mol*K^3)",
                                                   "J/(mol*K^4)","J/(mol*K^5)","(J*K^2)/mol","J/mol","J/(mol*K^1.5)","J/(mol*K)"} );
                    oTPMethods0["m_heat_capacity_ft_coeffs"] = oCp;
                }
                if( dcp->HKFc && dcp->PdcC != 'W')
                {
                    QJsonObject oHKFc = thf_object( "values", dcp->HKFc, MAXHKFCOEF );
                    oHKFc.insert("names", QJsonArray{"a1","a2","a3","a4","c1","c2","wref"} );
                    oHKFc.insert("units", QJsonArray{"cal/(mol*bar)", "cal/mol", "(cal*K)/(mol*bar)", "(cal*K)/mol",
                                                     "cal/(mol*K)", "(cal*K)/mol", "cal/mol"} );
                    oTPMethods0["eos_hkf_coeffs"] = oHKFc;
                }
                arrTPMethods.append(oTPMethods0);
            }
        }

        method = thf_convert_dcomp_TPMethods1(dcp->pct[1]);
        QJsonObject cpft, sintr;
        cpft["0"] ="cp_ft_equation";
        sintr["43"] ="standard_entropy_cp_integration";
        if( !method.empty() && !(method0 == cpft && method == sintr)) // not writting method standard_entropy_cp_integration if cp_ft_equation is already present
        {
            QJsonObject oTPMethods1;
            oTPMethods1["method"] = method;
            if( dcp->FtBer )
                oTPMethods1["m_landau_phase_trans_props"] = thf_object( "values", dcp->FtBer, 3 );;
            oTPMethods1["m_lambda_trans_props"] = thf_object( "values", &dcp->Smax, 2 );

            arrTPMethods.append(oTPMethods1);
        }

        method = thf_convert_dcomp_TPMethods2(dcp->pct[2]);
        if( !method.empty() )
        {
            QJsonObject oTPMethods2;
            oTPMethods2["method"] = method;
            if( dcp->CpFS )
                oTPMethods2["eos_akinfiev_diamond_coeffs"] = thf_object( "values", dcp->CpFS, MAXCPFSCOEF );;
            if( dcp->Vt )
                oTPMethods2["m_volume_fpt_coeffs"] = thf_object( "values", dcp->Vt, MAXVTCOEF );
            if( dcp->CPg )
            {
                QJsonObject oCPg = thf_object( "values", dcp->CPg, MAXCRITPARAM );
                oCPg.insert("names", QJsonArray{"Tcr","Pcr","W","k1"});
                oCPg.insert("units", QJsonArray{"K","bar","",""});
                oTPMethods2["eos_gas_crit_props"] = oCPg;
            }
            if( dcp->Cemp )
                oTPMethods2["eos_churakov_gottschalk_coeffs"] = thf_object( "values", dcp->Cemp, dcp->Nemp );
            if( dcp->ODc )
                oTPMethods2["eos_birch_murnaghan_coeffs"] = thf_object( "values", dcp->ODc, MAXODCOEF );

            arrTPMethods.append(oTPMethods2);
        }
        if( !arrTPMethods.empty() )
            thermo_data["TPMethods"] = arrTPMethods;
        allArray.append(thermo_data);
        thf_exported_keys.push_back( "DC\t" + plan_key + "\t" + symbol );
    }
    return allArray;
    //o_dccpint, /*17 */  o_dccp,  /*18 */ o_dccpfs,  /*19 */
    //o_dchkf,   /*20 */  o_dcvt,  /*21 */ o_dccritpg,/*22 */
    //o_dcodc,   /*23 */  o_dcftp, /*24 */  o_dcftpb,  /*25 */
    //o_dccemp,  /*26 */  o_dcfttyp, /*27 */ o_dcsdref, /*28 */
    //o_dcsdval, /*29 */  o_dctprn, /*txt*/
}


QJsonArray TReacDC::all_to_thermofun( QJsonArray& subsArray )
{
    QJsonArray allArray;
    TCStringArray skipped_reactdc;
    QJsonObject temp_data;

    // the plan of the export was made with the records of DComp (TDComp::all_to_thermofun)
    for( const auto& plan_key: thf_plan.rc_order )
    {
        RecInput( thf_plan.rc_raw[plan_key].c_str() );
        const std::string species_symbol = thf_plan.rc_symbol[plan_key];
        QString reaction_symbol = species_symbol.c_str();
        thf_check_reaction( plan_key, species_symbol );

        // the last reactant is the species of the record; the reactants are the records of the plan
        QJsonArray oreactants;
        for( int jj=0; jj<rcp->nDC; jj++ )
        {
            const std::string raw( rcp->DCk[jj], DC_RKLEN );
            const std::string reactant = (jj == rcp->nDC-1) ? species_symbol : thf_plan.reactant_symbol( raw );
            oreactants.append(QJsonObject{{"coefficient",rcp->scDC[jj]},
                                          {"symbol", reactant.c_str()}});
        }

        // Set data to substance
        QJsonObject thermo_data;
        thermo_data["name"] = (std::string("_rdc_")+ thf_string(rcp->name, MAXFORMULA)).c_str();
        thermo_data["symbol"] = species_symbol.c_str();
        thermo_data["reaction"] = species_symbol.c_str();
        thermo_data["formula"] = thf_string(rcp->form, MAXFORMULA).c_str();
        thermo_data["formula_charge"] = rcp->Zz;
        thermo_data["mass_per_mole"] = QJsonObject({{"values", QJsonArray({rcp->mwt})}, {"units",QJsonArray({"g/mol"})}});
        thermo_data["aggregate_state"] = thf_convert_dcomp_aggregate_state(*db->FldKey(0));
        thermo_data["class_"] = thf_convert_dcomp_class(rcp->PreC);
        thermo_data["Tst"] = rcp->TCst+273.15;
        thermo_data["Pst"] = rcp->Pst*1e05;
        thermo_data["sm_heat_capacity_p"] = QJsonObject( {{"units",QJsonArray({"J/(mol*K)"})},
                                                          {"values",QJsonArray({rcp->Cps[1]})},
                                                          {"errors",QJsonArray({thf_value(rcp->Cps[2])})}});
        thermo_data["sm_gibbs_energy"] = QJsonObject( {{"units",QJsonArray({"J/mol"})},
                                                       {"values",QJsonArray({rcp->Gs[1]})},
                                                       {"errors",QJsonArray({thf_value(rcp->Gs[2])})}});
        thermo_data["sm_enthalpy"] = QJsonObject( {{"units",QJsonArray({"J/mol"})},
                                                   {"values",QJsonArray({rcp->Hs[1]})},
                                                   {"errors",QJsonArray({thf_value(rcp->Hs[2])})}});
        thermo_data["sm_entropy_abs"] = QJsonObject( {{"units",QJsonArray({"J/(mol*K)"})},
                                                      {"values",QJsonArray({rcp->Ss[1]})},
                                                      {"errors",QJsonArray({thf_value(rcp->Ss[2])})}});
        thermo_data["sm_volume"] = QJsonObject( {{"units",QJsonArray({"J/bar"})},
                                                 {"values",QJsonArray({rcp->Vs[1]})},
                                                 {"errors",QJsonArray({thf_value(rcp->Vs[2])})}});
        //     Undefined
        //     thermo_data["datasources"] =  QJsonObject();
        //     thermo_data["m_compressibility"] = QJsonObject();
        //     thermo_data["m_expansivity"] = QJsonObject();
        //     thermo_data["reaction"] = QJsonObject(); // reaction_symbol[0]  not in DComp ?
        //     thermo_data["limitsTP"] = QJsonObject(); //   s.properties.limitsTP only into TPMethods ?
        subsArray.append(thermo_data);
        thf_exported_keys.push_back( "RC\t" + plan_key + "\t" + species_symbol );

        // Set data to reaction
        QJsonObject reaction_data;
        reaction_data["symbol"] = reaction_symbol;
        reaction_data["Tst"] = rcp->TCst+273.15;
        reaction_data["Pst"] = rcp->Pst*1e05;
        reaction_data["logKr"] = QJsonObject( {{"values",QJsonArray({rcp->Ks[1]})},
                                               {"errors",QJsonArray({thf_value(rcp->Ks[2])})}});
        reaction_data["drsm_heat_capacity_p"] = QJsonObject( {{"units",QJsonArray({"J/(mol*K)"})},
                                                              {"values",QJsonArray({rcp->Cps[0]})}});
        reaction_data["drsm_gibbs_energy"] = QJsonObject( {{"units",QJsonArray({"J/mol"})},
                                                           {"values",QJsonArray({rcp->Gs[0]})}});
        reaction_data["drsm_enthalpy"] = QJsonObject( {{"units",QJsonArray({"J/mol"})},
                                                       {"values",QJsonArray({rcp->Hs[0]})}});
        reaction_data["drsm_entropy"] = QJsonObject( {{"units",QJsonArray({"J/(mol*K)"})},
                                                      {"values",QJsonArray({rcp->Ss[0]})}});
        reaction_data["drsm_volume"] = QJsonObject( {{"units",QJsonArray({"J/bar"})},
                                                     {"values",QJsonArray({rcp->Vs[0]})}});
        QJsonArray datasources;
        for(int isd=0; isd<rcp->Nsd; isd++)
            datasources.append( thf_string(rcp->sdref[isd],V_SD_RKLEN).c_str() );
        reaction_data["datasources "] = datasources;

        // Undefined
        //reaction_data["equation"] = QJsonObject();//:    r.properties.equation, \n "
        //reaction_data["limitsTP"] = QJsonObject();//:   r.properties.limitsTP, \n "

        reaction_data["reactants"] = oreactants;

        QJsonArray arrTPMethods;
        QJsonObject method0, pendingMerge;
        bool hasPendingMerge = false;
        auto method = thf_convert_reacdc_TPMethods0(rcp->pct[0]);
        if( !method.empty() )
        {
            QJsonObject oTPMethods0;
            oTPMethods0["method"] = method;
            if( rcp->pKt )
            {
                QJsonObject opKt = thf_object( "values", rcp->pKt, MAXCPCOEF );
                oTPMethods0["logk_ft_coeffs"] = opKt;
            }
            if( rcp->DCp )
            {
                QJsonObject oDCp = thf_object( "values", rcp->DCp, MAXCPCOEF );
                oTPMethods0["dr_heat_capacity_ft_coeffs"] = oDCp;
            }
            if( rcp->TCint )
                oTPMethods0["limitsTP"] = QJsonObject( {{"lowerT", rcp->TCint[0]+273.15 },
                                                        {"upperT", rcp->TCint[1]+273.15 },
                                                        {"lowerP", rcp->Pint[0]*1e05 },
                                                        {"upperP", rcp->Pint[1]*1e05 }});

            if( oTPMethods0.contains("logk_ft_coeffs") )
                arrTPMethods.append(oTPMethods0);
            else
            {
                // no logk_ft_coeffs: drop this TPMethods entry and carry its
                // other fields forward to merge into the next TPMethods entry
                method0 = method;
                pendingMerge = oTPMethods0;
                pendingMerge.remove("method");
                hasPendingMerge = true;
            }
        }

        method = thf_convert_reacdc_TPMethods1(rcp->pct[1]);
        if( !method.empty() )
        {
            QJsonObject oTPMethods1;
            if( hasPendingMerge )
            {
                oTPMethods1 = pendingMerge;
                hasPendingMerge = false;
            }
            oTPMethods1["method"] = method;
            if( rcp->DSt )
            {
                if( method.contains("10") )
                    oTPMethods1["dr_ryzhenko_coeffs"] = thf_object( "values", rcp->DSt, MAXCPCOEF );
                else
                    oTPMethods1["dr_marshall_franck_coeffs"] = thf_object( "values", rcp->DSt, MAXCPCOEF );
            }
            arrTPMethods.append(oTPMethods1);
        }

        method = thf_convert_reacdc_TPMethods2(rcp->pct[2]);
        if( !method.empty() )
        {
            QJsonObject oTPMethods2;
            if( hasPendingMerge )
            {
                oTPMethods2 = pendingMerge;
                hasPendingMerge = false;
            }
            oTPMethods2["method"] = method;
            if( rcp->DVt )
                oTPMethods2["dr_volume_fpt_coeffs"] = thf_object( "values",rcp->DVt, MAXVTCOEF );
            arrTPMethods.append(oTPMethods2);
        }

        if( hasPendingMerge )
        {
            // nowhere to merge into: keep the original TPMethods[0] entry as-is
            pendingMerge["method"] = method0;
            arrTPMethods.append(pendingMerge);
        }

        if( !arrTPMethods.empty() )
            reaction_data["TPMethods"] = arrTPMethods;

        allArray.append(reaction_data);
    }

    if( !skipped_reactdc.empty() )
    {
        std::string warn_mess = "Skipped records when generating ThermoFun input file."
                               "Need to recalculate records: \n   ";
        for( const auto& rec_key: skipped_reactdc)
        {
           warn_mess  += rec_key+"\n   ";
        }
        gui_logger->warn(warn_mess);
    }

    return allArray;

    //    o_repct /*0*/, o_reprun,   /*1*/
    //    o_repra,    /*2 8*/ o_rermtm ,  o_rezz,  o_remwt,
    //    o_reks,   /*6d 3*/   o_regs,   /*d 3*/   o_rehs,   /*d 3*/
    //    o_ress,   /*9f 3*/   o_recps,  /*f 3*/   o_remvs,  /*f 3*/
    //    o_renix,  /*12f 3*/   o_reptst, /*f 2*/   o_realbet,/*f 2*/
    //    o_reder,  /*15f 2*/
    //    /* record 20 fields */
    //    o_restr /*---*/,  o_rename /*16*/,  o_reform /*17*/,  o_redck, /*18 s dyn*/  o_rerdc, /*19 c dyn*/
    //    o_redim,    /*20 8*/     o_refloat1, /*---*/     o_redbl1,   /*----*/
    //    o_refloat2, /*---*/    o_rescdc,  /*21 d dyn*/  o_retcint, /* 22 f dyn*/
    //    o_repint,  /*23 f dyn*/  o_redcp,   /*24 f dyn*/  o_rehkfc,  /*25 f dyn*/
    //    o_redvt,   /*26 f dyn*/  o_redst,   /*27 f dyn*/  o_repkt,   /*28 f dyn*/
    //    o_relgK,   /*29 f dyn*/  o_resdref, /*s dyn*/  o_resdval ;/*s dyn*/
}


