#include <algorithm>
#include <map>
#include <cctype>
#include <filesystem>
namespace fs = std::filesystem;

#include "service.h"
#include "visor.h"
#include "headless_script.h"
#include "v_mod.h"
#include "m_icomp.h"
#include "m_param.h"
#include "ms_rmults.h"
#include "filters_data.h"
#include "s_formula.h"

HeadlessScript& HeadlessScript::get()
{
    static HeadlessScript script;
    return script;
}

TCStringArray vfFiles(const std::string& dir_name, const std::string& ext) {
    TCStringArray files;
    fs::path dir_path(dir_name);
    std::error_code ec;
    for (fs::directory_iterator it(dir_path, ec), end; !ec && it != end; it.increment(ec)) {
        if (it->is_regular_file(ec) && (ext.empty() || it->path().extension() == ext)) {
            std::string file = it->path().filename().string();
            gui_logger->trace("Adding file: {}", file);
            files.push_back(file);
        }
    }
    return files;
}


bool vfExist(const std::string &file_path)
{
    // return !(::access(file_path.c_str(), 0 ));
    fs::path ps(file_path);
    return fs::exists(ps);
}

int vfQuestYesNoCancel(QWidget*, const std::string& title, const std::string& mess)
{
    gui_logger->info("vfQuestYesNoCancel {} {}", title, mess);
    return 0;
}

bool vfQuestion(QWidget*, const std::string& title, const std::string& mess)
{
    gui_logger->info("vfQuestion {} {}", title, mess);
    return false;
}

// returns VF3_1, VF3_2 or VF3_3
int vfQuestion3(QWidget*, const std::string& title, const std::string& mess,
                const std::string&, const std::string&, const std::string&, bool )
{
    gui_logger->info("vfQuestion3 {} {}", title, mess);
    return 0;
}

int vfQuestionYesNoAll(QWidget*, const std::string& title, const std::string& mess, const std::string&)
{
    gui_logger->info("vfQuestionYesNoAll {} {}", title, mess);
    if (HeadlessScript::get().replaceExisting)
        return VF_YES_ALL;
    return 0;
}

void vfMessage(QWidget*, const std::string& title, const std::string& mess, WarnType)
{
    gui_logger->info("vfMessage {} {}", title, mess);
}

int vfChoice(QWidget*, TCStringArray& arr, const char* prompt, int)
{
    gui_logger->info("vfChoice {} {}", prompt, arr.size());
    return 0;
}

int vfChoice2(QWidget*, TCStringArray& arr, const char* prompt, int, bool&)
{
    gui_logger->info("vfChoice2 {} {}", prompt, arr.size());
    return 0;
}

int vfChoice(QWidget*, const char* title, const char* prompt, int nVal, int *, int)
{
    gui_logger->info("vfChoice3 {} {} {}", title, prompt, nVal);
    return 0;
}

TCIntArray vfMultiChoice(QWidget*, TCStringArray& arr, const char* prompt)
{
    gui_logger->info("vfMultiChoice {} {}", prompt, arr.size());
    return {};
}

TCIntArray vfMultiChoiceSet(QWidget*, TCStringArray& arr, const char* prompt, TCIntArray&)
{
    gui_logger->info("vfMultiChoiceSet {} {}", prompt, arr.size());
    return {};
}

TCStringArray vfMultiKeys(QWidget*, const char* caption, unsigned int iRt, const char* key)
{
    gui_logger->info("vfMultiKeys {} {} {}", caption, key, iRt);
    return {};
}

TCStringArray vfMultiKeysSet(QWidget*, const char* caption,
                             unsigned int iRt, const char* key, TCStringArray&)
{
    gui_logger->info("vfMultiKeysSet {} {} {}", caption, key, iRt);
    return {};
}

TCStringArray vfRDMultiKeysSet(QWidget*, const char* caption,
                               const char* key, TCStringArray&, short)
{
    gui_logger->info("vfRDMultiKeysSet {} {}", caption, key);
    return {};
}

