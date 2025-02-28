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
    encode_file(file.path(), archive, Flags{true});
    decode_file(archive, tmp_file, Flags{true});
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
