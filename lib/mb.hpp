#include <iostream>
#include <iomanip>
#include <string>
#include <bitset>

#include <windows.h>

namespace mb
{

/*
    Check if byte is UTF-8 continuation byte.

    UTF-8 byte scheme:

    1-byte sequence (0xxxxxxx): ASCII characters
    2-byte sequence (110xxxxx 10xxxxxx)
    3-byte sequence (1110xxxx 10xxxxxx 10xxxxxx)
    4-byte sequence (11110xxx 10xxxxxx 10xxxxxx 10xxxxxx)

    Note:
    CR carriage return \r 0x0D 00001101
    LF line feed       \n 0x0A 00001010

    0xC0 is 11000000 in binary, e.g. we are lookin at the two highest bits
    0x80 is 10000000 in binary
*/
inline bool is_continuation(unsigned char c)
{
    return (c & 0xC0) == 0x80;
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
        unsigned char c = s[i];  // Narrowing conversion!
        if ( !is_continuation(c) ) ++count;
    }
    return count;
}

/*
    Check if character is whitespace.

    This is just a wrapper around std::isspace which return non-zero value
    if character is whitespace; or zero if character is not whitespace.

    space           ' '  0x20 0b00100000
    form feed       '\f' 0x0c 0b00001100
    line feed       '\n' 0x0a 0b00001010
    carriage return '\r' 0x0d 0b00001101
    horizontal tab  '\t' 0x09 0b00001001
    vertical tab    '\v' 0x0b 0b00001011
*/
inline bool is_whitespace(int c)
{
    // IF std::isspace == 0 is TRUE THEN WE HAVE WHITESPACE
    // std::isspace == 0 IS FALSE FOR NON-WHITESPACE
    return std::isspace(c) != 0;
}

void hex(std::string_view s)
{
    auto flags { std::cout.flags() };   // Save initial flags.
    for(unsigned char c : s)
    {
        std::cout << std::hex
                  << std::setw(2)
                  << std::setfill('0')
                  << static_cast<int>(c)
                  << ' ';
    }
    std::cout << '\n';
    std::cout.flags(flags);         // Restore initial flags.
}
void hex(char c, char end = '\n')
{
    auto flags { std::cout.flags() };
    std::cout << std::hex
              << std::setw(2)
              << std::setfill('0')
              << static_cast<int>(c);
    std::cout.flags(flags);
    std::cout << ' ';
}

/*
    Return binary representation of string or character as string.
*/
std::string bit(std::string_view s)
{
    std::string res;
    for(auto c : s)
    {
        res += std::bitset<8>{static_cast<int>(c)}.to_string() + " ";
    }
    return res;
}
std::string bit(char c)
{
    return std::bitset<8>{c}.to_string();
}

/*
    Read a character with specific byte count.
*/
std::string read_character(std::ifstream& in, size_t bytes)
{
    char cs[bytes];
    in.read(cs, bytes);
    return std::string{ cs, bytes };
}
/*
    Read one UTF-8 character.
*/
std::string read_character(std::ifstream& in)
{
    size_t size { 0 };
    char c { in.peek() };
    while ( c & 0b100000000 )
    {
        c <<= 1;
        ++size;
    }
    if ( size == 0 ) size = 1;
    return read_character(in, size);
}

}   // namespace mb
