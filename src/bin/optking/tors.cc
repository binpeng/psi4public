/*! \file tors.cc
    \ingroup OPT10
    \brief tors class
*/

#include "tors.h"

#include <string>
#include <iostream>
#include <sstream>

#include "print.h"
#include "v3d.h"
#include "physconst.h"
#include "opt_params.h"

namespace opt {

extern OPT_PARAMS Opt_params;
using namespace v3d;
using std::ostringstream;

// constructor - canonical order is A < D
TORS::TORS(int A_in, int B_in, int C_in, int D_in) : SIMPLE(tors_type, 4) {
  //fprintf(stdout,"constructing TORS A_in:%d B_in:%d C_in:%d D_in: %d\n", A_in, B_in, C_in, D_in);
  if ( A_in==B_in || A_in==C_in || A_in==D_in || B_in==C_in || B_in==D_in || C_in==D_in)
    throw("TORS::TORS() Atoms defining tors are not unique.");

  if (A_in < D_in) {
    s_atom[0] = A_in;
    s_atom[1] = B_in;
    s_atom[2] = C_in;
    s_atom[3] = D_in;
  }
  else {
    s_atom[0] = D_in;
    s_atom[1] = C_in;
    s_atom[2] = B_in;
    s_atom[3] = A_in;
  }
  near_180 = 0;
}

void TORS::fix_near_180(GeomType geom) {
  double tval = value(geom);
  if ( tval > Opt_params.fix_tors_near_pi)
    near_180 = +1;
  else if ( tval < -1*Opt_params.fix_tors_near_pi)
    near_180 = -1;
  else
    near_180 = 0;
  return;
}

// compute angle and store value in radians
double TORS::value(GeomType geom) const {
  double tau;

  if (! v3d_tors(geom[s_atom[0]], geom[s_atom[1]], geom[s_atom[2]], geom[s_atom[3]], tau) )
    throw("TORS::compute_val: bond angles will not permit torsion computation");

  // Extend domain of torsion angles by checking past
  // extend domain of torsions so delta(vals) can be calculated
  if (near_180 == -1 && tau > Opt_params.fix_tors_near_pi)
    return (-_pi - (_pi - tau));
  else if (near_180 == +1 && tau < -1*Opt_params.fix_tors_near_pi)
    return (+_pi + (_pi + tau));
  else
    return tau;
}

inline int zeta(const int a, const int m, const int n) {
  if (a == m) return 1;
  else if (a == n) return -1;
  else return 0;
}

inline int delta(const int i, const int j) {
  if (i == j) return 1;
  else return 0;
}


// torsion is m-o-p-n
double ** TORS::DqDx(GeomType geom) const {
  double **dqdx = init_matrix(4,3);

  double u[3], v[3], w[3];
  v3d_axpy(-1, geom[s_atom[1]], geom[s_atom[0]], u); // u=m-o eBA
  v3d_axpy(-1, geom[s_atom[2]], geom[s_atom[3]], v); // v=n-p eCD
  v3d_axpy(-1, geom[s_atom[1]], geom[s_atom[2]], w); // w=p-o eBC
  double Lu = v3d_norm(u); // RBA
  double Lv = v3d_norm(v); // RCD
  double Lw = v3d_norm(w); // RBC
  v3d_scm(1.0/Lu, u);  // eBA
  v3d_scm(1.0/Lv, v);  // eCD
  v3d_scm(1.0/Lw, w);  // eBC

  double cos_u = v3d_dot(u,w);
  double cos_v = - v3d_dot(v,w);
  if (1.0 - cos_u*cos_u <= 1.0e-12) return dqdx; // abort and leave zero if 0 or 180 angle
  if (1.0 - cos_v*cos_v <= 1.0e-12) return dqdx; // abort and leave zero if 0 or 180 angle
  double sin_u = sqrt(1.0 - cos_u*cos_u);
  double sin_v = sqrt(1.0 - cos_v*cos_v);

  double uXw[3], vXw[3];
  v3d_cross_product(u, w, uXw);
  v3d_cross_product(v, w, vXw);

  double tval1, tval2, tval3;
  for (int a=0; a<4; ++a) // atoms
    for (int i=0; i<3; ++i) {  //i=a_xyz
      tval1 = tval2 = tval3 = 0.0;

      if ((a == 0) || (a == 1))
        tval1 = zeta(a,0,1) * uXw[i] / (Lu*sin_u*sin_u);

      if ((a == 2) || (a == 3))
        tval2 = zeta(a,2,3) * vXw[i] / (Lv*sin_v*sin_v);

      if ((a == 1) || (a == 2)) // "+" sign below differs from JCP, 117, 9164 (2002)
        tval3 = zeta(a,1,2) * (uXw[i]*cos_u/(Lw*sin_u*sin_u) + vXw[i]*cos_v/(Lw*sin_v*sin_v));

      dqdx[a][i] = tval1 + tval2 + tval3;
    }

  return dqdx;
}

// torsion is m-o-p-n
double ** TORS::Dq2Dx2(GeomType geom) const {
  double **dq2dx2 = init_matrix(12,12);

  double u[3], v[3], w[3];
  v3d_axpy(-1, geom[s_atom[1]], geom[s_atom[0]], u); // u=m-o eBA
  v3d_axpy(-1, geom[s_atom[2]], geom[s_atom[3]], v); // v=n-p eCD
  v3d_axpy(-1, geom[s_atom[1]], geom[s_atom[2]], w); // w=p-o eBC
  double Lu, Lv, Lw;
  Lu = v3d_norm(u); // RBA 
  Lv = v3d_norm(v); // RCD 
  Lw = v3d_norm(w); // RBC 
  v3d_scm(1.0/Lu, u);  // eBA
  v3d_scm(1.0/Lv, v);  // eCD
  v3d_scm(1.0/Lw, w);  // eBC

  double cos_u = v3d_dot(u,w);
  double cos_v = - v3d_dot(v,w);
  if (1.0 - cos_u*cos_u <= 1.0e-12) return dq2dx2; // abort and leave zero if 0 or 180 angle
  if (1.0 - cos_v*cos_v <= 1.0e-12) return dq2dx2; // abort and leave zero if 0 or 180 angle
  double sin_u = sqrt(1.0 - cos_u*cos_u);
  double sin_v = sqrt(1.0 - cos_v*cos_v);

  double uXw[3], vXw[3];
  v3d_cross_product(u, w, uXw);
  v3d_cross_product(v, w, vXw);

  double sinu4 = sin_u*sin_u*sin_u*sin_u;
  double sinv4 = sin_v*sin_v*sin_v*sin_v;
  double cosu3 = cos_u*cos_u*cos_u;
  double cosv3 = cos_v*cos_v*cos_v;

  double tval;
  int k; // cartesian ; not i or j
  for (int a=0; a<4; ++a)
    for (int i=0; i<3; ++i) //i = a_xyz
      for (int b=0; b<4; ++b)
        for (int j=0; j<3; ++j) { //j=b_xyz

          tval = zeta(a,0,1)*zeta(b,0,1)*(uXw[i]*(w[j]*cos_u-u[j]) + uXw[j]*(w[i]*cos_u-u[i]))
                   / (Lu*Lu*sinu4);

          tval +=zeta(a,3,2)*zeta(b,3,2)*(vXw[i]*(w[j]*cos_v-v[j]) + vXw[j]*(w[i]*cos_v-v[i]))
                   / (Lv*Lv*sinv4);

          tval += (zeta(a,0,1)*zeta(b,1,2)+zeta(a,2,1)*zeta(b,1,0)) *
            (uXw[i] * (w[j]-2*u[j]*cos_u+w[j]*cos_u*cos_u) +
             uXw[j] * (w[i]-2*u[i]*cos_u+w[i]*cos_u*cos_u)) / (2*Lu*Lw*sinu4);

          tval += (zeta(a,3,2)*zeta(b,2,1)+zeta(a,2,1)*zeta(b,3,2)) *
            (vXw[i] * (w[j]+2*u[j]*cos_v+w[j]*cos_v*cos_v) +
             vXw[j] * (w[i]+2*u[i]*cos_v+w[i]*cos_v*cos_v)) / (2*Lu*Lw*sinv4);

          tval += zeta(a,1,2)*zeta(b,2,1)*
            (uXw[i]*(u[j]+u[j]*cos_u*cos_u-3*w[j]*cos_u+w[j]*cosu3) +
             uXw[j]*(u[i]+u[i]*cos_u*cos_u-3*w[i]*cos_u+w[i]*cosu3)) / (2*Lw*Lw*sinu4);
             
          tval += zeta(a,1,2)*zeta(b,1,2)*
            (vXw[i]*(v[j]+v[j]*cos_v*cos_v+3*w[j]*cos_v-w[j]*cosv3) +
             vXw[j]*(v[i]+v[i]*cos_v*cos_v+3*w[i]*cos_v-w[i]*cosv3)) / (2*Lw*Lw*sinv4);

          if ((a != b) && (i != j)) {
            if ( i!=0 && j!=0 ) k = 0; // k is unique coordinate not i or j
            else if ( i!=1 && j!=1 ) k = 1;
            else k = 2;

            tval += (zeta(a,0,1)*zeta(b,1,2)+zeta(a,2,1)*zeta(b,1,0)) * (j-i) *
              pow(-0.5,fabs(j-i)) * (w[k]*cos_u - u[k]) / (Lu*Lw*sin_u);

            tval += (zeta(a,3,1)*zeta(b,1,2)+zeta(a,2,1)*zeta(b,1,0)) * (j-i) *
              pow(-0.5,fabs(j-i)) * (w[k]*cos_v - v[k]) / (Lv*Lw*sin_v);
          }
          dq2dx2[3*a+i][3*b+j] = tval;
        }
}


void TORS::print(FILE *fp, GeomType geom) const {
  ostringstream iss(ostringstream::out); // create stream; allow output to it
  iss << "T(" << s_atom[0]+1 << "," << s_atom[1]+1 << "," << s_atom[2]+1 << "," << s_atom[3]+1 << ")";
  double val = value(geom);
  fprintf(fp,"\t %-15s  =  %15.6lf\t%15.6lf\n",
    iss.str().c_str(), val, val/_pi*180.0);
}

void TORS::print_disp(FILE *fp, const double q_old, const double f_q,
    const double dq, const double q_new) const {
  ostringstream iss(ostringstream::out); // create stream; allow output to it
  iss << "T(" << s_atom[0]+1 << "," << s_atom[1]+1 << "," << s_atom[2]+1 << "," << s_atom[3]+1 << ")";
  fprintf(fp,"\t %-15s = %13.6lf%13.6lf%13.6lf%13.6lf\n",
    iss.str().c_str(), q_old/_pi*180.0, f_q*_pi/180.0,dq/_pi*180.0, q_new/_pi*180.0);
}

void TORS::print_intco_dat(FILE *fp) const {
  fprintf(fp, "S%6d%6d%6d%6d\n", s_atom[0]+1, s_atom[1]+1, s_atom[2]+1, s_atom[3]+1);
}

void TORS::print_s(FILE *fp, GeomType geom) const {
  fprintf(fp,"S vector for tors, T(%d %d %d %d): \n",
    s_atom[0]+1, s_atom[1]+1, s_atom[2]+1, s_atom[3]+1);
  double **dqdx = DqDx(geom);
  fprintf(fp, "Atom 1: %12.8f %12.8f,%12.8f\n", dqdx[0][0], dqdx[0][1], dqdx[0][2]);
  fprintf(fp, "Atom 2: %12.8f %12.8f,%12.8f\n", dqdx[1][0], dqdx[1][1], dqdx[1][2]);
  fprintf(fp, "Atom 3: %12.8f %12.8f,%12.8f\n", dqdx[2][0], dqdx[2][1], dqdx[2][2]);
  fprintf(fp, "Atom 4: %12.8f %12.8f,%12.8f\n", dqdx[3][0], dqdx[3][1], dqdx[3][2]);
  free_matrix(dqdx);
}

bool TORS::operator==(const SIMPLE & s2) const {
  fprintf(stdout,"TORS::operator==\n");
  if (tors_type != s2.g_type())
    return false;

  if (this->s_atom[0] == s2.g_atom(0) && this->s_atom[1] == s2.g_atom(1) &&
      this->s_atom[2] == s2.g_atom(2) && this->s_atom[3] == s2.g_atom(3) )
        return true;
  else if (this->s_atom[0] == s2.g_atom(3) && this->s_atom[1] == s2.g_atom(2) &&
           this->s_atom[2] == s2.g_atom(1) && this->s_atom[3] == s2.g_atom(0) )
        return true;
  else
        return false;
}

}

