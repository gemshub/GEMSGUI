#include <iostream>
#include "visor.h"
#include "m_param.h"

int main(int argc, char *argv[])
{
    try{

    auto pVisor_sh = std::make_shared<TVisor>(argc, argv);
    pVisor = pVisor_sh.get();
    pVisor->Setup();

    std::string profile_key = "Kaolinite Test-JNC                              ";
    if( !TProfil::pm->initCalcMode(profile_key.c_str()) )
    {
        pVisor->ProfileMode = MDD_DATABASE;
        std::cout << "Error when read project: " << profile_key;
        return 1;
    }


        // Generate data from process
        TCStringArray savedSystems;

        // Save systems
        std::string systemsPath = pVisor->localDir()+"out_Standalone/";
        vfMakeDirectory( nullptr, systemsPath.c_str(), 2 );
        TProfil::pm->allSystems2GEMS3K( savedSystems, true, systemsPath, true, false);
    }
    catch( TError& xcpt )
    {
        std::cout << xcpt.title << " " << xcpt.mess << std::endl;
        gui_logger->error("Record out error: {}", xcpt.mess);
    }
    return 0;
}
