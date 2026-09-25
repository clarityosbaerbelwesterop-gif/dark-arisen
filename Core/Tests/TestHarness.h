#pragma once

#include <cmath>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

/** Minimal dependency-free test registry. Keeps the core buildable with only a compiler. */
namespace DarkArisen::Test
{
    struct Case
    {
        const char* Name;
        std::function<void()> Body;
    };

    inline std::vector<Case>& Registry()
    {
        static std::vector<Case> Cases;
        return Cases;
    }

    struct Registrar
    {
        Registrar(const char* Name, std::function<void()> Body) { Registry().push_back({Name, std::move(Body)}); }
    };

    inline int& Failures()
    {
        static int Count = 0;
        return Count;
    }

    inline void Fail(const char* File, const int Line, const std::string& Message)
    {
        ++Failures();
        std::cerr << File << ":" << Line << ": FAILED: " << Message << "\n";
    }
}

#define DA_CONCAT_INNER(A, B) A##B
#define DA_CONCAT(A, B) DA_CONCAT_INNER(A, B)
#define TEST_CASE(Name)                                                                                  \
    static void DA_CONCAT(TestBody_, __LINE__)();                                                        \
    static const ::DarkArisen::Test::Registrar DA_CONCAT(TestRegistrar_, __LINE__)(Name, &DA_CONCAT(TestBody_, __LINE__)); \
    static void DA_CONCAT(TestBody_, __LINE__)()

#define CHECK(Expression)                                                                                \
    do { if (!(Expression)) ::DarkArisen::Test::Fail(__FILE__, __LINE__, #Expression); } while (false)

#define CHECK_EQ(Actual, Expected)                                                                       \
    do {                                                                                                 \
        const auto& DaActual = (Actual);                                                                 \
        const auto& DaExpected = (Expected);                                                             \
        if (!(DaActual == DaExpected)) {                                                                 \
            std::ostringstream DaStream;                                                                 \
            DaStream << #Actual << " == " << #Expected;                                                  \
            ::DarkArisen::Test::Fail(__FILE__, __LINE__, DaStream.str());                                \
        }                                                                                                \
    } while (false)

#define CHECK_NEAR(Actual, Expected, Tolerance)                                                          \
    do {                                                                                                 \
        const double DaActual = static_cast<double>(Actual);                                             \
        const double DaExpected = static_cast<double>(Expected);                                         \
        if (!(std::fabs(DaActual - DaExpected) <= (Tolerance))) {                                        \
            std::ostringstream DaStream;                                                                 \
            DaStream << #Actual << " = " << DaActual << ", expected " << DaExpected;                     \
            ::DarkArisen::Test::Fail(__FILE__, __LINE__, DaStream.str());                                \
        }                                                                                                \
    } while (false)
