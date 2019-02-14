#include <cassert>
#include "uftree.hpp"

using namespace std;

namespace stool{
    uint64_t UFTree::size()
    {
        return this->parentVec->size();
    }

    void UFTree::initialize(vector<uint64_t> &_parentVec)
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

    UFTree::CINDEX UFTree::getClusterID(UnionFind::GINDEX node)
    {
        UnionFind::GINDEX id = uf.find(node);
        CINDEX clusterID = clusterRootIDVec[id];
        assert(uf.find(node) == uf.find(clusterID));

        return clusterID;
    }
    UFTree::CINDEX UFTree::getParent(UnionFind::GINDEX i)
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
    UFTree::CINDEX UFTree::unionParent(UnionFind::GINDEX i)
    {
        CINDEX clusterID = this->getClusterID(i);
        CINDEX parentID = this->getParent(i);
        if (parentID == UINT64_MAX)
            return UINT64_MAX;
        //assert(uf.find(parentID) != uf.find(clusterID));
        assert(uf.find(parentID) != uf.find(clusterID));

        UnionFind::GINDEX newID = this->uf.unionOperation(this->uf.find(clusterID), this->uf.find(parentID ));
        assert(uf.find(parentID) == uf.find(clusterID));
        this->mergeVec[parentID] = true;
        this->clusterRootIDVec[newID] = parentID;
        assert(newID == uf.find(parentID));
        assert(newID == uf.find(i));


        return parentID;
    }
    bool UFTree::isRoot(UnionFind::GINDEX i)
    {
        uint64_t parentID = this->getParent(i);
        return parentID == UINT64_MAX;
    }

    bool UFTree::checkMerge(UnionFind::GINDEX i)
    {
        uint64_t clusterID = this->getClusterID(i);
        return this->mergeVec[clusterID];
    }
}
