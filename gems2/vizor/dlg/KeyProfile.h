//-------------------------------------------------------------------
// Id: gems/vizor/dlg/KeyProfile.h  version 2.0.0    2001
//
// Declaration of KeyProfile class
//
// Created : 970207    Modified: 010908
//
// Copyright (C) 1996-2001  A.Rysin, S.Dmytriyeva
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

#ifndef KeyProfile_included
#define KeyProfile_included

#include "KeyProfileData.h"
#include "gstring.h"

//using namespace std;

class KeyProfile : public KeyProfileData
{
    Q_OBJECT

    int iRt;
    bool newKey;

protected slots:
    void CmNew();
    void CmHelp();  // 05.01.01

public:

    KeyProfile(QWidget* win, int irt, const char* caption = 0);
    virtual ~KeyProfile();

    gstring getKey();
    bool   getAqGasState();
    bool   getFilesState();
};
#endif // KeyProfile_included


