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

class DynamicIntervalTree
{
private:
  UnionFindTree uftree;
  std::vector<bool> removeVec;
  std::vector<LCPInterval<uint64_t>> &intervals;
  std::vector<uint64_t> &parents;
  LeafRankDataStructure rangeArray;
public:

  DynamicIntervalTree(std::vector<LCPInterval<uint64_t>> &_intervals, std::vector<uint64_t> &_parents, uint64_t textSize);

  uint64_t getLowestLCPIntervalID(SINDEX sa_index);
  bool removeLowestLCPInterval(SINDEX sa_index);
  bool checkRemovedInterval(uint64_t intervalID);
  std::stack<MinimalSubstringInfo> constructSortedMinimumSubstrings(std::vector<uint64_t> &sa);

  
  std::vector<uint64_t> constructLeafIDVec(uint64_t textSize){
    
    std::vector<uint64_t> r;
    r.resize(textSize, UINT64_MAX);
    for (uint64_t i = 0; i < textSize; i++)
    {
      r[i] = this->getLowestLCPIntervalID(i);
     // if(r[i] == UINT64_MAX){
      //  r[i] = 0;
      //}
    }
    return r;
  }
};

} // namespace lazy
} // namespace stool