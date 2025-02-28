#pragma once

#include <fstream>
#include <string>
#include <map>
#include <optional>

#include "..\lib\mb\mb.hpp"
#include "..\lib\tree\linked.hpp"
#include "..\lib\heap\heap.hpp"


std::map<std::string, size_t> count_frequencies(std::ifstream& in)
{
    std::map<std::string, size_t> frequencies;
    std::string key { mb::read_character(in) };
    do
    {
        frequencies[key] += 1;
        key = mb::read_character(in);
    } while ( in );
    return frequencies;
}

struct Point
{
    std::string character;
    size_t frequency;

    Point() {}
    Point(std::string _character, size_t _frequency)
        : character{_character}, frequency{_frequency} {}

    auto operator<=>(const Point& other) const
    {
        return frequency <=> other.frequency;
    }
    bool operator==(const Point& other) const
    {
        return frequency == other.frequency;
    }
};

tree::Node<Point> generate_tree(std::map<std::string, size_t> frequencies)
{
    Heap<tree::Node<Point>, std::greater<tree::Node<Point>>> pq;
    for ( const auto& [character, frequency] : frequencies )
        pq.emplace(character, frequency);

    while ( pq.size() > 1)
    {
        auto point_1 { pq.extract_top() };
        auto point_2 { pq.extract_top() };
        size_t total_frequency { point_1.data.frequency + point_2.data.frequency };

        tree::Node<Point> parent { "", total_frequency };
        parent.left(std::move(point_1));
        parent.right(std::move(point_2));

        pq.emplace(std::move(parent));
    }

    return pq.extract_top();
}

void generate_codes_(tree::Node<Point>& node, std::map<std::string, std::string>& table, std::string code)
{
    if ( node.left()  )                  generate_codes_(*node.left(), table, code + "0");
    if ( node.right() )                  generate_codes_(*node.right(), table, code + "1");
    if ( !node.left() && !node.right() ) table[node.data.character] = code;

}
std::map<std::string, std::string> generate_codes(tree::Node<Point>& node)
{
    std::map<std::string, std::string> table;
    std::string code;
    generate_codes_(node, table, code);
    return table;
}

void write_header(std::ofstream& out, const std::map<std::string, size_t>& frequencies)
{
    auto count { frequencies.size() };
    out.write(reinterpret_cast<const char*>(&count), sizeof(count));
    for ( auto const& [character, frequency] : frequencies )
    {
        out.write(character.data(), character.size());
        out.write(reinterpret_cast<const char*>(&frequency), sizeof(frequency));
    }
}

std::map<std::string, size_t> read_header(std::ifstream& in)
{
    size_t count { 0 };
    in.read(reinterpret_cast<char*>(&count), sizeof(count));
    std::string character;
    size_t frequency { 0 };
    std::map<std::string, size_t> frequencies;
    for ( size_t i {0}; i < count; ++i )
    {
        character = mb::read_character(in);
        in.read(reinterpret_cast<char*>(&frequency), sizeof(frequency));
        frequencies[character] = frequency;
    }
    return frequencies;
}

const size_t BUFFER_SIZE { sizeof(unsigned long) * CHAR_BIT };

void encode(std::ifstream& in, std::ofstream& out, const std::map<std::string, std::string>& codes)
{
    /*
        in = input plain file.
        out = output encoded file.
    */
    std::bitset<BUFFER_SIZE> buffer { 0 };
    size_t buffer_counter { 0 };

    auto character { mb::read_character(in) };
    do
    {
        auto code { codes.at(character) };
        for ( const auto& code_bit : code ) // fill up buffer
        {
            if ( code_bit == '1' ) buffer.set(buffer_counter);
            ++buffer_counter;
            if ( buffer_counter == BUFFER_SIZE  ) // If buffer is full, write it out
            {
                auto n = buffer.to_ulong() ;
                out.write(reinterpret_cast<const char*>(&n), sizeof(n));
                buffer_counter = 0;
                buffer.reset();
            }
        }
        character = mb::read_character(in);
    } while ( in );
    if ( buffer_counter != 0 )
    {
        auto n = buffer.to_ulong() ;
        out.write( reinterpret_cast<const char*>(&n), sizeof(n) ) ;
    }
}

