/** 
 *
 * \date Jun 7, 2009
 * \author Jonas Juselius <jonas.juselius@uit.no> \n
 *         CTCC, University of Tromsø
 *
 *
 */

#ifndef PERIODICTABLE_H_
#define PERIODICTABLE_H_

#include <string>
#include <map>

class Element;

class PeriodicTable {
public:
    PeriodicTable() { }
    virtual ~PeriodicTable() { }
    const Element &getElement(int Z) const;
    const Element &getElement(const char *id) const;
    static const int nElements=112;
protected:
    typedef std::map<std::string, const Element *> map_t;
    static const Element elements[];
    static map_t byName; 
    static map_t bySymbol; 
private:
    static map_t _init_byname();
    static map_t _init_bysymbol();
};

#endif /* PERIODICTABLE_H_ */
