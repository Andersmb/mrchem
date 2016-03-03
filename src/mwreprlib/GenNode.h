
/*
 *
 *  \date Oct 18, 2009
 *  \author Jonas Juselius <jonas.juselius@uit.no> \n
 *          CTCC, University of Tromsø
 *
 * \breif
 */

#ifndef GENNODE_H_
#define GENNODE_H_

#include "FunctionNode.h"

template<int D> class ProjectedNode;

template<int D>
class GenNode: public FunctionNode<D> {
public:
    GenNode(ProjectedNode<D> &p, int cIdx);
    GenNode(GenNode<D> &p, int cIdx);
    GenNode(const GenNode<D> &n);
    GenNode& operator=(const GenNode<D> &n) { NOT_IMPLEMENTED_ABORT; }
    virtual ~GenNode();

    double getComponentNorm(int i);

    void clearGenerated();
    void purgeGenerated();

    void setCoefs(const Eigen::VectorXd &c);
    Eigen::VectorXd& getCoefs();
    const Eigen::VectorXd& getCoefs() const;

    void mwTransform(int kind);
    void cvTransform(int kind);

    const ProjectedNode<D> *getGenRootNode() const { return this->genRootNode; }
    ProjectedNode<D> *getGenRootNode() { return this->genRootNode; }

protected:
    void calcComponentNorms() { }
    double calcSquareNorm();
    double calcScalingNorm();
    double calcWaveletNorm() { return 0.0; }

private:
    ProjectedNode<D> *genRootNode;

    void createChild(int i);
    void genChild(int i);

    void lockSiblings();
    void unlockSiblings();

    void regenerateCoefs();
    void releaseCoefs();

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & boost::serialization::base_object<FunctionNode<D> >(*this);
    }
};


#endif /* GENNODE_H_ */
