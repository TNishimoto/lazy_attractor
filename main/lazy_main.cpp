#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <set>
#include "stool/include/cmdline.h"
#include "stool/include/io.hpp"
#include "stool/include/io.hpp"
#include "stool/include/sa_bwt_lcp.hpp"
#include "stool/include/print.hpp"
#include "stool/include/debug.hpp"

#include "../include/lazy/lazy_attractor.hpp"
#include "stool/include/print.hpp"
#include "esaxx/include/minimal_substrings/minimal_substring_iterator.hpp"
#include "../include/common.hpp"
#include "list_order_maintenance/include/online_suffix_sort.hpp"
#include "libdivsufsort/sa.hpp"

//#include "esaxx/include/minimal_substrings/minimal_substring_tree.hpp"

using namespace std;
using namespace sdsl;
using namespace stool;
using namespace stool::lazy;



using INDEX = uint64_t;

template <typename CHAR>
vector<uint64_t> compute_suffix_array(std::vector<CHAR> &text){
    return stool::lom::construct_suffix_array(text);
    //return stool::construct_suffix_array(text);
}
template<> vector<uint64_t> compute_suffix_array(std::vector<uint8_t> &text){
    return stool::construct_suffix_array(text);
}


template <typename CHAR>
void compute_lazy_attractors_from_file(std::string filename, string mSubstrFile,uint64_t k_attr, uint64_t &textSize, uint64_t &minimalStringsCount,vector<uint64_t> &attrs)
{
    // Loading Input Text
    std::vector<CHAR> text;
    stool::lazy::load_vector(filename, text, false); // input text
    text.push_back(std::numeric_limits<CHAR>::min());
    textSize = text.size();

    std::cout << "Constructing Suffix Array" << std::endl;
    std::vector<INDEX> sa = compute_suffix_array(text);

    // Loading Minimal Substrings
    vector<stool::LCPInterval<uint64_t>> minimalSubstrings = stool::lazy::loadOrConstructMS(mSubstrFile, text, sa, k_attr);

    vector<uint64_t> parents = stool::esaxx::MinimalSubstringIntervals<uint8_t, uint64_t, vector<uint64_t>>::iterator<>::constructMSIntervalParents(minimalSubstrings);
    //stool::write_vector(mSubstrFile, minimalSubstrings);

    //Loader
    minimalStringsCount = minimalSubstrings.size();
    //vector<uint64_t> attrs;

    //LazyAttractorAlgorithm algo(text, intervals, parents);

    vector<uint64_t> tmp = LazyAttractor::computeLazyAttractors(text, sa, minimalSubstrings, parents);
    attrs.swap(tmp);
    
    #ifdef DEBUG
    std::cout << "check lazy attractors..." << std::endl;
    vector<uint64_t> correctAttrs = LazyAttractor::naiveComputeLazyAttractors(text, sa, minimalSubstrings);
    if (attrs.size() != correctAttrs.size())
    {
        stool::Printer::print("Lazy        ", attrs);
        stool::Printer::print("Correct Lazy", correctAttrs);
    }
    assert(attrs.size() == correctAttrs.size());
    std::cout << "OK!" << std::endl;

    #endif
    
}

int main(int argc, char *argv[])
{
    //using CHAR = uint8_t;
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
    p.add<string>("msubstr_file", 'm', "(option) The file name of minimal substrings for the input text file(the default minimal substrings filename is 'input_file.msub')", false, "");
    p.add<string>("character_type", 'c', "(option) character type", false, "uint8_t");

    p.add<uint64_t>("k-attr", 'k', "(option) the value of k-attractor", false, 0);

    p.parse_check(argc, argv);
    string inputFile = p.get<string>("input_file");
    string outputFile = p.get<string>("output_file");
    string outputMode = p.get<string>("output_type");
    string mSubstrFile = p.get<string>("msubstr_file");
    string characterType = p.get<string>("character_type");

    uint64_t k_attr = p.get<uint64_t>("k-attr");

    if (outputFile.size() == 0)
    {
        std::string ext = ".lazy";
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
    auto start = std::chrono::system_clock::now();
    vector<uint64_t> attrs;
    vector<stool::LCPInterval<uint64_t>> minimalSubstrings;
    uint64_t textSize, mSubstrCount;
    if(characterType == "char"){
        compute_lazy_attractors_from_file<char>(inputFile, mSubstrFile,k_attr, textSize, mSubstrCount ,attrs);
    }else if(characterType == "uint8_t"){
        compute_lazy_attractors_from_file<uint8_t>(inputFile, mSubstrFile,k_attr, textSize, mSubstrCount, attrs);
    }else if(characterType == "int32_t"){
        compute_lazy_attractors_from_file<int32_t>(inputFile, mSubstrFile,k_attr, textSize, mSubstrCount, attrs);
    }else if(characterType == "uint32_t"){
        compute_lazy_attractors_from_file<uint32_t>(inputFile, mSubstrFile,k_attr, textSize, mSubstrCount, attrs);
    }else if(characterType == "int64_t"){
        compute_lazy_attractors_from_file<int64_t>(inputFile, mSubstrFile,k_attr, textSize, mSubstrCount, attrs);
    }else if(characterType == "uint64_t"){
        compute_lazy_attractors_from_file<uint64_t>(inputFile, mSubstrFile,k_attr, textSize, mSubstrCount, attrs);
    }else{
        throw std::runtime_error("invalid character type!");
    }
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
        stool::IO::write(outputFile, attrs);

        //IO::write(outputFile, attrs, UINT64_MAX - 1);
    }

    std::cout << "\033[36m";
    std::cout << "============= INFO ===============" << std::endl;
    std::cout << "File: " << inputFile << std::endl;
    std::cout << "The input file content: ";
    if(characterType == "uint8_t" && textSize <= 100){
        std::vector<char> s;
        stool::lazy::load_vector(inputFile, s, false, false); 
        for(auto& c: s) std::cout << c;
        std::cout << std::endl;
    }else{
        std::cout << "(we omit to print it on the console if its size is larger than 100)" << std::endl;
    }
    std::cout << "Output: " << outputFile << std::endl;
    std::cout << "Attractor type: " << (k_attr == 0 ? "n" : std::to_string(k_attr)) << "-attractor" << std::endl;    
    std::cout << "The length of the input text (with additional last special marker): " << textSize << std::endl;
    std::cout << "Character type: " << characterType << std::endl;
 
    std::cout << "\033[33m";
    std::cout << "=============RESULT===============" << std::endl;   
    double charperms = (double)textSize / elapsed;
    std::cout << "The number of minimal substrings: " << mSubstrCount << std::endl;
    std::cout << "The number of attractors: " << attrs.size() << std::endl;
    std::cout << "The obtained attractors: ";
    if(attrs.size() <= 100){
        stool::Printer::print(attrs);
    }else{
        std::cout << "(we omit to print it on the console if its size is larger than 100)" << std::endl;
    }
    
    std::cout << "Excecution time: " << ((uint64_t)elapsed) << "ms";
    std::cout << "[" << charperms << "chars/ms]" << std::endl;
    std::cout << "==================================" << std::endl;
    std::cout << "\033[39m" << std::endl;

    return 0;
}
