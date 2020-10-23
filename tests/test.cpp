#include "test.h"
#include <imgui.h>

namespace test
{

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
TestMenu::TestMenu(Application *app, Test*& currentTest)
    : Test(app),
      _currentTest(currentTest)
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
    ImGui::SetNextItemOpen( true );
    if ( ImGui::CollapsingHeader("Tests") )
    {
        for ( auto& test : _tests )
        {
            ImGui::SetNextItemOpen( false );
            if (ImGui::TreeNode(test.first.c_str()))
            {
                _currentTest = test.second();
                ImGui::TreePop();
            }
        }
    }
}

}

