#pragma once

#include <iostream>
#include <string>

struct My_Data
{
    int key;
    std::string name;
    My_Data() {};
    My_Data(int key_, std::string name_)
        : key{key_}, name{name_} {}
    My_Data(const My_Data& other)
    {
        this->key = other.key;
        this->name = other.name;
    }
    My_Data(My_Data&& other)
        : key{other.key}, name{std::move(other.name)} {}
    ~My_Data() {}

    auto operator<=>(const My_Data& other) const
    {
        return key <=> other.key;
    }
    auto operator==(const My_Data& other) const
    {
        return key == other.key;
    }
};

std::ostream& operator<<(std::ostream& os, const My_Data& obj)
{
    return os << "(" << obj.key << " " << obj.name << ")";
}

