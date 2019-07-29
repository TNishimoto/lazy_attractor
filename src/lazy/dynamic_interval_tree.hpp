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
  uint64_t getLongestLCPIntervalID(SINDEX pos);
  bool removeLongestLCPInterval(SINDEX pos);
  bool checkRemovedInterval(uint64_t intervalID);
  std::stack<MinimalSubstringInfo> constructSortedMinimumSubstrings(std::vector<uint64_t> &sa);
};

} // namespace lazy
} // namespace stool