// Without the dialog (show_dlg false) the selection is what the dialog shows first: the files that are open now,
// built-in files before project files in each module. Only a new project from the scripted element dialog uses it.
bool vfListFiles(QWidget*, bool show_dlg, const char* prfName,
                 TCStringArray& fls, TCIntArray& cnt)
{
    gui_logger->info("vfListFiles {} {}", prfName, show_dlg);
    if (show_dlg || HeadlessScript::get().projectKey.empty())
        return false;
    fls.clear();
    cnt.clear();
    for (size_t i = 0; i < aMod.size(); i++) {
        if (aMod[i]->IsSubModule())
            continue;
        TCStringArray names;
        TCIntArray indx, sel;
        rt[i]->GetFileList(closef | openf | oldself, names, indx, sel);
        int n = 0;
        for (int kernel = 1; kernel >= 0; kernel--)
            for (size_t ii = 0; ii < names.size(); ii++) {
                const bool inKernel = names[ii].find(pVisor->sysDBDir()) != std::string::npos;
                const bool inProject = names[ii].find(prfName) != std::string::npos;
                if (kernel ? !inKernel : !inProject)
                    continue;
                if (std::find(sel.begin(), sel.end(), indx[ii]) == sel.end())
                    continue;
                fls.push_back(names[ii].substr(0, names[ii].find_first_of(' ')));
                n++;
            }
        cnt.push_back(n);
    }
    return true;
}

std::string vfKeyEdit(QWidget*, const char* title, unsigned int iRt, const char* key)
{
    gui_logger->info("vfKeyEdit {} {} {}", title, iRt, key);
    return "";
}

std::string vfKeyProfile(QWidget*, const char* caption, int iRt,
                         bool&, bool&, bool&, std::string&, int&, int&, int&)
{
    gui_logger->info("vfKeyProfile {} {}", caption, iRt);

    if (!HeadlessScript::get().projectKey.empty())
        return ALLKEY;   // new project
    return "";
}


std::string vfKeyTemplEdit(QWidget*, const char* title, unsigned int iRt, const char* key, bool)
{
    gui_logger->info("vfKeyTemplEdit {} {} {}", title, iRt, key);
    HeadlessScript& hs = HeadlessScript::get();
    if (iRt == RT_PARAM && !hs.projectKey.empty())
        return hs.projectKey;
    if (!hs.nextKey.empty()) {
        std::string k = hs.nextKey;
        hs.nextKey.clear();
        return k;
    }
    return "";
}

bool vfKeyCanged(QWidget*, const char* caption, std::string& from_str, std::string& to_str, int)
{
    gui_logger->info("vfKeyCanged {} {} {}", caption, from_str, to_str);
    return false;
}

bool vfExcludeFillEdit(QWidget*, const char* caption, TCStringArray& aICkeys, std::vector<bool>&, double&)
{
    gui_logger->info("vfExcludeFillEdit {} {}", caption, aICkeys.size());
    return false;
}

