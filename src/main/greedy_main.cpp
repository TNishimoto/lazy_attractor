#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <set>
#include "stool/include/io.hpp"
#include "stool/include/io.hpp"
#include "stool/include/print.hpp"

#include "stool/include/cmdline.h"

#include "esaxx/include/minimal_substrings/minimal_substring_iterator.hpp"
//#include "sa.hpp"
#include "../src/greedy/greedy_attractor.hpp"
#include "../src/greedy/position_frequency_set.hpp"
#include "../src/common.hpp"
#include "libdivsufsort/sa.hpp"

//using namespace std;
//using namespace sdsl;
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
    p.add<std::string>("input_file", 'i', "Input text file name", true);
    p.add<std::string>("output_file", 'o', "(option) Output attractor file name(the default output name is 'input_file.greedy.attrs')", false, "");
    p.add<std::string>("output_type", 't', "(option) Output mode(binary or text)", false, "binary");
    p.add<uint64_t>("k-attr", 'k', "(option) the value of k-attractor", false, 0);

    p.add<std::string>("msubstr_file", 'm', "(option) The file name of minimal substrings for the input text file(the default minimal substrings filename is 'input_file.msub')", false, "");

    p.parse_check(argc, argv);
    std::string inputFile = p.get<std::string>("input_file");
    std::string mSubstrFile = p.get<std::string>("msubstr_file");
    std::string outputFile = p.get<std::string>("output_file");
    std::string outputMode = p.get<std::string>("output_type");
    uint64_t k_attr = p.get<uint64_t>("k-attr");

    if (outputFile.size() == 0)
    {
        std::string ext = ".greedy";
        if (k_attr != 0)
        {
            ext += "." + std::to_string(k_attr);
        }
        ext += ".attrs";
        if (outputMode == "text")
        {
            ext += ".txt";
        }
        outputFile = inputFile + ext;
    }

    // Loading Input Text
    std::vector<uint8_t> text = stool::load_text_from_file(inputFile, true); // input text

    std::cout << "Constructing Suffix Array" << std::endl;
    std::vector<INDEX> sa = stool::construct_suffix_array(text);

    // Loading Minimal Substrings
    std::vector<stool::LCPInterval<uint64_t>> minimalSubstrings = stool::lazy::loadOrConstructMS(mSubstrFile, text, sa, k_attr);

    uint64_t mSubstrCount = minimalSubstrings.size();

    std::vector<uint64_t> attrs;
    auto start = std::chrono::system_clock::now();
    //std::vector<uint64_t> sa = stool::constructSA<uint8_t, uint64_t>(text);

    if (outputMode == "weight")
    {
        std::vector<uint64_t> weights = stool::lazy::PositionFrequencySet::computeFrequencyVector(sa, minimalSubstrings);
        uint64_t sum = std::accumulate(weights.begin(), weights.end(), (uint64_t)0);
        auto end = std::chrono::system_clock::now();
        double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        std::cout << "\033[36m";
        std::cout << "=============RESULT===============" << std::endl;
        std::cout << "File : " << inputFile << std::endl;
        std::cout << "Output : " << outputFile << std::endl;
        std::cout << "The length of the input text (with the last special marker): " << text.size() << std::endl;
        double charperms = (double)text.size() / elapsed;
        std::cout << "The number of minimal substrings : " << mSubstrCount << std::endl;
        std::cout << "The sum of position weights : " << sum << std::endl;
        std::cout << "Excecution time : " << ((uint64_t)elapsed) << "ms";
        std::cout << "[" << charperms << "chars/ms]" << std::endl;
        std::cout << "==================================" << std::endl;
        std::cout << "\033[39m" << std::endl;
    }
    else
    {
        std::vector<INDEX> isa = stool::constructISA<CHAR, INDEX>(text, sa);
        std::vector<uint64_t> greedyAttrs = GreedyAttractorAlgorithm::computeFasterGreedyAttractors(sa, isa, minimalSubstrings);

#ifdef DEBUG
        std::vector<uint64_t> correctAttrs = GreedyAttractorAlgorithm::computeGreedyAttractors(sa, minimalSubstrings);
        if (correctAttrs.size() != greedyAttrs.size())
        {
            stool::Printer::print("Correct Greedy Attractors", correctAttrs);
            stool::Printer::print("Greedy Attractors        ", greedyAttrs);
        }
        assert(correctAttrs.size() == greedyAttrs.size());

        std::cout << "CORRECT!" << std::endl;

#endif

        attrs.swap(greedyAttrs);

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
            stool::write_vector(outputFile, attrs, true);
            //IO::write(outputFile, attrs, UINT64_MAX - 1);
        }
        std::cout << "\033[36m";
        std::cout << "============= INFO ===============" << std::endl;
        std::cout << "File : " << inputFile << std::endl;
        std::cout << "The input file content: ";
        if (text.size() <= 100)
        {
            std::vector<char> s;
            stool::lazy::load_vector(inputFile, s, false, false);
            for (auto &c : s)
                std::cout << c;
            std::cout << std::endl;
        }
        else
        {
            std::cout << "(we omit to print it on the console if its size is larger than 100)" << std::endl;
        }
        std::cout << "Output : " << outputFile << std::endl;
        std::cout << "Attractor type : " << (k_attr == 0 ? "n" : std::to_string(k_attr)) << "-attractor" << std::endl;

        std::cout << "\033[33m";
        std::cout << "=============RESULT===============" << std::endl;
        std::cout << "The length of the input text (with the last special marker): " << text.size() << std::endl;
        double charperms = (double)text.size() / elapsed;
        std::cout << "The number of minimal substrings : " << mSubstrCount << std::endl;
        std::cout << "The number of attractors : " << attrs.size() << std::endl;
        std::cout << "The obtained attractors: ";
        if (attrs.size() <= 100)
        {
            stool::Printer::print(attrs);
        }
        else
        {
            std::cout << "(we omit to print it on the console if its size is larger than 100)" << std::endl;
        }

        std::cout << "Excecution time : " << ((uint64_t)elapsed) << "ms";
        std::cout << "[" << charperms << "chars/ms]" << std::endl;
        std::cout << "==================================" << std::endl;
        std::cout << "\033[39m" << std::endl;
        return 0;
    }
}
