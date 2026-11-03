// gems-project: builds and fills a Modelling Project without the window, with the same module code as the GUI.
//   gems-project -s <Resources dir> -u <user dir> new  <project key> <database filter> <components,comma separated>
//   gems-project -s <Resources dir> -u <user dir> dump <project key> <out.json>
//   gems-project -s <Resources dir> -u <user dir> replay <project key> <dump.json> <out.tsv> [solver mode aia|sia|aop|hop]
// "new" is the Project > New command with the element dialog (Basis configuration): database files named
// <type>.<filter>.*, listed elements and default phase-type filters. Other commands are in the sections below.
#include <chrono>
#include <cmath>
#include <fstream>
#include <functional>
#include <map>
#include <iomanip>
#include <QFile>
#include <iostream>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include "GEMS3K/nodearray.h"
#include "visor.h"
#include "m_param.h"
#include "m_syseq.h"
#include "m_proces.h"
#include "m_gem2mt.h"
#include "ms_system.h"
#include "v_mod.h"
#include "v_object.h"
#include "ms_multi_new.h"
#include "headless_script.h"

namespace {

std::vector<std::string> split(const std::string& s, char sep)
{
    std::vector<std::string> out;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, sep))
        if (!item.empty()) out.push_back(item);
    return out;
}

int newProject(const std::vector<std::string>& args)
{
    if (args.size() != 3) {
        std::cerr << "usage: new <project key> <database filter> <components,comma separated>\n";
        return 2;
    }
    HeadlessScript& hs = HeadlessScript::get();
    hs.projectKey = args[0];
    hs.tdbFilter = args[1];
    hs.elements = split(args[2], ',');
    pVisor->setElemPrMode(true);
    if (!TProfil::pm->initCalcMode(nullptr)) {
        std::cerr << "the project was not created\n";
        return 1;
    }
    std::cout << "project " << args[0] << " created\n";
    return 0;
}

// dump <project key> <out.json>: the bulk composition table, the switches and the key of every equilibrium record
int dumpSystems(const std::vector<std::string>& args)
{
    if (args.size() != 2) {
        std::cerr << "usage: dump <project key> <out.json>\n";
        return 2;
    }
    if (!TProfil::pm->initCalcMode(args[0].c_str())) {
        std::cerr << "cannot open project " << args[0] << "\n";
        return 1;
    }
    HeadlessScript& hs = HeadlessScript::get();
    const std::string filter = TProfil::pm->projectName() + ":*:*:*:*:*:*:*:";
    TCStringArray keys;
    TCIntArray nrs;
    rt[RT_SYSEQ]->GetKeyList(filter.c_str(), keys, nrs);
    QJsonArray all;
    for (const auto& key : keys) {
        QJsonObject rec;
        rec["key"] = QString::fromStdString(key);
        try {
            TProfil::pm->loadSystat(key.c_str());
            SYSTEM* syp = TSyst::sm->GetSY();
            std::string flags;
            for (char c : { syp->PE, syp->PV, syp->PPfst, syp->PTfst, syp->PbIC, syp->PbDC, syp->PbAC, syp->PbPH,
                            syp->DLLim, syp->DULim, syp->PGEX, syp->PYOF, syp->PAalp, syp->PParc, syp->PpHtg, syp->PEhtg })
                flags += c;
            rec["flags"] = QString::fromStdString(flags);
            hs.capture = true;
            TProfil::pm->systbcInput(nullptr, key.c_str());
            hs.capture = false;
            QJsonArray rows;
            for (const auto& c : hs.captured) {
                QJsonObject r;
                r["page"] = QString::fromStdString(c.page);
                r["name"] = QString::fromStdString(c.name);
                r["nObj"] = c.nObj;
                r["nIdx"] = c.nIdx;
                r["val"] = c.val;
                r["unit"] = QString(QChar(c.unit));
                rows.append(r);
            }
            rec["rows"] = rows;
            // entries switched on in the record
            RMULTS* mup = TRMults::sm->GetMU();
            QJsonArray on;
            for (short i = 0; i < mup->N; i++) if (syp->Icl[i] != S_OFF) on.append("IComp|" + QString::fromLatin1(mup->SB[i], IC_RKLEN));
            for (short j = 0; j < mup->L; j++) if (syp->Dcl[j] != S_OFF) on.append("DComp|" + QString::fromLatin1(mup->SM[j], DC_RKLEN));
            for (short k = 0; k < mup->Fi; k++) if (syp->Pcl[k] != S_OFF) on.append("Phase|" + QString::fromLatin1(mup->SF[k], PH_RKLEN));
            rec["on"] = on;
        }
        catch (TError& e) {
            hs.capture = false;
            rec["error"] = QString::fromStdString(e.title + ": " + e.mess);
        }
        all.append(rec);
    }
    // species of the project with their stoichiometry, to find them again by it in a project of another database version
    QJsonObject dcs;
    {
        RMULTS* mup = TRMults::sm->GetMU();
        for (short j = 0; j < mup->L; j++)
            dcs[QString::fromLatin1(mup->SM[j], MAXSYMB + MAXDRGROUP + MAXDCNAME)] = QString::fromStdString(headlessDcComposition(j));
    }
    QJsonObject doc;
    doc["equilibria"] = all;
    doc["dcs"] = dcs;
    QFile f(QString::fromStdString(args[1]));
    if (!f.open(QIODevice::WriteOnly)) {
        std::cerr << "cannot write " << args[1] << "\n";
        return 1;
    }
    f.write(QJsonDocument(doc).toJson());
    std::cout << "dumped " << keys.size() << " equilibria of " << args[0] << "\n";
    return 0;
}

