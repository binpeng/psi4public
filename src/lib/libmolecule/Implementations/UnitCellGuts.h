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
#ifndef SRC_LIB_LIBFRAG_LIBMOLECULE_IMPLEMENTATIONS_UNITCELLGUTS_H_
#define SRC_LIB_LIBFRAG_LIBMOLECULE_IMPLEMENTATIONS_UNITCELLGUTS_H_
#include "LibFragMolecule.h"
namespace psi{
namespace LibMolecule{
class UnitCellGuts: public Molecule{
   protected:
      ///Performs a deep copy
      void Copy(const UnitCellGuts& other);
      ///The angles of the unit cell, in Radians
      double angles_[3];
      ///The lengths of the sides of the unit cell, in a.u.
      double sides_[3];
      ///The fractional coordinate to cart transformation matrix
      double Frac2Cart_[9];
      ///The cart w fractional transform
      double Cart2Frac_[9];
      void SetAngles(const double alpha,const double beta,
                     const double gamma,const bool IsDegree=true);
      void SetSides(const double a, const double b, const double c,
                    const bool IsBohr=false);
      void SetTrans();

      UnitCellGuts(const Molecule& Mol,const double* Sides,
            const double* Angles, const bool IsFrac=true,
            const bool IsBohr=false,const bool IsDegree=true);
      UnitCellGuts(){}
      UnitCellGuts(const UnitCellGuts& other):Molecule(other){
         this->Copy(other);
      }
      ///RMR removed the virtual to rid warning
      const UnitCellGuts& operator=(const UnitCellGuts& other){
         Molecule::operator=(other);
         if(this!=&other)this->Copy(other);
         return *this;
      }
      virtual ~UnitCellGuts(){}
      virtual const double* Frac2Cart()const{return Frac2Cart_;}
      virtual const double* Cart2Frac()const{return Cart2Frac_;}
      virtual const double* Angles()const{return angles_;}
      virtual const double* Sides()const{return sides_;}


};



}}//End namespaces



#endif /* SRC_LIB_LIBFRAG_LIBMOLECULE_IMPLEMENTATIONS_UNITCELLGUTS_H_ */
