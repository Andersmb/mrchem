/**
*
*
*  \date May 23, 2014
*  \author Stig Rune Jensen <stig.r.jensen@uit.no> \n
*   CTCC, University of Tromsø
*
*/

#ifndef ELEMENT_H_
#define ELEMENT_H_

#include <string>

/** Basic chemical element data.
 *  
 */
class AtomicElement {
public:
    AtomicElement(int z, const char *s, const char *n, double m, double vdw, double cov) 
	: Z(z), symbol(s), name(n), mass(m), r_vdw(vdw), r_cov(cov) { }
    virtual ~AtomicElement() { }

    const std::string &getSymbol() const { return this->symbol; }
    const std::string &getName() const { return this->name; }

    double getMass() const { return this->mass; }
    double getVdw() const { return this->r_vdw; }
    double getCov() const { return this->r_cov; }
    int getZ() const { return this->Z; }

protected:
    const int Z; /** atomic number */
    const std::string symbol; /** atomic symbol */
    const std::string name; /** element name */
    const double mass; /** atomic mass */
    const double r_vdw; /** van der waals radius */
    const double r_cov; /** covalent radius */
};

#endif /* ELEMENT_H_ */
