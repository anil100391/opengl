#include "test.h"
#include <imgui.h>

namespace test
{

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
TestMenu::TestMenu(Test*& currentTest)
    : _currentTest(currentTest)
{
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
TestMenu::~TestMenu()
{
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestMenu::OnImGuiRender()
{
    for ( auto& test : _tests )
    {
        if (ImGui::Button(test.first.c_str()))
        {
            _currentTest = test.second();
        }
    }
}

}
