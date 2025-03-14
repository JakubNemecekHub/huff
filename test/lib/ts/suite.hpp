#pragma once

#include <iostream>
#include <functional>
#include <vector>
#include <string>
#include <sstream>
#include <concepts>

/*
    Concept of a streamable type
*/
template<typename T>
concept Streamable = requires(std::ostream& os, T value)
{
    { os << value } -> std::same_as<std::ostream&>;
};
template <typename T>
std::string to_string_if_streamable(const T& value) {
    if constexpr (Streamable<T>) {
        std::ostringstream oss;
        oss << value;
        return oss.str();
    } else {
        return "(non-streamable type)";
    }
}


namespace ts
{

// ANSI escape sequences for colours.
constexpr std::string RED   { "\033[31m" };
constexpr std::string GREEN { "\033[32m" };
constexpr std::string CYAN  { "\033[36m" };
constexpr std::string RESET { "\033[0m"  };


class Suite
{
public:

    using test_function_t = std::function<void()>;

private:

    std::string name_;
    std::vector<std::pair<std::string, test_function_t>> tests_;

public:

    Suite(const std::string& name = "Test Suite")   // TO DO check why const &
        : name_{name} {}

    void register_test(const std::string& message, test_function_t test)
    {
        tests_.emplace_back(message, test);
    }

    void run()
    {
        int passed { 0 };
        int failed { 0 };
        std::cout << "Running tests from " << CYAN << name_ << RESET <<'\n';
        for ( const auto& [message, test] : tests_ )
        {
            try
            {
                test();
                std::cout << GREEN << "[PASSED] " << RESET << message << '\n';
                ++passed;
            }
            catch(const std::exception& e)
            {
                std::cout << RED << "[FAILED] " << RESET << message << e.what() << '\n';
                ++failed;
            }
            catch(...)
            {
                std::cout << "[FAILED] " << message << ": unknown error\n";
                ++failed;
            }
        }
        std::cout << "Suite Summary: " << passed << " passed, " << failed << " failed.\n";
        std::cout << "==================================\n\n";
    }

};

#define CONCAT_INNER(a, b) a ## b
#define CONCAT(a, b) CONCAT_INNER(a, b)
// Fo the indirection here see https://stackoverflow.com/questions/13301428/token-pasting-and-line/13301627#13301627
#define NAME(base) CONCAT(base, __LINE__)

#define TEST(suite, message)                                   \
    void NAME(TestFunction_)();                                \
    struct NAME(TestRegistrar_)                                \
    {                                                          \
        NAME(TestRegistrar_)()                                 \
        {                                                      \
            suite.register_test(message, NAME(TestFunction_)); \
        }                                                      \
    } NAME(test_registrar_);                                   \
    void NAME(TestFunction_)()

#define ASSERT_TRUE(condition)                                        \
if ( !(condition) )                                                   \
{                                                                     \
    std::ostringstream oss;                                           \
    oss << "\n\t" << ts::RED << "[ASSERT TRUE failed] " << ts::RESET  \
        << #condition;                                                \
    throw std::runtime_error(oss.str());                              \
}

#define ASSERT_FALSE(condition)                                        \
if ( (condition) )                                                     \
{                                                                      \
    std::ostringstream oss;                                            \
    oss << "\n\t" << ts::RED << "[ASSERT FALSE failed] " << ts::RESET  \
    << #condition;                                                     \
    throw std::runtime_error(oss.str());                               \
}

#define ASSERT_EQ(lhs, rhs)                                            \
if ( !(lhs == rhs) )                                                   \
{                                                                      \
    std::ostringstream oss;                                            \
    oss << "\n\t" << ts::RED << "[ASSERT EQUAL failed] " << ts::RESET  \
        << #lhs << " == " << #rhs                                      \
        << "\n\t" << "[VALUES] " << to_string_if_streamable(lhs)       \
                     << ", and " << to_string_if_streamable(rhs);      \
    throw std::runtime_error(oss.str());                               \
}

#define ASSERT_NEQ(lhs, rhs)                                               \
if ( !(lhs != rhs) )                                                       \
{                                                                          \
    std::ostringstream oss;                                                \
    oss << "\n\t" << ts::RED << "[ASSERT NOT EQUAL failed] " << ts::RESET  \
        << #lhs << " != " << #rhs                                          \
        << "\n\t" << "[VALUES] " << lhs << ", and " << rhs;                \
    throw std::runtime_error(oss.str());                                   \
}

#define ASSERT_THROWS(expression)                                      \
try                                                                    \
{                                                                      \
    expression;                                                        \
    std::ostringstream oss;                                            \
    oss << "\n\t" << ts::RED << "[ASSERT THROWS failed]" << ts::RESET  \
        << #expression << " didn't throw";                             \
    throw std::runtime_error(oss.str());                               \
}                                                                      \
catch (...)                                                            \
{                                                                      \
    /* Exception was thrown as expected. */                            \
}                                                                      \

}  // namespace ts
