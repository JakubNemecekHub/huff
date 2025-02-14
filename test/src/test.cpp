#include <windows.h>

#include "..\lib\ts\tester.hpp"

#include "_tests.hpp"

int main()
{

    SetConsoleOutputCP(CP_UTF8);

    ts::Tester tester;
    tester.add(tests_frequencies, "Tests frequencies");
    tester.run();

}
