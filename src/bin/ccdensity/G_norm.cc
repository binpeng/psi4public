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
    \ingroup CCDENSITY
    \brief Enter brief description of file here 
*/
#include <cstdio>
#include <libdpd/dpd.h>
#include "MOInfo.h"
#include "Params.h"
#include "Frozen.h"
#define EXTERN
#include "globals.h"

namespace psi { namespace ccdensity {
                                                                                                    
void G_norm(void) {
  dpdfile2 G1;
  dpdbuf4 G;
  double value, value1, dot_IA, dot_ia, dot_AI, dot_ai;
  int G_irr = 0;

  fprintf(outfile,"Calculating overlaps of CC_OEI\n");
  dpd_->file2_init(&G1, PSIF_CC_OEI, G_irr, 0, 1, "DIA");
  dot_IA = dpd_->file2_dot_self(&G1);
  dpd_->file2_close(&G1);
  dpd_->file2_init(&G1, PSIF_CC_OEI, G_irr, 0, 1, "Dia");
  dot_ia = dpd_->file2_dot_self(&G1);
  dpd_->file2_close(&G1);
  dpd_->file2_init(&G1, PSIF_CC_OEI, G_irr, 0, 1, "DAI");
  dot_AI = dpd_->file2_dot_self(&G1);
  dpd_->file2_close(&G1);
  dpd_->file2_init(&G1, PSIF_CC_OEI, G_irr, 0, 1, "Dai");
  dot_ai = dpd_->file2_dot_self(&G1);
  dpd_->file2_close(&G1);
  /*
  fprintf(outfile,"<DIA|DIA> = %15.10lf\n", dot_IA);
  fprintf(outfile,"<Dia|Dia> = %15.10lf\n", dot_ia);
  fprintf(outfile,"<DAI|DAI> = %15.10lf\n", dot_AI);
  fprintf(outfile,"<Dai|Dai> = %15.10lf\n", dot_ai);
  */
  fprintf(outfile,"\t<Dpq|Dqp>     = %15.10lf\n", dot_IA+dot_ia+dot_AI+dot_ai);

  fprintf(outfile,"Calculating overlaps of CC_GAMMA\n");

  dpd_->buf4_init(&G, PSIF_CC_GAMMA, G_irr, 2, 2, 2, 2, 0, "GIJKL");
  value = dpd_->buf4_dot_self(&G);
  dpd_->buf4_close(&G);
  dpd_->buf4_init(&G, PSIF_CC_GAMMA, G_irr, 2, 2, 2, 2, 0, "Gijkl");
  value += dpd_->buf4_dot_self(&G);
  dpd_->buf4_close(&G);
  dpd_->buf4_init(&G, PSIF_CC_GAMMA, G_irr, 0, 0, 0, 0, 0, "GIjKl");
  value += dpd_->buf4_dot_self(&G);
  dpd_->buf4_close(&G);
  fprintf(outfile,"\t<Gijkl|Gijkl> = %15.10lf\n", value);

  dpd_->buf4_init(&G, PSIF_CC_GAMMA, G_irr, 2, 10, 2, 10, 0, "GIJKA");
  value = dpd_->buf4_dot_self(&G);
  dpd_->buf4_close(&G);
  dpd_->buf4_init(&G, PSIF_CC_GAMMA, G_irr, 2, 10, 2, 10, 0, "Gijka");
  value += dpd_->buf4_dot_self(&G);
  dpd_->buf4_close(&G);
  dpd_->buf4_init(&G, PSIF_CC_GAMMA, G_irr, 0, 10, 0, 10, 0, "GIjKa");
  value += dpd_->buf4_dot_self(&G);
  dpd_->buf4_close(&G);
  dpd_->buf4_init(&G, PSIF_CC_GAMMA, G_irr, 0, 10, 0, 10, 0, "GiJkA");
  value += dpd_->buf4_dot_self(&G);
  dpd_->buf4_close(&G);
  fprintf(outfile,"\t<Gijka|Gijka> = %15.10lf\n",value);

  dpd_->buf4_init(&G, PSIF_CC_GAMMA, G_irr, 2, 7, 2, 7, 0, "GIJAB");
  value = dpd_->buf4_dot_self(&G);
  dpd_->buf4_close(&G);
  dpd_->buf4_init(&G, PSIF_CC_GAMMA, G_irr, 2, 7, 2, 7, 0, "Gijab");
  value += dpd_->buf4_dot_self(&G);
  dpd_->buf4_close(&G);
  dpd_->buf4_init(&G, PSIF_CC_GAMMA, G_irr, 0, 5, 0, 5, 0, "GIjAb");
  value += dpd_->buf4_dot_self(&G);
  dpd_->buf4_close(&G);
  fprintf(outfile,"\t<Gijab|Gijab> = %15.10lf\n", value);


  dpd_->buf4_init(&G, PSIF_CC_GAMMA, G_irr, 10, 10, 10, 10, 0, "GIBJA");
  value = dpd_->buf4_dot_self(&G);
  dpd_->buf4_close(&G);
  dpd_->buf4_init(&G, PSIF_CC_GAMMA, G_irr, 10, 10, 10, 10, 0, "Gibja");
  value += dpd_->buf4_dot_self(&G);
  dpd_->buf4_close(&G);
  dpd_->buf4_init(&G, PSIF_CC_GAMMA, G_irr, 10, 10, 10, 10, 0, "GIbJa");
  value += dpd_->buf4_dot_self(&G);
  dpd_->buf4_close(&G);
  dpd_->buf4_init(&G, PSIF_CC_GAMMA, G_irr, 10, 10, 10, 10, 0, "GiBjA");
  value += dpd_->buf4_dot_self(&G);
  dpd_->buf4_close(&G);
  dpd_->buf4_init(&G, PSIF_CC_GAMMA, G_irr, 10, 10, 10, 10, 0, "GIbjA");
  value += dpd_->buf4_dot_self(&G);
  dpd_->buf4_close(&G);
  dpd_->buf4_init(&G, PSIF_CC_GAMMA, G_irr, 10, 10, 10, 10, 0, "GiBJa");
  value += dpd_->buf4_dot_self(&G);
  dpd_->buf4_close(&G);
  fprintf(outfile,"\t<Gibja|Gibja> = %15.10lf\n",value);

  dpd_->buf4_init(&G, PSIF_CC_GAMMA, G_irr, 11, 7, 11, 7, 0, "GCIAB");
  value = dpd_->buf4_dot_self(&G);
  dpd_->buf4_close(&G);
  dpd_->buf4_init(&G, PSIF_CC_GAMMA, G_irr, 11, 7, 11, 7, 0, "Gciab");
  value += dpd_->buf4_dot_self(&G);
  dpd_->buf4_close(&G);
  dpd_->buf4_init(&G, PSIF_CC_GAMMA, G_irr, 11, 5, 11, 5, 0, "GCiAb");
  value += dpd_->buf4_dot_self(&G);
  dpd_->buf4_close(&G);
  dpd_->buf4_init(&G, PSIF_CC_GAMMA, G_irr, 11, 5, 11, 5, 0, "GcIaB");
  value += dpd_->buf4_dot_self(&G);
  dpd_->buf4_close(&G);
  fprintf(outfile,"\t<Gciab|Gciab> = %15.10lf\n",value);

  dpd_->buf4_init(&G, PSIF_CC_GAMMA, G_irr, 7, 7, 7, 7, 0, "GABCD");
  value = dpd_->buf4_dot_self(&G);
  dpd_->buf4_close(&G);
  dpd_->buf4_init(&G, PSIF_CC_GAMMA, G_irr, 7, 7, 7, 7, 0, "Gabcd");
  value += dpd_->buf4_dot_self(&G);
  dpd_->buf4_close(&G);
  dpd_->buf4_init(&G, PSIF_CC_GAMMA, G_irr, 5, 5, 5, 5, 0, "GAbCd");
  value += dpd_->buf4_dot_self(&G);
  dpd_->buf4_close(&G);
  fprintf(outfile,"\t<Gabcd|Gabcd> = %15.10lf\n", value);

  return;
}

}} // namespace psi::ccdensity
