//masstransport
//-------------------------------------------------------------------


#include "m_param.h"

int
main(int argc, char* argv[])
{
    TProfil task_;
    try
    {
      gstring fin = "input_multi.txt";
      gstring fout = "out_multi.txt";

      // from argv
      if (argc >= 2 )
        fin = argv[1];
      if (argc >= 3 )
        fout = argv[2];

      fstream f_in(fin.c_str(), ios::in|ios::binary);
      ErrorIf( !f_in.good() , fin.c_str(), "Fileopen error");
      task_.readMulti(f_in);

      //calc part
      task_.calcMulti();

      fstream f_out(fout.c_str(), ios::out|ios::binary);
      ErrorIf( !f_out.good() , fout.c_str(), "Fileopen error");
      task_.outMulti(f_out);

      task_.multi->multi_free();
      return 1;
    }
    catch(TError& err)
    {
        cout << err.title.c_str() << "  : " << err.mess.c_str();
    }
    catch(...)
    {
        return -1;
    }
    return 0;
}

//--------------------- End of main.cpp ---------------------------

