#include <iostream>
#include <string>
#include <memory>
#include "stool/src/print.hpp"
#include "stool/src/cmdline.h"
#include "stool/src/io.hpp"
#include "stool/src/debug.hpp"

#include "../src/greedy/greedy_attractor.hpp"
#include "../src/greedy/position_frequency_set.hpp"
#include "../src/common.hpp"
using namespace std;
using INDEX = uint64_t;
using namespace sdsl;
using namespace stool;
using namespace stool::lazy;

template <typename CHAR>
void greedy_attractor_test(vector<CHAR> &text)
{    
    //stool::Printer::print(text);
    std::cout << "@" << std::flush;
    std::vector<INDEX> sa = stool::construct_naive_SA<CHAR, INDEX>(text);
    std::vector<stool::LCPInterval<uint64_t>> minimalSubstrings = stool::lazy::loadOrConstructMS("", text,sa, 0);
    std::vector<INDEX> isa = stool::constructISA<CHAR, INDEX>(text, sa);
    std::vector<uint64_t> correct_attrs = GreedyAttractorAlgorithm::computeGreedyAttractors(sa, minimalSubstrings);
    std::vector<uint64_t> test_attrs = GreedyAttractorAlgorithm::computeFasterGreedyAttractors(sa, isa, minimalSubstrings);

    

    try
    {
        stool::equal_check(correct_attrs, test_attrs);
    }
    catch (const std::logic_error &e)
    {
        throw e;
    }
    
}
void greedy_attractor_test(uint64_t loop, uint64_t size){
    std::cout << "Minimal substrings tests" << std::endl;

    /*
    std::cout << ":Short string" << std::endl;
    for (size_t i = 0; i < 100; i++)
    {
        for (uint64_t alphabet = 1; alphabet < 64; alphabet *= 2)
        {
            if (i % 100 == 0)
                std::cout << "+" << std::flush;
            std::vector<char> text = stool::create_deterministic_integers<char>(i, alphabet, 0, i + alphabet);
            text.push_back(std::numeric_limits<char>::min());
            greedy_attractor_test(text);
        }
    }
    std::cout << std::endl;
    */
    
    std::cout << ":Char string" << std::endl;
    for (size_t i = 0; i < loop; i++)
    {
        if (i % 100 == 0)
            std::cout << "+" << std::flush;
        std::vector<char> text = stool::create_deterministic_integers<char>(size, 64, 0, i);
        text.push_back(std::numeric_limits<char>::min());
        greedy_attractor_test(text);
    }
    std::cout << std::endl;


    std::cout << ":uint8_t string" << std::endl;
    for (size_t i = 0; i < loop; i++)
    {
        if (i % 100 == 0)
            std::cout << "+" << std::flush;
        std::vector<uint8_t> text = stool::create_deterministic_integers<uint8_t>(size, 255, 1, i);
        text.push_back(std::numeric_limits<uint8_t>::min());

       greedy_attractor_test(text);
    }
    std::cout << std::endl;

    std::cout << ":int32_t string" << std::endl;
    for (size_t i = 0; i < loop; i++)
    {
        if (i % 100 == 0)
            std::cout << "+" << std::flush;
        std::vector<int32_t> text = stool::create_deterministic_integers<int32_t>(size, 255, -255, i);
        text.push_back(std::numeric_limits<int32_t>::min());
        greedy_attractor_test(text);
    }
    std::cout << std::endl;

    std::cout << ":uint32_t string" << std::endl;
    for (size_t i = 0; i < loop; i++)
    {
        if (i % 100 == 0)
            std::cout << "+" << std::flush;
        std::vector<uint32_t> text = stool::create_deterministic_integers<uint32_t>(size, 510, 1, i);
        text.push_back(std::numeric_limits<uint32_t>::min());
        greedy_attractor_test(text);
    }
    std::cout << std::endl;

    std::cout << ":int64_t string" << std::endl;
    for (size_t i = 0; i < loop; i++)
    {
        if (i % 100 == 0)
            std::cout << "+" << std::flush;
        std::vector<int64_t> text = stool::create_deterministic_integers<int64_t>(size, 1024, -1024, i);
        text.push_back(std::numeric_limits<int64_t>::min());
        greedy_attractor_test(text);
    }
    std::cout << std::endl;

    std::cout << ":uint64_t string" << std::endl;
    for (size_t i = 0; i < loop; i++)
    {
        if (i % 100 == 0)
            std::cout << "+" << std::flush;
        std::vector<uint64_t> text = stool::create_deterministic_integers<uint64_t>(size, 2048, 1, i);
        text.push_back(std::numeric_limits<uint64_t>::min());
        greedy_attractor_test(text);
    }
    std::cout << std::endl;
    
}

int main(int argc, char *argv[])
{

    cmdline::parser p;
    p.add<uint64_t>("size", 'l', "text length", true);
    p.parse_check(argc, argv);
    uint64_t size = p.get<uint64_t>("size");
    uint64_t loop = 10;

    greedy_attractor_test(loop, size);
    std::cout << "END" << std::endl;
}