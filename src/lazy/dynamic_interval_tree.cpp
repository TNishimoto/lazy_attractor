#include <stack>
#include "stool/src/sa_bwt_lcp.hpp"
#include "dynamic_interval_tree.hpp"

namespace stool
{
namespace lazy
{
DynamicIntervalTree::DynamicIntervalTree(std::vector<LCPInterval<uint64_t>> &_intervals, std::vector<uint64_t> &_parents, uint64_t textSize) : intervals(_intervals), parents(_parents)
{

    std::cout << "construct rangeArray" << std::endl;
    rangeArray.construct(_intervals, _parents, textSize);

    //this->removeVec.resize(_intervals.size(), false);

    std::cout << "Constructing UnionFindTree..." << std::flush;
    this->uftree.initialize(_parents);
    std::cout << "[END]" << std::endl;
}
uint64_t DynamicIntervalTree::getLowestLCPIntervalID(SINDEX sa_index)
{
    uint64_t id = this->rangeArray.getLowestNodeID(sa_index);
    uint64_t clusterRootID = this->uftree.getClusterID(id);
    return clusterRootID;
}

bool DynamicIntervalTree::removeLowestLCPInterval(SINDEX sa_index)
{
    uint64_t leafID = this->getLowestLCPIntervalID(sa_index);
    //assert(!this->removeVec[leafID]);
    uint64_t result = this->uftree.unionParent(leafID);
    bool b = result != UINT64_MAX;

    /*
    if (b)
    {
        this->removeVec[leafID] = true;
    }
    */
    return b;
}
bool DynamicIntervalTree::hasInterval(uint64_t intervalID)
{
    uint64_t clusterID = this->uftree.getClusterID(intervalID);
    return clusterID == intervalID;
    //return this->removeVec[intervalID] || this->uftree.checkMerge(intervalID);
    //return this->removeVec[intervalID];
}

std::stack<MinimalSubstringInfo> DynamicIntervalTree::constructSortedMinimumSubstrings(std::vector<uint64_t> &sa)
{
    std::stack<MinimalSubstringInfo> outputSortedMinimumSubstrings;
    //vec.resize(intervals.size(), UINT64_MAX);
    std::vector<uint64_t> &parents = this->parents;

    std::vector<uint64_t> minimalOccurrenceVec = this->rangeArray.constructMinimalOccurrenceVec(sa, intervals.size());

    for (int64_t i = this->intervals.size() - 1; i >= 0; i--)
    {
        assert(minimalOccurrenceVec[i] != UINT64_MAX);
        if (parents[i] != UINT64_MAX)
        {
            if (minimalOccurrenceVec[i] < minimalOccurrenceVec[parents[i]])
            {
                minimalOccurrenceVec[parents[i]] = minimalOccurrenceVec[i];
            }
        }
    }

    std::vector<MinimalSubstringInfo> sortedMinimumSubstringVec;
    sortedMinimumSubstringVec.resize(minimalOccurrenceVec.size());

    for (uint64_t i = 0; i < minimalOccurrenceVec.size(); i++)
    {
        sortedMinimumSubstringVec[i] = MinimalSubstringInfo(i, minimalOccurrenceVec[i]);
    }

    sort(sortedMinimumSubstringVec.begin(), sortedMinimumSubstringVec.end(), [&](const MinimalSubstringInfo &x, const MinimalSubstringInfo &y) {
        if (x.minOcc == y.minOcc)
        {
            return intervals[x.id].lcp > intervals[y.id].lcp;
        }
        else
        {
            return x.minOcc < y.minOcc;
        }
    });

    for (uint64_t i = 0; i < sortedMinimumSubstringVec.size(); i++)
    {
        if (intervals[sortedMinimumSubstringVec[i].id].lcp != 0)
        {
            outputSortedMinimumSubstrings.push(sortedMinimumSubstringVec[i]);
        }
    }
    return outputSortedMinimumSubstrings;
}
std::vector<uint64_t> DynamicIntervalTree::constructLeafIDVec(uint64_t textSize){
    
    std::vector<uint64_t> r;
    r.resize(textSize, UINT64_MAX);
    for (uint64_t i = 0; i < textSize; i++)
    {
      r[i] = this->getLowestLCPIntervalID(i);
    }
    return r;
  }
} // namespace lazy
} // namespace stool