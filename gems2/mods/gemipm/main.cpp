//masstransport
//-------------------------------------------------------------------

#include "verror.h"
#include "m_param.h"

// read string as: "<characters>",
istream&
f_getline(istream& is, gstring& str, char delim)
{
    char ch;
    is.get(ch);
    str="";

    while( is.good() && ( ch==' ' || ch=='\n' || ch== '\t') )
        is.get(ch);
    if(ch == '\"')
        is.get(ch);
    while( is.good() &&  ch!=delim && ch!= '\"' )
    {
        str += ch;
        is.get(ch);
    }
    while( is.good() &&  ch!=delim )
            is.get(ch);

   return is;
}



int
main(int argc, char* argv[])
{
    TProfil task_;
    try
    {
     gstring multu_in = "input_multi.txt";
     gstring chbr_in   = "mtr_data.txt";

      // from argv
      if (argc >= 2 )
        multu_in = argv[1];
      if (argc >= 3 )
        chbr_in = argv[2];

// read multi structure
      GemDataStream f_m(multu_in, ios::in|ios::binary);
      task_.readMulti(f_m);

// read dataCH structure, and some databr structures
// mtr_data structure is:
//   "<dataCH file name>" ,"<dataBR file1 name>", ..., "<dataBR fileN name>"
// Get DataCH file name
      fstream f_chbr(chbr_in.c_str(), ios::in );
      ErrorIf( !f_chbr.good() , chbr_in.c_str(), "Fileopen error");

      gstring datachbr_file;
      f_getline( f_chbr, datachbr_file, ',');
// Read dataCH file
      GemDataStream f_ch(datachbr_file, ios::in|ios::binary);
      task_.multi->datach_from_file(f_ch);

// for all databr files
      while( !f_chbr.eof() )
      {
// read and unpack dataBR structure
         f_getline( f_chbr, datachbr_file, ',');
         GemDataStream in_br(datachbr_file, ios::in|ios::binary);
         task_.multi->databr_from_file(in_br);
         task_.multi->unpackDataBr();

//calc part
         task_.calcMulti();

//output resalts

         task_.multi->packDataBr();
         GemDataStream out_br(datachbr_file, ios::out|ios::binary);
         task_.multi->databr_to_file(out_br);

        }
//test resalts
      gstring out_s = "multi_out.txt";
      GemDataStream o_m( out_s, ios::out|ios::binary);
      task_.outMulti(o_m);

// free data
      task_.multi->multi_free();
      task_.multi->datach_free();
      task_.multi->databr_free();

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

