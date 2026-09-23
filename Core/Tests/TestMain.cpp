#include "TestHarness.h"

#include <cstring>

int main(int ArgumentCount, char** Arguments)
{
    const char* Filter = ArgumentCount > 1 ? Arguments[1] : nullptr;
    int Ran = 0;
    for (const auto& Case : DarkArisen::Test::Registry())
    {
        if (Filter != nullptr && std::strstr(Case.Name, Filter) == nullptr)
        {
            continue;
        }
        const int Before = DarkArisen::Test::Failures();
        Case.Body();
        ++Ran;
        std::cout << (DarkArisen::Test::Failures() == Before ? "[ PASS ] " : "[ FAIL ] ") << Case.Name << "\n";
    }
    std::cout << Ran << " test cases, " << DarkArisen::Test::Failures() << " failed checks\n";
    return Ran > 0 && DarkArisen::Test::Failures() == 0 ? 0 : 1;
}
