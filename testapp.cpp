#include "renderer.h"

#include "app.h"

#include "tests/testclearcolor.h"
#include "tests/testfragmentshader.h"
#include "tests/testobjloader.h"
#include "tests/testtexture2d.h"
#include "tests/testchess.h"
#include "tests/testgame.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
const char *glsl_version = "#version 130";

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class TestsApplication : public Application
{
public:
    TestsApplication();
    ~TestsApplication();

    virtual void Update() override;

    virtual bool OnEvent( Event &evt ) override
    {
        if ( _currentTest )
        {
            _currentTest->OnEvent( evt );
        }

        return true;
    }

private:
    test::Test *    _currentTest = nullptr;
    test::TestMenu *_testMenu    = nullptr;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
TestsApplication::TestsApplication() : Application()
{
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL( _window, true );
    ImGui_ImplOpenGL3_Init( glsl_version );

    auto& style = ImGui::GetStyle();
    style.ChildRounding     = 0.0f;
    style.WindowRounding    = 0.0f;
    style.FrameRounding     = 0.0f;
    style.GrabRounding      = 0.0f;
    style.PopupRounding     = 0.0f;
    style.ScrollbarRounding = 0.0f;
    // style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.56f, 0.24, 1.0f);
    // style.Colors[ImGuiCol_ResizeGrip]    = ImVec4(0.08f, 0.56f, 0.24, 1.0f);
    // style.Colors[ImGuiCol_Button]        = ImVec4(0.08f, 0.56f, 0.24, 1.0f);


    _testMenu    = new test::TestMenu( this, _currentTest );
    _currentTest = _testMenu;

    _testMenu->RegisterTest<test::TestClearColor>( "Clear Color" );
    _testMenu->RegisterTest<test::TestTexture2D>( "2D Texture" );
    _testMenu->RegisterTest<test::TestObjLoader>( "Obj Viewer" );
    _testMenu->RegisterTest<test::TestFragmentShader>( "FragmentShader" );
    _testMenu->RegisterTest<test::TestChess>( "Chess" );
    _testMenu->RegisterTest<test::TestGame>( "Game" );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
TestsApplication::~TestsApplication()
{
    delete _currentTest;
    if ( _testMenu != _currentTest )
    {
        delete _testMenu;
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestsApplication::Update()
{
    Renderer renderer;
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    renderer.Clear();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    {
        ImGui::Text( "Application average %.3f ms/frame (%.1f FPS)",
                     1000.0f / ImGui::GetIO().Framerate,
                     ImGui::GetIO().Framerate );
    }

    if ( _currentTest )
    {
        double time = glfwGetTime();
        _currentTest->OnUpdate( time );
        _currentTest->OnRender();
        ImGui::Begin( "Test" );
        if ( _currentTest != _testMenu && ImGui::Button( "< " ) )
        {
            delete _currentTest;
            _currentTest = _testMenu;
        }
        _currentTest->OnImGuiRender();
        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );

    Application::Update();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
int main( int argc, char *argv[] )
{
    TestsApplication app;
    app.Run();
    return 0;
}

