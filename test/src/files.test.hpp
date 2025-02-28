#pragma once

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

#include "..\lib\ts\suite.hpp"

#include "..\..\src\huff.hpp"


ts::Suite tests_files { "Test opening of files." };


TEST(tests_files, "Encode nonexisting file.")
{
    auto file_in  { fs::path("this\\does\\not\\exists.txt")      };
    auto file_out { fs::path("this\\also\\does\\not\\exists.hz") };
    ASSERT_THROWS ( encode_file(file_in, file_out) )
}

TEST(tests_files, "Decode nonexisting file.")
{
    auto file_in  { fs::path("this\\does\\not\\exists.hz")        };
    auto file_out { fs::path("this\\also\\does\\not\\exists.txt") };
    ASSERT_THROWS ( decode_file(file_in, file_out) )
}

TEST(tests_files, "Encode file with wrong extension.")
{
    auto file_in  { fs::path(".\\test\\data\\chapter_1.hz")      };
    auto file_out { fs::path("this\\also\\does\\not\\exists.hz") };
    ASSERT_THROWS ( encode_file(file_in, file_out) )
}

TEST(tests_files, "Decode file with wrong extension.")
{
    auto file_in  { fs::path(".\\test\\data\\chapter_1.txt")      };
    auto file_out { fs::path("this\\also\\does\\not\\exists.txt") };
    ASSERT_THROWS ( decode_file(file_in, file_out) )
}

TEST(tests_files, "Encode file without read permission.")
{
    auto file_in  { fs::path(".\\test\\data\\no_read\\chapter_1.txt") };
    auto file_out { fs::path("this\\also\\does\\not\\exists.hz")      };
    ASSERT_THROWS ( encode_file(file_in, file_out) )
}

TEST(tests_files, "Decode file without read permission.")
{
    auto file_in  { fs::path(".\\test\\data\\no_read\\chapter_1.hz") };
    auto file_out { fs::path("this\\also\\does\\not\\exists.txt")    };
    ASSERT_THROWS ( decode_file(file_in, file_out) )
}

TEST(tests_files, "Encode into existing read-only file.")
{
    auto file_in  { fs::path(".\\test\\data\\read_only\\chapter_1.txt") };
    auto file_out { fs::path(".\\test\\data\\read_only\\chapter_1.hz")  };
    ASSERT_THROWS ( encode_file(file_in, file_out) )
}

TEST(tests_files, "Decode into existing read-only file.")
{
    auto file_in  { fs::path(".\\test\\data\\read_only\\chapter_1.hz")  };
    auto file_out { fs::path(".\\test\\data\\read_only\\chapter_1.txt") };
    ASSERT_THROWS ( decode_file(file_in, file_out) )
}
