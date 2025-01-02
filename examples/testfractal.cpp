#include <map>
#include <glm/gtc/matrix_transform.hpp>

#include <random>
#include <iostream>
#include <functional>

#include <gui/camera.h>

#include <utils/meshgl.h>

#include <graphics/texture.h>
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
class FractalApp : public Application
{
public:
    FractalApp();
    ~FractalApp();

    virtual void OnRender();
    virtual void OnImGuiRender();

    virtual void Update() override;

    virtual bool OnEvent( Event &evt ) override
    {
        return true;
    }

private:

    void UpdateFractal( float time = 0.0f );
    void GetGoodRandomPixel( float &x0, float &y0, unsigned int min_iter, unsigned int max_iter );

    std::vector<uint8_t>     _buffer;
    Camera                   _camera;
    std::unique_ptr<MeshGL>  _glMesh;
    std::unique_ptr<Shader>  _shader;
    std::unique_ptr<Texture> _texture;

    float                     _MIN_X = -2.0f;
    float                     _MAX_X = 0.5f;
    float                     _MIN_Y = -1.12f;
    float                     _MAX_Y = 1.12f;
    float                     _ASPECT_RATIO = (_MAX_X - _MIN_X) / (_MAX_Y - _MIN_Y);
    unsigned int              _RESX = 1024;
    unsigned int              _RESY = _RESX / _ASPECT_RATIO;

    // random number
    std::random_device                    _rd;
    std::mt19937                          _rngEngine;
    std::uniform_real_distribution<float> _distribution;
    std::uniform_real_distribution<float> _rndColor;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
FractalApp::FractalApp()
    : Application( {512, 512, "Mandelbrot Set"} ),
      _rngEngine( _rd() ),
      _distribution( 0.0f, 1.0f ),
      _rndColor( 0.0f, 1.0f )
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

    _camera.SetLookAt( glm::vec3( (_MAX_X + _MIN_X) / 2.0f, (_MAX_Y + _MIN_Y) / 2.0f, 0.0f ) );
    _camera.SetPosition( glm::vec3( (_MAX_X + _MIN_X) / 2.0f, (_MAX_Y + _MIN_Y) / 2.0f, 3.0f ) );
    _camera.SetUpVec( glm::vec3( 0.0f, 1.0f, 0.0f ) );

    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    float aspect_ratio = _ASPECT_RATIO;

    // xy plane
    float min_x = _MIN_X;
    float max_x = _MAX_X;
    float min_y = _MIN_Y;
    float max_y = _MAX_Y;

    unsigned int resx = _RESX;
    unsigned int resy = _RESY;

    _buffer.resize( 4u * resx * resy );

    std::vector<float> xyplane
    {
        min_x, min_y, 0.0f, 0.0f, 0.0f,
        max_x, min_y, 0.0f, 1.0f, 0.0f,
        max_x, max_y, 0.0f, 1.0f, 1.0f,
        min_x, max_y, 0.0f, 0.0f, 1.0f
    };

    std::vector<unsigned int> xyconn{0, 1, 2, 0, 2, 3};
    VertexBufferLayout layout;
    layout.Push<float>( 3u ); // 2d coord
    layout.Push<float>( 2u ); // texture coord
    _glMesh = std::make_unique<MeshGL>( xyplane, layout, xyconn );

