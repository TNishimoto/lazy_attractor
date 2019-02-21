#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <set>
#include "cmdline.h"
#include "io.h"
#include "sa_lcp.hpp"
//#include <sdsl/suffix_arrays.hpp>
//#include <sdsl/suffix_trees.hpp>
//#include <sdsl/bit_vectors.hpp>
//#include "sa_lcp.hpp"
//#include "lcp_interval.hpp"
#include "minimal_substring_tree.hpp"
#include "fast_mstree.hpp"

using namespace std;
using namespace sdsl;
using namespace stool;

string toLogLine(string &text, vector<uint64_t> &sa, LCPInterval &interval)
{
    string log = "";
    string mstr = text.substr(sa[interval.i], interval.lcp);
    log.append("\"");
    log.append(mstr);
    log.append("\" ");

    log.append("SA[");
    log.append(std::to_string(interval.i));
    log.append(", ");
    log.append(std::to_string(interval.j));
    log.append("] ");
    //log.append(std::to_string(interval.lcp));
    
    log.append("occ: ");

    vector<uint64_t> occs;
    for (uint64_t x = interval.i; x <= interval.j; x++)
    {
        uint64_t pos = sa[x];
        occs.push_back(pos);
    }
    std::sort(occs.begin(), occs.end());

    for (uint64_t i = 0; i < occs.size(); i++)
    {
        uint64_t pos = occs[i];
        uint64_t endPos = occs[i] + interval.lcp - 1;
        string occ = "[" + to_string(pos) + ".." + to_string(endPos) + "]";
        log.append(occ);
    }
    
    return log;
}

int main(int argc, char *argv[])
{
#ifdef DEBUG
    std::cout << "\033[41m";
    std::cout << "DEBUG MODE!";
    std::cout << "\e[m" << std::endl;
//std::cout << "\033[30m" << std::endl;
#endif
    cmdline::parser p;
    p.add<string>("input_file", 'i', "Input text file name", true);
    p.add<string>("output_file", 'o', "(option) Output attractor file name(the default output name is 'input_file.msub')", false, "");
    p.add<string>("output_type", 't', "(option) Output mode(binary or text)", false, "binary");
    p.add<string>("algorithm", 'a', "(option) algorithm(fast or lightweight)", false, "lightweight");

    p.parse_check(argc, argv);
    string inputFile = p.get<string>("input_file");
    string outputFile = p.get<string>("output_file");
    string outputMode = p.get<string>("output_type");
    string algorithm = p.get<string>("algorithm");
    if(algorithm != "fast") algorithm = "lightweight";
    if (outputFile.size() == 0)
    {
        if (outputMode == "text")
        {
            outputFile = inputFile + ".msub.txt";
        }
        else
        {
            outputFile = inputFile + ".msub";
        }
    }
    //string parentFile = outputFile + ".parents";


    // Loading Input Text
    std::ifstream ifs(inputFile);
    bool inputFileExist = ifs.is_open();
    if (!inputFileExist)
    {
        std::cout << inputFile << " cannot open." << std::endl;
        return -1;
    }
    string text;
    IO::load(inputFile, text);

    MinimalSubstringTree mstree;
    //vector<LCPInterval> intervals;
    //vector<uint64_t> parents;
    auto start = std::chrono::system_clock::now();
    
    if(algorithm == "fast"){
        MinimalSubstringsTreeConstruction::construct(text, mstree.nodes, mstree.parents);
    }else{
        MinimalSubstringTree::construct(text, mstree.nodes, mstree.parents);
    }
    
    
    //MinimalSubstringsConstruction::computeMinimalSubstrings(text, intervals);
    auto end = std::chrono::system_clock::now();
    double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    if (outputMode == "text")
    {
        vector<uint64_t> sa;
        constructSA(text, sa);
        string otext = "";
        for (uint64_t i = 0; i < mstree.nodes.size(); i++)
        {
            otext.append(toLogLine(text, sa, mstree.nodes[i]));
            if (i + 1 != mstree.nodes.size())
                otext.append("\r\n");
        }
        IO::write(outputFile, otext);
    }
    else
    {
        mstree.write(outputFile, text);
        //IO::write(outputFile, intervals);
        //IO::write(parentFile, parents);

    }

    std::cout << "\033[36m";
    std::cout << "=============RESULT===============" << std::endl;
    std::cout << "File : " << inputFile << std::endl;
    std::cout << "Output MSUB file : " << outputFile << std::endl;
    std::cout << "The length of the input text : " << text.size() << std::endl;
    double charperms = (double)text.size() / elapsed;
    std::cout << "The number of minimal substrings : " << mstree.nodes.size() << std::endl;
    std::cout << "Algorithm : " << algorithm << std::endl;
    std::cout << "Excecution time : " << elapsed << "ms";
    std::cout << "[" << charperms << "chars/ms]" << std::endl;
    std::cout << "==================================" << std::endl;
    std::cout << "\033[39m" << std::endl;

    return 0;
}
