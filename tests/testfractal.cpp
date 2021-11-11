#include "testfractal.h"
#include "../renderer.h"
#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../events/mouseevent.h"
#include "../events/windowevent.h"
#include "../app.h"

namespace test
{

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
TestFractal::TestFractal(Application *app)
    : Test(app),
      _mouseX(0.354f),
      _mouseY(0.361f),
      _spanY(3.0f),
      _centerX(0.0f),
      _centerY(0.0f)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDisable(GL_DEPTH_TEST);

    _app->GetWindowSize(_windowWidth, _windowHeight);
    PopulateMeshBuffers();

    _shader = std::make_unique<Shader>("res/shaders/julia.shader");
    _shader->Bind();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
TestFractal::~TestFractal()
{
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestFractal::OnUpdate(float deltaTime)
{
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestFractal::OnRender()
{
    Draw();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestFractal::PopulateMeshBuffers()
{
    float w = 1.0f * _windowWidth;
    float h = 1.0f * _windowHeight;
    std::vector<float> positions = { 0.0f, 0.0f, 0.0f, 0.0f,
                                     w,    0.0f, 1.0f, 0.0f,
                                     w,    h,    1.0f, 1.0f,
                                     0.0f, h,    0.0f, 1.0f };

    std::vector<unsigned int> indices = { 0, 1, 2, 0, 2, 3 };

    VertexBufferLayout layout;
    layout.Push<float>(2);
    layout.Push<float>(2);
    _quadGL = std::make_unique<MeshGL>(positions, layout, indices);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestFractal::Draw()
{
    Renderer renderer;
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glClear(GL_COLOR_BUFFER_BIT);

    glm::mat4 viewMat(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)));
    glm::mat4 projMat(glm::ortho(0.0f, (float)_windowWidth, 0.0f, (float)_windowHeight, -1.0f, 1.0f));
    {
        glm::mat4 mvp = projMat * viewMat;

        _shader->SetUniform1f("u_AspectRatio", 1.0f * _windowWidth / _windowHeight);
        _shader->SetUniform1f("u_SpanY", _spanY);
        _shader->SetUniform2f("u_C0", _mouseX, _mouseY);
        _shader->SetUniform2f("u_Center", _centerX, _centerY);
        _shader->SetUniformMat4f("u_MVP", mvp);
        _shader->SetUniform1i("u_Mode", _mode);

        _shader->Bind();
        renderer.Draw(*_quadGL->vao(), *_quadGL->ibo(), *_shader);
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestFractal::OnImGuiRender()
{
    ImGui::SetNextItemOpen( true );
    if ( ImGui::CollapsingHeader( "Fractal Mode" ) )
    {
        // ImGui::SliderFloat("startx", &_mouseX, -1.5f, 1.5f);
        // ImGui::SliderFloat("starty", &_mouseY, -1.5f, 1.5f);
        ImGui::RadioButton( "Julia", &_mode, 0 );
        ImGui::RadioButton( "Mandelbrot", &_mode, 1 );
        ImGui::RadioButton( "Newton", &_mode, 2 );
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestFractal::OnEvent(Event &evt)
{
    auto evtType = evt.GetEventType();

    switch (evtType)
    {
        case EventType::MouseMoved:
        case EventType::MouseButtonPressed:
        case EventType::MouseButtonReleased:
        {
            auto& mouseEvt = static_cast<MouseEvent&>(evt);
            float ar = 1.0f * _windowWidth / _windowHeight;
            float spanX = _spanY * ar;
            float spanY = _spanY;
            float minCornerX = _centerX - 0.5f * spanX;
            float minCornerY = _centerY - 0.5f * spanY;
            _mouseX = minCornerX + (spanX * mouseEvt.X()) / _windowWidth;
            _mouseY = minCornerY + (spanY * (_windowHeight - mouseEvt.Y())) / _windowHeight;
            break;
        }
        case EventType::MouseScrolled:
        {
            auto& mouseEvt = static_cast<MouseScrollEvent&>(evt);
            float oldSize = _spanY;
            _spanY += static_cast<float>((0.1 * mouseEvt.YOffset()));
            if ( _spanY < 0.001f )
            {
                _spanY = oldSize;
                return;
            }

            float newSize = _spanY;
            float scale = newSize / oldSize;
            // recompute center so that it gives the illusion that zoomin/out
            // happened at mouse pos location
            // coord of center wrt mouse pos
            float localCenterX = _centerX - _mouseX;
            float localCenterY = _centerY - _mouseY;
            _centerX = _mouseX + localCenterX * scale;
            _centerY = _mouseY + localCenterY * scale;

            break;
        }
        case EventType::WindowResize:
        {
            auto& wre = static_cast<WindowResizeEvent&>(evt);
            _windowWidth = wre.Width();
            _windowHeight = wre.Height();
            PopulateMeshBuffers();
            break;
        }
        default: break;
    }
}

}
