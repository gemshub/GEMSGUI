#include "m_param.h"
#include "gdatastream.h"


//---------------------------------------------------------//

#ifndef IPMGEMPLUGIN

// writing DataCH to binary file
void TMulti::makeStartDataChBR(
  TCIntArray& selIC, TCIntArray& selDC, TCIntArray& selPH )
{
// set sizes for DataCh
  uint ii;

  data_CH.nIC = pm.N;
  data_CH.nDC = pm.L;
  data_CH.nPH = pm.FI;
  data_CH.nPS = pm.FIs;
  data_CH.nTp = data_CH.nPp = 1;
  if( pm.Aalp )
    data_CH.nAalp = 1;
  else
    data_CH.nAalp = 0;
  data_CH.uRes2 = 0;

// These dimensionalities define sizes of dynamic data in DATABT structure!!!

  data_CH.nICb = (short)selIC.GetCount();
  data_CH.nDCb = (short)selDC.GetCount();
  data_CH.nPHb = (short)selPH.GetCount();
  data_CH.nPSb = 0;
  for( ii=0; ii< selPH.GetCount(); ii++, data_CH.nPSb++ )
   if( selPH[ii] >= pm.FIs )
       break;
  data_CH.uRes3 = 0;
  data_CH.uRes4 = 0;

  memset( &data_CH.Tmin, 0 , 10*sizeof(double));

// realloc structures DataCh&DataBr

  datach_realloc();
  databr_realloc();

// set dynamic data to DataCH

  memcpy( data_CH.nDCinPH, pm.L1 , data_CH.nPH*sizeof(short));
  for( ii=0; ii< selIC.GetCount(); ii++ )
    data_CH.xIC[ii] = (short)selIC[ii];
  for( ii=0; ii< selDC.GetCount(); ii++ )
    data_CH.xDC[ii] = (short)selDC[ii];
  for( ii=0; ii< selPH.GetCount(); ii++ )
    data_CH.xPH[ii] = (short)selPH[ii];

  memcpy( data_CH.A, pm.A , data_CH.nIC*data_CH.nDC*sizeof(float));

  for( ii=0; ii< data_CH.nIC; ii++ )
     data_CH.ICmm[ii] = pm.Awt[ii];

  memcpy( data_CH.DCmm, pm.MM , data_CH.nDC*sizeof(double));

// must be changed to matrix structure  ???????
// setted data_CH.nPp*data_CH.nTp = 1
  memcpy( data_CH.G0, pm.G0 , data_CH.nDC*sizeof(double));
  memcpy( data_CH.V0, pm.Vol , data_CH.nDC*sizeof(double));

  for( ii=0; ii< data_CH.nDC; ii++ )
  if ( pm.H0 )
      data_CH.H0[ii] = pm.H0[ii];
  else
      data_CH.H0[ii] = 0.;

  for( ii=0; ii< data_CH.nDC; ii++ )
  if ( pm.Cp0 )
      data_CH.Cp0[ii] = pm.Cp0[ii];
  else
      data_CH.Cp0[ii] = 0.;

  if( data_CH.nAalp >0 )
      for( ii=0; ii< data_CH.nPH; ii++ )
         data_CH.Aalp[ii] = pm.Aalp[ii];

  memcpy( data_CH.ICNL, pm.SB , MaxICN*data_CH.nIC*sizeof(char));
  memcpy( data_CH.DCNL, pm.SM , MaxDCN*data_CH.nDC*sizeof(char));
  memcpy( data_CH.PHNL, pm.SF , MaxPHN*data_CH.nPH*sizeof(char));

  memcpy( data_CH.ccIC, pm.ICC , data_CH.nIC*sizeof(char));
  memcpy( data_CH.ccDC, pm.DCC , data_CH.nDC*sizeof(char));
  memcpy( data_CH.ccDCW, pm.DCCW , data_CH.nDC*sizeof(char));
  memcpy( data_CH.ccPH, pm.PHC , data_CH.nPH*sizeof(char));

// set default data to DataBr

   data_BR.NodeHandle = 0;
   data_BR.NodeTypeHY = initital;
   data_BR.NodeTypeMT = normal;
   data_BR.NodeStatusFMT = Initial_RUN;
   data_BR.NodeStatusCH = NEED_GEM_AIA;
   data_BR.IterDone = 0;

   memset( &data_BR.T, 0, 36*sizeof(double));
   data_BR.T = pm.Tc; //25
   data_BR.P = pm.Pc; //1
   data_BR.Ms = pm.MBX;

// arrays
   for( ii=0; ii<data_CH.nICb; ii++ )
    data_BR.bIC[ii] = pm.B[ data_CH.xIC[ii] ];

// set calculated&dynamic data to DataBR

   packDataBr();

}

