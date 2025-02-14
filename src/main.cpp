#include <windows.h>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <array>

namespace fs = std::filesystem;

#include "..\lib\argparse.hpp"

#include "huff.hpp"


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
    auto frequencies { count_frequencies(in) };
    for ( const auto [character, frequency] : frequencies )
        std::cout << character << ": " << frequency << '\n';

    return 0;
}
