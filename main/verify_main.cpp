#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <set>
#include "stool/include/cmdline.h"
#include "stool/include/io.hpp"
#include "stool/include/sa_bwt_lcp.hpp"
#include "../include/verification_attractor.hpp"
#include "esaxx/include/minimal_substrings/minimal_substring_iterator.hpp"
#include "../include/common.hpp"
#include "libdivsufsort/sa.hpp"
//#include "minimal_substrings.hpp"
//#include "mstree.hpp"
//#include "esaxx/include/minimal_substrings/minimal_substring_tree.hpp"

using namespace std;
using namespace sdsl;
using namespace stool;
//using namespace stool::lazy;



/*
void loadAttractors(string attractorFile, vector<uint64_t> &attractors)
{
    std::ifstream a_ifs(attractorFile);
    bool attractorFileExist = a_ifs.is_open();
    if (!attractorFileExist)
    {
        std::cout << attractorFile << " cannot open." << std::endl;
        throw - 1;
    }

    uint64_t len = attractorFile.size();
    bool b = false;
    if (len >= 4)
    {
        string ext = attractorFile.substr(len - 4);
        b = ext == ".txt";
    }
    else
    {
        b = false;
    }

    if (b)
    {
        string text;
        IO::load(attractorFile, text);
        vector<string> strs;
        split(text, ",", strs);
        for (auto &character : strs)
        {
            uint64_t c = atoi(character.c_str());
            attractors.push_back(c);
        }
    }
    else
    {
        IO::load<uint64_t>(attractorFile, attractors);
    }

    sort(attractors.begin(), attractors.end());
}
*/


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
    p.add<string>("msubstr_file", 'm', "(option) Minimal substrings file name(the default minimal substrings filename is 'input_file.msub')", false, "");
    p.add<string>("attractor_file", 'a', "Attractors file name", true);
    p.add<string>("attractor_file_type", 't', "(option) Input attractor file type(binary or text)", true);
    p.add<string>("output_file", 'o', "(option) Error log file name (the default output name is 'input_file.verify.log')", false, "");
    p.add<uint64_t>("k-attr", 'k', "(option) the value of k-attractor", false, 0);

    p.parse_check(argc, argv);
    string inputFile = p.get<string>("input_file");
    string mSubstrFile = p.get<string>("msubstr_file");
    string attractorFile = p.get<string>("attractor_file");
    string attractorFileType = p.get<string>("attractor_file_type");
    uint64_t k_attr = p.get<uint64_t>("k-attr");

    string outputFile = p.get<string>("output_file");

    if (outputFile.size() == 0)
    {
        outputFile = inputFile + ".verify.log";
    }

    // Loading Input Text
    vector<CHAR> text; 
    stool::IO::load(inputFile, text);
    text.push_back(0);

    // Loading Attractor File
    vector<uint64_t> attractors;
    stool::lazy::loadAttractorFile(attractorFile, attractorFileType, attractors);

    std::cout << "Constructing Suffix Array" << std::endl;
    std::vector<INDEX> sa = stool::construct_suffix_array(text);

    // Loading Minimal Substrings
    std::vector<stool::LCPInterval<uint64_t>> minimalSubstrings = stool::lazy::loadOrConstructMS(mSubstrFile, text, sa, k_attr);

    auto start = std::chrono::system_clock::now();
    vector<uint64_t> isa = stool::constructISA(text, sa);
    vector<uint64_t> freeIntervalIndexes = lazy::VerificationAttractor::getFreeIntervals(sa, isa, minimalSubstrings, attractors);
    auto end = std::chrono::system_clock::now();
    double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    if (freeIntervalIndexes.size() == 0)
    {
    }
    else
    {
        string log = "\"";
        for (uint64_t j = 0; j < freeIntervalIndexes.size(); j++)
        {
            LCPInterval<INDEX> interval = minimalSubstrings[freeIntervalIndexes[j]];
            //uint64_t spos = *(sa.begin()+interval.i);
            //string mstr(text.begin() + spos, text.begin()+ spos + interval.lcp -1);
            string mstr = stool::lazy::substr(text, sa[interval.i], interval.lcp);
            //string mstr = text.substr(sa[interval.i], interval.lcp);
            log.append(mstr);
            log.append("\" occs: ");

            for (uint64_t x = interval.i; x <= interval.j; x++)
            {
                uint64_t pos = sa[x];
                uint64_t endPos = sa[x] + interval.lcp - 1;
                string occ = "[" + to_string(pos) + ".." + to_string(endPos) + "]";
                log.append(occ);

                if (x - interval.i > 20)
                {
                    log.append(", and so on");
                    break;
                }
            }
            log.append("\r\n");

            if (j > 100 || log.size() > 1000000)
            {
                log.append(", and so on");
                break;
            }
        }
        IO::write(outputFile, log);
    }

    std::cout << "\033[36m";
    std::cout << "============= INFO ===============" << std::endl;
    std::cout << "File : " << inputFile << std::endl;
    std::cout << "The input file content: ";
    if (text.size() <= 100)
    {
        std::vector<char> s;
        //stool::load_text
        stool::lazy::load_vector(inputFile, s, false, false);
        for (auto &c : s)
            std::cout << c;
        std::cout << std::endl;
    }
    else
    {
        std::cout << "(we omit to print it on the console if its size is larger than 100)" << std::endl;
    }

    std::cout << "Attractor File : " << attractorFile << std::endl;
    std::cout << "Attractor type : " << (k_attr == 0 ? "n" : std::to_string(k_attr)) << "-attractor" << std::endl;
    std::cout << "The length of the input text (with last special marker) : " << text.size() << std::endl;
    double charperms = (double)text.size() / elapsed;
    std::cout << "The number of minimal substrings : " << minimalSubstrings.size() << std::endl;
    std::cout << "The number of attractors : " << attractors.size() << std::endl;

    std::cout << "\033[33m";
    std::cout << "=============RESULT===============" << std::endl;
    std::cout << "Attractor? : ";
    if (freeIntervalIndexes.size() == 0)
    {
        std::cout << "YES" << std::endl;
    }
    else
    {
        std::cout << "NO" << std::endl;
        std::cout << "Output minimal substrings not containing the input positions." << std::endl;
        std::cout << "See also " << outputFile << std::endl;
    }
    std::cout << "Excecution time : " << ((uint64_t)elapsed) << "ms";
    std::cout << "[" << charperms << "chars/ms]" << std::endl;
    std::cout << "==================================" << std::endl;
    std::cout << "\033[39m" << std::endl;

    return 0;
}