#endif

// set nessasary data from Multi structure to DataBr structure
void TMulti::packDataBr()
{
 short ii;
// numbers
  data_BR.NodeStatusCH = OK_GEM_AIA;
  data_BR.IterDone = pm.IT;

// values
  data_BR.Vs = pm.VXc;
  data_BR.Gs = pm.FX;
  data_BR.Hs = pm.HXc;
  data_BR.IC = pm.IC;
  data_BR.pH = pm.pH;
  data_BR.pe = pm.pe;
  data_BR.Eh = pm.Eh;
  data_BR.denW = pm.denW;
  data_BR.denWg = pm.denWg;
  data_BR.epsW = pm.epsW;
  data_BR.epsWg = pm.epsWg;

  // added
  data_BR.Ms = pm.MBX;
// arrays
   for( ii=0; ii<data_CH.nDCb; ii++ )
    data_BR.xDC[ii] = pm.X[ data_CH.xDC[ii] ];
   for( ii=0; ii<data_CH.nDCb; ii++ )
    data_BR.gam[ii] = pm.Gamma[ data_CH.xDC[ii] ];

   for( ii=0; ii<data_CH.nPHb; ii++ )
    data_BR.xPH[ii] = pm.XF[ data_CH.xPH[ii] ];
   for( ii=0; ii<data_CH.nPSb; ii++ )
    data_BR.vPS[ii] = pm.FVOL[ data_CH.xPH[ii] ];
   for( ii=0; ii<data_CH.nPSb; ii++ )
    data_BR.mPS[ii] = pm.FWGT[ data_CH.xPH[ii] ];

   for( ii=0; ii<data_CH.nPSb; ii++ )
   for(short jj=0; jj<data_CH.nICb; jj++ )
   { short new_ndx= (ii*data_CH.nICb)+jj,
           mul_ndx = ( data_CH.xPH[ii]*data_CH.nIC )+ data_CH.xIC[jj];
     data_BR.bPS[new_ndx] = pm.BF[ mul_ndx ];
   }
   for( ii=0; ii<data_CH.nPSb; ii++ )
    data_BR.xPA[ii] = pm.XFA[ data_CH.xPH[ii] ];

//   for( ii=0; ii<data_CH.nICb; ii++ )          ??? only insert
//    data_BR.bIC[ii] = pm.B[ data_CH.xIC[ii] ];
   for( ii=0; ii<data_CH.nICb; ii++ )
    data_BR.rMB[ii] = pm.C[ data_CH.xIC[ii] ];
   for( ii=0; ii<data_CH.nICb; ii++ )
    data_BR.uIC[ii] = pm.U[ data_CH.xIC[ii] ];

}

