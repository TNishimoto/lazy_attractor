#pragma once
//#include "io.h"
#include <stack>
//#include "uftree.hpp"
#include "stool/src/sa_bwt_lcp.hpp"
#include <sdsl/bit_vectors.hpp>
#include <sdsl/int_vector.hpp>
using namespace sdsl;

namespace stool
{
namespace lazy
{
using SINDEX = uint64_t;
using TINDEX = uint64_t;
/*
  We call an interval on SA lowest node interval if the lowest nodes on positions in the interval are same.
  This instance stores a lowst node interval and the id of the lowest node on the interval.
  However this instance only stores the id and the starting position of the interval.
 */
struct LowestNodeInterval{
  uint64_t id;
  uint64_t rangeStartPosition;
  LowestNodeInterval(){

  }
  LowestNodeInterval(uint64_t _id,  uint64_t _rangeStartPosition) : id(_id), rangeStartPosition(_rangeStartPosition){
    
  }

};


// This data structure supports the function which
// receives a position i on suffix array and returns the leaf ID containing i in minimal substring tree.
class LowestNodeQuery
{
  std::vector<uint64_t> idVec;
  bit_vector startingPositions;
  rank_support_v<1> bv_rank;
  uint64_t textSize;

  static void checkRangeArray(std::vector<LCPInterval<uint64_t>> &intervals, std::vector<uint64_t> &parents);

public:
  LowestNodeQuery()
  {
  }
  uint64_t getTextSize(){
    return this->textSize;
  }

  void construct(const std::vector<LCPInterval<uint64_t>> &intervals,const std::vector<uint64_t> &parents, uint64_t _textSize);
  // Return the lowest node ID containing pos in minimal substring tree.
  uint64_t getLowestNodeID(SINDEX pos)
  {
    return this->idVec[bv_rank(pos + 1) - 1];
  }
  // Construct the vector X of length n such that X[i] stores the longest LCP interval containing the SA position i in the stored interval tree, 
  // where n is the given textSize. 
  static std::vector<uint64_t> constructLowestNodeIDVec(const std::unordered_set<uint64_t> &currentIntervals,const std::vector<LCPInterval<uint64_t>> &intervals, uint64_t textSize);
  // Compute lowest node intervals in the left-to-right order for the interval tree.
  // See also LowestNodeInterval class.
  static std::vector<LowestNodeInterval> constructLowestNodeIntervalVec(const std::vector<LCPInterval<uint64_t>> &intervals,const std::vector<uint64_t> &parents);
};
} // namespace lazy
} // namespace stool