#include "lazy_attractor.hpp"

namespace stool
{
namespace lazy
{
bool LazyAttractor::removeMSIntervalsCapturedByTheLastAttractor(TINDEX lastAttractor, std::vector<LCPInterval<uint64_t>> &intervals, std::vector<uint64_t> &isa, DynamicIntervalTree &tree, std::stack<MinimalSubstringInfo> &sortedMinimumSubstrings)
{
    int64_t currentPos = (int64_t)lastAttractor;
    while (sortedMinimumSubstrings.size() > 0)
    {

        uint64_t id = tree.getLowestLCPIntervalID(isa[currentPos]);
        bool noMSIntervalCapturedByTheLastAttractorOnTheCurrentPos = false;
        assert(id != UINT64_MAX);
        uint64_t longestIntervalLength = intervals[id].lcp;
        if (longestIntervalLength > 0 && currentPos + longestIntervalLength - 1 >= lastAttractor)
        {
#ifdef DEBUG
            bool xb = tree.removeLowestLCPInterval(isa[currentPos]);
            assert(xb);
#else
            tree.removeLowestLCPInterval(isa[currentPos]);
#endif
        }
        else
        {
            noMSIntervalCapturedByTheLastAttractorOnTheCurrentPos = true;
        }

        /*
       Remove minimal substrings starting at positioin the current pos from the stack.       
       */
        if (noMSIntervalCapturedByTheLastAttractorOnTheCurrentPos)
        {
            while (sortedMinimumSubstrings.size() > 0)
            {
                auto top = sortedMinimumSubstrings.top();
                if (top.minOcc == (uint64_t)currentPos)
                {

                    if (!tree.checkRemovedInterval(top.id))
                    {
                        return true;
                    }
                    else
                    {
                        sortedMinimumSubstrings.pop();
                    }
                }
                else
                {
                    --currentPos;
                    break;
                }
            }
        }
    }
    return false;
}
std::vector<uint64_t> LazyAttractor::computeLazyAttractors(std::vector<uint8_t> &text, std::vector<LCPInterval<uint64_t>> &_intervals, std::vector<uint64_t> &_parents)
{
    std::vector<uint64_t> sa = constructSA(text);
    std::vector<uint64_t> isa = constructISA(text, sa);
    return computeLazyAttractors(text,sa,isa,_intervals,_parents);

}

std::vector<uint64_t> LazyAttractor::computeLazyAttractors(std::vector<uint8_t> &text, std::vector<uint64_t> &sa, std::vector<uint64_t> &isa, std::vector<LCPInterval<uint64_t>> &_intervals, std::vector<uint64_t> &_parents){
    std::vector<uint64_t> outputAttrs;
    DynamicIntervalTree lufTree(_intervals, _parents, text.size());
    //lufTree.initialize;
   std::stack<MinimalSubstringInfo> sortedMinimumSubstrings = lufTree.constructSortedMinimumSubstrings(sa);

    while (sortedMinimumSubstrings.size() > 0)
    {
        auto top = sortedMinimumSubstrings.top();
        if (outputAttrs.size() > 0)
        {
            assert(outputAttrs[outputAttrs.size() - 1] != top.minOcc);
        }

        outputAttrs.push_back(top.minOcc);
        //std::cout << top.minOcc << std::endl;
        assert(lufTree.getLowestLCPIntervalID(isa[top.minOcc]) != UINT64_MAX);
        removeMSIntervalsCapturedByTheLastAttractor(top.minOcc, _intervals, isa, lufTree, sortedMinimumSubstrings);
    }

    std::cout << std::endl;

    std::sort(outputAttrs.begin(), outputAttrs.end());
    return outputAttrs;
}

} // namespace lazy
} // namespace stool