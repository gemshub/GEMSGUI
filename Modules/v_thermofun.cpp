#include "v_thermofun.h"
#include "m_icomp.h"
#include "m_dcomp.h"
#include "m_reacdc.h"

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
    TCIntArray anR;
    TCStringArray aDC1;
    QJsonObject temp_data;
    db->GetKeyList( "*:*:*:*:", aDC1, anR );

    for( size_t ii=0; ii<aDC1.size(); ii++ )
    {
        RecInput( aDC1[ii].c_str() );
        QJsonObject thermo_data;

        thermo_data["name"] = thf_string(dcp->name, MAXFORMULA).c_str();
        thermo_data["symbol"] = thf_string(db->FldKey(2), db->FldLen(2)).c_str();
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

            arrTPMethods.append(oTPMethods2);
        }
        if( !arrTPMethods.empty() )
            thermo_data["TPMethods"] = arrTPMethods;
        allArray.append(thermo_data);
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
    TCIntArray anR;
    TCStringArray aRC1, error_reactdc;
    QJsonObject temp_data;
    db->GetKeyList( "*:*:*:*:", aRC1, anR );

    for( size_t ii=0; ii<aRC1.size(); ii++ )
    {
        RecInput( aRC1[ii].c_str() );

        // Set data to substance
        QJsonObject thermo_data;
        thermo_data["name"] = (std::string("_rdc_")+ thf_string(rcp->name, MAXFORMULA)).c_str();
        thermo_data["symbol"] = thf_string(db->FldKey(2), db->FldLen(2)).c_str();
        thermo_data["reaction"] = thf_string(db->FldKey(2), db->FldLen(2)).c_str();
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

        // Set data to reaction
        QJsonObject reaction_data;
        reaction_data["symbol"] = thf_string(db->FldKey(2), db->FldLen(2)).c_str();
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

        QJsonArray oreactants;
        for( int ii=0; ii<rcp->nDC; ii++ )
        {
            oreactants.append(QJsonObject{{"coefficient",rcp->scDC[ii]},
                                          {"symbol", thf_string(rcp->DCk[ii]+MAXSYMB+MAXDRGROUP, MAXDCNAME).c_str()}});
        }
        reaction_data["reactants"] = oreactants;
        if( oreactants.last().toObject()["symbol"] != reaction_data["symbol"] )
        {
            error_reactdc.push_back(aRC1[ii]);
        }

        QJsonArray arrTPMethods;
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
            arrTPMethods.append(oTPMethods0);
        }

        method = thf_convert_reacdc_TPMethods1(rcp->pct[1]);
        if( !method.empty() )
        {
            QJsonObject oTPMethods1;
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
            oTPMethods2["method"] = method;
            if( rcp->DVt )
                oTPMethods2["dr_volume_fpt_coeffs"] = thf_object( "values",rcp->DVt, MAXVTCOEF );
            arrTPMethods.append(oTPMethods2);
        }
        if( !arrTPMethods.empty() )
            reaction_data["TPMethods"] = arrTPMethods;

        allArray.append(reaction_data);
    }

    if( !error_reactdc.empty() )
    {
        std::string err_mess = "Error when generate ThermoFun input file."
                               "Need to recalculate records: \n   ";
        for( const auto& rec_key: error_reactdc)
        {
           err_mess  += rec_key+"\n   ";
        }
        Error( GetName(), err_mess);
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


