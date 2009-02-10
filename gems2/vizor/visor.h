//-------------------------------------------------------------------
// $Id$
//
// Declaration of TVisor class, setup and config functions
//
// Copyright (C) 1996-2001 A.Rysin,S.Dmytriyeva,D.Kulik
// Uses  gstring class (C) A.Rysin 1999
//
// This file is part of the GEM-Vizor library which uses the
// Qt v.2.x GUI Toolkit (Troll Tech AS, http://www.trolltech.com)
// according to the Qt Duo Commercial license
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL)
//
// See http://gems.web.psi.ch/ for more information
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

    gstring LocalDir;

    gstring LocalDocDir;
    gstring RemoteDocURL;
    bool LocalDoc;

    gstring ImgDir;

    bool isElementsProfileMode;
    bool dbChangeMode;


    void initModules();
    void addModule(TCModule* pm, bool selectFiles=false);

    void load();
    void fromDAT( bool option_c, bool option_v );
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

    TVisor(int argc, char* argv[]);
    ~TVisor();

    void Update(bool force=true);
    void CalcFinished();

    bool isDBChangeMode() const { return dbChangeMode; }

    gstring sysGEMDir() const {
        return SysGEMDir;
    }
    
    gstring sysDBDir() const {
        return SysGEMDir + DefDBDir;
    }
    
    gstring sysProfDir() {
        return SysGEMDir + DefProfDir;
    }
    
    gstring userGEMDir() const {
        return UserGEMDir;
    }
    
    gstring userProfDir() const {
        return UserGEMDir + UserProfDir;
    }

    gstring imgDir() {
        return SysGEMDir + ImgDir;
    }

    const gstring& localDir() const {
        return LocalDir;
    }

    gstring docDir() const {
	return (LocalDoc) ? LocalDocDir : RemoteDocURL;
//        return SysGEMDir + DocDir;
    }

    const gstring& localDocDir() const {
        return LocalDocDir;
    }

    const gstring& remoteDocURL() const {
        return RemoteDocURL;
    }

    bool localDoc() const {
        return LocalDoc;
    }

    void setLocalDir(const gstring& localDir) {
        LocalDir = localDir;
    }

    void setLocalDocDir(const gstring& localDir) {
        LocalDocDir = localDir;
    }

    void setRemoteDocURL(const gstring& remoteURL) {
        RemoteDocURL = remoteURL;
    }

    void setLocalDoc(bool local) {
        LocalDoc = local;
    }


    void deleteDBDir( const char * dir);
    void CopyF( const char * fName, const char* fTempl );
    void makeDBDir(const char *dir);
    TCStringArray readPDBDir(const char *dir, const char *filter );

    void OpenHelp(const char* file, const char* item=0, QWidget* parent=0);
    void OpenModule(QWidget* parent, int i, int page=0, bool viewmode=false);
    void OpenProgress(bool step=false);
    void CloseProgress();
    bool Message( QWidget* parent, const char* name,
             const char* msg, int prog =0, int total=-1);
    void ProcessProgress( QWidget* parent );
    void CloseMessage();


    bool getElemPrMode() const
    {
        return isElementsProfileMode;
    }
    void setElemPrMode(bool newData)
    {
        isElementsProfileMode = newData;
    }

};

extern TVisor* pVisor;
//extern TVisorImp* pVisorImp;

#endif   // _visor_h
