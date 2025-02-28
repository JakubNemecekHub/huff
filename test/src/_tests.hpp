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

#pragma once

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

#include "..\lib\ts\suite.hpp"
#include "..\..\src\huff.hpp"

ts::Suite tests_encode { "Test encoding and decoding a file." };

bool compare_files(std::ifstream& lhs, std::ifstream& rhs)
{
    if ( lhs.tellg() != rhs.tellg() ) return false;
    lhs.seekg(0);
    rhs.seekg(0);
    return std::equal(std::istreambuf_iterator<char>(lhs.rdbuf()),
                      std::istreambuf_iterator<char>(),
                      std::istreambuf_iterator<char>(rhs.rdbuf()));
}

void encode_and_decode(fs::directory_entry file, fs::path tmp)
{
    std::cout << "Testing " << file.path() << '\n';
    auto tmp_file { tmp / file.path().filename() };
    auto archive_name { file.path().filename().replace_extension(".hz") };
    auto archive { tmp / archive_name };
    encode_file(file.path(), archive);
    decode_file(archive, tmp_file);
    auto original { std::ifstream(file.path(), std::ios::binary ) };
    if ( !original.is_open() )
    {
        std::cerr << "Could not open the file " << fs::absolute(file.path()) << ".";
        return;
    }
    auto processed { std::ifstream(tmp_file, std::ios::binary ) };
    if ( !processed.is_open() )
    {
        std::cerr << "Could not open the file " << fs::absolute(tmp_file) << ".";
        return;
    }
    // ASSERT_TRUE( compare_files(original, processed) )
    original.close();
    processed.close();
}

TEST(tests_encode, "Test encoding and decoding.")
{
    const auto data { fs::path("C:\\Workspace\\cpp\\huff\\test\\data") };
    const auto tmp  { data / "tmp" };
    fs::create_directory(tmp);
    for ( const auto& entry : fs::directory_iterator(data) )
    {
        if (!fs::is_regular_file(entry.path()) || entry.path().extension() != ".txt" ) continue;
        encode_and_decode(entry, tmp);
    }
    fs::remove_all(tmp);
}

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

#pragma once

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

#include "..\lib\ts\suite.hpp"

#include "..\..\src\huff.hpp"


ts::Suite tests_header { "Test writing and reading the header." };

void inner_test(fs::directory_entry file, fs::path tmp)
{
    std::cout << "Testing " << file.path() << '\n';
    auto file_name_path { file.path().filename().replace_extension(".hz") };
    auto archive_file_path { tmp / file_name_path };

    auto in { std::ifstream(file.path(), std::ios::binary ) };
    if ( !in.is_open() )
    {
        std::ostringstream oss;
        oss << "Could not open the file " << fs::absolute(file.path()) << ".";
        throw std::runtime_error(oss.str());
    }
    auto original_frequencies { count_frequencies(in) };
    in.close();

    auto out { std::ofstream(archive_file_path, std::ios::binary) };
    if ( !out.is_open() )
    {
        std::ostringstream oss;
        oss << "Could not open the file " << fs::absolute(archive_file_path) << ".";
        throw std::runtime_error(oss.str());
    }
    write_header(out, original_frequencies);
    out.close();

    auto in_2 { std::ifstream(archive_file_path, std::ios::binary ) };
    if ( !in_2.is_open() )
    {
        std::ostringstream oss;
        oss << "Could not open the file " << fs::absolute(archive_file_path) << ".";
        throw std::runtime_error(oss.str());
    }
    auto header_frequencies { read_header(in_2) };
    in_2.close();

    ASSERT_EQ_M( original_frequencies, header_frequencies )

    auto original_tree { std::make_unique<tree::Node<Point>>(generate_tree(original_frequencies)) };
    auto header_tree { std::make_unique<tree::Node<Point>>(generate_tree(header_frequencies)) };

    ASSERT_TRUE( tree::compare(original_tree, header_tree) )

}

TEST(tests_header, "Test header.")
{
    const auto data { fs::path("C:\\Workspace\\cpp\\huff\\test\\data") };
    const auto tmp  { data / "tmp" };
    fs::create_directory(tmp);
    for ( const auto& entry : fs::directory_iterator(data) )
    {
        if (!fs::is_regular_file(entry.path()) || entry.path().extension() != ".txt" ) continue;
        inner_test(entry, tmp);
    }
    fs::remove_all(tmp);
}