// "PitzerTHE G  Cc-CaSO4    0 ..." (fields side by side) as "TestTHERE:G:Cc-CaSO4:0:...", with the project field replaced
std::string recordKey(const std::string& packed, const std::string& project)
{
    std::string out = project + ":";
    size_t pos = rt[RT_SYSEQ]->FldLen(0);
    for (int f = 1; f < rt[RT_SYSEQ]->KeyNumFlds(); f++) {
        std::string fld = packed.substr(pos, rt[RT_SYSEQ]->FldLen(f));
        pos += rt[RT_SYSEQ]->FldLen(f);
        strip(fld);
        out += fld + ":";
    }
    return out;
}

// Species of the open project by stoichiometry. oldComp: composition text of the species in the old project.
struct SpeciesIndex
{
    std::vector<std::string> comp;   // per DC of the open project
    SpeciesIndex()
    {
        RMULTS* mup = TRMults::sm->GetMU();
        for (short j = 0; j < mup->L; j++) comp.push_back(headlessDcComposition(j));
    }
    // type letter and composition equal; among several, those in phases of `preferPhases` (indexes in the open project) first
    int find(const std::string& oldKey, const std::string& oldComp, const std::set<int>& preferPhases) const
    {
        RMULTS* mup = TRMults::sm->GetMU();
        if (oldComp.empty()) return -1;
        int first = -1;
        size_t j0 = 0;
        for (short k = 0; k < mup->Fi; k++) {
            for (short jj = 0; jj < mup->Ll[k]; jj++) {
                const size_t j = j0 + jj;
                if (comp[j] == oldComp && mup->SM[j][0] == oldKey[0]) {
                    if (preferPhases.empty() || preferPhases.count(k)) return int(j);
                    if (first < 0) first = int(j);
                }
            }
            j0 += mup->Ll[k];
        }
        return first;
    }
};

