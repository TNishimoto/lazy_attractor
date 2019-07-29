#pragma once
//#include "stool/src/io.h"
#include <stack>
#include "uftree.hpp"
#include "stool/src/sa_bwt_lcp.hpp"

#include "range_array.hpp"
using namespace sdsl;

namespace stool
{
namespace lazy
{

struct MinimalSubstringInfo{
  uint64_t id;
  uint64_t minOcc;
  MinimalSubstringInfo(){

  };
  MinimalSubstringInfo(uint64_t _id, uint64_t _minOcc): id(_id), minOcc(_minOcc){

  }
};

class LazyUFTree
{
private:
  UFTree uftree;
  std::vector<bool> removeVec;
  std::vector<LCPInterval<uint64_t>> &intervals;
  std::vector<uint64_t> &parents;
  SAPositionToMSLeaf rangeArray;

  uint64_t getLongestLCPIntervalID(SINDEX pos);
  uint64_t getTreeNodeID(SINDEX pos);
  bool removeLongestLCPInterval(SINDEX pos);

  bool checkRemovedInterval(uint64_t intervalID);
  uint64_t removeMSIntervals(TINDEX pos, std::vector<uint64_t> &isa, TINDEX lastAttractor);
  std::stack<MinimalSubstringInfo> constructSortedMinimumSubstrings(std::vector<uint64_t> &sa);

public:
  LazyUFTree(std::vector<LCPInterval<uint64_t>> &_intervals, std::vector<uint64_t> &_parents, uint64_t textSize);
  void initialize(std::vector<LCPInterval<uint64_t>> &_intervals, std::vector<uint64_t> &_parents, uint64_t textSize);
  static void computeAttractors(vector<uint8_t> &text, std::vector<LCPInterval<uint64_t>> &_intervals, std::vector<uint64_t> &_parents, std::vector<uint64_t> &outputAttrs);
};
} // namespace lazy
} // namespace stool