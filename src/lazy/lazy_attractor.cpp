#include "lazy_attractor.hpp"
#include "stool/include/print.hpp"
//#include "libdivsufsort/sa.hpp"

namespace stool
{
namespace lazy
{
std::vector<uint64_t> LazyAttractor::constructMinimalOccurrencesOfMinimalSubstrings(const std::vector<LCPInterval<uint64_t>> &intervals,const  std::vector<uint64_t> &parents,const  std::vector<uint64_t> &sa)
{
    uint64_t textSize = sa.size();
    std::vector<LowestNodeInterval> lowestNodeInfoVec = LowestNodeQuery::constructLowestNodeIntervalVec(intervals, parents);

    std::vector<uint64_t> minimalOccurrenceVec;
    minimalOccurrenceVec.resize(intervals.size(), UINT64_MAX);

    for (uint64_t i = 0; i < lowestNodeInfoVec.size(); i++)
    {
        LowestNodeInterval &info = lowestNodeInfoVec[i];
        uint64_t endPosition = i + 1 < lowestNodeInfoVec.size() ? lowestNodeInfoVec[i+1].rangeStartPosition - 1 : textSize - 1;
        for (uint64_t x = info.rangeStartPosition; x <= endPosition; x++)
        {
            if (minimalOccurrenceVec[info.id] > sa[x])
            {
                minimalOccurrenceVec[info.id] = sa[x];
            }
        }
    }
    for (int64_t i = intervals.size() - 1; i >= 0; i--)
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
    return minimalOccurrenceVec;


    
}
std::stack<MinimalSubstringInfo> LazyAttractor::constructSortedMinimalOccurrenceStack(const std::vector<LCPInterval<uint64_t>> &intervals,const  std::vector<uint64_t> &parents,const  std::vector<uint64_t> &sa){
    std::vector<uint64_t> minimalOccurrenceVec =  constructMinimalOccurrencesOfMinimalSubstrings(intervals,parents, sa);
    std::vector<MinimalSubstringInfo> sortedMinimumSubstringVec;
    std::stack<MinimalSubstringInfo> outputSortedMinimumSubstrings;

    sortedMinimumSubstringVec.resize(minimalOccurrenceVec.size());

    for (uint64_t i = 0; i < minimalOccurrenceVec.size(); i++)
    {
        sortedMinimumSubstringVec[i] = MinimalSubstringInfo(i, minimalOccurrenceVec[i]);
    }
    minimalOccurrenceVec.resize(0);
    minimalOccurrenceVec.shrink_to_fit();

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
bool LazyAttractor::removeMSIntervalsCapturedByTheLastAttractor(TINDEX lastAttractor,const  std::vector<LCPInterval<uint64_t>> &intervals,const  std::vector<uint64_t> &isa, DynamicIntervalTree &tree, std::stack<MinimalSubstringInfo> &sortedMinimumSubstrings)
{
    int64_t currentPos = (int64_t)lastAttractor;
    //uint64_t removedIntervalCount = 0;
    //uint64_t sortedMinimumSubstringsSize = sortedMinimumSubstrings.size();

    while (sortedMinimumSubstrings.size() > 0)
    {

        /*
        Remove minimal substrings captured by the last attractor from the interval tree.       
        */
        uint64_t id = tree.getLowestLCPIntervalID(isa[currentPos]);
        uint64_t longestIntervalLength = intervals[id].lcp;
        while (bool isCapturedInterval = (longestIntervalLength > 0 && currentPos + longestIntervalLength - 1 >= lastAttractor))
        {
            tree.removeLowestLCPInterval(isa[currentPos]);
            id = tree.getLowestLCPIntervalID(isa[currentPos]);
            longestIntervalLength = intervals[id].lcp;
        }
        /*
       Remove minimal substrings starting at positioin the current pos from the stack.       
       */
        while (sortedMinimumSubstrings.size() > 0 && !tree.hasInterval(sortedMinimumSubstrings.top().id))
        {
            sortedMinimumSubstrings.pop();
        }

        if (sortedMinimumSubstrings.size() > 0 && sortedMinimumSubstrings.top().minOcc == (uint64_t)currentPos)
        {
            break;
        }
        else
        {
            --currentPos;
        }
    }
    return false;
}
template <typename CHAR>
std::vector<uint64_t> LazyAttractor::computeLazyAttractors(std::vector<CHAR> &text, std::vector<uint64_t> &sa, std::vector<LCPInterval<uint64_t>> &_intervals, std::vector<uint64_t> &_parents)
{
    //std::vector<uint64_t> sa = constructSA(text);
    if(sa.size() > 1000000)std::cout << "Constructing ISA" << std::flush;
    std::vector<uint64_t> isa = constructISA(text, sa);
    if(sa.size() > 1000000)std::cout << "[END]" << std::endl;

    return computeLazyAttractors(text, sa, isa, _intervals, _parents);
}

template std::vector<uint64_t> LazyAttractor::computeLazyAttractors(std::vector<char> &, std::vector<uint64_t> &, std::vector<LCPInterval<uint64_t>> &, std::vector<uint64_t> &);
template std::vector<uint64_t> LazyAttractor::computeLazyAttractors(std::vector<uint8_t> &, std::vector<uint64_t> &, std::vector<LCPInterval<uint64_t>> &, std::vector<uint64_t> &);
template std::vector<uint64_t> LazyAttractor::computeLazyAttractors(std::vector<int32_t> &, std::vector<uint64_t> &, std::vector<LCPInterval<uint64_t>> &, std::vector<uint64_t> &);
template std::vector<uint64_t> LazyAttractor::computeLazyAttractors(std::vector<uint32_t> &, std::vector<uint64_t> &, std::vector<LCPInterval<uint64_t>> &, std::vector<uint64_t> &);
template std::vector<uint64_t> LazyAttractor::computeLazyAttractors(std::vector<int64_t> &, std::vector<uint64_t> &, std::vector<LCPInterval<uint64_t>> &, std::vector<uint64_t> &);
template std::vector<uint64_t> LazyAttractor::computeLazyAttractors(std::vector<uint64_t> &, std::vector<uint64_t> &, std::vector<LCPInterval<uint64_t>> &, std::vector<uint64_t> &);


template <typename CHAR>
std::vector<uint64_t> LazyAttractor::computeLazyAttractors(std::vector<CHAR> &text, std::vector<uint64_t> &sa, std::vector<uint64_t> &isa, std::vector<LCPInterval<uint64_t>> &_intervals, std::vector<uint64_t> &_parents)
{
    bool isPrint = sa.size() > 100000;
    std::vector<uint64_t> outputAttrs;
    DynamicIntervalTree lufTree(_intervals, _parents, text.size());
    std::stack<MinimalSubstringInfo> sortedMinimumSubstrings = constructSortedMinimalOccurrenceStack(_intervals, _parents, sa);
    stool::Counter printCounter;

    if(isPrint)std::cout << "Computing lazy attractors" << std::flush;
    while (sortedMinimumSubstrings.size() > 0)
    {
        if(isPrint)printCounter.increment();
        auto top = sortedMinimumSubstrings.top();
        assert(outputAttrs.size() == 0 || outputAttrs[outputAttrs.size() - 1] != top.minOcc);
        assert(lufTree.getLowestLCPIntervalID(isa[top.minOcc]) != UINT64_MAX);
        outputAttrs.push_back(top.minOcc);
        removeMSIntervalsCapturedByTheLastAttractor(top.minOcc, _intervals, isa, lufTree, sortedMinimumSubstrings);
    }
    if(isPrint)std::cout << "[END]" << std::endl;

    std::sort(outputAttrs.begin(), outputAttrs.end());
    return outputAttrs;
}

template std::vector<uint64_t> LazyAttractor::computeLazyAttractors(std::vector<char> &, std::vector<uint64_t> &, std::vector<uint64_t> &, std::vector<LCPInterval<uint64_t>> &, std::vector<uint64_t> &);
template std::vector<uint64_t> LazyAttractor::computeLazyAttractors(std::vector<uint8_t> &, std::vector<uint64_t> &, std::vector<uint64_t> &, std::vector<LCPInterval<uint64_t>> &, std::vector<uint64_t> &);
template std::vector<uint64_t> LazyAttractor::computeLazyAttractors(std::vector<int32_t> &, std::vector<uint64_t> &, std::vector<uint64_t> &, std::vector<LCPInterval<uint64_t>> &, std::vector<uint64_t> &);
template std::vector<uint64_t> LazyAttractor::computeLazyAttractors(std::vector<uint32_t> &, std::vector<uint64_t> &, std::vector<uint64_t> &, std::vector<LCPInterval<uint64_t>> &, std::vector<uint64_t> &);
template std::vector<uint64_t> LazyAttractor::computeLazyAttractors(std::vector<int64_t> &, std::vector<uint64_t> &, std::vector<uint64_t> &, std::vector<LCPInterval<uint64_t>> &, std::vector<uint64_t> &);
template std::vector<uint64_t> LazyAttractor::computeLazyAttractors(std::vector<uint64_t> &, std::vector<uint64_t> &, std::vector<uint64_t> &, std::vector<LCPInterval<uint64_t>> &, std::vector<uint64_t> &);

template <typename CHAR>
std::vector<uint64_t> LazyAttractor::naiveComputeLazyAttractors(std::vector<CHAR> &text, std::vector<uint64_t> &sa, std::vector<LCPInterval<uint64_t>> &intervals)
{
    bool isPrint = sa.size() > 1000000;
    //std::vector<uint64_t> sa = stool::construct_suffix_array(text);
    std::vector<uint64_t> minOccVec;
    minOccVec.resize(intervals.size(), UINT64_MAX);

    std::unordered_set<uint64_t> currentIntervals;
    for (uint64_t i = 0; i < intervals.size(); i++)
    {
        if (intervals[i].lcp != 0)
        {
            currentIntervals.insert(i);
            LCPInterval<uint64_t> &interval = intervals[i];

            for (uint64_t x = interval.i; x <= interval.j; x++)
            {
                if (minOccVec[i] > sa[x])
                {
                    minOccVec[i] = sa[x];
                }
            }
        }
    }
    std::vector<uint64_t> outputAttrs;
    uint64_t counter = 0;
    while (currentIntervals.size() > 0)
    {
        if (isPrint && counter++ % 100 == 0)
        {
            std::cout << "\r"
                      << "Computing Lazy Attractors : [" << currentIntervals.size() << "/" << intervals.size() << "]\r" << std::flush;
        }
        uint64_t max_occ = 0;
        for (auto &it : currentIntervals)
        {
            if (minOccVec[it] > max_occ)
            {
                max_occ = minOccVec[it];
            }
        }
        outputAttrs.push_back(max_occ);
        //std::cout << max_occ << ", " << std::flush;
        std::vector<uint64_t> tmp;
        for (auto &it : currentIntervals)
        {
            LCPInterval<uint64_t> &interval = intervals[it];
            for (uint64_t x = interval.i; x <= interval.j; x++)
            {
                if (sa[x] <= max_occ && max_occ <= sa[x] + interval.lcp - 1)
                {
                    tmp.push_back(it);
                    break;
                }
            }
        }
        for (auto &it : tmp)
        {
            currentIntervals.erase(it);
        }
    }
    if(isPrint)std::cout << std::endl;
    std::sort(outputAttrs.begin(), outputAttrs.end());
    return outputAttrs;
}

template std::vector<uint64_t> LazyAttractor::naiveComputeLazyAttractors(std::vector<char> &, std::vector<uint64_t> &, std::vector<LCPInterval<uint64_t>> &);
template std::vector<uint64_t> LazyAttractor::naiveComputeLazyAttractors(std::vector<uint8_t> &, std::vector<uint64_t> &, std::vector<LCPInterval<uint64_t>> &);
template std::vector<uint64_t> LazyAttractor::naiveComputeLazyAttractors(std::vector<uint32_t> &, std::vector<uint64_t> &, std::vector<LCPInterval<uint64_t>> &);
template std::vector<uint64_t> LazyAttractor::naiveComputeLazyAttractors(std::vector<int32_t> &, std::vector<uint64_t> &, std::vector<LCPInterval<uint64_t>> &);
template std::vector<uint64_t> LazyAttractor::naiveComputeLazyAttractors(std::vector<uint64_t> &, std::vector<uint64_t> &, std::vector<LCPInterval<uint64_t>> &);
template std::vector<uint64_t> LazyAttractor::naiveComputeLazyAttractors(std::vector<int64_t> &, std::vector<uint64_t> &, std::vector<LCPInterval<uint64_t>> &);


} // namespace lazy
} // namespace stool