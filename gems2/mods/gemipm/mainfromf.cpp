//masstransport
//-------------------------------------------------------------------

//#include <iostream>

#include "verror.h"
#include "m_param.h"
#include "gdatastream.h"
istream& f_getline(istream& is, gstring& str, char delim);


//-------------------------------------------------------------------
// Read startup data for structures
// first argument number of nodes in task
// second argv name of binary file with MULTI structure
// next argv file that contained
// -t/-b <dataCH file name> <dataBR file name1> ... <dataBR file nameN>
//

extern "C"
int __stdcall MAIF_START( int nNodes,
   int  c_to_i1[30], int c_to_i2[30], int *nodeTypes )
{

//-------------------------------------------------------------------
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

    if( nodeTypes )
      for( int ii=0; ii<nNodes; ii++)
         if( nodeTypes[ii] < 0  )
             nodeTypes[ii] = 0;


//-------------------------------------------------------------------------
    fstream f_log("ipmlog.txt", ios::out );
    try
    {
      TProfil::pm = new TProfil( nNodes );  // alloc all memory
      bool binary_f = true;
      gstring multu_in = string_cto_i1c;
      gstring chbr_in = string_cto_i2c;

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
      TProfil::pm->readMulti(f_m);

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
      }

// Read dataCH file
     gstring dat_ch = datachbr_file;
      if( binary_f )
      {  GemDataStream f_ch(dat_ch, ios::in|ios::binary);
         TProfil::pm->multi->datach_from_file(f_ch);
       }
      else
      { fstream f_ch(dat_ch.c_str(), ios::in );
         ErrorIf( !f_ch.good() , dat_ch.c_str(), "DataCH ccc Fileopen error");
         TProfil::pm->multi->datach_from_text_file(f_ch);
      }

// read and unpack dataBR structure
     int i = 0;
// for all databr files
     while( !f_chbr.eof() )
     {
// read and unpack dataBR structure
         f_getline( f_chbr, datachbr_file, ',');

         if( binary_f )
         {
             GemDataStream in_br(datachbr_file, ios::in|ios::binary);
             TProfil::pm->multi->databr_from_file(in_br);
          }
         else
          {   fstream in_br(datachbr_file.c_str(), ios::in );
		 ErrorIf( !in_br.good() , datachbr_file.c_str(),
                    "DataBR Fileopen error");
               TProfil::pm->multi->databr_from_text_file(in_br);
          }
          TProfil::pm->multi->unpackDataBr();
// put copy databr to array
          if( nodeTypes )
             for( int ii=0; ii<nNodes; ii++)
                  if( nodeTypes[ii] == i )
                  {    TProfil::pm->multi->SaveCopyTo(ii);
                       TProfil::pm->multi->SaveCopyFrom(ii);
                   }
          i++;
     }

     ErrorIf( i==0, datachbr_file.c_str(), "No any dataBR record" );
     for( int ii=0; ii<nNodes; ii++)
       if(   !nodeTypes || nodeTypes[ii] >= i )
       {    TProfil::pm->multi->SaveCopyTo(ii);
            TProfil::pm->multi->SaveCopyFrom(ii);
       }


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


//-------------------------------------------------------------------------
// calc one node data bridge instances

extern "C"
int __stdcall MAIF_CALC( int  iNode,
   short& p_NodeHandle,    // Node identification handle
   short& p_NodeTypeHY,    // Node type (hydraulic); see typedef NODETYPE
   short& p_NodeTypeMT,    // Node type (mass transport); see typedef NODETYPE
   short& p_NodeStatusFMT, // Node status code FMT; see typedef NODECODEFMT
   short& p_NodeStatusCH,  // Node status code CH;  see typedef NODECODECH
   short& p_IterDone,      // Number of iterations performed by IPM (if not need GEM)
// Chemical scalar variables
   double& p_T,     // Temperature T, K                        +      +      -     -
   double& p_P,     // Pressure P, bar                         +      +      -     -
   double& p_Vs,    // Volume V of reactive subsystem, cm3     -      -      +     +
   double& p_Vi,    // Volume of inert subsystem  ?            +      -      -     +
   double& p_Ms,    // Mass of reactive subsystem, kg          +      +      -     -
   double& p_Mi,    // Mass of inert part, kg    ?             +      -      -     +
   double& p_Gs,    // Gibbs energy of reactive subsystem (J)  -      -      +     +
   double& p_Hs,    // Enthalpy of reactive subsystem (J)      -      -      +     +
   double& p_Hi,    // Enthalpy of inert subsystem (J) ?       +      -      -     +
   double& p_IC,    // Effective molal aq ionic strength           -      -      +     +
   double& p_pH,    // pH of aqueous solution                      -      -      +     +
   double& p_pe,    // pe of aqueous solution                      -      -      +     +
   double& p_Eh,    // Eh of aqueous solution, V                   -      -      +     +
   double& p_denW,
   double& p_denWg, // Density of H2O(l) and steam at T,P      -      -      +     +
   double& p_epsW,
   double& p_epsWg, // Diel.const. of H2O(l) and steam at T,P  -      -      +     +
//  FMT variables (units need dimensionsless form)
   double& p_Tm,    // actual total simulation time
   double& p_dt,    // actual time step
   double& p_dt1,   // priveous time step
   double& p_ot,    // output time control for postprocessing
   double& p_Vt,    // total volume of node (voxel) = dx*dy*dz, m**3
   double& p_eps,   // effective (actual) porosity normalized to 1
   double& p_Km,    // actual permeability, m**2
   double& p_Kf,    // actual DARCY`s constant, m**2/s
   double& p_S,	    // specific storage coefficient, dimensionless
   double& p_Tr,    // transmissivity m**2/s
   double& p_h,	    // actual hydraulic head (hydraulic potential), m
   double& p_rho,   // actual carrier density for density driven flow, g/cm**3
   double& p_al,    // specific longitudinal dispersivity of porous media, m
   double& p_at,    // specific transversal dispersivity of porous media, m
   double& p_av,    // specific vertical dispersivity of porous media, m
   double& p_hDl,   // hydraulic longitudinal dispersivity, m**2/s, diffusities from chemical database
   double& p_hDt,   // hydraulic transversal dispersivity, m**2/s
   double& p_hDv,   // hydraulic vertical dispersivity, m**2/s
   double& p_nPe,   // node Peclet number, dimensionless
// Dynamic data - dimensions see in DATACH.H and DATAMT.H structures
// exchange of values occurs through lists of indices, e.g. xDC, xPH
   double  *p_xDC,    // DC mole amounts at equilibrium [nDCb]      -      -      +     +
   double  *p_gam,    // activity coeffs of DC [nDCb]               -      -      +     +
   double  *p_xPH,  // total mole amounts of phases [nPHb]          -      -      +     +
   double  *p_vPS,  // phase volume, cm3/mol        [nPSb]          -      -      +     +
   double  *p_mPS,  // phase (carrier) mass, g      [nPSb]          -      -      +     +
   double  *p_bPS,  // bulk compositions of phases  [nPSb][nICb]    -      -      +     +
   double  *p_xPA,  // amount of carrier in phases  [nPSb] ??       -      -      +     +
   double  *p_bIC,  // bulk mole amounts of IC[nICb]                +      +      -     -
   double  *p_rMB,  // MB Residuals from GEM IPM [nICb]             -      -      +     +
   double  *p_uIC,  // IC chemical potentials (mol/mol)[nICb]       -      -      +     +
  // What else?
   double  *p_dRes1,
   double  *p_dRes2
)
{
  fstream f_log("ipmlog.txt", ios::out|ios::app );
  try
  {

//---------------------------------------------
// need unpack data from transport into DATABR
   TProfil::pm->multi->SaveCopyFrom(iNode);
   TProfil::pm->multi->fromMT(  p_NodeHandle,  p_NodeStatusCH,
      p_T, p_P, p_Ms, p_dt, p_dt1,  p_bIC );   // test simplex

// put data for internal gems structures
    TProfil::pm->multi->unpackDataBr();
//calc part
    TProfil::pm->calcMulti();
//output results to node data bridge
    TProfil::pm->multi->packDataBr();

//----------------------------------------------
// need pack data to transport from DATABR
//   TProfil::pm->multi->toMT();
   DATABR  *dBR = TProfil::pm->multi->data_BR;
   DATACH  *dCH = TProfil::pm->multi->data_CH;

   p_NodeHandle = dBR->NodeHandle;
   p_NodeStatusCH = dBR->NodeStatusCH;
   p_IterDone = dBR->IterDone;

   p_Vs = dBR->Vs;
   p_Gs = dBR->Gs;
   p_Hs = dBR->Hs;
   p_IC = dBR->IC;
   p_pH = dBR->pH;
   p_pe = dBR->pe;
   p_Eh = dBR->Eh;
   p_denW = dBR->denW;
   p_denWg = dBR->denWg;
   p_epsW = dBR->epsW;
   p_epsWg = dBR->epsWg;

  memcpy( p_xDC, dBR->xDC, dCH->nDCb*sizeof(double) );
  memcpy( p_gam, dBR->gam, dCH->nDCb*sizeof(double) );
  memcpy( p_xPH, dBR->xPH, dCH->nPHb*sizeof(double) );
  memcpy( p_vPS, dBR->vPS, dCH->nPSb*sizeof(double) );
  memcpy( p_mPS, dBR->mPS, dCH->nPSb*sizeof(double) );
  memcpy( p_bPS, dBR->bPS, dCH->nPSb*dCH->nICb*sizeof(double) );
  memcpy( p_xPA, dBR->xPA, dCH->nPSb*sizeof(double) );
  memcpy( p_bIC, dBR->bIC, dCH->nICb*sizeof(double) );
  memcpy( p_rMB, dBR->rMB, dCH->nICb*sizeof(double) );
  memcpy( p_uIC, dBR->uIC, dCH->nICb*sizeof(double) );

//**************************************************************
// only for test output results for files
    gstring strr= "calculated.dbr";
// binary DATABR
    GemDataStream out_br(strr, ios::out|ios::binary);
    TProfil::pm->multi->databr_to_file(out_br);
// text DATABR
    fstream out_br_t("calculated_dbr.dat", ios::out );
    ErrorIf( !out_br_t.good() , "calculated_dbr.dat",
                "DataBR text file open error");
    TProfil::pm->multi->databr_to_text_file(out_br_t);
// output multy
    strr = "calc_multi.ipm";
    GemDataStream o_m( strr, ios::out|ios::binary);
    TProfil::pm->outMulti(o_m, strr );
//********************************************************* */

    TProfil::pm->multi->SaveCopyTo(iNode);
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


//-------------------------------------------------------------------------
// internal functions


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