// replay <project key> <dump.json> <out.tsv> [mode]: makes the recorded equilibria in the open project, calculates and saves them
int replaySystems(const std::vector<std::string>& args)
{
    if (args.size() < 3) {
        std::cerr << "usage: replay <project key> <dump.json> <out.tsv> [aia|sia|aop|hop]\n";
        return 2;
    }
    if (!TProfil::pm->initCalcMode(args[0].c_str())) {
        std::cerr << "cannot open project " << args[0] << "\n";
        return 1;
    }
    const std::string mode = args.size() > 3 ? args[3] : "aia";
    TProfil::pm->solver_mode = mode == "sia" ? SolverMode::SIA : mode == "aop" ? SolverMode::AOP
                             : mode == "hop" ? SolverMode::HOP : SolverMode::AIA;
    QFile in(QString::fromStdString(args[1]));
    if (!in.open(QIODevice::ReadOnly)) {
        std::cerr << "cannot read " << args[1] << "\n";
        return 1;
    }
    const QJsonObject dumpDoc = QJsonDocument::fromJson(in.readAll()).object();
    const QJsonArray all = dumpDoc["equilibria"].toArray();
    const QJsonObject oldDcs = dumpDoc["dcs"].toObject();
    const SpeciesIndex species;
    std::ofstream tsv(args[2]);
    tsv << "key\tstatus\tskipped\tpH\tEh\tIS\tG\tITG\tseconds\tphases\n";
    HeadlessScript& hs = HeadlessScript::get();
    const std::string project = TProfil::pm->projectName();
    int nOk = 0, nFail = 0;
    for (const auto& v : all) {
        const QJsonObject rec = v.toObject();
        const std::string key = recordKey(rec["key"].toString().toStdString(), project);
        std::string status = "ok", phases;
        double pH = 0, Eh = 0, IS = 0, G = 0, secs = 0;
        long itg = 0;
        try {
            hs.rows.clear();
            for (const auto& rv : rec["rows"].toArray()) {
                const QJsonObject r = rv.toObject();
                HeadlessScript::Row row;
                row.page = r["page"].toString().toStdString();
                row.name = r["name"].toString().toStdString();
                row.nObj = r["nObj"].toInt();
                row.nIdx = r["nIdx"].toInt();
                row.val = r["val"].toDouble();
                row.unit = r["unit"].toString().toLatin1().at(0);
                hs.rows.push_back(row);
            }
            // phases of the record that exist in this project (by name): species are looked for in them first
            RMULTS* mupR = TRMults::sm->GetMU();
            std::set<int> phasesOn;
            for (const auto& ov : rec["on"].toArray()) {
                const std::string o = ov.toString().toStdString();
                if (o.compare(0, 6, "Phase|") != 0) continue;
                if (const int k = headlessPageEntryIndex("Phase", o.substr(6)); k >= 0) phasesOn.insert(k);
            }
            auto resolveDc = [&](const std::string& oldKeyText) {
                const std::string k28 = oldKeyText.substr(0, MAXSYMB + MAXDRGROUP + MAXDCNAME);
                int idx = headlessPageEntryIndex("DComp", oldKeyText);
                if (idx < 0)
                    idx = species.find(k28, oldDcs[QString::fromLatin1(k28.c_str(), int(k28.size()))].toString().toStdString(), phasesOn);
                return idx;
            };
            for (auto& r : hs.rows) {
                if (r.name.empty() || r.page == "IComp" || r.page == "Compos" || r.page == "Phase") continue;
                if (const int idx = resolveDc(r.name); idx >= 0)
                    r.name = std::string(mupR->SM[idx], DC_RKLEN);
            }
            hs.useRows = true;
            hs.nextKey = key;
            if (const int old = rt[RT_SYSEQ]->Find(key.c_str()); old >= 0)
                rt[RT_SYSEQ]->Del(old);   // a replay again makes the record again
            TProfil::pm->newSystat(VF_BYPASS);
            hs.useRows = false;
            // include switches: the record turns off entries
            SYSTEM* syp = TSyst::sm->GetSY();
            auto countOn = [&](const char* when) {
                RMULTS* mup = TRMults::sm->GetMU();
                long ni = 0, nd = 0, np = 0;
                for (short i = 0; i < mup->N; i++) ni += syp->Icl[i] != S_OFF;
                for (short j = 0; j < mup->L; j++) nd += syp->Dcl[j] != S_OFF;
                for (short k = 0; k < mup->Fi; k++) np += syp->Pcl[k] != S_OFF;
                gui_logger->info("{} {}: IC {}/{} DC {}/{} phases {}/{} Pask {}", key, when, ni, mup->N, nd, mup->L, np, mup->Fi, syp->Pask);
            };
            countOn("new record");
            // include switches: only the entries that the record has on stay on (database names mapped, see headlessPageEntryIndex)
            RMULTS* mupS = TRMults::sm->GetMU();
            for (short i = 0; i < mupS->N; i++) syp->Icl[i] = S_OFF;
            for (short j = 0; j < mupS->L; j++) syp->Dcl[j] = S_OFF;
            for (short k = 0; k < mupS->Fi; k++) syp->Pcl[k] = S_OFF;
            hs.skipped.clear();
            // phases first: the database versions name their solids differently, a phase is found by its name
            for (int pass = 0; pass < 2; pass++)
            for (const auto& ov : rec["on"].toArray()) {
                const std::string o = ov.toString().toStdString();
                const auto bar = o.find('|');
                const std::string page = o.substr(0, bar), name = o.substr(bar + 1);
                if ((pass == 0) != (page != "DComp")) continue;
                const int idx = page == "DComp" ? resolveDc(name) : headlessPageEntryIndex(page, name);
                if (idx < 0) { if (pass == 1 || page == "Phase") hs.skipped.push_back(page + " " + name); continue; }
                if (page == "IComp") syp->Icl[idx] = S_ON;
                else if (page == "DComp") syp->Dcl[idx] = S_ON;
                else if (page == "Phase") syp->Pcl[idx] = S_ON;
            }
            // the species of a single-component phase that is on (the species names of solids differ between database versions)
            {
                size_t jj = 0;
                for (short k = 0; k < mupS->Fi; k++) {
                    if (mupS->Ll[k] == 1 && syp->Pcl[k] != S_OFF) syp->Dcl[jj] = S_ON;
                    jj += mupS->Ll[k];
                }
            }
            // "elements": the entries that exist for exactly these elements are on (all phases whose species contain no other element)
            if (rec.contains("elements")) {
                std::set<std::string> els;
                for (const auto& ev : rec["elements"].toArray()) els.insert(ev.toString().toStdString());
                for (short i = 0; i < mupS->N; i++) {
                    std::string sym(mupS->SB[i], MAXICNAME);
                    strip(sym);
                    if (els.count(sym)) syp->Icl[i] = S_ON;
                }
                size_t jj = 0;
                for (short k = 0; k < mupS->Fi; k++) {
                    int nOkDc = 0;
                    for (short l = 0; l < mupS->Ll[k]; l++) {
                        bool ok = true;
                        std::istringstream cs(headlessDcComposition(int(jj + l)));
                        std::string item;
                        while (std::getline(cs, item, ',')) {
                            const auto colon = item.find(':');
                            if (colon == std::string::npos) continue;
                            const std::string el = item.substr(0, colon);
                            if (!els.count(el) && std::atof(item.c_str() + colon + 1) != 0.) ok = false;
                        }
                        if (ok) { syp->Dcl[jj + l] = S_ON; nOkDc++; }
                    }
                    if (nOkDc) syp->Pcl[k] = S_ON;
                    jj += mupS->Ll[k];
                }
            }
            countOn("after switches");
            // title and comment of the recipe dialog
            if (rec.contains("title"))
                strncpy(TSysEq::pm->ssp->name, rec["title"].toString().toUtf8().constData(), MAXFORMULA - 1);
            if (rec.contains("comment"))
                strncpy(TSysEq::pm->ssp->notes, rec["comment"].toString().toUtf8().constData(), MAXFORMULA - 1);
            double kdt = 0.;
            const auto t0 = std::chrono::steady_clock::now();
            TProfil::pm->CalcEqstat(kdt, -1, 0.);
            secs = std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count();
            const MULTI* pm = TMulti::sm->GetPM();
            pH = pm->pH; Eh = pm->Eh; IS = pm->IC; G = pm->FX; itg = pm->ITG;
            for (long k = 0; k < pm->FI; ++k)
                if (pm->XF[k] > 0.) {
                    std::string n(pm->SF[k], MAXPHNAME + MAXSYMB);
                    strip(n);
                    phases += n + "=" + std::to_string(pm->XF[k]) + ";";
                }
            ++nOk;
        }
        catch (TError& e) {
            hs.useRows = false;
            status = e.title + ": " + e.mess;
            ++nFail;
        }
        catch (...) {
            hs.useRows = false;
            status = "unknown error";
            ++nFail;
        }
        // the record is saved with its recipe, switches and, if it calculated, its result
        try { TSysEq::pm->SaveCurrentKey(); }
        catch (TError& e) { status += " [save: " + e.mess + "]"; }
        for (char& c : status) if (c == '\t' || c == '\n') c = ' ';
        if (!hs.skipped.empty()) {
            std::string sk;
            for (const auto& x : hs.skipped) { std::string t = x; for (char& c : t) if (c == '\t') c = ' '; sk += t + " | "; }
            gui_logger->warn("{}: not found in this project: {}", key, sk);
        }
        tsv << key << '\t' << status << '\t' << hs.skipped.size() << '\t' << std::setprecision(10) << pH << '\t' << Eh << '\t' << IS
            << '\t' << G << '\t' << itg << '\t' << secs << '\t' << phases << '\n';
        tsv.flush();
    }
    std::cout << "replayed " << all.size() << " equilibria: " << nOk << " calculated, " << nFail << " failed\n";
    return 0;
}