// The element dialog with the answers of HeadlessScript: the built-in database files named <type>.<tdbFilter>.*
// (the SDref and Const files of that database too), the listed elements, the default phase-type filters.
bool vfElements(QWidget*, const char* prfName, elmWindowData& elm_data, setFiltersData& sf_data)
{
    gui_logger->info("vfElements {}", prfName);
    HeadlessScript& hs = HeadlessScript::get();
    if (hs.projectKey.empty() || hs.tdbFilter.empty())
        return false;

    // files of the built-in database that are selected (the dialog's openFilesSelection())
    const std::string prefix = hs.tdbFilter.front() == '.' ? hs.tdbFilter.substr(1) : hs.tdbFilter;
    for (size_t i = RT_SDATA; i <= RT_PHASE; i++) {
        if (aMod[i]->IsSubModule())
            continue;
        TCStringArray names, selKeywds;
        TCIntArray indx, sel;
        rt[i]->GetFileList(closef | openf | oldself, names, indx, sel);
        for (const auto& nm : names) {
            if (nm.find(pVisor->sysDBDir()) == std::string::npos)
                continue;
            const auto sp = nm.find_first_of(' ');
            const auto sl = nm.rfind('/');
            const std::string fname = nm.substr(sl + 1);
            const std::string kw = nm.substr(0, sp);
            const std::string tag = std::string(rt[i]->GetKeywd()) + "." + prefix;
            if (fname.compare(0, tag.size(), tag) == 0)
                selKeywds.push_back(kw);
        }
        rt[i]->GetProfileFileKeywds(prfName, selKeywds);
        rt[i]->SetNewOpenFileList(selKeywds);
        gui_logger->info("vfElements {}: {} files open", rt[i]->GetKeywd(), selKeywds.size());
    }

    // elements and additional components
    TCIntArray aIndMT;
    TCStringArray aIC;
    TIComp* aICdata = dynamic_cast<TIComp*>(aMod[RT_ICOMP].get());
    aICdata->GetElements(false, aIC, aIndMT);
    elm_data.ICrds.clear();
    std::vector<bool> found(hs.elements.size(), false);
    for (const auto& key : aIC) {
        std::string name(key, 0, rt[RT_ICOMP]->FldLen(0));
        strip(name);
        for (size_t k = 0; k < hs.elements.size(); k++)
            if (name == hs.elements[k]) {
                elm_data.ICrds.push_back(key);
                found[k] = true;
            }
    }
    for (size_t k = 0; k < found.size(); k++)
        if (!found[k])
            gui_logger->warn("vfElements: component {} is not in the selected database files", hs.elements[k]);

    sf_data = setFiltersData();
    sf_data.ic_d.newIComps = elm_data.ICrds;
    elm_data.aSelNames = elm_data.getFlags();
    elm_data.aSelNames += "<TDBfilters> = " + hs.tdbFilter + ";\n";
    TProfil::pm->useAqPhase = elm_data.flags[cbAqueous_];
    TProfil::pm->useGasPhase = elm_data.flags[cbGaseous_];
    return true;
}

