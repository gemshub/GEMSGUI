//-------------------------------------------------------------------
// $Id: visor.h 1223 2009-02-10 15:09:29Z gems $
//
// Declaration of TVisor class, setup and config functions
//
// Copyright (C) 1996-2001 A.Rysin,S.Dmytriyeva,D.Kulik
//
// This file is part of the GEM-Selektor GUI library which uses the
// Qt v.4 cross-platform App & UI framework (https://qt.io/download-open-source)
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

#include "v_module.h"

class QWidget;

// new enums
enum { MDD_DATABASE=0, MDD_SYSTEM=1 };

class TVisor
{
    friend class TVisorImp;

    int argc;
    char** argv;

    std::string lastProjectKey;
    std::string lastSystemKey;

    std::string SysGEMDir;
    std::string DefDBDir;
    std::string DefProfDir;
    std::string UserGEMDir;
    std::string UserProfDir;
    std::string LocalDir;

    // Path to gems3_server executable
    // <ServerGems3Dir>/server_data/toServer-dat.lst - path to gems3 server data
    //std::string ServerGems3Dir;

    std::string LocalDocDir;
    std::string RemoteHTML;
    bool LocalDoc;         // obsolete
    std::string ImgDir;

    bool isElementsProfileMode;
    std::string DefaultBuiltinTDB;
    bool dbChangeMode;
    bool configAutosave = false;

    void initModules();
    template <class T>
    void addModule(T* pm, bool selectFiles=false)
    {
        aMod.push_back( std::shared_ptr<T>(pm));
        pm->ods_link();
        //  pm->dyn_set();

        TCIntArray arr;
        if (selectFiles)
            arr = pm->SelectFileList(openf | closef);
        rt[pm->rtNum()]->Open(selectFiles, UPDATE_DBV, arr);
        rt[pm->rtNum()]->SetKey(ALLKEY);
    }

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


public:
    int ProfileMode;

    TVisor(int argc, char* argv[]);
    ~TVisor();

    void Setup();
    bool isDBChangeMode() const {
        return dbChangeMode;
    }

    std::string sysGEMDir() const {
        return SysGEMDir;
    }
    
    std::string sysDBDir() const {
        return SysGEMDir + DefDBDir;
    }
    
    std::string sysProfDir() {
        return SysGEMDir + DefProfDir;
    }
    
    std::string userGEMDir() const {
        return UserGEMDir;
    }
    
    std::string userProfDir() const {
        return UserGEMDir + UserProfDir;
    }

    std::string imgDir() {
        return SysGEMDir + ImgDir;
    }

    const std::string& localDir() const {
        return LocalDir;
    }

    std::string docDir() const {
        return LocalDocDir;
        //	return (LocalDoc) ? LocalDocDir : RemoteDocURL;
    }

    const std::string& localDocDir() const {
        return LocalDocDir;
    }

    const std::string& remoteHTML() const {
        return RemoteHTML;
    }

    bool localDoc() const { // obsolete
        return LocalDoc;
    }

    void setLocalDir(const std::string& localDir) {
        LocalDir = localDir;
    }

    void setLocalDocDir(const std::string& localDir) {
        LocalDocDir = localDir;
    }

    void setRemoteHTML(const std::string& remoteURL) {
        RemoteHTML = remoteURL;
    }

    void setLocalDoc(bool local) { // obsolete
        LocalDoc = local;
    }

    // const std::string& serverGems3Dir() const {
    //     return ServerGems3Dir;
    // }

    bool getElemPrMode() const {
        return isElementsProfileMode;
    }
    void setElemPrMode(bool newData) {
        isElementsProfileMode = newData;
    }

    const std::string& defaultBuiltinTDBL() const {
        return DefaultBuiltinTDB;
    }
    void setDefaultBuiltinTDB(const std::string& aDefaultBuiltinTDB) {
        DefaultBuiltinTDB = aDefaultBuiltinTDB;
    }

    void setConfigAutosave(bool autosave)
    {  configAutosave = autosave;  }
    bool getConfigAutosave() const
    {  return configAutosave;   }

    int getDoubleDigits() const
    {  return TValBase::doublePrecision;  }
    void setDoubleDigits(int newDoubleDigits)
    {  TValBase::doublePrecision = newDoubleDigits;   }


    /// Generate full path to current directory from filename and extension
    std::string filePathFromName(const std::string& filename, const std::string& extension);

    void deleteDBDir(const char * dir);
    void CopyF( const char * fName, const char* fTempl );
    void makeDBDir(const char *dir);
    TCStringArray readPDBDir(const char *dir, const char *filter );

    // GUI
    QWidget* window();
    void Update(bool force=true);
    void OpenModule(QWidget* parent, uint i, int page=0, int viewmode=0, bool select=false);
    bool Message( QWidget* parent, const char* name,
                 const char* msg, int prog =0, int total=-1, bool move = false);
    void ProcessProgress( QWidget* parent, int nRT );
    void CloseMessage();
    void defineModuleKeysList(size_t nRT);
};

extern TVisor* pVisor;

#endif   // _visor_h
