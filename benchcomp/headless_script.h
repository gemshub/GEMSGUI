// Scripted answers for the dialogs that benchcomp tools cannot show. A tool fills the fields it needs; a dialog
// mock in service_new.cpp reads them. With the fields empty the mocks answer "cancelled", as before.
#ifndef HEADLESS_SCRIPT_H
#define HEADLESS_SCRIPT_H

#include <map>
#include <string>
#include <vector>

struct HeadlessScript
{
    // New Modelling Project with the element dialog (Basis configuration, step 1 and 2)
    std::string projectKey;                 // key of the new project record, e.g. "TestTHERE:comment:"
    std::string tdbFilter;                  // built-in database files taken: those named <type>.<tdbFilter>.*
    std::vector<std::string> elements;      // independent components taken, by name (Na, Cl, ... N_atm)

    // Key answered to the next "enter a new record key" dialog of a module other than the project
    std::string nextKey;

    // Bulk composition dialog (Create a new equilibrium): rows to put in the table
    struct Row {
        std::string page;       // window name: Compos, DComp, IComp, Phase, Kin.lower, Kin.upper, G0 shift, Targets, Other Inputs
        std::string name;       // element/species/phase key text (leading part, blanks stripped); empty for scalars
        int nObj = -1;          // object index, scalars only
        int nIdx = 0;           // index in the object, scalars only
        double val = 0.;
        char unit = '_';
    };
    std::vector<Row> rows;
    bool useRows = false;
    std::vector<std::string> skipped;   // rows that found no entry
    // species of the project that the records come from: first 28 characters of the DC key -> formula
    std::map<std::string, std::string> oldDcFormula;

    // Bulk composition dialog in capture mode: the table the dialog was given
    struct Captured { std::string page; std::string name; int nObj; int nIdx; double val; char unit; };
    std::vector<Captured> captured;
    bool capture = false;

    // Process creation dialog (Process wizard): values replace those the record has when not empty / not -1
    std::string procFlags;                  // 24 characters, ' ' keeps the value of the record
    std::vector<int> procSize;              // 7 sizes: Nxi, Nmc, dimXY[1], dimEF[0], dimEF[1], Nsd, dimX; -1 keeps
    std::vector<int> procTabInt;            // 6 integers (tmi, NVi: start, end, step); negative keeps
    std::vector<double> procTable;          // 24 doubles (Pi, Ti, Vi, Taui, pXii, Nui, pHi, pei, 3 each); NaN keeps
    std::string procCalc, procOut, procX, procY;
    std::vector<std::string> procNames;
    bool useProc = false;

    // GEM2MT creation dialog (GEM2MT wizard)
    std::string g2mFlags;                   // 32 characters, ' ' keeps
    std::vector<int> g2mSize;               // 20 sizes (nC, nIV, ntM, nPG, nFD, nSFD, nYS, nE, nYE, Lbi, nEl, Nqpg, nPai, nTai, Nsd, ...)
    std::vector<double> g2mTai, g2mPai, g2mTau;   // 4, 4, 3
    std::string g2mCalc, g2mOut, g2mX, g2mY;
    std::vector<std::string> g2mNames;
    bool useG2m = false;
    bool replaceExisting = false;           // "Replace the record?" is answered yes (JSON import)

    static HeadlessScript& get();
};

/// Index of the entry of a bulk-composition page (IComp, Compos, Phase, DComp) whose record key starts like `name`, or -1.
int headlessPageEntryIndex(const std::string& page, const std::string& name);

/// Stoichiometry text of DC j of the open project, e.g. "Ca:1,Na:2,O:8,S:2,Zz:0"; equal text is the same species.
std::string headlessDcComposition(int j);

#endif