namespace {

// the record key text (blanks stripped) of entry idx of a window page of the bulk composition dialog
std::string pageEntryKey(const std::string& page, int idx)
{
    RMULTS* mup = TRMults::sm->GetMU();
    const char* key = nullptr;
    size_t len = 0;
    if (page == "IComp" && idx < mup->N) { key = mup->SB[idx]; len = IC_RKLEN; }
    else if (page == "Compos" && idx < mup->La) { key = mup->SA[idx]; len = BC_RKLEN; }
    else if (page == "Phase" && idx < mup->Fi) { key = mup->SF[idx]; len = PH_RKLEN; }
    else if ((page == "DComp" || page == "Kin.lower" || page == "Kin.upper" || page == "G0 shift") && idx < mup->L) {
        key = mup->SM[idx]; len = DC_RKLEN;
    }
    if (!key) return "";
    return std::string(key, len);
}

// index of the entry of a page whose record key starts with the first three fields of `name`
// (type, group or symbol, name; the data-source tail of the key may differ between database versions)
int pageEntryIndex(const std::string& page, const std::string& name)
{
    RMULTS* mup = TRMults::sm->GetMU();
    size_t n = 0, head = 0;
    if (page == "IComp") { n = mup->N; head = MAXICNAME + MAXSYMB; }
    else if (page == "Compos") { n = mup->La; head = MAXCMPNAME; }
    else if (page == "Phase") { n = mup->Fi; head = MAXSYMB + MAXPHSYMB + MAXPHNAME; }
    else { n = mup->L; head = MAXSYMB + MAXDRGROUP + MAXDCNAME; }
    for (size_t i = 0; i < n; i++)
        if (pageEntryKey(page, i).compare(0, head, name, 0, head) == 0)
            return int(i);
    if (page == "IComp" || page == "Compos")
        return -1;
    if (page == "Phase") {
        // the same phase has another group code in another database version (Na2Ca(SO4)2: NaCaSO or CaNaSO): type and name
        const size_t nm = MAXSYMB + MAXPHSYMB, len = MAXPHNAME;
        for (size_t i = 0; i < n; i++) {
            const std::string key = pageEntryKey(page, i);
            if (key.compare(0, MAXSYMB, name, 0, MAXSYMB) == 0 && key.compare(nm, len, name, nm, len) == 0)
                return int(i);
        }
        return -1;
    }
    // THEREDA 2022 writes charges as "Ca<2+>", "Cl<->", "H4SiO4<0>"; the 2026 files as "Ca+2", "Cl-", "Si(OH)4@" for species;
    // a phase or species name that differs only in this notation is the same one
    const size_t nameAt = page == "Phase" ? MAXSYMB + MAXPHSYMB : MAXSYMB + MAXDRGROUP;
    const size_t nameLen = head - nameAt;
    auto translate = [](std::string nm) {
        strip(nm);
        const auto lt = nm.rfind('<'), gt = nm.rfind('>');
        if (lt == std::string::npos || gt != nm.size() - 1)
            return nm;
        const std::string ch = nm.substr(lt + 1, gt - lt - 1);   // "2+", "+", "0", "3-"
        std::string out = nm.substr(0, lt);
        if (ch == "0") return out + "@";
        if (ch.size() == 1) return out + ch;                      // "+" or "-"
        return out + ch.substr(1) + ch.substr(0, 1);              // "2+" -> "+2"
    };
    // formulas of the 2026 files have no brackets round a group that is used once: Ca(SO4)<0> is CaSO4@, Mg(OH)<+> is MgOH+
    auto unbracket = [](const std::string& nm) {
        std::string out;
        for (size_t i = 0; i < nm.size(); i++) {
            if (nm[i] == '(') {
                const auto close = nm.find(')', i);
                if (close != std::string::npos && (close + 1 >= nm.size() || !std::isdigit((unsigned char)nm[close + 1]))) {
                    out += nm.substr(i + 1, close - i - 1);
                    i = close;
                    continue;
                }
            }
            out += nm[i];
        }
        return out;
    };
    const std::string want = translate(name.substr(nameAt, nameLen));
    const std::string want2 = unbracket(want);
    for (size_t i = 0; i < n; i++) {
        const std::string key = pageEntryKey(page, i);
        if (key.compare(0, nameAt, name, 0, nameAt) != 0)
            continue;
        std::string have = key.substr(nameAt, nameLen);
        strip(have);
        if (have == want || have == want2)
            return int(i);
    }
    if (std::getenv("GEMS_PROJECT_DEBUG")) {
        std::string cand;
        for (size_t i = 0; i < n; i++) {
            const std::string key = pageEntryKey(page, i);
            if (key.compare(0, nameAt, name, 0, nameAt) == 0) cand += "[" + key.substr(0, head) + "]";
        }
        gui_logger->info("no match for [{}] want [{}] nameAt {} candidates {}", name.substr(0, head), want, nameAt, cand);
        if (n > 0) gui_logger->info("first keys [{}] [{}] [{}]", pageEntryKey(page, 0), pageEntryKey(page, n / 2), pageEntryKey(page, n - 1));
    }
    return -1;
}

}

// Stoichiometry of DC j of the open project as text over its independent components: "Ca:1,Na:2,O:8,S:2,Zz:0". Equal text
// is the same chemical species whatever the name (THEREDA 2022 "Na2Ca(SO4)2(cr)" and 2026 "Gla").
std::string headlessDcComposition(int j)
{
    RMULTS* mup = TRMults::sm->GetMU();
    try {
        TFormula aFo;
        const std::string form = aFo.form_extr(j, mup->L, mup->DCF);
        aFo.SetFormula(form.c_str());
        std::vector<double> sml(mup->N, 0.);
        aFo.Stm_line(mup->N, sml.data(), reinterpret_cast<char*>(mup->SB), mup->Val);
        std::map<std::string, double> byName;
        for (short i = 0; i < mup->N; i++) {
            if (sml[i] == 0.) continue;
            std::string nm(mup->SB[i], MAXICNAME);
            strip(nm);
            byName[nm] = sml[i];
        }
        std::string out;
        char buf[32];
        for (const auto& [nm, v] : byName) {
            snprintf(buf, sizeof buf, "%.6g", v);
            out += nm + ":" + buf + ",";
        }
        return out;
    }
    catch (...) { return ""; }
}

int headlessPageEntryIndex(const std::string& page, const std::string& name)
{
    return pageEntryIndex(page, name);
}

