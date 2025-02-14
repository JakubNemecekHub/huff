#pragma once

#include <fstream>
#include <string>
#include <unordered_map>

#include "..\lib\mb\mb.hpp"

std::unordered_map<std::string, int> count_frequencies(std::ifstream& in)
{
    std::unordered_map<std::string, int> frequencies;
    std::string key { mb::read_character(in) };
    do
    {
        // Update frequencies map.
        if ( frequencies.contains(key) ) frequencies.at(key) += 1;
        else frequencies.insert({key, 1});
        // Get next character.
        key = mb::read_character(in);
    } while ( in );
    return std::move(frequencies);
}
