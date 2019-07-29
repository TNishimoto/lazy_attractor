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

    this->removeVec.resize(_intervals.size() - 1, false);

    std::cout << "Constructing UnionFindTree..." << std::flush;
    this->uftree.initialize(_parents);
    std::cout << "[END]" << std::endl;
}
/*
uint64_t DynamicIntervalTree::getTreeNodeID(SINDEX pos)
{
    //uint64_t x = this->isa[pos];
    return this->rangeArray.getLeafID(pos);
}
*/
uint64_t DynamicIntervalTree::getLongestLCPIntervalID(SINDEX pos)
{
    uint64_t id = this->rangeArray.getLeafID(pos);
    while (!this->uftree.isRoot(id))
    {
        if (this->checkRemovedInterval(id))
        {
            //id = this->uftree.getParent(id);
            uint64_t parent = this->uftree.getParent(id);
            if (this->checkRemovedInterval(parent))
            {

#ifdef DEBUG
                uint64_t p = this->uftree.unionParent(id);
                assert(p != UINT64_MAX);
#else
                this->uftree.unionParent(id);
#endif
            }
            else
            {
                return parent;
            }
        }
        else
        {            
            return id;
        }
    }
    assert(!this->checkRemovedInterval(id));
    return id;
    //return UINT64_MAX;
}

bool DynamicIntervalTree::removeLongestLCPInterval(SINDEX pos)
{
    uint64_t id = this->rangeArray.getLeafID(pos);
    if (this->removeVec[id])
    {
        uint64_t result = this->uftree.unionParent(id);
        bool b = result != UINT64_MAX;
        if (b)
        {
            this->removeVec[id] = true;
        }
        return b;
    }
    else
    {
        this->removeVec[id] = true;
        return true;
    }
}
bool DynamicIntervalTree::checkRemovedInterval(uint64_t intervalID)
{
    return this->removeVec[intervalID] || this->uftree.checkMerge(intervalID);
}
/*
uint64_t DynamicIntervalTree::removeMSIntervals(TINDEX pos, vector<uint64_t> &isa, TINDEX lastAttractor)
{
    while (true)
    {
        uint64_t id = getLongestLCPIntervalID(isa[pos]);
        if (id == UINT64_MAX)
        {
            return id;
        }
        else
        {
            uint64_t longestIntervalLength = intervals[id].lcp;
            if (pos + longestIntervalLength - 1 >= lastAttractor)
            {
                this->removeLongestLCPInterval(isa[pos]);
            }
            else
            {
                return id;
            }
        }
    }
    return UINT64_MAX;
}
*/

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
        if(intervals[sortedMinimumSubstringVec[i].id].lcp != 0){
            outputSortedMinimumSubstrings.push(sortedMinimumSubstringVec[i]);
        }
    }
    return outputSortedMinimumSubstrings;
}

} // namespace lazy
} // namespace stool