// export <module keyword> <project key> <out.json>: all records of a module of the open project as JSON (Database > Export to JSON)
int exportRecords(const std::vector<std::string>& args)
{
    if (args.size() != 3) {
        std::cerr << "usage: export <proces|gtdemo|...> <project key> <out.json>\n";
        return 2;
    }
    if (!TProfil::pm->initCalcMode(args[1].c_str())) {
        std::cerr << "cannot open project " << args[1] << "\n";
        return 1;
    }
    for (size_t i = 0; i < aMod.size(); i++)
        if (!aMod[i]->IsSubModule() && args[0] == rt[i]->GetKeywd()) {
            aMod[i]->RecListToJSON("*:*:*:*:*:*:*:*:*:*:*:*:", args[2], true);
            std::cout << "exported " << args[0] << " records to " << args[2] << "\n";
            return 0;
        }
    std::cerr << "unknown module " << args[0] << "\n";
    return 2;
}

// "PitzerTHE:G:X:" -> "TestTHERE:G:X:" in every string of a JSON value that starts with the old project name
QJsonValue renameProject(const QJsonValue& v, const QString& from, const QString& to)
{
    if (v.isString()) {
        const QString t = v.toString();
        if (t == from) return to;
        if (t.startsWith(from + ":")) return to + t.mid(from.size());
        return v;
    }
    if (v.isArray()) {
        QJsonArray out;
        for (const auto& e : v.toArray()) out.append(renameProject(e, from, to));
        return out;
    }
    if (v.isObject()) {
        QJsonObject out;
        const QJsonObject o = v.toObject();
        for (auto it = o.begin(); it != o.end(); ++it) out[it.key()] = renameProject(it.value(), from, to);
        return out;
    }
    return v;
}

// import <module keyword> <project key> <in.json> <old project name>: records exported from another project, with its
// name (in keys and in the keys of the equilibria they list) changed to the open project (Database > Import from JSON)
int importRecords(const std::vector<std::string>& args)
{
    if (args.size() < 4) {
        std::cerr << "usage: import <proces|...> <project key> <in.json> <old project name> [dump.json]\n";
        return 2;
    }
    if (!TProfil::pm->initCalcMode(args[1].c_str())) {
        std::cerr << "cannot open project " << args[1] << "\n";
        return 1;
    }
    QFile in(QString::fromStdString(args[2]));
    if (!in.open(QIODevice::ReadOnly)) {
        std::cerr << "cannot read " << args[2] << "\n";
        return 1;
    }
    const QString to = QString::fromStdString(TProfil::pm->projectName());
    QJsonArray all = renameProject(QJsonDocument::fromJson(in.readAll()).array(), QString::fromStdString(args[3]), to).toArray();
    if (args.size() > 4) {
        // species names in the scripts and graph names: {Ca<2+>} of the old database is {Ca+2} in this one
        QFile df(QString::fromStdString(args[4]));
        if (!df.open(QIODevice::ReadOnly)) {
            std::cerr << "cannot read " << args[4] << "\n";
            return 1;
        }
        const QJsonObject oldDcs = QJsonDocument::fromJson(df.readAll()).object()["dcs"].toObject();
        const SpeciesIndex species;
        RMULTS* mup = TRMults::sm->GetMU();
        std::map<std::string, std::string> nameMap;   // old species name -> name in this project
        for (auto it = oldDcs.begin(); it != oldDcs.end(); ++it) {
            const std::string k28 = it.key().toStdString();
            std::string oldName = k28.substr(MAXSYMB + MAXDRGROUP, MAXDCNAME);
            strip(oldName);
            int idx = headlessPageEntryIndex("DComp", k28);
            if (idx < 0) idx = species.find(k28, it.value().toString().toStdString(), {});
            if (idx < 0) { std::cerr << "no species like " << oldName << " in " << to.toStdString() << "\n"; continue; }
            std::string newName(mup->SM[idx] + MAXSYMB + MAXDRGROUP, MAXDCNAME);
            strip(newName);
            if (!nameMap.count(oldName)) nameMap[oldName] = newName;
        }
        std::function<QJsonValue(const QJsonValue&)> fix = [&](const QJsonValue& v) -> QJsonValue {
            if (v.isString()) {
                QString t = v.toString();
                QString out;
                int pos = 0;
                for (;;) {   // {name} in scripts
                    const int a = t.indexOf('{', pos), b = a < 0 ? -1 : t.indexOf('}', a);
                    if (a < 0 || b < 0) { out += t.mid(pos); break; }
                    out += t.mid(pos, a - pos);
                    const std::string nm = t.mid(a + 1, b - a - 1).toStdString();
                    const auto m = nameMap.find(nm);
                    out += "{" + (m == nameMap.end() ? QString::fromStdString(nm) : QString::fromStdString(m->second)) + "}";
                    pos = b + 1;
                }
                std::string bare = out.toStdString();   // a graph line name that is only a species name
                std::string tr = bare;
                strip(tr);
                if (const auto m = nameMap.find(tr); m != nameMap.end() && tr == bare.substr(0, tr.size()) && bare.find('{') == std::string::npos)
                    return QString::fromStdString(m->second);
                return out;
            }
            if (v.isArray()) {
                QJsonArray a;
                for (const auto& e : v.toArray()) a.append(fix(e));
                return a;
            }
            if (v.isObject()) {
                QJsonObject o;
                const QJsonObject src = v.toObject();
                for (auto it = src.begin(); it != src.end(); ++it)
                    o[it.key()] = (it.key() == "key" || it.key() == "project") ? it.value() : fix(it.value());
                return o;
            }
            return v;
        };
        all = fix(all).toArray();
    }
    const std::string tmp = args[2] + ".renamed.json";
    QFile out(QString::fromStdString(tmp));
    if (!out.open(QIODevice::WriteOnly)) {
        std::cerr << "cannot write " << tmp << "\n";
        return 1;
    }
    out.write(QJsonDocument(all).toJson());
    out.close();
    HeadlessScript::get().replaceExisting = true;   // a record with the same key is replaced
    for (size_t i = 0; i < aMod.size(); i++)
        if (!aMod[i]->IsSubModule() && args[0] == rt[i]->GetKeywd()) {
            aMod[i]->RecListFromJSON(tmp);
            std::cout << "imported " << all.size() << " " << args[0] << " records into " << to.toStdString() << "\n";
            return 0;
        }
    std::cerr << "unknown module " << args[0] << "\n";
    return 2;
}

