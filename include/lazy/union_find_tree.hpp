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

/*
 Union-Find Tree is a rooted tree such that every node belongs to a cluster.
 In the initial state, every node belongs to a distinct cluster, i.e., the size of every cluster is 1.
 The tree supports follongs operations:
 (1) getClusterID: Return the cluster id of the given node id.
 (2) getParent: Return the cluster parent id of the cluster of the given node id. 
 (3) unionParent: Union the cluster of the given node id and the parent of the cluster, and return the new cluster id.  
*/
class UnionFindTree
{
public:
  using CINDEX = uint64_t;

private:
  std::vector<CINDEX> clusterRootIDVec;
  std::vector<bool> mergeVec;
  const std::vector<uint64_t> *parentVec;

  UnionFind uf;

public:
  uint64_t size();

  void initialize(const std::vector<uint64_t> &_parentVec);
  /*
   Return the cluster id of the given node id.
   */
  CINDEX getClusterID(UnionFind::GINDEX nodeID);
  /*
   Return the cluster parent id of the cluster of the given node id.
   */
  CINDEX getParentClusterID(UnionFind::GINDEX nodeID);
  /*
   Union the cluster of the given node id and the parent of the cluster, and return the new cluster id.  
   */
  CINDEX unionParent(UnionFind::GINDEX nodeID);
  /*
    Return true iff the cluster of the given node id is the root cluster.
   */
  bool isRootCluster(UnionFind::GINDEX nodeID);
  bool checkMerge(UnionFind::GINDEX nodeID);

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
