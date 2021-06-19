#include "testgame.h"
#include "../renderer.h"
#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../events/keyevent.h"
#include "../events/mouseevent.h"
#include "../events/windowevent.h"
#include "../utils/meshbufferobjects.h"
#include "../app.h"

namespace test
{

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
TestGame::TestGame(Application *app)
    : Test(app),
      _mesh("res/suzanne.obj")
{
    SetupTerrain();
    SetupCamera();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
TestGame::~TestGame()
{
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestGame::OnRender()
{
    Renderer renderer;
    glClearColor(_color[0], _color[1],_color[2], _color[3]);
    glClear(GL_COLOR_BUFFER_BIT);

    // set uniforms
    glm::mat4 model(1.0f); // identity
    auto viewMat = _camera.GetViewMatrix();
    int width, height;
    _app->GetWindowSize(width, height);
    auto projMat = glm::perspective(glm::radians(45.0f), (float) width / (float)height, 0.1f, 100.0f);

    auto mvp = model * viewMat * projMat;
    _shader->Bind();
    // set transformation matrices uniforms
    _shader->SetUniformMat4f("u_MVP", mvp);

    renderer.Draw(*_vao, *_ibo, *_shader);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestGame::OnEvent(Event &evt)
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
            if ( dragging )
            {
                const glm::vec3& cor = _camera.GetLookAt();
                const glm::vec3& pos = _camera.GetPosition();
                int dx = mouseEvt.X() - startDragX;
                if ( dx != 0 )
                    dx = dx / abs(dx);
                startDragX = mouseEvt.X();
                startDragY = mouseEvt.Y();
                double dtheta = 2.0 * dx * M_PI / 180.0;
                double theta = std::atan2(pos[1], pos[0]);
                double len = std::sqrt(pos[0]*pos[0] + pos[1]*pos[1]);
                theta += dtheta;
                glm::vec3 newpos = pos;
                newpos[0] = static_cast<float>(len * std::cos(theta));
                newpos[1] = static_cast<float>(len * std::sin(theta));
                _camera.SetPosition(newpos);
            }
            break;
        }
        case EventType::MouseButtonPressed:
        {
            auto& mouseEvt = static_cast<MousePressedEvent&>(evt);
            if ( mouseEvt.GetButton() == MouseEvent::Button::MIDDLE )
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
            if ( mouseEvt.GetButton() == MouseEvent::Button::MIDDLE )
            {
                buttonPressed = false;
            }
            break;
        }
        case EventType::MouseScrolled:
        {
            auto& mouseEvt = static_cast<MouseScrollEvent&>(evt);
            const auto& lookAt = _camera.GetLookAt();
            const auto& eye    = _camera.GetPosition();
            glm::vec3 dir = eye - lookAt;
            float scale = static_cast<float>(0.1 * mouseEvt.YOffset());
            dir = dir +  dir * scale;
            _camera.SetPosition(lookAt + dir);
            break;
        }
        case EventType::KeyPressed:
        {
            auto& keyEvent = static_cast<KeyPressedEvent&>(evt);
            if ( keyEvent.GetKeyCode() == GLFW_KEY_W )
            {
                _color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
            }
            else if ( keyEvent.GetKeyCode() == GLFW_KEY_A )
            {
                _color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
            }
            else if ( keyEvent.GetKeyCode() == GLFW_KEY_S )
            {
                _color = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
            }
            else if ( keyEvent.GetKeyCode() == GLFW_KEY_D )
            {
                _color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
            }
            else
            {
            }
            break;
        }
        case EventType::WindowResize:
        default: break;
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestGame::SetupCamera()
{
    const box3& box      = _mesh.bbox();
    const float* max     = box.max();
    const glm::vec3& cog = _mesh.cog();

    _camera.SetPosition(glm::vec3(5*max[0], 5*max[1], 5*max[2]));
    _camera.SetLookAt(cog);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestGame::SetupTerrain()
{
    bool flatShading = true;
    std::vector<float> vertices = mbos::vbo(_mesh, flatShading);
    std::vector<unsigned int> conn = mbos::ibo(_mesh, flatShading);

    _vao = std::make_unique<VertexArray>();
    _vbo = std::make_unique<VertexBuffer>( vertices.data(), static_cast<unsigned int>(vertices.size() * sizeof( float )) );

    VertexBufferLayout layout;
    layout.Push<float>(3); // position
    layout.Push<float>(3); // normal
    layout.Push<float>(2); // texture coordinate
    _vao->AddBuffer(*_vbo, layout);

    _ibo = std::make_unique<IndexBuffer>(conn.data(), static_cast<unsigned int>(conn.size()));

    _shader = std::make_unique<Shader>("res/shaders/basic.shader");
}

}
