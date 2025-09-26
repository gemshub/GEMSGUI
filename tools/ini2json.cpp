#include <iostream>
#include <iomanip>
#include <fstream>
#include <nlohmann/json.hpp>
#include "config.h"
#include "GEMS3K/v_detail.h"
#include "GEMS3K/v_service.h"

const char *VISOR_INI = "Resources/data/vis_cn.ini";
const char *OBJECT_INI = "Resources/data/vis_od.ini";
const char *UNITS_INI = "Resources/data/units.ini";
typedef signed char ObjType;

void TUnitsList_load(const std::string& f_units)
{
    TConfig cnf(f_units, ' ');
    std::string par;
    std::string str;

    auto units_array_object =  nlohmann::json::array();

    par = cnf.getFirst();
    while (!par.empty())
    {
        cnf.getcStr(str);
        auto units_list = split(str, "/");
        auto object =  nlohmann::json::object();
        object["values"] = units_list;
        object["label"] = par;
        units_array_object.push_back(object);
        par = cnf.getNext();
    }

    std::ofstream ojson(f_units+".json");
    ojson << std::setw(4) << units_array_object << std::endl;
}

void TObjList_load(const std::string& f_obj)
{
    TConfig cnf(f_obj,' ');

    std::string par;
    int N;
    int M;
    short i;
    ObjType objectType = 0;
    std::string astr[6];

    auto objects_array =  nlohmann::json::array();

    par = cnf.getFirst();
    while( !par.empty() )
    {
        auto object =  nlohmann::json::object();
        cnf.getcStrings(6, astr);
        sscanf( astr[2].c_str(),"%d", &N);
        sscanf( astr[3].c_str(),"%d", &M);

        object["id"] = par;
        object["label"] = astr[0];
        if( is( i, astr[1] ))
        {
            objectType = i;
            object["type"] = objectType;
        }
        else
        {
            object["type"] = astr[1];
        }
        object["dimN"] = abs(N);
        object["dimM"] = M;
        object["dyna"] = N<0;
        object["ndxs"] = astr[4].substr(0,1);
        object["hint"] = astr[5];

        objects_array.push_back(object);
        par = cnf.getNext();
    }

    std::ofstream ojson(f_obj+".json");
    ojson << std::setw(4) << objects_array << std::endl;
}


nlohmann::json PageInfo_load(TConfig& cnf)
{
    // object type len p1 p2 abcd
    std::string obj;
    std::string mode;
    std::string astr[5];
    int npos;

    auto page_ini_array =  nlohmann::json::array();

    obj = cnf.getFirst();
    while (!obj.empty())
    {
        cnf.getcStrings(5, astr);
        int maxm, maxn;
        if (sscanf(astr[2].c_str(), "%d", &maxm) != 1)
            throw TError(obj.c_str(), "Bad field width");
        if (sscanf(astr[3].c_str(), "%d", &maxn) != 1)
            throw TError(obj.c_str(), "Bad field heigth");


        auto object =  nlohmann::json::object();
        object["label"] = obj;
        object["type"] = astr[0];
        if( astr[0] != "F_CHECKBOX" && is( npos, astr[1] ))
        {
            object["lenth"] = npos;
        }
        else
        {
            object["lenth"] = astr[1];
        }
        object["maxM"] = maxm;
        object["maxN"] = maxn;
        object["locus"] = astr[4];

        page_ini_array.push_back(object);
        obj = cnf.getNext();
    }
    return page_ini_array;
}

void CWinInfo_load(const std::string& f_win)
{
    TConfig cnf(f_win,' ');
    int init_width;
    int init_height;
    std::string par;
    std::string astr[6];

    auto wininfo_map =  nlohmann::json::object();

    par = cnf.readSectionName();

    while( !par.empty() )
    {
        auto win_obj =  nlohmann::json::object();
        //std::cout << par << std::endl;

        std::string size_params[2];
        if (cnf.getFirst() == "size")
        {
            cnf.getcStrings(2, size_params);

            if (sscanf(size_params[0].c_str(), "%d", &init_width) != 1
                    || sscanf(size_params[1].c_str(), "%d", &init_height) != 1)
                Error("Visor configuration", "Window size is bad!");

            win_obj["size"]["width"] = init_width;
            win_obj["size"]["height"] = init_height;
        }

        auto pages_array =  nlohmann::json::array();
        std::string ss = cnf.GetFirstSubSection();
        while (!ss.empty())
        {
            auto page_obj =  nlohmann::json::object();
            page_obj["page"] = ss;
            page_obj["fields"] = PageInfo_load(cnf);

            pages_array.push_back(page_obj);
            ss = cnf.GetNextSubSection();
        }

        win_obj["pages"] = pages_array;
        wininfo_map[par] = win_obj;
        par = cnf.readSectionName();
    }

    std::ofstream ojson(f_win+".json");
    ojson << std::setw(4) << wininfo_map << std::endl;
}


int main(int argc, char* argv[])
{
    std::string dir_path;

    if( argc>1 ) {
        dir_path = argv[1];
        dir_path += "/";
    }

    try
    {
        TUnitsList_load(dir_path+UNITS_INI);
        TObjList_load(dir_path+OBJECT_INI);
        CWinInfo_load(dir_path+VISOR_INI);
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

    std::cout << "finished convert" << std::endl;
    return 0;
}
