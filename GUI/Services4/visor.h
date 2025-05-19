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

class TVisor
{
    friend class TVisorImp;

    int argc;
    char** argv;

    string lastProjectKey;
    string lastSystemKey;

    string SysGEMDir;
    string DefDBDir;
    string DefProfDir;
    string UserGEMDir;
    string UserProfDir;
    string LocalDir;

    // Path to gems3_server executable
    // <ServerGems3Dir>/server_data/toServer-dat.lst - path to gems3 server data
    //string ServerGems3Dir;

    string LocalDocDir;
    string RemoteHTML;
    bool LocalDoc;         // obsolete

    string ImgDir;

    bool isElementsProfileMode;
    string DefaultBuiltinTDB;
    bool dbChangeMode;


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
    void Setup();


public:
    int ProfileMode;

    TVisor(int argc, char* argv[]);
    ~TVisor();

    void Update(bool force=true);

    bool isDBChangeMode() const { return dbChangeMode; }

    string sysGEMDir() const {
        return SysGEMDir;
    }
    
    string sysDBDir() const {
        return SysGEMDir + DefDBDir;
    }
    
    string sysProfDir() {
        return SysGEMDir + DefProfDir;
    }
    
    string userGEMDir() const {
        return UserGEMDir;
    }
    
    string userProfDir() const {
        return UserGEMDir + UserProfDir;
    }

    string imgDir() {
        return SysGEMDir + ImgDir;
    }

    const string& localDir() const {
        return LocalDir;
    }

    string docDir() const {
        return LocalDocDir;
     //	return (LocalDoc) ? LocalDocDir : RemoteDocURL;
    }

    const string& localDocDir() const {
        return LocalDocDir;
    }

    const string& remoteHTML() const {
        return RemoteHTML;
    }

    bool localDoc() const { // obsolete
        return LocalDoc;
    }

    void setLocalDir(const string& localDir) {
        LocalDir = localDir;
    }

    void setLocalDocDir(const string& localDir) {
        LocalDocDir = localDir;
    }

    void setRemoteHTML(const string& remoteURL) {
        RemoteHTML = remoteURL;
    }

    void setLocalDoc(bool local) { // obsolete
        LocalDoc = local;
    }

    // const string& serverGems3Dir() const {
    //     return ServerGems3Dir;
    // }

    /// Generate full path to current directory from filename and extension
    std::string filePathFromName( const std::string& filename, const std::string& extension );

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

    const string& defaultBuiltinTDBL() const {
        return DefaultBuiltinTDB;
    }
    void setDefaultBuiltinTDB(const string& aDefaultBuiltinTDB) {
        DefaultBuiltinTDB = aDefaultBuiltinTDB;
    }


    QWidget* window();
};

extern TVisor* pVisor;
//extern TVisorImp* pVisorImp;

#endif   // _visor_h