// set nessasary data from DataBr structure to Multi structure
void TMulti::unpackDataBr()
{
 short ii;
// numbers
  data_BR.NodeStatusCH = NEED_GEM_AIA;
  data_BR.IterDone = 0;

// values
  pm.Tc = data_BR.T;
  pm.Pc  = data_BR.P;
  pm.MBX = data_BR.Ms;

// arrays
   for( ii=0; ii<data_CH.nICb; ii++ )
    pm.B[ data_CH.xIC[ii] ] = data_BR.bIC[ii];

// added  to compare SD 15/07/04  not need
/*   for( ii=0; ii<data_CH.nDCb; ii++ )
    pm.X[ data_CH.xDC[ii] ] = data_BR.xDC[ii];
   for( ii=0; ii<data_CH.nDCb; ii++ )
    pm.Gamma[ data_CH.xDC[ii] ] = data_BR.gam[ii];

   for( ii=0; ii<data_CH.nPHb; ii++ )
    pm.XF[ data_CH.xPH[ii] ] = data_BR.xPH[ii];
   for( ii=0; ii<data_CH.nPSb; ii++ )
    pm.FVOL[ data_CH.xPH[ii] ] = data_BR.vPS[ii];
   for( ii=0; ii<data_CH.nPSb; ii++ )
    pm.FWGT[ data_CH.xPH[ii] ] = data_BR.mPS[ii];

   for( ii=0; ii<data_CH.nPSb; ii++ )
   for(short jj=0; jj<data_CH.nICb; jj++ )
   { short new_ndx= (ii*data_CH.nICb)+jj,
           mul_ndx = ( data_CH.xPH[ii]*data_CH.nIC )+ data_CH.xIC[jj];
     pm.BF[ mul_ndx ] = data_BR.bPS[new_ndx];
   }
   for( ii=0; ii<data_CH.nPSb; ii++ )
    pm.XFA[ data_CH.xPH[ii] ] = data_BR.xPA[ii];

   for( ii=0; ii<data_CH.nICb; ii++ )
    pm.C[ data_CH.xIC[ii] ] = data_BR.rMB[ii];
   for( ii=0; ii<data_CH.nICb; ii++ )
    pm.U[ data_CH.xIC[ii] ] = data_BR.uIC[ii];
*/
}


//---------------------------------------------------------------

// new structures i/o

// writing DataCH to binary file
void TMulti::datach_to_file( GemDataStream& ff )
{
// const data
   ff.writeArray( &data_CH.nIC, 14 );
   ff.writeArray( &data_CH.Tmin, 10 );

//dynamic data
   ff.writeArray( data_CH.nDCinPH, data_CH.nPH );
//   if( data_CH.nICb >0 )
   ff.writeArray( data_CH.xIC, data_CH.nICb );
   ff.writeArray( data_CH.xDC, data_CH.nDCb );
   ff.writeArray( data_CH.xPH, data_CH.nPHb );

   ff.writeArray( data_CH.A, data_CH.nIC*data_CH.nDC );
   ff.writeArray( data_CH.ICmm, data_CH.nIC );
   ff.writeArray( data_CH.DCmm, data_CH.nDC );

   ff.writeArray( data_CH.G0,  data_CH.nDC*data_CH.nPp*data_CH.nTp );
   ff.writeArray( data_CH.V0,  data_CH.nDC*data_CH.nPp*data_CH.nTp );
   ff.writeArray( data_CH.H0,  data_CH.nDC*data_CH.nPp*data_CH.nTp );
   ff.writeArray( data_CH.Cp0, data_CH.nDC*data_CH.nPp*data_CH.nTp );

   if( data_CH.nAalp >0 )
     ff.writeArray( data_CH.Aalp, data_CH.nPH );

   ff.writeArray( (char *)data_CH.ICNL, MaxICN*data_CH.nIC );
   ff.writeArray( (char *)data_CH.DCNL, MaxDCN*data_CH.nDC );
   ff.writeArray( (char *)data_CH.PHNL, MaxPHN*data_CH.nPH );

   ff.writeArray( data_CH.ccIC, data_CH.nIC );
   ff.writeArray( data_CH.ccDC, data_CH.nDC );
   ff.writeArray( data_CH.ccDCW, data_CH.nDC );
   ff.writeArray( data_CH.ccPH, data_CH.nPH );

}

