#pragma once
//#include "io.h"
#include <stack>
#include "union_find.hpp"
#include <vector>
#include <unordered_set>
//using namespace std;

namespace stool
{
namespace lazy
{
// Union-Find Tree
class UnionFindTree
{
public:
  using CINDEX = uint64_t;

private:
  std::vector<CINDEX> clusterRootIDVec;
  std::vector<bool> mergeVec;
  std::vector<uint64_t> *parentVec;

  UnionFind uf;

public:
  uint64_t size();

  /*
    vector<uint64_t>& getParentVecReference(){
      return *this->parentVec;
    }
    */

  void initialize(std::vector<uint64_t> &_parentVec);
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

  std::vector<uint64_t> getClusterRoots(){
    std::unordered_set<uint64_t> r;
    std::vector<uint64_t> r2;
    for(uint64_t i=0;i<this->clusterRootIDVec.size();i++){
      r.insert(this->getClusterID(i));      
    }
    for(auto &it : r){
      r2.push_back(it);
    }
    return r2;
  }
};
} // namespace lazy
} // namespace stool
