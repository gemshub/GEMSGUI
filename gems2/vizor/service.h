//-------------------------------------------------------------------
// $Id$
//
// Declaration of  some GUI service functions
//
// Copyright (C) 1996-2001  A.Rysin, S.Dmytriyeva
// Uses  gstring class (C) A.Rysin 1999
//
// This file is part of the GEM-Vizor library which uses the
// Qt v.2.x GUI Toolkit (Troll Tech AS, http://www.trolltech.com)
// according to the Qt Duo Commercial license
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL)
//
// See http://les.web.psi.ch/Software/GEMS-PSI/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#ifndef _service_h
#define _service_h

#include "array.h"
#include "gstring.h"
#include "v_user.h"

class QWidget;
/// remove numbers!
const int VF_YES =6;
const int VF_NO = 7;
const int VF_CANCEL = 2;

int vfQuestYesNoCancel(QWidget* par, const gstring& title, const gstring& mess);

bool vfQuestion(QWidget* par, const gstring& title, const gstring& mess);

/// remove numbers!
const int VF3_1 = 6;
const int VF3_2 = 7;
const int VF3_3 = 2;

// returns VF3_1, VF3_2 or VF3_3

int vfQuestion3(QWidget* par, const gstring& title, const gstring& mess,
                const gstring& s1, const gstring& s2,  const gstring& s3="&Cancel",
                bool i_mov = false);

enum WarnType { vfInfo=0, vfWarn, vfErr };

void
vfMessage(QWidget* par, const gstring& title, const gstring& mess, WarnType type=vfWarn);

int
vfChoice(QWidget* par, TCStringArray& arr, const char* prompt, int sel=0);
int
vfChoice2(QWidget* par, TCStringArray& arr, const char* prompt,
               int sel, bool& all_);

TCIntArray
vfMultiChoice(QWidget* par, TCStringArray& arr, const char* prompt);

TCIntArray
vfMultiChoiceSet(QWidget* par, TCStringArray& arr, const char* prompt, TCIntArray& sel);

TCStringArray
vfMultiKeys(QWidget* par, const char* caption,
        int iRt, const char* key );

TCStringArray
vfMultiKeysSet(QWidget* par, const char* caption,
        int iRt, const char* key, TCStringArray& sel );

bool
vfListFiles(QWidget* par, bool show_dlg, const char * prfName,
                TCStringArray& fls, TCIntArray& cnt );

struct elmWindowData;
struct setFiltersData;
bool
vfElements(QWidget* par, const char * prfName,
           elmWindowData& elm_data, setFiltersData& sf_data );

bool
vfProcessSet(QWidget* par, const char * p_key,
              char flds[24], int size[6] );
bool
vfComposSet(QWidget* par, const char * p_key,
            char flgs[6], int sizes[2], double& r2 );

bool
vfPhaseSet(QWidget* par, const char * p_key,
            char flgs[12], int sizes[6], double& r2 );


bool
 vfAutoPhaseSet(QWidget* par, const char* pr_key, gstring& a_key, gstring& g_key,
        char& acode, char& gcode, float apar[4], float gpar[4] );

gstring
vfKeyEdit(QWidget* par, const char* title, int iRt, const char* key=0);

gstring
vfKeyProfile(QWidget* par, const char* title, int iRt,
        bool& chAqGas, bool& addFiles,
    bool& remake, gstring& key_templ  );

gstring
vfKeyTemplEdit(QWidget* par, const char* title, int iRt, const char* key=0,
               bool allowTemplate=true);
bool
vfKeyCanged(QWidget* par, const char* caption,
            gstring& from_str, gstring& to_str );

bool
vfExcludeFillEdit(QWidget* par, const char* caption,
   TCStringArray& aICkeys, TOArray<bool>& sel, double& fill_data );


bool vfChooseFileOpen(QWidget* par, gstring& path, const char* title=0);
bool vfChooseFileSave(QWidget* par, gstring& path,
  const char* title=0 , const char* filter=0);

class TObject;
void vfObjToFile(QWidget* par, TObject* obj);

#endif // _service_h

