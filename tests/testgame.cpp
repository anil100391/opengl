#include "testgame.h"
#include "../renderer.h"
#include "../light.h"
#include "../events/mouseevent.h"
#include "../events/keyevent.h"
#include <imgui.h>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include "../app.h"
#include "../utils/meshbufferobjects.h"

namespace test
{

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
constexpr float PLAYER_SCALE = 0.00075;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
TestGame::TestGame(Application *app)
    : Test(app),
      _playerOrientation(3.0, 4.133, 0.748),
      _material { glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), // ambient
                  glm::vec4(0.5f, 0.5f, 0.5f, 1.0f), // diffuse
                  glm::vec4(0.5f, 0.5f, 0.5f, 1.0f), // specular
                  32.0f }                            // shininess
{
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    _shader = std::make_unique<Shader>("res/shaders/obj.shader");
    _shader->Bind();

    CreateEarth();
    CreatePlayer();
    SetupCamera();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestGame::CreateEarth()
{
    _earth._mesh = std::make_unique<mesh>("res/earth.obj");

    std::vector<float> vertices = mbos::vbo(*_earth._mesh, true);
    std::vector<unsigned int> conn = mbos::ibo(*_earth._mesh, true);

    VertexBufferLayout layout;
    layout.Push<float>(3); // position
    layout.Push<float>(3); // normal
    layout.Push<float>(2); // texture coordinate

    _earth._glMesh = std::make_unique<MeshGL>(vertices, layout, conn);
    _earth._texture = std::make_unique<Texture>("res/textures/8k_earth_daymap.jpg");
    _earth._texture->Bind(0);
    _shader->SetUniform1i("u_Texture", 0);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestGame::CreatePlayer()
{
    _player._mesh = std::make_unique<mesh>("res/plane.obj");

    std::vector<float> vertices = mbos::vbo(*_player._mesh, true);
    std::vector<unsigned int> conn = mbos::ibo(*_player._mesh, true);

    VertexBufferLayout layout;
    layout.Push<float>(3); // position
    layout.Push<float>(3); // normal
    layout.Push<float>(2); // texture coordinate

    _player._glMesh = std::make_unique<MeshGL>(vertices, layout, conn);
    _player._texture = std::make_unique<Texture>("res/textures/plane.png");
    _player._texture->Bind(0);
    _shader->SetUniform1i("u_Texture", 0);

    // position player
    float scale = PLAYER_SCALE;
    _playerXform = glm::scale(glm::mat4(1.0), glm::vec3(scale, scale, scale));

    _playerXform = glm::rotate(glm::mat4(1.0), _playerOrientation[0], glm::vec3(1.0, 0.0, 0.0)) * _playerXform;
    _playerXform = glm::rotate(glm::mat4(1.0), _playerOrientation[1], glm::vec3(0.0, 1.0, 0.0)) * _playerXform;
    _playerXform = glm::rotate(glm::mat4(1.0), _playerOrientation[2], glm::vec3(0.0, 0.0, 1.0)) * _playerXform;

    _playerXform = glm::translate(glm::mat4(1.0), glm::vec3(1.0f, 1.0f, 1.0f)) * _playerXform;
    _playerXform = glm::rotate(glm::mat4(1.0), (float)_time, glm::vec3(0.0, 0.0, 1.0)) * _playerXform;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
TestGame::~TestGame()
{
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestGame::OnUpdate(float deltaTime)
{
    if ( !_paused )
    {
        _time += 0.002;

        float scale = PLAYER_SCALE;
        _playerXform = glm::scale(glm::mat4(1.0), glm::vec3(scale, scale, scale));

        _playerXform = glm::rotate(glm::mat4(1.0), _playerOrientation[0], glm::vec3(1.0, 0.0, 0.0)) * _playerXform;
        _playerXform = glm::rotate(glm::mat4(1.0), _playerOrientation[1], glm::vec3(0.0, 1.0, 0.0)) * _playerXform;
        _playerXform = glm::rotate(glm::mat4(1.0), _playerOrientation[2], glm::vec3(0.0, 0.0, 1.0)) * _playerXform;

        _playerXform = glm::translate(glm::mat4(1.0), glm::vec3(1.0f, 1.0f, 1.0f)) * _playerXform;

        _playerXform = glm::rotate(glm::mat4(1.0), (float)_time, glm::vec3(0.0, 0.0, 1.0)) * _playerXform;
        SetupCamera();
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestGame::OnRender()
{
    Renderer renderer;
    glClearColor( 0.2f, 0.2f, 0.2f, 1.0f );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    {
        glm::mat4 model(1.0f); // identity
        // model = glm::rotate(glm::mat4(1.0), (float)_time, glm::vec3(0.0, 0.0, 1.0));
        _viewMat = _camera.GetViewMatrix();
        int width, height;
        _app->GetWindowSize(width, height);
        _projMat = glm::perspective(glm::radians(45.0f), (float) width / (float)height, 0.1f, 100.0f);

        _shader->Bind();
        // set transformation matrices uniforms
        _shader->SetUniformMat4f("u_M", model);
        _shader->SetUniformMat4f("u_V", _viewMat);
        _shader->SetUniformMat4f("u_P", _projMat);

        // set material uniforms
        const material& m = GetMaterial();

        _shader->SetUniform4f("material.ambient", m._ambient);
        _shader->SetUniform4f("material.diffuse", m._diffuse);
        _shader->SetUniform4f("material.specular", m._specular);
        _shader->SetUniform1f("material.shininess", m._shininess);

        // set light uniforms
        pointlight l = GetLight();

        _shader->SetUniform3f("light.position", l._position);
        _shader->SetUniform4f("light.ambient", l._ambient);
        _shader->SetUniform4f("light.diffuse", l._diffuse);
        _shader->SetUniform4f("light.specular", l._specular);

        // render earth
        _earth._texture->Bind(0);
        _shader->SetUniform1i("u_Texture", 0);
        renderer.Draw(*_earth._glMesh->vao(), *_earth._glMesh->ibo(), *_shader);
        _earth._texture->Unbind();

        //render player
        _shader->SetUniformMat4f("u_M", _playerXform);
        _player._texture->Bind(0);
        _shader->SetUniform1i("u_Texture", 0);
        renderer.Draw(*_player._glMesh->vao(), *_player._glMesh->ibo(), *_shader);
        _player._texture->Unbind();
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestGame::OnImGuiRender()
{
    ImGui::SliderFloat3("Rotation", &_playerOrientation[0], 0, 2.0f * glm::pi<float>() );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestGame::OnEvent( Event &evt )
{
    auto evtType = evt.GetEventType();

    switch (evtType)
    {
        case EventType::KeyPressed:
        {
            auto& keyEvent = static_cast<KeyPressedEvent&>(evt);
            if ( keyEvent.GetKeyCode() == GLFW_KEY_SPACE )
            {
                _paused = !_paused;
            }
            else if ( keyEvent.GetKeyCode() == GLFW_KEY_LEFT )
            {
                _playerOrientation[1] += 0.1f;
            }
            else if ( keyEvent.GetKeyCode() == GLFW_KEY_RIGHT )
            {
                _playerOrientation[1] -= 0.1f;
            }
            break;
        }
        case EventType::MouseMoved:
        case EventType::MouseButtonPressed:
        case EventType::MouseButtonReleased:
        case EventType::MouseScrolled:
        case EventType::WindowResize:
        default: break;
    }

    _camera.OnEvent( evt );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
const material& TestGame::GetMaterial() const
{
    return _material;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestGame::SetMaterial(const material& m)
{
    _material = m;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
pointlight TestGame::GetLight() const
{
    const box3& meshBBox = _earth._mesh->bbox();
    const float* min = meshBBox.max();
    const glm::vec3 p(5 * min[0], 5 * min[1], 5 * min[2]);
    return { glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), // ambient
             glm::vec4(0.5f, 0.5f, 0.5f, 1.0f), // diffuse
             glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), // specular
             glm::vec3(p[0], p[1], p[2])        // position
           };
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestGame::SetupCamera()
{
    _camera.SetLookAt(_earth._mesh->cog());
    const box3& meshBBox = _earth._mesh->bbox();
    const float* min = meshBBox.max();

    const auto& playerCOG = _player._mesh->cog();
    auto loc = _playerXform * glm::vec4(playerCOG, 1.0f);

    auto dir = glm::vec3(loc[0], loc[1], loc[2]) - _camera.GetLookAt();

    _camera.SetPosition(_camera.GetLookAt() + 1.07f * dir);
}
}
