#include <iostream>
#include "help.h"
#include "GEMS3K/v_detail.h"


int main(int argc, char* argv[])
{
    std::string dir_path;

    if( argc>1 ) {
        dir_path = argv[1];
        dir_path += "/";
    }

    try
    {
        HelpConfigurator qhp_generator;
        qhp_generator.readDir(dir_path);
        qhp_generator.writeFile();
    }
    catch(const TError& err)
    {
        std::cerr << "gems3 error: " << err.title << " : " <<err.mess << std::endl;
    }
    catch(std::exception& e)
    {
        std::cout  << "std::exception: " << e.what() << std::endl;
    }
    catch(...)
    {
        std::cerr << "Unknown exception: program aborted" << std::endl;
        return -1;
    }

    std::cout << "finished generation" << std::endl;
    return 0;
}
