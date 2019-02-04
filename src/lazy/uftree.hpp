#pragma once
#include "io.h"
#include <stack>
#include "union_find.hpp"
using namespace std;

namespace stool
{

class UFTree
{
  public:
    using CINDEX = uint64_t;

    vector<uint64_t> parentVec;
    vector<CINDEX> clusterRootIDVec;
    vector<bool> mergeVec;

    UnionFind uf;
    uint64_t size()
    {
        return this->parentVec.size();
    }

    void initialize(vector<uint64_t> &_parentVec)
    {
        std::move(_parentVec.begin(), _parentVec.end(), back_inserter(this->parentVec));
        this->uf.initialize(this->size());
        this->clusterRootIDVec.resize(this->size(), UINT64_MAX);
        for (uint64_t i = 0; i < this->clusterRootIDVec.size(); i++)
        {
            this->clusterRootIDVec[i] = i;
        }

        this->mergeVec.resize(this->size(), false);
    }

    CINDEX getClusterID(UnionFind::GINDEX node)
    {
        UnionFind::GINDEX id = uf.find(node);
        CINDEX clusterID = clusterRootIDVec[id];
        assert(uf.find(node) == uf.find(clusterID));

        return clusterID;
    }
    CINDEX getParent(UnionFind::GINDEX i)
    {
        CINDEX clusterID = this->getClusterID(i);
        UnionFind::GINDEX parentID = parentVec[clusterID];
        if (parentID == UINT64_MAX)
            return UINT64_MAX;
        CINDEX clusterParentID = this->getClusterID(parentID);
        assert(uf.find(parentID) != uf.find(clusterID));
        assert(uf.find(parentID) == uf.find(clusterParentID));

        return clusterParentID;
    }
    CINDEX unionParent(UnionFind::GINDEX i)
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
    bool isRoot(UnionFind::GINDEX i)
    {
        uint64_t parentID = this->getParent(i);
        return parentID == UINT64_MAX;
    }

    bool checkMerge(UnionFind::GINDEX i)
    {
        uint64_t clusterID = this->getClusterID(i);
        return this->mergeVec[clusterID];
    }
};
} // namespace stool