// run <project key> <out.tsv> [mode]: calculates every equilibrium record of the project as it is stored (Calculate, no save)
int runSystems(const std::vector<std::string>& args)
{
    if (args.size() < 2) {
        std::cerr << "usage: run <project key> <out.tsv> [aia|sia|aop|hop]\n";
        return 2;
    }
    if (!TProfil::pm->initCalcMode(args[0].c_str())) {
        std::cerr << "cannot open project " << args[0] << "\n";
        return 1;
    }
    const std::string mode = args.size() > 2 ? args[2] : "aia";
    TProfil::pm->solver_mode = mode == "sia" ? SolverMode::SIA : mode == "aop" ? SolverMode::AOP
                             : mode == "hop" ? SolverMode::HOP : SolverMode::AIA;
    const std::string filter = TProfil::pm->projectName() + ":*:*:*:*:*:*:*:";
    TCStringArray keys;
    TCIntArray nrs;
    rt[RT_SYSEQ]->GetKeyList(filter.c_str(), keys, nrs);
    std::ofstream tsv(args[1]);
    tsv << "key\tstatus\tpH\tEh\tIS\tG\tITG\tseconds\tphases\n";
    int nOk = 0;
    for (const auto& key : keys) {
        std::string status = "ok", phases;
        double pH = 0, Eh = 0, IS = 0, G = 0, secs = 0;
        long itg = 0;
        try {
            TProfil::pm->loadSystat(key.c_str());
            double kdt = 0.;
            const auto t0 = std::chrono::steady_clock::now();
            TProfil::pm->CalcEqstat(kdt, -1, 0.);
            secs = std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count();
            const MULTI* pm = TMulti::sm->GetPM();
            pH = pm->pH; Eh = pm->Eh; IS = pm->IC; G = pm->FX; itg = pm->ITG;
            for (long k = 0; k < pm->FI; ++k)
                if (pm->XF[k] > 0.) {
                    std::string n(pm->SF[k], MAXPHNAME + MAXSYMB);
                    strip(n);
                    phases += n + "=" + std::to_string(pm->XF[k]) + ";";
                }
            ++nOk;
        }
        catch (TError& e) { status = e.title + ": " + e.mess; }
        for (char& c : status) if (c == '\t' || c == '\n') c = ' ';
        std::string k2 = key;
        tsv << k2 << '\t' << status << '\t' << std::setprecision(10) << pH << '\t' << Eh << '\t' << IS << '\t' << G << '\t' << itg
            << '\t' << secs << '\t' << phases << '\n';
    }
    std::cout << "ran " << keys.size() << " equilibria of " << args[0] << ": " << nOk << " calculated\n";
    return 0;
}

// g0 <project key> <system key> <out.tsv>: standard molar Gibbs energy (J/mol), at the T and P of the record, of each species in it
int speciesG0(const std::vector<std::string>& args)
{
    if (args.size() != 3) {
        std::cerr << "usage: g0 <project key> <system key> <out.tsv>\n";
        return 2;
    }
    if (!TProfil::pm->initCalcMode(args[0].c_str())) {
        std::cerr << "cannot open project " << args[0] << "\n";
        return 1;
    }
    TProfil::pm->loadSystat(args[1].c_str());
    double kdt = 0.;
    TProfil::pm->CalcEqstat(kdt, -1, 0.);
    const MULTI* pm = TMulti::sm->GetPM();
    std::ofstream tsv(args[2]);
    tsv << "species\tcomposition\tG0\tTC\tP\n";
    for (long j = 0; j < pm->L; ++j) {
        std::string n(pm->SM[j], MAXDCNAME);
        strip(n);
        tsv << n << '\t' << '-' << '\t' << std::setprecision(12) << pm->G0[j] << '\t' << pm->TC << '\t' << pm->P << '\n';
    }
    return 0;
}

// plot line styles for the curves of a record (model curves, then measured ones): [{gnm, gtp, gsz, grd, ggr, gbl, gndx}] as in the exported record
static void applyPlotStyles(int objIdx, int nLines, const QJsonArray& styles)
{
    if (styles.isEmpty() || nLines <= 0) return;
    TPlotLine* pl = static_cast<TPlotLine*>(aObj[objIdx]->Alloc(nLines, sizeof(TPlotLine)));
    for (int i = 0; i < nLines && i < styles.size(); i++) {
        const QJsonObject o = styles[i].toObject();
        const int sz = o["gsz"].toInt();
        pl[i] = TPlotLine(o["gnm"].toString().toUtf8().constData(), o["gtp"].toInt(), sz % 100, sz / 100 % 10,
                          o["grd"].toInt(), o["ggr"].toInt(), o["gbl"].toInt());
        pl[i].setIndex(o["gndx"].toInt());
    }
}

