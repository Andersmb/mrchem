#pragma once

#include "qmoperators/one_electron/QMPotential.h"
#include "mrdft/XCFunctional.h"
#include "parallel.h"

/** @class XCPotential
 *
 * @brief Exchange-Correlation potential defined by a particular (spin) density
 *
 * The XC potential is computed by mapping of the density through a XC functional,
 * provided by the XCFun library. There are two ways of defining the density:
 *
 *  1) Use getDensity() prior to setup() and build the density as you like.
 *  2) Provide a default set of orbitals in the constructor that is used to
 *     compute the density on-the-fly in setup().
 *
 * If a set of orbitals has NOT been given in the constructor, the density
 * MUST be explicitly computed prior to setup(). The density will be computed
 * on-the-fly in setup() ONLY if it is not already available. After setup() the
 * operator will be fixed until clear(), which deletes both the density and the
 * potential.
 *
 * LDA and GGA functionals are supported as well as two different ways to compute
 * the XC potentials: either with explicit derivatives or gamma-type derivatives.
 */

namespace mrchem {

class XCPotential : public QMPotential {
public:
 XCPotential(mrdft::XCFunctional *F, OrbitalVector *Phi)
        : QMPotential(1, mpi::share_xc_pot)
        , functional(F)
        , orbitals(Phi)
        , energy(0.0) {}

    friend class XCOperator;

protected:
    OrbitalVector *orbitals;                   ///< External set of orbitals used to build the density
    double energy;                             ///< XC energy
    mrdft::XCFunctional *functional;           ///< External XC functional to be used

    double getEnergy() const { return this->energy; }
    int getOrder() const { return this->functional->getOrder(); }

    mrcpp::FunctionTree<3> &getDensity(int spin);
    virtual void setupPotential(double prec) {}    
    virtual Orbital apply(Orbital phi) = 0;

    void setupDensity(double prec = -1.0);
    void setupPotential();

};

} //namespace mrchem
