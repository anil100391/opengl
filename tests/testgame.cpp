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
constexpr float PLAYER_SCALE = 0.00060;
constexpr float WINDWAKE_SIZE = 30;
static float COUNT = 0;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
TestGame::TestGame(Application *app)
    : Test(app),
      _playerOrientation(3.0, 4.133, 0.748),
      _dwing(0.0f),
      _material { glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), // ambient
                  glm::vec4(0.5f, 0.5f, 0.5f, 1.0f), // diffuse
                  glm::vec4(0.5f, 0.5f, 0.5f, 1.0f), // specular
                  32.0f }                            // shininess
{
    COUNT = 0;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);

    _shader = std::make_unique<Shader>("res/shaders/obj.shader");

    _lineShader = std::make_unique<Shader>("res/shaders/planeWake.shader");

    CreateEarth();
    CreatePlayer();
    SetupCamera();
    _windWake[0].reserve(7 * WINDWAKE_SIZE);
    _windWake[1].reserve(7 * WINDWAKE_SIZE);

    // initialize irrKlang for audio
    _soundEngine = irrklang::createIrrKlangDevice();
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
void TestGame::CreateWake()
{
    std::vector<unsigned int> conn;
    conn.resize(2*_windWake[0].size() / 7);
    for (size_t ii = 0; ii <_windWake[0].size() / 7 - 1; ++ii )
    {
        conn[2 * ii] = ii;
        conn[2 * ii + 1] = ii + 1;
    }

    VertexBufferLayout layout;
    layout.Push<float>(3); // position
    layout.Push<float>(4); // color

    _wake._glMesh = std::make_unique<MeshGL>(_windWake[1], layout, conn, GL_STREAM_DRAW);
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
        _time += _dtime;

        float scale = PLAYER_SCALE;
        _playerXform = glm::scale(glm::mat4(1.0), glm::vec3(scale, scale, scale));

        _playerXform = glm::rotate(glm::mat4(1.0), _playerOrientation[0], glm::vec3(1.0, 0.0, 0.0)) * _playerXform;
        _playerXform = glm::rotate(glm::mat4(1.0), _playerOrientation[1], glm::vec3(0.0, 1.0, 0.0)) * _playerXform;
        _playerXform = glm::rotate(glm::mat4(1.0), _playerOrientation[2], glm::vec3(0.0, 0.0, 1.0)) * _playerXform;

        _playerXform = glm::translate(glm::mat4(1.0), glm::vec3(1.0f, 1.0f, 1.0f)) * _playerXform;

        _playerXform = glm::rotate(glm::mat4(1.0), (float)_time, glm::vec3(0.0, 0.0, 1.0)) * _playerXform;
        SetupCamera();

        float dTime = 0.002;
        // if (_time > count * dTime)
        {
            ++COUNT;

            glm::vec3 start(4.3209f, 0.0f, 0.24193f);
            auto loc4_0 = _playerXform * glm::vec4(start, 1.0f);
            glm::vec3 loc3_0 (loc4_0[0], loc4_0[1], loc4_0[2]);
            // loc3_0 = loc3_0 + _dwing;

            start = glm::vec3(-4.3209f, 0.0f, 0.24193f);
            auto loc4_1 = _playerXform * glm::vec4(start, 1.0f);
            glm::vec3 loc3_1 (loc4_1[0], loc4_1[1], loc4_1[2]);
            loc3_1 = loc3_1 + _dwing;
            if ( COUNT <= WINDWAKE_SIZE )
            {
                float size = _windWake[0].size() / 7;

                // wake 0
                _windWake[0].push_back(loc3_0[0]);
                _windWake[0].push_back(loc3_0[1]);
                _windWake[0].push_back(loc3_0[2]);

                //color
                _windWake[0].push_back(0.7f);
                _windWake[0].push_back(0.7f);
                _windWake[0].push_back(0.7f);
                _windWake[0].push_back((1.0f * size) / WINDWAKE_SIZE);

                // wake 1
                _windWake[1].push_back(loc3_1[0]);
                _windWake[1].push_back(loc3_1[1]);
                _windWake[1].push_back(loc3_1[2]);

                //color
                _windWake[1].push_back(0.7f);
                _windWake[1].push_back(0.7f);
                _windWake[1].push_back(0.7f);
                _windWake[1].push_back((1.0f * size) / WINDWAKE_SIZE);
                if (COUNT > 2 )
                {
                    CreateWake();
                }
            }
            else
            {
                std::rotate(_windWake[0].begin(), _windWake[0].begin()+7, _windWake[0].end());
                std::rotate(_windWake[1].begin(), _windWake[1].begin()+7, _windWake[1].end());

                // wake 0
                //color
                float size = _windWake[0].size() / 7;
                _windWake[0][7*WINDWAKE_SIZE-1] = (1.0f * size) / WINDWAKE_SIZE;
                _windWake[0][7*WINDWAKE_SIZE-2] = 0.7f;
                _windWake[0][7*WINDWAKE_SIZE-3] = 0.7f;
                _windWake[0][7*WINDWAKE_SIZE-4] = 0.7f;

                _windWake[0][7*WINDWAKE_SIZE-5] = loc3_0[2];
                _windWake[0][7*WINDWAKE_SIZE-6] = loc3_0[1];
                _windWake[0][7*WINDWAKE_SIZE-7] = loc3_0[0];

                for (size_t ii = 0; ii <_windWake[0].size() / 7; ++ii )
                {
                    _windWake[0][7 * ii + 3] = 0.7f;
                    _windWake[0][7 * ii + 4] = 0.7f;
                    _windWake[0][7 * ii + 5] = 0.7f;
                    _windWake[0][7 * ii + 6] = (1.0f * ii) / WINDWAKE_SIZE;
                }

                // wake 1
                //color
                _windWake[1][7*WINDWAKE_SIZE-1] = (1.0f * size) / WINDWAKE_SIZE;
                _windWake[1][7*WINDWAKE_SIZE-2] = 0.7f;
                _windWake[1][7*WINDWAKE_SIZE-3] = 0.7f;
                _windWake[1][7*WINDWAKE_SIZE-4] = 0.7f;

                _windWake[1][7*WINDWAKE_SIZE-5] = loc3_1[2];
                _windWake[1][7*WINDWAKE_SIZE-6] = loc3_1[1];
                _windWake[1][7*WINDWAKE_SIZE-7] = loc3_1[0];

                for (size_t ii = 0; ii <_windWake[0].size() / 7; ++ii )
                {
                    _windWake[1][7 * ii + 3] = 0.7f;
                    _windWake[1][7 * ii + 4] = 0.7f;
                    _windWake[1][7 * ii + 5] = 0.7f;
                    _windWake[1][7 * ii + 6] = (1.0f * ii) / WINDWAKE_SIZE;
                }

                _wake._glMesh->vbo()->Bind();
                _wake._glMesh->vbo()->BufferData(_windWake[1].data(), _windWake[1].size() * sizeof(float), GL_STREAM_DRAW);
                _wake._glMesh->vbo()->Unbind();
            }
        }
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
        _shader->Unbind();

        // render wake
        if ( _wake._glMesh )
        {
            glLineWidth(2.0f);
            _lineShader->Bind();
            // set transformation matrices uniforms
            _lineShader->SetUniformMat4f("u_M", model);
            _lineShader->SetUniformMat4f("u_V", _viewMat);
            _lineShader->SetUniformMat4f("u_P", _projMat);
            // wake 0
            _wake._glMesh->vbo()->Bind();
            _wake._glMesh->vbo()->BufferData(_windWake[0].data(), _windWake[0].size() * sizeof(float), GL_STREAM_DRAW);
            renderer.Draw(*_wake._glMesh->vao(), *_wake._glMesh->ibo(), *_lineShader, Renderer::LINES);
            // wake 0
            _wake._glMesh->vbo()->Bind();
            _wake._glMesh->vbo()->BufferData(_windWake[1].data(), _windWake[1].size() * sizeof(float), GL_STREAM_DRAW);
            renderer.Draw(*_wake._glMesh->vao(), *_wake._glMesh->ibo(), *_lineShader, Renderer::LINES);

            _lineShader->Unbind();
        }
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestGame::OnImGuiRender()
{
    ImGui::SliderFloat3("Rotation", &_playerOrientation[0], 0, 2.0f * glm::pi<float>() );
    ImGui::SliderFloat3("Wing", &_dwing[0], -0.01f, 0.01f);
    ImGui::SliderFloat("Speed", &_dtime, 0.0005f, 0.002f);
    ImGui::Text("%d", (int)_windWake[0].size()/7);
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
                if (!_paused)
                {
                     _soundEngine->setSoundVolume(0.2f);
                     _soundEngine->play2D("res/sounds/propellar.wav", true);
                }
                else
                     _soundEngine->stopAllSounds();
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
        case EventType::MouseScrolled:
        {
            if (!_paused)
            {
                auto &mouseEvt = static_cast<MouseScrollEvent &>(evt);
                float sign = (mouseEvt.YOffset() > 0) ? 1.0f : -1.0f;
                _playerOrientation[1] = _playerOrientation[1] + sign * 0.2f;
            }
            break;
        }
        case EventType::MouseMoved:
        case EventType::MouseButtonPressed:
        case EventType::MouseButtonReleased:
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

    const auto& playerCOG = _player._mesh->cog();
    auto loc4 = _playerXform * glm::vec4(playerCOG, 1.0f);
    glm::vec3 loc3(loc4[0], loc4[1], loc4[2]);

    auto dir = loc3 - _camera.GetLookAt();

    _camera.SetPosition(_camera.GetLookAt() + 1.07f * dir);
}
}
