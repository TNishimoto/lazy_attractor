#pragma once
//#include "io.h"
#include <stack>
#include "union_find.hpp"
using namespace std;

namespace stool
{
namespace lazy
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
  /*
   Return the cluster id of the given node id i.
   */
  CINDEX getClusterID(UnionFind::GINDEX node);
  /*
   Return the cluster parent id of the cluster of the given node id i.
   */
  CINDEX getParent(UnionFind::GINDEX i);
  /*
   Union the given cluster of i and the parent of the cluster, and return the new cluster id.  
   */
  CINDEX unionParent(UnionFind::GINDEX i);
  bool isRoot(UnionFind::GINDEX i);
  bool checkMerge(UnionFind::GINDEX i);
};
} // namespace lazy
} // namespace stool
