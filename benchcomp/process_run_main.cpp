// process-run: runs one GEMSGUI Process record without the window, in a chosen solver mode, and
// writes the process result table (x0, y0) as tab-separated text. It uses the same code as the
// Process "Calculate" command of the GUI: TProcess::RecCalc() -> TProfil::CalcEqstat() per step.
//
// Usage: process-run -s <Resources dir> -u <user dir> <project> <process key> <aia|sia|aop|sop|hop|shp> <out.tsv>
// The process saves system records into the project, so run it on a copy of the user dir.
// PROCESS_RUN_MODULE=gtdemo runs a GtDemo record (key in place of the process key) instead of a Process.
// PROCESS_RUN_SCRIPTS=1 prints the scripts of the Process record.
// PROCESS_RUN_DEBUG=1 shows the debug messages of the GUI, for example which solver call each calculation used.
// PROCESS_RUN_MODULE=syseq calculates one SysEq record (key in place of the process key); PROCESS_RUN_PH and
// PROCESS_RUN_EH then set the pH and Eh targets of its recipe (Targets, tg_) before the calculation;
// PROCESS_RUN_MODULE=thermofun writes the ThermoFun JSON file of the project to <out.tsv> and, in <out.tsv>.gems, the
// GEMS values (G0, H0, S0, Cp0, V0) of every DComp and ReacDC record. PROCESS_RUN_TP="T;P,T;P" lists the points in
// deg C and bar (default 25;1,50;1,100;1,200;200,25;1000). The key argument is not used.
// PROCESS_RUN_EXPORT=<path>-dat.lst exports the system as GEMS3K files after it.
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <stdexcept>
#include <iomanip>
#include <iostream>
#include <memory>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>
// the thermofun module reads the record lists of the modules, which are protected members
#define protected public
#include "visor.h"
#include "m_param.h"
#include "m_proces.h"
#include "m_syseq.h"
#include "m_gtdemo.h"
#include "m_dcomp.h"
#include "m_reacdc.h"
#include "s_tpwork.h"
#include "ms_system.h"
#include "nodearray_gui.h"
#include "ms_multi_new.h"
#include "v_mod.h"
#include "v_object.h"

extern std::vector<std::string> thf_exported_keys;   // v_thermofun.cpp

