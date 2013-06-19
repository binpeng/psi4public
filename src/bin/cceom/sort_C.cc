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
    \ingroup CCEOM
    \brief Enter brief description of file here 
*/
/* sorts C vectors each iteration to prepare for hbar contractions */

#include <cstdio>
#include <cmath>
#include "MOInfo.h"
#include "Params.h"
#include "Local.h"
#define EXTERN
#include "globals.h"

namespace psi { namespace cceom {
#include <physconst.h>

void sort_C(int C_index, int C_irr) {
  dpdbuf4 CMNEF, Cmnef, CMnEf, CMnfE, CMneF, C2;
  char lbl[32];

  /* Copy used in WmbejDD */
  if (params.eom_ref == 1) { /* ROHF */
    sprintf(lbl, "%s %d", "CMNEF", C_index);
    dpd_->buf4_init(&CMNEF, PSIF_EOM_CMNEF, C_irr, 0, 5, 2, 7, 0, lbl);
    dpd_->buf4_sort(&CMNEF, PSIF_EOM_TMP, prqs, 10, 10, "CMENF");
    dpd_->buf4_close(&CMNEF);
    sprintf(lbl, "%s %d", "Cmnef", C_index);
    dpd_->buf4_init(&Cmnef, PSIF_EOM_Cmnef, C_irr, 0, 5, 2, 7, 0, lbl);
    dpd_->buf4_sort(&Cmnef, PSIF_EOM_TMP, prqs, 10, 10, "Cmenf");
    dpd_->buf4_close(&Cmnef);
  }
  else if (params.eom_ref == 2) { /* UHF */
    sprintf(lbl, "%s %d", "CMNEF", C_index);
    dpd_->buf4_init(&CMNEF, PSIF_EOM_CMNEF, C_irr, 0, 5, 2, 7, 0, lbl);
    dpd_->buf4_sort(&CMNEF, PSIF_EOM_TMP, prqs, 20, 20, "CMENF");
    dpd_->buf4_close(&CMNEF);
    sprintf(lbl, "%s %d", "Cmnef", C_index);
    dpd_->buf4_init(&Cmnef, PSIF_EOM_Cmnef, C_irr, 10, 15, 12, 17, 0, lbl);
    dpd_->buf4_sort(&Cmnef, PSIF_EOM_TMP, prqs, 30, 30, "Cmenf");
    dpd_->buf4_close(&Cmnef);
  }

  /* now do sorts of CMnEf */
  if (params.eom_ref < 2) {
  /* Copy used in WmbejDD */
    sprintf(lbl, "%s %d", "CMnEf", C_index);
    dpd_->buf4_init(&CMnEf, PSIF_EOM_CMnEf, C_irr, 0, 5, 0, 5, 0, lbl);
    dpd_->buf4_sort(&CMnEf, PSIF_EOM_TMP, prqs, 10, 10, "CMEnf");
    /* Copy used in WmnieSD */ 
    dpd_->buf4_sort(&CMnEf, PSIF_EOM_TMP, qprs, 0, 5, "CnMEf");
    /* Copy of current C vector used in WmnieSD and WabefDD */
    dpd_->buf4_sort(&CMnEf, PSIF_EOM_TMP, pqsr, 0, 5, "CMnfE");
    dpd_->buf4_close(&CMnEf);
    /* Copy used in WmbejDD */
    dpd_->buf4_init(&CMnEf, PSIF_EOM_TMP, C_irr, 10, 10, 10, 10, 0, "CMEnf");
    dpd_->buf4_sort(&CMnEf, PSIF_EOM_TMP, psrq, 10, 10, "CMfnE");
    dpd_->buf4_close(&CMnEf);

    dpd_->buf4_init(&CMnEf, PSIF_EOM_TMP, C_irr, 0, 5, 0, 5, 0, "CnMEf");
    dpd_->buf4_sort(&CMnEf, PSIF_EOM_TMP, prqs, 10, 10, "CnEMf");
    dpd_->buf4_close(&CMnEf);
    /* Copy used in FDD, FSD, WamefSD, WmnefDD, WmnieSD */
    dpd_->buf4_init(&CMnEf, PSIF_EOM_TMP, C_irr, 0, 5, 0, 5, 0, "CnMEf");
    dpd_->buf4_sort(&CMnEf, PSIF_EOM_TMP, pqsr, 0, 5, "CmNeF");
    dpd_->buf4_close(&CMnEf);
    /* Copy used in WmbejDD */
    dpd_->buf4_init(&CMnEf, PSIF_EOM_TMP, C_irr, 0, 5, 0, 5, 0, "CmNeF");
    dpd_->buf4_sort(&CMnEf, PSIF_EOM_TMP, prqs, 10, 10, "CmeNF");
    dpd_->buf4_close(&CMnEf);
  }
  else { /* UHF CMnEf sorts */
    sprintf(lbl, "%s %d", "CMnEf", C_index);
    dpd_->buf4_init(&CMnEf, PSIF_EOM_CMnEf, C_irr, 22, 28, 22, 28, 0, lbl);
    dpd_->buf4_sort(&CMnEf, PSIF_EOM_TMP, prqs, 20, 30, "CMEnf");
    /* Copy used in WmnieSD */
    dpd_->buf4_sort(&CMnEf, PSIF_EOM_TMP, qprs, 23, 28, "CnMEf");
    /* Copy used in WmnieSD and WabefDD */
    dpd_->buf4_sort(&CMnEf, PSIF_EOM_TMP, pqsr, 22, 29, "CMnfE");
    dpd_->buf4_close(&CMnEf);
    /* Copy used in WmbejDD */
    dpd_->buf4_init(&CMnEf, PSIF_EOM_TMP, C_irr, 20, 30, 20, 30, 0, "CMEnf");
    dpd_->buf4_sort(&CMnEf, PSIF_EOM_TMP, psrq, 24, 27, "CMfnE");
    dpd_->buf4_close(&CMnEf);

    dpd_->buf4_init(&CMnEf, PSIF_EOM_TMP, C_irr, 23, 28, 23, 28, 0, "CnMEf");
    dpd_->buf4_sort(&CMnEf, PSIF_EOM_TMP, prqs, 27, 24, "CnEMf");
    dpd_->buf4_close(&CMnEf);
    /* Copy used in FDD, FSD, WamefSD, WmnefDD, WmnieSD */
    dpd_->buf4_init(&CMnEf, PSIF_EOM_TMP, C_irr, 23, 28, 23, 28, 0, "CnMEf");
    dpd_->buf4_sort(&CMnEf, PSIF_EOM_TMP, pqsr, 23, 29, "CmNeF");
    dpd_->buf4_close(&CMnEf);
    /* Copy used in WmbejDD */
    dpd_->buf4_init(&CMnEf, PSIF_EOM_TMP, C_irr, 23, 29, 23, 29, 0, "CmNeF");
    dpd_->buf4_sort(&CMnEf, PSIF_EOM_TMP, prqs, 30, 20, "CmeNF");
    dpd_->buf4_close(&CMnEf);
  }

  if (params.eom_ref == 0) { /* special sorts for RHF */
    sprintf(lbl, "%s %d", "CMnEf", C_index);
    dpd_->buf4_init(&CMnEf, PSIF_EOM_CMnEf, C_irr, 0, 5, 0, 5, 0, lbl);
    dpd_->buf4_copy(&CMnEf, PSIF_EOM_TMP, "2CMnEf - CMnfE");
    dpd_->buf4_close(&CMnEf);

    dpd_->buf4_init(&CMnEf, PSIF_EOM_TMP, C_irr, 0, 5, 0, 5, 0, "2CMnEf - CMnfE");
    dpd_->buf4_scm(&CMnEf, 2.0); 
    dpd_->buf4_init(&CMnfE, PSIF_EOM_TMP, C_irr, 0, 5, 0, 5, 0, "CMnfE");
    dpd_->buf4_axpy(&CMnfE, &CMnEf, -1.0);
    dpd_->buf4_close(&CMnfE);
    dpd_->buf4_close(&CMnEf);

    dpd_->buf4_init(&CMnEf, PSIF_EOM_TMP, C_irr, 10, 10, 10, 10, 0, "CMEnf");
    dpd_->buf4_scmcopy(&CMnEf, PSIF_EOM_TMP, "2CMEnf-CMfnE", 2.0);
    dpd_->buf4_close(&CMnEf);

    dpd_->buf4_init(&CMnEf, PSIF_EOM_TMP, C_irr, 10, 10, 10, 10, 0, "2CMEnf-CMfnE");
    dpd_->buf4_init(&C2, PSIF_EOM_TMP, C_irr, 10, 10, 10, 10, 0, "CMfnE");
    dpd_->buf4_axpy(&C2, &CMnEf, -1.0);
    dpd_->buf4_close(&C2);
    dpd_->buf4_close(&CMnEf);
  }
}


}} // namespace psi::cceom
