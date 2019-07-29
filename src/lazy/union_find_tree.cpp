#include <cassert>
#include "union_find_tree.hpp"

using namespace std;

namespace stool
{
namespace lazy
{
uint64_t UnionFindTree::size()
{
    return this->parentVec->size();
}

void UnionFindTree::initialize(vector<uint64_t> &_parentVec)
{
    this->parentVec = &_parentVec;
    //this->parentVec.swap(_parentVec);
    //std::move(_parentVec.begin(), _parentVec.end(), back_inserter(this->parentVec));
    this->uf.initialize(this->size());
    this->clusterRootIDVec.resize(this->size(), UINT64_MAX);
    for (uint64_t i = 0; i < this->clusterRootIDVec.size(); i++)
    {
        this->clusterRootIDVec[i] = i;
    }

    this->mergeVec.resize(this->size(), false);
}

UnionFindTree::CINDEX UnionFindTree::getClusterID(UnionFind::GINDEX node)
{
    UnionFind::GINDEX id = uf.find(node);
    CINDEX clusterID = clusterRootIDVec[id];
    assert(uf.find(node) == uf.find(clusterID));

    return clusterID;
}
UnionFindTree::CINDEX UnionFindTree::getParent(UnionFind::GINDEX i)
{
    CINDEX clusterID = this->getClusterID(i);
    UnionFind::GINDEX parentID = (*this->parentVec)[clusterID];
    if (parentID == UINT64_MAX)
        return UINT64_MAX;
    CINDEX clusterParentID = this->getClusterID(parentID);
    assert(uf.find(parentID) != uf.find(clusterID));
    assert(uf.find(parentID) == uf.find(clusterParentID));

    return clusterParentID;
}
UnionFindTree::CINDEX UnionFindTree::unionParent(UnionFind::GINDEX i)
{
    CINDEX clusterID = this->getClusterID(i);
    CINDEX parentID = this->getParent(i);
    if (parentID == UINT64_MAX)
        return UINT64_MAX;
    //assert(uf.find(parentID) != uf.find(clusterID));
    assert(uf.find(parentID) != uf.find(clusterID));

    UnionFind::GINDEX newID = this->uf.unionOperation(this->uf.find(clusterID), this->uf.find(parentID));
    assert(uf.find(parentID) == uf.find(clusterID));
    this->mergeVec[parentID] = true;
    this->clusterRootIDVec[newID] = parentID;
    assert(newID == uf.find(parentID));
    assert(newID == uf.find(i));

    return parentID;
}
bool UnionFindTree::isRoot(UnionFind::GINDEX i)
{
    uint64_t parentID = this->getParent(i);
    return parentID == UINT64_MAX;
}

bool UnionFindTree::checkMerge(UnionFind::GINDEX i)
{
    uint64_t clusterID = this->getClusterID(i);
    return this->mergeVec[clusterID];
}
} // namespace lazy
} // namespace stool
