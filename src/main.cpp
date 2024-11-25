#include <filesystem>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <array>
#include <unordered_map>

namespace fs = std::filesystem;

#include "..\lib\argparse.hpp"
#include "..\lib\mb.hpp"


int main(int argc, char* argv[])
{

    SetConsoleOutputCP(CP_UTF8);

    argparse::ArgumentParser program("huff", "0.0.1");

    program.add_argument("file")
        .help("Input file");

    try
    {
        program.parse_args(argc, argv);
    }
    catch ( const std::exception& e )
    {
        std::cerr << e.what() << '\n';
        std::cerr << program;
        return 1;
    }

    std::string file   { program.get<std::string>("file") };
    fs::path file_path { fs::path(file)                   };
    // TO DO: Check if file exists. Is it necessary if we check .is_open later?
    std::ifstream in   { std::ifstream(file_path, std::ios::binary ) };
    if ( !in.is_open() )
    {
        std::cerr << "Could not open the file " << fs::absolute(file_path) << ".";
    }

    // Create a frequency map
    std::unordered_map<std::string, int> frequencies;
    char c { in.peek() };
    do
    {
        // Get a character.
        std::string key { mb::read_character(in) };
        // std::cout << key << " | ";
        c = in .peek();
        // Update frequencies map.
        if ( frequencies.contains(key) ) frequencies.at(key) += 1;
        else frequencies.insert({key, 1});
    } while ( !in.eof() );
    for ( const auto [character, frequency] : frequencies )
    {
        std::cout << character << ": " << frequency << '\n';
    }


    return 0;
}
