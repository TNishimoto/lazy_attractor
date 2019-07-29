#include "lazy_attractor.hpp"

namespace stool
{
namespace lazy
{
bool LazyAttractor::removeMSIntervalsCapturedByTheLastAttractor(TINDEX lastAttractor, std::vector<LCPInterval<uint64_t>> &intervals, vector<uint64_t> &isa, DynamicIntervalTree &tree, stack<MinimalSubstringInfo> &sortedMinimumSubstrings)
{
    int64_t currentPos = (int64_t)lastAttractor;
    while (sortedMinimumSubstrings.size() > 0)
    {

        uint64_t id = tree.getLongestLCPIntervalID(isa[currentPos]);
        bool noMSIntervalCapturedByTheLastAttractorOnTheCurrentPos = false;
        assert(id != UINT64_MAX);
        uint64_t longestIntervalLength = intervals[id].lcp;
        if (longestIntervalLength > 0 && currentPos + longestIntervalLength - 1 >= lastAttractor)
        {
#ifdef DEBUG
            bool xb = tree.removeLongestLCPInterval(isa[currentPos]);
            assert(xb);
#else
            tree.removeLongestLCPInterval(isa[currentPos]);
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
void LazyAttractor::computeAttractors(vector<uint8_t> &text, std::vector<LCPInterval<uint64_t>> &_intervals, std::vector<uint64_t> &_parents, std::vector<uint64_t> &outputAttrs)
{
    vector<uint64_t> sa = constructSA(text);
    vector<uint64_t> isa = constructISA(text, sa);

    DynamicIntervalTree lufTree(_intervals, _parents, text.size());
    //lufTree.initialize;
    stack<MinimalSubstringInfo> sortedMinimumSubstrings = lufTree.constructSortedMinimumSubstrings(sa);

    while (sortedMinimumSubstrings.size() > 0)
    {
        auto top = sortedMinimumSubstrings.top();
        if (outputAttrs.size() > 0)
        {
            assert(outputAttrs[outputAttrs.size() - 1] != top.minOcc);
        }

        outputAttrs.push_back(top.minOcc);
        //std::cout << top.minOcc << std::endl;
        assert(lufTree.getLongestLCPIntervalID(isa[top.minOcc]) != UINT64_MAX);
        removeMSIntervalsCapturedByTheLastAttractor(top.minOcc, _intervals, isa, lufTree, sortedMinimumSubstrings);
    }

    std::cout << std::endl;

    sort(outputAttrs.begin(), outputAttrs.end());
}
} // namespace lazy
} // namespace stool