#pragma once

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

#include "..\lib\ts\suite.hpp"

#include "..\..\src\huff.hpp"


ts::Suite tests_frequencies { "Test for counting frequencies of characters in a file." };

std::map<std::string, size_t> set_up(std::string file)
{
    fs::path file_path { fs::path(file) };
    std::ifstream in   { std::ifstream(file_path, std::ios::binary ) };
    if ( !in.is_open() )
        std::cerr << "Could not open the file " << fs::absolute(file_path) << ".";
    return count_frequencies(in);
}

TEST(tests_frequencies, "Test file test_1.txt")
{
    auto frequencies { set_up(".\\test\\data\\test_1.txt") };
    ASSERT_EQ( frequencies.size(), 3 )
    ASSERT_EQ( frequencies.at("a"), 3 )
    ASSERT_EQ( frequencies.at("b"), 2 )
    ASSERT_EQ( frequencies.at("c"), 1 )
}

TEST(tests_frequencies, "Test file test_2.txt")
{
    auto frequencies { set_up(".\\test\\data\\test_2.txt") };
    ASSERT_EQ( frequencies.size(), 3 )
    ASSERT_EQ( frequencies.at("ř"), 3 )
    ASSERT_EQ( frequencies.at("b"), 2 )
    ASSERT_EQ( frequencies.at("c"), 1 )
}

TEST(tests_frequencies, "Test file test_3.txt")
{
    auto frequencies { set_up(".\\test\\data\\test_3.txt") };
    ASSERT_EQ( frequencies.size(), 2 )
    ASSERT_EQ( frequencies.at("ř"), 1 )
    ASSERT_EQ( frequencies.at("á"), 1 )
}

TEST(tests_frequencies, "Test file test_5.txt")
{
    auto frequencies { set_up(".\\test\\data\\test_5.txt") };
    ASSERT_EQ( frequencies.size(), 5 )
    ASSERT_EQ( frequencies.at("こ"), 2 )
    ASSERT_EQ( frequencies.at("ん"), 1 )
    ASSERT_EQ( frequencies.at("に"), 1 )
    ASSERT_EQ( frequencies.at("ち"), 2 )
    ASSERT_EQ( frequencies.at("は"), 1 )
}
