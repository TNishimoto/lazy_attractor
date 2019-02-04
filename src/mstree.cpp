
#include <sdsl/suffix_arrays.hpp>
#include <sdsl/suffix_trees.hpp>
#include <sdsl/bit_vectors.hpp>
#include "mstree.hpp"
namespace stool{
    void MinimalSubstringsTreeConstruction::computeMinimalSubstringsTreeWithoutIndexLeave(string &text, vector<LCPInterval> &outputIntervals, vector<uint64_t> &outputParents)
    {
        CST cst;
        printf("  Done. Constructing CST... This may take 5 minutes or so...\n");
        auto start = std::chrono::system_clock::now();
        construct_im(cst, text, 1);
        auto end = std::chrono::system_clock::now();
        double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << "created cst[" << elapsed << "ms]" << std::endl;

        std::stack<MSTStackInfo> stack;

        auto rootID = cst.id(cst.root());
        uint64_t parentIntervalID = UINT64_MAX;

        stack.push(MSTStackInfo(rootID, 0, UINT64_MAX));

        uint64_t nodeNum = cst.nodes();
        uint64_t removeCount = 0;
        while (stack.size() > 0)
        {
            MSTStackInfo top = stack.top();
            stack.pop();

            auto v = cst.inv_id(top.parent);
            if (top.child < cst.degree(v))
            {
                auto child = cst.select_child(v, top.child + 1);
                stack.push(MSTStackInfo(top.parent, top.child + 1, top.parentIntervalID));

                bool b = CSTHelper::isMinimalSubstring(cst, text, top.parent, top.child);
                uint64_t parentIntervalID = top.parentIntervalID;
                if (b)
                {
                    //auto child = cst.select_child(v, top.child + 1);
                    LCPInterval interval = LCPInterval(cst.lb(child) - 1, cst.rb(child) - 1, cst.depth(v) + 1);
                    outputIntervals.push_back(interval);
                    outputParents.push_back(top.parentIntervalID);
                    parentIntervalID = outputIntervals.size() - 1;
                }
                stack.push(MSTStackInfo(cst.id(child), 0, parentIntervalID));
            }
            else
            {
                removeCount++;

                if (removeCount % 1000 == 0)
                    std::cout << "\r"
                              << "computing LCP intervals... : [" << removeCount << "/" << nodeNum << "]" << std::flush;
            }
        }
    }
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