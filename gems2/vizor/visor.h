//-------------------------------------------------------------------
// Id: gems/vizor/visor.h  version 2.0.0    2001
//
// Declaration of TVisor class, setup and config functions
//
// Created : 970207    Modified: 010908
//
// Copyright (C) 1996-2001 A.Rysin,S.Dmytriyeva,D.Kulik
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
//
#ifndef _visor_h_
#define _visor_h_

#include "gstring.h"
using namespace std;

#include "v_module.h"
class QWidget;

class TVisor
{
    friend class TVisorImp;

    int argc;
    char** argv;

    gstring SysGEMDir;
    gstring DefDBDir;
    gstring DefProfDir;
    gstring UserGEMDir;
    gstring UserProfDir;

    gstring DocDir;
    gstring ImgDir;

    void initModules();
    void addModule(TCModule* pm, bool selectFiles=false);

    void load();
    void fromDAT( bool op_c );
    void toDAT();
    void defaultCFG();
    void toModCFG();
    void fromModCFG();
    void toWinCFG();
    void fromWinCFG();

    void Exit();

protected:
    bool CanClose();
    void Setup();

public:
    bool ProfileMode;
    bool DBChangedMode;

    TVisor(int argc, char* argv[]);
    ~TVisor();

    void Update(bool force=true);
    void CalcFinished();

    const gstring sysGEMDir()
    {
        return SysGEMDir;
    }
    const gstring sysDBDir()
    {
        return SysGEMDir + DefDBDir;
    }
    const gstring sysProfDir()
    {
        return SysGEMDir + DefProfDir;
    }
    const gstring userGEMDir()
    {
        return UserGEMDir;
    }
    const gstring userProfDir()
    {
        return UserGEMDir + UserProfDir;
    }

    const gstring imgDir()
    {
        return SysGEMDir + ImgDir;
    }
    const gstring docDir()
    {
        return SysGEMDir + DocDir;
    }

    void deleteDBDir( const char * dir);
    void CopyF( const char * fName, const char* fTempl );
    void makeDBDir(const char *dir);
    TCStringArray readPDBDir(const char *dir, const char *filter );

    void OpenHelp(const char* file, const char* item=0, QWidget* parent=0);
    void OpenModule(QWidget* parent, int i, int page=0, bool viewmode=false);
    void OpenProgress(bool step=false);
    void CloseProgress();
    void Message( QWidget* parent, const char* name,
             const char* msg, int prog =0, int total=-1);
    void CloseMessage();
};

extern TVisor* pVisor;
extern TVisorImp* pVisorImp;

#endif   // _visor_h