// Bulk composition dialog: in capture mode the table is recorded, with the record keys of its entries; with rows it
// is replaced by them. A row that has no entry in this project is skipped and reported.
bool vfSystemInput(QWidget*, const char* p_key, std::vector<windowSetupData>& wnData,
                   std::vector<tableSetupData>& tbData, std::vector<pagesSetupData>&)
{
    gui_logger->info("vfSystemInput {}", p_key);
    HeadlessScript& hs = HeadlessScript::get();
    if (hs.capture) {
        hs.captured.clear();
        for (const auto& t : tbData) {
            const std::string& page = wnData[t.nWin].pageName;
            const bool listed = page == "IComp" || page == "Compos" || page == "Phase" || page == "DComp" ||
                                page == "Kin.lower" || page == "Kin.upper" || page == "G0 shift";
            hs.captured.push_back({ page, listed ? pageEntryKey(page, t.nIdx) : "", t.nObj, t.nIdx, t.val, t.unit });
        }
        return false;
    }
    if (!hs.useRows)
        return false;
    tbData.clear();
    hs.skipped.clear();
    for (const auto& r : hs.rows) {
        size_t win = 0;
        while (win < wnData.size() && wnData[win].pageName != r.page) win++;
        if (win == wnData.size()) { hs.skipped.push_back(r.page + " (no such page) " + r.name); gui_logger->warn("bulk composition dialog has no page {} (pages: {})", r.page, wnData.size()); continue; }
        int nObj = r.nObj, nIdx = r.nIdx;
        if (!r.name.empty()) {
            nIdx = pageEntryIndex(r.page, r.name);
            if (nIdx < 0) { hs.skipped.push_back(r.page + " " + r.name); gui_logger->warn("bulk composition row not found: {} {}", r.page, r.name); continue; }
            nObj = wnData[win].nObj;
        }
        if (r.page == "Compos") gui_logger->info("compos row {} idx {} obj {} {} val {} unit {}", r.name, nIdx, nObj, aObj[nObj]->GetKeywd(), r.val, r.unit);
        tbData.push_back(tableSetupData(win, nObj, aObj[nObj]->GetKeywd(), nIdx, "", r.val, r.unit));
    }
    return true;
}

bool vfProcessSet(QWidget*, const char* p_key, char flgs[24], int size[8], short tabInt[6], double tab[24],
                  std::string& calcScript, std::string& outScript, TCStringArray& names, std::string& xName, std::string& yName)
{
    gui_logger->info("vfProcessSet {}", p_key);
    HeadlessScript& hs = HeadlessScript::get();
    if (!hs.useProc)
        return false;
    for (size_t i = 0; i < 24 && i < hs.procFlags.size(); i++)
        if (hs.procFlags[i] != ' ') flgs[i] = hs.procFlags[i];
    for (size_t i = 0; i < 7 && i < hs.procSize.size(); i++)
        if (hs.procSize[i] >= 0) size[i] = hs.procSize[i];
    for (size_t i = 0; i < 6 && i < hs.procTabInt.size(); i++)
        if (hs.procTabInt[i] >= 0) tabInt[i] = short(hs.procTabInt[i]);
    for (size_t i = 0; i < 24 && i < hs.procTable.size(); i++)
        if (hs.procTable[i] == hs.procTable[i]) tab[i] = hs.procTable[i];
    calcScript = hs.procCalc;
    outScript = hs.procOut;
    names = hs.procNames;
    xName = hs.procX;
    yName = hs.procY;
    return true;
}

