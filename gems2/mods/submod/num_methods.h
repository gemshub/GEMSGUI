//-------------------------------------------------------------------
// $Id:  $
//
// C/C++ Some functions from NUMERICAL RECIPES IN C
//
// Copyright (C) 2006 S.Dmytriyeva, D.Kulik
//
// This file is part of a GEM-Selektor library for thermodynamic
// modelling by Gibbs energy minimization
// Uses: GEM-Vizor GUI DBMS library, gems/lib/gemvizor.lib
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL)
//
// See http://les.web.psi.ch/Software/GEMS-PSI for more information
// E-mail: gems2.support@psi.ch
//-------------------------------------------------------------------

double enorm( int n, double *x );
int CholeskyDecomposition( int N, double* R, double* X, double* A  );
int LUDecomposition( int N, double* A, double* X  );
void InverseofMatrix( int N, double* A, double* Y );
double DeterminantofMatrix( int N, double* A );


//-----------------------End of num_methods.h--------------------------