// plot window of a record: axis = [x grid, background r, g, b, graph type, y grid]; region = [xmin, xmax, ymin, ymax]; title
template <class Rec>
static void applyPlotWindow(Rec* r, const QJsonObject& spec)
{
    const QJsonArray axis = spec["axis"].toArray(), region = spec["region"].toArray();
    for (int i = 0; i < 6 && i < axis.size(); i++) r->axisType[i] = short(axis[i].toInt());
    for (int i = 0; i < 4 && i < region.size(); i++) {
        const double lo = region[i / 2 * 2].toDouble(), hi = region[i / 2 * 2 + 1].toDouble();
        r->size[0][i] = float(region[i].toDouble());
        r->size[1][i] = float(lo + (hi - lo) * (i % 2 + 1) / 3.);
    }
    if (!spec["title"].toString().isEmpty())
        strncpy(r->name, spec["title"].toString().toUtf8().constData(), MAXFORMULA - 1);
}

// newprocess <project key> <spec.json> [mode]: makes a Process record (Process > Create) from a spec and runs it (Calculate), saving the record
//   spec: key, name, flags (24 chars), sizes (7), table (24 numbers, null keeps), calc, out, names, x, y
int newProcess(const std::vector<std::string>& args)
{
    if (args.size() < 2) {
        std::cerr << "usage: newprocess <project key> <spec.json> [aia|sia|aop|hop]\n";
        return 2;
    }
    if (!TProfil::pm->initCalcMode(args[0].c_str())) {
        std::cerr << "cannot open project " << args[0] << "\n";
        return 1;
    }
    QFile in(QString::fromStdString(args[1]));
    if (!in.open(QIODevice::ReadOnly)) {
        std::cerr << "cannot read " << args[1] << "\n";
        return 1;
    }
    const QJsonObject spec = QJsonDocument::fromJson(in.readAll()).object();
    const std::string mode = args.size() > 2 ? args[2] : "aia";
    TProfil::pm->solver_mode = mode == "sia" ? SolverMode::SIA : mode == "aop" ? SolverMode::AOP
                             : mode == "hop" ? SolverMode::HOP : SolverMode::AIA;
    HeadlessScript& hs = HeadlessScript::get();
    hs.useProc = true;
    hs.procFlags = spec["flags"].toString().toStdString();
    hs.procSize.clear();
    for (const auto& v : spec["sizes"].toArray()) hs.procSize.push_back(v.toInt());
    hs.procTabInt.clear();
    for (const auto& v : spec["tabint"].toArray()) hs.procTabInt.push_back(v.toInt());
    hs.procTable.clear();
    for (const auto& v : spec["table"].toArray()) hs.procTable.push_back(v.isNull() ? std::nan("") : v.toDouble());
    hs.procCalc = spec["calc"].toString().toStdString();
    hs.procOut = spec["out"].toString().toStdString();
    hs.procX = spec["x"].toString().toStdString();
    hs.procY = spec["y"].toString().toStdString();
    hs.procNames.clear();
    for (const auto& v : spec["names"].toArray()) hs.procNames.push_back(v.toString().toStdString());
    const std::string key = spec["key"].toString().toStdString();
    // the start equilibrium is loaded as the Process dialog does before the record is made
    TProfil::pm->loadSystat(spec["start"].toString().toStdString().c_str());
    {
        double kdt = 0.;
        TProfil::pm->CalcEqstat(kdt, -1, 0.);   // the multi of the start equilibrium is made, as in the GUI where it was just calculated
    }
    if (const int old = rt[RT_PROCES]->Find(key.c_str()); old >= 0)
        rt[RT_PROCES]->Del(old);
    TProcess::pm->RecBuild(key.c_str(), VF_REMAKE);
    hs.useProc = false;
    // measured points drawn with the calculated curves: expt = {names, x[], y[point][curve]}; sizes[3], sizes[4] give their dimensions
    const QJsonObject expt = spec["expt"].toObject();
    if (!expt.isEmpty()) {
        PROCESS* pep = TProcess::pm->pep;
        const QJsonArray ex = expt["x"].toArray(), ey = expt["y"].toArray(), en = expt["names"].toArray();
        if (pep->PvEF == S_OFF || pep->dimEF[0] != ex.size() || pep->dimEF[1] != en.size()) {
            std::cerr << "expt dimensions differ from sizes[3], sizes[4] of the spec\n";
            return 1;
        }
        for (int j = 0; j < en.size(); j++)
            strncpy(pep->lNamE[j], en[j].toString().toUtf8().constData(), MAXGRNAME - 1);
        for (int i = 0; i < ex.size(); i++) {
            pep->xE[i] = ex[i].toDouble();
            for (int j = 0; j < en.size(); j++)
                pep->yE[i * en.size() + j] = ey[i].toArray()[j].isNull() ? DOUBLE_EMPTY : ey[i].toArray()[j].toDouble();
        }
    }
    applyPlotWindow(TProcess::pm->pep, spec);
    applyPlotStyles(o_pcplline, TProcess::pm->pep->dimXY[1] + TProcess::pm->pep->dimEF[1], spec["plot"].toArray());
    TProcess::pm->RecSave(key.c_str());
    std::cout << "process " << key << " made\n";
    return 0;
}

// recalc <reacdc|dcomp> <project key> <record key>: Calculate in the module of one record of the open project, then save it
int recalcRecord(const std::vector<std::string>& args)
{
    if (args.size() != 3) {
        std::cerr << "usage: recalc <reacdc|dcomp> <project key> <record key, e.g. s:CaAlClO:Cl-ettringite:th_:>\n";
        return 2;
    }
    if (!TProfil::pm->initCalcMode(args[1].c_str())) {
        std::cerr << "cannot open project " << args[1] << "\n";
        return 1;
    }
    for (size_t i = 0; i < aMod.size(); i++)
        if (!aMod[i]->IsSubModule() && args[0] == rt[i]->GetKeywd()) {
            aMod[i]->RecInput(args[2].c_str());
            aMod[i]->RecCalc(args[2].c_str());
            aMod[i]->RecSave(args[2].c_str());
            std::cout << "recalculated " << args[0] << " " << args[2] << "\n";
            return 0;
        }
    std::cerr << "unknown module " << args[0] << "\n";
    return 2;
}