// reading DataCH from binary file
void TMulti::datach_from_file( GemDataStream& ff )
{
// const data
   ff.readArray( &data_CH.nIC, 14 );
   ff.readArray( &data_CH.Tmin, 10 );

  datach_realloc();
  databr_realloc();

//dynamic data
   ff.readArray( data_CH.nDCinPH, data_CH.nPH );
//   if( data_CH.nICb >0 )
   ff.readArray( data_CH.xIC, data_CH.nICb );
   ff.readArray( data_CH.xDC, data_CH.nDCb );
   ff.readArray( data_CH.xPH, data_CH.nPHb );

   ff.readArray( data_CH.A, data_CH.nIC*data_CH.nDC );
   ff.readArray( data_CH.ICmm, data_CH.nIC );
   ff.readArray( data_CH.DCmm, data_CH.nDC );

   ff.readArray( data_CH.G0,  data_CH.nDC*data_CH.nPp*data_CH.nTp );
   ff.readArray( data_CH.V0,  data_CH.nDC*data_CH.nPp*data_CH.nTp );
   ff.readArray( data_CH.H0,  data_CH.nDC*data_CH.nPp*data_CH.nTp );
   ff.readArray( data_CH.Cp0, data_CH.nDC*data_CH.nPp*data_CH.nTp );

   if( data_CH.nAalp >0 )
     ff.readArray( data_CH.Aalp, data_CH.nPH );

   ff.readArray( (char *)data_CH.ICNL, MaxICN*data_CH.nIC );
   ff.readArray( (char *)data_CH.DCNL, MaxDCN*data_CH.nDC );
   ff.readArray( (char *)data_CH.PHNL, MaxPHN*data_CH.nPH );

   ff.readArray( data_CH.ccIC, data_CH.nIC );
   ff.readArray( data_CH.ccDC, data_CH.nDC );
   ff.readArray( data_CH.ccDCW, data_CH.nDC );
   ff.readArray( data_CH.ccPH, data_CH.nPH );

}


void TMulti::datach_to_text_file( fstream& ff )
{
// fstream ff("DataCH.out", ios::out );
// ErrorIf( !ff.good() , "DataCH.out", "Fileopen error");

  outArray( ff, "sCon",  &data_CH.nIC, 14 );
  outArray( ff, "dCon",  &data_CH.Tmin, 10 );

//dynamic data
   outArray( ff, "nDCinPH", data_CH.nDCinPH, data_CH.nPH);
//   if( data_CH.nICb >0 )
   outArray( ff, "xIC", data_CH.xIC, data_CH.nICb);
   outArray( ff, "xDC", data_CH.xDC, data_CH.nDCb);
   outArray( ff, "xPH", data_CH.xPH, data_CH.nPHb);

   outArray( ff, "A", data_CH.A, data_CH.nDC*data_CH.nIC, data_CH.nIC );
   outArray( ff, "ICmm", data_CH.ICmm, data_CH.nIC);
   outArray( ff, "DCmm", data_CH.DCmm, data_CH.nDC);

   outArray( ff, "G0", data_CH.G0,  data_CH.nDC*data_CH.nPp*data_CH.nTp);
   outArray( ff, "V0", data_CH.V0,  data_CH.nDC*data_CH.nPp*data_CH.nTp);
   outArray( ff, "H0", data_CH.H0,  data_CH.nDC*data_CH.nPp*data_CH.nTp);
   outArray( ff, "Cp0", data_CH.Cp0,data_CH.nDC*data_CH.nPp*data_CH.nTp);

   if( data_CH.nAalp >0 )
      outArray( ff, "Aalp", data_CH.Aalp, data_CH.nPH);

   outArray( ff, "ICNL", data_CH.ICNL[0], data_CH.nIC, MaxICN );
   outArray( ff, "DCNL", data_CH.DCNL[0], data_CH.nDC, MaxDCN );
   outArray( ff, "PHNL", data_CH.PHNL[0], data_CH.nPH, MaxPHN );

   outArray( ff, "ccIC", data_CH.ccIC, data_CH.nIC, 1 );
   outArray( ff, "ccDC", data_CH.ccDC, data_CH.nDC, 1 );
   outArray( ff, "ccDCW", data_CH.ccDCW, data_CH.nDC, 1 );
   outArray( ff, "ccPH", data_CH.ccPH, data_CH.nPH, 1 );

}


