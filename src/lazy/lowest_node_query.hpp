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
struct LowestNodeInfo{
  uint64_t id;
  uint64_t rangeStartPosition;
  LowestNodeInfo(){

  }
  LowestNodeInfo(uint64_t _id,  uint64_t _rangeStartPosition) : id(_id), rangeStartPosition(_rangeStartPosition){
    
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

  void construct(std::vector<LCPInterval<uint64_t>> &intervals, std::vector<uint64_t> &parents, uint64_t _textSize);
  // Return the leaf ID containing pos in minimal substring tree.
  uint64_t getLowestNodeID(SINDEX pos)
  {
    return this->idVec[bv_rank(pos + 1) - 1];
  }
  static std::vector<uint64_t> constructLeafIDVec(std::unordered_set<uint64_t> &currentIntervals,std::vector<LCPInterval<uint64_t>> &intervals, uint64_t textSize);
  static std::vector<LowestNodeInfo> constructLowestNodeInfoVec(std::vector<LCPInterval<uint64_t>> &intervals, std::vector<uint64_t> &parents);
};
} // namespace lazy
} // namespace stool