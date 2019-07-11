#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <set>
#include "cmdline.h"
#include "io.h"
#include "sa_lcp.hpp"
#include "minimal_substring_tree.hpp"
#include "greedy_attractor.hpp"

//using namespace std;
using namespace sdsl;
using namespace stool;

int main(int argc, char *argv[])
{
#ifdef DEBUG
    std::cout << "\033[41m";
    std::cout << "DEBUG MODE!";
    std::cout << "\e[m" << std::endl;
//std::cout << "\033[30m" << std::endl;
#endif
    cmdline::parser p;
    p.add<std::string>("input_file", 'i', "Input text file name", true);
    p.add<std::string>("output_file", 'o', "(option) Output attractor file name(the default output name is 'input_file.greedy.attrs')", false, "");
    p.add<std::string>("output_type", 't', "(option) Output mode(binary or text)", false, "binary");
    p.add<std::string>("msubstr_file", 'm', "(option) Minimal substrings file name(the default minimal substrings filename is 'input_file.msub')", false, "");
    p.add<uint>("block_size", 'b', "(option) block size", false, 1000);

    p.parse_check(argc, argv);
    std::string inputFile = p.get<std::string>("input_file");
    std::string mSubstrFile = p.get<std::string>("msubstr_file");
    std::string outputFile = p.get<std::string>("output_file");
    std::string outputMode = p.get<std::string>("output_type");
    uint blockSize = p.get<uint>("block_size");
    if (blockSize < 5)
        blockSize = 5;

    if (outputFile.size() == 0)
    {
        if (outputMode == "text")
        {
            outputFile = inputFile + ".greedy.attrs.txt";
        }
        else
        {
            outputFile = inputFile + ".greedy.attrs";
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
    std::string text;
    IO::load(inputFile, text);

    // Loading Minimal Substrings
    if (mSubstrFile.size() == 0)
    {
        mSubstrFile = inputFile + ".msub";
    }
    MinimalSubstringTree mstree;    
    mstree.loadOrConstruct(mSubstrFile, &text);

    uint64_t mSubstrCount = mstree.nodes.size();

    std::vector<uint64_t> sa, attrs;
    auto start = std::chrono::system_clock::now();
    constructSA(text, sa);
    GreedyAttractorAlgorithm::compute(sa, mstree.nodes, blockSize, attrs);
    auto end = std::chrono::system_clock::now();
    double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    if (outputMode == "text")
    {
        std::string otext = "";
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
    std::cout << "The length of the input text : " << text.size() << std::endl;
    double charperms = (double)text.size() / elapsed;
    std::cout << "The number of minimal substrings : " << mSubstrCount << std::endl;

    std::cout << "The number of attractors : " << attrs.size() << std::endl;
    std::cout << "Block size : " << blockSize << std::endl;
    std::cout << "Excecution time : " << ((uint64_t)elapsed) << "ms";
    std::cout << "[" << charperms << "chars/ms]" << std::endl;
    std::cout << "==================================" << std::endl;
    std::cout << "\033[39m" << std::endl;
    return 0;
}
