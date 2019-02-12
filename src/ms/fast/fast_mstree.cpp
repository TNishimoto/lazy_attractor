
#include "fast_mstree.hpp"
namespace stool{
    
    bool MinimalSubstringsTreeConstruction::isMinimalSubstring(OfflineSuffixTree &st, uint64_t id)
    {
        if (st.intervals[id].i == 0)
        {
            return false;
        }
        if (!st.isRoot(id))
        {
            char c = st.getFirstChar(id);
            uint64_t sl = st.suffixLinks[st.parents[id]];
            uint64_t child = st.getChild(sl, c);

            return st.getLeaveNum(id) != st.getLeaveNum(child);
        }
        else
        {
            return false;
        }
    }
    void MinimalSubstringsTreeConstruction::construct(string &text, vector<LCPInterval> &outputIntervals, vector<uint64_t> &outputParents)
    {
        OfflineSuffixTree st;
        st.initialize(text);
        std::cout << "\r" << "Computing minimal substrings..." << std::flush;
        vector<uint64_t> parentMSIntervals;
        parentMSIntervals.resize(st.size(), UINT64_MAX);

        std::stack<MSTStackInfo> stack;

        uint64_t nodeNum = st.size();
        uint64_t removeCount = 0;
        uint64_t parentIntervalID = UINT64_MAX;
        stack.push(MSTStackInfo(st.getRoot(), 0, UINT64_MAX));

        while (stack.size() > 0)
        {
            MSTStackInfo top = stack.top();
            stack.pop();

            auto v = top.parent;
            if (top.child < st.getDegree(v))
            {
                auto child = st.selectChild(v, top.child);
                stack.push(MSTStackInfo(top.parent, top.child + 1, top.parentIntervalID));

                bool b = isMinimalSubstring(st, child);
                uint64_t parentIntervalID = top.parentIntervalID;
                if (b)
                {
                    LCPInterval &interval = st.intervals[child];
                    uint64_t edgeLen = st.getEdgeLength(child);
                    outputIntervals.push_back(LCPInterval(interval.i - 1, interval.j - 1, interval.lcp - edgeLen + 1));
                    outputParents.push_back(top.parentIntervalID);
                    parentIntervalID = outputIntervals.size() - 1;
                }

                stack.push(MSTStackInfo(child, 0, parentIntervalID));
            }
            else
            {
                removeCount++;

                if (removeCount % 100000 == 0)
                    std::cout << "\r"
                              << "Computing minimal substrings... : [" << removeCount << "/" << nodeNum << "]" << std::flush;
            }
        }
        std::cout << "[END]" << std::endl;

        text.resize(text.size() - 1);
    }
}