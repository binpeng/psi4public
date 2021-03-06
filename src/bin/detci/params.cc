/*
 *@BEGIN LICENSE
 *
 * PSI4: an ab initio quantum chemistry software package
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 *@END LICENSE
 */

/*! \file
    \ingroup DETCI
    \brief Enter brief description of file here
*/
/*
** PARAMS.CC: File contains functions which get or print the running
**    parameters for the CI calculation.
**
** David Sherrill, 16 November 1994
**
*/

#define EXTERN

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <boost/lexical_cast.hpp>
#include <libciomr/libciomr.h>
#include <libqt/qt.h>
#include <libchkpt/chkpt.h>
#include <libpsio/psio.h>
#include <psifiles.h>
#include <libmints/molecule.h>
#include "structs.h"
#include "globals.h"
#include "psi4-dec.h"

namespace psi { namespace detci {

/*
** get_parameters(): Function gets the program running parameters such
**   as convergence.  These are stored in the Parameters data structure.
*/
void get_parameters(Options &options)
{
  int i, j, k, errcod;
  int iopen=0, tval;
  char line1[133];
  double junk;

  /* need to figure out wheter to filter tei's */
  Parameters.dertype = options.get_str("DERTYPE");
  Parameters.wfn = options.get_str("WFN");

 
  // CDS-TODO: Check these
  /* Parameters.print_lvl is set in detci.cc */
  /* Parameters.have_special_conv is set in detci.cc */

  Parameters.ex_lvl = options.get_int("EX_LEVEL");
  Parameters.cc_ex_lvl = options.get_int("CC_EX_LEVEL");
  Parameters.val_ex_lvl = options.get_int("VAL_EX_LEVEL");
  Parameters.cc_val_ex_lvl = options.get_int("CC_VAL_EX_LEVEL");

  Parameters.cc_a_val_ex_lvl = -1;
  Parameters.cc_b_val_ex_lvl = -1;

  Parameters.num_roots = options.get_int("NUM_ROOTS");

  Parameters.istop = options["ISTOP"].to_integer();
  Parameters.print_ciblks = options["CIBLKS_PRINT"].to_integer();

  // CDS-TODO: We might override the default for PRINT by command line
  // (or similar mechanism) in CASSCF, etc.
  // DGAS: A lot of MCSCF parameters have been removed, we need to print this for now.
  // CASSCF printing needs to be reworked
  //if (Parameters.wfn == "DETCAS" ||
  //    Parameters.wfn == "CASSCF"   || Parameters.wfn == "RASSCF") {
  //  Parameters.print_lvl = 0;
  //}
  //else
  Parameters.print_lvl = 1;
  if (options["PRINT"].has_changed()) {
    Parameters.print_lvl = options.get_int("PRINT");
  }

  Parameters.opentype = PARM_OPENTYPE_UNKNOWN;

  Parameters.ref_sym = options.get_int("REFERENCE_SYM");

  Parameters.oei_file = PSIF_OEI;  /* always need fzc operator */
  Parameters.tei_file = PSIF_MO_TEI;

  Parameters.hd_ave = EVANGELISTI;

  if (Parameters.wfn == "CASSCF") {
    Parameters.fci = 1;
    Parameters.mcscf = 1;
  }
  else if (Parameters.wfn == "RASSCF"){
    Parameters.fci = 0;
    Parameters.mcscf = 1;
  }
  else {
    Parameters.fci = 0;
    Parameters.mcscf = 0;
  }

  if (Parameters.wfn == "ZAPTN") {
    Parameters.mpn = 1;
    Parameters.zaptn = 1;
  }
  else {
    Parameters.mpn = 0;
    Parameters.zaptn = 0;
  }

  Parameters.z_scale_H = 0;
  Parameters.ras1_lvl = -1;
  Parameters.ras1_min = -1;
  Parameters.a_ras1_lvl = -1;
  Parameters.a_ras1_min = -1;
  Parameters.b_ras1_lvl = -1;
  Parameters.b_ras1_min = -1;
  Parameters.ras3_lvl = -1;
  Parameters.ras4_lvl = -1;

  if (Parameters.wfn == "DETCAS" ||
      Parameters.wfn == "CASSCF" ||
      Parameters.wfn == "RASSCF")
    Parameters.guess_vector = PARM_GUESS_VEC_DFILE;
  else
    Parameters.guess_vector = PARM_GUESS_VEC_H0_BLOCK;


  Parameters.neg_only = 1;
  Parameters.nunits = 1;
  Parameters.first_tmp_unit = 50;
  Parameters.first_hd_tmp_unit = 0;
  Parameters.num_hd_tmp_units = 0;
  Parameters.first_c_tmp_unit = 0;
  Parameters.num_c_tmp_units = 0;
  Parameters.first_s_tmp_unit = 0;
  Parameters.num_s_tmp_units = 0;
  Parameters.first_d_tmp_unit = 0;
  Parameters.num_d_tmp_units = 0;

  Parameters.cc = options["CC"].to_integer();

  if (Parameters.dertype == "FIRST" ||
      Parameters.wfn == "DETCAS" ||
      Parameters.wfn == "CASSCF" ||
      Parameters.wfn == "RASSCF")
  {
     Parameters.convergence = 1e-7;
     Parameters.energy_convergence = 1e-8;
     Parameters.opdm = 1;
     Parameters.opdm_write = 1;
     Parameters.tpdm = 1;
     Parameters.tpdm_write = 1;
     Parameters.maxiter = 12;
  }

  else {
    if (Parameters.cc) {
      Parameters.convergence = 1e-5;
      Parameters.energy_convergence = 1e-7;
      Parameters.maxiter = 20;
    }
    else {
      Parameters.convergence = 1e-4;
      Parameters.energy_convergence = 1e-6;
      Parameters.maxiter = 12;
    }
    Parameters.opdm = 0;
    Parameters.opdm_write = 0;
    Parameters.tpdm = 0;
    Parameters.tpdm_write = 0;
  }

  Parameters.opdm_file = PSIF_MO_OPDM;
  Parameters.opdm_diag = 0;
  Parameters.opdm_wrtnos = 0;
  Parameters.opdm_orbsfile = 76;
  Parameters.tpdm_file = PSIF_MO_TPDM;

  if (options["MAXITER"].has_changed()) {
    Parameters.maxiter = options.get_int("MAXITER");
  }
  if (options["R_CONVERGENCE"].has_changed()) {
    Parameters.convergence = options.get_double("R_CONVERGENCE");
  }
  if (options["E_CONVERGENCE"].has_changed()) {
    Parameters.energy_convergence = options.get_double("E_CONVERGENCE");
  }

  Parameters.multp = Process::environment.molecule()->multiplicity();
  Parameters.S = (((double) Parameters.multp) - 1.0) / 2.0;
  if (options["S"].has_changed())
    Parameters.S = options.get_double("S");

  /* specify OPENTYPE (SINGLET if open-shell singlet) */
  Parameters.ref = options.get_str("REFERENCE");
  if (Parameters.ref == "RHF") {
    Parameters.opentype = PARM_OPENTYPE_NONE;
    Parameters.Ms0 = 1;
  }
  else if (Parameters.ref == "ROHF") {
    if (Parameters.multp == 1) {
      Parameters.opentype = PARM_OPENTYPE_SINGLET;
      Parameters.Ms0 = 1;
    }
    else {
      Parameters.opentype = PARM_OPENTYPE_HIGHSPIN;
      Parameters.Ms0 = 0;
    }
  }  /* end ROHF parsing */
  else {
    throw InputException("Invalid DETCI reference " + Parameters.ref,
      "REFERENCE", __FILE__, __LINE__);
  }

  if (options["MS0"].has_changed())
    Parameters.Ms0 = options["MS0"].to_integer();

  Parameters.h0blocksize = options.get_int("H0_BLOCKSIZE");
  Parameters.h0guess_size = Parameters.h0blocksize;
  if (options["H0_GUESS_SIZE"].has_changed())
    Parameters.h0guess_size = options.get_int("H0_GUESS_SIZE");
  if (Parameters.h0guess_size > Parameters.h0blocksize)
    Parameters.h0guess_size = Parameters.h0blocksize;

  Parameters.h0block_coupling_size = options.get_int("H0_BLOCK_COUPLING_SIZE");
  Parameters.h0block_coupling = options["H0_BLOCK_COUPLING"].to_integer();

  Parameters.nprint = options.get_int("NUM_DETS_PRINT");
  Parameters.cc_nprint = options.get_int("NUM_AMPS_PRINT");

  if (options["FCI"].has_changed())
    Parameters.fci = options["FCI"].to_integer();

  Parameters.fci_strings = 0;
  if (Parameters.fci) Parameters.fci_strings = 1;
  if (options["FCI_STRINGS"].has_changed())
    Parameters.fci_strings = options["FCI_STRINGS"].to_integer();

  Parameters.mixed = options["MIXED"].to_integer();
  Parameters.mixed4 = options["MIXED4"].to_integer();
  Parameters.r4s = options["R4S"].to_integer();
  Parameters.repl_otf = options["REPL_OTF"].to_integer();
  Parameters.calc_ssq = options["CALC_S_SQUARED"].to_integer();

  if (options["MPN"].has_changed())
    Parameters.mpn = options["MPN"].to_integer();

  // NOTE: In cases like this where the default value changes based
  // on other parameters, we need to make sure there's always a default
  // set here in this file, because we can't get the default from input
  // parsing --- the input parser is only called if the keyword is
  // present in the input file, because we're checking
  // options["XX"].has_changed() --CDS 8/2011
  if (Parameters.mpn) {
    Parameters.fci = 1;
    Parameters.mpn_schmidt = FALSE;
    Parameters.wigner = TRUE;
    Parameters.guess_vector = PARM_GUESS_VEC_UNIT;
    Parameters.hd_ave = ORB_ENER;
    Parameters.update = UPDATE_DAVIDSON;
    Parameters.hd_otf = TRUE;
    Parameters.nodfile = TRUE;
  }
  else {
    Parameters.update = UPDATE_DAVIDSON;
    Parameters.mpn_schmidt = FALSE;
    Parameters.wigner = FALSE;
    Parameters.hd_otf = TRUE;
    Parameters.nodfile = FALSE;
  }

  Parameters.save_mpn2 = options.get_int("MPN_ORDER_SAVE");
  Parameters.perturbation_parameter =
    options.get_double("PERTURB_MAGNITUDE");

  if (Parameters.perturbation_parameter <= 1.0 &&
      Parameters.perturbation_parameter >= -1.0) Parameters.z_scale_H = 1;
/*
   else { outfile->Printf( "Parameters.perturbation_parameters beyond the"
                 "bounds of -1.0 >= z <= 1.0\n");
         exit(0);
        }
*/
  if (options["MPN_SCHMIDT"].has_changed())
    Parameters.mpn_schmidt = options["MPN_SCHMIDT"].to_integer();

  if (options["MPN_WIGNER"].has_changed())
    Parameters.wigner = options["MPN_WIGNER"].to_integer();

  Parameters.a_ras3_max = options.get_int("A_RAS3_MAX");
  Parameters.b_ras3_max = options.get_int("B_RAS3_MAX");
  Parameters.ras3_max = options.get_int("RAS3_MAX");
  Parameters.ras4_max = options.get_int("RAS4_MAX");
  Parameters.ras34_max = options.get_int("RAS34_MAX");

  Parameters.cc_a_ras3_max = options.get_int("CC_A_RAS3_MAX");
  Parameters.cc_b_ras3_max = options.get_int("CC_B_RAS3_MAX");
  Parameters.cc_ras3_max = options.get_int("CC_RAS3_MAX");
  Parameters.cc_ras4_max = options.get_int("CC_RAS4_MAX");
  Parameters.cc_ras34_max = options.get_int("CC_RAS34_MAX");

  if (options["GUESS_VECTOR"].has_changed()) {
    std::string line1 = options.get_str("GUESS_VECTOR");
    if (line1 == "UNIT")
      Parameters.guess_vector = PARM_GUESS_VEC_UNIT;
    else if (line1 == "H0_BLOCK")
      Parameters.guess_vector = PARM_GUESS_VEC_H0_BLOCK;
    else if (line1 == "DFILE")
      Parameters.guess_vector = PARM_GUESS_VEC_DFILE;
 /* else if (Parameters.mpn) Parameters.guess_vector = PARM_GUESS_VEC_UNIT; */
    else if (line1 == "IMPORT")
      Parameters.guess_vector = PARM_GUESS_VEC_IMPORT;
    else Parameters.guess_vector = PARM_GUESS_VEC_UNIT;
  }

  Parameters.icore = options.get_int("ICORE");

  if (options["HD_AVG"].has_changed()) {
    std::string line1 = options.get_str("HD_AVG");
    if (line1 == "HD_EXACT")    Parameters.hd_ave = HD_EXACT;
    if (line1 == "HD_KAVE")     Parameters.hd_ave = HD_KAVE;
    if (line1 == "ORB_ENER")    Parameters.hd_ave = ORB_ENER;
    if (line1 == "EVANGELISTI") Parameters.hd_ave = EVANGELISTI;
    if (line1 == "LEININGER")   Parameters.hd_ave = LEININGER;
    if (line1 == "Z_KAVE")      Parameters.hd_ave = Z_HD_KAVE;
    /* if (Parameters.mpn) Parameters.hd_ave = ORB_ENER; */
  }

  if (options["HD_OTF"].has_changed())
    Parameters.hd_otf = options["HD_OTF"].to_integer();

  if (Parameters.hd_otf == 0) {
    outfile->Printf( "Warning: HD_OTF FALSE has not been tested recently\n");
  }

  if (options["NO_DFILE"].has_changed())
    Parameters.nodfile = options["NO_DFILE"].to_integer();
  if (Parameters.num_roots > 1) Parameters.nodfile = FALSE;

  Parameters.diag_method = METHOD_DAVIDSON_LIU_SEM;
  if (options["DIAG_METHOD"].has_changed()) {
    std::string line1 = options.get_str("DIAG_METHOD");
    if (line1 == "RSP") Parameters.diag_method = METHOD_RSP;
    if (line1 == "OLSEN") Parameters.diag_method = METHOD_OLSEN;
    if (line1 == "MITRUSHENKOV")
      Parameters.diag_method = METHOD_MITRUSHENKOV;
    if (line1 == "DAVIDSON")
      Parameters.diag_method = METHOD_DAVIDSON_LIU_SEM;
    if (line1 == "SEM")
      Parameters.diag_method = METHOD_DAVIDSON_LIU_SEM;
    if (line1 == "SEMTEST")
      Parameters.diag_method = METHOD_RSPTEST_OF_SEM;
  }

  Parameters.precon = PRECON_DAVIDSON;
  if (options["PRECONDITIONER"].has_changed()) {
    std::string line1 = options.get_str("PRECONDITIONER");
    if (line1 == "LANCZOS") Parameters.precon = PRECON_LANCZOS;
    if (line1 == "DAVIDSON") Parameters.precon = PRECON_DAVIDSON;
    if (line1 == "GEN_DAVIDSON")
      Parameters.precon = PRECON_GEN_DAVIDSON;
    if (line1 == "H0BLOCK") Parameters.precon = PRECON_GEN_DAVIDSON;
    if (line1 == "H0BLOCK_INV")
      Parameters.precon = PRECON_H0BLOCK_INVERT;
    if (line1 == "ITER_INV")
      Parameters.precon = PRECON_H0BLOCK_ITER_INVERT;
    if (line1 == "H0BLOCK_COUPLING")
      Parameters.precon = PRECON_H0BLOCK_COUPLING;
    if (line1 == "EVANGELISTI")
      Parameters.precon = PRECON_EVANGELISTI;
  }

  if (options["UPDATE"].has_changed()) {
    std::string line1 = options.get_str("UPDATE");
    if (line1 == "DAVIDSON") Parameters.update = UPDATE_DAVIDSON;
    if (line1 == "OLSEN") Parameters.update = UPDATE_OLSEN;
  }

  if (Parameters.diag_method < METHOD_DAVIDSON_LIU_SEM &&
      Parameters.update==UPDATE_DAVIDSON) {
    outfile->Printf("DAVIDSON update not available for OLSEN or MITRUSH"
            " iterators\n");
    Parameters.update = UPDATE_OLSEN;
  }
  if (Parameters.precon==PRECON_EVANGELISTI &&
      (Parameters.update!=UPDATE_DAVIDSON
        || Parameters.diag_method!=METHOD_DAVIDSON_LIU_SEM)) {
    outfile->Printf("EVANGELISTI preconditioner not available for OLSEN or"
                    " MITRUSH iterators or updates.\n");
    Parameters.update = UPDATE_DAVIDSON;
  }

  // errcod = ip_boolean("ZERO_BLOCKS",&(Parameters.zero_blocks),0);
  // if (Parameters.icore || !Parameters.mpn) Parameters.zero_blocks = 0;
  Parameters.num_init_vecs = Parameters.num_roots;
  if (options["NUM_INIT_VECS"].has_changed())
    Parameters.num_init_vecs = options.get_int("NUM_INIT_VECS");

  Parameters.collapse_size = options.get_int("COLLAPSE_SIZE");
  if (Parameters.collapse_size < 1) Parameters.collapse_size = 1;

  Parameters.lse = options["LSE"].to_integer();

  Parameters.lse_collapse = options.get_int("LSE_COLLAPSE");
  if (Parameters.lse_collapse < 1) Parameters.lse_collapse = 3;

  Parameters.lse_tolerance = options.get_double("LSE_TOLERANCE");

  Parameters.maxnvect = options.get_int("MAX_NUM_VECS");

  if (Parameters.maxnvect == 0 &&
      Parameters.diag_method == METHOD_DAVIDSON_LIU_SEM) {
    Parameters.maxnvect = Parameters.maxiter * Parameters.num_roots
      + Parameters.num_init_vecs;
  }
  else if (Parameters.maxnvect == 0 &&
           Parameters.diag_method == METHOD_RSPTEST_OF_SEM) {
    Parameters.maxnvect = Parameters.maxiter * Parameters.num_roots
      + Parameters.num_init_vecs;
  }
  else if (Parameters.maxnvect == 0 &&
           Parameters.diag_method == METHOD_MITRUSHENKOV) {
    Parameters.maxnvect = 2;
  }
  else if (Parameters.maxnvect == 0 &&
           Parameters.diag_method == METHOD_OLSEN) {
    Parameters.maxnvect = 1;
  }
  else { /* the user tried to specify a value for maxnvect...check it */
  /*    if (Parameters.maxnvect / (Parameters.collapse_size *
        Parameters.num_roots) < 2) {
        outfile->Printf( "maxnvect must be at least twice collapse_size *");
        outfile->Printf( " num_roots.\n");
        exit(0);
        }
  */
  }

  if (Parameters.first_hd_tmp_unit == 0)
    Parameters.first_hd_tmp_unit = Parameters.first_tmp_unit;
/*if ( (Parameters.num_hd_tmp_units == 0) && (!Parameters.hd_otf) ) */
  if (Parameters.num_hd_tmp_units == 0)
    Parameters.num_hd_tmp_units = 1;
  if (Parameters.first_c_tmp_unit == 0) Parameters.first_c_tmp_unit =
     Parameters.first_hd_tmp_unit + Parameters.num_hd_tmp_units;
  if (Parameters.num_c_tmp_units == 0) Parameters.num_c_tmp_units =
     Parameters.nunits;
  if (Parameters.first_s_tmp_unit == 0) Parameters.first_s_tmp_unit =
     Parameters.first_c_tmp_unit + Parameters.num_c_tmp_units;
  if (Parameters.num_s_tmp_units == 0) Parameters.num_s_tmp_units =
     Parameters.nunits;
  if (Parameters.first_d_tmp_unit == 0) Parameters.first_d_tmp_unit =
      Parameters.first_s_tmp_unit + Parameters.num_s_tmp_units;
/*if ( (Parameters.num_d_tmp_units == 0) && (!Parameters.nodfile) ) */
  if (Parameters.num_d_tmp_units == 0)
    Parameters.num_d_tmp_units = 1;

  Parameters.restart = options["RESTART"].to_integer();

 /* obsolete due to new restart procedure
   errcod = ip_data("RESTART_ITER","%d",&(Parameters.restart_iter),0);
   errcod = ip_data("RESTART_VECS","%d",&(Parameters.restart_vecs),0);
   if (Parameters.restart && (errcod!=IPE_OK || Parameters.restart_vecs==0)) {
      outfile->Printf( "For RESTART must specify nonzero RESTART_VECS\n");
      exit(0);
      }
 */

  Parameters.bendazzoli = options["BENDAZZOLI"].to_integer();
  if (Parameters.bendazzoli && !Parameters.fci) Parameters.bendazzoli=0;

  /* Parse the OPDM stuff.  It is possible to give incompatible options,
   * but we will try to eliminate some of those.  Parameters_opdm will
   * function as the master switch for all other OPDM parameters.
   */
  Parameters.opdm_print = options["OPDM_PRINT"].to_integer();
  // Make this an internal parameter
  // errcod = ip_data("OPDM_FILE","%d",&(Parameters.opdm_file),0);
  Parameters.opdm_wrtnos = options["NAT_ORBS_WRITE"].to_integer();
  // Make this an internal parameter, essentially same as NAT_ORBS_WRITE
  // errcod = ip_boolean("OPDM_DIAG",&(Parameters.opdm_diag),0);
  Parameters.opdm_ave = options["OPDM_AVG"].to_integer();
  // Make an internal parameter
  // errcod = ip_data("ORBSFILE","%d",&(Parameters.opdm_orbsfile),0);

  // User numbering starts from 1, but internal numbering starts from 0
  Parameters.opdm_orbs_root = options.get_int("NAT_ORBS_WRITE_ROOT");
  Parameters.opdm_orbs_root -= 1;
  if (Parameters.opdm_orbs_root < 0) Parameters.opdm_orbs_root = 0;

  Parameters.opdm_ke = options["OPDM_KE"].to_integer();

  if (Parameters.opdm_wrtnos) Parameters.opdm_diag = 1;
  if (Parameters.opdm_print || Parameters.opdm_diag || Parameters.opdm_wrtnos
      || Parameters.opdm_ave || Parameters.opdm_ke) Parameters.opdm = 1;
  if (options["OPDM"].has_changed())
    Parameters.opdm = options["OPDM"].to_integer();
  if (Parameters.opdm) Parameters.opdm_write = 1;
//   No reason why the user would need to change this, make internal param
//   errcod = ip_boolean("OPDM_WRITE",&(Parameters.opdm_write),0);

  /* transition density matrices */
  Parameters.tdm_print = 0; Parameters.tdm_write = 0;
  Parameters.transdens = 0;
  Parameters.tdm_print = options["TDM_PRINT"].to_integer();
  Parameters.tdm_write = options["TDM_WRITE"].to_integer();

  if (Parameters.tdm_print || Parameters.tdm_write ||
      (Parameters.num_roots > 1))
    Parameters.transdens = 1;
  else
    Parameters.transdens = 0;

  if (options["TDM"].has_changed())
    Parameters.transdens = options["TDM"].to_integer();
  if (Parameters.transdens && !options["TDM_WRITE"].has_changed())
    Parameters.tdm_write = 1;

  /* dipole or transition dipole moment? */
  if (Parameters.opdm) Parameters.dipmom = 1;
  else Parameters.dipmom = 0;

  if (Parameters.transdens) Parameters.dipmom = 1;

  if (Parameters.wfn == "RASSCF" ||
      Parameters.wfn == "CASSCF" ||
      Parameters.wfn == "DETCAS")
    Parameters.dipmom = 0;

  if (options["DIPMOM"].has_changed())
    Parameters.dipmom = options["DIPMOM"].to_integer();

  if (Parameters.dipmom == 1) Parameters.opdm = 1;

  Parameters.root = options.get_int("FOLLOW_ROOT");
  Parameters.root -= 1;
  if (Parameters.root < 0) Parameters.root = 0;

  if (options["TPDM"].has_changed())
    Parameters.tpdm = options["TPDM"].to_integer();

  if (Parameters.tpdm) Parameters.tpdm_write = 1;

  Parameters.tpdm_print = options["TPDM_PRINT"].to_integer();

  if (Parameters.guess_vector == PARM_GUESS_VEC_DFILE &&
      Parameters.wfn != "DETCAS" &&
      Parameters.wfn != "CASSCF" &&
      Parameters.wfn != "RASSCF")
  {

    chkpt_init(PSIO_OPEN_OLD);
    i = chkpt_rd_phase_check();
    chkpt_close();

    if (!i) {
      outfile->Printf( "Can't use d file guess: SCF phase not checked\n");
      if (Parameters.h0guess_size) {
        Parameters.guess_vector = PARM_GUESS_VEC_H0_BLOCK;
        if (Parameters.precon == PRECON_GEN_DAVIDSON)
          Parameters.precon = PRECON_H0BLOCK_ITER_INVERT;
      }
      else Parameters.guess_vector = PARM_GUESS_VEC_UNIT;
    }
  }

  if (Parameters.num_init_vecs < Parameters.num_roots)
    Parameters.num_init_vecs = Parameters.num_roots;
  if (Parameters.guess_vector == PARM_GUESS_VEC_UNIT &&
      Parameters.num_init_vecs > 1) {
    Parameters.guess_vector = PARM_GUESS_VEC_H0_BLOCK;
    outfile->Printf("Warning: Unit vec option not available for more than"
            " one root\n");
  }
  if (Parameters.guess_vector == PARM_GUESS_VEC_UNIT)
    Parameters.h0blocksize = Parameters.h0guess_size = 1;

  
  Parameters.nthreads = Process::environment.get_n_threads();
  if (options["CI_NUM_THREADS"].has_changed()){
     Parameters.nthreads = options.get_int("CI_NUM_THREADS");
  }
  if (Parameters.nthreads < 1) Parameters.nthreads = 1;

  Parameters.export_ci_vector = options["VECS_WRITE"].to_integer();

  Parameters.num_export = 0;
  if (Parameters.export_ci_vector) {
    Parameters.num_export = 1;
    if (options["NUM_VECS_WRITE"].has_changed())
      Parameters.num_export = options.get_int("NUM_VECS_WRITE");
    if (Parameters.num_export > Parameters.num_roots) {
      outfile->Printf( "Warning: can't export %d roots if %d requested\n",
              Parameters.num_export, Parameters.num_roots);
      Parameters.num_export = Parameters.num_roots;
    }
  }

  Parameters.sf_restrict = options["SF_RESTRICT"].to_integer();
  Parameters.print_sigma_overlap = options["SIGMA_OVERLAP"].to_integer();

  if (Parameters.cc) Parameters.ex_lvl = Parameters.cc_ex_lvl + 2;

  Parameters.ex_allow = (int *)malloc(Parameters.ex_lvl*sizeof(int));
  if (options["EX_ALLOW"].has_changed()) {
    i = options["EX_ALLOW"].size(); // CDS-TODO: Check that this really works
    if (i != Parameters.ex_lvl) {
      std::string str = "Dim. of EX_ALLOW must be";
      str += boost::lexical_cast<std::string>( Parameters.ex_lvl) ;
      throw PsiException(str,__FILE__,__LINE__);
    }
    options.fill_int_array("EX_ALLOW", Parameters.ex_allow);
  }
  else {
    for (i=0;i<Parameters.ex_lvl;i++) {
      Parameters.ex_allow[i] = 1;
    }
  }

  /* The filter_guess options are used to filter out some trial
     vectors which may not have the appropriate phase convention
     between two determinants.  This is useful to remove, e.g.,
     delta states when a sigma state is desired.  The user
     inputs two determinants (by giving the absolute alpha string
     number and beta string number for each), and also the
     desired phase between these two determinants for guesses
     which are to be kept.
   */

  Parameters.filter_guess = options["FILTER_GUESS"].to_integer();

  if (Parameters.filter_guess == 1) {
    Parameters.filter_guess_sign = options.get_int("FILTER_GUESS_SIGN");
    if (Parameters.filter_guess_sign != 1 &&
         Parameters.filter_guess_sign != -1) {
      outfile->Printf( "FILTER_GUESS_SIGN should be 1 or -1 !\n");
      abort();
    }

    if (options["FILTER_GUESS_DET1"].size() != 2) {
      outfile->Printf( "Need to specify FILTER_GUESS_DET1 = "
                        "(alphastr betastr)\n");
      abort();
    }
    Parameters.filter_guess_Ia = options["FILTER_GUESS_DET1"][0].to_integer();
    Parameters.filter_guess_Ib = options["FILTER_GUESS_DET1"][1].to_integer();

    if (options["FILTER_GUESS_DET2"].size() != 2) {
      outfile->Printf( "Need to specify FILTER_GUESS_DET2 = "
                        "(alphastr betastr)\n");
      abort();
    }
    Parameters.filter_guess_Ja = options["FILTER_GUESS_DET2"][0].to_integer();
    Parameters.filter_guess_Jb = options["FILTER_GUESS_DET2"][1].to_integer();

  } /* end the filter_guess stuff */

  /* sometimes the filter guess stuff is not sufficient in that
     some states come in that we don't want.  We can help exclude
     them by explicitly zeroing out certain determinants, if that
     is correct for the desired state.  This stuff will allow the
     user to select a determinant which should always have a zero
     coefficient in the desired target state
   */
   Parameters.filter_zero_det = 0;
  if (options["FILTER_ZERO_DET"].has_changed()) {
    if (options["FILTER_ZERO_DET"].size() != 2) {
      outfile->Printf( "Need to specify FILTER_ZERO_DET = "
                       "(alphastr betastr)\n");
      abort();
    }
    Parameters.filter_zero_det = 1;
    Parameters.filter_zero_det_Ia = options["FILTER_ZERO_DET"][0].to_integer();
    Parameters.filter_zero_det_Ib = options["FILTER_ZERO_DET"][1].to_integer();
  }

  /* Does the user request a state-averaged calculation? */
  if (options["AVG_STATES"].has_changed()) {
    i = options["AVG_STATES"].size();
    if (i < 1 || i > Parameters.num_roots) {
      std::string str = "Invalid number of states to average (";
      str += boost::lexical_cast<std::string>( i) ;
      str += ")";
      throw PsiException(str,__FILE__,__LINE__);
    }

    Parameters.average_states = init_int_array(i);
    Parameters.average_weights = init_array(i);
    Parameters.average_num = i;
    for (i=0;i<Parameters.average_num;i++) {
      Parameters.average_states[i] = options["AVG_STATES"][i].to_integer();
      if (Parameters.average_states[i] < 1) {
        std::string str = "AVG_STATES start numbering from 1.\n";
        str += "Invalid state number ";
        str += boost::lexical_cast<std::string>( Parameters.average_states[i]) ;
        throw PsiException(str,__FILE__,__LINE__);
      }
      Parameters.average_states[i] -= 1; /* number from 1 externally */
      Parameters.average_weights[i] = 1.0/((double)Parameters.average_num);
    }

    if (options["AVG_WEIGHTS"].has_changed()) {
      if (options["AVG_WEIGHTS"].size() != Parameters.average_num) {
        std::string str = "Mismatched number of average weights (";
        str += boost::lexical_cast<std::string>( i) ;
        str += ")";
        throw PsiException(str,__FILE__,__LINE__);
      }
      for (i=0; i<Parameters.average_num; i++) {
        Parameters.average_weights[i] =
          options["AVG_WEIGHTS"][i].to_double();
      }
    }

    if (Parameters.average_num > 1) Parameters.opdm_ave = 1;

    if ((!options["FOLLOW_ROOT"].has_changed()) && (Parameters.average_num==1)) {
      Parameters.root = Parameters.average_states[0];
    }
  }

  else {
    Parameters.average_num = 1;
    Parameters.average_states = init_int_array(1);
    Parameters.average_weights = init_array(1);
    Parameters.average_states[0] = Parameters.root;
    Parameters.average_weights[0] = 1.0;
  } /* end state-average parsing */


  Parameters.follow_vec_num = 0;
  if (options["FOLLOW_VECTOR"].has_changed()) {
    i = options["FOLLOW_VECTOR"].size();
    Parameters.follow_vec_num = i;
    Parameters.follow_vec_coef   = init_array(i);
    Parameters.follow_vec_Ia     = init_int_array(i);
    Parameters.follow_vec_Ib     = init_int_array(i);
    Parameters.follow_vec_Iac    = init_int_array(i);
    Parameters.follow_vec_Ibc    = init_int_array(i);
    Parameters.follow_vec_Iaridx = init_int_array(i);
    Parameters.follow_vec_Ibridx = init_int_array(i);

    /* now parse each piece */
    for (i=0; i<Parameters.follow_vec_num; i++) {

      int isize = options["FOLLOW_VECTOR"][i].size();
      if (isize != 2) {
        outfile->Printf( "Need format FOLLOW_VECTOR = \n");
        outfile->Printf( "  [ [[alphastr_i, betastr_i], coeff_i], ... ] \n");
        abort();
      }

      int iisize = options["FOLLOW_VECTOR"][i][0].size();
      if (iisize != 2) {
        outfile->Printf( "Need format FOLLOW_VECTOR = \n");
        outfile->Printf( "  [ [[alphastr_i, betastr_i], coeff_i], ... ] \n");
        abort();
      }

      Parameters.follow_vec_Ia[i] =
        options["FOLLOW_VECTOR"][i][0][0].to_integer();

      Parameters.follow_vec_Ib[i] =
        options["FOLLOW_VECTOR"][i][0][1].to_integer();

      Parameters.follow_vec_coef[i] =
        options["FOLLOW_VECTOR"][i][1].to_double();

    } /* end loop over parsing */
  } /* end follow vector stuff */


  /* make sure SA weights add up to 1.0 */
  for (i=0,junk=0.0; i<Parameters.average_num; i++) {
    junk += Parameters.average_weights[i];
  }
  if (junk <= 0.0) {
    std::string str = "Error: AVERAGE WEIGHTS add up to ";
    char*str2 = new char[25];
    sprintf(str2,"%20.15lf",junk);
    str += str2;
    delete str2;
    throw PsiException(str,__FILE__,__LINE__);
  }
  for (i=0; i<Parameters.average_num; i++) {
    Parameters.average_weights[i] /= junk;
  }

  Parameters.cc_export = options["CC_VECS_WRITE"].to_integer();
  Parameters.cc_import = options["CC_VECS_READ"].to_integer();
  Parameters.cc_fix_external = options["CC_FIX_EXTERNAL"].to_integer();
  Parameters.cc_fix_external_min = options.get_int("CC_FIX_EXTERNAL_MIN");
  Parameters.cc_variational = options["CC_VARIATIONAL"].to_integer();
  Parameters.cc_mixed = options["CC_MIXED"].to_integer();
  /* update using orb eigvals or not? */
  Parameters.cc_update_eps = options["CC_UPDATE_EPS"].to_integer();

  /* DIIS only kicks in for CC anyway, no need to prefix with CC_ */
  Parameters.diis = options["DIIS"].to_integer();
  /* Iteration to turn on DIIS */
  Parameters.diis_start = options.get_int("DIIS_START_ITER");
  /* Do DIIS every n iterations */
  Parameters.diis_freq = options.get_int("DIIS_FREQ");
  Parameters.diis_min_vecs = options.get_int("DIIS_MIN_VECS");
  Parameters.diis_max_vecs = options.get_int("DIIS_MAX_VECS");

  /* parse cc_macro = [
       [ex_lvl, max_holes_I, max_parts_IV, max_I+IV]
       ...
     ]
     This says to eliminate T blocks in which: [the excitation level
     (holes in I + II) is equal to ex_lvl] AND [there are more than
     max_holes_I holes in RAS I, there are more than max_parts_IV
     particles in RAS IV, OR there are more than max_I+IV quasiparticles
     in RAS I + RAS IV]
   */

  Parameters.cc_macro_on = 0;
  if (Parameters.cc && options["CC_MACRO"].has_changed()) {
    i = options["CC_MACRO"].size();
    Parameters.cc_macro = init_int_matrix(Parameters.cc_ex_lvl +
      Parameters.cc_val_ex_lvl + 1, 3);
    Parameters.cc_macro_parsed = init_int_array(Parameters.cc_ex_lvl +
      Parameters.cc_val_ex_lvl + 1);
    for (j=0; j<i; j++) {
      // errcod = ip_data("CC_MACRO","%d",&k,2,j,0);
      k = options["CC_MACRO"][j][0].to_integer();
      // errcod = ip_data("CC_MACRO","%d",Parameters.cc_macro[k],2,j,1);
      Parameters.cc_macro[k][0] = options["CC_MACRO"][j][1].to_integer();
      // errcod = ip_data("CC_MACRO","%d",Parameters.cc_macro[k]+1,2,j,2);
      Parameters.cc_macro[k][1] = options["CC_MACRO"][j][2].to_integer();
      // errcod = ip_data("CC_MACRO","%d",Parameters.cc_macro[k]+2,2,j,3);
      Parameters.cc_macro[k][2] = options["CC_MACRO"][j][3].to_integer();
      Parameters.cc_macro_parsed[k] = 1;
    }
    Parameters.cc_macro_on = 1;
  } /* end parsing of CC_MACRO */

}


/*
** print_parameters(): Function prints the program's running parameters
**   found in the Parameters structure.
*/
void print_parameters(void)
{
   int i;

   outfile->Printf( "\n");
   outfile->Printf( "DETCI PARAMETERS: \n");
   outfile->Printf( "   EX LEVEL      =   %6d      H0 BLOCKSIZE =   %6d\n",
      Parameters.ex_lvl, Parameters.h0blocksize);
   outfile->Printf( "   VAL EX LEVEL  =   %6d      H0 GUESS SIZE=   %6d\n",
      Parameters.val_ex_lvl, Parameters.h0guess_size);
   outfile->Printf( "   H0COUPLINGSIZE=   %6d      H0 COUPLING  =   %6s\n",
      Parameters.h0block_coupling_size, Parameters.h0block_coupling ? "yes" : "no");
   outfile->Printf( "   MAXITER       =   %6d      NUM PRINT    =   %6d\n",
      Parameters.maxiter, Parameters.nprint);
   outfile->Printf( "   NUM ROOTS     =   %6d      ICORE        =   %6d\n",
      Parameters.num_roots, Parameters.icore);
   outfile->Printf( "   PRINT         =   %6d      FCI          =   %6s\n",
      Parameters.print_lvl, Parameters.fci ? "yes" : "no");
   if (Parameters.have_special_conv)
      outfile->Printf(
         "   R CONV         = %8.2g    MIXED        =   %6s\n",
         Parameters.special_conv, Parameters.mixed ? "yes" : "no");
   else
      outfile->Printf( "   R CONV        = %6.2e      MIXED        =   %6s\n",
         Parameters.convergence, Parameters.mixed ? "yes" : "no");

   outfile->Printf( "   E CONV        = %6.2e      MIXED4       =   %6s\n",
      Parameters.energy_convergence, Parameters.mixed4 ? "yes" : "no");
   outfile->Printf( "   OEI FILE      =   %6d      R4S          =   %6s\n",
      Parameters.oei_file, Parameters.r4s ? "yes" : "no");
   outfile->Printf( "   REPL OTF      =   %6s\n",
      Parameters.repl_otf ? "yes" : "no");
   outfile->Printf( "   TEI FILE      =   %6d      DIAG METHOD  =   ",
      Parameters.tei_file);

   switch (Parameters.diag_method) {
      case 0:
         outfile->Printf( "%6s\n", "RSP");
         break;
      case 1:
         outfile->Printf( "%6s\n", "OLSEN");
         break;
      case 2:
         outfile->Printf( "%6s\n", "MITRUS");
         break;
      case 3:
         outfile->Printf( "%6s\n", "SEM");
         break;
      case 4:
         outfile->Printf( "%6s\n", "SEMTEST");
         break;
      default:
         outfile->Printf( "%6s\n", "???");
         break;
      }

   outfile->Printf( "   PRECONDITIONER= ");
   switch (Parameters.precon) {
      case PRECON_LANCZOS:
         outfile->Printf( "%6s", " LANCZOS    ");
         break;
      case PRECON_DAVIDSON:
         outfile->Printf( "%6s", "DAVIDSON    ");
         break;
      case PRECON_GEN_DAVIDSON:
         outfile->Printf( "%6s", "GEN_DAVIDSON");
         break;
      case PRECON_H0BLOCK_INVERT:
         outfile->Printf( "%6s", "H0BLOCK_INV ");
         break;
      case PRECON_H0BLOCK_ITER_INVERT:
         outfile->Printf( "%6s", "ITER_INV    ");
         break;
      case PRECON_H0BLOCK_COUPLING:
         outfile->Printf( "%6s", "H0_COUPLING ");
         break;
      case PRECON_EVANGELISTI:
         outfile->Printf( "%6s", "EVANGELISTI ");
         break;
      default:
         outfile->Printf( "%6s", "???         ");
         break;
      }

   outfile->Printf( "  UPDATE       = ");
   switch (Parameters.update) {
     case 1:
       outfile->Printf("DAVIDSON\n");
       break;
     case 2:
       outfile->Printf("OLSEN\n");
       break;
     default:
       outfile->Printf("???\n");
       break;
      }

   outfile->Printf( "   S             =   %.4lf      Ms0          =   %6s\n",
      Parameters.S, Parameters.Ms0 ? "yes" : "no");
   outfile->Printf( "   MAX NUM VECS  =   %6d\n", Parameters.maxnvect);
   outfile->Printf( "   RESTART       =   %6s\n",
      Parameters.restart ? "yes" : "no");
   outfile->Printf( "   GUESS VECTOR  =  ");
   switch (Parameters.guess_vector) {
      case PARM_GUESS_VEC_UNIT:
         outfile->Printf( "%7s", "UNIT");
         break;
      case PARM_GUESS_VEC_H0_BLOCK:
         outfile->Printf( "%7s", "H0BLOCK");
         break;
      case PARM_GUESS_VEC_DFILE:
         outfile->Printf( "%7s", "D FILE");
         break;
      case PARM_GUESS_VEC_IMPORT:
         outfile->Printf( "%7s", "IMPORT");
         break;
      default:
         outfile->Printf( "%7s", "???");
         break;
      }
   outfile->Printf( "      OPENTYPE     = ");
   switch (Parameters.opentype) {
      case PARM_OPENTYPE_NONE:
         outfile->Printf( "%8s\n", "NONE");
         break;
      case PARM_OPENTYPE_HIGHSPIN:
         outfile->Printf( "%8s\n", "HIGHSPIN");
         break;
      case PARM_OPENTYPE_SINGLET:
         outfile->Printf( "%8s\n", "SINGLET");
         break;
      default:
         outfile->Printf( "%8s\n", "???");
         break;
      }
   if (Parameters.ref_sym == -1)
      outfile->Printf( "   REF SYM       =   %6s\n", "auto");
   else
      outfile->Printf( "   REF SYM       =   %6d\n", Parameters.ref_sym);

   outfile->Printf( "   COLLAPSE SIZE =   %6d", Parameters.collapse_size);
   outfile->Printf( "      HD AVG       = ");
   switch (Parameters.hd_ave) {
     case HD_EXACT:
       outfile->Printf("HD_EXACT\n");
       break;
     case HD_KAVE:
       outfile->Printf("HD_KAVE\n");
       break;
     case ORB_ENER:
       outfile->Printf("ORB_ENER\n");
       break;
     case EVANGELISTI:
       outfile->Printf("EVANGELISTI\n");
       break;
     case LEININGER:
       outfile->Printf("LEININGER\n");
       break;
     default:
       outfile->Printf("???\n");
       break;
     }

   outfile->Printf( "   LSE           =   %6s      LSE ITER     =   %6d\n",
           Parameters.lse ? "yes" : "no", Parameters.lse_iter);
   outfile->Printf( "   HD OTF        =   %6s      NO DFILE     =   %6s\n",
           Parameters.hd_otf ? "yes" : "no", Parameters.nodfile ? "yes":"no");
   outfile->Printf( "   MPN           =   %6s      MPN SCHMIDT  =   %6s\n",
           Parameters.mpn ? "yes":"no", Parameters.mpn_schmidt ? "yes":"no");
   outfile->Printf( "   ZAPTN         =   %6s      MPN WIGNER   =   %6s\n",
           Parameters.zaptn ? "yes":"no", Parameters.wigner ? "yes":"no");
   outfile->Printf( "   PERT Z        =   %1.4f      FOLLOW ROOT  =   %6d\n",
           Parameters.perturbation_parameter, Parameters.root);
   outfile->Printf( "   NUM THREADS   =   %6d\n",
           Parameters.nthreads);
   outfile->Printf( "   VECS WRITE    =   %6s      NUM VECS WRITE = %6d\n",
           Parameters.export_ci_vector ? "yes":"no", Parameters.num_export);
   outfile->Printf( "   FILTER GUESS  =   %6s      SF RESTRICT  =   %6s\n",
           Parameters.filter_guess ?  "yes":"no",
           Parameters.sf_restrict ? "yes":"no");
   if (Parameters.cc && Parameters.diis) {
     outfile->Printf( "   DIIS START    =   %6d      DIIS FREQ    =   %6d\n",
             Parameters.diis_start, Parameters.diis_freq);
     outfile->Printf( "   DIIS MIN VECS =   %6d      DIIS MAX VECS=   %6d\n",
             Parameters.diis_min_vecs, Parameters.diis_max_vecs);
   }
   outfile->Printf( "   OPDM          =   %6s      TRANS DENSITY=   %6s\n",
           Parameters.opdm ?  "yes":"no",
           Parameters.transdens ? "yes":"no");
   outfile->Printf( "\n   FILES         = %3d %2d %2d %2d\n",
      Parameters.first_hd_tmp_unit, Parameters.first_c_tmp_unit,
      Parameters.first_s_tmp_unit, Parameters.first_d_tmp_unit);

   outfile->Printf( "\n   EX ALLOW      = ");
   for (i=0;i<Parameters.ex_lvl;i++) {
     outfile->Printf( "%2d ", Parameters.ex_allow[i]);
   }

   outfile->Printf( "\n   STATE AVERAGE = ");
   for (i=0; i<Parameters.average_num; i++) {
     if (i%5==0 && i!=0) outfile->Printf( "\n");
     outfile->Printf( "%2d(%4.2lf) ",Parameters.average_states[i]+1,
       Parameters.average_weights[i]);
   }

   outfile->Printf( "\n   STATE AVERAGE = ");
   for (i=0; i<Parameters.average_num; i++) {
     if (i%5==0 && i!=0) outfile->Printf( "\n");
     outfile->Printf( "%2d(%4.2lf) ",Parameters.average_states[i]+1,
       Parameters.average_weights[i]);
   }

   if (Parameters.follow_vec_num > 0) {
     outfile->Printf("\nDensity matrices will follow vector like:\n");
     for (i=0; i<Parameters.follow_vec_num; i++)
       outfile->Printf( "(%d %d) %12.6lf\n", Parameters.follow_vec_Ia[i],
         Parameters.follow_vec_Ib[i], Parameters.follow_vec_coef[i]);
   }

   outfile->Printf( "\n\n");
   
}


/*
** set_ras_parms(): Set the RAS parameters or their conventional equivalents
**   (i.e. fermi level, etc).
**
*/
void set_ras_parms(void)
{
   int i,j,cnt;
   int errcod;
   int tot_expl_el,nras2alp,nras2bet,betsocc;
   int *ras1, *ras2, *ras3;
   int *orbsym;

   /* If the user asked for FCI=true, then override the other keywords
      if necessary to ensure that it's really a FCI
    */
   if (Parameters.fci == 1 &&
       (CalcInfo.num_alp_expl + CalcInfo.num_bet_expl) > Parameters.ex_lvl)
   {
     Parameters.val_ex_lvl = 0;
     Parameters.ex_lvl = CalcInfo.num_alp_expl + CalcInfo.num_bet_expl;
     free(Parameters.ex_allow);
     Parameters.ex_allow = init_int_array(Parameters.ex_lvl);
     for (i=0; i<Parameters.ex_lvl; i++) Parameters.ex_allow[i] = 1;

     if (Parameters.print_lvl) {
       outfile->Printf( "Note: Calculation requested is a full CI.\n");
       outfile->Printf(
               "Resetting EX_LEVEL to %d and turning on all excitations\n\n",
               Parameters.ex_lvl);
     }

   } /* end FCI override */

   /* reset ex_lvl if incompatible with number of electrons */
   if (Parameters.cc && (Parameters.cc_ex_lvl >
     CalcInfo.num_alp_expl + CalcInfo.num_bet_expl)) {
     Parameters.cc_ex_lvl = CalcInfo.num_alp_expl + CalcInfo.num_bet_expl;
   }
   if (Parameters.ex_lvl > CalcInfo.num_alp_expl + CalcInfo.num_bet_expl) {
     Parameters.ex_lvl = CalcInfo.num_alp_expl + CalcInfo.num_bet_expl;
   }

   for (i=0,j=0; i<CalcInfo.nirreps; i++) j += CalcInfo.ras_opi[0][i];
   Parameters.a_ras1_lvl = Parameters.b_ras1_lvl = Parameters.ras1_lvl = j-1;

   /* figure out how many electrons are in RAS II */
   /* alpha electrons */
   for (i=0,nras2alp=0,betsocc=0; i<CalcInfo.nirreps; i++) {
      j = CalcInfo.docc[i] - CalcInfo.dropped_docc[i] - CalcInfo.ras_opi[0][i];
      if (Parameters.opentype == PARM_OPENTYPE_HIGHSPIN) {
         j += CalcInfo.socc[i];
         }
      else if (Parameters.opentype == PARM_OPENTYPE_SINGLET) {
         if (betsocc + CalcInfo.socc[i] <= CalcInfo.spab)
            betsocc += CalcInfo.socc[i];
         else {
            j += CalcInfo.socc[i] - (CalcInfo.spab - betsocc);
            betsocc = CalcInfo.spab;
            }
         }
      if (j > 0) nras2alp += j;
      if (j > CalcInfo.ras_opi[1][i]) {
         outfile->Printf( "(set_ras_parms): detecting %d electrons ",
            j - CalcInfo.ras_opi[1][i]);
         outfile->Printf( "in RAS III for irrep %d.\n", i);
         outfile->Printf( "Some parts of DETCI assume all elec in I and II\n");
         }
      }
   /* beta electrons */
   for (i=0,nras2bet=0,betsocc=0; i<CalcInfo.nirreps; i++) {
      j = CalcInfo.docc[i] - CalcInfo.dropped_docc[i] - CalcInfo.ras_opi[0][i];
      if (Parameters.opentype == PARM_OPENTYPE_SINGLET && CalcInfo.socc[i]) {
         if (betsocc + CalcInfo.socc[i] <= CalcInfo.spab)
            j += CalcInfo.socc[i];
         else {
            j += CalcInfo.spab - betsocc;
            betsocc = CalcInfo.spab;
            }
         }
      if (j > 0) nras2bet += j;
      if (j > CalcInfo.ras_opi[1][i]) {
         outfile->Printf( "(set_ras_parms): detecting %d electrons ",
            j - CalcInfo.ras_opi[1][i]);
         outfile->Printf( "in RAS III for irrep %d.\n", i);
         outfile->Printf( "Some parts of DETCI assume all elec in I and II\n");
         }
      }

   Parameters.a_ras1_max = (CalcInfo.num_alp_expl >
         Parameters.a_ras1_lvl + 1) ? Parameters.a_ras1_lvl + 1 :
         (CalcInfo.num_alp_expl) ;
   // CDS 4/15
   // if (Parameters.fzc) Parameters.a_ras1_max += CalcInfo.num_fzc_orbs; 

   Parameters.b_ras1_max = (CalcInfo.num_bet_expl >
         Parameters.b_ras1_lvl + 1) ? Parameters.b_ras1_lvl + 1:
         (CalcInfo.num_bet_expl) ;
   // CDS 4/15
   // if (Parameters.fzc) Parameters.b_ras1_max += CalcInfo.num_fzc_orbs;

   for (i=0,j=0; i<CalcInfo.nirreps; i++) j += CalcInfo.ras_opi[1][i];
   Parameters.ras3_lvl = Parameters.ras1_lvl + j + 1;

   for (i=0,j=0; i<CalcInfo.nirreps; i++) j += CalcInfo.ras_opi[2][i];
   Parameters.ras4_lvl = Parameters.ras3_lvl + j;


   /* check Parameters to make sure everything consistent */

   if (Parameters.cc) {
     if (Parameters.cc_a_val_ex_lvl == -1)
       Parameters.cc_a_val_ex_lvl = Parameters.cc_val_ex_lvl;
     if (Parameters.cc_b_val_ex_lvl == -1)
       Parameters.cc_b_val_ex_lvl = Parameters.cc_val_ex_lvl;
     if (Parameters.cc_a_val_ex_lvl > Parameters.ras3_lvl -
         Parameters.ras1_lvl - 1)
       Parameters.cc_a_val_ex_lvl = Parameters.ras3_lvl-Parameters.ras1_lvl-1;
     if (Parameters.cc_b_val_ex_lvl > Parameters.ras3_lvl -
         Parameters.ras1_lvl - 1)
       Parameters.cc_b_val_ex_lvl = Parameters.ras3_lvl-Parameters.ras1_lvl-1;
     if (Parameters.cc_val_ex_lvl > Parameters.cc_a_val_ex_lvl +
         Parameters.cc_b_val_ex_lvl)
       Parameters.cc_val_ex_lvl = Parameters.cc_a_val_ex_lvl +
         Parameters.cc_b_val_ex_lvl;
   }

   /* deduce Parameters.cc_a_ras3_max and Parameters.cc_b_ras3_max if needed */
   if (Parameters.cc & (Parameters.cc_a_ras3_max == -1 ||
       Parameters.cc_b_ras3_max == -1)) {
      if (Parameters.cc_ras3_max != -1) { /* have parsed cc_ras3_max */
         Parameters.cc_a_ras3_max =
            (Parameters.cc_ras3_max <= CalcInfo.num_alp_expl)
            ? Parameters.cc_ras3_max : CalcInfo.num_alp_expl;
         Parameters.cc_b_ras3_max =
            (Parameters.cc_ras3_max <= CalcInfo.num_bet_expl)
            ? Parameters.cc_ras3_max : CalcInfo.num_bet_expl;
         }
      else {
         Parameters.cc_a_ras3_max =
            (Parameters.cc_ex_lvl <= CalcInfo.num_alp_expl)
            ? Parameters.cc_ex_lvl : CalcInfo.num_alp_expl;
         Parameters.cc_b_ras3_max =
            (Parameters.cc_ex_lvl <= CalcInfo.num_bet_expl)
            ? Parameters.cc_ex_lvl : CalcInfo.num_bet_expl;
         }
      }

   if (Parameters.cc) {
      Parameters.a_ras3_max =
         (Parameters.cc_a_ras3_max+2<=CalcInfo.num_alp_expl)
         ? Parameters.cc_a_ras3_max+2 : CalcInfo.num_alp_expl;
      Parameters.b_ras3_max =
         (Parameters.cc_b_ras3_max+2<=CalcInfo.num_bet_expl)
         ? Parameters.cc_b_ras3_max+2 : CalcInfo.num_bet_expl;
      }

   if (Parameters.a_ras3_max == -1 || Parameters.b_ras3_max == -1) {
      if (Parameters.ras3_max != -1) { /* have parsed ras3_max */
         Parameters.a_ras3_max = (Parameters.ras3_max <= CalcInfo.num_alp_expl)
            ? Parameters.ras3_max : CalcInfo.num_alp_expl;
         Parameters.b_ras3_max = (Parameters.ras3_max <= CalcInfo.num_bet_expl)
            ? Parameters.ras3_max : CalcInfo.num_bet_expl;
         }
      else {
         Parameters.a_ras3_max = (Parameters.ex_lvl <= CalcInfo.num_alp_expl)
            ? Parameters.ex_lvl : CalcInfo.num_alp_expl;
         Parameters.b_ras3_max = (Parameters.ex_lvl <= CalcInfo.num_bet_expl)
            ? Parameters.ex_lvl : CalcInfo.num_bet_expl;
         }
      }

   if (Parameters.cc) {
     if (Parameters.cc_ras4_max != -1) { /* have parsed */
        Parameters.cc_a_ras4_max =
          (Parameters.cc_ras4_max <= CalcInfo.num_alp_expl)
          ? Parameters.cc_ras4_max : CalcInfo.num_alp_expl;
        Parameters.cc_b_ras4_max =
          (Parameters.cc_ras4_max <= CalcInfo.num_bet_expl)
          ? Parameters.cc_ras4_max : CalcInfo.num_bet_expl;
        }
     else {
        Parameters.cc_a_ras4_max = Parameters.cc_a_ras3_max;
        Parameters.cc_b_ras4_max = Parameters.cc_b_ras3_max;
     }
     Parameters.a_ras4_max =
       (Parameters.cc_a_ras4_max+2 <= CalcInfo.num_alp_expl)
       ? Parameters.cc_a_ras4_max+2 : CalcInfo.num_alp_expl;
     Parameters.b_ras4_max =
       (Parameters.cc_b_ras4_max+2 <= CalcInfo.num_bet_expl)
       ? Parameters.cc_b_ras4_max+2 : CalcInfo.num_bet_expl;
   }
   else {
     if (Parameters.ras4_max != -1) { /* have parsed */
        Parameters.a_ras4_max = (Parameters.ras4_max <= CalcInfo.num_alp_expl)
          ? Parameters.ras4_max : CalcInfo.num_alp_expl;
        Parameters.b_ras4_max = (Parameters.ras4_max <= CalcInfo.num_bet_expl)
          ? Parameters.ras4_max : CalcInfo.num_bet_expl;
        }
     else {
        Parameters.a_ras4_max = Parameters.a_ras3_max;
        Parameters.b_ras4_max = Parameters.b_ras3_max;
     }
   }


   if (Parameters.cc) {
     if (Parameters.cc_ras34_max != -1) { /* have parsed */
       Parameters.cc_a_ras34_max = Parameters.cc_ras34_max;
       Parameters.cc_b_ras34_max = Parameters.cc_ras34_max;
     }
     else {
       Parameters.cc_a_ras34_max = Parameters.cc_a_ras3_max
                                 + Parameters.cc_a_ras4_max;
       Parameters.cc_b_ras34_max = Parameters.cc_b_ras3_max
                                 + Parameters.cc_b_ras4_max;
     }
     if (Parameters.ras34_max != -1) { /* have parsed */
        Parameters.a_ras34_max = Parameters.ras34_max;
        Parameters.b_ras34_max = Parameters.ras34_max;
        }
     else {
        Parameters.a_ras34_max = Parameters.cc_a_ras34_max+2;
        if (Parameters.a_ras34_max > CalcInfo.num_alp_expl)
          Parameters.a_ras34_max = CalcInfo.num_alp_expl;
        Parameters.b_ras34_max = Parameters.cc_b_ras34_max+2;
        if (Parameters.b_ras34_max > CalcInfo.num_bet_expl)
          Parameters.b_ras34_max = CalcInfo.num_bet_expl;
        Parameters.ras34_max = Parameters.cc_ras34_max+2;
        if (Parameters.ras34_max > Parameters.a_ras34_max +
            Parameters.b_ras34_max)
          Parameters.ras34_max = Parameters.a_ras34_max +
            Parameters.b_ras34_max;
        }
   }
   else { /* non-CC */
     if (Parameters.ras34_max != -1) { /* have parsed */
       Parameters.a_ras34_max = Parameters.ras34_max;
       Parameters.b_ras34_max = Parameters.ras34_max;
     }
     else {
       Parameters.a_ras34_max = Parameters.a_ras3_max;
       Parameters.b_ras34_max = Parameters.b_ras3_max;
     }
   }

   i = Parameters.ras4_lvl - Parameters.ras3_lvl;
   if (Parameters.a_ras3_max > i) Parameters.a_ras3_max = i;
   if (Parameters.b_ras3_max > i) Parameters.b_ras3_max = i;
   if (Parameters.cc) {
     if (Parameters.cc_a_ras3_max > i) Parameters.cc_a_ras3_max = i;
     if (Parameters.cc_b_ras3_max > i) Parameters.cc_b_ras3_max = i;
   }

   i = CalcInfo.num_ci_orbs - Parameters.ras4_lvl;
   if (Parameters.a_ras4_max > i) Parameters.a_ras4_max = i;
   if (Parameters.b_ras4_max > i) Parameters.b_ras4_max = i;
   if (Parameters.cc) {
     if (Parameters.cc_a_ras4_max > i) Parameters.cc_a_ras4_max = i;
     if (Parameters.cc_b_ras4_max > i) Parameters.cc_b_ras4_max = i;
   }

   i = CalcInfo.num_ci_orbs - Parameters.ras3_lvl;
   if (Parameters.a_ras34_max > i) Parameters.a_ras34_max = i;
   if (Parameters.b_ras34_max > i) Parameters.b_ras34_max = i;
   if (Parameters.cc) {
     if (Parameters.cc_a_ras34_max > i) Parameters.cc_a_ras34_max = i;
     if (Parameters.cc_b_ras34_max > i) Parameters.cc_b_ras34_max = i;
   }

   i = (CalcInfo.num_alp_expl <= Parameters.a_ras1_lvl + 1) ?
      CalcInfo.num_alp_expl : Parameters.a_ras1_lvl + 1;
   Parameters.a_ras1_min = i - Parameters.ex_lvl -
      Parameters.val_ex_lvl;
   if (Parameters.a_ras1_min < 0) Parameters.a_ras1_min = 0;
   // CDS 4/15 no longer include dropped core in ras1_min
   // Parameters.a_ras1_min += CalcInfo.num_fzc_orbs;
   Parameters.a_ras1_min += CalcInfo.num_expl_cor_orbs;

   i = (CalcInfo.num_bet_expl <= Parameters.b_ras1_lvl + 1) ?
      CalcInfo.num_bet_expl : Parameters.b_ras1_lvl + 1;
   Parameters.b_ras1_min = i - Parameters.ex_lvl -
      Parameters.val_ex_lvl;
   if (Parameters.b_ras1_min < 0) Parameters.b_ras1_min = 0;
   // CDS 4/15 no longer include dropped core in ras1_min
   // Parameters.b_ras1_min += CalcInfo.num_fzc_orbs;
   Parameters.b_ras1_min += CalcInfo.num_expl_cor_orbs;

   tot_expl_el = CalcInfo.num_alp_expl + CalcInfo.num_bet_expl;
   if (Parameters.cc) {
     if (Parameters.cc_val_ex_lvl != 0) i = Parameters.cc_val_ex_lvl;
     else i = Parameters.cc_ex_lvl;
     if (Parameters.cc_ras3_max == -1) {
       Parameters.cc_ras3_max = (i <= tot_expl_el) ?  i : tot_expl_el ;
     }
     else {
       if (Parameters.cc_ras3_max > tot_expl_el)
         Parameters.cc_ras3_max = tot_expl_el;
     }
     if (Parameters.ras3_max == -1)
       Parameters.ras3_max = Parameters.cc_ras3_max + 2;
   }
   if (Parameters.ras3_max == -1) {
      Parameters.ras3_max = (Parameters.ex_lvl <= tot_expl_el) ?
         Parameters.ex_lvl : tot_expl_el ;
      }
   else {
      if (Parameters.ras3_max > tot_expl_el)
         Parameters.ras3_max = tot_expl_el;
      }

   i = 2 * (Parameters.ras4_lvl - Parameters.ras3_lvl);
   if (i < Parameters.ras3_max) Parameters.ras3_max = i;
   if (Parameters.cc) {
     if (i < Parameters.cc_ras3_max) Parameters.cc_ras3_max = i;
   }


   i = (tot_expl_el < 2*(Parameters.ras1_lvl + 1)) ? tot_expl_el :
      2*(Parameters.ras1_lvl + 1) ;

   // CDS 4/15 no longer include dropped core in ras1_min
   //Parameters.ras1_min = i - Parameters.ex_lvl -
   //   Parameters.val_ex_lvl + 2 * CalcInfo.num_fzc_orbs;
   Parameters.ras1_min = i - Parameters.ex_lvl - Parameters.val_ex_lvl;

   if (Parameters.a_ras1_min + Parameters.b_ras1_min > Parameters.ras1_min)
      Parameters.ras1_min = Parameters.a_ras1_min + Parameters.b_ras1_min;

   if (Parameters.cc && Parameters.cc_ras4_max == -1) {
     Parameters.cc_ras4_max = (Parameters.cc_ex_lvl <= tot_expl_el) ?
       Parameters.cc_ex_lvl : tot_expl_el;
   }

   if (Parameters.ras4_max == -1) {
     if (Parameters.cc) {
       Parameters.ras4_max = (Parameters.cc_ras4_max+2 <= tot_expl_el) ?
         Parameters.cc_ras4_max+2 : tot_expl_el;
     }
     else Parameters.ras4_max = (Parameters.ex_lvl <= tot_expl_el) ?
       Parameters.ex_lvl : tot_expl_el;
   }

   i = 2 * (CalcInfo.num_ci_orbs - Parameters.ras4_lvl);
   if (i < Parameters.ras4_max) Parameters.ras4_max = i;
   if (Parameters.cc) {
     if (i < Parameters.cc_ras4_max) Parameters.cc_ras4_max = i;
   }

   if (Parameters.cc && Parameters.cc_ras34_max == -1)
     Parameters.cc_ras34_max = Parameters.cc_ras3_max + Parameters.cc_ras4_max;
   i = 2 * (CalcInfo.num_ci_orbs - Parameters.ras3_lvl);
   if (Parameters.cc) {
     if (i < Parameters.cc_ras34_max) Parameters.cc_ras34_max = i;
   }

   if (Parameters.ras34_max == -1 && !Parameters.cc)
     Parameters.ras34_max = Parameters.ras3_max;
   else
     Parameters.ras34_max = Parameters.cc_ras34_max + 2;

   i = 2 * (CalcInfo.num_ci_orbs - Parameters.ras3_lvl);
   if (i < Parameters.ras34_max) Parameters.ras34_max = i;
   if (Parameters.ras34_max > tot_expl_el) Parameters.ras34_max = tot_expl_el;

   if (Parameters.a_ras34_max > Parameters.a_ras3_max + Parameters.a_ras4_max)
     Parameters.a_ras34_max = Parameters.a_ras3_max + Parameters.a_ras4_max;
   if (Parameters.cc && (Parameters.cc_a_ras34_max >
     Parameters.cc_a_ras3_max + Parameters.cc_b_ras4_max))
     Parameters.cc_a_ras34_max = Parameters.cc_a_ras3_max +
     Parameters.cc_a_ras4_max;

   if (Parameters.b_ras34_max > Parameters.b_ras3_max + Parameters.b_ras4_max)
     Parameters.b_ras34_max = Parameters.b_ras3_max + Parameters.b_ras4_max;
   if (Parameters.cc && (Parameters.cc_b_ras34_max >
     Parameters.cc_b_ras3_max + Parameters.cc_b_ras4_max))
     Parameters.cc_b_ras34_max = Parameters.cc_b_ras3_max +
     Parameters.cc_b_ras4_max;

   /* now just re-check some basic things */
   if (Parameters.a_ras34_max > CalcInfo.num_alp_expl)
     Parameters.a_ras34_max = CalcInfo.num_alp_expl;
   if (Parameters.b_ras34_max > CalcInfo.num_bet_expl)
     Parameters.b_ras34_max = CalcInfo.num_bet_expl;
   if (Parameters.cc) {
     if (Parameters.cc_a_ras34_max > CalcInfo.num_alp_expl)
       Parameters.cc_a_ras34_max = CalcInfo.num_alp_expl;
     if (Parameters.cc_b_ras34_max > CalcInfo.num_bet_expl)
       Parameters.cc_b_ras34_max = CalcInfo.num_bet_expl;
   }

   if (Parameters.ras34_max > Parameters.a_ras34_max + Parameters.b_ras34_max)
     Parameters.ras34_max = Parameters.a_ras34_max + Parameters.b_ras34_max;

}


/*
** print_ras_parms(): Set the RAS parameters or their conventional equivalents
**   (i.e. fermi level, etc).
**
*/
void print_ras_parms(void)
{
  int i, j;

  outfile->Printf( "ORBITALS:\n") ;
  outfile->Printf( "   NMO          =   %6d\n", CalcInfo.nmo);
  outfile->Printf( "   FROZEN CORE  =   %6d      RESTR CORE   =   %6d\n",
    CalcInfo.num_fzc_orbs, CalcInfo.num_rsc_orbs);
  outfile->Printf( "   FROZEN VIRT  =   %6d      RESTR VIRT   =   %6d\n",
    CalcInfo.num_fzv_orbs, CalcInfo.num_rsv_orbs);
  outfile->Printf( "   DROPPED CORE =   %6d      DROPPED VIRT =   %6d\n",
    CalcInfo.num_drc_orbs, CalcInfo.num_drv_orbs);
  outfile->Printf( "   EXPLICIT CORE=   %6d      ORBS IN CI   =   %6d\n",
    CalcInfo.num_expl_cor_orbs, CalcInfo.num_ci_orbs);
  outfile->Printf( "   NUM ALP      =   %6d      NUM BET      =   %6d\n",
    CalcInfo.num_alp, CalcInfo.num_bet);
  outfile->Printf( "   NUM ALP EXPL =   %6d      NUM BET EXPL =   %6d\n",
    CalcInfo.num_alp_expl, CalcInfo.num_bet_expl);
  outfile->Printf( "   IOPEN        =   %6s\n", CalcInfo.iopen ? "yes" :
    "no");
  outfile->Printf( "   RAS1 LVL     =   %6d      A RAS3 MAX   =   %6d\n",
    Parameters.ras1_lvl, Parameters.a_ras3_max);
  outfile->Printf( "   RAS1 MIN     =   %6d      B RAS3 MAX   =   %6d\n",
    Parameters.ras1_min, Parameters.b_ras3_max);
  outfile->Printf( "   A RAS1 LVL   =   %6d      RAS4 LVL     =   %6d\n",
    Parameters.a_ras1_lvl, Parameters.ras4_lvl);
  outfile->Printf( "   A RAS1 MIN   =   %6d      A RAS4 MAX   =   %6d\n",
    Parameters.a_ras1_min, Parameters.a_ras4_max);
  outfile->Printf( "   A RAS1 MAX   =   %6d      B RAS4 MAX   =   %6d\n",
    Parameters.a_ras1_max, Parameters.b_ras4_max);
  outfile->Printf( "   B RAS1 LVL   =   %6d      RAS4 MAX     =   %6d\n",
    Parameters.b_ras1_lvl, Parameters.ras4_max);
  outfile->Printf( "   B RAS1 MIN   =   %6d      A RAS34 MAX  =   %6d\n",
    Parameters.b_ras1_min, Parameters.a_ras34_max);
  outfile->Printf( "   B RAS1 MAX   =   %6d      B RAS34 MAX  =   %6d\n",
    Parameters.b_ras1_max, Parameters.b_ras34_max);
  outfile->Printf( "   RAS3 LVL     =   %6d      RAS34 MAX    =   %6d\n",
    Parameters.ras3_lvl, Parameters.ras34_max);
  outfile->Printf( "   RAS3 MAX     =   %6d\n", Parameters.ras3_max);
  if (Parameters.cc) {
    outfile->Printf( "   CC RAS3 MAX  =   %6d      CC RAS4 MAX  =   %6d\n",
      Parameters.cc_ras3_max, Parameters.cc_ras4_max);
    outfile->Printf( "   CC A RAS3 MAX=   %6d      CC B RAS3 MAX=   %6d\n",
      Parameters.cc_a_ras3_max, Parameters.cc_b_ras3_max);
    outfile->Printf( "   CC A RAS4 MAX=   %6d      CC B RAS4 MAX=   %6d\n",
      Parameters.cc_a_ras4_max, Parameters.cc_b_ras4_max);
    outfile->Printf( "   CC RAS34 MAX =   %6d\n",
      Parameters.cc_ras34_max);
    outfile->Printf( "   CC A RAS34 MAX=  %6d      CC B RAS34 MAX=  %6d\n",
      Parameters.cc_a_ras34_max, Parameters.cc_b_ras34_max);
    outfile->Printf( "   CC MIXED     =   %6s      CC FIX EXTERN =  %6s\n",
      Parameters.cc_mixed ? "yes" : "no",
      Parameters.cc_fix_external ? "yes" : "no");
    outfile->Printf( "   CC VARIATIONAL=  %6s\n",
      Parameters.cc_variational ? "yes" : "no");
  }

  outfile->Printf( "\n   DOCC            = ") ;
  for (i=0; i<CalcInfo.nirreps; i++) {
    outfile->Printf( "%2d ", CalcInfo.docc[i]) ;
  }
  outfile->Printf( "\n   SOCC            = ") ;
  for (i=0; i<CalcInfo.nirreps; i++) {
    outfile->Printf( "%2d ", CalcInfo.socc[i]) ;
  }
  outfile->Printf("\n");
  outfile->Printf( "\n   FROZEN DOCC     = ") ;
  for (i=0; i<CalcInfo.nirreps; i++) {
    outfile->Printf( "%2d ", CalcInfo.frozen_docc[i]) ;
  }
  outfile->Printf( "\n   RESTRICTED DOCC = ");
  for (i=0; i<CalcInfo.nirreps; i++) {
    outfile->Printf( "%2d ", CalcInfo.rstr_docc[i]) ;
  }
  for (i=0; i<4; i++) {
    outfile->Printf( "\n   RAS %d           = ",i+1);
    for (j=0; j<CalcInfo.nirreps; j++) {
      outfile->Printf("%2d ",CalcInfo.ras_opi[i][j]);
    }
  }
  outfile->Printf( "\n   RESTRICTED UOCC = ") ;
  for (i=0; i<CalcInfo.nirreps; i++) {
    outfile->Printf( "%2d ", CalcInfo.rstr_uocc[i]) ;
  }
  outfile->Printf( "\n   FROZEN UOCC     = ") ;
  for (i=0; i<CalcInfo.nirreps; i++) {
    outfile->Printf( "%2d ", CalcInfo.frozen_uocc[i]) ;
  }
  outfile->Printf("\n");

  outfile->Printf(
     "*******************************************************\n\n");
}

void set_mcscf_parameters(Options &options)
{
  int i, errcod;
  char line1[133];

  MCSCF_Parameters.print_lvl = 1;
  MCSCF_CalcInfo.mo_hess = NULL;
  MCSCF_CalcInfo.mo_hess_diag = NULL;
  MCSCF_CalcInfo.energy_old = 0;
   
  MCSCF_Parameters.wfn = options.get_str("WFN");
  MCSCF_Parameters.dertype = options.get_str("DERTYPE");
  if (MCSCF_Parameters.dertype == "NONE") {
    MCSCF_Parameters.rms_grad_convergence = 1e-4;
    MCSCF_Parameters.energy_convergence = 1e-7;
  }
  else {
    MCSCF_Parameters.rms_grad_convergence = 1e-7;
    MCSCF_Parameters.energy_convergence = 1e-10;
  }

  if (options["MCSCF_R_CONVERGENCE"].has_changed()) {
    MCSCF_Parameters.rms_grad_convergence = options.get_double("MCSCF_R_CONVERGENCE");
  }
  if (options["MCSCF_E_CONVERGENCE"].has_changed()) {
    MCSCF_Parameters.energy_convergence = options.get_double("MCSCF_E_CONVERGENCE");
  }

  MCSCF_Parameters.max_iter = options.get_int("MCSCF_MAXITER"); 

  MCSCF_Parameters.filter_ints = 0;  /* assume we need all for MCSCF */
  MCSCF_Parameters.oei_file = PSIF_OEI;  /* contains frozen core operator */
  MCSCF_Parameters.tei_file = PSIF_MO_TEI;
  MCSCF_Parameters.opdm_file = PSIF_MO_OPDM;
  MCSCF_Parameters.tpdm_file = PSIF_MO_TPDM;
  MCSCF_Parameters.lag_file = PSIF_MO_LAG;

  MCSCF_Parameters.ignore_fz = true;     /* ignore frozen orbitals for ind pairs? */
  
  if ((MCSCF_Parameters.wfn == "CASSCF") || (MCSCF_Parameters.wfn == "DETCAS"))
    MCSCF_Parameters.ignore_ras_ras = true;   /* ignore RAS/RAS independent pairs? */
  else
    MCSCF_Parameters.ignore_ras_ras = false;

  MCSCF_Parameters.print_lvl = options.get_int("PRINT");
  MCSCF_Parameters.print_mos = options.get_bool("MCSCF_PRINT_MOS");

  MCSCF_Parameters.oei_erase = options.get_bool("MCSCF_OEI_ERASE");
  MCSCF_Parameters.tei_erase = options.get_bool("MCSCF_TEI_ERASE");
  MCSCF_Parameters.ignore_fz = options.get_bool("MCSCF_IGNORE_FZ");
  MCSCF_Parameters.scale_grad = true; /* scale orb grad by inverse Hessian? */

  MCSCF_Parameters.diis_start = options.get_int("MCSCF_DIIS_START");
  MCSCF_Parameters.diis_freq = options.get_int("MCSCF_DIIS_FREQ");
  MCSCF_Parameters.diis_min_vecs = options.get_int("MCSCF_DIIS_MIN_VECS");
  MCSCF_Parameters.diis_max_vecs = options.get_int("MCSCF_DIIS_MAX_VECS");

  MCSCF_Parameters.scale_step = options.get_double("MCSCF_SCALE_STEP");
  MCSCF_Parameters.use_fzc_h = options.get_bool("MCSCF_USE_FZC_H");
  MCSCF_Parameters.invert_hessian = options.get_bool("MCSCF_INVERT_HESSIAN");
  MCSCF_Parameters.hessian = options.get_str("MCSCF_HESSIAN");

  MCSCF_Parameters.level_shift = options.get_bool("MCSCF_DO_LEVEL_SHIFT");
  MCSCF_Parameters.shift = options.get_double("MCSCF_SHIFT");
  MCSCF_Parameters.determ_min = options.get_double("MCSCF_DETERM_MIN");
  MCSCF_Parameters.step_max = options.get_double("MCSCF_STEP_MAX");
  MCSCF_Parameters.use_thetas = options.get_bool("MCSCF_USE_THETAS");
  MCSCF_Parameters.force_step = options.get_bool("MCSCF_FORCE_STEP");
  MCSCF_Parameters.force_pair = options.get_int("MCSCF_FORCE_PAIR");
  MCSCF_Parameters.force_value = options.get_double("MCSCF_FORCE_VALUE");
  MCSCF_Parameters.scale_act_act = options.get_double("MCSCF_SCALE_ACT_ACT");
  MCSCF_Parameters.bfgs = options.get_bool("MCSCF_BFGS");
  MCSCF_Parameters.ds_hessian = options.get_bool("MCSCF_DS_HESSIAN");
}

/*
** mcscf_print_parameters(): Function prints the program's running parameters
**   found in the Parameters structure.
*/
void mcscf_print_parameters(void)
{
  outfile->Printf("\n") ;
  outfile->Printf("DETCAS PARAMETERS: \n") ;
  outfile->Printf("   PRINT          =   %6d      PRINT_MOS     =   %6s\n",
      MCSCF_Parameters.print_lvl, MCSCF_Parameters.print_mos ? "yes" : "no");
  outfile->Printf("   R_CONVERGENCE  =   %6.2e    E CONVERG     =   %6.2e\n",
      MCSCF_Parameters.rms_grad_convergence, MCSCF_Parameters.energy_convergence);
  outfile->Printf("   IGNORE_RAS_RAS =   %6s      IGNORE_FZ     =   %6s\n",
      MCSCF_Parameters.ignore_ras_ras ? "yes" : "no", MCSCF_Parameters.ignore_fz ? "yes" : "no") ;
  outfile->Printf("   OEI FILE       =   %6d      OEI ERASE     =   %6s\n",
      MCSCF_Parameters.oei_file, MCSCF_Parameters.oei_erase ? "yes" : "no");
  outfile->Printf("   TEI FILE       =   %6d      TEI ERASE     =   %6s\n",
      MCSCF_Parameters.tei_file, MCSCF_Parameters.tei_erase ? "yes" : "no");
  outfile->Printf("   OPDM FILE      =   %6d      OPDM ERASE    =   %6s\n",
      MCSCF_Parameters.lag_file, MCSCF_Parameters.opdm_erase ? "yes" : "no");
  outfile->Printf("   TPDM FILE      =   %6d      TPDM ERASE    =   %6s\n",
      MCSCF_Parameters.tpdm_file, MCSCF_Parameters.tpdm_erase ? "yes" : "no");
  outfile->Printf("   LAG FILE       =   %6d      LAG ERASE     =   %6s\n",
      MCSCF_Parameters.lag_file, MCSCF_Parameters.lag_erase ? "yes" : "no");
  outfile->Printf("   DIIS START     =   %6d      DIIS FREQ     =   %6d\n",
      MCSCF_Parameters.diis_start, MCSCF_Parameters.diis_freq);
  outfile->Printf("   DIIS MIN VECS  =   %6d      DIIS MAX VECS =   %6d\n",
      MCSCF_Parameters.diis_min_vecs, MCSCF_Parameters.diis_max_vecs);
  outfile->Printf("   SCALE STEP     =   %6.2E    MAX STEP      =   %6.2lf\n",
      MCSCF_Parameters.scale_step, MCSCF_Parameters.step_max);
  outfile->Printf("   DO_LEVEL SHIFT =   %6s      SHIFT         =   %6.2lf\n",
      MCSCF_Parameters.level_shift ? "yes" : "no", MCSCF_Parameters.shift);
  outfile->Printf("   USE FZC H      =   %6s      HESSIAN       = %-12s\n",
      MCSCF_Parameters.use_fzc_h ? "yes" : "no", MCSCF_Parameters.hessian.c_str());
  outfile->Printf("\n") ;
}


}} // namespace psi::detci
 
