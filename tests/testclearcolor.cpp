#include "testclearcolor.h"
#include "../renderer.h"
#include <imgui.h>

namespace test
{

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
TestClearColor::TestClearColor(Application *app)
    : Test(app)
{
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
TestClearColor::~TestClearColor()
{
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestClearColor::OnUpdate(float deltaTime)
{
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestClearColor::OnRender()
{
    glClearColor(_clearColor[0], _clearColor[1], _clearColor[2], _clearColor[3]);
    glClear(GL_COLOR_BUFFER_BIT);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestClearColor::OnImGuiRender()
{
    ImGui::ColorEdit4("Clear Color", _clearColor);
}

}
