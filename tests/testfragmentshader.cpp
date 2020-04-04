#include "testfragmentshader.h"
#include "../renderer.h"
#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../events/mouseevent.h"
#include "../events/windowevent.h"

namespace test
{

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
TestFragmentShader::TestFragmentShader()
    : _startX(0.354f),
      _startY(0.361f),
      _size(3.0f),
      _centerX(1.5f),
      _centerY(1.5f)
{
    float positions[] = { 0.0f, 0.0f, 0.0f, 0.0f,
                          512.0f, 0.0f, 1.0f, 0.0f,
                          512.0f,  512.0f, 1.0f, 1.0f,
                          0.0f,  512.0f, 0.0f, 1.0f };

    unsigned int indices[] = { 0, 1, 2, 0, 2, 3 };

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDisable(GL_DEPTH_TEST);

    _vao = std::make_unique<VertexArray>();
    _vbo = std::make_unique<VertexBuffer>(positions, 4 * 4 * sizeof(float));

    VertexBufferLayout layout;
    layout.Push<float>(2);
    layout.Push<float>(2);
    _vao->AddBuffer(*_vbo, layout);

    _ibo = std::make_unique<IndexBuffer>(indices, 6);

    _shader = std::make_unique<Shader>("res/shaders/julia.shader");
    _shader->Bind();

    _shader->SetUniform2f("u_StartPos", _startX, _startY);
    _shader->SetUniform1f("u_Scale", _size);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
TestFragmentShader::~TestFragmentShader()
{
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestFragmentShader::OnUpdate(float deltaTime)
{
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestFragmentShader::OnRender()
{
    Draw();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestFragmentShader::Draw()
{
    Renderer renderer;
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glClear(GL_COLOR_BUFFER_BIT);

    glm::mat4 viewMat(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)));
    glm::mat4 projMat(glm::ortho(0.0f, (float)_windowWidth, 0.0f, (float)_windowHeight, -1.0f, 1.0f));
    {
        glm::mat4 mvp = projMat * viewMat;

        _shader->SetUniform1f("u_AspectRatio", 1.0f * _windowWidth / _windowHeight);
        _shader->SetUniform1f("u_Scale", _size);
        _shader->SetUniform2f("u_StartPos", _startX, _startY);
        _shader->SetUniformMat4f("u_MVP", mvp);

        _shader->Bind();
        renderer.Draw(*_vao, *_ibo, *_shader);
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestFragmentShader::OnImGuiRender()
{
    ImGui::SliderFloat("startx", &_startX, -1.5f, 1.5f);
    ImGui::SliderFloat("starty", &_startY, -1.5f, 1.5f);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestFragmentShader::OnEvent(Event &evt)
{
    auto evtType = evt.GetEventType();

    switch (evtType)
    {
        case EventType::MouseMoved:
        case EventType::MouseButtonPressed:
        case EventType::MouseButtonReleased:
        {
            auto& mouseEvt = static_cast<MouseEvent&>(evt);
            _startX = _size * mouseEvt.X() / _windowWidth - 1.5f;
            _startY = _size * mouseEvt.Y() / _windowHeight - 1.5f;
            break;
        }
        case EventType::MouseScrolled:
        {
            auto& mouseEvt = static_cast<MouseScrollEvent&>(evt);
            _size += (0.1 * mouseEvt.YOffset());
            break;
        }
        case EventType::WindowResize:
        {
            auto& wre = static_cast<WindowResizeEvent&>(evt);
            _windowWidth = wre.Width();
            _windowHeight = wre.Height();

            // recreate vbo and vao
            _vao.reset(nullptr);
            _vbo.reset(nullptr);

            float w = 1.0f * _windowWidth;
            float h = 1.0f * _windowHeight;
            float positions[] = { 0.0f, 0.0f, 0.0f, 0.0f,
                                  w,    0.0f, 1.0f, 0.0f,
                                  w,    h,    1.0f, 1.0f,
                                  0.0f, h,    0.0f, 1.0f };

            unsigned int indices[] = { 0, 1, 2, 0, 2, 3 };

            _vao = std::make_unique<VertexArray>();
            _vbo = std::make_unique<VertexBuffer>(positions, 4 * 4 * sizeof(float));

            VertexBufferLayout layout;
            layout.Push<float>(2);
            layout.Push<float>(2);
            _vao->AddBuffer(*_vbo, layout);
            break;
        }
        default: break;
    }
}

}

