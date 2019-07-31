#pragma once
//#include "stool/src/io.h"
#include <stack>
#include "union_find_tree.hpp"
#include "stool/src/sa_bwt_lcp.hpp"
#include "leaf_rank.hpp"
using namespace sdsl;

namespace stool
{
namespace lazy
{

struct MinimalSubstringInfo
{
  uint64_t id;
  uint64_t minOcc;
  MinimalSubstringInfo(){

  };
  MinimalSubstringInfo(uint64_t _id, uint64_t _minOcc) : id(_id), minOcc(_minOcc)
  {
  }
};

/*
  This class stores LCP intervals as a tree and supports the following two operations.
  (1) removeLowestLCPInterval : Remove the longest LCP interval containing the given sa-index. 
  (2) getLowestLCPIntervalID : Return the longest LCP interval containing the given sa-index. 
 */
class DynamicIntervalTree
{
private:
  UnionFindTree uftree;
  //std::vector<bool> removeVec;
  std::vector<LCPInterval<uint64_t>> &intervals;
  std::vector<uint64_t> &parents;
  LeafRankDataStructure rangeArray;
public:

  DynamicIntervalTree(std::vector<LCPInterval<uint64_t>> &_intervals, std::vector<uint64_t> &_parents, uint64_t textSize);

  /*
  Return the longest LCP interval containing the given sa-index in the interval tree. 
  */
  uint64_t getLowestLCPIntervalID(SINDEX sa_index);
  /*
  Remove the longest LCP interval containing the given sa-index from the interval tree, and return true.
  It returns false when such LCP interval does not exist.
  */
  bool removeLowestLCPInterval(SINDEX sa_index);
  bool hasInterval(uint64_t intervalID);

  /*
  Return minimal substrings on the given suffix array. The minimal substrings are sorted in the order of the their minimal occrrences.
   */
  std::stack<MinimalSubstringInfo> constructSortedMinimumSubstrings(std::vector<uint64_t> &sa);

  
  std::vector<uint64_t> constructLeafIDVec(uint64_t textSize);

  void print(){
    std::vector<uint64_t> r = this->uftree.getClusterRoots();
    std::sort(r.begin(), r.end());
    for(auto &it : r){
      std::cout << this->intervals[it].to_string();
    } 
    std::cout << std::endl;
  }
};

} // namespace lazy
} // namespace stool