tree::Node<Point>* decode_(std::bitset<BUFFER_SIZE>& buffer, size_t& counter,
                    tree::Node<Point>* node)
{
    if ( counter == 32 )                        return node;
    if ( node->degree() == tree::Degree::none ) return node;
    if ( buffer.test(counter) )                 return decode_(buffer, ++counter, node->right().get());
    else                                        return decode_(buffer, ++counter, node->left().get());
}
void decode(std::ifstream& in, std::ofstream& out, tree::Node<Point>& tree)
{
    /*
        in = input encoded file.
        out = output decoded file.
    */
    unsigned long buffer_ { 0 };
    std::bitset<BUFFER_SIZE> buffer { 0 };
    size_t buffer_counter { 0 };
    size_t character_count { tree.data.frequency };
    size_t character_counter { 0 };
    tree::Node<Point>* root { &tree };
    tree::Node<Point>* it   { &tree };

    in.read(reinterpret_cast<char*>(&buffer_), sizeof(buffer_));
    buffer = buffer_;
    do
    {
        while ( buffer_counter < BUFFER_SIZE )
        {
            auto* res { decode_(buffer, buffer_counter, it) };
            if ( res->data.character.empty() )
            {
                it = res;
                continue;
            }
            else
            {
                out << res->data.character;
                ++character_counter;
                if ( character_counter == character_count ) return;
                it = root;
            }
        }
        buffer_counter = 0;
        in.read(reinterpret_cast<char*>(&buffer_), sizeof(buffer_));
        buffer = buffer_;
    } while ( in );
}


fs::path final_output_(const fs::path& input, fs::path output, std::string_view ext)
{
    if ( output.has_extension() )
    {
        output.replace_extension(ext);
    }
    else if ( ( output.native().back() == fs::path::preferred_separator ) ||
              fs::is_directory(output) )
    {
        auto output_file { input.filename() };
        output_file.replace_extension(ext);
        output /= output_file;
    }
    return output;
}

struct Flags
{
    bool force { false };
};

void encode_file(const fs::path& input, fs::path output, Flags flags = Flags{})
{
    if ( input.extension() != ".txt" )
        throw std::runtime_error("Wrong file format. Expected \".txt\". Got \"" + input.extension().string() + "\".");
    output = final_output_(input, output, "hz");
    if ( fs::exists(output) && !flags.force )
    {
        bool query_result { true };
        do
        {
            std::cout << "File " << output << " already exists. Do you want to overwrite it? [Y]es, [N]o ";
            char answer;
            std::cin >> answer;
            if      ( answer == 'N' ||answer == 'n'  ) return;
            else if ( answer != 'Y' && answer != 'y' ) query_result = false;
        } while ( !query_result );
    }
    auto in { std::ifstream(input, std::ios::binary ) };
    if ( !in.is_open() )
        throw std::runtime_error("Could not open file \"" + fs::absolute(input).string() + "\".");
    auto out { std::ofstream(output, std::ios::binary ) };
    if ( !out.is_open() )
        throw std::runtime_error("Could not open file \"" + fs::absolute(output).string() + "\".");
    auto frequencies { count_frequencies(in)      };
    auto tree        { generate_tree(frequencies) };
    auto codes       { generate_codes(tree)       };
    in.clear();
    in.seekg(0);
    write_header(out, frequencies);
    encode(in, out, codes);
    in.close();
    out.close();
    std::cout << "File archived into " << output << ".\n";
}


void decode_file(const fs::path& input, fs::path output, Flags flags = Flags{})
{
    if ( input.extension() != ".hz" )
        throw std::runtime_error("Wrong file format. Expected \".hz\". Got \"" + input.extension().string() + "\".");
    output = final_output_(input, output, "txt");
    if ( fs::exists(output) && !flags.force )
    {
        bool query_result { true };
        do
        {
            std::cout << "File " << output << " already exists. Do you want to overwrite it? [Y]es, [N]o ";
            char answer;
            std::cin >> answer;
            if      ( answer == 'N' ||answer == 'n'  ) return;
            else if ( answer != 'Y' && answer != 'y' ) query_result = false;
        } while ( !query_result );
    }
    auto in { std::ifstream(input, std::ios::binary ) };
    if ( !in.is_open() )
        throw std::runtime_error("Could not open file \"" + fs::absolute(input).string() + "\".");
    auto out { std::ofstream(output, std::ios::binary ) };
    if ( !out.is_open() )
        throw std::runtime_error("Could not open file \"" + fs::absolute(output).string() + "\".");
    auto frequencies { read_header(in)            };
    auto tree        { generate_tree(frequencies) };
    decode(in, out, tree);
    in.close();
    out.close();
    std::cout << "File extracted into " << output << ".\n";
}
