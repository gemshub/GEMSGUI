//-------------------------------------------------------------------
// $Id: m_prfget.cpp 1360 2009-07-15 13:37:30Z gems $
//
// Implementation of TProfile class, loading, making and calc functions
//
// Rewritten from C to C++ by S.Dmytriyeva
// Copyright (C) 1995-2001 S.Dmytriyeva, D.Kulik
//
// This file is part of a GEM-Selektor library for thermodynamic
// modelling by Gibbs energy minimization
// Uses: GEM-Selektor GUI GUI DBMS library, gems/lib/gemvizor.lib
//
// This file may be distributed under the GPL v.3 license

//
// See http://gems.web.psi.ch/ for more information
// E-mail: gems2.support@psi.ch
//-------------------------------------------------------------------
//

#include "m_syseq.h"
#include "visor.h"
#include "node.h"

// Run process of calculate equilibria into the GEMSGUI shell
double  TProfil::CalculateEquilibriumGUI( const gstring& lst_path)
{
   bool brief_mode = false;
   bool add_mui = true;
   CurrentSystem2GEMS3K( lst_path, brief_mode, add_mui );

   // run gem_ipm
   auto ret = CalculateEquilibriumServer( lst_path );

   CmReadMulti( lst_path.c_str(), true );

   return ret;
}

// Run process of calculate equilibria into the GEMS3K side
double  TProfil::CalculateEquilibriumServer( const gstring& lst_f_name )
{
    double ret=0.;
    try
    {
        auto dbr_lst_f_name = lst_f_name;
        dbr_lst_f_name = dbr_lst_f_name.replace("-dat","-dbr");

        // Creates TNode structure instance accessible through the "node" pointer
        TNode* node  = new TNode(multi->GetPM());

        // (1) Initialization of GEMS3K internal data by reading  files
        //     whose names are given in the lst_f_name
        if( node->GEM_init( lst_f_name.c_str() ) )
        {
            // error occured during reading the files
            cout << "error occured during reading the files" << endl;
            return ret;
        }

        // (2) re-calculating equilibrium by calling GEMS3K, getting the status back
        long NodeStatusCH = node->GEM_run( false );
        ret  = node->GEM_CalcTime();

        if( NodeStatusCH == OK_GEM_AIA || NodeStatusCH == OK_GEM_SIA  ){
            // (3) Writing results in default DBR file
            node->GEM_write_dbr( dbr_lst_f_name.c_str(), false, true, false );
            node->GEM_print_ipm( "GEMipmOK.txt" );   // possible debugging printout
        }
        else {
            // (4) possible return status analysis, error message
            node->GEM_print_ipm( "GEMipmError.txt" );   // possible debugging printout
            return ret; // GEM IPM did not converge properly - error message needed
        }

    }catch(TError& err)
    {


    }
    catch(...)
    {

    }
    return ret;
}
