#include <windows.h>

#include "..\lib\ts\tester.hpp"

#include <filesystem>
namespace fs = std::filesystem;

#include "_tests.hpp"

int main()
{

    SetConsoleOutputCP(CP_UTF8);

    auto tmp { fs::path(".\\test\\data\\tmp") };
    fs::remove_all(tmp);

    ts::Tester tester;
    tester.add(tests_encode, "Tests encoding and decoding.");
    tester.add(tests_header, "Tests Header");
    tester.add(tests_frequencies, "Tests frequencies");
    tester.add(tests_data, "Test creation of frequencies, tree and codes.");
    tester.add(tests_files, "Test opening of files.");
    tester.run();

}
