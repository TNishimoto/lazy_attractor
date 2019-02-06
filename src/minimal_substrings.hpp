#pragma once
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

using CST = cst_sct3<csa_bitcompressed<>>;
//using CST = csa_wt<wt_int<rrr_vector<>>>;
namespace stool
{
class CSTHelper
{
  public:
    static string getStr(CST &cst, string &text, uint64_t id)
    {
        auto v = cst.inv_id(id);
        uint64_t len = cst.depth(v);
        uint64_t pos = cst.csa[cst.lb(v)];
        if (pos == text.size())
        {
            return len == 0 ? "" : "#";
        }
        else
        {
            if (pos + len == text.size())
            {
                return text.substr(pos) + "#";
            }
            else
            {
                return text.substr(pos, len);
            }
        }
    }
    static bool isMinimalSubstring(CST &cst, string &text, uint64_t id, uint64_t childNth)
    {
        auto v = cst.inv_id(id);
        uint64_t len = cst.depth(v);
        char c = getBranchChracter(cst, text, id, childNth);
        if (c == 0)
            return false;
        auto v1 = cst.select_child(v, childNth + 1);
        uint64_t occCount1 = cst.rb(v1) - cst.lb(v1) + 1;

        if (len == 0)
        {
            return true;
        }
        else
        {
            auto sl = cst.sl(v);
            auto v2 = cst.child(sl, c);
            uint64_t occCount2 = cst.rb(v2) - cst.lb(v2) + 1;
            return occCount1 != occCount2;
        }
    }
    static char getBranchChracter(CST &cst, string &text, uint64_t id, uint64_t childNth)
    {
        auto v = cst.inv_id(id);

        uint64_t len = cst.depth(v);
        uint64_t degree = cst.degree(v);
        auto child = cst.select_child(v, childNth + 1);
        uint64_t lb = cst.lb(child);
        uint64_t pos = cst.csa[cst.lb(child)];
        if (pos + len == text.size())
        {
            return 0;
        }
        else
        {
            char pc = text[pos + len];
            return pc;
        }
    }
    static void constructESA2(string &text, vector<LCPInterval> &outputIntervals, vector<uint64_t> &outputParentVec, vector<bool> &msVec)
    {

        CST cst;
        printf("  Done. Constructing CST... This may take 5 minutes or so...\n");
        auto start = std::chrono::system_clock::now();
        construct_im(cst, text, 1);
        auto end = std::chrono::system_clock::now();
        double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << "created cst[" << elapsed << "ms]" << std::endl;

        vector<bool> spec;
        vector<uint64_t> orderVec;
        uint64_t csize = cst.size(cst.root()) * 2;
        spec.resize(csize);
        orderVec.resize(csize, UINT64_MAX);
        uint64_t x = 0;
        for (auto &p : cst)
        {
            uint64_t id = cst.id(p);

            if (id >= spec.size())
                throw - 1;
            if (!spec[id])
            {
                orderVec[id] = x;
                x++;
                spec[id] = true;
                if (x % 1000 == 0)
                    std::cout << "\r"
                              << "computing LCP intervals... : [" << x << "/" << csize << "]" << std::flush;
            }
        }

        uint64_t size = x;
        outputIntervals.resize(size, LCPInterval(UINT64_MAX, UINT64_MAX, UINT64_MAX));
        outputParentVec.resize(size, UINT64_MAX);
        vector<bool> checker;
        checker.resize(size, false);
        msVec.resize(size, false);

        vector<uint64_t> slvec;
        slvec.resize(size);

        uint64_t x3 = 0;
        for (auto &p : cst)
        {
            uint64_t id = cst.id(p);
            uint64_t xid = orderVec[id];
            if (!checker[xid])
            {
                x3++;
                if (x3 % 1000 == 0)
                    std::cout << "\r"
                              << "computing LCP intervals... : [" << x3 << "/" << size << "]" << std::flush;

                slvec[xid] = cst.id(cst.sl(p));
                uint64_t depth = cst.depth(p);
                if (depth > 0)
                {
                    auto parent = cst.parent(p);
                    auto parentID = cst.id(parent);
                    outputParentVec[xid] = orderVec[parentID];
                }
            }
        }

        uint64_t x2 = 0;
        for (auto &p : cst)
        {
            uint64_t id = cst.id(p);
            uint64_t xid = orderVec[id];
            if (!checker[xid])
            {
                x2++;
                if (x2 % 1000 == 0)
                    std::cout << "\r"
                              << "computing LCP intervals... : [" << x2 << "/" << size << "]" << std::flush;

                uint64_t lb = cst.lb(p);
                uint64_t rb = cst.rb(p);
                uint64_t depth = cst.depth(p);
                outputIntervals[xid] = LCPInterval(lb, rb, depth);
                if (depth > 0)
                {
                    auto parent = cst.parent(p);
                    auto parentID = cst.id(parent);
                    outputParentVec[xid] = orderVec[parentID];
                }
                uint64_t degree = cst.degree(p);
                for (uint64_t z = 0; z < degree; z++)
                {
                    bool b = CSTHelper::isMinimalSubstring(cst, text, id, z);
                    if (b)
                    {
                        auto child = cst.select_child(p, z + 1);
                        msVec[orderVec[cst.id(child)]] = true;
                    }
                }
                checker[xid] = true;
            }
        }
        /*
        for (uint64_t i = 0; i < size; i++)
        {
            std::cout << "aaa" << i << "/" << outputIntervals[i].toString() << outputParentVec[i] << std::endl;
        }
        */
    }
    static void constructMSTree(string &text, vector<LCPInterval> &outputIntervals, vector<uint64_t> &outputParentVec)
    {
        vector<LCPInterval> tmpIntervals;
        vector<uint64_t> tmpParentVec;
        vector<bool> msVec;
        constructESA2(text, tmpIntervals, tmpParentVec, msVec);
        vector<uint64_t> parentMSVec;

        parentMSVec.resize(tmpIntervals.size(), UINT64_MAX);

        for (uint64_t i = 0; i < tmpIntervals.size(); i++)
        {
            auto &interval = tmpIntervals[i];
            if (msVec[i])
            {
                if (interval.i != 0)
                {
                    uint64_t lcp = tmpParentVec[i] == UINT64_MAX ? 1 : tmpIntervals[tmpParentVec[i]].lcp + 1;
                    outputIntervals.push_back(LCPInterval(interval.i - 1, interval.j - 1, lcp));
                    uint64_t msParent = parentMSVec[tmpParentVec[i]];
                    outputParentVec.push_back(msParent);
                    parentMSVec[i] = outputIntervals.size() - 1;
                }
            }
            else
            {
                uint64_t m = tmpParentVec[i] == UINT64_MAX ? UINT64_MAX : parentMSVec[tmpParentVec[i]];
                parentMSVec[i] = m;
            }
        }
    }
};

class MinimalSubstringsConstruction
{
  private:
    static void constructMSIntervals(string &text, vector<LCPInterval> &outputIntervals);
    static void constructMSIntervalParents(vector<LCPInterval> &intervals, vector<uint64_t> &outputParents);

  public:
    static void computeMinimalSubstrings(string &text, vector<LCPInterval> &intervals);
    static void construct(string &text, vector<LCPInterval> &outputIntervals, vector<uint64_t> &outputParents);
};

} // namespace stool
