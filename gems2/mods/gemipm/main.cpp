//masstransport
//-------------------------------------------------------------------

//#include <iostream>

#include "verror.h"
#include "m_param.h"
#include "gdatastream.h"

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

gstring
u_makepath(const gstring& dir,
           const gstring& name, const gstring& ext)
{
    gstring Path(dir);
    if( dir != "")
      Path += "/";
    Path += name;
    Path += ".";
    Path += ext;

    return Path;
}


void
u_splitpath(const gstring& Path, gstring& dir,
            gstring& name, gstring& ext)
{
    size_t pos = Path.rfind("/");
    if( pos != npos )
        dir = Path.substr(0, pos), pos++;
    else
        dir = "",    pos = 0;

    size_t pose = Path.rfind(".");
    if( pose != npos )
    {
        ext = Path.substr( pose+1, npos );
        name = Path.substr(pos, pose-pos);
    }
    else
    {
        ext = "";
        name = Path.substr(pos, npos);
    }
}


// first argv name of binary file with MULTI structure
// second argv file that contained
// -t/-b <dataCH file name> <dataBR file name1> ... <dataBR file nameN>


// extern "C" int __stdcall
int MAIF ( int  c_to_i1[30], int c_to_i2[30] )
{

#include <string.h>

    int i ;
    char c_to_ic[30];
    char string_cto_i1c[31];
    char string_cto_i2c[31];

    for (i=0; i<=29;i++)
    {
      c_to_ic[i] = char (c_to_i1[i]);
      string_cto_i1c[i]= c_to_ic[i];
    }
    string_cto_i1c[30]= '\0';          // end string in cpp

    for (i=0; i<=29;i++)
    {
      	c_to_ic[i] = char (c_to_i2[i]);
         string_cto_i2c[i]= c_to_ic[i];
    }
    string_cto_i2c[30]= '\0';          // end string in cpp

    TProfil task_;
    fstream f_log("ipmlog.txt", ios::out );
    try
    {
     bool binary_f = true;
     gstring multu_in = string_cto_i1c;     //"xxxx1.ipm";
     gstring chbr_in = string_cto_i2c;           //////"ipmfiles-dat.lst";

// test working with txt files
/*      fstream f_ch(multu_in.c_str(), ios::in );
      ErrorIf( !f_ch.good() , multu_in.c_str(), "DataCH Fileopen error");
      task_.multi->datach_from_text_file(f_ch);

      fstream f_br(chbr_in.c_str(), ios::in );
      ErrorIf( !f_br.good() , chbr_in.c_str(), "DataBr Fileopen error");
      task_.multi->databr_from_text_file(f_br);


      fstream f_ch1(multu_in.c_str(), ios::out );
      ErrorIf( !f_ch1.good() , multu_in.c_str(), "DataCH out Fileopen error");
      task_.multi->datach_to_text_file(f_ch1);

      fstream f_br1(chbr_in.c_str(), ios::out );
      ErrorIf( !f_br1.good() , chbr_in.c_str(), "DataBr out Fileopen error");
      task_.multi->databr_to_text_file(f_br1);
*/

// read multi structure
      GemDataStream f_m(multu_in, ios::in|ios::binary);
      task_.readMulti(f_m);

// read dataCH structure, and some databr structures
// mtr_data structure is:
//  -t/-b  "<dataCH file name>" ,"<dataBR file1 name>", ..., "<dataBR fileN name>"
// Get DataCH file name
      fstream f_chbr(chbr_in.c_str(), ios::in );
      ErrorIf( !f_chbr.good() , chbr_in.c_str(), "Fileopen error");

      gstring datachbr_file;
      f_getline( f_chbr, datachbr_file, ' ');
//test flag -t or -b (by default -b)
      size_t pos = datachbr_file.find( '-');
      if( pos != /*gstring::*/npos )
      {
         if( datachbr_file[pos+1] == 't' )
            binary_f = false;

         f_getline( f_chbr, datachbr_file, ',');
 /*        if( datachbr_file[pos+1] != '\0' )
            pos +=2;
         while(  ( datachbr_file[pos] ==' ' ||
                   datachbr_file[pos] == '\n' ||
                   datachbr_file[pos]== '\t') )
             pos++;
         if( datachbr_file[pos] == '\"')
             pos++;
         datachbr_file = datachbr_file.substr(pos);
*/      }

// Read dataCH file
     gstring dat_ch = datachbr_file;
      if( binary_f )
      {  GemDataStream f_ch(dat_ch, ios::in|ios::binary);
         task_.multi->datach_from_file(f_ch);
       }
      else
      { fstream f_ch(dat_ch.c_str(), ios::in );
         ErrorIf( !f_ch.good() , dat_ch.c_str(), "DataCH ccc Fileopen error");
         task_.multi->datach_from_text_file(f_ch);
		 }
// for all databr files
      while( !f_chbr.eof() )
      {
// read and unpack dataBR structure
         f_getline( f_chbr, datachbr_file, ',');

         if( binary_f )
         {
             GemDataStream in_br(datachbr_file, ios::in|ios::binary);
             task_.multi->databr_from_file(in_br);
          }
         else
          {   fstream in_br(datachbr_file.c_str(), ios::in );

		 ErrorIf( !in_br.good() , datachbr_file.c_str(),
                    "DataBR vvv Fileopen error");
               task_.multi->databr_from_text_file(in_br);
         }
         task_.multi->unpackDataBr();

//calc part
         task_.calcMulti();

//output results

         task_.multi->packDataBr();

         if( binary_f )
         {
            GemDataStream out_br(datachbr_file, ios::out|ios::binary);
            task_.multi->databr_to_file(out_br);
         }
         else
         {      fstream out_br(datachbr_file.c_str(), ios::out );
                ErrorIf( !out_br.good() , datachbr_file.c_str(),
                    "DataBR out yyyy Fileopen error");
                task_.multi->databr_to_text_file(out_br);
         }
      f_log << datachbr_file.c_str() << " +\n";
     }
//test resalts

    gstring dir;
    gstring name;
    gstring ext;
    u_splitpath( multu_in, dir, name, ext );
    multu_in = u_makepath( dir, name, "out" );

    GemDataStream o_m( multu_in, ios::out|ios::binary);
    task_.outMulti(o_m, multu_in);

      if( binary_f )
      {  GemDataStream f_ch(dat_ch, ios::out|ios::binary);
         task_.multi->datach_to_file(f_ch);
       }
       else
       { fstream f_ch(dat_ch.c_str(), ios::out );
         ErrorIf( !f_ch.good() , dat_ch.c_str(), "DataCH Fileopen error");
         task_.multi->datach_to_text_file(f_ch);
       }


// free data
      task_.multi->multi_free();
      task_.multi->datach_free();
      task_.multi->databr_free();

      return 1;
    }
    catch(TError& err)
    {
      f_log << err.title.c_str() << "  : " << err.mess.c_str();
    }
    catch(...)
    {
        return -1;
    }
    return 0;
}


int
main(int argc, char* argv[])
{
     gstring multu_in = "input_multi.txt";
     gstring chbr_in   = "mtr_data.txt";

      // from argv
      if (argc >= 2 )
        multu_in = argv[1];
      if (argc >= 3 )
        chbr_in = argv[2]; \
      int  c_to_i1[30];
      int  c_to_i2[30];
      for( int i=0; i<30; i++ )
      {
        c_to_i1[i]=multu_in[i];
        c_to_i2[i]=chbr_in[i];
      }

      MAIF ( c_to_i1, c_to_i2 );

    return 0;
}

//--------------------- End of main.cpp ---------------------------

