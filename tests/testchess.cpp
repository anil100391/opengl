#include "testchess.h"
#include "../renderer.h"
#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../events/keyevent.h"
#include "../events/mouseevent.h"
#include "../events/windowevent.h"

#include "../app.h"

namespace test
{

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
TestChess::TestChess(Application *app)
    : Test(app),
      _viewMat(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0))),
      _projMat(glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f))
{
    GenerateBoardGLBuffers();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
TestChess::~TestChess()
{
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestChess::OnRender()
{
    Renderer renderer;
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glClear(GL_COLOR_BUFFER_BIT);

    _shader->Bind();

    int w = 0;
    int h = 0;
    _app->GetWindowSize(w, h);
    _projMat = glm::ortho(0.0f, 1.0f * w, 0.0f, 1.0f * h, -1.0f, 1.0f);

    float size = std::min(w, h) / 8.0f;

    glm::mat4 offset;
    if (w > h)
    {
        offset = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f * (w - h), 0.0f, 0.0f));
    }
    else
    {
        offset = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f * (h - w), 0.0f));
    }

    for (unsigned int ii = 0; ii < 64; ++ii )
    {
        unsigned int row  = ii / 8;
        unsigned int col = ii % 8;


        glm::mat4 model = glm::translate(offset, glm::vec3(size * row, size * col, 0));
        glm::mat4 mvp = _projMat * _viewMat * model;
        _shader->SetUniformMat4f("u_MVP", mvp);

        _shader->SetUniform1i("u_Cell", ii);

        renderer.Draw(*_vao, *_ibo, *_shader);
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestChess::OnEvent(Event& evt)
{
    auto evtType = evt.GetEventType();

    static bool buttonPressed = false;
    static bool dragging = false;
    static int startDragX = 0;
    static int startDragY = 0;

    switch (evtType)
    {
    case EventType::MouseMoved:
    {
        auto& mouseEvt = static_cast<MouseMoveEvent&>(evt);
        dragging = buttonPressed;
        if (dragging)
        {
        }
        break;
    }
    case EventType::MouseButtonPressed:
    {
        auto& mouseEvt = static_cast<MousePressedEvent&>(evt);
        if (mouseEvt.GetButton() == MouseEvent::Button::MIDDLE)
        {
            buttonPressed = true;
            startDragX = mouseEvt.X();
            startDragY = mouseEvt.Y();
        }
        break;
    }
    case EventType::MouseButtonReleased:
    {
        auto& mouseEvt = static_cast<MouseReleasedEvent&>(evt);
        if (mouseEvt.GetButton() == MouseEvent::Button::MIDDLE)
        {
            buttonPressed = false;
        }
        break;
    }
    case EventType::MouseScrolled:
    {
        auto& mouseEvt = static_cast<MouseScrollEvent&>(evt);
        break;
    }
    case EventType::KeyPressed:
    {
        auto& keyEvent = static_cast<KeyPressedEvent&>(evt);
        if (keyEvent.GetKeyCode() == GLFW_KEY_S)
        {
        }
        break;
    }
    case EventType::WindowResize:
        GenerateBoardGLBuffers();
        break;
    default: break;
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestChess::GenerateBoardGLBuffers()
{
    int w = 0, h = 0;
    _app->GetWindowSize(w, h);
    float size = std::min(w, h) / 8.0f;

    float cellCoords[] = { 0.0f, 0.0f,
                           size, 0.0f,
                           size, size,
                           0.0f, size };

    unsigned int indices[] = { 0, 1, 2,
                               2, 3, 0 };

    _vao = std::make_unique<VertexArray>();
    _vbo = std::make_unique<VertexBuffer>(cellCoords, 4 * 2 * sizeof(float));

    VertexBufferLayout layout;
    layout.Push<float>(2);

    _vao->AddBuffer(*_vbo, layout);

    if ( !_ibo )
        _ibo = std::make_unique<IndexBuffer>(indices, 6);

    if ( !_shader )
    {
        _shader = std::make_unique<Shader>("res/shaders/chess.shader");
        _shader->Bind();
    }
}

}
