/*
 * deriv.cc
 *
 *  Created on: Feb 24, 2009
 *      Author: jturney
 */

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "deriv.h"
#include "coupling.h"

using namespace std;
using namespace psi;
using namespace boost;

namespace psi { namespace deriv {

Deriv::Deriv(reftype ref, shared_ptr<MatrixFactory>& factory, shared_ptr<BasisSet>& basis)
    : basis_(basis), natom_(basis->molecule()->natom()), factory_(factory), ref_(ref)
{
    // Initialize an integral object.
    shared_ptr<IntegralFactory> integral(new IntegralFactory(basis_, basis_, basis_, basis_));

    // Create new one-electron integral evaluators telling them we want 1st derivatives (1)
    shared_ptr<OneBodyInt> oei_dK(integral->kinetic(1));
    shared_ptr<OneBodyInt> oei_dV(integral->potential(1));
    shared_ptr<OneBodyInt> oei_dS(integral->overlap(1));

    // Allocate memory to store integrals.
    vector<SharedSimpleMatrix> dK;
    vector<SharedSimpleMatrix> dV;

    // Results go here.
    QdH_ = SharedSimpleMatrix(factory_->create_simple_matrix("One-electron contribution to gradient", natom_, 3));
    WdS_ = SharedSimpleMatrix(factory_->create_simple_matrix("Overlap contribution to gradient", natom_, 3));
    tb_  = SharedSimpleMatrix(factory_->create_simple_matrix("Two-electron contribution to gradient", natom_, 3));

    // Allocate memory for dK, dV, and dH.
    for (int i=0; i<3*natom_; ++i) {
        std::string name = "dK - AO-basis " + to_string(i);
        // These are just temporary, so who cares if they're named.
        dK.push_back(SharedSimpleMatrix(factory_->create_simple_matrix(name)));

        name = "dV - AO-basis " + to_string(i);
        dV.push_back(SharedSimpleMatrix(factory_->create_simple_matrix()));

        // These are going to stay around until the class is destroyed, so let's name them.
        name = "dH - AO-basis " + to_string(i);
        dH_.push_back(SharedSimpleMatrix(factory_->create_simple_matrix(name)));
        name = "dS - AO-basis " + to_string(i);
        dS_.push_back(SharedSimpleMatrix(factory_->create_simple_matrix(name)));
    }

    // Compute K and V derivatives for dH
    oei_dK->compute_deriv1(dK);
    oei_dV->compute_deriv1(dV);
    oei_dS->compute_deriv1(dS_);

    // Add dK and dV to give dH
    for (int i=0; i<3*natom_; ++i) {
        dH_[i]->add(dK[i]);
        dH_[i]->add(dV[i]);
    }

//    basis->uso_to_ao()->print();
//    basis->uso_to_bf()->print();

//    for (int i=0; i<3*natom_; ++i)
//        dS_[i]->print();
//    for (int i=0; i<3*natom_; ++i)
//        dK[i]->print();
//    for (int i=0; i<3*natom_; ++i)
//        dV[i]->print();
}

void Deriv::compute(SharedSimpleMatrix& C, SharedSimpleMatrix& Q, SharedSimpleMatrix& G, SharedSimpleMatrix& W)
{
    // Initialize an integral object.
    shared_ptr<IntegralFactory> integral(new IntegralFactory(basis_, basis_, basis_, basis_));

    // Initialize an integral iterator for computing two-electron integral derivatives.
    ShellCombinationsIterator shells = integral->shells_iterator();

    // Initialize an ERI object requesting derivatives.
    shared_ptr<TwoBodyInt> eri(integral->eri(1));

    // Gain access to the eri buffer
    double *buffer = const_cast<double*>(eri->buffer());

    if (ref_ == ref_rhf) {
        int n=0;
        for (int i=0; i<natom_; ++i) {
            // One-electron contribution
            QdH_->set(i, 0, 2.0 *Q->vector_dot(dH_[n]));
            QdH_->set(i, 1, 2.0 *Q->vector_dot(dH_[n+1]));
            QdH_->set(i, 2, 2.0 *Q->vector_dot(dH_[n+2]));

            // Overlap contribution
            WdS_->set(i, 0, 2.0*W->vector_dot(dS_[n]));
            WdS_->set(i, 1, 2.0*W->vector_dot(dS_[n+1]));
            WdS_->set(i, 2, 2.0*W->vector_dot(dS_[n+2]));

            n += 3;
        }

        // The term in the equation has a minus sign in front of it (go a head and apply it here).
        WdS_->scale(-1.0);

        for (shells.first(); !shells.is_done(); shells.next()){
            int shell_p = shells.p();
            int shell_q = shells.q();
            int shell_r = shells.r();
            int shell_s = shells.s();

            int cart_p = basis_->shell(shell_p)->ncartesian();
            int cart_q = basis_->shell(shell_q)->ncartesian();
            int cart_r = basis_->shell(shell_r)->ncartesian();
            int cart_s = basis_->shell(shell_s)->ncartesian();

            int num_p = basis_->shell(shell_p)->nfunction();
            int num_q = basis_->shell(shell_q)->nfunction();
            int num_r = basis_->shell(shell_r)->nfunction();
            int num_s = basis_->shell(shell_s)->nfunction();

            int center_i = basis_->shell(shell_p)->ncenter();
            int center_j = basis_->shell(shell_q)->ncenter();
            int center_k = basis_->shell(shell_r)->ncenter();
            int center_l = basis_->shell(shell_s)->ncenter();

            eri->compute_shell_deriv1(shell_p, shell_q, shell_r, shell_s);
            size_t size = cart_p * cart_q * cart_r * cart_s;

            IntegralsIterator intsIter = shells.integrals_iterator();
            for (intsIter.first(); intsIter.is_done() == false; intsIter.next()) {
                int mu = intsIter.i();
                int nu = intsIter.j();
                int ro = intsIter.k();
                int si = intsIter.l();
                int index = intsIter.index();

                double prefactor = 1.0;

                if (mu == nu)
                    prefactor *= 0.5;
                if (ro == si)
                    prefactor *= 0.5;
                if (INDEX2(mu, nu) == INDEX2(ro, si))
                    prefactor *= 0.5;

                double four_index_D = (4.0 * Q->get(mu, nu) * Q->get(ro, si)
                                       - Q->get(mu, ro) * Q->get(nu, si)
                                       - Q->get(mu, si) * Q->get(nu, ro)) * prefactor;

                tb_->add(center_i, 0, four_index_D * buffer[0*size+index]);
                tb_->add(center_i, 1, four_index_D * buffer[1*size+index]);
                tb_->add(center_i, 2, four_index_D * buffer[2*size+index]);
                tb_->add(center_j, 0, four_index_D * buffer[3*size+index]);
                tb_->add(center_j, 1, four_index_D * buffer[4*size+index]);
                tb_->add(center_j, 2, four_index_D * buffer[5*size+index]);
                tb_->add(center_k, 0, four_index_D * buffer[6*size+index]);
                tb_->add(center_k, 1, four_index_D * buffer[7*size+index]);
                tb_->add(center_k, 2, four_index_D * buffer[8*size+index]);
                tb_->add(center_l, 0, four_index_D * buffer[9*size+index]);
                tb_->add(center_l, 1, four_index_D * buffer[10*size+index]);
                tb_->add(center_l, 2, four_index_D * buffer[11*size+index]);
            }
        }
        tb_->scale(4.0);
    }
    else if (ref_ == ref_rohf) {

    }
}

}}