int main(int argc, char* argv[])
{
    if (argc < 9) {
        std::cerr << "usage: process-run -s <Resources dir> -u <user dir> <project> <process key> <mode> <out.tsv>\n";
        return 2;
    }
    const std::string project = argv[argc - 4], key = argv[argc - 3], mode = argv[argc - 2], out = argv[argc - 1];
    std::shared_ptr<TVisor> visor;
    try {
        visor = std::make_shared<TVisor>(argc - 4, argv);
        pVisor = visor.get();
        pVisor->Setup();
        pVisor->ProfileMode = MDD_SYSTEM;
        if (!TProfil::pm->initCalcMode(project.c_str())) {
            std::cerr << "cannot open project " << project << "\n";
            pVisor->CanClose();
            return 1;
        }
        SolverMode m = SolverMode::AIA;
        if (mode == "aia") m = SolverMode::AIA;
        else if (mode == "sia") m = SolverMode::SIA;
        else if (mode == "aop") m = SolverMode::AOP;
        else if (mode == "sop") m = SolverMode::SOP;
        else if (mode == "hop") m = SolverMode::HOP;
        else if (mode == "shp") m = SolverMode::SHP;
        else {
            std::cerr << "unknown solver mode '" << mode << "' (aia|sia|aop|sop|hop|shp)\n";
            pVisor->CanClose();
            return 2;
        }
        TProfil::pm->solver_mode = m;

        if (std::getenv("PROCESS_RUN_DEBUG"))
            gui_logger->set_level(spdlog::level::debug);
        if (const char* mod = std::getenv("PROCESS_RUN_MODULE"); mod && std::string(mod) == "gtdemo") {
            TGtDemo::pm->RecInput(key.c_str());
            std::cout << "GtDemo records " << TGtDemo::pm->gdp->Nlrk << " chain " << TGtDemo::pm->gdp->nRT
                      << " recalculate " << TGtDemo::pm->gdp->PsSys << "\n";
            auto dumpGt = [&](const std::string& name) {
                const long nr = aObj[o_gdx0]->GetN(), nx = aObj[o_gdx0]->GetM(), ny = aObj[o_gdy0]->GetM();
                std::ofstream f(name);
                if (!f) throw std::runtime_error("cannot write " + name);
                f << std::setprecision(10);
                for (long i = 0; i < nr; ++i) {
                    for (long j = 0; j < nx; ++j) f << aObj[o_gdx0]->Get(i, j) << '\t';
                    for (long j = 0; j < ny; ++j) f << aObj[o_gdy0]->Get(i, j) << (j + 1 < ny ? '\t' : '\n');
                }
            };
            dumpGt(out + ".stored");
            TGtDemo::pm->RecCalc(key.c_str());
            dumpGt(out);
            std::cout << "GtDemo done\n";
            pVisor->CanClose();
            return 0;
        }
        if (const char* mod = std::getenv("PROCESS_RUN_MODULE"); mod && std::string(mod) == "thermofun") {
            // PROCESS_RUN_SYSTEM=<system key>: export the records of that system (switch '+') instead of the project
            if (const char* sk = std::getenv("PROCESS_RUN_SYSTEM"))
                TProfil::pm->loadSystat(sk);
            std::stringstream js;
            TProfil::pm->generate_ThermoFun_input_file_stream(js, false);
            {
                std::ofstream f(out);
                if (!f) throw std::runtime_error("cannot write " + out);
                f << js.str();
            }
            std::vector<std::pair<double, double>> points;
            {
                std::string spec = std::getenv("PROCESS_RUN_TP") ? std::getenv("PROCESS_RUN_TP") : "25;1,50;1,100;1,200;200,25;1000";
                std::stringstream ss(spec);
                std::string item;
                while (std::getline(ss, item, ',')) {
                    const auto sc = item.find(';');
                    points.emplace_back(std::atof(item.substr(0, sc).c_str()), std::atof(item.substr(sc + 1).c_str()));
                }
            }
            // the T,P of the system records of the project (key fields 7 and 8), with P = 0 read as 1 bar
            {
                TCIntArray anS;
                TCStringArray skeys;
                TSysEq::pm->db->GetKeyList("*:*:*:*:*:*:*:*:", skeys, anS);
                for (const auto& sk : skeys) {
                    std::vector<std::string> f;
                    std::stringstream sks(sk);
                    std::string t;
                    while (std::getline(sks, t, ':')) f.push_back(t);
                    if (f.size() < 8) continue;
                    const double tc = std::atof(f[6].c_str()), pb = std::atof(f[7].c_str());
                    const std::pair<double, double> pt{tc, pb > 0. ? pb : 1.};
                    bool known = false;
                    for (const auto& q : points) known = known || (std::abs(q.first - pt.first) < 1e-9 && std::abs(q.second - pt.second) < 1e-9);
                    if (!known && points.size() < 12) points.push_back(pt);
                }
            }
            // the records in the file: "DC" or "RC", tab, key, tab, symbol in the file
            std::map<std::string, std::string> exported;
            for (const auto& e : thf_exported_keys) {
                const auto t = e.rfind('\t');
                exported[e.substr(0, t)] = e.substr(t + 1);
            }
            std::ofstream g(out + ".gems");
            if (!g) throw std::runtime_error("cannot write " + out + ".gems");
            g << std::setprecision(17) << "kind\tkey\tsymbol\tpct\tTC\tP\tstatus\tG\tH\tS\tCp\tV\n";
            auto sym = [](TCModule* m) { return std::string(m->db->FldKey(2), m->db->FldLen(2)); };
            for (int kind = 0; kind < 2; ++kind) {
                TCModule* mod2 = kind == 0 ? static_cast<TCModule*>(TDComp::pm) : static_cast<TCModule*>(TReacDC::pm);
                TCIntArray anR;
                TCStringArray keys;
                mod2->db->GetKeyList("*:*:*:*:", keys, anR);
                for (const auto& key2 : keys) {
                    // only the records that are in the file
                    std::string trimmed = key2;
                    while (!trimmed.empty() && (trimmed.back() == ' ' || trimmed.back() == ':')) trimmed.pop_back();
                    const auto found = exported.find(std::string(kind == 0 ? "DC\t" : "RC\t") + trimmed);
                    if (found == exported.end()) continue;
                    for (const auto& pt : points) {
                        std::string status = "ok", pct, symbol;
                        double G = 0, H = 0, S = 0, Cp = 0, V = 0;
                        try {
                            mod2->RecInput(key2.c_str());
                            symbol = found->second;
                            pct = std::string(kind == 0 ? TDComp::pm->dcp->pct : TReacDC::pm->rcp->pct, 6);
                            aW.ods_link(0);
                            aW.set_zero(0);
                            aW.twp->P = pt.second;
                            aW.twp->TC = pt.first;
                            aW.twp->T = pt.first + C_to_K;
                            aW.twp->unE = 'j';
                            aW.twp->unV = 'j';
                            aSpc.on_sat_curve = false;
                            if (kind == 0) {
                                aW.twp->TCst = TDComp::pm->dcp->TCst;
                                aW.twp->Pst = TDComp::pm->dcp->Pst;
                                aW.twp->Tst = aW.twp->TCst + C_to_K;
                                TDComp::pm->DCthermo(0, 0);
                            } else {
                                aW.twp->TCst = TReacDC::pm->rcp->TCst;
                                aW.twp->Pst = TReacDC::pm->rcp->Pst;
                                aW.twp->Tst = aW.twp->TCst + C_to_K;
                                TReacDC::pm->RCthermo(0, 0);
                                TReacDC::pm->ods_link(0);
                                aW.ods_link(0);
                            }
                            G = aW.twp->G; H = aW.twp->H; S = aW.twp->S; Cp = aW.twp->Cp; V = aW.twp->V;
                        } catch (TError& e) {
                            status = "ERR " + e.mess;
                        } catch (std::exception& e) {
                            status = std::string("ERR ") + e.what();
                        }
                        for (auto& c : status) if (c == '\t' || c == '\n') c = ' ';
                        g << (kind == 0 ? "DC" : "RC") << '\t' << key2 << '\t' << symbol << '\t' << pct << '\t' << pt.first << '\t'
                          << pt.second << '\t' << status << '\t' << G << '\t' << H << '\t' << S << '\t' << Cp << '\t' << V << '\n';
                    }
                }
            }
            std::cout << "thermofun: wrote " << out << " and " << out << ".gems\n";
            pVisor->CanClose();
            return 0;
        }
        if (const char* mod = std::getenv("PROCESS_RUN_MODULE"); mod && std::string(mod) == "syseq") {
            TProfil::pm->loadSystat(key.c_str());
            SYSTEM* syp = TSyst::sm->GetSY();
            if (const char* v = std::getenv("PROCESS_RUN_PH")) { syp->PpHtg = S_ON; syp->pHtg = std::atof(v); }
            if (const char* v = std::getenv("PROCESS_RUN_EH")) { syp->PEhtg = S_ON; syp->Ehtg = std::atof(v); }
            double kdt = 0.;
            TProfil::pm->CalcEqstat(kdt, -1, 0.);
            const MULTI* pm = TMulti::sm->GetPM();
            std::cout << std::setprecision(10) << "syseq: pH " << pm->pH << " Eh " << pm->Eh << " G " << pm->FX
                      << " IC";
            for (long i = 0; i < pm->N; ++i) std::cout << " " << std::string(pm->SB[i], MAXICNAME).substr(0, 2) << "=" << pm->B[i];
            std::cout << "\n";
            for (long k = 0; k < pm->FI; ++k)
                if (pm->XF[k] > 0.) std::cout << "  " << std::string(pm->SF[k], MAXPHNAME + MAXSYMB) << " XF " << pm->XF[k] << "\n";
            // the b tables: recipe input B_ (o_syb), result b (o_wi_b), residuals Cb (o_wo_c); then saved, reloaded
            auto tables = [&](const char* when) {
                std::cout << when;
                for (long i = 0; i < pm->N; ++i)
                    std::cout << "  " << std::string(pm->SB[i], MAXICNAME).substr(0, 2) << ": B_ " << aObj[o_syb]->Get(pm->mui[i], 0)
                              << " b " << aObj[o_wi_b]->Get(i, 0) << " Cb " << aObj[o_wo_c]->Get(i, 0);
                std::cout << "\n";
            };
            tables("after the calculation:");
            // PROCESS_RUN_EXPORT=<path>-dat.lst writes the system as GEMS3K files, as the Data menu export does
            if (const char* ex = std::getenv("PROCESS_RUN_EXPORT")) {   // as TProfil::CurrentSystem2GEMS3K()
                double Tai[4] = { pm->TCc, pm->TCc, 0., 0.1 }, Pai[4] = { pm->Pc, pm->Pc, 0., 0.1 };
                auto na = TNodeArrayGUI::create(1, TMulti::sm);
                na->setExportNode();
                na->MakeNodeStructuresOne(nullptr, true, Tai, Pai);
                ProcessProgressFunction msg = [](const std::string&, long) { return false; };
                na->genGEMS3KInputFiles(ex, msg, 1, GEMS3KGenerator::default_type_f, false, false, false, false);
            }
            if (std::getenv("PROCESS_RUN_SAVE")) {
                TSysEq::pm->SaveCurrentKey();
                TProfil::pm->loadSystat(key.c_str());
                tables("saved and loaded again:");
            }
            pVisor->CanClose();
            return 0;
        }
        TProcess::pm->RecInput(key.c_str());
        if (std::getenv("PROCESS_RUN_SCRIPTS")) {
            if (TProcess::pm->pep->Expr) std::cout << "--- process script\n" << TProcess::pm->pep->Expr << "\n";
            if (TProcess::pm->pep->gr_expr) std::cout << "--- sampling script\n" << TProcess::pm->pep->gr_expr << "\n---\n";
        }
        std::cout << "time-dependent " << TProcess::pm->pep->PsRT << " Taui " << TProcess::pm->pep->Taui[0] << " "
                  << TProcess::pm->pep->Taui[1] << " " << TProcess::pm->pep->Taui[2] << "\n";
        // PROCESS_RUN_PVR1=0|1 overrides the process flag that sets a cold (0) or warm (1) start of each step
        if (const char* v = std::getenv("PROCESS_RUN_PVR1"))
            TProcess::pm->pep->PvR1 = (std::string(v) == "1") ? S_ON : S_OFF;
        std::cout << "PvR1 " << TProcess::pm->pep->PvR1 << "\n";
        // the result table stored in the record, before it is calculated again
        {
            const long nr = aObj[o_pcx0]->GetN(), nx = aObj[o_pcx0]->GetM(), ny = aObj[o_pcy0]->GetM();
            std::ofstream f(out + ".stored");
            if (!f) throw std::runtime_error("cannot write " + out + ".stored");
            f << std::setprecision(10);
            for (long i = 0; i < nr; ++i) {
                for (long j = 0; j < nx; ++j) f << aObj[o_pcx0]->Get(i, j) << '\t';
                for (long j = 0; j < ny; ++j) f << aObj[o_pcy0]->Get(i, j) << (j + 1 < ny ? '\t' : '\n');
            }
        }
        TProcess::pm->RecCalc(key.c_str());

        // the process result table: one row per step, x0 = abscissa columns, y0 = sampled values
        const long nr = aObj[o_pcx0]->GetN(), nx = aObj[o_pcx0]->GetM(), ny = aObj[o_pcy0]->GetM();
        std::ofstream f(out);
        if (!f) throw std::runtime_error("cannot write " + out);
        f << std::setprecision(10);
        for (long i = 0; i < nr; ++i) {
            for (long j = 0; j < nx; ++j) f << aObj[o_pcx0]->Get(i, j) << '\t';
            for (long j = 0; j < ny; ++j) f << aObj[o_pcy0]->Get(i, j) << (j + 1 < ny ? '\t' : '\n');
        }
        std::cout << "rows " << nr << " x " << nx << " y " << ny << "\n";
        // final state of the last step: phases with an amount, and the species of multicomponent phases
        const MULTI* pm = TMulti::sm->GetPM();
        std::cout << std::setprecision(10) << "last step: T " << pm->TCc << " C, G " << pm->FX << ", status MK " << pm->MK << "\n";
        for (long k = 0, j0 = 0; k < pm->FI; j0 += pm->L1[k], ++k) {
            if (pm->XF[k] <= 0.) continue;
            std::cout << "  " << std::string(pm->SF[k], MAXPHNAME + MAXSYMB) << " XF " << pm->XF[k] << "\n";
            if (pm->L1[k] > 1 && k > 0)
                for (long j = j0; j < j0 + pm->L1[k]; ++j)
                    std::cout << "     " << std::string(pm->SM[j], MAXDCNAME) << " X " << pm->X[j] << " Wx " << pm->Wx[j] << "\n";
        }
        pVisor->CanClose();
    }
    catch (TError& e) {
        std::cerr << "error: " << e.title << ": " << e.mess << "\n";
        if (visor) pVisor->CanClose();   // close the modules, also after an error, or the exit frees memory twice
        return 1;
    }
    catch (std::exception& e) {
        std::cerr << "error: " << e.what() << "\n";
        if (visor) pVisor->CanClose();
        return 1;
    }
    return 0;
}
