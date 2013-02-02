// utility visod_to_json: converts GEMS DOD configurator into JSON-like format
// (C) 2013, D.Kulik
//
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cctype>

using namespace std;

int main()
{
    ifstream ini( "/Users/kulik/DevGEMS/devPhase/visod_to_json_build/vis_od.ini" );
    ofstream json( "/Users/kulik/DevGEMS/devPhase/visod_to_json_build/vis_od.json" );
    char buf[512];

    if(!ini.good())
    {
        cout << "visod_to_json: vis_od.ini input file not found!" << endl;
        return 1;
    }
    cout << "visod_to_json: working...";

    while (ini.good())
    {
       ini >> buf;
       if( strncmp( buf, "o_", 2 ) == 0 )
       {
           json << endl << "{ \"id\": " << buf << " , \"label\": \"";
           ini >> buf;
           json << buf << "\" ,  \"dotype\": \"";
           ini >> buf;
           json << buf << "\" , \"dimN\": ";
           ini >> buf;
           json << buf << " , \"dimM\": ";
           ini >> buf;
           json << buf << " , \"ndxs\": \"";
           ini >> buf;
           json << buf << "\" , \"values\": [  ] },";
           continue;
       }
    }
    json.close();
    ini.close();
    cout << " Done!" << endl;
    return 0;
}

