#include <map>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <functional>

#include <gui/camera.h>

#include <utils/meshgl.h>

#include <graphics/renderer.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <gui/app.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
const char *glsl_version = "#version 130";

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class PlotApplication : public Application
{
public:
    PlotApplication();
    ~PlotApplication();

    virtual void OnRender();
    virtual void OnImGuiRender();

    virtual void Update() override;

    virtual bool OnEvent( Event &evt ) override
    {
        return true;
    }

private:

    void UpdateWave0( float time = 0.0f );
    void UpdateWave1( float time = 0.0f );

    std::vector<float>        _wave0;
    std::vector<float>        _wave1;
    std::vector<unsigned int> _conn;
    Camera                    _camera;
    std::unique_ptr<MeshGL>   _glMesh0;
    std::unique_ptr<MeshGL>   _glMesh1;
    std::unique_ptr<MeshGL>   _glXyMesh;
    std::unique_ptr<Shader>   _shader;
    std::unique_ptr<Shader>   _xyplaneShader;

    const size_t              _NUM_POINTS = 128u;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
PlotApplication::PlotApplication() : Application( {512, 512, "TESTS"} )
{
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL( _window, true );
    ImGui_ImplOpenGL3_Init( glsl_version );

    auto &io = ImGui::GetIO();
    auto font = io.Fonts->AddFontFromFileTTF( "res/fonts/Open_Sans/OpenSans-Regular.ttf", 16.0f );

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

    _camera.SetLookAt( glm::vec3(0, 0, 0) );
    _camera.SetPosition( 1.5f * glm::vec3( 0, 0.01, 10 ) );

    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    _conn.reserve( 2 * (_NUM_POINTS - 1) );
    for ( auto ii = 0u; ii < _NUM_POINTS - 1; ++ii )
    {
        _conn.push_back( ii );
        _conn.push_back( ii + 1 );
    }

    _shader = std::make_unique<Shader>( "res/shaders/v3c4.shader" );

    // xy plane
    float SPAN = 10.0f;
    std::vector<float> xyplane
    {
        -SPAN, -SPAN, 0.0f,
         SPAN, -SPAN, 0.0f,
         SPAN,  SPAN, 0.0f,
        -SPAN,  SPAN, 0.0f,
    };

    std::vector<unsigned int> xyconn{0, 1, 2, 0, 2, 3};
    VertexBufferLayout layout;
    layout.Push<float>( 3u );
    _glXyMesh = std::make_unique<MeshGL>( xyplane, layout, xyconn );

    _xyplaneShader = std::make_unique<Shader>( "res/shaders/cartesian.shader" );

    glLineWidth( 2.0f );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
PlotApplication::~PlotApplication()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void PlotApplication::OnRender()
{
    Renderer renderer;
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // set up mvp matrices
    glm::mat4 model( 1.0f );

    const auto& viewMat = _camera.GetViewMatrix();

    int width, height;
    GetWindowSize( width, height );
    auto projMat = glm::perspective( glm::radians( 45.0f ), (float)width / (float)height, 0.1f, 100.0f );
    glViewport( 0, 0, width, height );

    _xyplaneShader->Bind();
    _xyplaneShader->SetUniformMat4f( "u_M", model );
    _xyplaneShader->SetUniformMat4f( "u_V", viewMat );
    _xyplaneShader->SetUniformMat4f( "u_P", projMat );

    _glXyMesh->ibo()->Bind();
    _glXyMesh->vao()->Bind();
    _glXyMesh->vbo()->Bind();

    renderer.Draw( *_glXyMesh->vao(), *_glXyMesh->ibo(), *_xyplaneShader, Renderer::TRIANGLES );

    _shader->Bind();
    _shader->SetUniformMat4f( "u_M", model );
    _shader->SetUniformMat4f( "u_V", viewMat );
    _shader->SetUniformMat4f( "u_P", projMat );

    _glMesh0->ibo()->Bind();
    _glMesh0->vao()->Bind();
    _glMesh0->vbo()->Bind();

    renderer.Draw( *_glMesh0->vao(), *_glMesh0->ibo(), *_shader, Renderer::LINES );

    _glMesh1->ibo()->Bind();
    _glMesh1->vao()->Bind();
    _glMesh1->vbo()->Bind();

    renderer.Draw( *_glMesh1->vao(), *_glMesh1->ibo(), *_shader, Renderer::LINES );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void PlotApplication::OnImGuiRender()
{
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void PlotApplication::Update()
{
    Renderer renderer;
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    renderer.Clear();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // ImGui::ShowDemoWindow();

    if ( ImGui::CollapsingHeader("Render Statistics") )
    {
        ImGui::Text( "Application average %.3f ms/frame (%.1f FPS)",
                     1000.0f / ImGui::GetIO().Framerate,
                     ImGui::GetIO().Framerate );
    }

    ImGui::Separator();

    {
        float time = static_cast<float>(glfwGetTime());
        UpdateWave0( time );
        UpdateWave1( time );

        {
            VertexBufferLayout layout;
            layout.Push<float>( 3 ); // vertices
            layout.Push<float>( 4 ); // color
            if ( !_glMesh0 )
            {
                _glMesh0 = std::make_unique<MeshGL>( _wave0, layout, _conn, GL_DYNAMIC_DRAW );
            }
            else
            {
                _glMesh0->vbo()->Bind();
                _glMesh0->vbo()->UpdateBufferData(_wave0.data(), _wave0.size() * sizeof(float));
            }

            if ( !_glMesh1 )
            {
                _glMesh1 = std::make_unique<MeshGL>( _wave1, layout, _conn, GL_DYNAMIC_DRAW );
            }
            else
            {
                _glMesh1->vbo()->Bind();
                _glMesh1->vbo()->UpdateBufferData( _wave1.data(), _wave1.size() * sizeof( float ) );
            }
        }

        OnRender();
        OnImGuiRender();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );

    Application::Update();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void PlotApplication::UpdateWave0( float time )
{
    _wave0.resize( 7 * _NUM_POINTS );
    for ( size_t ii = 0u; ii < _NUM_POINTS; ++ii )
    {
        float t = (1.0f * ii) / (_NUM_POINTS - 1);
        float x = 4.0f * M_PI * t;
        float y = std::sin( x - 4 * time ) + 0.5 * std::cos( x - 5 * time );

        float *pos = &_wave0[7 * ii];
        pos[0] = x;
        pos[1] = y;
        pos[2] = 0.0f;

        float *color = &_wave0[7 * ii + 3];
        color[0] = 0.7f;
        color[1] = 0.7f;
        color[2] = 0.0f;
        color[3] = 1.0f;
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void PlotApplication::UpdateWave1( float time )
{
    _wave1.resize( 7 * _NUM_POINTS );
    for ( size_t ii = 0u; ii < _NUM_POINTS; ++ii )
    {
        float t = (1.0f * ii) / (_NUM_POINTS - 1);
        float x = 4.0f * M_PI * t;
        float y = std::sin( x - 4 * time ) + std::cos( 2 * x - 4 * time );

        float *pos = &_wave1[7 * ii];
        pos[0] = x;
        pos[1] = y;
        pos[2] = 0.0f;

        float *color = &_wave1[7 * ii + 3];
        color[0] = std::abs( 1 );
        color[1] = std::abs( 0 );
        color[2] = std::abs( 1 );
        color[3] = 1.0f;
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
int main( int argc, char *argv[] )
{
    PlotApplication app;
    app.Run();
    return 0;
}

