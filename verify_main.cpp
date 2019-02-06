#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <set>
#include "cmdline.h"
#include "io.h"
#include "sa_lcp.hpp"
#include "verification_attractor.hpp"
#include "minimal_substrings.hpp"
#include "mstree.hpp"

using namespace std;
using namespace sdsl;
using namespace stool;

template <typename List>
void split(const std::string &s, const std::string &delim, List &result)
{
    result.clear();

    using string = std::string;
    string::size_type pos = 0;

    while (pos != string::npos)
    {
        string::size_type p = s.find(delim, pos);

        if (p == string::npos)
        {
            result.push_back(s.substr(pos));
            break;
        }
        else
        {
            result.push_back(s.substr(pos, p - pos));
        }

        pos = p + delim.size();
    }
}

void loadAttractorFile(string attractorFile, string type, vector<uint64_t> &attractors)
{
    std::ifstream a_ifs(attractorFile);
    bool attractorFileExist = a_ifs.is_open();
    if (!attractorFileExist)
    {
        std::cout << attractorFile << " cannot open." << std::endl;
        throw - 1;
    }

    if (type == "text")
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
#ifdef DEBUG
    std::cout << "\033[41m";
    std::cout << "DEBUG MODE!";
    std::cout << "\e[m" << std::endl;
//std::cout << "\033[30m" << std::endl;
#endif
    cmdline::parser p;

    p.add<string>("input_file", 'i', "Input text file name", true);
    p.add<string>("attractor_file", 'a', "Attractors file name", true);
    p.add<string>("msubstr_file", 'm', "(option) Minimal substrings file name(the default minimal substrings filename is 'input_file.msub')", false, "");
    p.add<string>("attractor_file_type", 't', "(option) Input attractor file type(binary or text)", false, "binary");
    p.add<string>("output_file", 'o', "(option) Error log file name (the default output name is 'input_file.verify.log')", false, "");

    p.parse_check(argc, argv);
    string inputFile = p.get<string>("input_file");
    string mSubstrFile = p.get<string>("msubstr_file");
    string attractorFile = p.get<string>("attractor_file");
    string attractorFileType = p.get<string>("attractor_file_type");

    string outputFile = p.get<string>("output_file");

    if (outputFile.size() == 0)
    {
        outputFile = inputFile + ".verify.log";
    }

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

    // Loading Attractor File
    vector<uint64_t> attractors;
    loadAttractorFile(attractorFile, attractorFileType, attractors);

    // Loading minimal Substrings
    if (mSubstrFile.size() == 0)
    {
        mSubstrFile = inputFile + ".msub";
    }
    string mSubstrParentFile = mSubstrFile + ".parents";
    vector<LCPInterval> intervals;
    vector<uint64_t> parents;

    std::ifstream m_ifs(mSubstrFile);
    bool mSubstrFileExist = m_ifs.is_open();
    if (!mSubstrFileExist)
    {
        MinimalSubstringsTreeConstruction::construct(text, intervals, parents);
        IO::write(mSubstrFile, intervals);
        IO::write(mSubstrParentFile, parents);
    }
    else
    {
        IO::load<LCPInterval>(mSubstrFile, intervals);
        IO::load<uint64_t>(mSubstrParentFile, parents);
    }

    vector<uint64_t> sa, isa, freeIntervalIndexes;
    auto start = std::chrono::system_clock::now();
    constructSA(text, sa, isa);
    VerificationAttractor::getFreeIntervals(sa, isa, intervals, attractors, freeIntervalIndexes);
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
            LCPInterval interval = intervals[freeIntervalIndexes[j]];
            string mstr = text.substr(sa[interval.i], interval.lcp);
            log.append(mstr);
            log.append("\" occs:");

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
    std::cout << "=============RESULT===============" << std::endl;
    std::cout << "File : " << inputFile << std::endl;
    std::cout << "Attractor File : " << attractorFile << std::endl;
    std::cout << "The length of the input text : " << text.size() << std::endl;
    double charperms = (double)text.size() / elapsed;
    std::cout << "The number of minimal substrings : " << intervals.size() << std::endl;
    std::cout << "The number of attractors : " << attractors.size() << std::endl;
    std::cout << "Excecution time : " << ((uint64_t)elapsed) << "ms";
    std::cout << "[" << charperms << "chars/ms]" << std::endl;
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
    std::cout << "==================================" << std::endl;
    std::cout << "\033[39m" << std::endl;

    return 0;
}
