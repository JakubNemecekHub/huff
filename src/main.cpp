#include <windows.h>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <array>
#include <queue>
#include <bitset>

namespace fs = std::filesystem;

#include "..\lib\argparse.hpp"
#include "..\lib\tree\linked.hpp"
#include "..\lib\heap\heap.hpp"

#include "huff.hpp"


int main(int argc, char* argv[])
{

    SetConsoleOutputCP(CP_UTF8);

    argparse::ArgumentParser program("huff", "1.0.0");

    auto& group { program.add_mutually_exclusive_group() };
    group.add_argument("-e", "--encode")
        .help("Encode text file into archive.")
        .flag();
    group.add_argument("-d", "--decode")
        .help("Decode archive back to txt file.")
        .flag();

    program.add_argument("-o", "--output")
        .help("Specify output file.");
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

    std::string input_file_ { program.get<std::string>("file") };
    fs::path input_file     { fs::path(input_file_)            };
    fs::path output_file;
    if ( auto user_output = program.present("-o") ) output_file = fs::path(program.get<std::string>("-o"));
    else                                            output_file = input_file;
    if      ( program.get<bool>("-e") ) encode_file(input_file, output_file);
    else if ( program.get<bool>("-d") ) decode_file(input_file, output_file);

    return 0;
}