void TMulti::datach_from_text_file(fstream& ff)
{
// fstream ff("DataCH.out", ios::in );
// ErrorIf( !ff.good() , "DataCH.out", "Fileopen error");

  inArray( ff, "sCon",  &data_CH.nIC, 14 );
  inArray( ff, "dCon",  &data_CH.Tmin, 10 );

  datach_realloc();
  databr_realloc();
  
//dynamic data
   inArray( ff, "nDCinPH", data_CH.nDCinPH, data_CH.nPH);
//   if( data_CH.nICb >0 )
   inArray( ff, "xIC", data_CH.xIC, data_CH.nICb);
   inArray( ff, "xDC", data_CH.xDC, data_CH.nDCb);
   inArray( ff, "xPH", data_CH.xPH, data_CH.nPHb);

   inArray( ff, "A", data_CH.A, data_CH.nDC*data_CH.nIC );
   inArray( ff, "ICmm", data_CH.ICmm, data_CH.nIC);
   inArray( ff, "DCmm", data_CH.DCmm, data_CH.nDC);

   inArray( ff, "G0", data_CH.G0,  data_CH.nDC*data_CH.nPp*data_CH.nTp);
   inArray( ff, "V0", data_CH.V0,  data_CH.nDC*data_CH.nPp*data_CH.nTp);
   inArray( ff, "H0", data_CH.H0,  data_CH.nDC*data_CH.nPp*data_CH.nTp);
   inArray( ff, "Cp0", data_CH.Cp0,data_CH.nDC*data_CH.nPp*data_CH.nTp);

   if( data_CH.nAalp >0 )
      inArray( ff, "Aalp", data_CH.Aalp, data_CH.nPH);

   inArray( ff, "ICNL", data_CH.ICNL[0], data_CH.nIC, MaxICN );
   inArray( ff, "DCNL", data_CH.DCNL[0], data_CH.nDC, MaxDCN );
   inArray( ff, "PHNL", data_CH.PHNL[0], data_CH.nPH, MaxPHN );

   inArray( ff, "ccIC", data_CH.ccIC, data_CH.nIC, 1 );
   inArray( ff, "ccDC", data_CH.ccDC, data_CH.nDC, 1 );
   inArray( ff, "ccDCW", data_CH.ccDCW, data_CH.nDC, 1 );
   inArray( ff, "ccPH", data_CH.ccPH, data_CH.nPH, 1 );
   
}


// allocate DataCH structure
void TMulti::datach_realloc()
{
 data_CH.nDCinPH = new short[data_CH.nPH];

 if( data_CH.nICb >0 )
   data_CH.xIC = new short[data_CH.nICb];
 else  data_CH.xIC = 0;
 if( data_CH.nDCb >0 )
   data_CH.xDC = new short[data_CH.nDCb];
 else  data_CH.xDC = 0;
 if( data_CH.nPHb >0 )
   data_CH.xPH = new short[data_CH.nPHb];
 else  data_CH.xPH = 0;

  data_CH.A = new float[data_CH.nIC*data_CH.nDC];
  data_CH.ICmm = new double[data_CH.nIC];
  data_CH.DCmm = new double[data_CH.nDC];

  data_CH.G0 = new double[data_CH.nDC*data_CH.nPp*data_CH.nTp];
  data_CH.V0 = new double[data_CH.nDC*data_CH.nPp*data_CH.nTp];
  data_CH.H0 = new double[data_CH.nDC*data_CH.nPp*data_CH.nTp];
  data_CH.Cp0 = new double[data_CH.nDC*data_CH.nPp*data_CH.nTp];

  if( data_CH.nAalp >0 )
     data_CH.Aalp = new double[data_CH.nPH];
  else data_CH.Aalp = 0;

  data_CH.ICNL = new char[data_CH.nIC][MaxICN];
  data_CH.DCNL = new char[data_CH.nDC][MaxDCN];
  data_CH.PHNL = new char[data_CH.nPH][MaxPHN];

  data_CH.ccIC = new char[data_CH.nIC];
  data_CH.ccDC = new char[data_CH.nDC];
  data_CH.ccDCW = new char[data_CH.nDC];
  data_CH.ccPH = new char[data_CH.nPH];
}