bool vfGEM2MTSet(QWidget*, const char* p_key, char flgs[32], int size[20], double Tai[4], double Pai[4], double Tau[3],
                 std::string& calcScript, std::string& outScript, TCStringArray& names, std::string& xName, std::string& yName,
                 TCIntArray&, TCIntArray&)
{
    gui_logger->info("vfGEM2MTSet {}", p_key);
    HeadlessScript& hs = HeadlessScript::get();
    if (!hs.useG2m)
        return false;
    for (size_t i = 0; i < 32 && i < hs.g2mFlags.size(); i++)
        if (hs.g2mFlags[i] != ' ') flgs[i] = hs.g2mFlags[i];
    for (size_t i = 0; i < 20 && i < hs.g2mSize.size(); i++)
        if (hs.g2mSize[i] >= 0) size[i] = hs.g2mSize[i];
    for (size_t i = 0; i < 4 && i < hs.g2mTai.size(); i++) Tai[i] = hs.g2mTai[i];
    for (size_t i = 0; i < 4 && i < hs.g2mPai.size(); i++) Pai[i] = hs.g2mPai[i];
    for (size_t i = 0; i < 3 && i < hs.g2mTau.size(); i++) Tau[i] = hs.g2mTau[i];
    calcScript = hs.g2mCalc;
    outScript = hs.g2mOut;
    names = hs.g2mNames;
    xName = hs.g2mX;
    yName = hs.g2mY;
    return true;
}

bool vfUnSpaceSet(QWidget*, const char* p_key, char [38], int [10])
{
    gui_logger->info("vfUnSpaceSet {}", p_key);
    return false;
}

bool vfGtDemoSet(QWidget*, const char* p_key, char [16], int [8], std::string&, std::string&,
                 TCStringArray&, std::string&, std::string&, TCStringArray&)
{
    gui_logger->info("vfGtDemoSet {}", p_key);
    return false;
}

bool vfComposSet(QWidget*, const char* p_key, char [6], int [2], double&)
{
    gui_logger->info("vfComposSet {}", p_key);
    return false;
}

bool vfDCompSet(QWidget*, const char* p_key, char [15], int [4])
{
    gui_logger->info("vfDCompSet {}", p_key);
    return false;
}

bool vfReacDCSet(QWidget*, const char* p_key, char [12], int [4], TCStringArray&)
{
    gui_logger->info("vfReacDCSet {}", p_key);
    return false;
}

bool vfRTparmSet(QWidget*, const char* p_key, char [10], int [7], double [6],
                 std::string&, std::string&, std::string&, TCStringArray&)
{
    gui_logger->info("vfRTparmSet {}", p_key);
    return false;
}

bool vfPhaseSet(QWidget*, const char* p_key, char [37], int [30], double&,
                TCStringArray&, TCStringArray&, TCStringArray&)
{
    gui_logger->info("vfPhaseSet {}", p_key);
    return false;
}

bool vfDualThSet(QWidget*, const char* p_key, char [20], int [8])
{
    gui_logger->info("vfDualThSet {}", p_key);
    return false;
}

bool vfProjectSet(QWidget*, const char* p_key, char [38], int&)
{
    gui_logger->info("vfProjectSet {}", p_key);
    return false;
}

bool vfSystemSet(QWidget*, const char* p_key, char [40], std::string&, std::string&, std::string&)
{
    gui_logger->info("vfSystemSet {}", p_key);
    return false;
}

// For a scripted new project: the aqueous phase is the one of the database files (chosen from the list), the gas phase is the
// generated ideal gas mixture.
bool vfAutoPhaseSet(QWidget*, const char* pr_key, std::string& a_key,
                    std::string& g_key, char& acode, char& gcode, float [8])
{
    gui_logger->info("vfAutoPhaseSet {} {} {}", pr_key, a_key, g_key);
    if (HeadlessScript::get().projectKey.empty())
        return false;
    acode = 'U';   // the aqueous phase of the selected database files
    gcode = 'I';
    g_key = "g   GASMXID gas_gen         gm  Ideal           ";   // as AutoPhaseDialog
    return true;
}

bool vfLookupDialogSet(QWidget*, char flags[6], double [4], double [4])
{
    gui_logger->info("vfSystemSet {}", flags);
    return false;
}

void vfPhaseInfo(QWidget*, bool, int, std::string phname,
                 std::vector<int>&, std::vector<std::string>&, int)
{
    gui_logger->info("vfPhaseInfo {}", phname);
}


bool vfChooseFileOpen(QWidget*, std::string&, const char* title, const char* filter)
{
    gui_logger->info("vfChooseFileOpen {} {}", title, filter);
    return false;
}

bool vfChooseFileSave(QWidget*, std::string&, const char* title, const char* filter)
{
    gui_logger->info("vfChooseFileSave {} {}", title, filter);
    return false;
}

