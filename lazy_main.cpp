#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <set>
#include "stool/src/cmdline.h"
#include "stool/src/io.hpp"
#include "stool/src/io.h"
#include "stool/src/sa_bwt_lcp.hpp"
//#include "mstree.hpp"
//#include "greedy_attractor.hpp"
#include "lazy_attractor.hpp"
#include "stool/src/print.hpp"
#include "esaxx/src/minimal_substrings/minimal_substring_tree.hpp"

using namespace std;
using namespace sdsl;
using namespace stool;
using namespace stool::lazy;

int main(int argc, char *argv[])
{
    using CHAR = uint8_t;
    using INDEX = uint64_t;
#ifdef DEBUG
    std::cout << "\033[41m";
    std::cout << "DEBUG MODE!";
    std::cout << "\e[m" << std::endl;
//std::cout << "\033[30m" << std::endl;
#endif
    cmdline::parser p;
    p.add<string>("input_file", 'i', "Input text file name", true);
    p.add<string>("output_file", 'o', "(option) Output attractor file name(the default output name is 'input_file.lazy.attrs')", false, "");
    p.add<string>("output_type", 't', "(option) Output mode(binary or text)", false, "binary");
    p.add<string>("msubstr_file", 'm', "(option) Minimal substrings file name(the default minimal substrings filename is 'input_file.msub')", false, "");

    p.parse_check(argc, argv);
    string inputFile = p.get<string>("input_file");
    string outputFile = p.get<string>("output_file");
    string outputMode = p.get<string>("output_type");
    string mSubstrFile = p.get<string>("msubstr_file");

    if (outputFile.size() == 0)
    {
        if (outputMode == "text")
        {
            outputFile = inputFile + ".lazy.attrs.txt";
        }
        else
        {
            outputFile = inputFile + ".lazy.attrs";
        }
    }

    // Loading Input Text
    std::ifstream ifs(inputFile);
    bool inputFileExist = ifs.is_open();
    if (!inputFileExist)
    {
        std::cout << inputFile << " cannot open." << std::endl;
        return -1;
    }
    //string text;
    //IO::load(inputFile, text);
    std::vector<uint8_t> text = stool::load_text_from_file(inputFile, true); // input text

    // Loading Minimal Substrings
    if (mSubstrFile.size() == 0)
    {
        mSubstrFile = inputFile + ".msub";
    }
    stool::esaxx::MinimalSubstringTree<CHAR,INDEX> mstree;    
    //mstree.loadOrConstruct(text, mSubstrFile);

    //text.push_back(0);
    //mstree.loadOrConstruct(mSubstrFile, &text);

    stool::esaxx::MinimalSubstringTree<uint8_t, uint64_t>::construct(text, mstree.nodes, mstree.parents);
    //text.pop_back();

    uint64_t mSubstrCount = mstree.nodes.size();
    vector<uint64_t> attrs;

    auto start = std::chrono::system_clock::now();
    //LazyAttractorAlgorithm algo(text, intervals, parents);
    LazyAttractor::computeAttractors(text, mstree.nodes, mstree.parents, attrs);
    auto end = std::chrono::system_clock::now();
    double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    if (outputMode == "text")
    {
        string otext = "";
        for (uint64_t i = 0; i < attrs.size(); i++)
        {
            otext.append(std::to_string(attrs[i]));
            if (i + 1 != attrs.size())
                otext.append(",");
        }
        IO::write(outputFile, otext);
    }
    else
    {
        IO::write(outputFile, attrs, UINT64_MAX-1);
    }

    std::cout << "\033[36m";
    std::cout << "=============RESULT===============" << std::endl;
    std::cout << "File : " << inputFile << std::endl;
    std::cout << "Output : " << outputFile << std::endl;
    std::cout << "The length of the input text (with last special marker) : " << text.size() << std::endl;
    double charperms = (double)text.size() / elapsed;
    std::cout << "The number of minimal substrings : " << mSubstrCount << std::endl;
    std::cout << "The number of attractors : " << attrs.size() << std::endl;
    std::cout << "Excecution time : " << ((uint64_t)elapsed) << "ms";
    std::cout << "[" << charperms << "chars/ms]" << std::endl;
    std::cout << "==================================" << std::endl;
    std::cout << "\033[39m" << std::endl;

    return 0;
}