// settitle <project key> <titles.json>: {"<system key>": ["title", "comment"], ...} sets the Title and Comment of the recipe dialog of
// stored single calculations; the rest of the record (recipe, result) is kept
int setTitles(const std::vector<std::string>& args)
{
    if (args.size() != 2) {
        std::cerr << "usage: settitle <project key> <titles.json>\n";
        return 2;
    }
    if (!TProfil::pm->initCalcMode(args[0].c_str())) {
        std::cerr << "cannot open project " << args[0] << "\n";
        return 1;
    }
    QFile in(QString::fromStdString(args[1]));
    if (!in.open(QIODevice::ReadOnly)) {
        std::cerr << "cannot read " << args[1] << "\n";
        return 1;
    }
    const QJsonObject titles = QJsonDocument::fromJson(in.readAll()).object();
    const std::string project = TProfil::pm->projectName();
    int nSet = 0;
    for (auto it = titles.begin(); it != titles.end(); ++it) {
        const std::string key = recordKey(it.key().toStdString(), project);
        const int ndx = rt[RT_SYSEQ]->Find(key.c_str());
        if (ndx < 0) {
            std::cerr << "no system " << key << "\n";
            continue;
        }
        rt[RT_SYSEQ]->Get(ndx);
        const QJsonArray t = it.value().toArray();
        SYSPACK* ssp = TSysEq::pm->ssp;
        memset(ssp->name, 0, MAXFORMULA);
        memset(ssp->notes, 0, MAXFORMULA);
        strncpy(ssp->name, t[0].toString().toUtf8().constData(), MAXFORMULA - 1);
        strncpy(ssp->notes, t[1].toString().toUtf8().constData(), MAXFORMULA - 1);
        rt[RT_SYSEQ]->Rep(ndx);
        ++nSet;
    }
    std::cout << "titles set on " << nSet << " of " << titles.size() << " systems\n";
    return 0;
}

// delete <module> <project key> <record key>: removes one record of a module of the open project (Database > Delete record)
int deleteRecord(const std::vector<std::string>& args)
{
    if (args.size() != 3) {
        std::cerr << "usage: delete <proces|syseq|gem2mt|compos|...> <project key> <record key>\n";
        return 2;
    }
    if (!TProfil::pm->initCalcMode(args[1].c_str())) {
        std::cerr << "cannot open project " << args[1] << "\n";
        return 1;
    }
    for (size_t i = 0; i < aMod.size(); i++)
        if (!aMod[i]->IsSubModule() && args[0] == rt[i]->GetKeywd()) {
            const int n = rt[i]->Find(args[2].c_str());
            if (n < 0) {
                std::cerr << "record not found: " << args[2] << "\n";
                return 1;
            }
            rt[i]->Del(n);
            std::cout << "deleted " << args[0] << " " << args[2] << "\n";
            return 0;
        }
    std::cerr << "unknown module " << args[0] << "\n";
    return 2;
}

// newgem2mt <project key> <spec.json>: makes a GEM2MT record (GEM2MT > Create) on the equilibrium `start`; flags, sizes, T, P, time, scripts from the spec
int newGem2mt(const std::vector<std::string>& args)
{
    if (args.size() < 2) {
        std::cerr << "usage: newgem2mt <project key> <spec.json>\n";
        return 2;
    }
    if (!TProfil::pm->initCalcMode(args[0].c_str())) {
        std::cerr << "cannot open project " << args[0] << "\n";
        return 1;
    }
    QFile in(QString::fromStdString(args[1]));
    if (!in.open(QIODevice::ReadOnly)) {
        std::cerr << "cannot read " << args[1] << "\n";
        return 1;
    }
    const QJsonObject spec = QJsonDocument::fromJson(in.readAll()).object();
    HeadlessScript& hs = HeadlessScript::get();
    auto nums = [&](const char* k) { std::vector<double> v; for (const auto& x : spec[k].toArray()) v.push_back(x.toDouble()); return v; };
    hs.useG2m = true;
    hs.g2mFlags = spec["flags"].toString().toStdString();
    hs.g2mSize.clear();
    for (const auto& v : spec["sizes"].toArray()) hs.g2mSize.push_back(v.toInt());
    hs.g2mTai = nums("tai"); hs.g2mPai = nums("pai"); hs.g2mTau = nums("tau");
    hs.g2mCalc = spec["calc"].toString().toStdString();
    hs.g2mOut = spec["out"].toString().toStdString();
    hs.g2mX = spec["x"].toString().toStdString();
    hs.g2mY = spec["y"].toString().toStdString();
    hs.g2mNames.clear();
    for (const auto& v : spec["names"].toArray()) hs.g2mNames.push_back(v.toString().toStdString());
    const std::string key = spec["key"].toString().toStdString();
    TProfil::pm->loadSystat(spec["start"].toString().toStdString().c_str());
    if (const int old = rt[RT_GEM2MT]->Find(key.c_str()); old >= 0)
        rt[RT_GEM2MT]->Del(old);
    TGEM2MT::pm->RecBuild(key.c_str(), VF_REMAKE);
    hs.useG2m = false;
    // measured points drawn with the calculated curves: expt = {names, x[], y[point][curve]}; sizes[7], sizes[8] give their dimensions
    const QJsonObject expt = spec["expt"].toObject();
    if (!expt.isEmpty()) {
        GEM2MT* mtp = TGEM2MT::pm->mtp;
        const QJsonArray ex = expt["x"].toArray(), ey = expt["y"].toArray(), en = expt["names"].toArray();
        if (mtp->PvEF == S_OFF || mtp->nE != ex.size() || mtp->nYE != en.size()) {
            std::cerr << "expt dimensions differ from sizes[7], sizes[8] of the spec\n";
            return 1;
        }
        for (int j = 0; j < en.size(); j++)
            strncpy(mtp->lNamE[j], en[j].toString().toUtf8().constData(), MAXGRNAME - 1);
        for (int i = 0; i < ex.size(); i++) {
            mtp->xEt[i] = ex[i].toDouble();
            for (int j = 0; j < en.size(); j++)
                mtp->yEt[i * en.size() + j] = ey[i].toArray()[j].isNull() ? DOUBLE_EMPTY : ey[i].toArray()[j].toDouble();
        }
    }
    applyPlotWindow(TGEM2MT::pm->mtp, spec);
    applyPlotStyles(o_mtplline, TGEM2MT::pm->mtp->nYS + TGEM2MT::pm->mtp->nYE, spec["plot"].toArray());
    TGEM2MT::pm->RecSave(key.c_str());
    std::cout << "GEM2MT " << key << " made\n";
    return 0;
}