bool vfChooseDirectory(QWidget*, std::string&, const char* title)
{
    gui_logger->info("vfChooseDirectory {}", title);
    return false;
}

void vfMakeDirectory(QWidget* , const char *dir, int askOverwrite)
{
    fs::path ps(dir);
    if (fs::exists(ps))  {
        if(askOverwrite)  {
            fs::remove_all(ps);
        }
    }
    fs::create_directories(ps);
}

const char * dfAqKeyD =  "a   AQELIA  aq_gen          aq  Davies          ";
const char * dfAqKeyH =  "a   AQELIA  aq_gen          aq  EDH_H           ";
const char * dfAqKey3 =  "a   AQELIA  aq_gen          aq  EDH_K           ";
const char * dfAqKey2 =  "a   AQELIA  aq_gen          aq  DH_K            ";
const char * dfAqKey1 =  "a   AQELIA  aq_gen          aq  DH_LL           ";
const char * dfAqKeyY =  "a   AQELIA  aq_gen          aq  EDH_Y           ";
const char * dfAqKeyU =  "a   AQELSI  aq_spec         aq  User-Provided   ";
const char * dfGasKey =  "g   GASMXID gas_gen         gm  Ideal           ";
const char * dfFluKeyF=  "f   CG      fluid_gen       gm  CG_EoS          ";
const char * dfFluKey7=  "f   PR      fluid_gen       gm  PR_EoS          ";
const char * dfFluKeyP=  "f   PRSV    fluid_gen       gm  PRSV_EoS        ";
const char * dfFluKeyE=  "f   SRK     fluid_gen       gm  SRK_EoS         ";
const char * dfFluKeyU = "f   FLUID   fluid_spec      gm  User-Provided   ";

//----------------------------------------------------------------
// Help file entry point constants

const char *WZSTEP = "WZSTEP";
const char *MWPAGE = "MWPAGE";
const char *DBM = "DBM";
const char *NUMSET = "NUMSET";

// modules
const char *GM_COMPOS_WZ_HTML = "gm_compos";
const char *GM_COMPOS_HTML = "gm_compos";
const char *GM_ICOMP_HTML = "gm_icomp";          //m_icomp.cpp
const char *GM_DCOMP_HTML = "gm_dcomp";          //m_dcomp.cpp
const char *GM_DCOMP_WZ_HTML = "gm_dcomp";       //DCompWizard
const char *GM_REACDC_HTML = "gm_reacdc";        //m_reacdc.cpp
const char *GM_REACDC_WZ_HTML = "gm_reacdc";     //ReacDCWizard
const char *GM_RTPARM_HTML = "gm_rtparm";        //m_rtparm.cpp
const char *GM_RTPARM_WZ_HTML = "gm_rtparm";     //RTparmWizard
const char *GM_PHASE_HTML = "gm_phase";          //m_phase.cpp
const char *GM_PHASE_WZ_HTML = "gm_phase";       //PhaseWizard
const char *GM_SDREF_HTML = "gm_sdref";          //m_sdata.cpp
const char *GM_CONST_HTML = "gm_const";          //m_const.cpp
const char *GM_PROJECT_HTML = "gm_project";      //m_param.cpp
const char *GM_PROJECT_WZ_HTML = "gm_project";   //ProjectWizard
const char *GEMS_NUMSET_HTML = "gm_project";     //m_param.cpp
const char *GM_SYSEQ_HTML = "gm_syseq";          //m_syseq.cpp
const char *GEMS_SYSTEM_WZ_HTML = "gems_bcc";    //SystemWizard
const char *GM_PROCES_HTML = "gm_proces";        //m_proces.cpp
const char *GM_PROCES_WZ_HTML = "gm_proces";     //ProcesWizard
const char *GM_GTDEMO_HTML = "gm_gtdemo";        //m_gtdemo.cpp
const char *GM_GTDEMO_WZ_HTML = "gm_gtdemo";     //GtDemoWizard
const char *GM_GEM2MT_HTML = "gm_gem2mt";        //m_gem2mt.cpp
const char *GM_GEM2MT_WZ_HTML = "gm_gem2mt";     //GEM2MTWizard
const char *GM_UNSPACE_HTML = "gm_unspace";      //m_unspace.cpp
const char *GM_UNSPACE_WZ_HTML = "gm_unspace";   //UnSpaceWizard
const char *GM_DUALTH_HTML = "gm_dualth";        //m_dualth.cpp
const char *GM_DUALTH_WZ_HTML = "gm_dualth";     //DualThWizard

