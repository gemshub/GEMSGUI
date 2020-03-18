//-------------------------------------------------------------------
// $Id: visor.h 1223 2009-02-10 15:09:29Z gems $
//
// Declaration of TVisor class, setup and config functions
//
// Copyright (C) 1996-2001 A.Rysin,S.Dmytriyeva,D.Kulik
// Uses  gstring class (C) A.Rysin 1999
//
// This file is part of the GEM-Selektor GUI library which uses the
// Qt v.4 cross-platform App & UI framework (http://qt.nokia.com)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the GPL v.3 license

//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------
//
#ifndef _visor_h_
#define _visor_h_

#include "gstring.h"
#include "v_module.h"

class QWidget;

class TVisor
{
    friend class TVisorImp;

    int argc;
    char** argv;

    gstring lastProjectKey;
    gstring lastSystemKey;

    gstring SysGEMDir;
    gstring DefDBDir;
    gstring DefProfDir;
    gstring UserGEMDir;
    gstring UserProfDir;
    gstring LocalDir;

    /// Path to gems3_server executable
    /// <ServerGems3Dir>/server_data/toServer-dat.lst - path to gems3 server data
    gstring ServerGems3Dir;


    gstring LocalDocDir;
    gstring RemoteHTML;
    bool LocalDoc;         // obsolete

    gstring ImgDir;

    bool isElementsProfileMode;
    gstring DefaultBuiltinTDB;
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
    int ProfileMode;

    TVisor(int argc, char* argv[]);
    ~TVisor();

    void Update(bool force=true);
//    void CalcFinished();

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
        return LocalDocDir;
     //	return (LocalDoc) ? LocalDocDir : RemoteDocURL;
    }

    const gstring& localDocDir() const {
        return LocalDocDir;
    }

    const gstring& remoteHTML() const {
        return RemoteHTML;
    }

    bool localDoc() const { // obsolete
        return LocalDoc;
    }

    void setLocalDir(const gstring& localDir) {
        LocalDir = localDir;
    }

    void setLocalDocDir(const gstring& localDir) {
        LocalDocDir = localDir;
    }

    void setRemoteHTML(const gstring& remoteURL) {
        RemoteHTML = remoteURL;
    }

    void setLocalDoc(bool local) { // obsolete
        LocalDoc = local;
    }

    const gstring& serverGems3Dir() const {
        return ServerGems3Dir;
    }


    void deleteDBDir( const char * dir);
    void CopyF( const char * fName, const char* fTempl );
    void makeDBDir(const char *dir);
    TCStringArray readPDBDir(const char *dir, const char *filter );

    //void OpenHelp(const char* file, const char* item=0, int page =-1 );
    void OpenModule(QWidget* parent, uint i, int page=0, int viewmode=0, bool select=false);
    bool Message( QWidget* parent, const char* name,
             const char* msg, int prog =0, int total=-1, bool move = false);
    void ProcessProgress( QWidget* parent, int nRT );
    void CloseMessage();


    bool getElemPrMode() const
    {
        return isElementsProfileMode;
    }
    void setElemPrMode(bool newData)
    {
        isElementsProfileMode = newData;
    }

    const gstring& defaultBuiltinTDBL() const {
        return DefaultBuiltinTDB;
    }
    void setDefaultBuiltinTDB(const gstring& aDefaultBuiltinTDB) {
        DefaultBuiltinTDB = aDefaultBuiltinTDB;
    }


    QWidget* window();
};

extern TVisor* pVisor;
//extern TVisorImp* pVisorImp;

#endif   // _visor_h
