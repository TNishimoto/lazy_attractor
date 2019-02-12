#pragma once
#include "io.h"
#include <stack>
#include "union_find.hpp"
using namespace std;

namespace stool
{

// Union-Find Tree
class UFTree
{
  public:
    using CINDEX = uint64_t;
    private:
    vector<CINDEX> clusterRootIDVec;
    vector<bool> mergeVec;
    vector<uint64_t> *parentVec;

    UnionFind uf;
    public:

    uint64_t size();

    /*
    vector<uint64_t>& getParentVecReference(){
      return *this->parentVec;
    }
    */

    void initialize(vector<uint64_t> &_parentVec);

    CINDEX getClusterID(UnionFind::GINDEX node);
    CINDEX getParent(UnionFind::GINDEX i);
    CINDEX unionParent(UnionFind::GINDEX i);
    bool isRoot(UnionFind::GINDEX i);
    bool checkMerge(UnionFind::GINDEX i);
};
} // namespace stool
