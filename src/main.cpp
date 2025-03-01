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

    auto& group { program.add_mutually_exclusive_group(true) };
    group.add_argument("-e", "--encode")
        .help("Encode text file into archive.")
        .flag();
    group.add_argument("-d", "--decode")
        .help("Decode archive back to txt file.")
        .flag();

    program.add_argument("-o", "--output")
        .help("Specify output file.");
    program.add_argument("-f", "--force")
        .help("Overwrite existing files without asking.")
        .flag();
    program.add_argument("-m", "--mute")
        .help("Mute input to standard output.")
        .flag();
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

    // Flags
    Flags flags;
    flags.force = program.get<bool>("-f");
    flags.mute = program.get<bool>("-m");

    std::string input_ { program.get<std::string>("file") };
    fs::path input     { fs::path(input_)                 };
    fs::path output;
    if ( auto user_output = program.present("-o") ) output = fs::path(program.get<std::string>("-o"));
    if ( program.get<bool>("-e") )
    {
        try
        {
            encode_file(input, output, flags);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            return 1;
        }
    }
    else if ( program.get<bool>("-d") )
    {
        try
        {
            decode_file(input, output, flags);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            return 1;
        }

    }

    return 0;
}
