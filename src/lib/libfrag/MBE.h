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

#ifndef MBE_H_
#define MBE_H_
#include <set>
#include <utility>
#include "GMBE.h"
#include <boost/math/special_functions/binomial.hpp>
namespace psi {
namespace LibFrag {

/** \brief Specialization of the GMBE to non-intersecting fragments
 *
 *  For the case of the MBE an \f$n\f$-body property \f$P\f$ of an
 *  \f$N\f$ fragment system can be written:
 *  \f[
 *  P\approx\sum_{i=1}^n(-1)^{n-i}{_{N-i-1}}C_{n-i}P^{(i)},
 *  \f]
 *  where \f${_{\alpha}}C_{\beta}\f$ is \f$\alpha\f$ choose \f$\beta\f$
 *  and \f$P^{(i)}\f$ is the sum of the property over all $i$-mers.
 *
 */
class MBE:public ExpanImplBase {
   private:
      ///Returns N choose M
      double Binomial(const int N, const int M)const {
         return boost::math::binomial_coefficient<double>(N, M);
      }
      ///Returns the total N-body property, assuming nfrags and m<N
      template <typename T>
      MBEProp<T> NBodyProp(const int N, const int nfrags,
            const MBEProp<T>& DeltaEs)const;
      ///Returns the coefficient for the m-th term of an N-body expansion of nfrags.  m=1 for Ens[0], m=N for Ens[N-1]
      double Coef(const int nfrags, const int N, const int m) const{
         return Phase(N-m)*Binomial(nfrags-m-1, N-m);
      }
   public:
      ///Sets MBE truncation order to N, default is 1
      MBE(int N=1) :ExpanImplBase(N) {
      }
      ///Computes and returns the energy
      template <typename T>
      MBEProp<T> PropertyImpl(const LibMolecule::FragmentedSystem& Systems,
            const MBEProp<T>& MonoProperties);
      bool IsGMBE() const {return false;}
};




template <typename T>
MBEProp<T> MBE::PropertyImpl(const LibMolecule::FragmentedSystem& Systems,
      const MBEProp<T>& MonoProperties) {
   typedef typename MBEProp<T>::const_iterator Itr_t;
   //This is the value of the property at all levels up to including N
   MBEProp<T> Value(N);
   //These will be the corrections to the property
   MBEProp<T> Corrs(N);
   LibMolecule::SerialNumber Dummy;
   Dummy.insert(0);
   int TrueNumMono=0;
   LibMolecule::FragmentedSystem::iterator MonoI=Systems.begin(0),
                                           MonoEnd=Systems.end(0);
   for(;MonoI!=MonoEnd;++MonoI)
      TrueNumMono+=Systems.Coef(0,(*MonoI)->GetSN());
   //Total property of each set of n-mers
   MBEProp<T> En(N);
   for (int i=0; i<N; i++) {
      Itr_t PropI=MonoProperties.begin(i),PropEnd=MonoProperties.end(i);
      MonoI=Systems.begin(i);
      for (; PropI!=PropEnd; ++PropI)
         En.Change(i, Dummy, PropI->second*Systems.Coef(i,(*MonoI)->GetSN()));
      Value.Change(i, Dummy, NBodyProp(i+1, TrueNumMono, En)(0, Dummy));
      if (i==0) continue;
      Corrs.Change(i, Dummy, Value(i,Dummy));
      Corrs.Change(i, Dummy, Value(i-1,Dummy)*-1.0);
   }
   LastResult_=Value.PrintOut();
   return Value;
}

template <typename T>
MBEProp<T> MBE::NBodyProp(const int N, const int nfrags,
      const MBEProp<T>& DeltaEs)const {
   LibMolecule::SerialNumber Dummy;
   Dummy.insert(0);
   MBEProp<T> Result(1,"Total Energy");
   if (N!=nfrags){
      for (int i=0; i<N; i++){
         Result.Change(0, Dummy, DeltaEs(i, Dummy)*MBE::Coef(nfrags, N, i+1));
      }
   }
   else Result.Change(0,Dummy,DeltaEs(N-1,Dummy));
   return Result;
}





}}//End namespaces

#endif /* MBE_H_ */
