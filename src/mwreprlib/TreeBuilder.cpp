#include "TreeBuilder.h"
#include "TreeProjector.h"
#include "TreeAdaptor.h"
#include "MWTree.h"
#include "MWNode.h"

using namespace std;

template<int D>
TreeBuilder<D>::TreeBuilder(TreeAdaptor<D> &a, int iter) {
    this->projector = 0;
    this->adaptor = &a;
    this->maxIter = iter;
}

template<int D>
TreeBuilder<D>::~TreeBuilder() {
    clearProjector();
}

template<int D>
void TreeBuilder<D>::clearProjector() {
    if (this->projector != 0) {
        delete this->projector;
        this->projector = 0;
    }
}

template<int D>
void TreeBuilder<D>::build(MWTree<D> &tree) {
    if (this->projector == 0) MSG_ERROR("Projector not initialized");
    if (this->adaptor == 0) MSG_ERROR("Adaptor not initialized");
    println(10, " == Building tree");

    NodeIndexSet *splitSet = 0;
    MRNodeVector *splitVec = 0;
    MRNodeVector *workVec = tree.copyEndNodeTable();
    MRNodeVector *endVec = tree.getEndNodeTable();
    endVec->clear();

    int iter = 0;
    while (workVec->size() > 0) {
        printout(10, "  -- #" << setw(3) << iter << ": Calculated   ");
        workVec = clearForeignNodes(workVec);
        this->projector->calcNodeVector(*workVec);
        tree.calcSquareNorm(workVec);
        if (maxIterReached(iter)) break;
        splitVec = this->adaptor->splitNodeVector(*workVec, endVec);
        splitSet = getNodeIndexSet(*splitVec);
        broadcast_index_list<D>(*splitSet);
        workVec->clear();
        tree.splitNodes(*splitSet, workVec);
        delete splitSet;
        delete splitVec;
        iter++;
    }
    delete workVec;
    tree.resetEndNodeTable();
    tree.calcSquareNorm();
}

template<int D>
MRNodeVector* TreeBuilder<D>::clearForeignNodes(MRNodeVector *oldVec) const {
    MRNodeVector *newVec = new MRNodeVector;
    for (int i = 0; i < oldVec->size(); i++) {
        MRNode<D> *node = (*oldVec)[i];
        if (node == 0) {
            continue;
        }
        if (not node->isForeign()) {
            newVec->push_back(node);
        }
    }
    delete oldVec;
    return newVec;
}

template<int D>
NodeIndexSet* TreeBuilder<D>::getNodeIndexSet(const MRNodeVector &nodeVec) const {
    NodeIndexSet *idxSet = new NodeIndexSet;
    for (int i = 0; i < nodeVec.size(); i++) {
        const NodeIndex<D> &idx = nodeVec[i]->getNodeIndex();
        idxSet->insert(&idx);
    }
    return idxSet;
}

template class TreeBuilder<1>;
template class TreeBuilder<2>;
template class TreeBuilder<3>;