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
