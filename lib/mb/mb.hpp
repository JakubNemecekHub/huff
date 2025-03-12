#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <climits>
#include <iomanip>
#include <string>
#include <bitset>

#include <vector>


namespace mb
{

// Check if byte is UTF-8 continuation byte.
inline bool is_continuation(unsigned char c)
{
    return (c & 0xC0) == 0x80;
    // 0xC0 is 11000000 in binary, e.g. we are looking at the two highest bits
    // 0x80 is 10000000 in binary
}

/*
    Return number of characters in string.

    Does not simply return the number of bytes of the string. It counts
    number of Unicode characters.
*/
int size(std::string_view s)
{
    int count {0};
    for(size_t i {0}; i < s.size(); ++i)
    {
        unsigned char c = s[i];
        if ( !is_continuation(c) ) ++count;
    }
    return count;
}

/*
    Check if character is whitespace.

    This is just a wrapper around std::isspace which return non-zero value
    if character is whitespace; or zero if character is not whitespace.
*/
inline bool is_whitespace(int c)
{
    // If std::isspace == 0 is true then we have whitespace
    // std::isspace == 0 is false for non-whitespace
    return std::isspace(c) != 0;
}

// TO DO: Get rid of the final space somehow better
// TO DO: Use char overload in string
/*
    Return hexadecimal representation of a string or single char as string.
*/
std::string hex(char c, bool prefix = false)
{
    std::ostringstream oss;
    oss << std::uppercase
        << std::hex
        << std::setw(2)
        << std::setfill('0');
        if ( prefix ) oss << "0x";
    oss << static_cast<int>(c);
    return oss.str();
}
std::string hex(std::string_view s, bool prefix = false)
{
    std::ostringstream oss;
    for(unsigned char c : s)
    {
        oss << std::uppercase
            << std::hex
            << std::setw(2)
            << std::setfill('0');
        if ( prefix ) oss << "0x";
        oss << static_cast<int>(c)
            << ' ';
    }
    auto res { oss.str() };
    return res.substr(0, res.size() - 1);
}

// TO DO: Use char overload in string
/*
    Return binary representation of a string or single char as string.
*/
std::string bin(char c, bool prefix = false)
{
    return (prefix ? "0b" : "") + std::bitset<CHAR_BIT>{static_cast<unsigned long long>(c)}.to_string();
}
std::string bin(std::string_view s, bool prefix = false)
{
    std::string res;
    for(auto c : s)
    {
        res += (prefix ? "0b" : "") + std::bitset<CHAR_BIT>{static_cast<unsigned long long>(c)}.to_string() + " ";
    }
    res.pop_back();
    return res;
}

/*
    Read a character with specific byte count.
*/
std::string read_character(std::ifstream& in, size_t bytes)
{
    std::vector<char> vec (bytes);
    in.read(vec.data(), bytes);
    return std::string{ vec.data(), bytes };
}
/*
    Read one UTF-8 character.
*/
std::string read_character(std::ifstream& in)
{
    size_t size { 0 };
    char c { static_cast<char>(in.peek()) };
    while ( c & 0b100000000 )
    {
        c <<= 1;
        ++size;
    }
    if ( size == 0 ) size = 1;
    return read_character(in, size);
}

}   // namespace mb