// free dynamic memory
void TMulti::datach_free()
{
 if( data_CH.nDCinPH )
  { delete[] data_CH.nDCinPH;
    data_CH.nDCinPH = 0;
  }
 if( data_CH.xIC )
  { delete[] data_CH.xIC;
    data_CH.xIC = 0;
  }
 if( data_CH.xDC )
  { delete[] data_CH.xDC;
    data_CH.xDC = 0;
  }
 if( data_CH.xPH )
  { delete[] data_CH.xPH;
    data_CH.xPH = 0;
  }
 if( data_CH.A )
  { delete[] data_CH.A;
    data_CH.A = 0;
  }
 if( data_CH.ICmm )
  { delete[] data_CH.ICmm;
    data_CH.ICmm = 0;
  }
 if( data_CH.DCmm )
  { delete[] data_CH.DCmm;
    data_CH.DCmm = 0;
  }

 if( data_CH.G0 )
  { delete[] data_CH.G0;
    data_CH.G0 = 0;
  }
 if( data_CH.V0 )
  { delete[] data_CH.V0;
    data_CH.V0 = 0;
  }
 if( data_CH.H0 )
  { delete[] data_CH.H0;
    data_CH.H0 = 0;
  }
 if( data_CH.Cp0 )
  { delete[] data_CH.Cp0;
    data_CH.Cp0 = 0;
  }
 if( data_CH.Aalp )
  { delete[] data_CH.Aalp;
    data_CH.Aalp = 0;
  }

 if( data_CH.ICNL )
  { delete[] data_CH.ICNL;
    data_CH.ICNL = 0;
  }
 if( data_CH.DCNL )
  { delete[] data_CH.DCNL;
    data_CH.DCNL = 0;
  }
 if( data_CH.PHNL )
  { delete[] data_CH.PHNL;
    data_CH.PHNL = 0;
  }

 if( data_CH.ccIC )
  { delete[] data_CH.ccIC;
    data_CH.ccIC = 0;
  }
 if( data_CH.ccDC )
  { delete[] data_CH.ccDC;
    data_CH.ccDC = 0;
  }
 if( data_CH.ccDCW )
  { delete[] data_CH.ccDCW;
    data_CH.ccDCW = 0;
  }
 if( data_CH.ccPH )
  { delete[] data_CH.ccPH;
    data_CH.ccPH = 0;
  }
}


// writing DataBR to binary file
void TMulti::databr_to_file( GemDataStream& ff )
{
// const data
   ff.writeArray( &data_BR.NodeHandle, 6 );
   ff.writeArray( &data_BR.T, 36 );

//dynamic data
   ff.writeArray( data_BR.xDC, data_CH.nDCb );
   ff.writeArray( data_BR.gam, data_CH.nDCb );
   ff.writeArray( data_BR.xPH, data_CH.nPHb );
   ff.writeArray( data_BR.vPS, data_CH.nPSb );
   ff.writeArray( data_BR.mPS, data_CH.nPSb );

   ff.writeArray( data_BR.bPS, data_CH.nPSb*data_CH.nICb );
   ff.writeArray( data_BR.xPA, data_CH.nPSb );
   ff.writeArray( data_BR.bIC, data_CH.nICb );
   ff.writeArray( data_BR.rMB, data_CH.nICb );
   ff.writeArray( data_BR.uIC, data_CH.nICb );

   data_BR.dRes1 = 0;
   data_BR.dRes2 = 0;

//   datach_to_text_file();
//   databr_to_text_file();
}

// reading DataBR to binary file
void TMulti::databr_from_file( GemDataStream& ff )
{
// const data
   ff.readArray( &data_BR.NodeHandle, 6 );
   ff.readArray( &data_BR.T, 36 );

//dynamic data
   ff.readArray( data_BR.xDC, data_CH.nDCb );
   ff.readArray( data_BR.gam, data_CH.nDCb );
   ff.readArray( data_BR.xPH, data_CH.nPHb );
   ff.readArray( data_BR.vPS, data_CH.nPSb );
   ff.readArray( data_BR.mPS, data_CH.nPSb );

   ff.readArray( data_BR.bPS, data_CH.nPSb*data_CH.nICb );
   ff.readArray( data_BR.xPA, data_CH.nPSb );
   ff.readArray( data_BR.bIC, data_CH.nICb );
   ff.readArray( data_BR.rMB, data_CH.nICb );
   ff.readArray( data_BR.uIC, data_CH.nICb );

   data_BR.dRes1 = 0;
   data_BR.dRes2 = 0;
}


