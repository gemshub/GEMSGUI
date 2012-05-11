struct typedef {  // Bridge to mass transport
  int N,     //  Number of IC in reactive part of the system
      L,     //  Number of non-zero DC in reactive part
      Fi,    //  Number of non-zero phases in reactive part
      iph[]; //  Indices of non-zero phases
  double T, // Temperature, K  (MT-o/EQ-i)
         P, // Pressure, bar   (MT-o/EQ-i)
         V, // Volume, cm3     (MT-o/EQ-i)
         Scale; //  Scale factor for B and V (MT-i/EQ-o)
  double *B,    //  Bulk composition of reactive subsystem [N](MT-o/EQ-i)
         *BA,   //  Matrix of bulk compositions of phases, moles [Fi][N] (MT-i/EQ-o)
         *VA,   //  Volumes of phases, cm3 [Fi]   (MT-i/EQ-o)
         *MA,   //  Masses of phases, g [Fi]      (MT-i/EQ-o)
// Optional data
         *CP,   //  Heat capacity of phases J/g/K [Fi]   (MT-i/EQ-o)
         *H,    //  Enthalpy of phases J/g        [Fi]   (MT-i/EQ-o)
         *D[2], //  IC Diffusion coefficients in aq and gas m2/sec [N](MT-i/EQ-o)
          } BRIDGE;