// rungem2mt <project key> <GEM2MT key> <out.tsv>: GEM2MT > Calculate; the plotted table (x column, y columns) is written
int runGem2mt(const std::vector<std::string>& args)
{
    if (args.size() != 3) {
        std::cerr << "usage: rungem2mt <project key> <GEM2MT key> <out.tsv>\n";
        return 2;
    }
    if (!TProfil::pm->initCalcMode(args[0].c_str())) {
        std::cerr << "cannot open project " << args[0] << "\n";
        return 1;
    }
    // the equilibrium in the key is the current system (the node structures are made from it), as in the GUI
    {
        std::string start = args[1];
        for (int cut = 0; cut < 2; cut++) start.erase(start.find_last_of(':', start.size() - 2) + 1);
        TProfil::pm->loadSystat(start.c_str());
        double kdt = 0.;
        TProfil::pm->CalcEqstat(kdt, -1, 0.);   // the node structures take the solution of the system
    }
    TGEM2MT::pm->RecInput(args[1].c_str());
    TGEM2MT::pm->RecCalc(args[1].c_str());
    if (std::getenv("GEMS_PROJECT_DEBUG")) {
        const auto* mt = TGEM2MT::pm->mtp;
        const auto* csd = TNodeArray::na->pCSD();
        for (long v = 0; v < mt->nIV; v++) {
            std::cerr << "variant " << v << " Bn:";
            for (long i = 0; i < mt->Nb; i++) if (mt->Bn[v * mt->Nb + i] != 0.) std::cerr << " " << i << "=" << mt->Bn[v * mt->Nb + i];
            std::cerr << "\n";
        }
        for (long q : { 0L, 1L }) {
            std::cerr << "node " << q << " bIC:";
            for (long i = 0; i < csd->nICb; i++) std::cerr << " " << csd->ICNL[csd->xic[i]] << "=" << TNodeArray::na->pNodT1()[q]->bIC[i];
            std::cerr << " TK " << TNodeArray::na->pNodT1()[q]->TK << " P " << TNodeArray::na->pNodT1()[q]->P << " status " << TNodeArray::na->pNodT1()[q]->NodeStatusCH << "\n";
        }
    }
    const long n = aObj[o_mtxt]->GetN(), ny = aObj[o_mtyt]->GetM();
    std::ofstream out(args[2]);
    out << std::setprecision(10);
    for (long i = 0; i < n; ++i) {
        out << aObj[o_mtxt]->Get(i, 0);
        for (long j = 0; j < ny; ++j) out << '\t' << aObj[o_mtyt]->Get(i, j);
        out << '\n';
    }
    std::cout << "GEM2MT " << args[1] << ": " << n << " rows\n";
    return 0;
}

}

int main(int argc, char* argv[])
{
    if (argc < 7 || std::string(argv[1]) != "-s" || std::string(argv[3]) != "-u") {
        std::cerr << "usage: gems-project -s <Resources dir> -u <user dir> <command> <arguments>\n";
        return 2;
    }
    const std::string cmd = argv[5];
    std::vector<std::string> args(argv + 6, argv + argc);
    std::shared_ptr<TVisor> visor;
    try {
        visor = std::make_shared<TVisor>(5, argv);
        pVisor = visor.get();
        pVisor->Setup();
        pVisor->ProfileMode = MDD_SYSTEM;
        int rc = 2;
        if (cmd == "new") rc = newProject(args);
        else if (cmd == "dump") rc = dumpSystems(args);
        else if (cmd == "export") rc = exportRecords(args);
        else if (cmd == "delete") rc = deleteRecord(args);
        else if (cmd == "settitle") rc = setTitles(args);
        else if (cmd == "rungem2mt") rc = runGem2mt(args);
        else if (cmd == "newgem2mt") rc = newGem2mt(args);
        else if (cmd == "recalc") rc = recalcRecord(args);
        else if (cmd == "newprocess") rc = newProcess(args);
        else if (cmd == "g0") rc = speciesG0(args);
        else if (cmd == "run") rc = runSystems(args);
        else if (cmd == "import") rc = importRecords(args);
        else if (cmd == "replay") rc = replaySystems(args);
        else std::cerr << "unknown command " << cmd << "\n";
        pVisor->CanClose();
        return rc;
    }
    catch (TError& e) {
        std::cerr << "error: " << e.title << ": " << e.mess << "\n";
    }
    catch (std::exception& e) {
        std::cerr << "error: " << e.what() << "\n";
    }
    if (pVisor) pVisor->CanClose();
    return 1;
}
