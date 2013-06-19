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

/*! \defgroup CCEOM cceom: Equation-of-Motion Coupled-Cluster */

#include <cstdio>
#include <cmath>
#include "MOInfo.h"
#include "Params.h"
#include "Local.h"
#define EXTERN
#include "globals.h"

namespace psi { namespace cceom {

/* This function computes the H-bar doubles-doubles block contribution
   to a Sigma vector stored at Sigma plus 'i' */

void FDD(int i, int C_irr) {
  dpdfile2 FAE, Fae, FMI, Fmi;
  dpdbuf4 SIJAB, Sijab, SIjAb, FP, FM;
  dpdbuf4 CMNEF, Cmnef, CMnEf, X, X2, Z, Z2, Z3;
  char CMNEF_lbl[32], Cmnef_lbl[32], CMnEf_lbl[32], CmNeF_lbl[32];
  char SIJAB_lbl[32], Sijab_lbl[32], SIjAb_lbl[32];

  if (params.eom_ref == 0) { /* RHF */
    sprintf(CMnEf_lbl, "%s %d", "CMnEf", i);
    sprintf(SIjAb_lbl, "%s %d", "SIjAb", i);

    /* SIjAb += Fbe * CIjAe - FAE * CIjbE */
    dpd_->buf4_init(&Z, PSIF_EOM_TMP, C_irr, 0, 5, 0, 5, 0, "FDD_Fbe Z(Ij,Ab)");
    dpd_->buf4_init(&CMnEf, PSIF_EOM_CMnEf, C_irr, 0, 5, 0, 5, 0, CMnEf_lbl);
    dpd_->file2_init(&FAE, PSIF_CC_OEI, H_IRR, 1, 1, "FAE");
    dpd_->contract424(&CMnEf, &FAE, &Z, 3, 1, 0, 1.0, 0.0);
    dpd_->file2_close(&FAE);
    dpd_->buf4_close(&CMnEf);

    dpd_->buf4_sort(&Z, PSIF_EOM_TMP, qpsr, 0, 5, "FDD_Fbe Z(jI,bA)");
    dpd_->buf4_init(&Z2, PSIF_EOM_TMP, C_irr, 0, 5, 0, 5, 0, "FDD_Fbe Z(jI,bA)");

    dpd_->buf4_init(&SIjAb, PSIF_EOM_SIjAb, C_irr, 0, 5, 0, 5, 0, SIjAb_lbl);
    dpd_->buf4_axpy(&Z, &SIjAb, 1.0);
    dpd_->buf4_axpy(&Z2, &SIjAb, 1.0);
    dpd_->buf4_close(&Z);
    dpd_->buf4_close(&Z2);
    dpd_->buf4_close(&SIjAb);

#ifdef EOM_DEBUG
    check_sum("FDD_Fbe",i,C_irr);
#endif

    /* SIjAb -= FMJ * CImAb - FMI * CjMAb */
    dpd_->buf4_init(&Z, PSIF_EOM_TMP, C_irr, 0, 5, 0, 5, 0, "FDD_Fmj Z(Ij,Ab)");
    dpd_->buf4_init(&CMnEf, PSIF_EOM_CMnEf, C_irr, 0, 5, 0, 5, 0, CMnEf_lbl);
    dpd_->file2_init(&FMI, PSIF_CC_OEI, H_IRR, 0, 0, "FMI");
    dpd_->contract244(&FMI, &CMnEf, &Z, 0, 0, 0, 1.0, 0.0);
    dpd_->file2_close(&FMI);
    dpd_->buf4_close(&CMnEf);
    dpd_->buf4_sort(&Z, PSIF_EOM_TMP, qpsr, 0, 5, "FDD_Fmj Z(jI,bA)"); 
    dpd_->buf4_init(&SIjAb, PSIF_EOM_SIjAb, C_irr, 0, 5, 0, 5, 0, SIjAb_lbl);
    dpd_->buf4_axpy(&Z, &SIjAb, -1.0);
    dpd_->buf4_close(&Z);
    dpd_->buf4_init(&Z, PSIF_EOM_TMP, C_irr, 0, 5, 0, 5, 0, "FDD_Fmj Z(jI,bA)");
    dpd_->buf4_axpy(&Z, &SIjAb, -1.0);
    dpd_->buf4_close(&Z);
    dpd_->buf4_close(&SIjAb);

#ifdef EOM_DEBUG
    check_sum("FDD_Fmj",i,C_irr);
#endif
  }

  else if (params.eom_ref == 1) { /* ROHF */
    sprintf(CMNEF_lbl, "%s %d", "CMNEF", i);
    sprintf(Cmnef_lbl, "%s %d", "Cmnef", i);
    sprintf(CMnEf_lbl, "%s %d", "CMnEf", i);
    sprintf(CmNeF_lbl, "%s %d", "CmNeF", i);
    sprintf(SIJAB_lbl, "%s %d", "SIJAB", i);
    sprintf(Sijab_lbl, "%s %d", "Sijab", i);
    sprintf(SIjAb_lbl, "%s %d", "SIjAb", i);

    /* SIJAB += FBE * CIJAE - FAE * CIJBE */
    dpd_->buf4_init(&FP, PSIF_EOM_TMP, C_irr, 2, 5, 2, 5, 0, "FDD_FBEP");
    dpd_->buf4_init(&CMNEF, PSIF_EOM_CMNEF, C_irr, 2, 5, 2, 7, 0, CMNEF_lbl);
    dpd_->file2_init(&FAE, PSIF_CC_OEI, H_IRR, 1, 1, "FAE");
    dpd_->contract424(&CMNEF, &FAE, &FP, 3, 1, 0, 1.0, 0.0);
    dpd_->file2_close(&FAE);
    dpd_->buf4_close(&CMNEF);
    dpd_->buf4_sort(&FP, PSIF_EOM_TMP, pqsr, 2, 5, "FDD_FBEM");
    dpd_->buf4_init(&SIJAB, PSIF_EOM_SIJAB, C_irr, 2, 5, 2, 7, 0, SIJAB_lbl);
    dpd_->buf4_axpy(&FP, &SIJAB, 1.0);
    dpd_->buf4_close(&FP);
    dpd_->buf4_init(&FM, PSIF_EOM_TMP, C_irr, 2, 5, 2, 5, 0, "FDD_FBEM");
    dpd_->buf4_axpy(&FM, &SIJAB, -1.0);
    dpd_->buf4_close(&FM);
    dpd_->buf4_close(&SIJAB);

    /* Sijab += Fbe * Cijae - Fae * Cijbe */
    dpd_->buf4_init(&FP, PSIF_EOM_TMP, C_irr, 2, 5, 2, 5, 0, "FDD_FBEP");
    dpd_->buf4_init(&Cmnef, PSIF_EOM_Cmnef, C_irr, 2, 5, 2, 7, 0, Cmnef_lbl);
    dpd_->file2_init(&Fae, PSIF_CC_OEI, H_IRR, 1, 1, "Fae");
    dpd_->contract424(&Cmnef, &Fae, &FP, 3, 1, 0, 1.0, 0.0);
    dpd_->file2_close(&Fae);
    dpd_->buf4_close(&Cmnef);
    dpd_->buf4_sort(&FP, PSIF_EOM_TMP, pqsr, 2, 5, "FDD_FBEM");
    dpd_->buf4_init(&Sijab, PSIF_EOM_Sijab, C_irr, 2, 5, 2, 7, 0, Sijab_lbl);
    dpd_->buf4_axpy(&FP, &Sijab, 1.0);
    dpd_->buf4_close(&FP);
    dpd_->buf4_init(&FM, PSIF_EOM_TMP, C_irr, 2, 5, 2, 5, 0, "FDD_FBEM");
    dpd_->buf4_axpy(&FM, &Sijab, -1.0);
    dpd_->buf4_close(&FM);
    dpd_->buf4_close(&Sijab);

    /* SIjAb += Fbe * CIjAe - FAE * CIjbE */
    dpd_->buf4_init(&SIjAb, PSIF_EOM_SIjAb, C_irr, 0, 5, 0, 5, 0, SIjAb_lbl);
    dpd_->buf4_init(&CMnEf, PSIF_EOM_CMnEf, C_irr, 0, 5, 0, 5, 0, CMnEf_lbl);
    dpd_->file2_init(&Fae, PSIF_CC_OEI, H_IRR, 1, 1, "Fae");
    dpd_->contract424(&CMnEf, &Fae, &SIjAb, 3, 1, 0, 1.0, 1.0);
    dpd_->file2_close(&Fae);
    dpd_->file2_init(&FAE, PSIF_CC_OEI, H_IRR, 1, 1, "FAE");
    dpd_->contract244(&FAE, &CMnEf, &SIjAb, 1, 2, 1, 1.0, 1.0);
    dpd_->file2_close(&FAE);
    dpd_->buf4_close(&CMnEf);
    dpd_->buf4_close(&SIjAb);

#ifdef EOM_DEBUG
    check_sum("Fbe_FDD ",i,C_irr);
#endif

    /* SIJAB -= FMJ * CIMAB - FMI * CJMAB */
    dpd_->buf4_init(&FM, PSIF_EOM_TMP, C_irr, 0, 7, 0, 7, 0, "FDD_FMJM");
    dpd_->buf4_init(&CMNEF, PSIF_EOM_CMNEF, C_irr, 0, 7, 2, 7, 0, CMNEF_lbl);
    dpd_->file2_init(&FMI, PSIF_CC_OEI, H_IRR, 0, 0, "FMI");
    dpd_->contract424(&CMNEF, &FMI, &FM, 1, 0, 1, 1.0, 0.0);
    dpd_->file2_close(&FMI);
    dpd_->buf4_close(&CMNEF);
    dpd_->buf4_sort(&FM, PSIF_EOM_TMP, qprs, 0, 7, "FDD_FMJP");
    dpd_->buf4_init(&SIJAB, PSIF_EOM_SIJAB, C_irr, 0, 7, 2, 7, 0, SIJAB_lbl);
    dpd_->buf4_axpy(&FM, &SIJAB, -1.0);
    dpd_->buf4_close(&FM);
    dpd_->buf4_init(&FP, PSIF_EOM_TMP, C_irr, 0, 7, 0, 7, 0, "FDD_FMJP");
    dpd_->buf4_axpy(&FP, &SIJAB, 1.0);
    dpd_->buf4_close(&FP);
    dpd_->buf4_close(&SIJAB);

    /* Sijab -= Fmj * Cimab - Fmi * Cjmab */
    dpd_->buf4_init(&FM, PSIF_EOM_TMP, C_irr, 0, 7, 0, 7, 0, "FDD_FMJM");
    dpd_->buf4_init(&Cmnef, PSIF_EOM_Cmnef, C_irr, 0, 7, 2, 7, 0, Cmnef_lbl);
    dpd_->file2_init(&Fmi, PSIF_CC_OEI, H_IRR, 0, 0, "Fmi");
    dpd_->contract424(&Cmnef, &Fmi, &FM, 1, 0, 1, 1.0, 0.0);
    dpd_->file2_close(&Fmi);
    dpd_->buf4_close(&Cmnef);
    dpd_->buf4_sort(&FM, PSIF_EOM_TMP, qprs, 0, 7, "FDD_FMJP");
    dpd_->buf4_init(&Sijab, PSIF_EOM_Sijab, C_irr, 0, 7, 2, 7, 0, Sijab_lbl);
    dpd_->buf4_axpy(&FM, &Sijab, -1.0);
    dpd_->buf4_close(&FM);
    dpd_->buf4_init(&FP, PSIF_EOM_TMP, C_irr, 0, 7, 0, 7, 0, "FDD_FMJP");
    dpd_->buf4_axpy(&FP, &Sijab, 1.0);
    dpd_->buf4_close(&FP);
    dpd_->buf4_close(&Sijab);

    /* SIjAb -= Fmj * CImAb - FMI * CjMAb */
    dpd_->buf4_init(&SIjAb, PSIF_EOM_SIjAb, C_irr, 0, 5, 0, 5, 0, SIjAb_lbl);
    dpd_->buf4_init(&CMnEf, PSIF_EOM_CMnEf, C_irr, 0, 5, 0, 5, 0, CMnEf_lbl);
    dpd_->file2_init(&Fmi, PSIF_CC_OEI, H_IRR, 0, 0, "Fmi");
    dpd_->contract424(&CMnEf, &Fmi, &SIjAb, 1, 0, 1, -1.0, 1.0);
    dpd_->file2_close(&Fmi);
    dpd_->file2_init(&FMI, PSIF_CC_OEI, H_IRR, 0, 0, "FMI");
    dpd_->contract244(&FMI, &CMnEf, &SIjAb, 0, 0, 0, -1.0, 1.0);
    dpd_->file2_close(&FMI);
    dpd_->buf4_close(&CMnEf);
    dpd_->buf4_close(&SIjAb);

#ifdef EOM_DEBUG
    check_sum("Fmj_DD",i,C_irr);
#endif
  }

  else if (params.eom_ref == 2) { /* UHF */
    sprintf(CMNEF_lbl, "%s %d", "CMNEF", i);
    sprintf(Cmnef_lbl, "%s %d", "Cmnef", i);
    sprintf(CMnEf_lbl, "%s %d", "CMnEf", i);
    sprintf(CmNeF_lbl, "%s %d", "CmNeF", i);
    sprintf(SIJAB_lbl, "%s %d", "SIJAB", i);
    sprintf(Sijab_lbl, "%s %d", "Sijab", i);
    sprintf(SIjAb_lbl, "%s %d", "SIjAb", i);

    /* SIJAB += FBE * CIJAE - FAE * CIJBE */
    dpd_->buf4_init(&FP, PSIF_EOM_TMP, C_irr, 2, 5, 2, 5, 0, "FDD_FBEP");
    dpd_->buf4_init(&CMNEF, PSIF_EOM_CMNEF, C_irr, 2, 5, 2, 7, 0, CMNEF_lbl);
    dpd_->file2_init(&FAE, PSIF_CC_OEI, H_IRR, 1, 1, "FAE");
    dpd_->contract424(&CMNEF, &FAE, &FP, 3, 1, 0, 1.0, 0.0);
    dpd_->file2_close(&FAE);
    dpd_->buf4_close(&CMNEF);
    dpd_->buf4_sort(&FP, PSIF_EOM_TMP, pqsr, 2, 5, "FDD_FBEM");
    dpd_->buf4_init(&SIJAB, PSIF_EOM_SIJAB, C_irr, 2, 5, 2, 7, 0, SIJAB_lbl);
    dpd_->buf4_axpy(&FP, &SIJAB, 1.0);
    dpd_->buf4_close(&FP);
    dpd_->buf4_init(&FM, PSIF_EOM_TMP, C_irr, 2, 5, 2, 5, 0, "FDD_FBEM");
    dpd_->buf4_axpy(&FM, &SIJAB, -1.0);
    dpd_->buf4_close(&FM);
    dpd_->buf4_close(&SIJAB);

    /* Sijab += Fbe * Cijae - Fae * Cijbe */
    dpd_->buf4_init(&FP, PSIF_EOM_TMP, C_irr, 12, 15, 12, 15, 0, "FDD_FbePB");
    dpd_->buf4_init(&Cmnef, PSIF_EOM_Cmnef, C_irr, 12, 15, 12, 17, 0, Cmnef_lbl);
    dpd_->file2_init(&Fae, PSIF_CC_OEI, H_IRR, 3, 3, "Fae");
    dpd_->contract424(&Cmnef, &Fae, &FP, 3, 1, 0, 1.0, 0.0);
    dpd_->file2_close(&Fae);
    dpd_->buf4_close(&Cmnef);
    dpd_->buf4_sort(&FP, PSIF_EOM_TMP, pqsr, 12, 15, "FDD_FbeMB");
    dpd_->buf4_init(&Sijab, PSIF_EOM_Sijab, C_irr, 12, 15, 12, 17, 0, Sijab_lbl);
    dpd_->buf4_axpy(&FP, &Sijab, 1.0);
    dpd_->buf4_close(&FP);
    dpd_->buf4_init(&FM, PSIF_EOM_TMP, C_irr, 12, 15, 12, 15, 0, "FDD_FbeMB");
    dpd_->buf4_axpy(&FM, &Sijab, -1.0);
    dpd_->buf4_close(&FM);
    dpd_->buf4_close(&Sijab);

    /* SIjAb += Fbe * CIjAe - FAE * CIjbE */
    dpd_->buf4_init(&SIjAb, PSIF_EOM_SIjAb, C_irr, 22, 28, 22, 28, 0, SIjAb_lbl);
    dpd_->buf4_init(&CMnEf, PSIF_EOM_CMnEf, C_irr, 22, 28, 22, 28, 0, CMnEf_lbl);
    dpd_->file2_init(&Fae, PSIF_CC_OEI, H_IRR, 3, 3, "Fae");
    dpd_->contract424(&CMnEf, &Fae, &SIjAb, 3, 1, 0, 1.0, 1.0);
    dpd_->file2_close(&Fae);
    dpd_->file2_init(&FAE, PSIF_CC_OEI, H_IRR, 1, 1, "FAE");
    dpd_->contract244(&FAE, &CMnEf, &SIjAb, 1, 2, 1, 1.0, 1.0);
    dpd_->file2_close(&FAE);
    dpd_->buf4_close(&CMnEf);
    dpd_->buf4_close(&SIjAb);

#ifdef EOM_DEBUG
    check_sum("Fbe_FDD ",i,C_irr);
#endif

    /* SIJAB -= FMJ * CIMAB - FMI * CJMAB */
    dpd_->buf4_init(&FM, PSIF_EOM_TMP, C_irr, 0, 7, 0, 7, 0, "FDD_FMJM");
    dpd_->buf4_init(&CMNEF, PSIF_EOM_CMNEF, C_irr, 0, 7, 2, 7, 0, CMNEF_lbl);
    dpd_->file2_init(&FMI, PSIF_CC_OEI, H_IRR, 0, 0, "FMI");
    dpd_->contract424(&CMNEF, &FMI, &FM, 1, 0, 1, 1.0, 0.0);
    dpd_->file2_close(&FMI);
    dpd_->buf4_close(&CMNEF);
    dpd_->buf4_sort(&FM, PSIF_EOM_TMP, qprs, 0, 7, "FDD_FMJP");
    dpd_->buf4_init(&SIJAB, PSIF_EOM_SIJAB, C_irr, 0, 7, 2, 7, 0, SIJAB_lbl);
    dpd_->buf4_axpy(&FM, &SIJAB, -1.0);
    dpd_->buf4_close(&FM);
    dpd_->buf4_init(&FP, PSIF_EOM_TMP, C_irr, 0, 7, 0, 7, 0, "FDD_FMJP");
    dpd_->buf4_axpy(&FP, &SIJAB, 1.0);
    dpd_->buf4_close(&FP);
    dpd_->buf4_close(&SIJAB);

    /* Sijab -= Fmj * Cimab - Fmi * Cjmab */
    dpd_->buf4_init(&FM, PSIF_EOM_TMP, C_irr, 10, 17, 10, 17, 0, "FDD_FmjMB");
    dpd_->buf4_init(&Cmnef, PSIF_EOM_Cmnef, C_irr, 10, 17, 12, 17, 0, Cmnef_lbl);
    dpd_->file2_init(&Fmi, PSIF_CC_OEI, H_IRR, 2, 2, "Fmi");
    dpd_->contract424(&Cmnef, &Fmi, &FM, 1, 0, 1, 1.0, 0.0);
    dpd_->file2_close(&Fmi);
    dpd_->buf4_close(&Cmnef);
    dpd_->buf4_sort(&FM, PSIF_EOM_TMP, qprs, 10, 17, "FDD_FmjPB");
    dpd_->buf4_init(&Sijab, PSIF_EOM_Sijab, C_irr, 10, 17, 12, 17, 0, Sijab_lbl);
    dpd_->buf4_axpy(&FM, &Sijab, -1.0);
    dpd_->buf4_close(&FM);
    dpd_->buf4_init(&FP, PSIF_EOM_TMP, C_irr, 10, 17, 10, 17, 0, "FDD_FmjPB");
    dpd_->buf4_axpy(&FP, &Sijab, 1.0);
    dpd_->buf4_close(&FP);
    dpd_->buf4_close(&Sijab);

    /* SIjAb -= Fmj * CImAb - FMI * CjMAb */
    dpd_->buf4_init(&SIjAb, PSIF_EOM_SIjAb, C_irr, 22, 28, 22, 28, 0, SIjAb_lbl);
    dpd_->buf4_init(&CMnEf, PSIF_EOM_CMnEf, C_irr, 22, 28, 22, 28, 0, CMnEf_lbl);
    dpd_->file2_init(&Fmi, PSIF_CC_OEI, H_IRR, 2, 2, "Fmi");
    dpd_->contract424(&CMnEf, &Fmi, &SIjAb, 1, 0, 1, -1.0, 1.0);
    dpd_->file2_close(&Fmi);
    dpd_->file2_init(&FMI, PSIF_CC_OEI, H_IRR, 0, 0, "FMI");
    dpd_->contract244(&FMI, &CMnEf, &SIjAb, 0, 0, 0, -1.0, 1.0);
    dpd_->file2_close(&FMI);
    dpd_->buf4_close(&CMnEf);
    dpd_->buf4_close(&SIjAb);

#ifdef EOM_DEBUG
    check_sum("Fmj_DD",i,C_irr);
#endif
  }
  return;
}

}} // namespace psi::cceom
