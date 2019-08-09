#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <set>
#include "stool/src/cmdline.h"
#include "stool/src/io.h"
#include "stool/src/sa_bwt_lcp.hpp"
#include "../src/verification_attractor.hpp"
#include "esaxx/src/minimal_substrings/minimal_substring_iterator.hpp"
#include "../src/common.hpp"
//#include "minimal_substrings.hpp"
//#include "mstree.hpp"
//#include "esaxx/src/minimal_substrings/minimal_substring_tree.hpp"

using namespace std;
using namespace sdsl;
using namespace stool;
//using namespace stool::lazy;

bool checkAttractorTextFile(string &text)
{
    std::unordered_set<char> okSet;
    okSet.insert('0');
    okSet.insert('1');
    okSet.insert('2');
    okSet.insert('3');
    okSet.insert('4');
    okSet.insert('5');
    okSet.insert('6');
    okSet.insert('7');
    okSet.insert('8');
    okSet.insert('9');
    okSet.insert(' ');
    okSet.insert(',');
    okSet.insert('\n');
    okSet.insert('\r');
    for (auto &it : text)
    {
        if (okSet.find(it) == okSet.end())
        {
            return false;
        }
    }
    return true;
}
std::string substr(std::vector<uint8_t> &text, uint64_t i, uint64_t len){
    std::string str;
    str.resize(len);
    for(uint64_t x=0;x<len;x++){
        if(x+i >= text.size()){
            throw -1;
        }
        str[x] = (char)text[x+i];
    }
    return str;
}

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
void loadAttractorFile(std::string attractorFile, std::string type, std::vector<uint64_t> &attractors)
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
        std::string text;
        IO::load(attractorFile, text);
        bool b = checkAttractorTextFile(text);
        if (!b)
        {
            std::cout << "\033[31m";
            std::cout << "Error: the text file for attractors allow to contain 0-9 characters, spaces, and commas.";
            std::cout << "\033[39m" << std::endl;
            std::exit(EXIT_FAILURE);
        }

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
        stool::load_vector(attractorFile, attractors);
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
    p.add<string>("attractor_file", 'a', "Attractors file name", true);
    p.add<string>("msubstr_file", 'm', "(option) Minimal substrings file name(the default minimal substrings filename is 'input_file.msub')", false, "");
    p.add<string>("attractor_file_type", 't', "(option) Input attractor file type(binary or text)", false, "binary");
    p.add<string>("output_file", 'o', "(option) Error log file name (the default output name is 'input_file.verify.log')", false, "");
    p.add<uint64_t>("k-attr", 'k', "(option) the value of k-attractor", false, 0 );

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
    vector<CHAR> text = stool::load_text_from_file(inputFile, true);

    // Loading Attractor File
    vector<uint64_t> attractors;
    loadAttractorFile(attractorFile, attractorFileType, attractors);


    std::cout << "Constructing Suffix Array" << std::endl;
    std::vector<INDEX> sa = stool::constructSA<CHAR, INDEX>(text);

    // Loading Minimal Substrings
    std::vector<stool::LCPInterval<uint64_t>> minimalSubstrings = stool::lazy::loadOrConstructMS(mSubstrFile, text,sa, k_attr);

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
            string mstr = substr(text, sa[interval.i], interval.lcp);
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
    std::cout << "=============RESULT===============" << std::endl;
    std::cout << "File : " << inputFile << std::endl;
    std::cout << "Attractor File : " << attractorFile << std::endl;
        std::cout << "Attractor type : " << (k_attr == 0 ? "n" : std::to_string(k_attr) )  << "-attractor" << std::endl;
    std::cout << "The length of the input text (with last special marker) : " << text.size() << std::endl;
    double charperms = (double)text.size() / elapsed;
    std::cout << "The number of minimal substrings : " << minimalSubstrings.size() << std::endl;
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
