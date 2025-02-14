#pragma once

#include <string>
#include <unordered_map>

#include "suite.hpp"

namespace ts
{

class Tester
{
private:

    std::string name_;
    std::unordered_map<std::string, Suite> suites_;

public:

    Tester(std::string name = "Suites of tests.")
        : name_{name} {}

    void add(Suite suite, std::string name)
    {
        suites_.insert(std::make_pair(name, suite));
    }

    void run()
    {
        std::cout << name_ << "\n\n";
        for ( auto [name, suite] : suites_ )
        {
            suite.run();
        }
    }

};

}  // namespace ts
