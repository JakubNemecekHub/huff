#pragma once

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

#include "..\lib\ts\suite.hpp"
#include "..\..\src\huff.hpp"


ts::Suite tests_data { "Test creation of frequencies, tree and codes." };

void create_frequencies_(fs::directory_entry file)
{
    auto in { std::ifstream(file.path(), std::ios::binary ) };
    if ( !in.is_open() )
    {
        std::cerr << "Could not open the file " << fs::absolute(file) << ".";
        return;
    }
    auto frequencies { count_frequencies(in) };
    ASSERT_FALSE( frequencies.empty() )
}
TEST(tests_data, "Create frequencies.")
{
    const auto data { fs::path("C:\\Workspace\\cpp\\huff\\test\\data") };
    for ( const auto& entry : fs::directory_iterator(data) )
    {
        if (!fs::is_regular_file(entry.path()) || entry.path().extension() != ".txt" ) continue;
        create_frequencies_(entry);
    }
}

void create_trees_(fs::directory_entry file)
{
    auto in { std::ifstream(file.path(), std::ios::binary ) };
    if ( !in.is_open() )
    {
        std::cerr << "Could not open the file " << fs::absolute(file) << ".";
        return;
    }
    auto frequencies { count_frequencies(in)      };
    auto tree        { generate_tree(frequencies) };
}
TEST(tests_data, "Create trees.")
{
    const auto data { fs::path("C:\\Workspace\\cpp\\huff\\test\\data") };
    for ( const auto& entry : fs::directory_iterator(data) )
    {
        if (!fs::is_regular_file(entry.path()) || entry.path().extension() != ".txt" ) continue;
        create_trees_(entry);
    }
}


void create_codes_(fs::directory_entry file)
{
    auto in { std::ifstream(file.path(), std::ios::binary ) };
    if ( !in.is_open() )
    {
        std::cerr << "Could not open the file " << fs::absolute(file) << ".";
        return;
    }
    auto frequencies { count_frequencies(in)      };
    auto tree        { generate_tree(frequencies) };
    auto codes       { generate_codes(tree)       };
    ASSERT_FALSE( codes.empty() )
}
TEST(tests_data, "Create code tables.")
{
    const auto data { fs::path("C:\\Workspace\\cpp\\huff\\test\\data") };
    for ( const auto& entry : fs::directory_iterator(data) )
    {
        if (!fs::is_regular_file(entry.path()) || entry.path().extension() != ".txt" ) continue;
        create_codes_(entry);
    }
}
