#include "renderer.h"

#include "app.h"

#include "tests/testclearcolor.h"
#include "tests/testfragmentshader.h"
#include "tests/testobjloader.h"
#include "tests/testtexture2d.h"
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

    static bool KeyHandler( Event &evt )
    {
        // std::cout << evt << std::endl;
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

    _testMenu    = new test::TestMenu( _currentTest );
    _currentTest = _testMenu;

    _testMenu->RegisterTest<test::TestClearColor>( "Clear Color" );
    _testMenu->RegisterTest<test::TestTexture2D>( "2D Texture" );
    _testMenu->RegisterTest<test::TestObjLoader>( "Obj Viewer" );
    _testMenu->RegisterTest<test::TestFragmentShader>( "FragmentShader" );
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
        _currentTest->OnUpdate( 0.0f );
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
    app.SetEventHandler( &TestsApplication::KeyHandler );
    app.Run();
    return 0;
}