const char *GSM_SYSTEM_HTML = "gems_bcc";        //ms_system.cpp
const char *GSM_MTPARM_HTML = "gems_systd";      //ms_mtparm.cpp
const char *GSM_EQDEMO_HTML = "gems_sys";        //ms_calc.h
const char *GSM_MULTI_HTML =  "gems_ipm";        //ms_calc.h

// dialogs
//const char *GEMS_ABOUT_HTML = "gems_about";       // MainDialog
const char *GEMS_TUTOR_HTML = "gems_tutorial";    // MainDialog
const char *GEMS_SETUP_HTML = "gems_setup";       // SettingsDialog
const char *GEMS_TDBAS_HTML = "gems_tdbase";      // DBDialog
const char *GEMS_MISCL_HTML = "gems_miscel";      // DBAdvDialog
const char *GEMS_FORMT_HTML = "gems_miscel.html#FORMATS";   // DBAdvDialog
const char *GEMS_REKEY_HTML = "gems_rekey";       // KeyFilter
const char *GEMS_MPROJ_HTML = "gm_project.html#HOWTO";    // KeyProfile
const char *GEMS_MPROJ_LF_HTML = "gm_project.html#FILES"; // ListFilesDialog
const char *GEMS_ONESYS_HTML = "gems_sys";        // NewSystemDialog
const char *GEMS_HOWTO_HTML = "gems_sys.html#HOWTO";
const char *GEMS_BCC_HTML = "gems_bcc";
const char *GEMS_HOWTOB_HTML = "gems_bcc.html#HOWTO";
const char *GEMS_IPM_HTML = "gems_ipm";
const char *GEMS_RECIPE_HTML = "gems_bcc.html#RECIPE"; // InputSystemDialog
const char *GEMS_GRAPH_HTML = "gems_graph"; // GraphDialog
const char *GEMS_GRAPH_CUS_HTML = "gems_graph.html#CUSTOMIZE"; // LegendDialog
const char *GEMS_MISSING_HTML = "gems_bcc.html#MISSING";  // ExcludeFillDialog
const char *GEMS_MODES_HTML = "gm_project.html#MODES";   //EQDialog
const char *GEMS_ELEMENTS_HTML = "gems_elements"; // ElementsDialog
const char *GEMS_SETFILT_HTML = "gems_elements.html#FILTERS"; // SetFiltersDialog
const char *GEMS_APHSETUP_HTML = "gems_aphsetup"; // AutoPhaseDialog
const char *GEMS_TEMPL_HTML = "gems_miscel.html#TEMPLATES";  // ChangeKeyDialog
const char *GEMS_PHINFO_HTML = "gems_miscel.html#PHASEINFO";  // PhaseInfoDialog
const char *GEMS_WRIPMF_HTML = "gems_ipm.html#WRITEIPMFILES"; // LookupDialog
const char *GEMS_MARK_HTML = "gems_miscel.html#MARK"; // SelectDialog
const char *GEMS_SELECT_HTML = "gems_miscel.html#SELECT"; // KeyDialog
const char *GEMS_CALCUL_HTML = "gems_miscel.html#CALCULATOR";  // CalcDialog
// MainDialog
const char *GEMS_HOWTOSTART_HTML = "gems_about.html#PAGE_HOWTOSTART";
//const char *GEMS_ABOUT_HTML = "gems_about.html#PAGE_ABOUT";
const char *GEMS_AUTHORS_HTML = "gems_about.html#PAGE_AUTHORS";
const char *GEMS_THANKS_HTML = "gems_about.html#PAGE_THANKS";
const char *GEMS_LICENSE_HTML = "gems_about.html#PAGE_LICENSE";
// old
const char *GEMS_TOC_HTML = "gems_toc"; // default for modules