    _shader  = std::make_unique<Shader>( "res/shaders/v2t2.shader" );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
FractalApp::~FractalApp()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void FractalApp::OnRender()
{
    glClearColor( 0.0, 0.0, 0.1, 1.0 );
    Renderer renderer;
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // set up mvp matrices
    glm::mat4 model( 1.0f );

    const auto& viewMat = _camera.GetViewMatrix();

    int width, height;
    GetWindowSize( width, height );
    auto projMat = glm::perspective( glm::radians( 45.0f ), (float)width / (float)height, 0.1f, 100.0f );
    glViewport( 0, 0, width, height );

    _shader->Bind();
    if ( _texture )
    {
        _texture->Bind( 0 );
        _shader->SetUniform1i( "u_Texture", 0 );
    }

    _shader->SetUniformMat4f( "u_M", model );
    _shader->SetUniformMat4f( "u_V", viewMat );
    _shader->SetUniformMat4f( "u_P", projMat );

    _glMesh->ibo()->Bind();
    _glMesh->vao()->Bind();
    _glMesh->vbo()->Bind();

    renderer.Draw( *_glMesh->vao(), *_glMesh->ibo(), *_shader, Renderer::TRIANGLES );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void FractalApp::OnImGuiRender()
{
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void FractalApp::Update()
{
    Renderer renderer;
    // glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
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
        UpdateFractal( time );

        {
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
void FractalApp::UpdateFractal( float time )
{
    const float delta           = 1.0f;
    static float lastUpdateTime = 0.0f;
    if ( (time - lastUpdateTime) < 1.0f )
    {
        return;
    }

    lastUpdateTime = time;

    uint8_t *pixel = _buffer.data();

    float aspect_ratio = _ASPECT_RATIO;
    float min_x = _MIN_X;
    float max_x = _MAX_X;
    float min_y = _MIN_Y;
    float max_y = _MAX_Y;

    unsigned int min_iter = 1000u;
    unsigned int max_iter = 2000u;
    /*
    // MANDELBROT SET
    for ( auto jj = 0u; jj < _RESY; ++jj )
    {
        float y0 = ((max_y - min_y) * jj) / (_RESY - 1) + min_y;
        for ( auto ii = 0u; ii < _RESX; ++ii )
        {
            float x0 = ((max_x - min_x) * ii) / (_RESX - 1) + min_x;

            // z_n = z_n-1 * z_n-1 + c0;
            // x + iy = ( x + iy ) ^ 2 + x0 + iy0;
            float x = 0u;
            float y = 0u;
            unsigned int iter = 0u;
            while ( x * x + y * y <= 4 && iter < max_iter )
            {
                float xtemp = x * x - y * y + x0;
                float ytemp = 2 * x * y + y0;
                x = xtemp;
                y = ytemp;
                ++iter;
            }

            if ( iter == max_iter )
                iter = 0u;

            pixel[0] = static_cast<uint8_t>((255.0f * iter) / max_iter);
            pixel[1] = static_cast<uint8_t>((255.0f * iter) / max_iter);
            pixel[2] = 0u;
            pixel[3] = 255u;
            pixel += 4u;
        }
    }
    */

    float x0, y0;
    GetGoodRandomPixel( x0, y0, min_iter, max_iter );

    uint8_t r = static_cast<uint8_t>(255 * _rndColor( _rngEngine ));
    uint8_t g = static_cast<uint8_t>(255 * _rndColor( _rngEngine ));
    uint8_t b = static_cast<uint8_t>(255 * _rndColor( _rngEngine ));

    // z_n = z_n-1 * z_n-1 + c0;
    // x + iy = ( x + iy ) ^ 2 + x0 + iy0;
    float x = 0u;
    float y = 0u;
    unsigned int iter = 0u;
    while ( x * x + y * y <= 4 && iter < max_iter )
    {
        // pixel at x, y
        if ( x >= min_x && x <= max_x && y >= min_y && y <= max_y )
        {
            auto px = static_cast<unsigned int>((x - min_x) * _RESX / (max_x - min_x));
            auto py = static_cast<unsigned int>((y - min_y) * _RESY / (max_y - min_y));
            auto pixel = &_buffer[4 * (_RESX * py + px)];
            pixel[0] = r;
            pixel[1] = g;
            pixel[2] = b;
            pixel[3] = 255;
        }

        float xtemp = x * x - y * y + x0;
        float ytemp = 2 * x * y + y0;
        x = xtemp;
        y = ytemp;
        ++iter;
    }

    if ( !_texture )
        _texture = std::make_unique<Texture>( _RESX, _RESY, _buffer );
    else
        _texture->UpdateTextureData( _buffer );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void FractalApp::GetGoodRandomPixel( float &x0, float &y0, unsigned int min_iter, unsigned int max_iter )
{
    while ( true )
    {
        // random pixel
        x0 = _distribution( _rngEngine );
        x0 = (_MAX_X - _MIN_X) * x0 + _MIN_X;

        y0 = _distribution( _rngEngine );
        y0 = (_MAX_Y - _MIN_Y) * y0 + _MIN_Y;

        float x = 0u;
        float y = 0u;
        unsigned int iter = 0u;
        while ( x * x + y * y <= 4 && iter < max_iter )
        {
            float xtemp = x * x - y * y + x0;
            float ytemp = 2 * x * y + y0;
            x = xtemp;
            y = ytemp;
            ++iter;
        }

        if ( iter > min_iter && iter < max_iter )
            return;
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
int main( int argc, char *argv[] )
{
    FractalApp app;
    app.Run();
    return 0;
}