void TMulti::databr_to_text_file( fstream& ff )
{
// fstream ff("DataBR.out", ios::out );
// ErrorIf( !ff.good() , "DataCH.out", "Fileopen error");

  outArray( ff, "sCon",  &data_BR.NodeHandle, 6 );
  outArray( ff, "dCon",  &data_BR.T, 36 );

  outArray( ff, "xDC",  data_BR.xDC, data_CH.nDCb );
  outArray( ff, "gam",  data_BR.gam, data_CH.nDCb );
  outArray( ff, "xPH",  data_BR.xPH, data_CH.nPHb );
  outArray( ff, "vPS",  data_BR.vPS, data_CH.nPSb );
  outArray( ff, "mPS",  data_BR.mPS, data_CH.nPSb );


  outArray( ff, "bPS",  data_BR.bPS, data_CH.nPSb*data_CH.nICb );
  outArray( ff, "xPA",  data_BR.xPA, data_CH.nPSb );
  outArray( ff, "bIC",  data_BR.bIC, data_CH.nICb );
  outArray( ff, "rMB",  data_BR.rMB, data_CH.nICb );
  outArray( ff, "uIC",  data_BR.uIC, data_CH.nICb );

}


void TMulti::databr_from_text_file( fstream& ff )
{
// fstream ff("DataBR.out", ios::out );
// ErrorIf( !ff.good() , "DataCH.out", "Fileopen error");

  inArray( ff, "sCon",  &data_BR.NodeHandle, 6 );
  inArray( ff, "dCon",  &data_BR.T, 36 );

  inArray( ff, "xDC",  data_BR.xDC, data_CH.nDCb );
  inArray( ff, "gam",  data_BR.gam, data_CH.nDCb );
  inArray( ff, "xPH",  data_BR.xPH, data_CH.nPHb );
  inArray( ff, "vPS",  data_BR.vPS, data_CH.nPSb );
  inArray( ff, "mPS",  data_BR.mPS, data_CH.nPSb );


  inArray( ff, "bPS",  data_BR.bPS, data_CH.nPSb*data_CH.nICb );
  inArray( ff, "xPA",  data_BR.xPA, data_CH.nPSb );
  inArray( ff, "bIC",  data_BR.bIC, data_CH.nICb );
  inArray( ff, "rMB",  data_BR.rMB, data_CH.nICb );
  inArray( ff, "uIC",  data_BR.uIC, data_CH.nICb );
}


// allocate DataBR structure
void TMulti::databr_realloc()
{
 data_BR.xDC = new double[data_CH.nDCb];
 data_BR.gam = new double[data_CH.nDCb];
 data_BR.xPH = new double[data_CH.nPHb];
 data_BR.vPS = new double[data_CH.nPSb];
 data_BR.mPS = new double[data_CH.nPSb];

 data_BR.bPS = new double[data_CH.nPSb*data_CH.nICb];
 data_BR.xPA = new double[data_CH.nPSb];
 data_BR.bIC = new double[data_CH.nICb];
 data_BR.rMB = new double[data_CH.nICb];
 data_BR.uIC = new double[data_CH.nICb];

 data_BR.dRes1 = 0;
 data_BR.dRes2 = 0;
}

// free dynamic memory
void TMulti::databr_free()
{
 if( data_BR.xDC )
  { delete[] data_BR.xDC;
    data_BR.xDC = 0;
  }
 if( data_BR.gam )
  { delete[] data_BR.gam;
    data_BR.gam = 0;
  }
 if( data_BR.xPH )
  { delete[] data_BR.xPH;
    data_BR.xPH = 0;
  }
 if( data_BR.vPS )
  { delete[] data_BR.vPS;
    data_BR.vPS = 0;
  }
 if( data_BR.mPS )
  { delete[] data_BR.mPS;
    data_BR.mPS = 0;
  }

 if( data_BR.bPS )
  { delete[] data_BR.bPS;
    data_BR.bPS = 0;
  }
 if( data_BR.xPA )
  { delete[] data_BR.xPA;
    data_BR.xPA = 0;
  }
 if( data_BR.bIC )
  { delete[] data_BR.bIC;
    data_BR.bIC = 0;
  }
 if( data_BR.rMB )
  { delete[] data_BR.rMB;
    data_BR.rMB = 0;
  }
 if( data_BR.uIC )
  { delete[] data_BR.uIC;
    data_BR.uIC = 0;
  }

}


//---------------------------------------------------------------

