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
    \ingroup CCENERGY
    \brief Enter brief description of file here 
*/
#include <cstdio>
#include <cstdlib>
#include <libdpd/dpd.h>
#include "Params.h"
#define EXTERN
#include "globals.h"

namespace psi { namespace ccenergy {

void local_filter_T2(dpdbuf4 *T2);

void dijabT2(void)
{
  dpdbuf4 newtIJAB, newtijab, newtIjAb, tIjAb;
  dpdbuf4 dIJAB, dijab, dIjAb;

  if(params.ref == 0) { /*** RHF ***/
    dpd_->buf4_init(&newtIjAb, PSIF_CC_TAMPS, 0, 0, 5, 0, 5, 0, "New tIjAb");
    dpd_->buf4_copy(&newtIjAb, PSIF_CC_TAMPS, "New tIjAb Increment");
    dpd_->buf4_close(&newtIjAb);

    dpd_->buf4_init(&newtIjAb, PSIF_CC_TAMPS, 0, 0, 5, 0, 5, 0, "New tIjAb Increment");
    if(params.local) {
      local_filter_T2(&newtIjAb);
    }
    else {
      dpd_->buf4_init(&dIjAb, PSIF_CC_DENOM, 0, 0, 5, 0, 5, 0, "dIjAb");
      dpd_->buf4_dirprd(&dIjAb, &newtIjAb);
      dpd_->buf4_close(&dIjAb);
    }
    dpd_->buf4_close(&newtIjAb);

    /* Add the new increment to the old tIjAb to get the new tIjAb */
    dpd_->buf4_init(&tIjAb, PSIF_CC_TAMPS, 0, 0, 5, 0, 5, 0, "tIjAb");
    dpd_->buf4_copy(&tIjAb, PSIF_CC_TAMPS, "New tIjAb");
    dpd_->buf4_close(&tIjAb);
    dpd_->buf4_init(&newtIjAb, PSIF_CC_TAMPS, 0, 0, 5, 0, 5, 0, "New tIjAb");
    dpd_->buf4_init(&tIjAb, PSIF_CC_TAMPS, 0, 0, 5, 0, 5, 0, "New tIjAb Increment");
    dpd_->buf4_axpy(&tIjAb, &newtIjAb, 1);
    dpd_->buf4_close(&tIjAb);
    dpd_->buf4_close(&newtIjAb);
  }
  else if(params.ref == 1) { /*** ROHF ***/
    dpd_->buf4_init(&newtIJAB, PSIF_CC_TAMPS, 0, 2, 7, 2, 7, 0, "New tIJAB");
    dpd_->buf4_init(&dIJAB, PSIF_CC_DENOM, 0, 1, 6, 1, 6, 0, "dIJAB");
    dpd_->buf4_dirprd(&dIJAB, &newtIJAB);
    dpd_->buf4_close(&newtIJAB);
    dpd_->buf4_close(&dIJAB);

    dpd_->buf4_init(&newtijab, PSIF_CC_TAMPS, 0, 2, 7, 2, 7, 0, "New tijab");
    dpd_->buf4_init(&dijab, PSIF_CC_DENOM, 0, 1, 6, 1, 6, 0, "dijab");
    dpd_->buf4_dirprd(&dijab, &newtijab);
    dpd_->buf4_close(&newtijab);
    dpd_->buf4_close(&dijab);

    dpd_->buf4_init(&newtIjAb, PSIF_CC_TAMPS, 0, 0, 5, 0, 5, 0, "New tIjAb");
    dpd_->buf4_init(&dIjAb, PSIF_CC_DENOM, 0, 0, 5, 0, 5, 0, "dIjAb");
    dpd_->buf4_dirprd(&dIjAb, &newtIjAb);
    dpd_->buf4_close(&newtIjAb);
    dpd_->buf4_close(&dIjAb);
  }
  else if(params.ref ==2) { /*** UHF ***/
    dpd_->buf4_init(&newtIJAB, PSIF_CC_TAMPS, 0, 2, 7, 2, 7, 0, "New tIJAB");
    dpd_->buf4_init(&dIJAB, PSIF_CC_DENOM, 0, 1, 6, 1, 6, 0, "dIJAB");
    dpd_->buf4_dirprd(&dIJAB, &newtIJAB);
    dpd_->buf4_close(&dIJAB);
    /*    dpd_buf4_print(&newtIJAB, outfile, 1); */
    dpd_->buf4_close(&newtIJAB);

    dpd_->buf4_init(&newtijab, PSIF_CC_TAMPS, 0, 12, 17, 12, 17, 0, "New tijab");
    dpd_->buf4_init(&dijab, PSIF_CC_DENOM, 0, 11, 16, 11, 16, 0, "dijab");
    dpd_->buf4_dirprd(&dijab, &newtijab);
    dpd_->buf4_close(&dijab);
    /*    dpd_buf4_print(&newtijab, outfile, 1); */
    dpd_->buf4_close(&newtijab);

    dpd_->buf4_init(&newtIjAb, PSIF_CC_TAMPS, 0, 22, 28, 22, 28, 0, "New tIjAb");
    dpd_->buf4_init(&dIjAb, PSIF_CC_DENOM, 0, 22, 28, 22, 28, 0, "dIjAb");
    dpd_->buf4_dirprd(&dIjAb, &newtIjAb);
    dpd_->buf4_close(&dIjAb);
    /*    dpd_buf4_print(&newtIjAb, outfile, 1); */
    dpd_->buf4_close(&newtIjAb);
  }
}
}} // namespace psi::ccenergy
