/*
 *
 *
 *  \date June 2, 2010
 *  \author Stig Rune Jensen \n
 *          CTCC, University of Tromsø
 *
 */

#ifndef INTERPOLATINGBASIS_H
#define INTERPOLATINGBASIS_H

#include "ScalingBasis.h"

class InterpolatingBasis : public ScalingBasis {
public:
    InterpolatingBasis(int k) : ScalingBasis(k, Interpol) { }
    virtual ~InterpolatingBasis() { }
};

#endif // INTERPOLATINGBASIS_H
