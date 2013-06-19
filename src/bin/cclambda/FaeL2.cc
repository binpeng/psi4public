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
    \ingroup CCLAMBDA
    \brief Enter brief description of file here 
*/
#include <cstdio>
#include <libdpd/dpd.h>
#include "MOInfo.h"
#include "Params.h"
#define EXTERN
#include "globals.h"

namespace psi { namespace cclambda {

/** The ROHF version of this contraction can be done with fewer contractions.
 **/

void FaeL2(int L_irr)
{
  dpdbuf4 Lijab, LIJAB, LIjAb;
  dpdbuf4 newLIJAB, newLijab, newLIjAb;
  dpdfile2 LFaet2, LFAEt2, F;
  dpdbuf4 X, X1, X2;
  dpdbuf4 L2, newL2;

  /* RHS += P(ab)*Lijae*Feb */

  if(params.ref == 0) { /** RHF **/

    dpd_->buf4_init(&X, PSIF_CC_TMP0, L_irr, 0, 5, 0, 5, 0, "X(Ij,Ab)");

    dpd_->buf4_init(&L2, PSIF_CC_LAMBDA, L_irr, 0, 5, 0, 5, 0, "LIjAb");
    dpd_->file2_init(&F, PSIF_CC_OEI, 0, 1, 1, "FAE");
    dpd_->contract424(&L2, &F, &X, 3, 0, 0, 1, 0);
    dpd_->file2_close(&F);
    dpd_->buf4_close(&L2);

    dpd_->buf4_sort_axpy(&X, PSIF_CC_LAMBDA, qpsr, 0, 5, "New LIjAb", 1);
    dpd_->buf4_init(&newL2, PSIF_CC_LAMBDA, L_irr, 0, 5, 0, 5, 0, "New LIjAb");
    dpd_->buf4_axpy(&X, &newL2, 1);
    dpd_->buf4_close(&newL2);

    dpd_->buf4_close(&X);

  }
  else if(params.ref == 1) { /** ROHF **/

    dpd_->file2_init(&LFAEt2, PSIF_CC_OEI, 0, 1, 1, "FAE");
    dpd_->file2_init(&LFaet2, PSIF_CC_OEI, 0, 1, 1, "Fae");

    dpd_->buf4_init(&LIJAB, PSIF_CC_LAMBDA, L_irr, 2, 5, 2, 7, 0, "LIJAB");
    dpd_->buf4_init(&X1, PSIF_CC_TMP1, L_irr, 2, 5, 2, 5, 0, "X(2,5) 1");
    dpd_->contract424(&LIJAB, &LFAEt2, &X1, 3, 0, 0, 1.0, 0.0);
    dpd_->buf4_init(&X2, PSIF_CC_TMP1, L_irr, 2, 5, 2, 5, 0, "X(2,5) 2");
    dpd_->contract244(&LFAEt2, &LIJAB, &X2, 0, 2, 1, 1.0, 0.0);
    dpd_->buf4_close(&LIJAB);
    dpd_->buf4_axpy(&X1, &X2, 1.0);
    dpd_->buf4_close(&X1);
    dpd_->buf4_init(&newLIJAB, PSIF_CC_LAMBDA, L_irr, 2, 5, 2, 7, 0, "New LIJAB");
    dpd_->buf4_axpy(&X2, &newLIJAB, 1.0);
    dpd_->buf4_close(&X2);
    dpd_->buf4_close(&newLIJAB);

    dpd_->buf4_init(&Lijab, PSIF_CC_LAMBDA, L_irr, 2, 5, 2, 7, 0, "Lijab");
    dpd_->buf4_init(&X1, PSIF_CC_TMP1, L_irr, 2, 5, 2, 5, 0, "X(2,5) 1");
    dpd_->contract424(&Lijab, &LFaet2, &X1, 3, 0, 0, 1.0, 0.0);
    dpd_->buf4_init(&X2, PSIF_CC_TMP1, L_irr, 2, 5, 2, 5, 0, "X(2,5) 2");
    dpd_->contract244(&LFaet2, &Lijab, &X2, 0, 2, 1, 1.0, 0.0);
    dpd_->buf4_close(&Lijab);
    dpd_->buf4_axpy(&X1, &X2, 1.0);
    dpd_->buf4_close(&X1);
    dpd_->buf4_init(&newLijab, PSIF_CC_LAMBDA, L_irr, 2, 5, 2, 7, 0, "New Lijab");
    dpd_->buf4_axpy(&X2, &newLijab, 1.0);
    dpd_->buf4_close(&X2);
    dpd_->buf4_close(&newLijab);

    dpd_->buf4_init(&LIjAb, PSIF_CC_LAMBDA, L_irr, 0, 5, 0, 5, 0, "LIjAb");
    dpd_->buf4_init(&newLIjAb, PSIF_CC_LAMBDA, L_irr, 0, 5, 0, 5, 0, "New LIjAb");
    dpd_->contract424(&LIjAb, &LFaet2, &newLIjAb, 3, 0, 0, 1.0, 1.0);
    dpd_->contract244(&LFAEt2, &LIjAb, &newLIjAb, 0, 2, 1, 1.0, 1.0);
    dpd_->buf4_close(&LIjAb);
    dpd_->buf4_close(&newLIjAb);

    dpd_->file2_close(&LFaet2);
    dpd_->file2_close(&LFAEt2);
  }
  else if(params.ref == 2) { /** UHF **/

    dpd_->file2_init(&LFAEt2, PSIF_CC_OEI, 0, 1, 1, "FAEt");
    dpd_->file2_init(&LFaet2, PSIF_CC_OEI, 0, 3, 3, "Faet");

    /** X(IJ,AB) = L_IJ^AE F_EB **/
    dpd_->buf4_init(&X, PSIF_CC_TMP1, L_irr, 2, 5, 2, 5, 0, "X(IJ,AB) A");
    dpd_->buf4_init(&LIJAB, PSIF_CC_LAMBDA, L_irr, 2, 5, 2, 7, 0, "LIJAB");
    dpd_->contract424(&LIJAB, &LFAEt2, &X, 3, 0, 0, 1, 0);
    dpd_->buf4_close(&LIJAB);
    /** X(IJ,AB) --> X'(IJ,BA) **/
    dpd_->buf4_sort(&X, PSIF_CC_TMP1, pqsr, 2, 5, "X'(IJ,BA)");
    dpd_->buf4_close(&X);
    /** X(IJ,AB) = X(IJ,AB) - X'(IJ,BA) **/
    dpd_->buf4_init(&X1, PSIF_CC_TMP1, L_irr, 2, 5, 2, 5, 0, "X(IJ,AB) A");
    dpd_->buf4_init(&X2, PSIF_CC_TMP1, L_irr, 2, 5, 2, 5, 0, "X'(IJ,BA)");
    dpd_->buf4_axpy(&X2, &X1, -1);
    dpd_->buf4_close(&X2);
    dpd_->buf4_close(&X1);
    /** L(IJ,AB) <-- X(IJ,AB) **/
    dpd_->buf4_init(&X, PSIF_CC_TMP1, L_irr, 2, 5, 2, 5, 0, "X(IJ,AB) A");
    dpd_->buf4_init(&newLIJAB, PSIF_CC_LAMBDA, L_irr, 2, 5, 2, 7, 0, "New LIJAB");
    dpd_->buf4_axpy(&X, &newLIJAB, 1.0);
    dpd_->buf4_close(&X);
    dpd_->buf4_close(&newLIJAB);

    /** X(ij,ab) = L_ij^ae F_eb **/
    dpd_->buf4_init(&X, PSIF_CC_TMP1, L_irr, 12, 15, 12, 15, 0, "X(ij,ab) A");
    dpd_->buf4_init(&LIJAB, PSIF_CC_LAMBDA, L_irr, 12, 15, 12, 17, 0, "Lijab");
    dpd_->contract424(&LIJAB, &LFaet2, &X, 3, 0, 0, 1, 0);
    dpd_->buf4_close(&LIJAB);
    /** X(ij,ab) --> X'(ij,ba) **/
    dpd_->buf4_sort(&X, PSIF_CC_TMP1, pqsr, 12, 15, "X'(ij,ba)");
    dpd_->buf4_close(&X);
    /** X(ij,ab) = X(ij,ab) - X'(ij,ba) **/
    dpd_->buf4_init(&X1, PSIF_CC_TMP1, L_irr, 12, 15, 12, 15, 0, "X(ij,ab) A");
    dpd_->buf4_init(&X2, PSIF_CC_TMP1, L_irr, 12, 15, 12, 15, 0, "X'(ij,ba)");
    dpd_->buf4_axpy(&X2, &X1, -1);
    dpd_->buf4_close(&X2);
    dpd_->buf4_close(&X1);
    /** L(ij,ab) <-- X(ij,ab) **/
    dpd_->buf4_init(&X, PSIF_CC_TMP1, L_irr, 12, 15, 12, 15, 0, "X(ij,ab) A");
    dpd_->buf4_init(&newLIJAB, PSIF_CC_LAMBDA, L_irr, 12, 15, 12, 17, 0, "New Lijab");
    dpd_->buf4_axpy(&X, &newLIJAB, 1.0);
    dpd_->buf4_close(&X);
    dpd_->buf4_close(&newLIJAB);

    /** L(Ij,Ab) <-- L(Ij,Ae) F(e,b) - F(E,A) L(Ij,Eb) **/
    dpd_->buf4_init(&LIjAb, PSIF_CC_LAMBDA, L_irr, 22, 28, 22, 28, 0, "LIjAb");
    dpd_->buf4_init(&newLIjAb, PSIF_CC_LAMBDA, L_irr, 22, 28, 22, 28, 0, "New LIjAb");
    dpd_->contract424(&LIjAb, &LFaet2, &newLIjAb, 3, 0, 0, 1, 1);
    dpd_->contract244(&LFAEt2, &LIjAb, &newLIjAb, 0, 2, 1, 1, 1);
    dpd_->buf4_close(&LIjAb);
    dpd_->buf4_close(&newLIjAb);

    dpd_->file2_close(&LFaet2);
    dpd_->file2_close(&LFAEt2);

  }

}

}} // namespace psi::cclambda
