#include <iostream>
#include <string>
#include <memory>
#include <map>
#include <stack>
#include <unordered_map>
#include <unordered_set>

#include "fmindex.hpp"

#include <sdsl/suffix_arrays.hpp>
#include <sdsl/suffix_trees.hpp>
#include <sdsl/bit_vectors.hpp>
#include "lcp_interval.hpp"
#include "minimal_substrings.hpp"

using CST = cst_sct3<csa_bitcompressed<>>;
//using CST = csa_wt<wt_int<rrr_vector<>>>;
namespace stool
{
void MinimalSubstringsConstruction::computeMinimalSubstrings(string &text, vector<LCPInterval> &intervals)
{

    CST cst;
    printf("  Done. Constructing CST... This may take 5 minutes or so...\n");
    auto start = std::chrono::system_clock::now();
    construct_im(cst, text, 1);
    auto end = std::chrono::system_clock::now();
    double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "created cst[" << elapsed << "ms]" << std::endl;

    uint64_t size = cst.nodes();
    //for (int64_t x = size-1; x >= 0; x--)
    for (uint64_t x = 0; x < size; x++)
    {
        if (x % 1000 == 0)
            std::cout << "\r"
                      << "computing LCP intervals... : [" << x << "/" << size << "]" << std::flush;
        auto p = cst.inv_id(x);
        auto p2 = cst.lb(p);
        auto p3 = cst.rb(p);
        uint64_t degree = cst.degree(p);

        for (uint64_t i = 0; i < degree; i++)
        {
            bool b = CSTHelper::isMinimalSubstring(cst, text, cst.id(p), i);
            if (b)
            {
                auto child = cst.select_child(p, i + 1);
                LCPInterval interval = LCPInterval(cst.lb(child) - 1, cst.rb(child) - 1, cst.depth(p) + 1);
                intervals.push_back(interval);
            }
        }
    }
    std::cout << "done." << std::endl;

    sort(intervals.begin(), intervals.end());
}
void MinimalSubstringsConstruction::constructMSIntervals(string &text, vector<LCPInterval> &outputIntervals)
{
    for (uint64_t i = 0; i < text.size(); i++)
    {
        if (text[i] == 0)
        {
            std::cout << "the input text contains zero." << std::endl;
            throw - 1;
        }
    }
    text.push_back(0);

    vector<uint64_t> sa, isa, lcp, parents;
    stool::constructSA(text, sa, isa);
    if (sa[0] != text.size() - 1)
    {
        std::cout << "the input text contains negative values." << std::endl;
        throw - 1;
    }
    constructLCP(text, lcp, sa, isa);
    vector<LCPInterval> intervals;

    createLCPIntervals(sa, lcp, intervals, parents);

    std::unordered_map<uint64_t, std::unordered_set<uint8_t>> charMap;
    FMIndex fmi(text, sa);
    std::unordered_map<uint64_t, std::unordered_map<uint64_t, uint64_t>> omap;

    std::cout << "Computing minimal substrings..." << std::flush;
    for (uint64_t i = 0; i < intervals.size(); i++)
    {

        if (i % 100000 == 0)
            std::cout << "\r"
                      << "Computing minimal substrings... :[1/4][" << i << "/" << intervals.size() << "]" << std::flush;

        vector<uint8_t> chars;
        auto &interval = intervals[i];

        if (parents[i] != UINT64_MAX)
        {

            if (charMap.find(parents[i]) == charMap.end())
            {
                charMap.insert(std::pair<uint64_t, std::unordered_set<uint8_t>>(parents[i], std::unordered_set<uint8_t>()));
            }
        }
        auto &fi = charMap[parents[i]];

        if (interval.j - interval.i == 0)
        {
            uint8_t bwtChar = fmi.bwt[interval.i];
            if (bwtChar != 0)
                fi.insert(fmi.bwt[interval.i]);
        }
        else
        {

            for (uint8_t it : charMap[i])
            {
                //std::cout << i << "/"<< it << std::endl;
                uint64_t b = interval.i;
                uint64_t e = interval.j;
                fmi.backwardSearch(b, e, it);
                if (interval.j - interval.i != e - b)
                {
                    if (b != 0)
                    {

                        uint64_t newLCP = parents[i] == UINT64_MAX ? 1 : intervals[parents[i]].lcp + 2;
                        if (omap.find(b) == omap.end())
                        {
                            omap.insert(std::pair<uint64_t, std::unordered_map<uint64_t, uint64_t>>(b, std::unordered_map<uint64_t, uint64_t>()));
                        }
                        auto &omap2 = omap[b];
                        if (omap2.find(e) == omap2.end())
                        {
                            omap2.insert(std::pair<uint64_t, uint64_t>(e, newLCP));
                        }
                        else
                        {
                            if (newLCP < omap2[e])
                            {
                                omap2[e] = newLCP;
                            }
                        }
                    }
                }
                fi.insert(it);
            }
        }
    }

    vector<uint64_t> bVec;
    uint64_t u = 0;
    for (auto &it : omap)
    {
        if (u % 100000 == 0)
            std::cout << "\r"
                      << "Computing minimal substrings... :[2/4][" << u << "/" << omap.size() << "]" << std::flush;
        u++;
        bVec.push_back(it.first);
    }
    sort(bVec.begin(), bVec.end());

    for (uint64_t i = 0; i < bVec.size(); i++)
    {

        if (i % 100000 == 0)
            std::cout << "\r"
                      << "Computing minimal substrings... :[3/4][" << i << "/" << bVec.size() << "]" << std::flush;
        auto &sub = omap[bVec[i]];
        vector<uint64_t> eVec;
        for (auto &it : sub)
        {
            eVec.push_back(it.first);
        }
        sort(eVec.begin(), eVec.end(), [&](const uint64_t &x, const uint64_t &y) {
            return x > y;
        });
        for (uint64_t j = 0; j < eVec.size(); j++)
        {
            outputIntervals.push_back(LCPInterval(bVec[i] - 1, eVec[j] - 1, sub[eVec[j]]));
        }
        omap.erase(bVec[i]);
    }

    text.pop_back();
}
void MinimalSubstringsConstruction::constructMSIntervalParents(vector<LCPInterval> &intervals, vector<uint64_t> &outputParents)
{
    stack<uint64_t> stack;
    outputParents.resize(intervals.size(), UINT64_MAX);
    for (uint64_t i = 0; i < intervals.size(); i++)
    {
        if (i % 100000 == 0)
            std::cout << "\r"
                      << "Computing minimal substrings... :[4/4][" << i << "/" << intervals.size() << "]" << std::flush;
        LCPInterval &interval = intervals[i];

        while (stack.size() > 0)
        {
            LCPInterval &parentInterval = intervals[stack.top()];

            if (parentInterval.i <= interval.i && interval.j <= parentInterval.j)
            {
                break;
            }
            else
            {
                stack.pop();
            }
        }
        if (stack.size() > 0)
        {
            outputParents[i] = stack.top();
        }
        stack.push(i);
    }
    std::cout << "[END]" << std::endl;
}

void MinimalSubstringsConstruction::construct(string &text, vector<LCPInterval> &outputIntervals, vector<uint64_t> &outputParents)
{
    MinimalSubstringsConstruction::constructMSIntervals(text, outputIntervals);
    MinimalSubstringsConstruction::constructMSIntervalParents(outputIntervals, outputParents);
    /*
    for (uint64_t i = 0; i < outputIntervals.size(); i++)
    {
        std::cout << outputIntervals[i].toString() << "/" << outputParents[i] << std::endl;
    }
    */

    //vector<std::unordered_set<char>> bwtEdges;
    //bwtEdges.resize(intervals.size());
}
void MinimalSubstringsConstruction::computeMinimalSubstringsTreeWithoutIndexLeave(string &text, vector<LCPInterval> &outputIntervals, vector<uint64_t> &outputParents)
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
} // namespace stool