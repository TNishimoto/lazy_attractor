#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <set>
#include "cmdline.h"
#include <sdsl/suffix_arrays.hpp>
#include <sdsl/suffix_trees.hpp>
#include <sdsl/bit_vectors.hpp>

#include "sa_lcp.hpp"
#include "lcp_interval.hpp"
#include "minimal_substrings.hpp"
#include "mstree.hpp"
#include "offline_suffix_tree.hpp"

using namespace std;
using namespace sdsl;
using namespace stool;

void computeMinimalSubstrings(string &text, vector<LCPInterval> &intervals)
{

    CST cst;
    printf("  Done. Constructing CST... This may take 5 minutes or so...\n");
    auto start = std::chrono::system_clock::now();
    construct_im(cst, text, 1);
    auto end = std::chrono::system_clock::now();
    double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "created cst[" << elapsed << "ms]" << std::endl;

    uint64_t size = cst.size();
    //for (int64_t x = size-1; x >= 0; x--)
    uint64_t x = 0;
    uint64_t z = 0;

    vector<bool> checker;
    checker.resize(size, false);
    for (auto &p : cst)
    {
        x++;
        //z += cst.id(cst.sl(p));
        uint64_t id = cst.id(p);
        if (!checker[id])
        {
            std::cout << cst.lb(p) << "/" << cst.rb(p) << std::endl;
            checker[id] = true;
        }
        /*
        if (x % 1000 == 0)
            std::cout << "\r"
                      << "computing LCP intervals... : [" << x << "/" << size << "]" << std::flush;
                      */
    }
    /*
    for (uint64_t x = 0; x < size; x++)
    {
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
    */
    std::cout << "done." << std::endl;
}
void check(string &text, vector<LCPInterval> &intervals, vector<uint64_t> &parentVec)
{

    vector<LCPInterval> intervals2;
    vector<uint64_t> parentVec2;
    MinimalSubstringsTreeConstruction::computeMinimalSubstringsTreeWithoutIndexLeave(text, intervals2, parentVec2);

    for (uint64_t i = 0; i < intervals.size(); i++)
    {
        std::cout << i << intervals[i].toString() << "/" << parentVec[i] << std::endl;
    }
    for (uint64_t i = 0; i < intervals2.size(); i++)
    {
        std::cout << i << intervals2[i].toString() << "/" << parentVec2[i] << std::endl;
    }

    bool b = true;
    if (intervals.size() == intervals2.size())
    {
        for (uint64_t i = 0; i < intervals2.size(); i++)
        {
            if (parentVec[i] != parentVec2[i])
            {
                b = false;
            }
            if (intervals[i].i != intervals2[i].i)
            {
                b = false;
            }
            if (intervals[i].j != intervals2[i].j)
            {
                b = false;
            }
            if (intervals[i].lcp != intervals2[i].lcp)
            {
                b = false;
            }
        }
    }
    else
    {
        b = false;
    }
    std::cout << std::endl;
    std::cout << (b ? "OK!" : "OUT") << std::endl;
}
int main(int argc, char *argv[])
{
    
    cmdline::parser p;
    p.add<string>("input_file", 'i', "input file name", true);
    p.add<string>("output_file", 'o', "output file name (the default output name is 'input_file.ext')", false, "");

    p.parse_check(argc, argv);
    string inputFile = p.get<string>("input_file");
    string outputFile = p.get<string>("output_file");

    string text;
    IO::load(inputFile, text);
    vector<LCPInterval> intervals1, intervals2;
    vector<uint64_t> parents1, parents2;

    auto start1 = std::chrono::system_clock::now();
    MinimalSubstringsConstruction::construct(text, intervals1, parents1);
    auto end1 = std::chrono::system_clock::now();
    double elapsed1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();

    auto start2 = std::chrono::system_clock::now();
    MinimalSubstringsTreeConstruction::construct(text, intervals2, parents2);
    auto end2 = std::chrono::system_clock::now();
    double elapsed2 = std::chrono::duration_cast<std::chrono::milliseconds>(end2 - start2).count();

    std::cout << intervals1.size() << "/" << intervals2.size() << std::endl;
    std::cout << elapsed1 << "/" << elapsed2 << std::endl;

    for(uint64_t i=0;i<intervals1.size();i++){
        if(intervals1[i].i != intervals2[i].i){
            throw -1;
        }
        if(intervals1[i].j != intervals2[i].j){
            throw -1;
        }
        if(intervals1[i].lcp != intervals2[i].lcp){
            throw -1;
        }
        if(parents1[i] != parents2[i]){
            throw -1;
        }

    }

    /*
    // Loading Minimal Substrings
    vector<LCPInterval> intervals;
    std::ifstream m_ifs(inputFile);
    bool mSubstrFileExist = m_ifs.is_open();
    if (!mSubstrFileExist)
    {
        return 1;
    }
    else
    {
        IO::load<LCPInterval>(inputFile, intervals);
    }
    uint64_t mSubstrCount = intervals.size();

    std::cout << "\033[36m";
    std::cout << "=============RESULT===============" << std::endl;
    std::cout << "File : " << inputFile << std::endl;
    std::cout << "The number of minimal substrings : " << mSubstrCount << std::endl;
    std::cout << "==================================" << std::endl;
    std::cout << "\033[39m" << std::endl;
    */

}
